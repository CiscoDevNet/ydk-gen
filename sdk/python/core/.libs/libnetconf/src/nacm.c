/**
 * \file nacm.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of NETCONF Access Control Module defined in RFC 6536
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
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "session.h"
#include "messages.h"
#include "messages_xml.h"
#include "netconf_internal.h"
#include "nacm.h"
#include "datastore.h"
#include "datastore/datastore_internal.h"
#include "notifications.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

extern struct ncds_ds *nacm_ds; /* NACM datastore from datastore.c */
static int nacm_initiated = 0;

typedef enum {
	NACM_RULE_NOTSET = 0,
	NACM_RULE_OPERATION = 1,
	NACM_RULE_NOTIF = 2,
	NACM_RULE_DATA = 3
} NACM_RULE_TYPE;

struct nacm_group {
	char* name;
	char** users;
};

struct nacm_ns {
	char* prefix;
	char* href;
	struct nacm_ns *next;
};

struct nacm_path {
	char* path;
	struct nacm_ns* ns_list;
};

struct nacm_rule {
	char* module;
	NACM_RULE_TYPE type;
	/*
	 * data item contains:
	 * - rpc-name for NACM_RULE_OPERATION type
	 * - notification-name for NACM_RULE_NOTIF type
	 * - path for NACM_RULE_DATA type
	 */
	union {
		struct nacm_path* path;
		char** rpc_names;
		char** ntf_names;
	} type_data;
	uint8_t access; /* macros NACM_ACCESS_* */
	bool action; /* false (0) for permit, true (1) for deny */
};

struct rule_list {
	char** groups;
	struct nacm_rule** rules;
};

static struct nacm_config {
	bool enabled;
	bool default_read; /* false (0) for permit, true (1) for deny */
	bool default_write; /* false (0) for permit, true (1) for deny */
	bool default_exec; /* false (0) for permit, true (1) for deny */
	bool external_groups;
	struct nacm_group** groups;
	struct rule_list** rule_lists;
} nacm_config = {false, false, true, false, true, NULL, NULL};

/* access to the NACM statistics */
extern struct nc_shared_info *nc_info;

static int nacm_config_refresh(void);

static void nacm_path_free(struct nacm_path* path)
{
	struct nacm_ns* aux;

	if (path != NULL) {
		free(path->path);
		for (aux = path->ns_list; aux!= NULL; aux = path->ns_list) {
			path->ns_list = aux->next;
			free(aux->prefix);
			free(aux->href);
			free(aux);
		}
		free(path);
	}
}

static struct nacm_path* nacm_path_parse(xmlNodePtr node)
{
	xmlNsPtr *ns;
	char *s = NULL;
	struct nacm_path* retval;
	struct nacm_ns *path_ns;
	int i;

	if (node == NULL) {
		return (NULL);
	}

	retval = malloc(sizeof(struct nacm_path));
	if (retval == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	retval->ns_list = NULL;
	if ((retval->path = nc_clrwspace((char*)node->children->content)) == NULL) {
		free(retval);
		return (NULL);
	}
	ns = xmlGetNsList(node->doc, node);

	for(i = 0; ns != NULL && ns[i] != NULL; i++) {
		/* \todo process somehow also default namespace */
		if (ns[i]->prefix != NULL) {
			if (asprintf(&s, "/%s:", ns[i]->prefix) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				nacm_path_free(retval);
				free(ns);
				return (NULL);
			}
			if (strstr(retval->path, s) != NULL) {
				/* namespace used in path */
				path_ns = malloc(sizeof(struct nacm_ns));
				if (path_ns == NULL) {
					ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
					nacm_path_free(retval);
					free(ns);
					return (NULL);
				}
				path_ns->prefix = strdup((char*)(ns[i]->prefix));
				path_ns->href = strdup((char*)(ns[i]->href));
				path_ns->next = retval->ns_list;
				retval->ns_list = path_ns;
			}
			free(s);
			s = NULL;
		}
	}

	free(ns);
	return (retval);
}

static struct nacm_path* nacm_path_dup(struct nacm_path* orig)
{
	struct nacm_path *new;
	struct nacm_ns *ns, *ns_new;

	if (orig == NULL || orig->path == NULL) {
		return (NULL);
	}

	new = malloc(sizeof(struct nacm_path));
	if (new == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	new->path = strdup(orig->path);
	new->ns_list = NULL;

	for(ns = orig->ns_list; ns != NULL; ns = ns->next) {
		ns_new = malloc(sizeof(struct nacm_ns));
		if (ns_new == NULL) {
			ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
			nacm_path_free(new);
			return (NULL);
		}
		ns_new->prefix = strdup(ns->prefix);
		ns_new->href = strdup(ns->href);
		ns_new->next = new->ns_list;
		new->ns_list = ns_new;
	}

	return (new);
}

static void nacm_group_free(struct nacm_group* g)
{
	char* s;
	int i;

	if (g != NULL) {
		free(g->name);
		if (g->users != NULL) {
			for (i = 0, s = g->users[i]; s != NULL; i++, s = g->users[i]) {
				free(s);
			}
			free(g->users);
		}
		free(g);
	}
}

static void nacm_rule_free(struct nacm_rule* r)
{
	int i;

	if (r != NULL) {
		if (r->type == NACM_RULE_NOTIF) {
			if (r->type_data.ntf_names != NULL) {
				for(i = 0; r->type_data.ntf_names[i] != NULL; i++) {
					free(r->type_data.ntf_names[i]);
				}
				free(r->type_data.ntf_names);
			}
		} else if (r->type == NACM_RULE_OPERATION) {
			if (r->type_data.rpc_names != NULL) {
				for(i = 0; r->type_data.rpc_names[i] != NULL; i++) {
					free(r->type_data.rpc_names[i]);
				}
				free(r->type_data.rpc_names);
			}
		} else if (r->type == NACM_RULE_DATA) {
			nacm_path_free(r->type_data.path);
		}
		free(r->module);
		free(r);
	}
}

void nacm_rule_list_free(struct rule_list* rl)
{
	int i;

	if (rl != NULL) {
		if (rl->groups != NULL) {
			for(i = 0; rl->groups[i] != NULL; i++) {
				free(rl->groups[i]);
			}
			free(rl->groups);
		}
		if (rl->rules != NULL) {
			for(i = 0; rl->rules[i] != NULL; i++) {
				nacm_rule_free(rl->rules[i]);
			}
			free(rl->rules);
		}
		free(rl);
	}
}

static struct rule_list* nacm_rule_list_dup(struct rule_list* r)
{
	struct rule_list *new = NULL;
	int i, j;

