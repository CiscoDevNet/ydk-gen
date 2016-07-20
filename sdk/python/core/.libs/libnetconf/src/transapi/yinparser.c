#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "../netconf_internal.h"
#include "../datastore.h"
#include "../transapi.h"
#include "yinparser.h"

static int get_node_namespace(struct ns_pair ns_mapping[], xmlNodePtr node, char** prefix, char** uri)
{
	int i;

	(*prefix) = NULL;
	if (((*uri) = (char*)xmlGetNsProp(node, BAD_CAST "ns", BAD_CAST "libnetconf")) == NULL) {
		return(EXIT_FAILURE);
	} else {
		for (i=0; ns_mapping[i].href != NULL; i++) {
			if (strcmp(ns_mapping[i].href, (*uri)) == 0) {
				(*prefix) = strdup(ns_mapping[i].prefix);
				break;
			}
		}
		if ((*prefix) == NULL) {
			return(EXIT_FAILURE);
		}
	}

	return(EXIT_SUCCESS);
}

static struct model_tree* yinmodel_parse_recursive(xmlNodePtr model_node, struct ns_pair ns_mapping[], struct model_tree* parent, int* children_count)
{
	struct model_tree *children = NULL, *choice, *new_tree, *augment_children;
	xmlNodePtr int_tmp, list_tmp, model_tmp = model_node->children;
	int count = 0, case_count, config, augment_children_count;
	char * keys, * key, * config_text;
	char **new_strlist;
	xmlChar *value;

