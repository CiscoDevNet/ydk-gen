/**
 * \file messages.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of functions to create NETCONF messages.
 *
 * Copyright (c) 2012-2014 CESNET, z.s.p.o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this
 * product may be distributed under the terms of the GNU General Public
 * License (GPL) version 2 or later, in which case the provisions
 * of the GPL apply INSTEAD OF those given above.
 *
 * This software is provided ``as is, and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 *
 */

#define _GNU_SOURCE
#define _BSD_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <unistd.h>

#include "messages.h"
#include "messages_xml.h"
#include "netconf_internal.h"
#include "error.h"
#include "messages_internal.h"
#include "with_defaults.h"
#include "nacm.h"
#include "datastore.h"

#ifndef DISABLE_URL
#	include "url_internal.h"
#endif

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

/**
 * @brief Skip XML declaration in the beginning of an XML document
 *
 * @param xmldoc String containing XML document where the XML declaration
 * should be skipped.
 * @return Pointer into the given string pointing after the xml declaration if
 * any. Leading whitespaces are also skipped. Original string is not modified
 * in any way and if it was dynamically allocated, it should be freed via xmldoc
 * pointer.
 */
static char* nc_skip_xmldecl(const char* xmldoc)
{
	char *s;

	if (xmldoc == NULL) {
		return (NULL);
	}

	/* skip leading whitespaces */
	s = index(xmldoc, '<');
	if (s == NULL) {
		/* not a valid XML document */
		return (NULL);
	}

	/* see http://www.w3.org/TR/REC-xml/#NT-XMLDecl */
	if (strncmp(s, "<?xml", 5) == 0) {
		/* We got a "real" XML document. Now move after the XML declaration */
		s = index(s, '>');
		if (s == NULL || s[-1] != '?') {
			/* invalid XML declaration, corrupted document */
			return (NULL);
		}
		s++; /* move after ?> */
	}

	return (s);
}

static struct nc_filter *nc_filter_new_subtree(const xmlNodePtr filter)
{
	struct nc_filter *retval;
	xmlNsPtr ns;

	retval = malloc(sizeof(struct nc_filter));
	if (retval == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	retval->type = NC_FILTER_SUBTREE;
	retval->subtree_filter = xmlNewNode(NULL, BAD_CAST "filter");
	if (retval->subtree_filter == NULL) {
		ERROR("xmlNewNode failed (%s:%d).", __FILE__, __LINE__);
		nc_filter_free(retval);
		return (NULL);
	}

	/* set namespace */
	ns = xmlNewNs(retval->subtree_filter, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(retval->subtree_filter, ns);

	xmlNewNsProp(retval->subtree_filter, ns, BAD_CAST "type", BAD_CAST "subtree");

	if (filter != NULL) {
		if (xmlAddChildList(retval->subtree_filter, xmlCopyNodeList(filter)) == NULL) {
			ERROR("xmlAddChildList failed (%s:%d).", __FILE__, __LINE__);
			nc_filter_free(retval);
			return (NULL);
		}
	} /* else Empty filter as defined in RFC 6241 sec. 6.4.2 is returned */

	return (retval);
}

API struct nc_filter* nc_filter_new(NC_FILTER_TYPE type, ...)
{
	struct nc_filter *retval;
	char* filter_s = NULL;
	const char* arg;
	xmlDocPtr filter;
	va_list argp;

	/* init variadic arguments list */
	va_start(argp, type);

	switch (type) {
	case NC_FILTER_SUBTREE:
		/* convert string representation into libxml2 structure */
		arg = nc_skip_xmldecl(va_arg(argp, const char*));
		if (arg == NULL) {
			/* content is corrupted */
			ERROR("Invalid XML data to create subtree filter");
			va_end(argp);
			return NULL;
		}
		if (asprintf(&filter_s, "<filter>%s</filter>", (arg == NULL) ? "" : arg) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			va_end(argp);
			return (NULL);
		}
		filter = xmlReadDoc(BAD_CAST filter_s, NULL, NULL, NC_XMLREAD_OPTIONS);
		free(filter_s);
		if (filter == NULL) {
			ERROR("xmlReadDoc() failed (%s:%d).", __FILE__, __LINE__);
			va_end(argp);
			return (NULL);
		}
		retval = nc_filter_new_subtree(filter->children->children);
		xmlFreeDoc(filter);
		break;
	default:
		ERROR("%s: Invalid filter type specified.", __func__);
		va_end(argp);
		return (NULL);
	}

	va_end(argp);
	return (retval);
}

API struct nc_filter* ncxml_filter_new(NC_FILTER_TYPE type, ...)
{
	struct nc_filter *retval;
	xmlNodePtr filter;
	va_list argp;

	/* init variadic arguments list */
	va_start(argp, type);

	switch (type) {
	case NC_FILTER_SUBTREE:
		filter = va_arg(argp, const xmlNodePtr);
		retval = nc_filter_new_subtree(filter);
		break;
	default:
		ERROR("%s: Invalid filter type specified.", __func__);
		va_end(argp);
		return (NULL);
	}

	va_end(argp);

	return (retval);
}

API void nc_filter_free(struct nc_filter* filter)
{
	if (filter != NULL) {
		if (filter->subtree_filter) {
			xmlFreeNode(filter->subtree_filter);
		}
		free(filter);
	}
}

static char* nc_msg_dump(const struct nc_msg *msg)
{
	xmlChar *buf;
	int len;

	if (msg == NULL || msg == NCDS_RPC_NOT_APPLICABLE || msg->doc == NULL) {
		ERROR("%s: invalid input parameter.", __func__);
		return (NULL);
	}

	xmlDocDumpFormatMemory(msg->doc, &buf, &len, 1);

	/*
	 * NACM info is not dumped - this information is refreshed at the moment
	 * of nc_msg_build()
	 */

	return ((char*) buf);
}

API char* nc_reply_dump(const nc_reply* reply)
{
	return (nc_msg_dump((struct nc_msg*)reply));
}

API xmlDocPtr ncxml_reply_dump(const nc_reply* reply)
{
	if (reply == NULL || reply == NCDS_RPC_NOT_APPLICABLE || reply->doc == NULL) {
		ERROR("%s: invalid input parameter.", __func__);
		return (NULL);
	}

	return (xmlCopyDoc(reply->doc, 1));
}

API char* nc_rpc_dump(const nc_rpc* rpc)
{
	return (nc_msg_dump((struct nc_msg*)rpc));
}

API xmlDocPtr ncxml_rpc_dump(const nc_rpc* rpc)
{
	return (xmlCopyDoc(rpc->doc, 1));
}

static struct nc_msg* nc_msg_build (const char* msg_dump)
{
	struct nc_msg * msg;
	const char* id;

	if ((msg = calloc (1, sizeof(struct nc_msg))) == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return NULL;
	}

	if ((msg->doc = xmlReadMemory (msg_dump, strlen(msg_dump), NULL, NULL, NC_XMLREAD_OPTIONS)) == NULL) {
		ERROR("%s: parsing message dump failed.", __func__);
		free (msg);
		return NULL;
	}

	/* create xpath evaluation context */
	if ((msg->ctxt = xmlXPathNewContext(msg->doc)) == NULL) {
		ERROR("%s: rpc message XPath context cannot be created.", __func__);
		nc_msg_free(msg);
		return NULL;
	}
	
	/* register base namespace for the rpc */
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_BASE10_ID, BAD_CAST NC_NS_BASE10) != 0) {
		ERROR("Registering base namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_NOTIFICATIONS_ID, BAD_CAST NC_NS_NOTIFICATIONS) != 0) {
		ERROR("Registering notifications namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_WITHDEFAULTS_ID, BAD_CAST NC_NS_WITHDEFAULTS) != 0) {
		ERROR("Registering with-defaults namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_MONITORING_ID, BAD_CAST NC_NS_MONITORING) != 0) {
		ERROR("Registering monitoring namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}

	if ((id = nc_msg_parse_msgid (msg)) != NULL) {
		msg->msgid = strdup(id);
	} else {
		msg->msgid = NULL;
	}
	msg->error = NULL;
	msg->with_defaults = NCWD_MODE_NOTSET;

	/* NACM is set to NULL by default, if it is needed, caller (such as
	 * nc_rpc_build() should store fresh NACM information */
	msg->nacm = NULL;

	return msg;
}

static struct nc_msg* ncxml_msg_build(xmlDocPtr msg_dump)
{
	struct nc_msg* msg;
	const char* id;

	if ((msg = malloc(sizeof(struct nc_msg))) == NULL) {
		return NULL;
	}

	msg->doc = msg_dump;
	msg->next = NULL;
	msg->error = NULL;
	msg->with_defaults = NCWD_MODE_NOTSET;
	msg->type.rpc = 0;

	if ((id = nc_msg_parse_msgid (msg)) != NULL) {
		msg->msgid = strdup(id);
	} else {
		msg->msgid = NULL;
	}

	/* create xpath evaluation context */
	if ((msg->ctxt = xmlXPathNewContext(msg->doc)) == NULL) {
		ERROR("%s: rpc message XPath context cannot be created.", __func__);
		nc_msg_free(msg);
		return NULL;
	}

	/* register base namespace for the rpc */
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_BASE10_ID, BAD_CAST NC_NS_BASE10) != 0) {
		ERROR("Registering base namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_NOTIFICATIONS_ID, BAD_CAST NC_NS_NOTIFICATIONS) != 0) {
		ERROR("Registering notifications namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_WITHDEFAULTS_ID, BAD_CAST NC_NS_WITHDEFAULTS) != 0) {
		ERROR("Registering with-defaults namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_MONITORING_ID, BAD_CAST NC_NS_MONITORING) != 0) {
		ERROR("Registering monitoring namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}

	return (msg);
}

NCWD_MODE nc_rpc_parse_withdefaults(nc_rpc* rpc, const struct nc_session* session)
{
	xmlXPathContextPtr rpc_ctxt = NULL;
	xmlXPathObjectPtr result = NULL;
	xmlChar* data;
	NCWD_MODE retval = NCWD_MODE_NOTSET;

	if (rpc == NULL || nc_rpc_get_type(rpc) == NC_RPC_HELLO) {
		return (NCWD_MODE_NOTSET);
	}

	if (rpc->with_defaults != NCWD_MODE_NOTSET) {
		/* already known */
		return (rpc->with_defaults);
	}

	/* create xpath evaluation context */
	if ((rpc_ctxt = xmlXPathNewContext(rpc->doc)) == NULL) {
		WARN("%s: Creating the XPath context failed.", __func__);
		/* with-defaults cannot be found */
		return (NCWD_MODE_NOTSET);
	}
	if (xmlXPathRegisterNs(rpc_ctxt, BAD_CAST "wd", BAD_CAST NC_NS_WITHDEFAULTS) != 0) {
		ERROR("Registering with-defaults capability namespace for the xpath context failed.");
		xmlXPathFreeContext(rpc_ctxt);
		return (NCWD_MODE_NOTSET);
	}

	/* set with-defaults if any */
	if ((result = xmlXPathEvalExpression(BAD_CAST "//wd:with-defaults", rpc_ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
			switch (result->nodesetval->nodeNr) {
			case 0:
				/* set basic mode */
				if (session != NULL) {
					retval = session->wd_basic;
				} else {
					retval = NCWD_MODE_NOTSET;
				}
				break;
			case 1:
				data = xmlNodeGetContent(result->nodesetval->nodeTab[0]);
				if (xmlStrcmp(data, BAD_CAST "report-all") == 0) {
					retval = NCWD_MODE_ALL;
				} else if (xmlStrcmp(data, BAD_CAST "report-all-tagged") == 0) {
					retval = NCWD_MODE_ALL_TAGGED;
				} else if (xmlStrcmp(data, BAD_CAST "trim") == 0) {
					retval = NCWD_MODE_TRIM;
				} else if (xmlStrcmp(data, BAD_CAST "explicit") == 0) {
					retval = NCWD_MODE_EXPLICIT;
				} else {
					WARN("%s: unknown with-defaults mode detected (%s), disabling with-defaults.", __func__, data);
					retval = NCWD_MODE_NOTSET;
				}
				xmlFree(data);
				break;
			default:
				/* retval = NCWD_MODE_NOTSET; */
				break;
			}
		}
		xmlXPathFreeObject(result);
	} else {
		/* set basic mode */
		retval = ncdflt_get_basic_mode();
	}
	xmlXPathFreeContext(rpc_ctxt);

	rpc->with_defaults = retval;
	return (retval);
}

NC_RPC_TYPE nc_rpc_parse_type(nc_rpc* rpc)
{
	NC_OP op;

	if (rpc == NULL) {
		return (NC_RPC_UNKNOWN);
	}

	/* try to detect the type from the message body (according to the operation) */
	op = nc_rpc_get_op(rpc);
	switch (op) {
	case (NC_OP_GETCONFIG):
	case (NC_OP_GETSCHEMA):
	case (NC_OP_GET):
	case (NC_OP_VALIDATE):
		rpc->type.rpc = NC_RPC_DATASTORE_READ;
		break;
	case (NC_OP_EDITCONFIG):
	case (NC_OP_COPYCONFIG):
	case (NC_OP_DELETECONFIG):
	case (NC_OP_LOCK):
	case (NC_OP_UNLOCK):
	case (NC_OP_COMMIT):
	case (NC_OP_DISCARDCHANGES):
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		break;
	case (NC_OP_CLOSESESSION):
	case (NC_OP_KILLSESSION):
	case (NC_OP_CREATESUBSCRIPTION):
		rpc->type.rpc = NC_RPC_SESSION;
		break;
	default:
		rpc->type.rpc = NC_RPC_UNKNOWN;
		break;
	}

	return (rpc->type.rpc);
}

static nc_rpc* _rpc_build(int libxml2, const struct nc_session* session, const char* str_dump, xmlDocPtr xml_dump)
{
	nc_rpc* rpc;
	NC_OP op;

	if (libxml2) {
		assert(xml_dump);
		rpc = ncxml_msg_build(xml_dump);
	} else {
		assert(str_dump);
		rpc = nc_msg_build(str_dump);
	}
	if (rpc == NULL) {
		return NULL;
	}

	/* assign operation value */
	op = nc_rpc_assign_op(rpc);

	/* assign source/target datastore types */
	if (op == NC_OP_GETCONFIG || op == NC_OP_COPYCONFIG || op == NC_OP_VALIDATE) {
		if (!nc_rpc_assign_ds(rpc, "source")) {
			ERROR("*%s: Missing <source> parameter of the RPC operation.", __func__);
			nc_rpc_free(rpc);
			return NULL;
		}

	}
	if (op == NC_OP_EDITCONFIG || op == NC_OP_COPYCONFIG
			|| op == NC_OP_DELETECONFIG || op == NC_OP_LOCK || op == NC_OP_UNLOCK) {
		if (!nc_rpc_assign_ds(rpc, "target")) {
			ERROR("*%s: Missing <target> parameter of the RPC operation.", __func__);
			nc_rpc_free(rpc);
			return NULL;
		}
	}

	/* set rpc type flag */
	nc_rpc_parse_type(rpc);

	/* set with-defaults if any */
	nc_rpc_parse_withdefaults(rpc, NULL);

	if (session != NULL) {
		/* NACM init */
		nacm_start(rpc, session);
	}

	return rpc;
}

API nc_rpc* nc_rpc_build(const char* rpc_dump, const struct nc_session* session)
{
	return _rpc_build(0, session, rpc_dump, NULL);
}

API nc_rpc* ncxml_rpc_build(xmlDocPtr rpc_dump, const struct nc_session* session)
{
	return _rpc_build(1, session, NULL, rpc_dump);
}

NC_REPLY_TYPE nc_reply_parse_type(nc_reply* reply)
{
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr node;

	if (reply == NULL) {
		return (NC_REPLY_UNKNOWN);
	}

	/* set default value */
	reply->type.reply = NC_REPLY_UNKNOWN;

	/* try to detect the type from the message body */
	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc-reply/"NC_NS_BASE10_ID":ok", reply->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval) && query_result->nodesetval->nodeNr == 1) {
			reply->type.reply = NC_REPLY_OK;
		}
		xmlXPathFreeObject(query_result);
	}
	if (reply->type.reply == NC_REPLY_UNKNOWN && (query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc-reply/"NC_NS_BASE10_ID":rpc-error", reply->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			reply->type.reply = NC_REPLY_ERROR;
			nc_err_parse(reply);
		}
		xmlXPathFreeObject(query_result);
	}
	/* NOTE: data element's namespace can vary (e.g. for get-schema) */
	if (reply->type.reply == NC_REPLY_UNKNOWN && (query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc-reply", reply->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval) && query_result->nodesetval->nodeNr == 1) {
			for (node = query_result->nodesetval->nodeTab[0]->children; node != NULL; node = node->next) {
				if (node->type != XML_ELEMENT_NODE) {
					continue;
				}
				if (xmlStrcmp(node->name, BAD_CAST "data") == 0) {
					reply->type.reply = NC_REPLY_DATA;
					break;
				}
			}
		}
		xmlXPathFreeObject(query_result);
	}

	return (reply->type.reply);
}

API nc_reply* nc_reply_build(const char* reply_dump)
{
	nc_reply * reply;

	if ((reply = nc_msg_build (reply_dump)) == NULL) {
		return (NULL);
	}

	/* set reply type flag */
	nc_reply_parse_type(reply);

	return (reply);
}

API nc_reply* ncxml_reply_build(xmlDocPtr reply_dump)
{
	nc_reply * reply;

	if ((reply = ncxml_msg_build (reply_dump)) == NULL) {
		return NULL;
	}

	/* set reply type flag */
	nc_reply_parse_type(reply);

	return (reply);
}

API const nc_msgid nc_reply_get_msgid(const nc_reply* reply)
{
	if (reply != NULL && reply != NCDS_RPC_NOT_APPLICABLE) {
		return (reply->msgid);
	} else {
		return (0);
	}
}

API const nc_msgid nc_rpc_get_msgid(const nc_rpc* rpc)
{
	if (rpc != NULL) {
		return (rpc->msgid);
	} else {
		return (0);
	}
}

API char* nc_rpc_get_ns(const nc_rpc* rpc)
{
	xmlNodePtr root, opnode;

	if (rpc == NULL || rpc->doc == NULL ) {
		ERROR("%s: Invalid parameter (missing message or message document).", __func__);
		return NULL;
	}

	if ((root = xmlDocGetRootElement(rpc->doc)) == NULL) {
		ERROR("%s: Invalid parameter (invalid message structure).", __func__);
		return NULL;
	}
	if (xmlStrcmp(root->name, BAD_CAST "rpc") != 0) {
		ERROR("%s: Invalid rpc message - not an <rpc> message.", __func__);
		return NULL;
	}

	/* return namespace of the first element node inside <rpc> */
	for (opnode = root->children; opnode != NULL && opnode->type != XML_ELEMENT_NODE; opnode = opnode->next);
	if (opnode == NULL) {
		ERROR("%s: Invalid message structure - no operation element.", __func__);
		return (NULL);
	} else if (opnode->ns == NULL) {
		WARN("%s: Bad message structure - operation element with missing namespace.", __func__);
		return (NULL);
	} else {
		return strdup((char *) opnode->ns->href);
	}
}

API NC_OP nc_rpc_get_op(const nc_rpc* rpc)
{
	if (rpc == NULL || rpc->doc == NULL) {
		ERROR("%s: Invalid parameter (missing message or message document).", __func__);
		return (NC_OP_UNKNOWN);
	}

	return rpc->op;
}

NC_OP nc_rpc_assign_op(nc_rpc* rpc)
{
	xmlNodePtr root, auxnode;

	assert(rpc);
	assert(rpc->doc);

	if ((root = xmlDocGetRootElement (rpc->doc)) == NULL || root->children == NULL) {
		ERROR("%s: Invalid parameter (invalid message structure).", __func__);
		return (NC_OP_UNKNOWN);
	}

	if (xmlStrcmp(root->name, BAD_CAST "rpc") != 0) {
		ERROR("%s: Invalid rpc message - not an <rpc> message.", __func__);
		return (NC_OP_UNKNOWN);
	}

	for (auxnode = root->children; auxnode; auxnode = auxnode->next) {
		if (auxnode->type != XML_ELEMENT_NODE) {
			/* not interesting node, go to another */
			continue;
		}
		/* If the operation is outside any namespace then it's treated as unknown.
		 * This check is done after the check for the node type to assure that
		 * commentary node won't cause a failure. */
		if (auxnode->ns == NULL) {
			return (NC_OP_UNKNOWN);
		}

		/* check known rpc operations */
		if ((xmlStrcmp(auxnode->name, BAD_CAST "copy-config") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_COPYCONFIG;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "delete-config") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_DELETECONFIG;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "edit-config") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_EDITCONFIG;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "get") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_GET;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "validate") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_VALIDATE;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "get-config") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_GETCONFIG;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "get-schema") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_MONITORING) == 0)) {
			rpc->op = NC_OP_GETSCHEMA;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "lock") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_LOCK;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "unlock") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_UNLOCK;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "commit") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_COMMIT;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "discard-changes") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_DISCARDCHANGES;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "kill-session") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_KILLSESSION;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "close-session") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_BASE10) == 0)) {
			rpc->op = NC_OP_CLOSESESSION;
		} else if ((xmlStrcmp(auxnode->name, BAD_CAST "create-subscription") == 0) &&
				(xmlStrcmp(auxnode->ns->href, BAD_CAST NC_NS_NOTIFICATIONS) == 0)) {
			rpc->op = NC_OP_CREATESUBSCRIPTION;
		} else {
			continue;
		}
		break;
	}
	return (rpc->op);
}