	if (r != NULL) {
		new = malloc(sizeof(struct rule_list));
		if (new != NULL) {
			if (r->groups != NULL) {
				for(i = 0; r->groups[i] != NULL; i++);
				new->groups = malloc((i+1) * sizeof(char*));
				if (new->groups == NULL) {
					free(new);
					return (NULL);
				}
				for(i = 0; r->groups[i] != NULL; i++) {
					new->groups[i] = strdup(r->groups[i]);
				}
				new->groups[i] = NULL;
			} else {
				new->groups = NULL;
			}

			if (r->rules != NULL) {
				for(i = 0; r->rules[i] != NULL; i++);
				new->rules = malloc((i+1) * sizeof(char*));
				if (new->rules == NULL) {
					nacm_rule_list_free(new);
					return (NULL);
				}
				for(i = 0; r->rules[i] != NULL; i++) {
					new->rules[i] = malloc(sizeof(struct nacm_rule));
					if (new->rules[i] == NULL) {
						nacm_rule_list_free(new);
						return (NULL);
					}
					new->rules[i]->type = r->rules[i]->type;
					switch(new->rules[i]->type) {
					case NACM_RULE_NOTIF:
						for (j = 0; r->rules[i]->type_data.ntf_names != NULL && r->rules[i]->type_data.ntf_names[j] != NULL; j++) ;
						if (j > 0) {
							new->rules[i]->type_data.ntf_names = malloc((j + 1) * sizeof(char*));
							if (new->rules[i]->type_data.ntf_names == NULL) {
								nacm_rule_list_free(new);
								return (NULL);
							}
							for (j = 0; r->rules[i]->type_data.ntf_names[j] != NULL; j++) {
								new->rules[i]->type_data.ntf_names[j] = strdup(r->rules[i]->type_data.ntf_names[j]);
							}
							new->rules[i]->type_data.ntf_names[j] = NULL; /* list terminating NULL byte */
						}
						break;
					case NACM_RULE_OPERATION:
						for (j = 0; r->rules[i]->type_data.rpc_names != NULL && r->rules[i]->type_data.rpc_names[j] != NULL; j++);
						if (j > 0) {
							new->rules[i]->type_data.rpc_names = malloc((j + 1) * sizeof(char*));
							if (new->rules[i]->type_data.rpc_names == NULL) {
								nacm_rule_list_free(new);
								return (NULL);
							}
							for (j = 0; r->rules[i]->type_data.rpc_names[j] != NULL; j++) {
								new->rules[i]->type_data.rpc_names[j] = strdup(r->rules[i]->type_data.rpc_names[j]);
							}
							new->rules[i]->type_data.rpc_names[j] = NULL; /* list terminating NULL byte */
						}
						break;
					case NACM_RULE_DATA:
						new->rules[i]->type_data.path = (r->rules[i]->type_data.path == NULL) ? NULL : nacm_path_dup(r->rules[i]->type_data.path);
						break;
					default:
						new->rules[i]->type_data.path = NULL; /* covers also type_data.rpc_names and type_data.ntf_names */
						break;
					}
					new->rules[i]->action = r->rules[i]->action;
					new->rules[i]->access = r->rules[i]->access;
					new->rules[i]->module = (r->rules[i]->module == NULL) ? NULL : strdup(r->rules[i]->module);
				}
				new->rules[i] = NULL;
			} else {
				new->rules = NULL;
			}
		}
	}

	return (new);
}

struct rule_list** nacm_rule_lists_dup(struct rule_list** list)
{
	int i;
	struct rule_list** new;

	if (list == NULL) {
		return (NULL);
	}

	for(i = 0; list[i] != NULL; i++);
	new = malloc((i+1) * sizeof(struct rule_list*));
	if (new == NULL) {
		return (NULL);
	}
	for(i = 0; list[i] != NULL; i++) {
		new[i] = nacm_rule_list_dup(list[i]);
		if (new[i] == NULL) {
			for(i--; i >= 0; i--) {
				nacm_rule_list_free(new[i]);
			}
			return (NULL);
		}
	}
	new[i] = NULL; /* list terminating NULL byte */

	return (new);
}

static struct nacm_rule* nacm_get_rule(xmlNodePtr rulenode)
{
	xmlNodePtr node;
	struct nacm_rule* rule;
	char *s, *s_orig, *t;
	char **new_strlist;
	int c, l;
	bool action = false;

	/* many checks for rule element are done in nacm_config_refresh() before calling this function */