	while (model_tmp) {
		count++;
		new_tree = realloc(children, sizeof (struct model_tree) * count);
		if (new_tree == NULL) {
			ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
			return (children);
		}
		children = new_tree;

		/* first check if node holds configuration */
		int_tmp = model_tmp->children;
		config = 1;
		while (int_tmp) {
			if (xmlStrEqual (int_tmp->name, BAD_CAST "config")) {
				config_text = (char*)xmlGetProp(int_tmp, BAD_CAST "value");
				if (strcasecmp (config_text, "false") == 0 || strcasecmp (config_text, "0") == 0) {
					config = 0;
				}
				free (config_text);
				break;
			}
			int_tmp = int_tmp->next;
		}
	
		if (!config) {
			count--;
			model_tmp = model_tmp->next;
			continue;
		}
		/* check if node is in other namespace */
		if (get_node_namespace(ns_mapping, model_tmp, &children[count-1].ns_prefix, &children[count-1].ns_uri)) {
			/* or inherit from parent */
			free(children[count-1].ns_prefix);
			free(children[count-1].ns_uri);
			children[count-1].ns_prefix = strdup(parent->ns_prefix);
			children[count-1].ns_uri = strdup(parent->ns_uri);
		}
		children[count-1].name = (char*)xmlGetProp (model_tmp, BAD_CAST "name");
		children[count-1].keys_count = 0;
		children[count-1].keys = NULL;
		if (xmlStrEqual(model_tmp->name, BAD_CAST "container")) {
			children[count-1].type = YIN_TYPE_CONTAINER;
			children[count-1].children = yinmodel_parse_recursive (model_tmp, ns_mapping, &children[count-1], &children[count-1].children_count);
		} else if (xmlStrEqual(model_tmp->name, BAD_CAST "leaf")) {
			children[count-1].type = YIN_TYPE_LEAF;
			children[count-1].children = NULL;
			children[count-1].children_count = 0;
		} else if (xmlStrEqual(model_tmp->name, BAD_CAST "leaf-list")) {
			children[count-1].type = YIN_TYPE_LEAFLIST;
			children[count-1].children = NULL;
			children[count-1].children_count = 0;
			children[count-1].ordering = YIN_ORDER_SYSTEM;
			list_tmp = model_tmp->children;
			while (list_tmp) {
				if (xmlStrEqual(list_tmp->name, BAD_CAST "ordered-by")) {
					value = xmlGetProp(list_tmp, BAD_CAST "value");
					if (xmlStrEqual(value, BAD_CAST "user")) {
						children[count-1].ordering = YIN_ORDER_USER;
					}
					xmlFree(value);
					break;
				}
				list_tmp = list_tmp->next;
			}
		} else if (xmlStrEqual(model_tmp->name, BAD_CAST "list")) {
			children[count-1].type = YIN_TYPE_LIST;
			children[count-1].children = NULL;
			children[count-1].children_count = 0;
			children[count-1].ordering = YIN_ORDER_SYSTEM;
			children[count-1].children = yinmodel_parse_recursive (model_tmp, ns_mapping, &children[count-1], &children[count-1].children_count);
			list_tmp = model_tmp->children;
			while (list_tmp) {
				if (xmlStrEqual(list_tmp->name, BAD_CAST "ordered-by")) {
					value = xmlGetProp(list_tmp, BAD_CAST "value");
					if (xmlStrEqual(value, BAD_CAST "user")) {
						children[count-1].ordering = YIN_ORDER_USER;
					}
					xmlFree(value);
				} else if (xmlStrEqual(list_tmp->name, BAD_CAST "key")) {
					keys = (char*)xmlGetProp (list_tmp, BAD_CAST "value");
					children[count-1].keys_count = 0;
					children[count-1].keys = NULL;
					key = strtok (keys, " ");
					while (key) {
						children[count-1].keys_count++;
						new_strlist = realloc (children[count-1].keys, sizeof (char*) * children[count-1].keys_count);
						if (new_strlist == NULL) {
							ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
							/* try to continue */
						} else {
							children[count-1].keys  = new_strlist;
							children[count-1].keys[children[count-1].keys_count-1] = strdup (key);
						}
						key = strtok (NULL, " ");
					}
					free (keys);
				}
				list_tmp = list_tmp->next;
			}
		} else if (xmlStrEqual(model_tmp->name, BAD_CAST "choice")) {
			children[count-1].type = YIN_TYPE_CHOICE;
			children[count-1].children = yinmodel_parse_recursive (model_tmp, ns_mapping, &children[count-1], &children[count-1].children_count);
		} else if (xmlStrEqual(model_tmp->name, BAD_CAST "anyxml")) {
			children[count-1].type = YIN_TYPE_ANYXML;
			children[count-1].children = NULL;
			children[count-1].children_count = 0;
		} else if (xmlStrEqual(model_tmp->name, BAD_CAST "case")) {
			choice = yinmodel_parse_recursive (model_tmp, ns_mapping, &children[count-1], &case_count);
			new_tree = realloc (children, sizeof (struct model_tree) * (case_count+count));
			if (new_tree == NULL) {
				ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				/* try to continue */
			} else {
				children = new_tree;
				free (children[count-1].name);
				free (children[count-1].ns_prefix);
				free (children[count-1].ns_uri);
				memcpy (&children[count-1], choice, case_count*sizeof(struct model_tree));
				count += case_count;
			}
			free (choice);

			/* remove the increment of the case statement */
			count--;
		} else if (xmlStrEqual(model_tmp->name, BAD_CAST "augment")) {
			augment_children = yinmodel_parse_recursive(model_tmp, ns_mapping, &children[count-1], &augment_children_count);
			if ((new_tree = realloc(children, sizeof(struct model_tree) * (augment_children_count+count))) == NULL) {
				ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				/* try to continue */
			} else {
				children = new_tree;
				free(children[count-1].name);
				free(children[count-1].ns_prefix);
				free(children[count-1].ns_uri);
				memcpy(&children[count-1], augment_children, augment_children_count*sizeof(struct model_tree));
				count += augment_children_count;
			}
			free(augment_children);
			count--;
		} else {
			free (children[count-1].name);
			free (children[count-1].ns_prefix);
			free (children[count-1].ns_uri);
			count--;
			model_tmp = model_tmp->next;
			continue;
			/* warning: unsupported or elsewhere processed (key,config) node type found */
		}

		model_tmp = model_tmp->next;
	}

	(*children_count) = count;
	return children;
}