API char* nc_rpc_get_op_name(const nc_rpc* rpc)
{
	xmlNodePtr root, auxnode;

	if (rpc == NULL || rpc->doc == NULL) {
		ERROR("%s: Invalid parameter (missing message or message document).", __func__);
		return (NULL);
	}

	if ((root = xmlDocGetRootElement (rpc->doc)) == NULL || root->children == NULL) {
		ERROR("%s: Invalid parameter (invalid message structure).", __func__);
		return (NULL);
	}

	if (xmlStrcmp(root->name, BAD_CAST "rpc") != 0) {
		ERROR("%s: Invalid rpc message - not an <rpc> message.", __func__);
		return (NULL);
	}

	auxnode = root->children;
	while (auxnode != NULL && auxnode->type != XML_ELEMENT_NODE) {
		auxnode = auxnode->next;
	}

	if (auxnode == NULL) {
		ERROR("%s: Invalid rpc message - missing operation.", __func__);
		return (NULL);
	}

	return (strdup((char*)(auxnode->name)));
}

API char* nc_rpc_get_op_namespace(const nc_rpc* rpc)
{
	xmlNodePtr root, auxnode;

	if (rpc == NULL || rpc->doc == NULL) {
		ERROR("%s: Invalid parameter (missing message or message document).", __func__);
		return (NULL);
	}

	if ((root = xmlDocGetRootElement (rpc->doc)) == NULL || root->children == NULL) {
		ERROR("%s: Invalid parameter (invalid message structure).", __func__);
		return (NULL);
	}

	if (xmlStrcmp(root->name, BAD_CAST "rpc") != 0) {
		ERROR("%s: Invalid rpc message - not an <rpc> message.", __func__);
		return (NULL);
	}

	auxnode = root->children;
	while (auxnode != NULL && auxnode->type != XML_ELEMENT_NODE) {
		auxnode = auxnode->next;
	}

	if (auxnode == NULL) {
		ERROR("%s: Invalid rpc message - missing operation.", __func__);
		return (NULL);
	}

	if (auxnode->ns == NULL || auxnode->ns->href == NULL) {
		return (NULL);
	} else {
		return (strdup((char*)(auxnode->ns->href)));
	}
}

API char* nc_rpc_get_op_content(const nc_rpc* rpc)
{
	char *retval = NULL;
	xmlDocPtr aux_doc;
	xmlNodePtr node;
	xmlBufferPtr buffer;
	xmlXPathObjectPtr result = NULL;
	int i;

	if (rpc == NULL || rpc->doc == NULL) {
		return NULL;
	}

	if ((result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/*", rpc->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
			buffer = xmlBufferCreate();
			if (buffer == NULL) {
				ERROR("%s: xmlBufferCreate failed (%s:%d).", __func__, __FILE__, __LINE__);
				xmlXPathFreeObject(result);
				return NULL;
			}

			/* by copying node, move all needed namespaces into the printed nodes */
			aux_doc = xmlNewDoc(BAD_CAST "1.0");
			for (i = 0; i < result->nodesetval->nodeNr; i++) {
				if ((node = xmlDocCopyNode(result->nodesetval->nodeTab[i], aux_doc, 1)) != NULL) {
					xmlNodeDump(buffer, aux_doc, node, 1, 1);
					xmlFreeNode(node);
				}
			}
			retval = strdup((char *) xmlBufferContent(buffer));
			xmlBufferFree(buffer);
			xmlFreeDoc(aux_doc);
		}
		xmlXPathFreeObject(result);
	}

	return retval;
}

API xmlNodePtr ncxml_rpc_get_op_content(const nc_rpc* rpc)
{
	xmlNodePtr root;

	if (rpc == NULL || rpc->doc == NULL) {
		return NULL;
	}

	if ((root = xmlDocGetRootElement (rpc->doc)) == NULL) {
		return NULL;
	}

	return (xmlCopyNodeList(root->children));
}