	rule = malloc(sizeof(struct nacm_rule));
	if (rule == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	rule->type = NACM_RULE_NOTSET;
	rule->type_data.path = NULL; /* also sets rpc_names and ntf_names to NULL */
	rule->module = NULL;
	rule->access = 0;

	for (node = rulenode->children; node != NULL; node = node->next) {
		if (node->type == XML_ELEMENT_NODE && node->ns != NULL && xmlStrcmp(node->ns->href, BAD_CAST NC_NS_NACM) == 0 &&
				node->children != NULL && node->children->type == XML_TEXT_NODE) {
			if (xmlStrcmp(node->name, BAD_CAST "module-name") == 0) {
				rule->module = nc_clrwspace((char*)node->children->content);
			} else if (xmlStrcmp(node->name, BAD_CAST "rpc-name") == 0) {
				if (rule->type != 0) {
					ERROR("%s: invalid rule definition (multiple cases from rule-type choice)", __func__);
					nacm_rule_free(rule);
					return (NULL);
				}
				rule->type = NACM_RULE_OPERATION;
				s_orig = s = nc_clrwspace((char*) node->children->content);
				for (c = l = 0; (t = strsep(&s, " \n\t")) != NULL; ) {
					if (strisempty(t)) {
						/* empty string (double delimiter) */
						continue;
					}
					if (c == l) {
						l += 10;
						new_strlist = realloc(rule->type_data.rpc_names, l * sizeof(char*));
						if (new_strlist == NULL) {
							ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
							nacm_rule_free(rule);
							free(s_orig);
							return (NULL);
						}
						rule->type_data.rpc_names = new_strlist;
					}
					rule->type_data.rpc_names[c++] = strdup(t);
					rule->type_data.rpc_names[c] = NULL; /* list terminating NULL byte */
				}
				free(s_orig);
			} else if (xmlStrcmp(node->name, BAD_CAST "notification-name") == 0) {
				if (rule->type != 0) {
					ERROR("%s: invalid rule definition (multiple cases from rule-type choice)", __func__);
					nacm_rule_free(rule);
					return (NULL);
				}
				rule->type = NACM_RULE_NOTIF;
				s_orig = s = nc_clrwspace((char*) node->children->content);
				for (c = l = 0; (t = strsep(&s, " \n\t")) != NULL; ) {
					if (strisempty(t)) {
						/* empty string (double delimiter) */
						continue;
					}
					if (c == l) {
						l += 10;
						new_strlist = realloc(rule->type_data.ntf_names, l * sizeof(char*));
						if (new_strlist == NULL) {
							ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
							nacm_rule_free(rule);
							free(s_orig);
							return (NULL);
						}
						rule->type_data.ntf_names = new_strlist;
					}
					rule->type_data.ntf_names[c++] = strdup(t);
					rule->type_data.ntf_names[c] = NULL; /* list terminating NULL byte */
				}
				free(s_orig);
			} else if (xmlStrcmp(node->name, BAD_CAST "path") == 0) {
				if (rule->type != 0) {
					ERROR("%s: invalid rule definition (multiple cases from rule-type choice)", __func__);
					nacm_rule_free(rule);
					return (NULL);
				}
				rule->type = NACM_RULE_DATA;
				rule->type_data.path = nacm_path_parse(node);
			} else if (xmlStrcmp(node->name, BAD_CAST "access-operations") == 0) {
				if (xmlStrstr(node->children->content, BAD_CAST "*")) {
					rule->access = NACM_ACCESS_ALL;
				} else {
					if (xmlStrstr(node->children->content, BAD_CAST "create")) {
						rule->access |= NACM_ACCESS_CREATE;
					} else if (xmlStrstr(node->children->content, BAD_CAST "read")) {
						rule->access |= NACM_ACCESS_READ;
					} else if (xmlStrstr(node->children->content, BAD_CAST "update")) {
						rule->access |= NACM_ACCESS_UPDATE;
					} else if (xmlStrstr(node->children->content, BAD_CAST "delete")) {
						rule->access |= NACM_ACCESS_DELETE;
					} else if (xmlStrstr(node->children->content, BAD_CAST "exec")) {
						rule->access |= NACM_ACCESS_EXEC;
					}
				}
			} else if (xmlStrcmp(node->name, BAD_CAST "action") == 0) {
				action = true;
				s = nc_clrwspace((char*) node->children->content);
				if (strcmp(s, "permit") == 0) {
					rule->action = NACM_PERMIT;
				} else if (strcmp(s, "deny") == 0) {
					rule->action = NACM_DENY;
				} else {
					ERROR("%s: Invalid /nacm/rule-list/rule/action value (%s).", __func__, s);
					nacm_rule_free(rule);
					free(s);
					return (NULL);
				}
				free(s);
			}
		}
	}

	if (!action || rule->access == 0) {
		WARN("%s: Invalid /nacm/rule-list/rule - missing some mandatory elements, skipping the rule.", __func__);
		nacm_rule_free(rule);
		return (NULL);
	}

	return (rule);
}

int nacm_init(void)
{
	if (nacm_initiated == 1) {
		return (EXIT_FAILURE);
	}

	nacm_initiated = 1;

	if (nacm_config_refresh() != EXIT_SUCCESS) {
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

void nacm_close(void)
{
	int i;

	if (nacm_initiated == 0) {
		return;
	}

	if (nacm_config.groups != NULL) {
		for (i = 0; nacm_config.groups[i] != NULL; i++) {
			nacm_group_free(nacm_config.groups[i]);
		}
		free(nacm_config.groups);
		nacm_config.groups = NULL;
	}
	if (nacm_config.rule_lists != NULL) {
		for (i = 0; nacm_config.rule_lists[i] != NULL; i++) {
			nacm_rule_list_free(nacm_config.rule_lists[i]);
		}
		free(nacm_config.rule_lists);
		nacm_config.rule_lists = NULL;
	}
	nacm_initiated = 0;
}

static int check_query_result(xmlXPathObjectPtr query_result, const char* object, int multiple, int textnode)
{
	if (query_result != NULL) {
		if (xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			ERROR("%s: No %s value in configuration data.", __func__, object);
			return (EXIT_FAILURE);
		} else if (!multiple && query_result->nodesetval->nodeNr > 1) {
			ERROR("%s: Multiple %s values in configuration data.", __func__, object);
			return (EXIT_FAILURE);
		} else if (textnode && (query_result->nodesetval->nodeTab[0]->children == NULL || query_result->nodesetval->nodeTab[0]->children->type != XML_TEXT_NODE)) {
			ERROR("%s: Invalid %s object - missing content.", __func__, object);
			return (EXIT_FAILURE);
		}
	} else {
		ERROR("%s: Unable to get value of %s configuration data", __func__, object);
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

/**
 * @brief Refresh internal structures according to the NACM configuration data.
 * @return 0 on success, -1 on error
 */
static int nacm_config_refresh(void)
{
	xmlXPathContextPtr data_ctxt = NULL;
	xmlXPathObjectPtr query_result = NULL;
	char* data;
	char** new_strlist;
	xmlNodePtr node;
	xmlChar* content = NULL;
	xmlDocPtr data_doc = NULL;
	int i, j, gl, rl, gc, rc;
	bool allgroups;
	struct nacm_group* gr;
	struct rule_list* rlist;
	struct nacm_rule** new_rules;
	struct nc_err *e = NULL;

	if (nacm_initiated == 0) {
		ERROR("%s: NACM Subsystem not initialized.", __func__);
		return (EXIT_FAILURE);
	}

	if (nacm_ds == NULL) {
		ERROR("%s: NACM internal datastore not initialized.", __func__);
		return (EXIT_FAILURE);
	}

	/* check if NACM  datastore was modified */
	if (nacm_ds->func.was_changed(nacm_ds) == 0) {
		/* it wasn't, we have up to date configuration data */
		return (EXIT_SUCCESS);
	}

	data = nacm_ds->func.getconfig(nacm_ds, NULL, NC_DATASTORE_RUNNING, &e);
	nc_err_free(e);
	if (data == NULL) {
		ERROR("%s: getting NACM configuration data from the datastore failed.", __func__);
		return (EXIT_FAILURE);
	}
	if (strcmp(data, "") == 0) {
		data_doc = xmlNewDoc(BAD_CAST "1.0");
	} else {
		data_doc = xmlReadDoc(BAD_CAST data, NULL, NULL, NC_XMLREAD_OPTIONS);
	}
	free(data);

	if (data_doc == NULL) {
		ERROR("%s: Reading configuration datastore failed.", __func__);
		return (EXIT_FAILURE);
	}

	/* process default values */
	ncdflt_default_values(data_doc, nacm_ds->ext_model, NCWD_MODE_ALL);

	/* create xpath evaluation context */
	if ((data_ctxt = xmlXPathNewContext(data_doc)) == NULL) {
		ERROR("%s: NACM configuration data XPath context can not be created.", __func__);
		goto errorcleanup;
	}
	/* register NACM namespace for the rpc */
	if (xmlXPathRegisterNs(data_ctxt, BAD_CAST NC_NS_NACM_ID, BAD_CAST NC_NS_NACM) != 0) {
		ERROR("Registering base namespace for the message xpath context failed.");
		goto errorcleanup;
	}

	/* fill the structure */
	/* /nacm/enable-nacm */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NACM_ID":nacm/"NC_NS_NACM_ID":enable-nacm", data_ctxt);
	if (check_query_result(query_result, "/nacm/enable-nacm", 0, 1) != 0) {
		goto errorcleanup;
	}
	content = (xmlChar*) nc_clrwspace((char*)query_result->nodesetval->nodeTab[0]->children->content);
	if (xmlStrcmp(content, BAD_CAST "true") == 0) {
		nacm_config.enabled = true;
	} else if (xmlStrcmp(BAD_CAST content, BAD_CAST "false") == 0) {
		nacm_config.enabled = false;
	} else {
		ERROR("%s: Invalid /nacm/enable-nacm value (%s).", __func__, content);
		goto errorcleanup;
	}
	xmlFree(content);
	content = NULL;
	xmlXPathFreeObject(query_result);

	/* /nacm/read-default */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NACM_ID":nacm/"NC_NS_NACM_ID":read-default", data_ctxt);
	if (check_query_result(query_result, "/nacm/read-default", 0, 1) != 0) {
		goto errorcleanup;
	}
	content = (xmlChar*) nc_clrwspace((char*)query_result->nodesetval->nodeTab[0]->children->content);
	if (xmlStrcmp(content, BAD_CAST "permit") == 0) {
		nacm_config.default_read = NACM_PERMIT;
	} else if (xmlStrcmp(BAD_CAST content, BAD_CAST "deny") == 0) {
		nacm_config.default_read = NACM_DENY;
	} else {
		ERROR("%s: Invalid /nacm/read-default value (%s).", __func__, content);
		goto errorcleanup;
	}
	xmlFree(content);
	content = NULL;
	xmlXPathFreeObject(query_result);

	/* /nacm/write-default */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NACM_ID":nacm/"NC_NS_NACM_ID":write-default", data_ctxt);
	if (check_query_result(query_result, "/nacm/write-default", 0 ,1) != 0) {
		goto errorcleanup;
	}
	content = (xmlChar*) nc_clrwspace((char*)query_result->nodesetval->nodeTab[0]->children->content);
	if (xmlStrcmp(content, BAD_CAST "permit") == 0) {
		nacm_config.default_write = NACM_PERMIT;
	} else if (xmlStrcmp(BAD_CAST content, BAD_CAST "deny") == 0) {
		nacm_config.default_write = NACM_DENY;
	} else {
		ERROR("%s: Invalid /nacm/write-default value (%s).", __func__, content);
		goto errorcleanup;
	}
	xmlFree(content);
	content = NULL;
	xmlXPathFreeObject(query_result);

	/* /nacm/exec-default */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NACM_ID":nacm/"NC_NS_NACM_ID":exec-default", data_ctxt);
	if (check_query_result(query_result, "/nacm/exec-default", 0, 1) != 0) {
		goto errorcleanup;
	}
	content = (xmlChar*) nc_clrwspace((char*)query_result->nodesetval->nodeTab[0]->children->content);
	if (xmlStrcmp(content, BAD_CAST "permit") == 0) {
		nacm_config.default_exec = NACM_PERMIT;
	} else if (xmlStrcmp(BAD_CAST content, BAD_CAST "deny") == 0) {
		nacm_config.default_exec = NACM_DENY;
	} else {
		ERROR("%s: Invalid /nacm/exec-default value (%s).", __func__, content);
		goto errorcleanup;
	}
	xmlFree(content);
	content = NULL;
	xmlXPathFreeObject(query_result);

	/* /nacm/enable-external-groups */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NACM_ID":nacm/"NC_NS_NACM_ID":enable-external-groups", data_ctxt);
	if (check_query_result(query_result, "/nacm/enable-external-groups", 0, 1) != 0) {
		goto errorcleanup;
	}
	content = (xmlChar*) nc_clrwspace((char*)query_result->nodesetval->nodeTab[0]->children->content);
	if (xmlStrcmp(content, BAD_CAST "true") == 0) {
		nacm_config.external_groups = true;
	} else if (xmlStrcmp(BAD_CAST content, BAD_CAST "false") == 0) {
		nacm_config.external_groups = false;
	} else {
		ERROR("%s: Invalid /nacm/enable-external-groups value (%s).", __func__, content);
		goto errorcleanup;
	}
	xmlFree(content);
	content = NULL;
	xmlXPathFreeObject(query_result);

