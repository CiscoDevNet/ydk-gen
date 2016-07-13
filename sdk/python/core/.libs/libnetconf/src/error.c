/**
 * \file error.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of NETCONF error handling functions.
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

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "error.h"
#include "netconf.h"
#include "netconf_internal.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

API struct nc_err* nc_err_new(NC_ERR error)
{
	struct nc_err* err = NULL;

	err = calloc(1, sizeof(struct nc_err));
	if (err == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	/* set error structure according to some predefined errors in RFC 6241 */
	switch (error) {
	case NC_ERR_EMPTY:
		/* do nothing */
		break;
	case NC_ERR_IN_USE:
		nc_err_set(err, NC_ERR_PARAM_TAG, "in-use");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "The request requires a resource that is already in use.");
		break;
	case NC_ERR_INVALID_VALUE:
		nc_err_set(err, NC_ERR_PARAM_TAG, "invalid-value");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "The request specifies an unacceptable value for one or more parameters.");
		break;
	case NC_ERR_TOO_BIG:
		nc_err_set(err, NC_ERR_PARAM_TAG, "too-big");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "The request or response is too large for the implementation to handle.");
		break;
	case NC_ERR_MISSING_ATTR:
		nc_err_set(err, NC_ERR_PARAM_TAG, "missing-attribute");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "An expected attribute is missing.");
		break;
	case NC_ERR_BAD_ATTR:
		nc_err_set(err, NC_ERR_PARAM_TAG, "bad-attribute");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "An attribute value is not correct.");
		break;
	case NC_ERR_UNKNOWN_ATTR:
		nc_err_set(err, NC_ERR_PARAM_TAG, "unknown-attribute");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "An unexpected attribute is present.");
		break;
	case NC_ERR_MISSING_ELEM:
		nc_err_set(err, NC_ERR_PARAM_TAG, "missing-element");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "An expected element is missing.");
		break;
	case NC_ERR_BAD_ELEM:
		nc_err_set(err, NC_ERR_PARAM_TAG, "bad-element");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "An element value is not correct.");
		break;
	case NC_ERR_UNKNOWN_ELEM:
		nc_err_set(err, NC_ERR_PARAM_TAG, "unknown-element");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "An unexpected element is present.");
		break;
	case NC_ERR_UNKNOWN_NS:
		nc_err_set(err, NC_ERR_PARAM_TAG, "unknown-namespace");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "An unexpected namespace is present.");
		break;
	case NC_ERR_ACCESS_DENIED:
		nc_err_set(err, NC_ERR_PARAM_TAG, "access-denied");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Access to the requested protocol operation or data model is denied because the authorization failed.");
		break;
	case NC_ERR_LOCK_DENIED:
		nc_err_set(err, NC_ERR_PARAM_TAG, "lock-denied");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "protocol");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Access to the requested lock is denied because the lock is currently held by another entity.");
		break;
	case NC_ERR_RES_DENIED:
		nc_err_set(err, NC_ERR_PARAM_TAG, "resource-denied");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "protocol");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Request could not be completed because of insufficient resources.");
		break;
	case NC_ERR_ROLLBACK_FAILED:
		nc_err_set(err, NC_ERR_PARAM_TAG, "rollback-failed");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Request to roll back some configuration change was not completed for some reason.");
		break;
	case NC_ERR_DATA_EXISTS:
		nc_err_set(err, NC_ERR_PARAM_TAG, "data-exists");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Request could not be completed because the relevant data model content already exists.");
		break;
	case NC_ERR_DATA_MISSING:
		nc_err_set(err, NC_ERR_PARAM_TAG, "data-missing");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Request could not be completed because the relevant data model content does not exist.");
		break;
	case NC_ERR_OP_NOT_SUPPORTED:
		nc_err_set(err, NC_ERR_PARAM_TAG, "operation-not-supported");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Request could not be completed because the requested operation is not supported by this implementation.");
		break;
	case NC_ERR_OP_FAILED:
		nc_err_set(err, NC_ERR_PARAM_TAG, "operation-failed");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "application");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "Some unspecified error occurred.");
		break;
	case NC_ERR_MALFORMED_MSG:
		nc_err_set(err, NC_ERR_PARAM_TAG, "malformed-message");
		nc_err_set(err, NC_ERR_PARAM_TYPE, "rpc");
		nc_err_set(err, NC_ERR_PARAM_SEVERITY, "error");
		nc_err_set(err, NC_ERR_PARAM_MSG, "A message could not be handled because it failed to be parsed correctly.");
		break;
	}

	return (err);
}