API NC_RPC_TYPE nc_rpc_get_type(const nc_rpc* rpc)
{
	if (rpc != NULL) {
		return (rpc->type.rpc);
	} else {
		return (NC_RPC_UNKNOWN);
	}
}

NC_DATASTORE nc_rpc_assign_ds(nc_rpc* rpc, const char* ds_type)
{
	xmlXPathObjectPtr query_result = NULL;
	NC_DATASTORE retval = NC_DATASTORE_ERROR, *rpcstore = NULL;
	int i;
	char** queries = NULL;
	static char* srcs[] = {
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":source/"NC_NS_BASE10_ID":candidate",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":source/"NC_NS_BASE10_ID":running",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":source/"NC_NS_BASE10_ID":startup",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":source/"NC_NS_BASE10_ID":url",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":source/"NC_NS_BASE10_ID":config"
	};
	static char* trgs[] = {
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":candidate",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":running",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":startup",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":url",
			"/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":config"
	};
	static NC_DATASTORE retvals[] = {
			NC_DATASTORE_CANDIDATE,
			NC_DATASTORE_RUNNING,
			NC_DATASTORE_STARTUP,
			NC_DATASTORE_URL,
			NC_DATASTORE_CONFIG
	};
#define nc_rpc_get_ds_RETVALS_COUNT 5

	if (rpc == NULL || rpc->doc == NULL || rpc->ctxt == NULL) {
		ERROR("%s: invalid rpc parameter", __func__);
		return NC_DATASTORE_ERROR;
	}

	if (strcmp(ds_type, "source") == 0) {
		if (rpc->op == NC_OP_COMMIT) {
			rpc->source = NC_DATASTORE_CANDIDATE;
			return (rpc->source);
		}
		queries = srcs;
		rpcstore = &(rpc->source);
	} else if (strcmp(ds_type, "target") == 0) {
		if (rpc->op == NC_OP_COMMIT) {
			rpc->target = NC_DATASTORE_RUNNING;
			return (rpc->target);
		}
		queries = trgs;
		rpcstore = &(rpc->target);
	} else {
		ERROR("%s: invalid ds_type parameter (%s)", __func__, ds_type);
		return NC_DATASTORE_ERROR;
	}

	for (i = 0; i < nc_rpc_get_ds_RETVALS_COUNT; i++) {
		if ((query_result = xmlXPathEvalExpression(BAD_CAST queries[i], rpc->ctxt)) != NULL) {
			if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval) && query_result->nodesetval->nodeNr == 1) {
				retval = retvals[i];
				xmlXPathFreeObject(query_result);
				break;
			}
			xmlXPathFreeObject(query_result);
		}
	}

	/* store for a future use */
	*rpcstore = retval;

	return(retval);
}

API NC_DATASTORE nc_rpc_get_source(const nc_rpc* rpc)
{
	if (!rpc) {
		ERROR("%s: Invalid parameter \"rpc\".", __func__);
		return NC_DATASTORE_ERROR;
	}

	return (rpc->source);
}

API NC_DATASTORE nc_rpc_get_target(const nc_rpc* rpc)
{
	if (!rpc) {
		ERROR("%s: Invalid parameter \"rpc\".", __func__);
		return NC_DATASTORE_ERROR;
	}

	return (rpc->target);
}

/**
 * return
 *   NULL on error
 *   NCDS_RPC_NOT_APPLICABLE if query not found
 *   standalone config node on success
 */
static xmlNodePtr ncxml_rpc_get_cfg_common(const nc_rpc* rpc, xmlChar* query, char* operation, int url)
{
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr retval, config;

#ifndef DISABLE_URL
	NC_URL_PROTOCOLS protocol;
	xmlChar* url_string;
	int url_buff_fd;
	xmlDocPtr url_doc = NULL;
#endif

	if ((query_result = xmlXPathEvalExpression(query, rpc->ctxt)) != NULL) {
		if (xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			//ERROR("%s: no source config data in the %s request", __func__, operation);
			xmlXPathFreeObject(query_result);
			return (NCDS_RPC_NOT_APPLICABLE);
		} else if (query_result->nodesetval->nodeNr > 1) {
			ERROR("%s: multiple source config data in the %s request", __func__, operation);
			xmlXPathFreeObject(query_result);
			return (NULL);
		}

		config = query_result->nodesetval->nodeTab[0];
		xmlXPathFreeObject(query_result);

		if (url) {
#ifndef DISABLE_URL
			/* check requested protocol */
			protocol = nc_url_get_protocol((char*) (url_string = xmlNodeGetContent(config)));
			xmlFree(url_string);
			if (protocol == NC_URL_UNKNOWN) {
				ERROR("%s: unknown URL protocol", __func__);
				return (NULL);
			}
			if (nc_url_is_enabled(protocol) == 0) {
				ERROR("%s: URL protocol (%d) not supported", __func__, protocol);
				return (NULL);
			}

			/* get data from URL */
			if ((url_buff_fd = nc_url_open((char*) (url_string = xmlNodeGetContent(config)))) < 0) {
				xmlFree(url_string);
				return (NULL);
			}
			xmlFree(url_string);
			if ((url_doc = xmlReadFd(url_buff_fd, NULL, NULL, NC_XMLREAD_OPTIONS)) == NULL ) {
				close(url_buff_fd);
				ERROR("%s: error reading from downloaded URL file", __func__);
				return (NULL);
			}
			close(url_buff_fd);

			/* config pointer is now silently moving from rpc->doc into url_doc! */
			config = xmlDocGetRootElement(url_doc);

			/* just check that content follows :url specification in RFC 6241 */
			if (xmlStrcmp(BAD_CAST "config", config->name) != 0) {
				/* \todo check also namespace */
				ERROR("%s: no config data in the downloaded URL file", __func__);
				xmlFreeDoc(url_doc);
				return (NULL);
			}
	#else
			/* this should not happen! */
			ERROR("%s: url capability is not supported", __func__ );
			return (NULL);
#endif
		}

		/* by copying nodelist, move all needed namespaces into the list nodes */
		retval = xmlNewNode(NULL, BAD_CAST "config");
		xmlAddChildList(retval, xmlCopyNodeList(config->children));

#ifndef DISABLE_URL
		xmlFreeDoc(url_doc);
#endif

		return (retval);
	} else {
		ERROR("%s: source config data not found in the %s request", __func__, operation);
		return (NULL);
	}
}

/**
 * return
 *   NULL on error
 *   NCDS_RPC_NOT_APPLICABLE if query not found
 *   dumped config on success
 */
static char* nc_rpc_get_cfg_common(const nc_rpc* rpc, xmlChar* query, char* operation, int url)
{
	xmlNodePtr config, aux_node;
	xmlDocPtr aux_doc;
	xmlBufferPtr resultbuffer;
	char * retval = NULL;

	config = ncxml_rpc_get_cfg_common(rpc, query, operation, url);

	if (config == NULL || config == NCDS_RPC_NOT_APPLICABLE) {
		return ((char*)config);
	}

	/* dump result into string */
	resultbuffer = xmlBufferCreate();
	if (resultbuffer == NULL) {
		ERROR("%s: xmlBufferCreate failed (%s:%d).", __func__, __FILE__, __LINE__);
		return (NULL);
	}
	if (config->children == NULL) {
		/* config is empty */
		xmlBufferFree(resultbuffer);
		return (strdup(""));
	}

	aux_doc = xmlNewDoc(BAD_CAST "1.0");
	xmlDocSetRootElement(aux_doc, config);
	for (aux_node = aux_doc->children->children; aux_node != NULL; aux_node = aux_node->next) {
		xmlNodeDump(resultbuffer, aux_doc, aux_node, 2, 1);
	}
	retval = strdup((char *) xmlBufferContent(resultbuffer));
	xmlBufferFree(resultbuffer);
	xmlFreeDoc(aux_doc);

	return retval;
}