	/* /nacm/groups/group */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NACM_ID":nacm/"NC_NS_NACM_ID":groups/"NC_NS_NACM_ID":group", data_ctxt);
	if (query_result != NULL) {
		/* free previously parsed list of groups if any */
		if (nacm_config.groups != NULL) {
			for (i = 0; nacm_config.groups[i] != NULL; i++) {
				nacm_group_free(nacm_config.groups[i]);
			}
			free(nacm_config.groups);
			nacm_config.groups = NULL;
		}
		/* parse the currently set groups */
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			nacm_config.groups = malloc((query_result->nodesetval->nodeNr + 1) * sizeof(struct nacm_group*));
			if (nacm_config.groups == NULL) {
				ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
				goto errorcleanup;
			}
			nacm_config.groups[0] = NULL; /* list terminating NULL byte */
			for (i = j = 0; i < query_result->nodesetval->nodeNr; i++) {
				gr = malloc(sizeof(struct nacm_group));
				if (gr == NULL) {
					ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
					goto errorcleanup;
				}
				gl = gc = 0;
				gr->users = NULL;
				gr->name = NULL;
				for (node = query_result->nodesetval->nodeTab[i]->children; node != NULL; node = node->next) {
					if (node->type == XML_ELEMENT_NODE && node->ns != NULL && xmlStrcmp(node->ns->href, BAD_CAST NC_NS_NACM) == 0 &&
							node->children != NULL && node->children->type == XML_TEXT_NODE) {
						if (xmlStrcmp(node->name, BAD_CAST "name") == 0) {
							gr->name = nc_clrwspace((char*)node->children->content);
						} else if (xmlStrcmp(node->name, BAD_CAST "user-name") == 0) {
							if (gc == gl) {
								gl += 10;
								new_strlist = realloc(gr->users, gl * sizeof(char*));
								if (new_strlist == NULL) {
									ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
									goto errorcleanup;
								}
								gr->users = new_strlist;
							}
							gr->users[gc] = nc_clrwspace((char*)node->children->content);
							if (gr->users[gc] != NULL) {
								gc++;
								gr->users[gc] = NULL; /* list terminating NULL */
							}
						}
					}
				}
				if (gr->name == NULL || gr->users == NULL) {
					nacm_group_free(gr);
				} else {
					nacm_config.groups[j++] = gr;
					nacm_config.groups[j] = NULL; /* list terminating NULL */
				}
			}
		}
		xmlXPathFreeObject(query_result);
	} else {
		ERROR("%s: Unable to get information about NACM groups", __func__);
		return (EXIT_FAILURE);
	}