static void yinmodel_free_recursive(struct model_tree* yin)
{
	int i;

	if (yin == NULL) {
		return;
	}

	free(yin->ns_prefix);
	free(yin->ns_uri);

	for (i=0; i<yin->keys_count; i++) {
		free(yin->keys[i]);
	}
	free (yin->keys);

	for (i=0; i<yin->children_count; i++) {
		yinmodel_free_recursive (&yin->children[i]);
	}
	free (yin->children);

	free (yin->name);
}

void yinmodel_free(struct model_tree* yin)
{
	if (yin != NULL) {
		yinmodel_free_recursive (yin);
		free (yin);
	}
}

struct model_tree* yinmodel_parse(xmlDocPtr model_doc, struct ns_pair ns_mapping[])
{
	xmlNodePtr model_root, stmt, cfg_stmt;
	struct model_tree * yin, * yin_act;
	int i;
	char *config_text, *key, **auxlist;
	YIN_TYPE type;
	int recursive;

	if ((model_root = xmlDocGetRootElement (model_doc)) == NULL) {
		/* cant get model root */
		return NULL;
	}

	if (!xmlStrEqual (model_root->name, BAD_CAST "module")) {
		/* model root != module => invalid model */
		return NULL;
	}

	yin = calloc (1, sizeof (struct model_tree));
	if (yin == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}	
	yin->type = YIN_TYPE_MODULE;
	yin->name = (char*)xmlGetProp (model_root, BAD_CAST "name");


	/* find namespace, prefix and root of configuration data for this module */
	for (stmt = model_root->children; stmt != NULL; stmt = stmt->next) {
		if (xmlStrEqual(stmt->name, BAD_CAST "namespace")) {
			yin->ns_uri = (char*)xmlGetProp(stmt, BAD_CAST "uri");
			for (i = 0; ns_mapping[i].href != NULL; i++) {
				if (strcmp(ns_mapping[i].href, yin->ns_uri) == 0) {
					yin->ns_prefix = strdup(ns_mapping[i].prefix);
					break;
				}
			}
			if (yin->ns_prefix == NULL) {
				yinmodel_free(yin);
				return(NULL);
			}

			/*
			 * stop the loop, start searching for the data-def-stmt
			 * (RFC 6020. p.144)
			 */
			break;
		}
	}