static char* nc_rpc_get_cfg_copyconfig(const nc_rpc* rpc)
{
	char* query = NULL;
	char* retval;

	if (asprintf(&query, "/%s"":rpc/%s:copy-config/%s:source/%s:config",
			NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	retval = nc_rpc_get_cfg_common(rpc, BAD_CAST query, "copy-config", 0);
	free(query);

#ifndef DISABLE_URL
	if (retval == NCDS_RPC_NOT_APPLICABLE) {
		/* try URL */
		if (asprintf(&query, "/%s"":rpc/%s:copy-config/%s:source/%s:url",
				NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		retval = nc_rpc_get_cfg_common(rpc, BAD_CAST query, "copy-config", 1);
		free(query);
	}
#endif

	return (retval);
}

static char* nc_rpc_get_cfg_editconfig(const nc_rpc* rpc)
{
	char* query = NULL;
	char* retval;

	if (asprintf(&query, "/%s"":rpc/%s:edit-config/%s:config",
			NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	retval = nc_rpc_get_cfg_common(rpc, BAD_CAST query, "edit-config", 0);
	free(query);

#ifndef DISABLE_URL
	if (retval == NCDS_RPC_NOT_APPLICABLE) {
		if (asprintf(&query, "/%s"":rpc/%s:edit-config/%s:url",
				NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		retval = nc_rpc_get_cfg_common(rpc, BAD_CAST query, "edit-config", 0);
		free(query);
	}
#endif

	return (retval);
}

static char* nc_rpc_get_cfg_validate(const nc_rpc* rpc)
{
	char* query = NULL;
	char* retval;

	if (asprintf(&query, "/%s"":rpc/%s:validate/%s:source/%s:config",
			NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	retval = nc_rpc_get_cfg_common(rpc, BAD_CAST query, "validate", 0);
	free(query);

#ifndef DISABLE_URL
	if (retval == NCDS_RPC_NOT_APPLICABLE) {
		if (asprintf(&query, "/%s"":rpc/%s:validate/%s:source/%s:url",
				NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		retval = nc_rpc_get_cfg_common(rpc, BAD_CAST query, "validate", 0);
		free(query);
	}
#endif

	return (retval);
}

static xmlNodePtr ncxml_rpc_get_cfg_copyconfig(const nc_rpc* rpc)
{
	char* query = NULL;
	xmlNodePtr retval;

	if (asprintf(&query, "/%s"":rpc/%s:validate/%s:source/%s:config",
			NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	retval = ncxml_rpc_get_cfg_common(rpc, BAD_CAST query, "copy-config", 0);
	free(query);

#ifndef DISABLE_URL
	if (retval == NCDS_RPC_NOT_APPLICABLE) {
		/* try URL */
		if (asprintf(&query, "/%s"":rpc/%s:copy-config/%s:source/%s:url",
				NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		retval = ncxml_rpc_get_cfg_common(rpc, BAD_CAST query, "copy-config", 1);
		free(query);
	}
#endif

	return (retval);
}

static xmlNodePtr ncxml_rpc_get_cfg_editconfig(const nc_rpc* rpc)
{
	char* query = NULL;
	xmlNodePtr retval;

	if (asprintf(&query, "/%s"":rpc/%s:edit-config/%s:config",
			NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	retval = ncxml_rpc_get_cfg_common(rpc, BAD_CAST query, "edit-config", 0);
	free(query);

#ifndef DISABLE_URL
	if (retval == NCDS_RPC_NOT_APPLICABLE) {
		if (asprintf(&query, "/%s"":rpc/%s:edit-config/%s:url",
				NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		retval = ncxml_rpc_get_cfg_common(rpc, BAD_CAST query, "edit-config", 0);
		free(query);
	}
#endif

	return (retval);
}

static xmlNodePtr ncxml_rpc_get_cfg_validate(const nc_rpc* rpc)
{
	char* query = NULL;
	xmlNodePtr retval;

	if (asprintf(&query, "/%s"":rpc/%s:validate/%s:source/%s:config",
			NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	retval = ncxml_rpc_get_cfg_common(rpc, BAD_CAST query, "validate", 0);
	free(query);

#ifndef DISABLE_URL
	if (retval == NCDS_RPC_NOT_APPLICABLE) {
		if (asprintf(&query, "/%s"":rpc/%s:validate/%s:source/%s:url",
				NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID, NC_NS_BASE10_ID) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		retval = ncxml_rpc_get_cfg_common(rpc, BAD_CAST query, "validate", 0);
		free(query);
	}
#endif

	return (retval);
}

API char* nc_rpc_get_config(const nc_rpc* rpc)
{
	char* retval = NULL;

	switch(nc_rpc_get_op(rpc)) {
	case NC_OP_COPYCONFIG:
		retval = nc_rpc_get_cfg_copyconfig(rpc);
		break;
	case NC_OP_EDITCONFIG:
		retval = nc_rpc_get_cfg_editconfig(rpc);
		break;
	case NC_OP_VALIDATE:
		retval = nc_rpc_get_cfg_validate(rpc);
		break;
	default:
		/* other operations do not have config parameter */
		return (NULL);
	}

	/* convert NCDS_RPC_NOT_APPLICABLE value to NULL */
	return ((retval == (char*)NCDS_RPC_NOT_APPLICABLE) ? NULL : retval);
}

API xmlNodePtr ncxml_rpc_get_config(const nc_rpc* rpc)
{
	xmlNodePtr retval = NULL;

	switch(nc_rpc_get_op(rpc)) {
	case NC_OP_COPYCONFIG:
		retval = ncxml_rpc_get_cfg_copyconfig(rpc);
		break;
	case NC_OP_EDITCONFIG:
		retval = ncxml_rpc_get_cfg_editconfig(rpc);
		break;
	case NC_OP_VALIDATE:
		retval = ncxml_rpc_get_cfg_validate(rpc);
		break;
	default:
		/* other operations do not have config parameter */
		return (NULL);
	}

	/* convert NCDS_RPC_NOT_APPLICABLE value to NULL */
	return ((retval == (xmlNodePtr)NCDS_RPC_NOT_APPLICABLE) ? NULL : retval);
}

API NC_EDIT_DEFOP_TYPE nc_rpc_get_defop(const nc_rpc* rpc)
{
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr defop = NULL;
	NC_EDIT_DEFOP_TYPE retval = NC_EDIT_DEFOP_NOTSET;

	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_BASE10_ID":edit-config/"NC_NS_BASE10_ID":default-operation", rpc->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple default-operation elements found in edit-config request", __func__);
				xmlXPathFreeObject(query_result);
				return (NC_EDIT_DEFOP_ERROR);
			}
			defop = query_result->nodesetval->nodeTab[0];

			if (defop != NULL) {
				if (defop->children == NULL || defop->children->type != XML_TEXT_NODE || defop->children->content == NULL) {
					ERROR("%s: invalid format of the edit-config's default-operation parameter", __func__);
					retval = NC_EDIT_DEFOP_ERROR;
				} else if (xmlStrEqual(defop->children->content, BAD_CAST "merge")) {
					retval = NC_EDIT_DEFOP_MERGE;
				} else if (xmlStrEqual(defop->children->content, BAD_CAST "replace")) {
					retval = NC_EDIT_DEFOP_REPLACE;
				} else if (xmlStrEqual(defop->children->content, BAD_CAST "none")) {
					retval = NC_EDIT_DEFOP_NONE;
				} else {
					ERROR("%s: unknown default-operation specified (%s)", __func__, defop->children->content);
					retval = NC_EDIT_DEFOP_ERROR;
				}
			}
		}
		xmlXPathFreeObject(query_result);
	}

	return retval;
}

API NC_EDIT_ERROPT_TYPE nc_rpc_get_erropt(const nc_rpc* rpc)
{
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr erropt = NULL;
	NC_EDIT_ERROPT_TYPE retval = NC_EDIT_ERROPT_NOTSET;

	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_BASE10_ID":edit-config/"NC_NS_BASE10_ID":error-option", rpc->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple error-option elements found in the edit-config request", __func__);
				xmlXPathFreeObject(query_result);
				return (NC_EDIT_ERROPT_ERROR);
			}
			erropt = query_result->nodesetval->nodeTab[0];

			if (erropt != NULL) {
				if (erropt->children == NULL || erropt->children->type != XML_TEXT_NODE || erropt->children->content == NULL) {
					ERROR("%s: invalid format of the edit-config's error-option parameter", __func__);
					retval = NC_EDIT_ERROPT_ERROR;
				} else if (xmlStrEqual(erropt->children->content, BAD_CAST "stop-on-error")) {
					retval = NC_EDIT_ERROPT_STOP;
				} else if (xmlStrEqual(erropt->children->content, BAD_CAST "continue-on-error")) {
					retval = NC_EDIT_ERROPT_CONT;
				} else if (xmlStrEqual(erropt->children->content, BAD_CAST "rollback-on-error")) {
					retval = NC_EDIT_ERROPT_ROLLBACK;
				} else {
					ERROR("%s: unknown error-option specified (%s)", __func__, erropt->children->content);
					retval = NC_EDIT_ERROPT_ERROR;
				}
			}
		}
		xmlXPathFreeObject(query_result);
	}

	return retval;
}

API NC_EDIT_TESTOPT_TYPE nc_rpc_get_testopt(const nc_rpc* rpc)
{
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr testopt = NULL;
	NC_EDIT_TESTOPT_TYPE retval = NC_EDIT_TESTOPT_NOTSET;

	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_BASE10_ID":edit-config/"NC_NS_BASE10_ID":test-option", rpc->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple test-option elements found in the edit-config request", __func__);
				xmlXPathFreeObject(query_result);
				return (NC_EDIT_TESTOPT_ERROR);
			}
			testopt = query_result->nodesetval->nodeTab[0];

			if (testopt != NULL) {
				if (testopt->children == NULL || testopt->children->type != XML_TEXT_NODE || testopt->children->content == NULL) {
					ERROR("%s: invalid format of the edit-config's test-option parameter", __func__);
					retval = NC_EDIT_TESTOPT_ERROR;
				} else if (xmlStrcmp(testopt->children->content, BAD_CAST "set") == 0) {
					retval = NC_EDIT_TESTOPT_SET;
				} else if (xmlStrcmp(testopt->children->content, BAD_CAST "test-only") == 0) {
					retval = NC_EDIT_TESTOPT_TEST;
				} else if (xmlStrcmp(testopt->children->content, BAD_CAST "test-then-set") == 0) {
					retval = NC_EDIT_TESTOPT_TESTSET;
				} else {
					ERROR("%s: unknown test-option specified (%s)", __func__, testopt->children->content);
					retval = NC_EDIT_TESTOPT_ERROR;
				}
			}
		}
		xmlXPathFreeObject(query_result);
	}

	return (retval);
}

API struct nc_filter* nc_rpc_get_filter(const nc_rpc* rpc)
{
	xmlXPathObjectPtr query_result = NULL;
	struct nc_filter * retval = NULL;
	xmlNodePtr filter_node = NULL;
	xmlChar *type_string;
	char* query;

	query = "/"NC_NS_BASE10_ID":rpc/"NC_NS_BASE10_ID":get/"NC_NS_BASE10_ID":filter | /"
			NC_NS_BASE10_ID":rpc/"NC_NS_BASE10_ID":get-config/"NC_NS_BASE10_ID":filter | /"
			NC_NS_BASE10_ID":rpc/"NC_NS_NOTIFICATIONS_ID":create-subscription/"NC_NS_NOTIFICATIONS_ID":filter";
	if ((query_result = xmlXPathEvalExpression(BAD_CAST query, rpc->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple filter elements found", __func__);
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			filter_node = query_result->nodesetval->nodeTab[0];
		}
		xmlXPathFreeObject(query_result);
	}

	if (filter_node != NULL) {
		retval = malloc(sizeof(struct nc_filter));
		type_string = xmlGetProp(filter_node, BAD_CAST "type");
		/* set filter type */
		if (type_string == NULL || xmlStrcmp(type_string, BAD_CAST "subtree") == 0) {
			/* includes implicit filter type (type property is not set) */
			retval->type = NC_FILTER_SUBTREE;
			retval->subtree_filter = xmlCopyNode(filter_node, 1);
		} else {
			/* some uknown filter type */
			retval->type = NC_FILTER_UNKNOWN;
			retval->subtree_filter = NULL;
		}
		xmlFree(type_string);
	}

	return retval;
}

API NC_REPLY_TYPE nc_reply_get_type(const nc_reply* reply)
{

	if (reply == NULL || reply == NCDS_RPC_NOT_APPLICABLE) {
		return (NC_REPLY_UNKNOWN);
	} else {
		return (reply->type.reply);
	}
}

API const char* nc_reply_get_data_ns(const nc_reply* reply)
{
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr data = NULL;
	const char* retval = NULL;

	/* NOTE: <data> in rpc-reply can be in various namespaces (e.g. for get-schema) */
	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc-reply", reply->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple rpc-reply elements found", __func__);
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			for (data = query_result->nodesetval->nodeTab[0]->children; data != NULL; data = data->next) {
				if (data->type != XML_ELEMENT_NODE) {
					continue;
				}
				if (xmlStrcmp(data->name, BAD_CAST "data") == 0) {
					break;
				}
			}
			if (data == NULL) {
				ERROR("%s: no data element found", __func__);
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			if (data->ns != NULL) {
				retval = (const char*)(data->ns->href);
			}
		}
		xmlXPathFreeObject(query_result);
	}

	return (retval);
}

API char* nc_reply_get_data(const nc_reply* reply)
{
	xmlXPathObjectPtr query_result = NULL;
	char *buf;
	xmlBufferPtr data_buf;
	xmlNodePtr data = NULL, aux_data;
	xmlDocPtr aux_doc;
	int gotdata = 0;

	/* NOTE: <data> in rpc-reply can be in various namespaces (e.g. for get-schema) */
	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc-reply", reply->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple rpc-reply elements found", __func__);
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			for (aux_data = query_result->nodesetval->nodeTab[0]->children; aux_data != NULL; aux_data = aux_data->next) {
				if (aux_data->type != XML_ELEMENT_NODE) {
					continue;
				}
				if (xmlStrcmp(aux_data->name, BAD_CAST "data") == 0) {
					break;
				}
			}
			if (aux_data == NULL) {
				ERROR("%s: no data element found", __func__);
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			data = xmlCopyNode(aux_data, 1);
		}
		xmlXPathFreeObject(query_result);
	}

	if (data == NULL) {
		ERROR("%s: parsing reply to get data failed. No data found.", __func__);
		return(NULL);
	}

	if ((data_buf = xmlBufferCreate()) == NULL) {
		return NULL;
	}

	aux_doc = xmlNewDoc(BAD_CAST "1.0");
	xmlDocSetRootElement(aux_doc, data);
	for (aux_data = aux_doc->children->children; aux_data != NULL; aux_data = aux_data->next) {
		if (aux_data->type == XML_ELEMENT_NODE || aux_data->type == XML_TEXT_NODE) {
			xmlNodeDump(data_buf, aux_doc, aux_data, 1, 1);
			gotdata = 1;
		}
	}
	if (gotdata == 1) {
		buf = strdup((char*) xmlBufferContent(data_buf));
	} else {
		/*
		 * Returned data content is empty, so return empty
		 * string without any error message. This can be a valid
		 * content of the reply, e.g. in case of filtering.
		 */
		buf = strdup("");
	}
	xmlBufferFree(data_buf);
	xmlFreeDoc(aux_doc);

	return (buf);
}

API xmlNodePtr ncxml_reply_get_data(const nc_reply *reply)
{
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr data = NULL;

	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc-reply/"NC_NS_BASE10_ID":data", reply->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple data elements found", __func__);
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			data = xmlCopyNode(query_result->nodesetval->nodeTab[0], 1);
		}
		xmlXPathFreeObject(query_result);
	}

	if (data == NULL) {
		ERROR("%s: parsing reply to get data failed. No data found.", __func__);
		return(NULL);
	}

	return (data);
}

API const char* nc_reply_get_errormsg(const nc_reply* reply)
{
	if (reply == NULL || reply == NCDS_RPC_NOT_APPLICABLE || reply->type.reply != NC_REPLY_ERROR) {
		return (NULL);
	}

	return ((reply->error == NULL) ? NULL : reply->error->message);
}

void nc_msg_free(struct nc_msg* msg)
{
	struct nc_err* e, *efree;
	int i;

	if (msg != NULL && msg != NCDS_RPC_NOT_APPLICABLE) {
		if (msg->doc != NULL) {
			xmlFreeDoc(msg->doc);
		}
		if (msg->ctxt != NULL) {
			xmlXPathFreeContext(msg->ctxt);
		}
		if ((e = msg->error) != NULL) {
			while(e != NULL) {
				efree = e;
				e = e->next;
				nc_err_free(efree);
			}
		}
		if (msg->msgid != NULL) {
			free(msg->msgid);
		}
		if (msg->nacm != NULL) {
			for (i = 0; msg->nacm->rule_lists != NULL && msg->nacm->rule_lists[i] != NULL; i++) {
				nacm_rule_list_free(msg->nacm->rule_lists[i]);
			}
			free(msg->nacm->rule_lists);
			free(msg->nacm);
		}
		free(msg);
	}
}

API void nc_rpc_free(nc_rpc *rpc)
{
	nc_msg_free((struct nc_msg*) rpc);
}

API void nc_reply_free(nc_reply *reply)
{
	nc_msg_free((struct nc_msg*) reply);
}

struct nc_msg *nc_msg_dup(struct nc_msg *msg)
{
	struct nc_msg *dupmsg;

	if (msg == NULL || msg == NCDS_RPC_NOT_APPLICABLE || msg->doc == NULL) {
		return (NULL);
	}

	dupmsg = calloc(1, sizeof(struct nc_msg));
	if (dupmsg == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}	
	dupmsg->doc = xmlCopyDoc(msg->doc, 1);
	dupmsg->type = msg->type;
	dupmsg->with_defaults = msg->with_defaults;
	dupmsg->op = msg->op;
	dupmsg->source = msg->source;
	dupmsg->target = msg->target;
	if (msg->nacm != NULL) {
		dupmsg->nacm = malloc(sizeof(struct nacm_rpc));
		dupmsg->nacm->default_exec = msg->nacm->default_exec;
		dupmsg->nacm->default_read = msg->nacm->default_read;
		dupmsg->nacm->default_write = msg->nacm->default_write;
		dupmsg->nacm->rule_lists = nacm_rule_lists_dup(msg->nacm->rule_lists);
	} else {
		dupmsg->nacm = NULL;
	}
	if (msg->msgid != NULL) {
		dupmsg->msgid = strdup(msg->msgid);
	} else {
		dupmsg->msgid = NULL;
	}
	if (msg->error != NULL) {
		dupmsg->error = nc_err_dup(msg->error);
	} else {
		dupmsg->error = NULL;
	}

	/* create xpath evaluation context */
	if ((dupmsg->ctxt = xmlXPathNewContext(dupmsg->doc)) == NULL) {
		ERROR("%s: rpc message XPath context cannot be created.", __func__);
		nc_msg_free(dupmsg);
		return NULL;
	}

	/* register base namespace for the rpc */
	if (xmlXPathRegisterNs(dupmsg->ctxt, BAD_CAST NC_NS_BASE10_ID, BAD_CAST NC_NS_BASE10) != 0) {
		ERROR("Registering base namespace for the message xpath context failed.");
		nc_msg_free(dupmsg);
		return NULL;
	}
	if (xmlXPathRegisterNs(dupmsg->ctxt, BAD_CAST NC_NS_NOTIFICATIONS_ID, BAD_CAST NC_NS_NOTIFICATIONS) != 0) {
		ERROR("Registering notifications namespace for the message xpath context failed.");
		nc_msg_free(dupmsg);
		return NULL;
	}
	if (xmlXPathRegisterNs(dupmsg->ctxt, BAD_CAST NC_NS_WITHDEFAULTS_ID, BAD_CAST NC_NS_WITHDEFAULTS) != 0) {
		ERROR("Registering with-defaults namespace for the message xpath context failed.");
		nc_msg_free(dupmsg);
		return NULL;
	}
	if (xmlXPathRegisterNs(dupmsg->ctxt, BAD_CAST NC_NS_MONITORING_ID, BAD_CAST NC_NS_MONITORING) != 0) {
		ERROR("Registering monitoring namespace for the message xpath context failed.");
		nc_msg_free(dupmsg);
		return NULL;
	}

	return (dupmsg);
}

API nc_rpc *nc_rpc_dup(const nc_rpc* rpc)
{
	return ((nc_rpc*)nc_msg_dup((struct nc_msg*)rpc));
}

API nc_reply *nc_reply_dup(const nc_reply* reply)
{
	return ((nc_reply*)nc_msg_dup((struct nc_msg*)reply));
}

/**
 * @brief Create a generic NETCONF message envelope according to the given
 * type (rpc or rpc-reply) and insert the given data
 *
 * @param[in] content pointer to the xml node containing the data
 * @param[in] msgtype string of the envelope element (rpc, rpc-reply)
 *
 * @return Prepared nc_msg structure.
 */
static struct nc_msg* nc_msg_create(const xmlNodePtr content, char* msgtype)
{
	struct nc_msg* msg;

	xmlDocPtr xmlmsg = NULL;
	xmlNsPtr ns;

	if (content == NULL) {
		ERROR("%s: Invalid \'content\' parameter.", __func__);
		return (NULL);
	}

	if ((xmlmsg = xmlNewDoc(BAD_CAST XML_VERSION)) == NULL) {
		ERROR("xmlNewDoc failed (%s:%d).", __FILE__, __LINE__);
		return NULL;
	}
	xmlmsg->encoding = xmlStrdup(BAD_CAST UTF8);

	if ((xmlmsg->children = xmlNewDocNode(xmlmsg, NULL, BAD_CAST msgtype, NULL)) == NULL) {
		ERROR("xmlNewDocNode failed (%s:%d).", __FILE__, __LINE__);
		xmlFreeDoc(xmlmsg);
		return NULL;
	}

	/* set namespace */
	ns = xmlNewNs(xmlmsg->children, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(xmlmsg->children, ns);

	if (xmlAddChildList(xmlmsg->children, xmlCopyNodeList(content)) == NULL) {
		ERROR("xmlAddChild failed (%s:%d).", __FILE__, __LINE__);
		xmlFreeDoc(xmlmsg);
		return NULL;
	}

	msg = calloc(1, sizeof(nc_rpc));
	if (msg == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	msg->doc = xmlmsg;
	msg->msgid = NULL;
	msg->error = NULL;
	msg->with_defaults = NCWD_MODE_NOTSET;
	msg->nacm = NULL;

	/* create xpath evaluation context */
	if ((msg->ctxt = xmlXPathNewContext(msg->doc)) == NULL) {
		ERROR("%s: rpc message XPath context cannot be created.", __func__);
		nc_msg_free(msg);
		return NULL;
	}

	/* register base namespace for the rpc */
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_BASE10_ID, BAD_CAST NC_NS_BASE10) != 0) {
		ERROR("Registering base namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_NOTIFICATIONS_ID, BAD_CAST NC_NS_NOTIFICATIONS) != 0) {
		ERROR("Registering notifications namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_WITHDEFAULTS_ID, BAD_CAST NC_NS_WITHDEFAULTS) != 0) {
		ERROR("Registering with-defaults namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_MONITORING_ID, BAD_CAST NC_NS_MONITORING) != 0) {
		ERROR("Registering monitoring namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}

#ifdef HAVE_XMLDOMWRAPRECONCILENAMESPACE
	/* remove duplicated namespace definitions */
	xmlDOMWrapReconcileNamespaces(NULL, msg->doc->children, 1);
#endif

	return (msg);
}

API nc_reply* nc_reply_ok(void)
{
	nc_reply *reply;
	xmlNodePtr content;
	xmlNsPtr ns;

	if ((content = xmlNewNode(NULL, BAD_CAST "ok")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	/* set the namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	reply = (nc_reply*)nc_msg_create(content,"rpc-reply");
	reply->type.reply = NC_REPLY_OK;
	xmlFreeNode(content);

	return (reply);
}

API nc_reply *nc_reply_data(const char* data)
{
	return (nc_reply_data_ns(data, NC_NS_BASE10));
}

API nc_reply *nc_reply_data_ns(const char* data, const char* ns)
{
	nc_reply *reply;
	xmlDocPtr doc_data;
	char* data_env;
	struct nc_err* e;
	int r;

	if (ns != NULL) {
		r = asprintf(&data_env, "<data xmlns=\"%s\">%s</data>", ns, (data == NULL) ? "" : data);
	} else {
		r = asprintf(&data_env, "<data>%s</data>", (data == NULL) ? "" : data);
	}
	if (r == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (nc_reply_error(nc_err_new(NC_ERR_OP_FAILED)));
	}

	/* prepare XML structure from given data */
	doc_data = xmlReadMemory(data_env, strlen(data_env), NULL, NULL, NC_XMLREAD_OPTIONS);
	if (doc_data == NULL) {
		ERROR("xmlReadMemory failed (%s:%d)", __FILE__, __LINE__);
		free(data_env);
		e = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(e, NC_ERR_PARAM_MSG, "Configuration data seems to be corrupted.");
		return (nc_reply_error(e));
	}

	reply = (nc_reply*)nc_msg_create(doc_data->children,"rpc-reply");
	reply->type.reply = NC_REPLY_DATA;
	xmlFreeDoc(doc_data);
	free(data_env);

	return (reply);
}

API nc_reply *ncxml_reply_data_ns(const xmlNodePtr data, const char* ns)
{
	nc_reply *reply;
	xmlNodePtr content;
	xmlNsPtr xmlns;

	content = xmlNewNode(NULL, BAD_CAST "data");
	if (content == NULL) {
		ERROR("xmlNewNode failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	if (data && xmlAddChildList(content, xmlCopyNodeList(data)) == NULL) {
		ERROR("xmlAddChildList failed (%s:%d).", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	/* set namespace */
	xmlns = xmlNewNs(content, (xmlChar *) ns, NULL);
	xmlSetNs(content, xmlns);

	reply = (nc_reply*)nc_msg_create(content,"rpc-reply");
	reply->type.reply = NC_REPLY_DATA;
	xmlFreeNode(content);

	return (reply);

}

API nc_reply *ncxml_reply_data(const xmlNodePtr data)
{
	return ncxml_reply_data_ns(data, NC_NS_BASE10);
}

static xmlNodePtr new_reply_error_content(struct nc_err* error)
{
	xmlNodePtr content, einfo = NULL, first = NULL;
	xmlNsPtr ns = NULL;

	while (error != NULL) {
		if ((content = xmlNewNode(NULL, BAD_CAST "rpc-error")) == NULL) {
			ERROR("xmlNewNode failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		if (ns == NULL) {
			/* set namespace */
			ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
		}
		xmlSetNs(content, ns);

		if (error->type != NULL) {
			if (xmlNewChild(content, ns, BAD_CAST "error-type", BAD_CAST error->type) == NULL) {
				ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
				xmlFreeNode(content);
				return (NULL);
			}
		}

		if (error->tag != NULL) {
			if (xmlNewChild(content, ns, BAD_CAST "error-tag", BAD_CAST error->tag) == NULL) {
				ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
				xmlFreeNode(content);
				return (NULL);
			}
		}

		if (error->severity != NULL) {
			if (xmlNewChild(content, ns, BAD_CAST "error-severity", BAD_CAST error->severity) == NULL) {
				ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
				xmlFreeNode(content);
				return (NULL);
			}
		}

		if (error->apptag != NULL) {
			if (xmlNewChild(content, ns, BAD_CAST "error-app-tag", BAD_CAST error->apptag) == NULL) {
				ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
				xmlFreeNode(content);
				return (NULL);
			}
		}

		if (error->path != NULL) {
			if (xmlNewChild(content, ns, BAD_CAST "error-path", BAD_CAST error->path) == NULL) {
				ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
				xmlFreeNode(content);
				return (NULL);
			}
		}

		if (error->message != NULL) {
			if (xmlNewChild(content, ns, BAD_CAST "error-message", BAD_CAST error->message) == NULL) {
				ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
				xmlFreeNode(content);
				return (NULL);
			}
		}

		/* error-info items */
		if (error->sid != NULL || error->attribute != NULL || error->element != NULL || error->ns != NULL) {
			/* prepare error-info */

			if ((einfo = xmlNewChild(content, ns, BAD_CAST "error-info", NULL)) == NULL) {
				ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
				xmlFreeNode(content);
				return (NULL);
			}

			if (error->sid != NULL) {
				if (xmlNewChild(einfo, ns, BAD_CAST "session-id", BAD_CAST error->sid) == NULL) {
					ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
					xmlFreeNode(content);
					return (NULL);
				}
			}

			if (error->attribute != NULL) {
				if (xmlNewChild(einfo, ns, BAD_CAST "bad-attribute", BAD_CAST error->attribute) == NULL) {
					ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
					xmlFreeNode(content);
					return (NULL);
				}
			}

			if (error->element != NULL) {
				if (xmlNewChild(einfo, ns, BAD_CAST "bad-element", BAD_CAST error->element) == NULL) {
					ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
					xmlFreeNode(content);
					return (NULL);
				}
			}

			if (error->ns != NULL) {
				if (xmlNewChild(einfo, ns, BAD_CAST "bad-namespace", BAD_CAST error->ns) == NULL) {
					ERROR("xmlNewChild failed (%s:%d).", __FILE__, __LINE__);
					xmlFreeNode(content);
					return (NULL);
				}
			}
		}

		if (first == NULL) {
			first = content;
		} else {
			xmlAddSibling(first, content);
		}
		error = error->next;
	}

	return(first);
}

API nc_reply *nc_reply_error(struct nc_err* error)
{
	nc_reply *reply;
	xmlNodePtr content;

	if (error == NULL) {
		ERROR("Empty error structure to create the rpc-error reply.");
		return (NULL);
	}

	if ((content = new_reply_error_content(error)) == NULL) {
		return (NULL);
	}
	if ((reply = (nc_reply*)nc_msg_create(content,"rpc-reply")) == NULL) {
		return (NULL);
	}
	reply->error = error;
	reply->type.reply = NC_REPLY_ERROR;
	xmlFreeNodeList(content);

	return (reply);
}

API int nc_reply_error_add(nc_reply *reply, struct nc_err* error)
{
	xmlNodePtr content;

	if (error == NULL || reply == NULL || reply == NCDS_RPC_NOT_APPLICABLE || reply->type.reply != NC_REPLY_ERROR) {
		return (EXIT_FAILURE);
	}
	if (reply->doc == NULL || reply->doc->children == NULL) {
		return (EXIT_FAILURE);
	}

	/* prepare new <rpc-error> part */
	if ((content = new_reply_error_content(error)) == NULL) {
		return (EXIT_FAILURE);
	}

	/* add new description into the reply */
	if (xmlAddChildList(reply->doc->children, xmlCopyNodeList(content)) == NULL) {
		ERROR("xmlAddChild failed (%s:%d).", __FILE__, __LINE__);
		xmlFreeNodeList(content);
		return (EXIT_FAILURE);
	}

	/* find last error */
	while (error->next != NULL) {
		error = error->next;
	}
	/* add error structure into the reply's list */
	error->next = reply->error;
	reply->error = error;

	/* cleanup */
	xmlFreeNodeList(content);

	return (EXIT_SUCCESS);
}

API nc_reply* nc_reply_merge(int count, ...)
{
	nc_reply *merged_reply = NULL;
	nc_reply ** to_merge = NULL;
	NC_REPLY_TYPE type = NC_REPLY_UNKNOWN, type_aux;
	va_list ap;
	struct nc_err *err;
	int i, j, t, len = 0;
	char * tmp, * data = NULL, *new_str;
	const char* data_ns = NULL;

	/* params check */
	if (count < 2) {
		WARN("%s: you should merge 2 or more reply messages (currently merging %d reply message)", __func__, count);
		if (count < 1) {
			return (NULL);
		}
	}

	to_merge = malloc((count + 1) * sizeof(nc_reply*));
	if (to_merge == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	/* initialize argument vector */
	va_start (ap, count);
	for(i = j = t = 0; i < count; i++, j++) {
		to_merge[j] = va_arg(ap, nc_reply*);
		if (to_merge[j] == NULL || to_merge[j] == NCDS_RPC_NOT_APPLICABLE) {
			/* invalid reply will not be merged */
			to_merge[j] = NULL; /* list terminating NULL byte */
			j--;
			continue;
		}

		if (t == 0) {
			/* no type set yet */
			t++;
			type = nc_reply_get_type(to_merge[j]);
		} else if (type != (type_aux = nc_reply_get_type(to_merge[j]))) {
			if ((type == NC_REPLY_UNKNOWN || type_aux == NC_REPLY_UNKNOWN) ||
			    (type == NC_REPLY_HELLO || type_aux == NC_REPLY_HELLO) ||
			    ((type == NC_REPLY_DATA || type == NC_REPLY_OK) && (type_aux == NC_REPLY_DATA || type_aux == NC_REPLY_OK))) {
				/* unable to merge such reply types */
				ERROR("%s: the type of the message %d differs (%d:%d)", __func__, i+1, type, type_aux);
				err = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(err, NC_ERR_PARAM_MSG, "Unable to prepare final operation result.");
				merged_reply = nc_reply_error(err);
				to_merge[j+1] = NULL; /* set list terminating NULL byte */
				goto finish;
			} else {
				type = NC_REPLY_ERROR;
			}
		}

		/* set list terminating NULL byte */
		to_merge[j+1] = NULL;
	}
	/* real count of valid reply messages to merge */
	count = j;

	if (count == 0) {
		/* no valid reply message to merge */
		free(to_merge);
		va_end(ap);
		return (NULL);
	} else if (count == 1) {
		/* we have only one message to merge - provide its duplication */
		merged_reply = nc_reply_dup(to_merge[0]);
		nc_reply_free(to_merge[0]);
		free(to_merge);
		va_end(ap);
		return (merged_reply);
	}

	/* merge all valid reply messages */
	switch (type) {
	case NC_REPLY_OK:
		/* just OK */
		merged_reply = nc_reply_ok();
		break;
	case NC_REPLY_DATA:
		data_ns = nc_reply_get_data_ns(to_merge[0]);
		/* join <data/> */
		for (i = 0; i < count; i++) {
			tmp = nc_reply_get_data(to_merge[i]);
			if (data == NULL) {
				len = strlen(tmp);
				data = strdup(tmp);
			} else {
				len += strlen(tmp);
				new_str = realloc(data, sizeof(char) * (len + 1));
				if (new_str == NULL) {
					ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
					free(data);
					free(tmp);
					free(to_merge);
					va_end(ap);
					return (NULL);
				}
				data = new_str;
				strcat(data, tmp);
			}
			free(tmp);
		}
		merged_reply = nc_reply_data_ns(data, data_ns);
		free(data);
		break;
	case NC_REPLY_ERROR:
		/* join all errors */
		for (i = 0; i < count; i++) {
			if (nc_reply_get_type(to_merge[i]) == NC_REPLY_ERROR) {
				if (merged_reply == NULL) {
					/* first error reply found */
					merged_reply = nc_reply_dup(to_merge[i]);
				} else {
					/* another error reply found - add error description to previous */
					nc_reply_error_add(merged_reply, to_merge[i]->error);
					to_merge[i]->error = NULL;
				}
			}
		}
		if (merged_reply == NULL) {
			/* shouldn't be here anytime */
			WARN("%s: some crappy reply merging - error reply detected but not found.", __func__);
			free(to_merge);
			va_end(ap);
			return (NULL);
		}
		break;
	default:
		break;
	}

finish:
	/* clean all merged messages */
	for (i = 0; i < count && to_merge[i] != NULL; i++) {
		nc_reply_free(to_merge[i]);
	}
	free(to_merge);

	/* finalize argument vector */
	va_end(ap);

	return (merged_reply);
}

nc_rpc *nc_rpc_closesession()
{
	nc_rpc *rpc;
	xmlNodePtr content;
	xmlNsPtr ns;

	if ((content = xmlNewNode(NULL, BAD_CAST "close-session")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_SESSION;
		rpc->op = NC_OP_CLOSESESSION;
	}
	xmlFreeNode(content);

	return (rpc);
}

static xmlNodePtr _xmlReplaceNode(xmlNodePtr old, xmlNodePtr cur)
{
	xmlNodePtr child;

	cur->children = old->children;
	cur->last = old->last;
	for (child = old->children; child != NULL; child = child->next) {
		child->parent = cur;
		/* todo copy necessary namespace declaration from old to cur */
	}
	old->children = NULL;
	old->last = NULL;
	return (old);
}

static int process_filter_param (xmlNodePtr content, const struct nc_filter* filter)
{
	xmlDocPtr doc_filter = NULL;
	xmlNodePtr node, ntf_filter;
	xmlNsPtr ns;

	if (filter != NULL) {
		if (filter->type == NC_FILTER_SUBTREE && filter->subtree_filter != NULL) {
			/*
			 * if the operation is create-subscription, change the
			 * namespace of filter element, but type has still the
			 * BASE10 namespace
			 */
			node = xmlCopyNode(filter->subtree_filter, 1);
			if (xmlStrcmp(content->name, BAD_CAST "create-subscription") == 0 &&
					xmlStrcmp(content->ns->href, BAD_CAST NC_NS_NOTIFICATIONS) == 0) {
				ntf_filter = xmlNewNode(content->ns, BAD_CAST "filter");
				ns = xmlNewNs(ntf_filter, BAD_CAST NC_NS_BASE10, BAD_CAST NC_NS_BASE10_ID);
				xmlNewNsProp(ntf_filter, ns, BAD_CAST "type", BAD_CAST "subtree");
				xmlFreeNode(_xmlReplaceNode(node, ntf_filter));
				node = ntf_filter;
			}

			/* process Subtree filter type */
			if (xmlAddChild(content, node) == NULL) {
				ERROR("xmlAddChild failed (%s:%d)", __FILE__, __LINE__);
				xmlFreeDoc(doc_filter);
				return (EXIT_FAILURE);
			}
		} else {
			WARN("%s: unknown filter type used - skipping filter.", __func__);
		}
	}
	return (EXIT_SUCCESS);
}

API int nc_rpc_capability_attr(nc_rpc* rpc, NC_CAP_ATTR attr, ...)
{
	va_list argp;
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr root, newnode;
	xmlNsPtr ns;
	NCWD_MODE mode;
	char *wd_mode;
	int i;

	if (rpc == NULL) {
		ERROR("%s: invalid RPC to modify.", __func__);
		return (EXIT_FAILURE);
	}

	va_start(argp, attr);

	switch(attr) {
	case NC_CAP_ATTR_WITHDEFAULTS_MODE:
		/* check operation from rpc */
		switch(nc_rpc_get_op(rpc)) {
		case NC_OP_GET:
		case NC_OP_GETCONFIG:
		case NC_OP_COPYCONFIG:
			/* ok */
			break;
		default:
			ERROR("%s: required operation (id %d) is not applicable to the given RPC message.", __func__, attr);
			va_end(argp);
			return (EXIT_FAILURE);
		}

		/* get variadic argument */
		mode = va_arg(argp, NCWD_MODE);

		if (mode != NCWD_MODE_NOTSET) {
			switch (mode) {
			case NCWD_MODE_ALL:
				wd_mode = "report-all";
				break;
			case NCWD_MODE_ALL_TAGGED:
				wd_mode = "report-all-tagged";
				break;
			case NCWD_MODE_TRIM:
				wd_mode = "trim";
				break;
			case NCWD_MODE_EXPLICIT:
				wd_mode = "explicit";
				break;
			default:
				ERROR("%s: Invalid with-defaults mode specified.", __func__);
				va_end(argp);
				return(EXIT_FAILURE);
				break;
			}

			if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_WITHDEFAULTS_ID":with-defaults", rpc->ctxt)) != NULL) {
				if (xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
					/* there is currently no with-defaults element */
					xmlXPathFreeObject(query_result);

					/* go to the part creating the new with-defaults element */
					goto nc_rpc_capability_attr_new_elem;
				}
				/* there is currently some with-defaults element, use it, but set required value */
				xmlNodeSetContent(query_result->nodesetval->nodeTab[0], BAD_CAST wd_mode);

				/* remove other with-defaults elements if exist - correcting the message */
				for (i = 1; i < query_result->nodesetval->nodeNr; i++) {
					xmlUnlinkNode(query_result->nodesetval->nodeTab[i]);
					xmlFreeNode(query_result->nodesetval->nodeTab[i]);
				}

				xmlXPathFreeObject(query_result);
			} else {
nc_rpc_capability_attr_new_elem:
				/* there is currently no with-defaults element */
				/* create a new with-defaults element in the operation element */
				root = xmlDocGetRootElement(rpc->doc);
				newnode = xmlNewChild(root->children, NULL, BAD_CAST "with-defaults", BAD_CAST wd_mode);
				if (newnode == NULL) {
					ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
					va_end(argp);
					return (EXIT_FAILURE);
				}
				ns = xmlNewNs(newnode, BAD_CAST NC_NS_WITHDEFAULTS, NULL);
				xmlSetNs(newnode, ns);
			}
		} else {
			/* requested NCWD_MODE_NOTSET -> remove \<with-defaults\> element if exists */
			if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_WITHDEFAULTS_ID":with-defaults", rpc->ctxt)) != NULL) {
				if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
					WARN("%s: removing with-defaults elements from the rpc", __func__);
					for (i = 0; i < query_result->nodesetval->nodeNr; i++) {
						xmlUnlinkNode(query_result->nodesetval->nodeTab[i]);
						xmlFreeNode(query_result->nodesetval->nodeTab[i]);
					}
				}
				xmlXPathFreeObject(query_result);
			}

		}
		rpc->with_defaults = mode;

		break;
	default:
		ERROR("%s: required operation (id %d) is not supported.", __func__, attr);
		va_end(argp);
		return (EXIT_FAILURE);
		break;
	}

	va_end(argp);
	return (EXIT_SUCCESS);
}

API nc_rpc* nc_rpc_getconfig(NC_DATASTORE source, const struct nc_filter *filter)
{
	nc_rpc *rpc;
	xmlNodePtr content, node;
	xmlNsPtr ns;
	char* datastore;


	switch (source) {
	case NC_DATASTORE_RUNNING:
		datastore = "running";
		break;
	case NC_DATASTORE_STARTUP:
		datastore = "startup";
		break;
	case NC_DATASTORE_CANDIDATE:
		datastore = "candidate";
		break;
	default:
		ERROR("Unknown source datastore for <get-config>.");
		return (NULL);
		break;
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "get-config")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	/* source */
	node = xmlNewChild(content, ns, BAD_CAST "source", NULL);
	if (node == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	if (xmlNewChild(node, ns, BAD_CAST datastore, NULL) == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	/* add filter specification if any required */
	if (process_filter_param(content, filter) != 0) {
		xmlFreeNode(content);
		return (NULL);
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_READ;
		rpc->op = NC_OP_GETCONFIG;
		rpc->source = source;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_get(const struct nc_filter *filter)
{
	nc_rpc *rpc;
	xmlNodePtr content;
	xmlNsPtr ns;

	if ((content = xmlNewNode(NULL, BAD_CAST "get")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	/* add filter specification if any required */
	if (process_filter_param(content, filter) != 0) {
		xmlFreeNode(content);
		return (NULL);
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_READ;
		rpc->op = NC_OP_GET;
	}
	xmlFreeNode(content);

	return (rpc);

}

API nc_rpc* nc_rpc_deleteconfig(NC_DATASTORE target, ...)
{
	nc_rpc *rpc;
	va_list argp;
	xmlNodePtr content, node_target;
	xmlNsPtr ns;
	char* datastore = NULL;
	const char* url;

	switch (target) {
	case NC_DATASTORE_RUNNING:
		ERROR("A running datastore cannot be deleted.");
		return (NULL);
		break;
	case NC_DATASTORE_STARTUP:
		datastore = "startup";
		break;
	case NC_DATASTORE_CANDIDATE:
		datastore = "candidate";
		break;
	case NC_DATASTORE_URL:
		/* work is done later */
		break;
	default:
		ERROR("Unknown target datastore for <delete-config>.");
		return (NULL);
		break;
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "delete-config")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	node_target = xmlNewChild(content, ns, BAD_CAST "target", NULL);
	if (node_target == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	if (target == NC_DATASTORE_URL) {
		/* url */
		va_start(argp, target);
		url = va_arg(argp, const char*);
		if (xmlNewChild(node_target, ns, BAD_CAST "url", BAD_CAST url) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			va_end(argp);
			return (NULL);
		}
		va_end(argp);
	} else {
		/* running, startup, candidate */
		if (xmlNewChild(node_target, ns, BAD_CAST datastore, NULL) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			return (NULL);
		}
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		rpc->op = NC_OP_DELETECONFIG;
		rpc->target = target;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_lock(NC_DATASTORE target)
{
	nc_rpc *rpc;
	xmlNodePtr content, node_target;
	xmlNsPtr ns;
	char* datastore;

	switch (target) {
	case NC_DATASTORE_RUNNING:
		datastore = "running";
		break;
	case NC_DATASTORE_STARTUP:
		datastore = "startup";
		break;
	case NC_DATASTORE_CANDIDATE:
		datastore = "candidate";
		break;
	default:
		ERROR("Unknown target datastore for <lock>.");
		return (NULL);
		break;
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "lock")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	node_target = xmlNewChild(content, ns, BAD_CAST "target", NULL);
	if (node_target == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}
	if (xmlNewChild(node_target, ns, BAD_CAST datastore, NULL) == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		rpc->op = NC_OP_LOCK;
		rpc->target = target;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_unlock(NC_DATASTORE target)
{
	nc_rpc *rpc;
	xmlNodePtr content, node_target;
	xmlNsPtr ns;
	char* datastore;

	switch (target) {
	case NC_DATASTORE_RUNNING:
		datastore = "running";
		break;
	case NC_DATASTORE_STARTUP:
		datastore = "startup";
		break;
	case NC_DATASTORE_CANDIDATE:
		datastore = "candidate";
		break;
	default:
		ERROR("Unknown target datastore for <unlock>.");
		return (NULL);
		break;
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "unlock")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	node_target = xmlNewChild(content, ns, BAD_CAST "target", NULL);
	if (node_target == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}
	if (xmlNewChild(node_target, ns, BAD_CAST datastore, NULL) == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		rpc->op = NC_OP_UNLOCK;
		rpc->target = target;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_validate(NC_DATASTORE source, ...)
{
	nc_rpc *rpc;
	xmlNodePtr content, node_source, node_config;
	xmlDocPtr config;
	xmlNsPtr ns;
	const char *datastore = NULL;
	const char *source_url = NULL;
	const char* config_s = NULL;
	char* config_S;
	va_list argp;

	va_start(argp, source);

	switch (source) {
	case NC_DATASTORE_CONFIG:
		config_s = nc_skip_xmldecl(va_arg(argp, const char*));
		if (config_s == NULL || strlen(config_s) < 4) { /* at least '<x/>' */
			ERROR("Invalid configuration data for validate operation");
			va_end(argp);
			return NULL;
		}
		break;
	case NC_DATASTORE_URL:
		source_url = va_arg(argp, const char*);
		break;
	case NC_DATASTORE_RUNNING:
		datastore = "running";
		break;
	case NC_DATASTORE_STARTUP:
		datastore = "startup";
		break;
	case NC_DATASTORE_CANDIDATE:
		datastore = "candidate";
		break;
	default:
		ERROR("Unknown source for <validate>.");
		va_end(argp);
		return NULL;
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "validate")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		va_end(argp);
		return NULL;
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	node_source = xmlNewChild(content, ns, BAD_CAST "source", NULL);
	if (node_source == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		va_end(argp);
		return NULL;
	}

	if (config_s != NULL) {
		/* source is specified as a content of the <config> element */

		/* transform string to the xmlNodePtr */
		/* add covering <config> element to allow to specify multiple root elements */
		if (asprintf(&config_S, "<config>%s</config>", config_s) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			xmlFreeNode(content);
			va_end(argp);
			return (NULL);
		}

		/* prepare XML structure from given data */
		config = xmlReadMemory(config_S, strlen(config_S), NULL, NULL, NC_XMLREAD_OPTIONS);
		free(config_S);
		if (config == NULL) {
			ERROR("xmlReadMemory failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			va_end(argp);
			return (NULL);
		}
		if (config->children == NULL || config->children->children == NULL) {
			ERROR("Invalid configuration data for validate operation");
			xmlFreeNode(content);
			xmlFreeDoc(config);
			va_end(argp);
			return NULL;
		}

		node_config = xmlNewChild(node_source, ns, BAD_CAST "config", NULL);
		if (node_config == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			xmlFreeDoc(config);
			va_end(argp);
			return NULL;
		}

		/* connect given configuration data with the rpc request */
		if (xmlAddChildList(node_config, xmlCopyNodeList(config->children->children)) == NULL) {
			ERROR("xmlAddChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			xmlFreeDoc(config);
			va_end(argp);
			return NULL;
		}
		xmlFreeDoc(config);
	} else {
		/* source is one of the standard datastores */
		if (datastore != NULL) {
			if (xmlNewChild(node_source, ns, BAD_CAST datastore, NULL) == NULL) {
				ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
				xmlFreeNode(content);
				va_end(argp);
				return NULL;
			}
		} else if (source_url != NULL) {
			/* url specified */
			if (xmlNewChild(node_source, ns, BAD_CAST "url", BAD_CAST source_url) == NULL) {
				ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
				xmlFreeNode(content);
				va_end(argp);
				return NULL;
			}
		} else {
			xmlFreeNode(content);
			va_end(argp);
			return NULL;
		}
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_READ;
		rpc->op = NC_OP_VALIDATE;
		rpc->source = source;
	}

	xmlFreeNode(content);
	va_end(argp);
	return rpc;
}


static nc_rpc* _rpc_copyconfig(NC_DATASTORE source, NC_DATASTORE target, const xmlNodePtr config, const char* source_url, const char* target_url)
{
	nc_rpc *rpc = NULL;
	xmlNodePtr content, node_target, node_source, node_config;
	xmlNsPtr ns;
	NC_DATASTORE params[2] = {source, target};
	char *datastores[2] = {NULL, NULL}; /* 0 - source, 1 - target */
	int i;

	if (target == source && target != NC_DATASTORE_URL) {
		ERROR("<copy-config>'s source and target parameters identify the same datastore.");
		return (NULL);
	}

	for (i = 0; i < 2; i++) {
		switch (params[i]) {
		case NC_DATASTORE_RUNNING:
			datastores[i] = "running";
			break;
		case NC_DATASTORE_STARTUP:
			datastores[i] = "startup";
			break;
		case NC_DATASTORE_CANDIDATE:
			datastores[i] = "candidate";
			break;
		case NC_DATASTORE_CONFIG:
			if (i == 1) {
				ERROR("Unknown target datastore for <copy-config>.");
				return (NULL);
			}
			break;
		case NC_DATASTORE_URL:
			if (i == 0) {
				if (source_url == NULL) {
					ERROR("Missing the URL specification for the <copy-config>'s source.");
					return (NULL);
				}
			} else { /* i == 1 */
				if (target_url == NULL) {
					ERROR("Missing the URL specification for the <copy-config>'s target.");
					return (NULL);
				}
			}
			break;
		default:
			ERROR("Unknown %s datastore for <copy-config>.", (i == 0) ? "source" : "target");
			return (NULL);
			break;
		}
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "copy-config")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);
	
	/* <target> */
	node_target = xmlNewChild(content, ns, BAD_CAST "target", NULL);
	if (node_target == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		goto cleanup;
	}
	if (params[1] == NC_DATASTORE_URL) {
		/* target is specified as URL */
		if (xmlNewChild(node_target, ns, BAD_CAST "url", BAD_CAST target_url) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	} else {
		/* standard datastore */
		if (xmlNewChild(node_target, ns, BAD_CAST datastores[1], NULL) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	}

	/* <source> */
	node_source = xmlNewChild(content, ns, BAD_CAST "source", NULL);
	if (node_source == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		goto cleanup;
	}
	if (params[0] == NC_DATASTORE_CONFIG) {
		/* source configuration is given as data parameter */

		/* if data is an empty string, create \<copy-config\> with empty \<config\> */
		/* only if the data is not an empty string fill \<config\> */
		/* RFC 6241 defines \<config\> as any xml and thus it can be empty */

		/* create empty config element in rpc request */
		if ((node_config = xmlNewChild(node_source, ns, BAD_CAST "config", NULL)) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
		if (config != NULL) {
			/* connect given configuration data with the rpc request */
			if (xmlAddChildList(node_config, xmlCopyNodeList(config)) == NULL) {
				ERROR("xmlAddChild failed (%s:%d)", __FILE__, __LINE__);
				goto cleanup;
			}
		}
	} else if (params[0] == NC_DATASTORE_URL) {
		/* source is specified as URL */
		if (xmlNewChild(node_source, ns, BAD_CAST "url", BAD_CAST source_url) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	} else {
		/* source is one of the standard datastores */
		if (xmlNewChild(node_source, ns, BAD_CAST datastores[0], NULL) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		rpc->op = NC_OP_COPYCONFIG;
		rpc->source = source;
		rpc->target = target;
	}

cleanup:
	xmlFreeNode(content);
	return (rpc);

}

API nc_rpc *ncxml_rpc_copyconfig(NC_DATASTORE source, NC_DATASTORE target, ...)
{
	xmlNodePtr config = NULL;
	const char *source_url = NULL, *target_url = NULL;
	nc_rpc* retval;
	va_list argp;

	va_start(argp, target);

	if (source == NC_DATASTORE_CONFIG) {
		config = va_arg(argp, xmlNodePtr);
	} else if (source == NC_DATASTORE_URL) {
		source_url = va_arg(argp, const char*);
	}

	if (target == NC_DATASTORE_URL) {
		target_url = va_arg(argp, const char*);
	}

	retval = _rpc_copyconfig(source, target, config, source_url, target_url);
	va_end(argp);
	return (retval);
}

API nc_rpc *nc_rpc_copyconfig(NC_DATASTORE source, NC_DATASTORE target, ...)
{
	xmlDocPtr config = NULL;
	const char* config_s;
	char* config_S;
	const char *source_url = NULL, *target_url = NULL;
	nc_rpc* retval;
	va_list argp;

	va_start(argp, target);

	if (source == NC_DATASTORE_CONFIG) {
		config_s = nc_skip_xmldecl(va_arg(argp, const char*));
		if (config_s == NULL) {
			/* content is corrupted */
			ERROR("Invalid configuration data for <copy-config> operation");
			va_end(argp);
			return NULL;
		}

		/* transform string to the xmlNodePtr */
		/* add covering <config> element to allow to specify multiple root elements */
		if (asprintf(&config_S, "<config>%s</config>", config_s) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			va_end(argp);
			return (NULL);
		}

		/* prepare XML structure from given data */
		config = xmlReadMemory(config_S, strlen(config_S), NULL, NULL, NC_XMLREAD_OPTIONS);
		free(config_S);
		if (config == NULL) {
			ERROR("xmlReadMemory failed (%s:%d)", __FILE__, __LINE__);
			va_end(argp);
			return (NULL);
		}
	} else if (source == NC_DATASTORE_URL) {
		source_url = va_arg(argp, const char*);
	}

	if (target == NC_DATASTORE_URL) {
		target_url = va_arg(argp, const char*);
	}

	retval = _rpc_copyconfig(source, target, (config != NULL) ? config->children->children : NULL, source_url, target_url);
	va_end(argp);
	xmlFreeDoc(config);
	return (retval);
}

static nc_rpc *_rpc_editconfig(NC_DATASTORE target, NC_DATASTORE source, NC_EDIT_DEFOP_TYPE default_operation, NC_EDIT_ERROPT_TYPE error_option, NC_EDIT_TESTOPT_TYPE test_option, const xmlNodePtr config, const char* source_url)
{
	nc_rpc *rpc = NULL;
	xmlNodePtr content, node_target, node_defop, node_config;
	xmlNsPtr ns;
	char* datastore, *defop = NULL, *erropt = NULL, *testopt = NULL;

	/* detect target datastore */
	switch (target) {
	case NC_DATASTORE_RUNNING:
		datastore = "running";
		break;
	case NC_DATASTORE_STARTUP:
		datastore = "startup";
		break;
	case NC_DATASTORE_CANDIDATE:
		datastore = "candidate";
		break;
	default:
		ERROR("Unknown target datastore for <edit-config>.");
		return (NULL);
		break;
	}


	/* detect default-operation parameter */
	if (default_operation != 0) {
		switch (default_operation) {
		case NC_EDIT_DEFOP_MERGE:
			defop = "merge";
			break;
		case NC_EDIT_DEFOP_NONE:
			defop = "none";
			break;
		case NC_EDIT_DEFOP_REPLACE:
			defop = "replace";
			break;
		default:
			ERROR("Unknown default-operation parameter for <edit-config>.");
			return (NULL);
			break;
		}
	}

	/* detect error-option parameter */
	if (error_option != 0) {
		switch (error_option) {
		case NC_EDIT_ERROPT_STOP:
			erropt = "stop-on-error";
			break;
		case NC_EDIT_ERROPT_CONT:
			erropt = "continue-on-error";
			break;
		case NC_EDIT_ERROPT_ROLLBACK:
			erropt = "rollback-on-error";
			break;
		default:
			ERROR("Unknown error-option parameter for <edit-config>.");
			return (NULL);
			break;
		}
	}

	/* detect test-option parameter */
	if (test_option != 0) {
		switch (test_option) {
		case NC_EDIT_TESTOPT_SET:
			testopt = "set";
			break;
		case NC_EDIT_TESTOPT_TEST:
			testopt = "test-only";
			break;
		case NC_EDIT_TESTOPT_TESTSET:
			testopt = "test-then-set";
			break;
		default:
			ERROR("Unknown test-option parameter for <edit-config>.");
			return (NULL);
			break;
		}
	}

	/* create edit-config envelope */
	if ((content = xmlNewNode(NULL, BAD_CAST "edit-config")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	/* set <target> element */
	node_target = xmlNewChild(content, ns, BAD_CAST "target", NULL);
	if (node_target == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		goto cleanup;
	}
	if (xmlNewChild(node_target, NULL, BAD_CAST datastore, NULL) == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		goto cleanup;
	}

	/* set <default-operation> element */
	if (default_operation != 0) {
		node_defop = xmlNewChild(content, ns, BAD_CAST "default-operation", BAD_CAST defop);
		if (node_defop == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	}

	/* set <error-option> element */
	if (error_option != 0) {
		if (xmlNewChild(content, ns, BAD_CAST "error-option", BAD_CAST erropt) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	}

	/* set <test-option> element */
	if (test_option != 0) {
		if (xmlNewChild(content, ns, BAD_CAST "test-option", BAD_CAST testopt) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	}

	if (source == NC_DATASTORE_CONFIG) {
		/* set <config> element */

		/* create empty config element in rpc request */
		if ((node_config = xmlNewChild(content, ns, BAD_CAST "config", NULL)) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
		if (config != NULL) {
			/* connect given configuration data with the rpc request */
			if (xmlAddChildList(node_config, xmlCopyNodeList(config)) == NULL) {
				ERROR("xmlAddChild failed (%s:%d)", __FILE__, __LINE__);
				goto cleanup;
			}
		}
	} else if (source == NC_DATASTORE_URL) {
		/* set <url> instead of <config> */
		if (xmlNewChild(content, ns, BAD_CAST "url", BAD_CAST source_url) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			goto cleanup;
		}
	} else {
		ERROR("%s: unknown (or prohibited) source for <edit-config>.", __func__);
		goto cleanup;
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		rpc->op = NC_OP_EDITCONFIG;
		rpc->target = target;
	}
cleanup:
	xmlFreeNode(content);
	return (rpc);
}

API nc_rpc* ncxml_rpc_editconfig(NC_DATASTORE target, NC_DATASTORE source, NC_EDIT_DEFOP_TYPE default_operation, NC_EDIT_ERROPT_TYPE error_option, NC_EDIT_TESTOPT_TYPE test_option, ...)
{
	xmlNodePtr config = NULL;
	const char* url = NULL;
	nc_rpc* retval;
	va_list argp;

	va_start(argp, test_option);
	switch (source) {
	case NC_DATASTORE_CONFIG:
		config = va_arg(argp, xmlNodePtr);
		break;
	case NC_DATASTORE_URL:
		url = va_arg(argp, const char*);
		break;
	default:
		ERROR("Unknown (or prohibited) source for <edit-config>.");
		va_end(argp);
		return (NULL);
		break;
	}

	retval = _rpc_editconfig(target, source, default_operation, error_option, test_option, config, url);
	va_end(argp);
	return(retval);
}

API nc_rpc* nc_rpc_editconfig(NC_DATASTORE target, NC_DATASTORE source, NC_EDIT_DEFOP_TYPE default_operation, NC_EDIT_ERROPT_TYPE error_option, NC_EDIT_TESTOPT_TYPE test_option, ...)
{
	xmlDocPtr config = NULL;
	const char* url = NULL, *config_s = NULL;
	char* config_S;
	nc_rpc* retval;
	va_list argp;

	va_start(argp, test_option);
	switch (source) {
	case NC_DATASTORE_CONFIG:
		config_s = nc_skip_xmldecl(va_arg(argp, const char*));
		if (config_s == NULL) {
			/* content is corrupted */
			ERROR("Invalid configuration data for <edit-config> operation");
			va_end(argp);
			return NULL;
		}
		break;
	case NC_DATASTORE_URL:
		url = va_arg(argp, const char*);
		break;
	default:
		ERROR("Unknown (or prohibited) source for <edit-config>.");
		va_end(argp);
		return (NULL);
		break;
	}

	/* transform string to the xmlNodePtr */
	/* add covering <config> element to allow to specify multiple root elements */
	if (asprintf(&config_S, "<config>%s</config>", config_s) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		va_end(argp);
		return (NULL);
	}

	/* prepare XML structure from given data */
	config = xmlReadMemory(config_S, strlen(config_S), NULL, NULL, NC_XMLREAD_OPTIONS);
	free(config_S);
	if (config == NULL) {
		ERROR("xmlReadMemory failed (%s:%d)", __FILE__, __LINE__);
		va_end(argp);
		return (NULL);
	}

	retval = _rpc_editconfig(target, source, default_operation, error_option, test_option, config->children->children, url);
	xmlFreeDoc(config);
	va_end(argp);
	return(retval);
}

API nc_rpc* nc_rpc_killsession(const char *kill_sid)
{
	nc_rpc *rpc;
	xmlNodePtr content, node_sid;
	xmlNsPtr ns;

	/* check input parameter */
	if (kill_sid == NULL || strlen(kill_sid) == 0) {
		ERROR("Invalid session id for the <kill-session> rpc message specified.");
		return (NULL);
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "kill-session")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	node_sid = xmlNewChild(content, ns, BAD_CAST "session-id", BAD_CAST kill_sid);
	if (node_sid == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_SESSION;
		rpc->op = NC_OP_KILLSESSION;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_getschema(const char* name, const char* version, const char* format)
{
	nc_rpc *rpc;
	xmlNodePtr content;
	xmlNsPtr ns;

	/* check mandatory input parameter */
	if (name == NULL) {
		ERROR("Invalid schema name specified.");
		return (NULL);
	}

	if ((content = xmlNewNode(NULL, BAD_CAST "get-schema")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	ns = xmlNewNs(content, BAD_CAST NC_NS_MONITORING, NULL);
	xmlSetNs(content, ns);

	if (xmlNewChild(content, ns, BAD_CAST "identifier", BAD_CAST name) == NULL) {
		ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
		xmlFreeNode(content);
		return (NULL);
	}

	if (version != NULL) {
		if (xmlNewChild(content, ns, BAD_CAST "version", BAD_CAST version) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			return (NULL);
		}
	}

	if (format != NULL) {
		if (xmlNewChild(content, ns, BAD_CAST "format", BAD_CAST format) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			return (NULL);
		}
	}

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_READ;
		rpc->op = NC_OP_GETSCHEMA;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_subscribe(const char* stream, const struct nc_filter *filter, const time_t* start, const time_t* stop)
{
	nc_rpc *rpc = NULL;
	xmlNodePtr content;
	xmlNsPtr ns;
	char* time;

	if ((content = xmlNewNode(NULL, BAD_CAST "create-subscription")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	ns = xmlNewNs(content, BAD_CAST NC_NS_NOTIFICATIONS, NULL);
	xmlSetNs(content, ns);

	/* add <stream> specification if set */
	if (stream != NULL) {
		if (xmlNewChild(content, ns, BAD_CAST "stream", BAD_CAST stream) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			return (NULL);
		}
	}

	/* add filter specification if any required */
	if (process_filter_param(content, filter) != 0) {
		xmlFreeNode(content);
		return (NULL);
	}

	/* add <startTime> specification if set */
	if (start != NULL) {
		time = nc_time2datetime(*start, NULL);
		if (time == NULL || xmlNewChild(content, ns, BAD_CAST "startTime", BAD_CAST time) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			if (time != NULL) {
				free (time);
			}
			return (NULL);
		}
		free(time);
	}

	/* add <stopTime> specification if set */
	if (stop != NULL) {
		time = nc_time2datetime(*stop, NULL);
		if (time == NULL || xmlNewChild(content, ns, BAD_CAST "stopTime", BAD_CAST time) == NULL) {
			ERROR("xmlNewChild failed (%s:%d)", __FILE__, __LINE__);
			xmlFreeNode(content);
			if (time != NULL) {
				free (time);
			}
			return (NULL);
		}
		free(time);
	}

	/* finnish the message building */
	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_SESSION;
		rpc->op = NC_OP_CREATESUBSCRIPTION;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_commit(void)
{
	nc_rpc *rpc;
	xmlNodePtr content;
	xmlNsPtr ns;

	if ((content = xmlNewNode(NULL, BAD_CAST "commit")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		rpc->op = NC_OP_COMMIT;
		rpc->source = NC_DATASTORE_CANDIDATE;
		rpc->target = NC_DATASTORE_RUNNING;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* nc_rpc_discardchanges(void)
{
	nc_rpc *rpc;
	xmlNodePtr content;
	xmlNsPtr ns;

	if ((content = xmlNewNode(NULL, BAD_CAST "discard-changes")) == NULL) {
		ERROR("xmlNewNode failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* set namespace */
	ns = xmlNewNs(content, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(content, ns);

	if ((rpc = (nc_rpc*)nc_msg_create(content, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_DATASTORE_WRITE;
		rpc->op = NC_OP_DISCARDCHANGES;
	}
	xmlFreeNode(content);

	return (rpc);
}

API nc_rpc* ncxml_rpc_generic(const xmlNodePtr data)
{
	nc_rpc *rpc;

	if (data == NULL) {
		ERROR("%s: parameter \'data\' cannot be NULL.", __func__);
		return (NULL);
	}
	if ((rpc = (nc_rpc*)nc_msg_create(data, "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_UNKNOWN;
		rpc->op = NC_OP_UNKNOWN;
	}

	return (rpc);
}

API nc_rpc* nc_rpc_generic(const char* data)
{
	nc_rpc *rpc;
	xmlDocPtr doc_data;

	if (data == NULL) {
		ERROR("%s: parameter \'data\' cannot be NULL.", __func__);
		return (NULL);
	}

	/* read data as XML document */
	doc_data = xmlReadMemory(data, strlen(data), NULL, NULL, NC_XMLREAD_OPTIONS);
	if (doc_data == NULL) {
		ERROR("xmlReadMemory failed (%s:%d)", __FILE__, __LINE__);
		return (NULL);
	}

	if ((rpc = (nc_rpc*)nc_msg_create(xmlDocGetRootElement(doc_data), "rpc")) != NULL) {
		rpc->type.rpc = NC_RPC_UNKNOWN;
		rpc->op = NC_OP_UNKNOWN;
	}
	xmlFreeDoc(doc_data);

	return (rpc);
}