	/* /nacm/rule-list */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NACM_ID":nacm/"NC_NS_NACM_ID":rule-list", data_ctxt);
	if (query_result != NULL) {
		/* free previously parsed list of rule-lists if any */
		if (nacm_config.rule_lists != NULL) {
			for (i = 0; nacm_config.rule_lists[i] != NULL; i++) {
				nacm_rule_list_free(nacm_config.rule_lists[i]);
			}
			free(nacm_config.rule_lists);
			nacm_config.rule_lists = NULL;
		}
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			nacm_config.rule_lists = malloc((query_result->nodesetval->nodeNr + 1) * sizeof(struct rule_list*));
			if (nacm_config.rule_lists == NULL) {
				ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
				goto errorcleanup;
			}
			nacm_config.rule_lists[0] = NULL; /* list terminating NULL byte */
			for (i = j = 0; i < query_result->nodesetval->nodeNr; i++) {
				rlist = malloc(sizeof(struct rule_list));
				if (rlist == NULL) {
					ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
					goto errorcleanup;
				}
				rl = rc = gl = gc = 0;
				rlist->rules = NULL;
				rlist->groups = NULL;
				allgroups = false;
				for (node = query_result->nodesetval->nodeTab[i]->children; node != NULL; node = node->next) {
					if (node->type == XML_ELEMENT_NODE && node->ns != NULL && xmlStrcmp(node->ns->href, BAD_CAST NC_NS_NACM) == 0) {
						if (!allgroups && node->children != NULL && node->children->type == XML_TEXT_NODE && xmlStrcmp(node->name, BAD_CAST "group") == 0) {
							if (gc == gl) {
								gl += 10;
								new_strlist = realloc(rlist->groups, gl * sizeof(char*));
								if (new_strlist == NULL) {
									ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
									goto errorcleanup;
								}
								rlist->groups = new_strlist;
							}
							rlist->groups[gc] = nc_clrwspace((char*) node->children->content);
							if (rlist->groups[gc] != NULL) {
								gc++;
								rlist->groups[gc] = NULL; /* list terminating NULL */

								if (strcmp(rlist->groups[gc-1], "*") == 0) {
									/* matchall string - store only single value and free the rest of group list (if any) */
									for (gc = 0; rlist->groups[gc] != NULL; gc++) {
										free(rlist->groups[gc]);
									}
									rlist->groups[0] = strdup("*");
									/* ignore rest of group elements in this rule-list */
									allgroups = true;
								}
							}
						} else if (node->children != NULL && xmlStrcmp(node->name, BAD_CAST "rule") == 0) {
							if (rc == rl) {
								rl += 10;
								new_rules = realloc(rlist->rules, rl * sizeof(struct nacm_rule*));
								if (new_rules == NULL) {
									ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
									goto errorcleanup;
								}
								rlist->rules = new_rules;
							}
							rlist->rules[rc] = nacm_get_rule(node);
							if (rlist->rules[rc] != NULL) {
								rc++;
								rlist->rules[rc] = NULL; /* list terminating NULL */
							}
						}
					}
				}
				if (rlist->groups == NULL || rlist->rules == NULL) {
					nacm_rule_list_free(rlist);
				} else {
					nacm_config.rule_lists[j++] = rlist;
					nacm_config.rule_lists[j] = NULL; /* list terminating NULL */
				}
			}
		}
		xmlXPathFreeObject(query_result);
	} else {
		ERROR("%s: Unable to get information about NACM's lists of rules", __func__);
		return (EXIT_FAILURE);
	}

	xmlXPathFreeContext(data_ctxt);
	xmlFreeDoc(data_doc);

	return (EXIT_SUCCESS);

errorcleanup:

	xmlXPathFreeObject(query_result);
	xmlXPathFreeContext(data_ctxt);
	xmlFreeDoc(data_doc);
	xmlFree(content);

	return (EXIT_FAILURE);
}

static struct nacm_rpc* nacm_rpc_struct(const struct nc_session* session)
{
	struct nacm_rpc* nacm_rpc;
	struct rule_list** new_rulelist;
	char** groups = NULL, **new_groups;
	int l, c, i, j, k;

	if (session == NULL || (session->status != NC_SESSION_STATUS_WORKING && session->status != NC_SESSION_STATUS_DUMMY)) {
		ERROR("%s: invalid session to use", __func__);
		return (NULL);
	}