	for (/* from previous loop*/; stmt != NULL; stmt = stmt->next) {
		if (stmt->type != XML_ELEMENT_NODE) {
			/* skip comments */
			continue;
		}

		/* get type of the data-def-stmt */
		type = 0;
		if (xmlStrEqual(stmt->name, BAD_CAST "container")) {
			type = YIN_TYPE_CONTAINER;
		} else if (xmlStrEqual(stmt->name, BAD_CAST "leaf")) {
			type = YIN_TYPE_LEAF;
		} else if (xmlStrEqual(stmt->name, BAD_CAST "leaf-list")) {
			type = YIN_TYPE_LEAFLIST;
		} else if (xmlStrEqual(stmt->name, BAD_CAST "list")) {
			type = YIN_TYPE_LIST;
		} else if (xmlStrEqual(stmt->name, BAD_CAST "choice")) {
			type = YIN_TYPE_CHOICE;
		} else if (xmlStrEqual(stmt->name, BAD_CAST "anyxml")) {
			type = YIN_TYPE_ANYXML;
		}

		if (type == 0) {
			/* non-interesting content */
			continue;
		}

		/* check config value */
		for (cfg_stmt = stmt->children; cfg_stmt != NULL; cfg_stmt = cfg_stmt->next) {
			if (cfg_stmt->type != XML_ELEMENT_NODE) {
				/* skip comments */
				continue;
			}

			if (xmlStrEqual(cfg_stmt->name, BAD_CAST "config")) {
				config_text = (char*) xmlGetProp(cfg_stmt, BAD_CAST "value");
				if (config_text && strcasecmp(config_text, "false") == 0) {
					free(config_text);

					/* state data, skip them */
					continue;
				}
				free(config_text);
			}
		}

		/* create difftree element */
		yin->children_count++;
		yin_act = realloc (yin->children, yin->children_count * sizeof (struct model_tree));
		if (yin_act == NULL) {
			yin->children_count--;
			yinmodel_free(yin);
			return(NULL);
		}
		yin->children = yin_act;
		yin->children[yin->children_count-1].name = (char*)xmlGetProp (stmt, BAD_CAST "name");
		yin->children[yin->children_count-1].type = type;
		yin->children[yin->children_count-1].keys_count = 0;
		yin->children[yin->children_count-1].keys = NULL;

		recursive = 0;
		switch (type) {
		case YIN_TYPE_CONTAINER:
		case YIN_TYPE_CHOICE:
			recursive = 1;
			break;
		case YIN_TYPE_LEAF:
		case YIN_TYPE_ANYXML:
			/* recursive = 0; */
			break;
		case YIN_TYPE_LEAFLIST:
			/* recursive = 0; */
			yin->children[yin->children_count-1].ordering = YIN_ORDER_SYSTEM;
			for (cfg_stmt = stmt->children; cfg_stmt != NULL; cfg_stmt = cfg_stmt->next) {
				if (cfg_stmt->type != XML_ELEMENT_NODE) {
					/* skip comments */
					continue;
				}

				if (xmlStrEqual(cfg_stmt->name, BAD_CAST "ordered-by")) {
					config_text = (char*) xmlGetProp(cfg_stmt, BAD_CAST "value");
					if (config_text && strcasecmp(config_text, "user") == 0) {
						yin->children[yin->children_count-1].ordering = YIN_ORDER_USER;
					}
					free(config_text);
					break;
				}
			}
			break;
		case YIN_TYPE_LIST:
			recursive = 1;

			yin->children[yin->children_count-1].ordering = YIN_ORDER_SYSTEM;
			for (cfg_stmt = stmt->children; cfg_stmt != NULL; cfg_stmt = cfg_stmt->next) {
				if (cfg_stmt->type != XML_ELEMENT_NODE) {
					/* skip comments */
					continue;
				}

				if (xmlStrEqual(cfg_stmt->name, BAD_CAST "ordered-by")) {
					config_text = (char*) xmlGetProp(cfg_stmt, BAD_CAST "value");
					if (config_text && strcasecmp(config_text, "user") == 0) {
						yin->children[yin->children_count-1].ordering = YIN_ORDER_USER;
					}
					free(config_text);
					break;
				} else if (xmlStrEqual(cfg_stmt->name, BAD_CAST "key")) {
					config_text = (char*) xmlGetProp(cfg_stmt, BAD_CAST "value");
					yin->children[yin->children_count-1].keys_count = 0;
					yin->children[yin->children_count-1].keys = NULL;
					for (key = strtok(config_text, " "); key != NULL; key = strtok(NULL, " ")) {
						yin->children[yin->children_count-1].keys_count++;
						auxlist = realloc(yin->children[yin->children_count-1].keys, sizeof(char*) * yin->children[yin->children_count-1].keys_count);
						if (auxlist == NULL) {
							ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
							/* try to continue */
						} else {
							yin->children[yin->children_count-1].keys  = auxlist;
							yin->children[yin->children_count-1].keys[yin->children[yin->children_count-1].keys_count-1] = strdup(key);
						}
					}
					free(config_text);
					break;
				}
			}

			break;
		default:
			WARN("%s: unsupported node type (%d) \"%s\"", type, yin->children[yin->children_count-1].name);
			free(yin->children[yin->children_count-1].name);
			free(yin->children[yin->children_count-1].ns_prefix);
			free(yin->children[yin->children_count-1].ns_uri);
			yin->children_count--;
			continue;
		}

		yin->children[yin->children_count-1].children_count = 0;
		yin->children[yin->children_count-1].children = NULL;
		if (get_node_namespace(ns_mapping, stmt, &yin->children[yin->children_count-1].ns_prefix, &yin->children[yin->children_count-1].ns_uri)) {
			yin->children[yin->children_count-1].ns_prefix = strdup(yin->ns_prefix);
			yin->children[yin->children_count-1].ns_uri = strdup(yin->ns_uri);
		}

		if (recursive) {
			yin_act = &yin->children[yin->children_count-1];
			yin_act->children = yinmodel_parse_recursive(stmt, ns_mapping, yin_act, &yin_act->children_count);
		}
	}

	return yin;
}