API struct nc_err* nc_err_dup(const struct nc_err* err)
{
	struct nc_err *duperr;

	if (err == NULL) {
		ERROR("%s: no error structure to duplicate.", __func__);
		return (NULL);
	}

	duperr = calloc(1, sizeof(struct nc_err));
	if (duperr == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	if (err->apptag) {
		duperr->apptag = strdup(err->apptag);
	}
	if (err->attribute) {
		duperr->attribute = strdup(err->attribute);
	}
	if (err->element) {
		duperr->element = strdup(err->element);
	}
	if (err->message) {
		duperr->message = strdup(err->message);
	}
	if (err->ns) {
		duperr->ns = strdup(err->ns);
	}
	if (err->path) {
		duperr->path = strdup(err->path);
	}
	if (err->severity) {
		duperr->severity = strdup(err->severity);
	}
	if (err->sid) {
		duperr->sid = strdup(err->sid);
	}
	if (err->tag) {
		duperr->tag = strdup(err->tag);
	}
	if (err->type) {
		duperr->type = strdup(err->type);
	}

	return (duperr);
}

API void nc_err_free(struct nc_err* err)
{
	if (err != NULL) {
		if (err->apptag) {
			free(err->apptag);
		}
		if (err->attribute) {
			free(err->attribute);
		}
		if (err->element) {
			free(err->element);
		}
		if (err->message) {
			free(err->message);
		}
		if (err->ns) {
			free(err->ns);
		}
		if (err->path) {
			free(err->path);
		}
		if (err->severity) {
			free(err->severity);
		}
		if (err->sid) {
			free(err->sid);
		}
		if (err->tag) {
			free(err->tag);
		}
		if (err->type) {
			free(err->type);
		}
		free(err);
	}
}

API const char* nc_err_get(const struct nc_err* err, NC_ERR_PARAM param)
{

	if (err == NULL) {
		ERROR("Invalid NETCONF error structure to set.");
		return (NULL);
	}

	switch (param) {
	case NC_ERR_PARAM_TYPE:
		return (err->type);
	case NC_ERR_PARAM_TAG:
		return (err->tag);
	case NC_ERR_PARAM_SEVERITY:
		return (err->severity);
	case NC_ERR_PARAM_APPTAG:
		return (err->apptag);
	case NC_ERR_PARAM_PATH:
		return (err->path);
	case NC_ERR_PARAM_MSG:
		return (err->message);
	case NC_ERR_PARAM_INFO_BADATTR:
		return (err->attribute);
	case NC_ERR_PARAM_INFO_BADELEM:
		return (err->element);
	case NC_ERR_PARAM_INFO_BADNS:
		return (err->ns);
	case NC_ERR_PARAM_INFO_SID:
		return (err->sid);
	default:
		ERROR("Unknown parameter for NETCONF error to get.");
		return (NULL);
	}
}

API int nc_err_set(struct nc_err* err, NC_ERR_PARAM param, const char* value)
{
	char** param_item = NULL;

	if (err == NULL) {
		ERROR("Invalid NETCONF error structure to set.");
		return (EXIT_FAILURE);
	}
	if (value == NULL) {
		ERROR("Invalid value for NETCONF error parameter.");
		return (EXIT_FAILURE);
	}

	/* find out which parameter will be set */
	switch (param) {
	case NC_ERR_PARAM_TYPE:
		param_item = &(err->type);
		break;
	case NC_ERR_PARAM_TAG:
		param_item = &(err->tag);
		break;
	case NC_ERR_PARAM_SEVERITY:
		param_item = &(err->severity);
		break;
	case NC_ERR_PARAM_APPTAG:
		param_item = &(err->apptag);
		break;
	case NC_ERR_PARAM_PATH:
		param_item = &(err->path);
		break;
	case NC_ERR_PARAM_MSG:
		param_item = &(err->message);
		break;
	case NC_ERR_PARAM_INFO_BADATTR:
		param_item = &(err->attribute);
		break;
	case NC_ERR_PARAM_INFO_BADELEM:
		param_item = &(err->element);
		break;
	case NC_ERR_PARAM_INFO_BADNS:
		param_item = &(err->ns);
		break;
	case NC_ERR_PARAM_INFO_SID:
		param_item = &(err->sid);
		break;
	default:
		ERROR("Unknown parameter for NETCONF error to set.");
		break;
	}

	/* set selected parameter to the specified value */
	if (param_item != NULL) {
		if (*param_item != NULL) {
			/* remove previous value if any */
			free(*param_item);
		}
		*param_item = strdup(value);
	}

	return (EXIT_SUCCESS);
}

/**
 * @brief Parse the given reply message and create a NETCONF error structure
 * describing the error from the reply. Reply must be of the #NC_REPLY_ERROR type.
 * @param[in] reply \<rpc-reply\> message to be parsed.
 * @return Filled error structure according to the given rpc-reply, it is up to the
 * caller to free the structure using nc_err_free().
 */
struct nc_err* nc_err_parse(nc_reply* reply)
{
	xmlXPathObjectPtr result = NULL;
	xmlNodePtr node, subnode;
	int i;
	struct nc_err* e = NULL, *eaux = NULL;

	if (reply == NULL || reply->doc == NULL || reply->type.reply != NC_REPLY_ERROR) {
		return (NULL);
	}
	if (reply->error != NULL) {
		/* error structure is already created */
		return (reply->error);
	}

	/* find all <rpc-error>s */
	result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc-reply/"NC_NS_BASE10_ID":rpc-error", reply->ctxt);
	if (result != NULL) {
		for (i = 0; i < result->nodesetval->nodeNr; i++) {
			/* error structure is not yet created */
			eaux = nc_err_new(NC_ERR_EMPTY);

			/* parse the content of the message */
			for (node = result->nodesetval->nodeTab[i]->children;
			                node != NULL; node = node->next) {
				if (node->type != XML_ELEMENT_NODE || node->ns == NULL || strcmp(NC_NS_BASE10, (char*)(node->ns->href)) != 0) {
					continue;
				}
				if (xmlStrcmp(node->name, BAD_CAST "error-tag") == 0) {
					eaux->tag = (char*) xmlNodeGetContent(node);
				} else if (xmlStrcmp(node->name, BAD_CAST "error-type") == 0) {
					eaux->type = (char*) xmlNodeGetContent(node);
				} else if (xmlStrcmp(node->name, BAD_CAST "error-severity") == 0) {
					eaux->severity = (char*) xmlNodeGetContent(node);
				} else if (xmlStrcmp(node->name, BAD_CAST "error-app-tag") == 0) {
					eaux->apptag = (char*) xmlNodeGetContent(node);
				} else if (xmlStrcmp(node->name, BAD_CAST "error-path") == 0) {
					eaux->path = (char*) xmlNodeGetContent(node);
				} else if (xmlStrcmp(node->name, BAD_CAST "error-message") == 0) {
					eaux->message = (char*) xmlNodeGetContent(node);
				} else if (xmlStrcmp(node->name, BAD_CAST "error-info") == 0) {
					subnode = node->children;
					while (subnode != NULL) {
						if (subnode->type != XML_ELEMENT_NODE || subnode->ns == NULL || strcmp(NC_NS_BASE10, (char*)(subnode->ns->href)) != 0) {
							subnode = subnode->next;
							continue;
						}
						if (xmlStrcmp(subnode->name, BAD_CAST "bad-atribute") == 0) {
							eaux->attribute = (char*) xmlNodeGetContent(subnode);
						} else if (xmlStrcmp(subnode->name, BAD_CAST "bad-element") == 0 ||
								xmlStrcmp(subnode->name, BAD_CAST "ok-element") == 0 ||
								xmlStrcmp(subnode->name, BAD_CAST "err-element") == 0 ||
								xmlStrcmp(subnode->name, BAD_CAST "noop-element") == 0) {
							eaux->element = (char*) xmlNodeGetContent(subnode);
						} else if (xmlStrcmp(subnode->name, BAD_CAST "bad-namespace") == 0) {
							eaux->ns = (char*) xmlNodeGetContent(subnode);
						} else if (xmlStrcmp(subnode->name, BAD_CAST "session-id") == 0) {
							eaux->sid = (char*) xmlNodeGetContent(subnode);
						}
						subnode = subnode->next;
					}
				}
			}

			if (e != NULL) {
				/* concatenate multiple rpc-errors in repl-reply */
				eaux->next = e;
			}
			e = eaux;
		}
		xmlXPathFreeObject(result);
	} else {
		ERROR("No error information in the reply message to parse.");
		/* NULL, which is default e's value, will be returned */
	}

	/* store the result for the further use */
	//reply->error = nc_err_dup(e);
	reply->error = e;

	return (e);
}