	nacm_rpc = malloc(sizeof(struct nacm_rpc));
	if (nacm_rpc == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	nacm_rpc->default_exec = nacm_config.default_exec;
	nacm_rpc->default_read = nacm_config.default_read;
	nacm_rpc->default_write = nacm_config.default_write;
	nacm_rpc->rule_lists = NULL;

	l = c = 0;
	/* get list of user's groups specified in NACM configuration */
	for (i = 0; nacm_config.groups != NULL && nacm_config.groups[i] != NULL; i++) {
		for (j = 0; nacm_config.groups[i]->users != NULL && nacm_config.groups[i]->users[j] != NULL; j++) {
			if (strcmp(nacm_config.groups[i]->users[j], session->username) == 0) {
				if (c+1 >= l) {
					l += 10;
					new_groups = realloc(groups, l * sizeof(char*));
					if (new_groups == NULL) {
						ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
						free(nacm_rpc);
						return (NULL);
					}
					groups = new_groups;
				}
				groups[c] = strdup(nacm_config.groups[i]->name);
				c++;
			}
		}
	}
	/* if enabled, add a list of system groups for the user */
	if (nacm_config.external_groups == true && session->groups != NULL) {
		for (i = 0; session->groups[i] != NULL; i++) {
			if (c+1 >= l) {
				l += 10;
				new_groups = realloc(groups, l * sizeof(char*));
				if (new_groups == NULL) {
					ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
					free(nacm_rpc);
					return (NULL);
				}
				groups = new_groups;
			}
			groups[c] = strdup(session->groups[i]);
			c++;
		}
	}
	if (groups != NULL) {
		groups[c] = NULL; /* list terminating NULL */

		l = c = 0;
		/* select rules for the groups associated with the user */
		for (i = 0; nacm_config.rule_lists != NULL && nacm_config.rule_lists[i] != NULL; i++) {
			for (j = 0; nacm_config.rule_lists[i]->groups != NULL && nacm_config.rule_lists[i]->groups[j] != NULL; j++) {
				for (k = 0; groups[k] != NULL; k++) {
					if (strcmp(nacm_config.rule_lists[i]->groups[j], groups[k]) == 0 ||
							strcmp(nacm_config.rule_lists[i]->groups[j], "*") == 0) {
						break;
					}
				}
				if (groups[k] != NULL) {
					/* we have found matching groups - add rule list to the rpc */
					if (c+1 >= l) {
						l += 10;
						new_rulelist = realloc(nacm_rpc->rule_lists, l * sizeof(struct rule_list*));
						if (new_rulelist == NULL) {
							ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
							for(k = 0; groups[k] != NULL; k++) {
								free(groups[k]);
							}
							free(groups);
							free(nacm_rpc);
							return (NULL);
						}
						nacm_rpc->rule_lists = new_rulelist;
					}
					nacm_rpc->rule_lists[c] = nacm_rule_list_dup(nacm_config.rule_lists[i]);
					if (nacm_rpc->rule_lists[c] != NULL) {
						c++;
						nacm_rpc->rule_lists[c] = NULL;  /* list terminating NULL */
					}
					break;
				}
			}
		}

		/* free groups */
		for(k = 0; groups[k] != NULL; k++) {
			free(groups[k]);
		}
		free(groups);
	}

	return (nacm_rpc);
}

int nacm_start(nc_rpc* rpc, const struct nc_session* session)
{
	if (rpc == NULL || session == NULL) {
		return (EXIT_FAILURE);
	}

	/*
	 * if NACM structure will not be added into the RPC structure, NACM rules
	 * will not be applied and NACM will not take effect.
	 */

	if (session->nacm_recovery == 1 || nacm_initiated == 0) {
		/* NACM is not enabled or we are in recovery session (NACM is ignored) */
		return (EXIT_SUCCESS);
	}

	if (nc_rpc_get_op(rpc) == NC_OP_CLOSESESSION) {
		/* close-session is always permitted */
		return (EXIT_SUCCESS);
	}

	nacm_config_refresh();

	if (nacm_config.enabled == false) {
		/* NACM subsystem is switched off */
		return (EXIT_SUCCESS);
	}

	/* connect NACM structure with RPC */
	rpc->nacm = nacm_rpc_struct(session);

	return (EXIT_SUCCESS);
}

static void nacm_check_data_read_recursion(xmlNodePtr subtree, const struct nacm_rpc* nacm)
{
	xmlNodePtr node, next;

	if (nacm_check_data(subtree, NACM_ACCESS_READ, nacm) == NACM_DENY) {
		xmlUnlinkNode(subtree);
		xmlFreeNode(subtree);
	} else {
		for (node = subtree->children; node != NULL; node = next) {
			next = node->next;
			if (node->type == XML_ELEMENT_NODE) {
				nacm_check_data_read_recursion(node, nacm);
			}
		}
	}
}

int nacm_check_data_read(xmlDocPtr doc, const struct nacm_rpc* nacm)
{
	xmlNodePtr node, next;

	if (doc == NULL) {
		return (EXIT_FAILURE);
	}

	if (nacm == NULL) {
		return (EXIT_SUCCESS);
	}

	for (node = doc->children; node != NULL; node = next) {
		next = node->next;
		if (node->type == XML_ELEMENT_NODE) {
			nacm_check_data_read_recursion(node, nacm);
		}
	}

	return (EXIT_SUCCESS);
}

/*
 * return 0 as false (nodes are not equivalent), 1 as true (model_node defines
 * node in model)
 */
static int compare_node_to_model(const xmlNodePtr node, const xmlNodePtr model_node, const char* model_namespace)
{
	xmlChar* name;
	xmlNodePtr model_parent;

	/* \todo Add support for augment models */

	if ((name = xmlGetProp (model_node, BAD_CAST "name")) == NULL) {
		return (0);
	}

	if (xmlStrcmp(node->name, name) != 0) {
		xmlFree(name);
		return (0);
	}
	xmlFree(name);

	if (node->ns == NULL || node->ns->href == NULL ||
	    xmlStrcmp(node->ns->href, BAD_CAST model_namespace) != 0) {
		return (0);
	}

	/* check namespace */
	if (node->parent->type == XML_DOCUMENT_NODE) {
		/* we are in the root */
		if (xmlStrcmp(model_node->parent->name, BAD_CAST "module")) {
			/* also in model we are on the top */
			return (1);
		} else {
			/* there is something left in model */
			return (0);
		}
	} else {
		/* do recursion */
		model_parent = model_node->parent;
		while ((model_parent != NULL) && (model_parent->type == XML_ELEMENT_NODE) &&
		    (xmlStrcmp(model_parent->name, BAD_CAST "choice") == 0 ||
		    xmlStrcmp(model_parent->name, BAD_CAST "case") == 0 ||
		    xmlStrcmp(model_parent->name, BAD_CAST "augment") == 0)) {
			model_parent = model_parent->parent;
		}
		return (compare_node_to_model(node->parent, model_parent, model_namespace));
	}
}

int nacm_check_data(const xmlNodePtr node, const int access, const struct nacm_rpc* nacm)
{
	xmlXPathObjectPtr defdeny;
	xmlXPathContextPtr model_ctxt = NULL;
	xmlXPathContextPtr ctxt = NULL;
	xmlXPathObjectPtr xpath_result = NULL;
	struct nacm_ns *ns;
	struct nacm_rule* rule;
	const struct data_model* module;
	int i, j, k;
	int retval = -1;

	if (access == 0 || node == NULL || node->doc == NULL) {
		/* invalid input parameter */
		return (-1);
	}

	if (nacm == NULL) {
		/* NACM will not affect this request */
		return (NACM_PERMIT);
	}

	if (node->type != XML_ELEMENT_NODE) {
		/* skip comments or other elements not covered by NACM rules */
		return (NACM_PERMIT);
	}

	/* get module name where the data node is defined */
	module = ncds_get_model_data((node->ns != NULL) ? (char*)(node->ns->href) : NULL);

	if (module != NULL) {
		for (i = 0; nacm->rule_lists != NULL && nacm->rule_lists[i] != NULL; i++) {
			for (j = 0; nacm->rule_lists[i]->rules != NULL && nacm->rule_lists[i]->rules[j] != NULL; j++) {
				/*
				 * check rules (all must be met):
				 * - module-name matches "*" or the name of the module where the data node is defined
				 * - type is NACM_RULE_NOTSET or type is NACM_RULE_DATA and data contain "*" or the operation name
				 * - access has set NACM_ACCESS_EXEC bit
				 */
				rule = nacm->rule_lists[i]->rules[j]; /* shortcut */

				/* 1) module name */
				if (!(strcmp(rule->module, "*") == 0 ||
				    strcmp(rule->module, module->name) == 0)) {
					/* rule does not match */
					continue;
				}

				/* 3) access - do it before 2 for optimize, the 2nd step is the most difficult */
				if ((rule->access & access) == 0) {
					/* rule does not match */
					continue;
				}

				/* 2) type and operation name */
				if (rule->type != NACM_RULE_NOTSET) {
					if (rule->type == NACM_RULE_DATA &&
					    rule->type_data.path != NULL) {
						/* create xPath context for search in node's document */
						if ((ctxt = xmlXPathNewContext(node->doc)) == NULL) {
							ERROR("%s: Creating XPath context failed.", __func__);
							return (-1);
						}

						/* register namespaces from the rule's path */
						for (ns = rule->type_data.path->ns_list; ns != NULL; ns = ns->next) {
							if (xmlXPathRegisterNs(ctxt, BAD_CAST ns->prefix, BAD_CAST ns->href) != 0) {
								ERROR("Registering NACM rule path namespace for the xpath context failed.");
								xmlXPathFreeContext(ctxt);
								return (-1);
							}
						}

						/* query the rule's path in the node's document and compare results with the node */
						if ((xpath_result = xmlXPathEvalExpression(BAD_CAST rule->type_data.path->path, ctxt)) != NULL) {
							if (xmlXPathNodeSetIsEmpty(xpath_result->nodesetval)) {
								/* rule does not match - path does not exist in document */
								xmlXPathFreeObject(xpath_result);
								xmlXPathFreeContext(ctxt);
								continue;
							}
							for (k = 0; k < xpath_result->nodesetval->nodeNr; k++) {
								if (node == xpath_result->nodesetval->nodeTab[k]) {
									/* the path selects the node */
									break;
								}
							}

							if (k == xpath_result->nodesetval->nodeNr) {
								/* rule does not match */
								xmlXPathFreeObject(xpath_result);
								xmlXPathFreeContext(ctxt);
								continue;
							}

							xmlXPathFreeObject(xpath_result);
						} else {
							WARN("%s: Unable to evaluate path \"%s\"", __func__, rule->type_data.path->path);
						}

						/* cleanup */
						xmlXPathFreeContext(ctxt);
					} else {
						/* rule does not match - another type of rule */
						continue;
					}
				}

				/* rule matches */
				retval = rule->action;
				goto result;
			}
		}
		/* no matching rule found */

		/* check nacm:default-deny-all and nacm:default-deny-write */
		if ((model_ctxt = xmlXPathNewContext(module->xml)) != NULL &&
		    xmlXPathRegisterNs(model_ctxt, BAD_CAST "yin", BAD_CAST NC_NS_YIN) == 0 &&
		    xmlXPathRegisterNs(model_ctxt, BAD_CAST "nacm", BAD_CAST NC_NS_NACM) == 0) {
			if ((defdeny = xmlXPathEvalExpression(BAD_CAST "/yin:module//nacm:default-deny-all", model_ctxt)) != NULL) {
				if (!xmlXPathNodeSetIsEmpty(defdeny->nodesetval)) {
					/* process all default-deny-all elements */
					for (i = 0; i < defdeny->nodesetval->nodeNr; i++) {
						if (compare_node_to_model(node, defdeny->nodesetval->nodeTab[i]->parent, module->ns) == 1) {
							xmlXPathFreeObject(defdeny);
							xmlXPathFreeContext(model_ctxt);
							retval = NACM_DENY;
							goto result;
						}
					}
				}
				xmlXPathFreeObject(defdeny);
			}
			if ((access & (NACM_ACCESS_CREATE | NACM_ACCESS_DELETE | NACM_ACCESS_UPDATE)) != 0) {
				/* check default-deny-write */
				if ((defdeny = xmlXPathEvalExpression(BAD_CAST "/yin:module//nacm:default-deny-write", model_ctxt)) != NULL ) {
					if (!xmlXPathNodeSetIsEmpty(defdeny->nodesetval)) {
						/* process all default-deny-all elements */
						for (i = 0; i < defdeny->nodesetval->nodeNr; i++) {
							if (compare_node_to_model(node, defdeny->nodesetval->nodeTab[i]->parent, module->ns) == 1) {
								xmlXPathFreeObject(defdeny);
								xmlXPathFreeContext(model_ctxt);
								retval = NACM_DENY;
								goto result;
							}
						}
					}
					xmlXPathFreeObject(defdeny);
				}
			}
		}
		xmlXPathFreeContext(model_ctxt);
	}
	/* no matching rule found */

	/* default action */
	if ((access & NACM_ACCESS_READ) != 0) {
		retval = nacm->default_read;
		goto result;
	}
	if ((access & (NACM_ACCESS_CREATE | NACM_ACCESS_DELETE | NACM_ACCESS_UPDATE)) != 0) {
		retval = nacm->default_write;
		goto result;
	}

	/* unknown access request - deny */
	retval = NACM_DENY;

result:
	/* update stats */
	if (retval == NACM_DENY && nc_info) {
		pthread_rwlock_wrlock(&(nc_info->lock));
		nc_info->stats_nacm.denied_data++;
		pthread_rwlock_unlock(&(nc_info->lock));
	}

	return (retval);
}

#ifndef DISABLE_NOTIFICATIONS

int nacm_check_notification(const nc_ntf* ntf, const struct nc_session* session)
{
	xmlXPathObjectPtr defdeny;
	xmlXPathContextPtr model_ctxt = NULL;
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr ntfnode;
	const struct data_model* ntfmodule;
	struct nacm_rpc *nacm;
	int i, j, k;
	int retval;
	NCNTF_EVENT event;

	if (ntf == NULL || session == NULL) {
		/* invalid input parameter */
		return (-1);
	}

	/* recovery session */
	if (session->nacm_recovery) {
		/* ignore NACM in recovery session */
		return (NACM_PERMIT);
	}

	if (nacm_initiated == 0 || nacm_config.enabled == false) {
		/* NACM subsystem not initiated or switched off */
		/*
		 * do not add NACM structure to the RPC, which means that
		 * NACM is not applied to the RPC
		 */
		return (NACM_PERMIT);
	}

	nacm_config_refresh();

	/* connect NACM structure with RPC */
	nacm = nacm_rpc_struct(session);

	if (nacm == NULL) {
		/* NACM will not affect this notification */
		retval = NACM_PERMIT;
		goto nacmfree;
	}

	event = ncntf_notif_get_type(ntf);
	if (event == NCNTF_REPLAY_COMPLETE || event == NCNTF_NTF_COMPLETE) {
		/* NACM will not affect this notification */
		retval = NACM_PERMIT;
		goto nacmfree;
	}

	/* get the notification name from the message */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_NOTIFICATIONS_ID":notification", ntf->ctxt);
	if (check_query_result(query_result, "/notification", 0, 0) != 0) {
		retval = -1;
		goto nacmfree;
	}
	ntfnode = query_result->nodesetval->nodeTab[0]->children;
	while(ntfnode != NULL) {
		if (ntfnode->type != XML_ELEMENT_NODE || xmlStrcmp(ntfnode->name, BAD_CAST "eventTime") == 0) {
			/* skip comments and eventTime element */
			ntfnode = ntfnode->next;
			continue;
		}
		break;
	}
	xmlXPathFreeObject(query_result);
	query_result = NULL;

	if (ntfnode == NULL) {
		/* invalid message with missing notification */
		retval = -1;
		goto nacmfree;
	}

	/* get module name where the notification is defined */
	ntfmodule = ncds_get_model_notification((char*)(ntfnode->name), (ntfnode->ns != NULL) ? (char*)(ntfnode->ns->href) : NULL);

	if (ntfmodule != NULL) {
		for (i = 0; nacm->rule_lists != NULL && nacm->rule_lists[i] != NULL; i++) {
			for (j = 0; nacm->rule_lists[i]->rules != NULL && nacm->rule_lists[i]->rules[j] != NULL; j++) {
				/*
				 * check rules (all must be met):
				 * - module-name matches "*" or the name of the module where the operation is defined
				 * - type is NACM_RULE_NOTSET or type is NACM_RULE_NOTIF and data contain "*" or the notification name
				 * - access has set NACM_ACCESS_READ bit
				 */

				/* 1) module name */
				if (!(strcmp(nacm->rule_lists[i]->rules[j]->module, "*") == 0 ||
				    strcmp(nacm->rule_lists[i]->rules[j]->module, ntfmodule->name) == 0)) {
					/* rule does not match */
					continue;
				}

				/* 2) type and notification name */
				if (nacm->rule_lists[i]->rules[j]->type != NACM_RULE_NOTSET) {
					if (nacm->rule_lists[i]->rules[j]->type == NACM_RULE_NOTIF &&
					    nacm->rule_lists[i]->rules[j]->type_data.ntf_names != NULL) {
						for (k = 0; nacm->rule_lists[i]->rules[j]->type_data.ntf_names[k] != NULL; k++) {
							if (strcmp(nacm->rule_lists[i]->rules[j]->type_data.ntf_names[k], "*") == 0 ||
							    strcmp(nacm->rule_lists[i]->rules[j]->type_data.ntf_names[k], (char*)(ntfnode->name)) == 0) {
								break;
							}
						}
						if (nacm->rule_lists[i]->rules[j]->type_data.ntf_names[k] == NULL) {
							/* rule does not match - notification names do not match */
							continue;
						}
					} else {
						/* rule does not match - another type of rule */
						continue;
					}
				}

				/* 3) access */
				if ((nacm->rule_lists[i]->rules[j]->access & NACM_ACCESS_READ) == 0) {
					/* rule does not match */
					continue;
				}
				/* rule matches */
				retval = nacm->rule_lists[i]->rules[j]->action;
				goto nacmfree;
			}
		}
		/* no matching rule found */

		/* check nacm:default-deny-all */
		if ((model_ctxt = xmlXPathNewContext(ntfmodule->xml)) != NULL &&
		    xmlXPathRegisterNs(model_ctxt, BAD_CAST "yin", BAD_CAST NC_NS_YIN) == 0 &&
		    xmlXPathRegisterNs(model_ctxt, BAD_CAST "nacm", BAD_CAST NC_NS_NACM) == 0) {
			if ((defdeny = xmlXPathEvalExpression(BAD_CAST "/yin:module/yin:notification//nacm:default-deny-all", model_ctxt)) != NULL) {
				if (!xmlXPathNodeSetIsEmpty(defdeny->nodesetval)) {
					/* process all default-deny-all elements */
					for (i = 0; i < defdeny->nodesetval->nodeNr; i++) {
						if (compare_node_to_model(ntfnode, defdeny->nodesetval->nodeTab[i]->parent, ntfmodule->ns) == 1) {
							xmlXPathFreeObject(defdeny);
							xmlXPathFreeContext(model_ctxt);
							return(NACM_DENY);
						}
					}
				}
				xmlXPathFreeObject(defdeny);
			}
		}
		xmlXPathFreeContext(model_ctxt);
	}
	/* no matching rule found */

	/* default action */
	retval = nacm->default_read;

nacmfree:
	if (query_result != NULL) {
		xmlXPathFreeObject(query_result);
	}
	/* free NACM structure */
	for (i = 0; nacm->rule_lists != NULL && nacm->rule_lists[i] != NULL; i++) {
		nacm_rule_list_free(nacm->rule_lists[i]);
	}
	free(nacm->rule_lists);
	free(nacm);

	return (retval);
}
#endif /* not DISABLE_NOTIFICATIONS */

int nacm_check_operation(const nc_rpc* rpc)
{
	xmlXPathObjectPtr defdeny;
	xmlXPathContextPtr model_ctxt = NULL;
	xmlXPathObjectPtr query_result = NULL;
	xmlNodePtr opnode;
	const struct data_model* opmodule;
	NC_OP op;
	int i, j, k;

	if (rpc == NULL) {
		/* invalid input parameter */
		return (-1);
	}

	if (rpc->nacm == NULL) {
		/* NACM will not affect this RPC */
		return (NACM_PERMIT);
	}

	/* get the operation name from the rpc */
	query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE_ID":rpc", rpc->ctxt);
	if (check_query_result(query_result, "/rpc", 0, 0) != 0) {
		return (-1);
	}
	opnode = query_result->nodesetval->nodeTab[0]->children;
	while(opnode != NULL && opnode->type != XML_ELEMENT_NODE) {
		opnode = opnode->next;
	}
	xmlXPathFreeObject(query_result);
	if (opnode == NULL) {
		/* invalid message with missing operation */
		return (-1);
	}

	/* get module name where the operation is defined */
	opmodule = ncds_get_model_operation((char*)(opnode->name), (opnode->ns != NULL) ? (char*)(opnode->ns->href) : NULL);

	if (opmodule != NULL) {
		for (i = 0; rpc->nacm->rule_lists != NULL && rpc->nacm->rule_lists[i] != NULL; i++) {
			for (j = 0; rpc->nacm->rule_lists[i]->rules != NULL && rpc->nacm->rule_lists[i]->rules[j] != NULL; j++) {
				/*
				 * check rules (all must be met):
				 * - module-name matches "*" or the name of the module where the operation is defined
				 * - type is NACM_RULE_NOTSET or type is NACM_RULE_OPERATION and data contain "*" or the operation name
				 * - access has set NACM_ACCESS_EXEC bit
				 */

				/* 1) module name */
				if (!(strcmp(rpc->nacm->rule_lists[i]->rules[j]->module, "*") == 0 ||
				    strcmp(rpc->nacm->rule_lists[i]->rules[j]->module, opmodule->name) == 0)) {
					/* rule does not match */
					continue;
				}

				/* 2) type and operation name */
				if (rpc->nacm->rule_lists[i]->rules[j]->type != NACM_RULE_NOTSET) {
					if (rpc->nacm->rule_lists[i]->rules[j]->type == NACM_RULE_OPERATION &&
					    rpc->nacm->rule_lists[i]->rules[j]->type_data.rpc_names != NULL) {
						for (k = 0; rpc->nacm->rule_lists[i]->rules[j]->type_data.rpc_names[k] != NULL; k++) {
							if (strcmp(rpc->nacm->rule_lists[i]->rules[j]->type_data.rpc_names[k], "*") == 0 ||
							    strcmp(rpc->nacm->rule_lists[i]->rules[j]->type_data.rpc_names[k], (char*)(opnode->name)) == 0) {
								break;
							}
						}
						if (rpc->nacm->rule_lists[i]->rules[j]->type_data.rpc_names[k] == NULL) {
							/* rule does not match - operation names do not match */
							continue;
						}
					} else {
						/* rule does not match - another type of rule */
						continue;
					}
				}

				/* 3) access */
				if ((rpc->nacm->rule_lists[i]->rules[j]->access & NACM_ACCESS_EXEC) == 0) {
					/* rule does not match */
					continue;
				}
				/* rule matches */
				return (rpc->nacm->rule_lists[i]->rules[j]->action);
			}
		}
		/* no matching rule found */

		/* check nacm:default-deny-all */
		if ((model_ctxt = xmlXPathNewContext(opmodule->xml)) != NULL &&
		    xmlXPathRegisterNs(model_ctxt, BAD_CAST "yin", BAD_CAST NC_NS_YIN) == 0 &&
		    xmlXPathRegisterNs(model_ctxt, BAD_CAST "nacm", BAD_CAST NC_NS_NACM) == 0) {
			if ((defdeny = xmlXPathEvalExpression(BAD_CAST "/yin:module/yin:rpc//nacm:default-deny-all", model_ctxt)) != NULL) {
				if (!xmlXPathNodeSetIsEmpty(defdeny->nodesetval)) {
					/* process all default-deny-all elements */
					for (i = 0; i < defdeny->nodesetval->nodeNr; i++) {
						if (compare_node_to_model(opnode, defdeny->nodesetval->nodeTab[i]->parent, opmodule->ns) == 1) {
							xmlXPathFreeObject(defdeny);
							xmlXPathFreeContext(model_ctxt);
							return(NACM_DENY);
						}
					}
				}
				xmlXPathFreeObject(defdeny);
			}
		}
		xmlXPathFreeContext(model_ctxt);
	}
	/* no matching rule found */

	/* deny delete-config and kill-session */
	op = nc_rpc_get_op(rpc);
	if (op == NC_OP_DELETECONFIG || op == NC_OP_KILLSESSION) {
		return (NACM_DENY);
	}

	/* default action */
	return (rpc->nacm->default_exec);
}
