/**
 * \file edit-config.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief NETCONF edit-config implementation independent on repository
 * implementation.
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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "edit_config.h"
#include "datastore_internal.h"
#include "../netconf.h"
#include "../netconf_internal.h"
#include "../nacm.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

#define XPATH_BUFFER 1024

#define NC_EDIT_OP_MERGE_STRING "merge"
#define NC_EDIT_OP_CREATE_STRING "create"
#define NC_EDIT_OP_DELETE_STRING "delete"
#define NC_EDIT_OP_REPLACE_STRING "replace"
#define NC_EDIT_OP_REMOVE_STRING "remove"
#define NC_EDIT_ATTR_OP "operation"

struct key_predicate {
	int position;
	char* prefix;
	char* href;
	char* name;
	char* value;
};

typedef enum {
	NC_CHECK_EDIT_DELETE = NC_EDIT_OP_DELETE,
	NC_CHECK_EDIT_CREATE = NC_EDIT_OP_CREATE
} NC_CHECK_EDIT_OP;

/* from datastore.c */
int is_key(xmlNodePtr parent, xmlNodePtr child, keyList keys);

int nc_nscmp(xmlNodePtr reference, xmlNodePtr node)
{
	int in_ns = 1;
	char* s = NULL;

	if (reference->ns != NULL && reference->ns->href != NULL) {

		/* XML namespace wildcard mechanism:
		 * 1) no namespace defined and namespace is inherited from message so it
		 *    is NETCONF base namespace
		 * 2) namespace is empty: xmlns=""
		 */
		if (!strcmp((char *)reference->ns->href, NC_NS_BASE10) ||
				!(s = nc_clrwspace((char*)(reference->ns->href))) ||
				strlen(s) == 0) {
			free(s);
			return 0;
		}
		free(s);

		in_ns = 0;
		if (node->ns != NULL) {
			if (!strcmp((char *)reference->ns->href, (char *)node->ns->href)) {
				in_ns = 1;
			}
		}
	}
	return (in_ns == 1 ? 0 : 1);
}

/**
 * \brief Get value of the operation attribute of the \<node\> element.
 * If no such attribute is present, defop parameter is used and returned.
 *
 * \param[in] node XML element to analyse
 * \param[in] defop Default operation to use if no specific operation is present
 * \param[out] err NETCONF error structure to store the error description in
 *
 * \return NC_OP_TYPE_ERROR on error, valid NC_OP_TYPE values otherwise
 */
static NC_EDIT_OP_TYPE get_operation(xmlNodePtr node, NC_EDIT_DEFOP_TYPE defop, struct nc_err** error)
{
	char *operation = NULL;
	NC_EDIT_OP_TYPE op;

	/* get specific operation the node */
	if ((operation = (char *) xmlGetNsProp(node, BAD_CAST NC_EDIT_ATTR_OP, BAD_CAST NC_NS_BASE)) != NULL) {
		if (!strcmp(operation, NC_EDIT_OP_MERGE_STRING)) {
			op = NC_EDIT_OP_MERGE;
		} else if (!strcmp(operation, NC_EDIT_OP_REPLACE_STRING)) {
			op = NC_EDIT_OP_REPLACE;
		} else if (!strcmp(operation, NC_EDIT_OP_CREATE_STRING)) {
			op = NC_EDIT_OP_CREATE;
		} else if (!strcmp(operation, NC_EDIT_OP_DELETE_STRING)) {
			op = NC_EDIT_OP_DELETE;
		} else if (!strcmp(operation, NC_EDIT_OP_REMOVE_STRING)) {
			op = NC_EDIT_OP_REMOVE;
		} else {
			if (error != NULL) {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_BAD_ATTR);
				}
				nc_err_set(*error, NC_ERR_PARAM_INFO_BADATTR, NC_EDIT_ATTR_OP);
			}
			op = NC_EDIT_OP_ERROR;
		}
	} else {
		if (defop != NC_EDIT_DEFOP_NONE) {
			op = (NC_EDIT_OP_TYPE) defop;
		} else {
			op = NC_EDIT_OP_NOTSET;
		}
	}
	free(operation);

	return op;
}

/**
 * \brief Get all the key elements from the configuration data model
 *
 * \param model         XML form (YIN) of the configuration data model.
 *
 * \return              keyList with references to all the keys in the data model.
 */
keyList get_keynode_list(xmlDocPtr model)
{
	xmlXPathContextPtr model_ctxt = NULL;
	xmlXPathObjectPtr result = NULL;

	if (model == NULL) {
		return (NULL);
	}

	/* create xpath evaluation context */
	model_ctxt = xmlXPathNewContext(model);
	if (model_ctxt == NULL) {
		return (NULL);
	}

	if (xmlXPathRegisterNs(model_ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
		xmlXPathFreeContext(model_ctxt);
		return (NULL);
	}

	result = xmlXPathEvalExpression(BAD_CAST "//" NC_NS_YIN_ID ":key", model_ctxt);
	if (result != NULL) {
		if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
			xmlXPathFreeObject(result);
			result = (NULL);
		}
	}
	xmlXPathFreeContext(model_ctxt);

	return ((keyList)result);
}

/* get the key nodes from the xml document */
static int find_key_elems(xmlNodePtr modelnode, xmlNodePtr node, int all, xmlNodePtr **result)
{
	xmlChar *str = NULL;
	char* s, *token;
	unsigned int i, c;

	/* get the name of the key node(s) from the 'value' attribute in key element in data model */
	if ((str = xmlGetProp(modelnode, BAD_CAST "value")) == NULL) {
		return EXIT_FAILURE;
	}

	/* attribute have the form of space-separated list of key nodes */
	/* get the number of keys */
	for (i = 0, c = 1; i < strlen((char*)str); i++) {
		if (str[i] == ' ') {
			c++;
		}
	}
	/* allocate sufficient array of pointers to key nodes */
	*result = (xmlNodePtr*)calloc(c + 1, sizeof(xmlNodePtr));
	if (*result == NULL) {
		xmlFree(str);
		return (EXIT_FAILURE);
	}

	/* and now process all key nodes defined in attribute value list */
	for (i = 0, s = (char*)str; i < c; i++, s = NULL) {
		token = strtok(s, " ");
		if (token == NULL) {
			break;
		}

		/* get key nodes in original xml tree - all keys are needed */
		(*result)[i] = node->children;
		while (((*result)[i] != NULL) && strcmp(token, (char*) ((*result)[i])->name)) {
			(*result)[i] = ((*result)[i])->next;
		}
		if ((*result)[i] == NULL) {
			if (all) {
				xmlFree(str);
				free(*result);
				*result = NULL;
				return (EXIT_FAILURE);
			} else {
				i--;
			}
		}
	}

	xmlFree(str);

	return EXIT_SUCCESS;
}

/**
 * \brief Get all the key nodes for the specific element.
 *
 * \param[in] keys List of key elements from the configuration data model.
 * \param[in] node Node for which the key elements are needed.
 * \param[in] all If set to 1, all the keys must be found in the node, non-zero is
 * returned otherwise.
 * \param[out] result List of pointers to the key elements from node's children.
 * \return Zero on success, non-zero otherwise.
 */
static int get_keys(keyList keys, xmlNodePtr node, int all, xmlNodePtr **result)
{
	xmlChar *str = NULL;
	int j, match;
	xmlNodePtr key_parent, node_parent;

	assert(keys != NULL);
	assert(node != NULL);
	assert(result != NULL);

	*result = NULL;

	for (j = 0; j < keys->nodesetval->nodeNr; j++) {
		/* get corresponding key definition from the data model */
		// name = xmlGetNsProp (keys->nodesetval->nodeTab[i]->parent, BAD_CAST "name", BAD_CAST NC_NS_YIN);
		match = 1;
		key_parent = keys->nodesetval->nodeTab[j]->parent;
		node_parent = node;

		while (1) {
			if ((str = xmlGetProp(key_parent, BAD_CAST "name")) == NULL) {
				match = 0;
				break;
			}
			if (xmlStrcmp(str, node_parent->name)) {
				xmlFree(str);
				match = 0;
				break;
			}
			xmlFree(str);

			do {
				key_parent = key_parent->parent;
			} while (key_parent && ((xmlStrcmp(key_parent->name, BAD_CAST "augment") == 0)
                    || (xmlStrcmp(key_parent->name, BAD_CAST "choice") == 0)
                    || (xmlStrcmp(key_parent->name, BAD_CAST "case") == 0)));
			node_parent = node_parent->parent;

			if ((!key_parent && node_parent) || (key_parent && !node_parent)) {
				match = 0;
				break;
			}

			if (!xmlStrcmp(key_parent->name, BAD_CAST "module") && (node_parent->type == XML_DOCUMENT_NODE)) {
				break;
			}
		}

		if (!match) {
			continue;
		}

		/*
		 * now we have key values for the appropriate node which was
		 * specified as function parameter, so there will be no other
		 * run in this for loop - no continue command is allowed from
		 * now to the end of the loop
		 */
		return find_key_elems(keys->nodesetval->nodeTab[j], node, all, result);
	}

	return (EXIT_SUCCESS);
}


/**
 * @return NULL if the node is not a part of the choice statement,
 * the branch node where the given node belongs to
 */
static xmlNodePtr is_partof_choice(xmlNodePtr node)
{
	xmlNodePtr aux;

	if (node == NULL) {
		return (NULL);
	}

	for (aux = node; aux->parent != NULL && aux->parent->type == XML_ELEMENT_NODE; aux = aux->parent) {
		if (xmlStrcmp(aux->parent->name, BAD_CAST "choice") == 0) {
			return (aux);
		}
	}
	return (NULL);
}

/**
 * @brief Check if the given node in the YIN data model defines list or leaf-list
 * ordered by user. In such a case, specific YANG attributes "insert", "value"
 * and "key" can appear.
 *
 * @param[in] node Model's node to check
 * @return 1 if the node is user ordered list, </br>
 * 2 if the node is user ordered leaf-list, </br>
 * 0 otherwise
 */
static int is_user_ordered_list(xmlNodePtr node)
{
	xmlNodePtr child;
	xmlChar *prop;
	int ret = 0;

	if (node == NULL) {
		return (0);
	}

	if (xmlStrcmp(node->name, BAD_CAST "list") == 0) {
		ret = 1;
	} else if (xmlStrcmp(node->name, BAD_CAST "leaf-list") == 0) {
		ret = 2;
	} else {
		return (0);
	}

	for (child = node->children; child != NULL; child = child->next) {
		if (child->type != XML_ELEMENT_NODE) {
			continue;
		}
		if (xmlStrcmp(child->name, BAD_CAST "ordered-by") != 0) {
			continue;
		}

		prop = xmlGetProp(child, BAD_CAST "value");
		if (prop != NULL) {
			if (xmlStrcmp(prop, BAD_CAST "user") == 0) {
				xmlFree(prop);
				break; /* for */
			}
			xmlFree(prop);
		}
	}

	if (child == NULL) {
		return (0);
	} else {
		return (ret);
	}
}

/**
 * \brief Compare 2 elements and decide if they are equal for NETCONF.
 *
 * Matching does not include attributes and children match (only key children are
 * checked). Furthemore, XML node types and namespaces are also checked.
 *
 * Supported XML node types are XML_TEXT_NODE and XML_ELEMENT_NODE.
 *
 * \param[in] node1 First node to compare.
 * \param[in] node2 Second node to compare.
 * \param[in] keys List of key elements from configuration data model.
 *
 * \return 0 - false, 1 - true (matching elements), -1 - error.
 */
int matching_elements(xmlNodePtr node1, xmlNodePtr node2, keyList keys, int leaf)
{
	xmlNodePtr *keynode_list;
	xmlNodePtr keynode, key;
	xmlChar *key_value = NULL, *keynode_value = NULL, *key_value2 = NULL, *keynode_value2 = NULL;
	char *aux1, *aux2;
	int i, ret;

	assert(node1 != NULL);
	assert(node2 != NULL);

	/* compare text nodes */
	if (node1->type == XML_TEXT_NODE && node2->type == XML_TEXT_NODE) {
		aux1 = nc_clrwspace((char*)(node1->content));
		aux2 = nc_clrwspace((char*)(node2->content));

		if (strcmp(aux1, aux2) == 0) {
			ret = 1;
		} else {
			ret = 0;
		}
		free(aux1);
		free(aux2);
		return ret;
	}

	/* check element types - only element nodes are processed */
	if ((node1->type != XML_ELEMENT_NODE) || (node2->type != XML_ELEMENT_NODE)) {
		return 0;
	}
	/* check element names */
	if (xmlStrcmp(node1->name, node2->name) != 0) {
		return 0;
	}

	/* check element namespace */
	if (nc_nscmp(node1, node2) != 0) {
		return 0;
	}

	/*
	 * if required, check children text node if exists, this is usually needed
	 * for leaf-list's items
	 */
	if (leaf == 1) {
		if (node1->children != NULL && node1->children->type == XML_TEXT_NODE &&
		    node2->children != NULL && node2->children->type == XML_TEXT_NODE) {
			/*
			 * we do not need to continue to keys checking since compared elements
			 * do not contain any children that can serve as a key
			 */
			return (matching_elements(node1->children, node2->children, NULL, 0));
		}
	}

	if (keys != NULL) {
		if (get_keys(keys, node1, 0, &keynode_list) != EXIT_SUCCESS) {
			return 0;
		}

		if (keynode_list != NULL) {
			keynode = keynode_list[0];
			for (i = 1; keynode != NULL; i++) {
				/* search in children for the key element */
				key = node2->children;
				while (key != NULL) {
					if (xmlStrcmp(key->name, keynode->name) == 0) {
						/* got key element, now check its value without leading/trailing whitespaces */
						key_value = xmlNodeGetContent(key);
						key_value2 = (xmlChar*)nc_clrwspace((char*)key_value);
						xmlFree(key_value);

						keynode_value = xmlNodeGetContent(keynode);
						keynode_value2 = (xmlChar*)nc_clrwspace((char*)keynode_value);
						xmlFree(keynode_value);
						if (xmlStrcmp(keynode_value2, key_value2) == 0) {
							/* value matches, go for next key if any */
							break; /* while loop */
						} else {
							/* key value does not match, this is always bad */
							xmlFree(key_value2);
							xmlFree(keynode_value2);
							free(keynode_list);
							return 0;
						}
					} else {
						/* this was not the key element, try the next one */
						key = key->next;
					}
				}

				/* cleanup for next round */
				xmlFree(key_value2);
				xmlFree(keynode_value2);

				if (key == NULL) {
					/* there is no matching node */
					free(keynode_list);
					return 0;
				}

				/* go to the next key if any */
				keynode = keynode_list[i];
			}
			free(keynode_list);
		}
	}

	return 1;
}

static xmlNodePtr find_element_model_compare(xmlNodePtr node, xmlNodePtr model_node)
{
	xmlNodePtr aux, retval;
	xmlChar* name;

	if (xmlStrcmp(model_node->name, BAD_CAST "choice") == 0 ||
	    xmlStrcmp(model_node->name, BAD_CAST "case") == 0 ||
	    xmlStrcmp(model_node->name, BAD_CAST "augment") == 0) {
		for (aux = model_node->children; aux != NULL; aux = aux->next) {
			retval = find_element_model_compare(node, aux);
			if (retval != NULL) {
				return (retval);
			}
		}
	} else {
		name = xmlGetProp(model_node, BAD_CAST "name");
		if (name == NULL) {
			return (NULL);
		}

		if (xmlStrcmp(node->name, name) == 0) {
			xmlFree(name);
			return (model_node);
		}
		xmlFree(name);
	}

	return (NULL);
}

/**
 * @brief Go recursively in the YIN model and find model's equivalent of the node
 * @param[in] node XML element which we want to find in the model
 * @param[in] model Configuration data model (YIN format)
 * @return model's equivalent of the node, NULL if no such element is found.
 */
xmlNodePtr find_element_model(xmlNodePtr node, xmlDocPtr model)
{
	xmlNodePtr mparent, aux, retval;

	if (node == NULL || node->parent == NULL) {
		return (NULL);
	}

	if (node->parent->type != XML_DOCUMENT_NODE) {
		mparent = find_element_model(node->parent, model);
	} else {
		mparent = xmlDocGetRootElement(model);
	}
	if (mparent == NULL) {
		return (NULL);
	}

	for (aux = mparent->children; aux != NULL; aux = aux->next) {
		retval = find_element_model_compare(node, aux);
		if (retval != NULL) {
			return (retval);
		}
	}

	return (NULL);
}

/**
 * @brief Get the default value of the node if a default value is defined in the model
 * @param[in] node XML element whose default value we want to get
 * @param[in] model Configuration data model for the document of the given node.
 * @return Default value of the node, NULL if no default value is defined or found.
 */
static xmlChar* get_default_value(xmlNodePtr node, xmlDocPtr model)
{
	xmlNodePtr mnode, aux;
	xmlChar* value = NULL;

	mnode = find_element_model(node, model);
	if (mnode == NULL) {
		return (NULL);
	}

	for (aux = mnode->children; aux != NULL; aux = aux->next) {
		if (xmlStrcmp(aux->name, BAD_CAST "default") == 0) {
			value = xmlGetNsProp(aux, BAD_CAST "value", BAD_CAST NC_NS_YIN);
			break;
		}
	}

	return (value);
}

/**
 * \brief Find an equivalent of the given edit node on orig_doc document.
 *
 * \param[in] orig_doc Original configuration document to edit.
 * \param[in] edit Element from the edit-config's \<config\>. Its equivalent in
 *                 orig_doc should be found.
 * \param[in] keys List of the key elements from the configuration data model.
 * \return Found equivalent element, NULL if no such element exists.
 */
xmlNodePtr find_element_equiv(xmlDocPtr orig_doc, xmlNodePtr edit, xmlDocPtr model, keyList keys)
{
	xmlNodePtr orig_parent, node, model_def;
	int leaf = 0;

	if (edit == NULL || orig_doc == NULL) {
		return (NULL);
	}

	/* go recursively to the root */
	if (edit->parent->type != XML_DOCUMENT_NODE) {
		orig_parent = find_element_equiv(orig_doc, edit->parent, model, keys);
	} else {
		if (orig_doc->children == NULL) {
			orig_parent = NULL;
		} else {
			orig_parent = orig_doc->children->parent;
		}
	}
	if (orig_parent == NULL) {
		return (NULL);
	}

	model_def = find_element_model(edit, model);
	if (model_def != NULL && xmlStrcmp(model_def->name, BAD_CAST "leaf-list") == 0) {
		/* check also children text element when checking elements matching */
		leaf = 1;
	}

	/* element check */
	node = orig_parent->children;
	while (node != NULL) {
		/* compare edit and node */
		if (matching_elements(edit, node, keys, leaf) == 0) {
			/* non matching nodes */
			node = node->next;
			continue;
		} else {
			/* matching nodes found */
			return (node);
		}
	}

	/* no corresponding node found */
	return (NULL);
}

/**
 * \brief Get the list of elements with the specified selected edit-config's operation.
 *
 * \param[in] op edit-config's operation type to search for.
 * \param[in] edit XML document covering edit-config's \<config\> element. The
 *                 elements with specified operation will be searched for in
 *                 this document.
 */
static xmlXPathObjectPtr get_operation_elements(NC_EDIT_OP_TYPE op, xmlDocPtr edit)
{
	xmlXPathContextPtr edit_ctxt = NULL;
	xmlXPathObjectPtr operation_nodes = NULL;
	xmlChar xpath[XPATH_BUFFER];
	char *opstring;

	assert(edit != NULL);

	switch (op) {
	case NC_EDIT_OP_MERGE:
		opstring = NC_EDIT_OP_MERGE_STRING;
		break;
	case NC_EDIT_OP_REPLACE:
		opstring = NC_EDIT_OP_REPLACE_STRING;
		break;
	case NC_EDIT_OP_CREATE:
		opstring = NC_EDIT_OP_CREATE_STRING;
		break;
	case NC_EDIT_OP_DELETE:
		opstring = NC_EDIT_OP_DELETE_STRING;
		break;
	case NC_EDIT_OP_REMOVE:
		opstring = NC_EDIT_OP_REMOVE_STRING;
		break;
	default:
		ERROR("Unsupported edit operation %d (%s:%d).", op, __FILE__, __LINE__);
		return (NULL);
	}

	/* create xpath evaluation context */
	edit_ctxt = xmlXPathNewContext(edit);
	if (edit_ctxt == NULL) {
		if (edit_ctxt != NULL) {
			xmlXPathFreeContext(edit_ctxt);
		}
		ERROR("Creating the XPath evaluation context failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	if (xmlXPathRegisterNs(edit_ctxt, BAD_CAST NC_NS_BASE_ID, BAD_CAST NC_NS_BASE) != 0) {
		xmlXPathFreeContext(edit_ctxt);
		ERROR("Registering a namespace for XPath failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	if (snprintf((char*)xpath, XPATH_BUFFER, "//*[@%s:operation='%s']", NC_NS_BASE_ID, opstring) <= 0) {
		xmlXPathFreeContext(edit_ctxt);
		ERROR("Preparing the XPath query failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	operation_nodes = xmlXPathEvalExpression(BAD_CAST xpath, edit_ctxt);

	/* clean up */
	xmlXPathFreeContext(edit_ctxt);

	return (operation_nodes);
}

/**
 * \brief Check edit-config's node operations hierarchy.
 *
 * In case of the removal ("remove" and "delete") operations, the supreme operation
 * (including the default operation) cannot be the creation ("create or "replace")
 * operation.
 *
 * In case of the creation operations, the supreme operation cannot be a removal
 * operation.
 *
 * \param[in] edit XML node from edit-config's \<config\> whose hierarchy
 *                 (supreme operations) will be checked.
 * \param[in] defop Default edit-config's operation for this edit-config call.
 * \param[out] err NETCONF error structure.
 * \return On error, non-zero is returned and err structure is filled. Zero is
 * returned on success.
 */
static int check_edit_ops_hierarchy(xmlNodePtr edit, NC_EDIT_DEFOP_TYPE defop, struct nc_err **error)
{
	xmlNodePtr parent;
	NC_EDIT_OP_TYPE op, parent_op;

	assert(error != NULL);

	op = get_operation(edit, NC_EDIT_DEFOP_NOTSET, error);
	if (op == (NC_EDIT_OP_TYPE)NC_EDIT_DEFOP_NOTSET) {
		/* no operation defined for this node */
		return EXIT_SUCCESS;
	} else if (op == NC_EDIT_OP_ERROR) {
		return EXIT_FAILURE;
	} else if (op == NC_EDIT_OP_DELETE || op == NC_EDIT_OP_REMOVE) {
		if (defop == NC_EDIT_DEFOP_REPLACE) {
			if (error != NULL) {
				*error = nc_err_new(NC_ERR_OP_FAILED);
			}
			return EXIT_FAILURE;
		}

		/* check parent elements for operation compatibility */
		parent = edit->parent;
		while (parent->type != XML_DOCUMENT_NODE) {
			parent_op = get_operation(parent, NC_EDIT_DEFOP_NOTSET, error);
			if (parent_op == NC_EDIT_OP_ERROR) {
				return EXIT_FAILURE;
			} else if (parent_op == NC_EDIT_OP_CREATE || parent_op == NC_EDIT_OP_REPLACE) {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_OP_FAILED);
				}
				return EXIT_FAILURE;
			}
			parent = parent->parent;
		}
	} else if (op == NC_EDIT_OP_CREATE || op == NC_EDIT_OP_REPLACE) {
		/* check parent elements for operation compatibility */
		parent = edit->parent;
		while (parent->type != XML_DOCUMENT_NODE) {
			parent_op = get_operation(parent, NC_EDIT_DEFOP_NOTSET, error);
			if (parent_op == NC_EDIT_OP_ERROR) {
				return EXIT_FAILURE;
			} else if (parent_op == NC_EDIT_OP_DELETE || parent_op == NC_EDIT_OP_REMOVE) {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_OP_FAILED);
				}
				return EXIT_FAILURE;
			}
			parent = parent->parent;
		}
	}

	return EXIT_SUCCESS;
}

/**
 * \brief Check edit-config's operation rules.
 *
 * In case of the "create" operation, if the configuration data exists, the
 * "data-exists" error is generated.
 *
 * In case of the "delete" operation, if the configuration data does not exist, the
 * "data-missing" error is generated.
 *
 * Operation hierarchy check check_edit_ops_hierarchy() is also applied.
 *
 * \param[in] op Operation type to check (only the "delete" and "create" operation
 * types are valid).
 * \param[in] defop Default edit-config's operation for this edit-config call.
 * \param[in] orig Original configuration document to edit.
 * \param[in] edit XML document covering edit-config's \<config\> element
 * supposed to edit the orig configuration data.
 * \param[in] model XML form (YIN) of the configuration data model appropriate
 * to the given repo.
 * \param[out] err NETCONF error structure.
 * \return On error, non-zero is returned and an err structure is filled. Zero is
 * returned on success.
 */
static int check_edit_ops(NC_CHECK_EDIT_OP op, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr orig, xmlDocPtr edit, xmlDocPtr model, struct nc_err **error)
{
	xmlXPathObjectPtr operation_nodes = NULL;
	xmlNodePtr node_to_process = NULL, n;
	keyList keys;
	xmlChar *defval = NULL, *value = NULL;
	int i;

	assert(orig != NULL);
	assert(edit != NULL);
	assert(error != NULL);

	keys = get_keynode_list(model);

	operation_nodes = get_operation_elements((NC_EDIT_OP_TYPE)op, edit);
	if (operation_nodes == NULL) {
		if (error != NULL) {
			*error = nc_err_new(NC_ERR_OP_FAILED);
		}
		if (keys != NULL) {
			keyListFree(keys);
		}
		return EXIT_FAILURE;
	}

	if (xmlXPathNodeSetIsEmpty(operation_nodes->nodesetval)) {
		xmlXPathFreeObject(operation_nodes);
		if (keys != NULL) {
			keyListFree(keys);
		}
		return EXIT_SUCCESS;
	}

	*error = NULL;
	for (i = 0; i < operation_nodes->nodesetval->nodeNr; i++) {
		node_to_process = operation_nodes->nodesetval->nodeTab[i];

		if (check_edit_ops_hierarchy(node_to_process, defop, error) != EXIT_SUCCESS) {
			xmlXPathFreeObject(operation_nodes);
			if (keys != NULL) {
				keyListFree(keys);
			}
			return EXIT_FAILURE;
		}

		/* \todo namespace handlings */
		n = find_element_equiv(orig, node_to_process, model, keys);
		if (op == NC_CHECK_EDIT_DELETE && n == NULL) {
			if (ncdflt_get_basic_mode() == NCWD_MODE_ALL) {
				/* A valid 'delete' operation attribute for a
				 * data node that contains its schema default
				 * value MUST succeed, even though the data node
				 * is immediately replaced by the server with
				 * the default value.
				 */
				defval = get_default_value(node_to_process, model);
				if (defval == NULL) {
					/* no default value for this node */
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_DATA_MISSING);
					}
					break;
				}
				value = xmlNodeGetContent(node_to_process);
				if (value == NULL) {
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_DATA_MISSING);
					}
					break;
				}
				if (xmlStrcmp(defval, value) != 0) {
					/* node do not contain default value */
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_DATA_MISSING);
					}
					break;
				} else {
					/* remove delete operation - it is valid
					 * but there is no reason to really
					 * perform it
					 */
					xmlUnlinkNode(node_to_process);
					xmlFreeNode(node_to_process);
				}
				xmlFree(defval);
				defval = NULL;
				xmlFree(value);
				value = NULL;
			} else {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_DATA_MISSING);
				}
				break;
			}
		} else if (op == NC_CHECK_EDIT_CREATE && n != NULL) {
			if (ncdflt_get_basic_mode() == NCWD_MODE_TRIM) {
				/* A valid 'create' operation attribute for a
				 * data node that has a schema default value
				 * defined MUST succeed.
				 */
				defval = get_default_value(node_to_process, model);
				if (defval == NULL) {
					/* no default value for this node */
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_DATA_EXISTS);
					}
					break;
				}
				value = xmlNodeGetContent(node_to_process);
				if (value == NULL) {
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_DATA_EXISTS);
					}
					break;
				}
				if (xmlStrcmp(defval, value) != 0) {
					/* node do not contain default value */
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_DATA_MISSING);
					}
					break;
				} else {
					/* remove old node in configuration to
					 * allow recreate it by the new one with
					 * the default value
					 */
					xmlUnlinkNode(n);
					xmlFreeNode(n);
				}
				xmlFree(defval);
				defval = NULL;
				xmlFree(value);
				value = NULL;

			} else {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_DATA_EXISTS);
				}
				break;
			}
		}
	}
	xmlXPathFreeObject(operation_nodes);
	if (defval != NULL) {
		xmlFree(defval);
	}
	if (value != NULL) {
		xmlFree(value);
	}
	if (keys != NULL) {
		keyListFree(keys);
	}

	if (*error != NULL) {
		return (EXIT_FAILURE);
	} else {
		return EXIT_SUCCESS;
	}
}

/**
 * \brief Perform edit-config's "delete" operation on the selected node.
 *
 * \param[in] node XML node from the configuration data to delete.
 * \return Zero on success, non-zero otherwise.
 */
static int edit_delete(xmlNodePtr node)
{
	assert(node != NULL);

	VERB("Deleting the node %s (%s:%d)", (char*)node->name, __FILE__, __LINE__);
	if (node != NULL) {
		xmlUnlinkNode(node);
		xmlFreeNode(node);
	}

	return EXIT_SUCCESS;
}

/**
 * \brief Perform edit-config's "remove" operation on the selected node.
 *
 * \param[in] orig_doc Original configuration document to edit.
 * \param[in] edit_node Node from the edit-config's \<config\> element with
 * the specified "remove" operation.
 * \param[in] keys  List of the key elements from the configuration data model.
 *
 * \return Zero on success, non-zero otherwise.
 */
static int edit_remove(xmlDocPtr orig_doc, xmlNodePtr edit_node, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error)
{
	xmlNodePtr old;
	char *msg = NULL;
	int ret;

	old = find_element_equiv(orig_doc, edit_node, model, keys);

	if (old == NULL) {
		ret = EXIT_SUCCESS;
	} else {
		/* remove edit node's equivalent from the original document */
		/* NACM */
		if (nacm_check_data(old, NACM_ACCESS_DELETE, nacm) == NACM_PERMIT) {
			/* remove the edit node's equivalent from the original document */
			edit_delete(old);

			/* in case of list, it can be possible to apply the node repeatedly */
			while ((old = find_element_equiv(orig_doc, edit_node, model, keys)) != NULL) {
				edit_delete(old);
			}

			ret = EXIT_SUCCESS;
		} else {
			if (error != NULL) {
				*error = nc_err_new(NC_ERR_ACCESS_DENIED);
				if (asprintf(&msg, "removing \"%s\" data node is not permitted.", (char*)(old->name)) != -1) {
					nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
					free(msg);
				}
			}
			ret = EXIT_FAILURE;
		}
	}

	/* remove the node from the edit document */
	edit_delete(edit_node);

	return ret;
}

/**
 * @param[in] predicate Definition of the instance identifier predicate
 * @param[out] prefix Node namespace prefix if any, call free()
 * @param[out] name Node's name, call free()
 * @param[out] value Specified content of the node
 * @return negative value on error, 0 on predicate-expr (out parameters are used)
 * or positive value as position
 */
static int parse_instance_predicate(const char* predicate, char **prefix, char **name, char **value)
{
	int i = 0, j;
	int retval = 0;
	char *p, *e, *c;

	if (predicate == NULL ) {
		return (-1);
	}
	p = strdup(predicate);
	if (p == NULL) {
		return (-1);
	}

	while(p[i] == ' ' || p[i] == '\t') {
		i++;
	}
	if (p[i] != '[') {
		free(p);
		return (-1);
	}
	i++;
	while(p[i] == ' ' || p[i] == '\t') {
		i++;
	}
	if ((e = strchr(p, '=')) == NULL) {
		/* predicate is specified as position */
		retval = atoi(&(p[i]));
		if (retval == 0) {
			free(p);
			return (-1);
		} else {
			free(p);
			return (retval);
		}
	} else {
		/* predicate is specified as predicate-expr */
		if (e == &(p[i])) {
			/* there is no node-identifier */
			free(p);
			return (-1);
		}
		/* get the name - remove trailing whitespaces */
		e[0] = 0;
		for (j = -1; &(e[j]) != &(p[i]) && (e[j] == ' ' || e[j] == '\t'); j--) {
			e[j] = 0;
		}
		if ((c = strchr(&(p[i]), ':')) != NULL) {
			c[0] = 0;
			if (prefix != NULL) {
				*prefix = strdup(&(p[i]));
			}
			if (name != NULL) {
				*name = strdup(&(c[1]));
			}
		} else {
			if (prefix != NULL) {
				*prefix = NULL;
			}
			if (name != NULL) {
				*name = strdup(&(p[i]));
			}
		}
		/* get the value */
		/* skip leading whitespaces */
		j = 1;
		while(e[j] == ' ' || e[j] == '\t') {
			j++;
		}
		if (e[j] != '"' && e[j] != '\'') {
			/* invalid format */
			free(p);
			if (name) {
				free(*name);
				*name = NULL;
			}
			if (prefix) {
				free(*prefix);
				*prefix = NULL;
			}
			return(-1);
		}
		j++;
		i = j;
		while (e[i] != '"' && e[i] != '\'') {
			i++;
		}
		e[i] = 0;
		if (value != NULL) {
			*value = strdup(&(e[j]));
		}
		free(p);
		return (0);
	}
}

static xmlNodePtr get_ref_list(xmlNodePtr parent, xmlNodePtr edit_node, struct nc_err **error)
{
	xmlChar *ref;
	xmlNsPtr ns;
	char *s, *token;
	int i, j;
	xmlNodePtr retval, node, keynode;
	struct key_predicate** keys;

	if ((ref = xmlGetNsProp(edit_node, BAD_CAST "key", BAD_CAST NC_NS_YANG)) == NULL) {
		/* insert reference specification is missing */
		if (error != NULL) {
			*error = nc_err_new(NC_ERR_MISSING_ATTR);
			nc_err_set(*error, NC_ERR_PARAM_INFO_BADATTR, "key");
			nc_err_set(*error, NC_ERR_PARAM_MSG, "Missing \"key\" attribute to insert list item");
		}
		return (NULL);
	}

	/* count the keys in predicate */
	for (i = 0, s = strchr((char*)ref, '['); s != NULL; i++, s = strchr(s+1, '['));
	if (i == 0) {
		/* something went wrong */
		if (error != NULL) {
			*error = nc_err_new(NC_ERR_BAD_ATTR);
			nc_err_set(*error, NC_ERR_PARAM_INFO_BADATTR, "key");
			nc_err_set(*error, NC_ERR_PARAM_MSG, "Invalid value of the \"key\" attribute to insert list item");
		}
		return (NULL);
	}
	keys = malloc((i + 1) * sizeof(struct key_predicate*));
	if (keys == NULL) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	for (j = 0, s = (char*)ref; j < i; j++, s = NULL) {
		token = strtok(s, "]");
		if (token == NULL) {
			keys[j] = NULL;
			break;
		}

		keys[j] = malloc(sizeof(struct key_predicate));
		if (keys[j] == NULL) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			retval = NULL;
			goto cleanup;
		}
		keys[j]->position = parse_instance_predicate(token, &(keys[j]->prefix), &(keys[j]->name), &(keys[j]->value));
		if (keys[j]->position == -1) {
			if (error != NULL) {
				*error = nc_err_new(NC_ERR_BAD_ATTR);
				nc_err_set(*error, NC_ERR_PARAM_INFO_BADATTR, "key");
				nc_err_set(*error, NC_ERR_PARAM_MSG, "Invalid value of the \"key\" attribute to insert list item");
			}
			keys[j+1] = NULL;
			retval = NULL;
			goto cleanup;
		}

		/* search for namespase (href) for the prefix */
		keys[j]->href = NULL;
		for (node = edit_node; node->type == XML_ELEMENT_NODE; node = node->parent) {
			for (ns = node->ns; ns != NULL; ns = ns->next) {
				if (keys[j]->prefix == NULL) {
					if (ns->prefix == NULL) {
						keys[j]->href = strdup((char*)(ns->href));
						break;
					}
				} else if (xmlStrcmp(ns->prefix, BAD_CAST (keys[j]->prefix)) == 0) {
					keys[j]->href = strdup((char*)(ns->href));
					break;
				}
			}
			if (keys[j]->href != NULL) {
				break;
			}

			for (ns = node->nsDef; ns != NULL; ns = ns->next) {
				if (keys[j]->prefix == NULL) {
					if (ns->prefix == NULL) {
						keys[j]->href = strdup((char*)(ns->href));
						break;
					}
				} else if (xmlStrcmp(ns->prefix, BAD_CAST (keys[j]->prefix)) == 0) {
					keys[j]->href = strdup((char*)(ns->href));
					break;
				}
			}
			if (keys[j]->href != NULL) {
				break;
			}
		}
		if (keys[j]->href == NULL) {
			if (error != NULL) {
				*error = nc_err_new(NC_ERR_BAD_ATTR);
				nc_err_set(*error, NC_ERR_PARAM_INFO_BADATTR, "key");
				nc_err_set(*error, NC_ERR_PARAM_MSG, "Invalid namespace prefix in value of the \"key\" attribute to insert list item");
			}
			keys[j+1] = NULL;
			retval = NULL;
			goto cleanup;
		}
	}
	xmlFree(ref);
	keys[j] = NULL; /* the list terminating NULL */

	/* search for the referenced node */
	retval = NULL;
	j = 1;
	for (node = parent->children; node != NULL; node = node->next) {
		if (node->type != XML_ELEMENT_NODE) {
			continue;
		}
		if (node->ns == NULL || xmlStrcmp(node->ns->href, edit_node->ns->href) != 0) {
			continue;
		}
		if (xmlStrcmp(node->name, edit_node->name) != 0) {
			continue;
		}

		/* reference specified as position */
		if (keys[0]->position > 0) {
			if (keys[0]->position == j) {
				retval = node;
				break;
			}
			j++;
			continue;
		}

		/* check key elements of this node */
		keynode = NULL;
		for (i = 0; keys[i] != NULL; i++) {
			if (keys[i]->position != 0) {
				/* this should not happen */
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_BAD_ATTR);
					nc_err_set(*error, NC_ERR_PARAM_INFO_BADATTR, "key");
					nc_err_set(*error, NC_ERR_PARAM_MSG, "Invalid mixing of the \"key\" attribute content to insert list item");
				}
				retval = NULL;
				goto cleanup;
			}
			for (keynode = node->children; keynode != NULL; keynode = keynode->next) {
				if (keynode->ns == NULL || keynode->ns->href == NULL) {
					continue;
				}

				if (xmlStrcmp(keynode->ns->href, BAD_CAST (keys[i]->href)) != 0) {
					continue;
				}

				if (xmlStrcmp(keynode->name, BAD_CAST(keys[i]->name)) != 0) {
					continue;
				}

				if (keynode->children == NULL || keynode->children->type != XML_TEXT_NODE) {
					continue;
				}

				s = nc_clrwspace((char*)(keynode->children->content));
				if (s == NULL || strcmp(s, keys[i]->value) != 0) {
					free(s);
					continue;
				}
				free(s);

				/* we have the match */
				break;
			}
			if (keynode == NULL) {
				/* key not found */
				break;
			}
		}
		if (keynode != NULL) {
			if (retval == NULL) {
				retval = node;
			} else {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_OP_FAILED);
					nc_err_set(*error, NC_ERR_PARAM_APPTAG, "data-not-unique");
					nc_err_set(*error, NC_ERR_PARAM_MSG, "Specified value of the \"key\" attribute to insert list item refers multiple data.");
				}
				retval = NULL;
				goto cleanup;
			}
		}
	}

cleanup:

	for (i = 0; keys[i] != NULL; i++) {
		free(keys[i]->name);
		free(keys[i]->prefix);
		free(keys[i]->href);
		free(keys[i]->value);
		free(keys[i]);
	}
	free(keys);

	return (retval);
}

static xmlNodePtr get_ref_leaflist(xmlNodePtr parent, xmlNodePtr edit_node, struct nc_err **error)
{
	xmlChar *ref;
	char *s;
	xmlNodePtr retval;

	if ((ref = xmlGetNsProp(edit_node, BAD_CAST "value", BAD_CAST NC_NS_YANG)) == NULL) {
		/* insert reference specification is missing */
		if (error != NULL) {
			*error = nc_err_new(NC_ERR_MISSING_ATTR);
			nc_err_set(*error, NC_ERR_PARAM_INFO_BADATTR, "value");
			nc_err_set(*error, NC_ERR_PARAM_MSG, "Missing \"value\" attribute to insert leaf-list");
		}
		return (NULL);
	}
	xmlRemoveProp(xmlHasNsProp(edit_node, BAD_CAST "value", BAD_CAST NC_NS_YANG));
	VERB("Reference value for leaf-list is \"%s\" (%s:%d)", ref, __FILE__, __LINE__);

	/* search for the referenced node */
	for (retval = parent->children; retval != NULL; retval = retval->next) {
		if (xmlStrcmp(retval->name, edit_node->name) != 0 ||
		    retval->children == NULL || retval->children->type != XML_TEXT_NODE) {
			continue;
		}

		s = nc_clrwspace((char*)(retval->children->content));
		if (xmlStrcmp(ref, BAD_CAST s) == 0) {
			free(s);
			break;
		}
		free(s);
	}
	xmlFree(ref);

	return (retval);
}

/**
 * @brief Learn whether the namespace definition is used as namespace in the
 * subtree.
 * @param[in] node Node where to start checking.
 * @param[in] ns Namespace to find.
 * @return 0 if the namespace is not used, 1 if the usage of the namespace was found
 */
static int nc_find_namespace_usage(xmlNodePtr node, xmlNsPtr ns)
{
	xmlNodePtr child;
	xmlAttrPtr prop;

	/* check the element itself */
	if (node->ns == ns) {
		return 1;
	} else {
		/* check attributes of the element */
		for (prop = node->properties; prop != NULL; prop = prop->next) {
			if (prop->ns == ns) {
				return 1;
			}
		}

		/* go recursive into children */
		for (child = node->children; child != NULL; child = child->next) {
			if (child->type == XML_ELEMENT_NODE && nc_find_namespace_usage(child, ns) == 1) {
				return 1;
			}
		}
	}

	return 0;
}

/**
 * @brief Remove namespace definition from the node which are no longer used.
 * @param[in] node XML element node where to check for namespace definitions
 */
static void nc_clear_namespaces(xmlNodePtr node)
{
	xmlNsPtr ns, prev = NULL;

	if (node == NULL || node->type != XML_ELEMENT_NODE) {
		return;
	}

	for (ns = node->nsDef; ns != NULL; ) {
		if (nc_find_namespace_usage(node, ns) == 0) {
			/* no one use the namespace - remove it */
			if (prev == NULL) {
				node->nsDef = ns->next;
				xmlFreeNs(ns);
				ns = node->nsDef;
			} else {
				prev->next = ns->next;
				xmlFreeNs(ns);
				ns = prev->next;
			}
		} else {
			/* check another namespace definition */
			prev = ns;
			ns = ns->next;
		}
	}
}

/**
 * Common routine to create a node
 */
static int edit_create_routine(xmlNodePtr parent, xmlNodePtr edit_node)
{
	if (parent == NULL || edit_node == NULL) {
		ERROR("%s: invalid input parameter.", __func__);
		return (EXIT_FAILURE);
	}

	/* create a new element in the configuration data as a copy of the element from the edit-config */
	VERB("Creating the node %s (%s:%d)", (char*)edit_node->name, __FILE__, __LINE__);
	if (parent->type == XML_DOCUMENT_NODE) {
		if (parent->children == NULL) {
			xmlDocSetRootElement(parent->doc, xmlCopyNode(edit_node, 1));
		} else {
			/* adding root's sibling! */
			xmlAddChild(parent, xmlCopyNode(edit_node, 1));
		}
	} else {
		if (xmlAddChild(parent, xmlCopyNode(edit_node, 1)) == NULL) {
			ERROR("%s: Creating new node (%s) failed (%s:%d)", __func__, (char*)(edit_node->name), __FILE__, __LINE__);
			return (EXIT_FAILURE);
		}
	}

	return (EXIT_SUCCESS);
}

static int edit_create_lists(xmlNodePtr parent, xmlNodePtr edit_node, xmlDocPtr model, keyList keys, struct nc_err** error)
{
	int list_type;
	xmlChar *insert;
	xmlNodePtr node = NULL, created = NULL;
	int before_flag = -1;

	if (parent == NULL) {
		return (EXIT_FAILURE);
	}

	if (error != NULL) {
		*error = NULL;
	}

	if ((list_type = is_user_ordered_list(find_element_model(edit_node, model))) == 0) {
		return (EXIT_FAILURE);
	}
	/*
	 * get the insert attribute and remove it from the edit node that will
	 * be placed into the configuration datastore
	 */
	insert = xmlGetNsProp(edit_node, BAD_CAST "insert", BAD_CAST NC_NS_YANG);
	xmlRemoveProp(xmlHasNsProp(edit_node, BAD_CAST "insert", BAD_CAST NC_NS_YANG));

	/* switch according to the insert value */
	if (insert == NULL || xmlStrcmp(insert, BAD_CAST "last") == 0) {
		if ((created = xmlAddChild(parent, xmlCopyNode(edit_node, 1))) == NULL) {
			goto error;
		}
	} else if (xmlStrcmp(insert, BAD_CAST "first") == 0) {
		if (parent->children == NULL) {
			if ((created = xmlAddChild(parent, xmlCopyNode(edit_node, 1))) == NULL) {
				goto error;
			}
		} else {
			/* check if the parent is list */
			if (is_user_ordered_list(find_element_model(parent, model)) != 0) {
				/* we are in the list, so the first nodes must be the keys and
				 * we have to place this new node only as the first instance of
				 * it, not as the first child node of its parent
				 */
				for (node = parent->children; node != NULL; node = node->next) {
					if (node->type != XML_ELEMENT_NODE || xmlStrcmp(node->name, edit_node->name) != 0) {
						continue;
					}
					/* we have currently first instance of the edit_node's list */
					break;
				}
				if (node != NULL) {
					/* put the new node before the currently first instance of the list */
					if ((created = xmlAddPrevSibling(node, xmlCopyNode(edit_node, 1))) == NULL) {
						goto error;
					}
				} else {
					/* put it as last node since there is currently no instance of the list */
					if ((created = xmlAddChild(parent, xmlCopyNode(edit_node, 1))) == NULL) {
						goto error;
					}
				}
			} else {
				/* it is not a list, so simply place it as the first child */
				if ((created = xmlAddPrevSibling(parent->children, xmlCopyNode(edit_node, 1))) == NULL) {
					goto error;
				}
			}
		}
	} else {
		/* check and remember the operation to avoid code duplication */
		if (xmlStrcmp(insert, BAD_CAST "before") == 0) {
			before_flag = 1; /* before flag */
		} else if (xmlStrcmp(insert, BAD_CAST "after") == 0) {
			before_flag = 0; /* after flag */
		} else {
			ERROR("Unknown (%s) leaf-list insert requested.", (char*)insert);
			goto error;
		}

		/* do the job */
		if (list_type == 2) { /* leaf-list */
			node = get_ref_leaflist(parent, edit_node, error);
		} else if (list_type == 1) {
			node = get_ref_list(parent, edit_node, error);
		}
		if (node == NULL) {
			/* insert reference node not found */
			if (error != NULL && *error == NULL) {
				*error = nc_err_new(NC_ERR_BAD_ATTR);
				nc_err_set(*error, NC_ERR_PARAM_APPTAG, "missing-instance");
			}
			goto error;
		} else {
			if (!matching_elements(edit_node, node, keys,(list_type == 2) ? 1 : 0)) {
				if (before_flag == 1) {
					/* place the node before its reference */
					xmlRemoveProp(xmlHasNsProp(edit_node, BAD_CAST "key", BAD_CAST NC_NS_YANG));
					if ((created = xmlAddPrevSibling(node, xmlCopyNode(edit_node, 1))) == NULL) {
						goto error;
					}
				} else if (before_flag == 0) {
					/* place the node after its reference */
					xmlRemoveProp(xmlHasNsProp(edit_node, BAD_CAST "key", BAD_CAST NC_NS_YANG));
					if ((created = xmlAddNextSibling(node, xmlCopyNode(edit_node, 1))) == NULL) {
						goto error;
					}
				} /* else nonsence */
			} /* else we are referencing the node being created */
		}
	}

	xmlFree(insert);
	nc_clear_namespaces(created);

	return (EXIT_SUCCESS);

error:
	xmlFree(insert);
	keyListFree(keys);
	return (EXIT_FAILURE);
}

/**
 * @brief Remove all nodes from other choice's cases then the except_node
 * @param[in] parent Parent configuration data node holding the choice content.
 * @param[in] except_node The configuration node from the choice's case that
 * should be preserved
 * @param[in] model Configuration data model in YIN format.
 * @return 0 in case of success, 1 in case of error
 */
static int edit_choice_clean(xmlNodePtr parent, xmlNodePtr except_node, xmlDocPtr model, const struct nacm_rpc* nacm, struct nc_err** error)
{
	xmlNodePtr choice_branch, child, aux;
	char* msg = NULL;
	int r;

	if (except_node == NULL || (choice_branch = is_partof_choice(find_element_model(except_node, model))) == NULL) {
		/* ignore request if the except_node is not a part of choice statement */
		return (EXIT_SUCCESS);
	}

	for (child = parent->children; child != NULL; ) {
		if (child->type != XML_ELEMENT_NODE) {
			child = child->next;
			continue;
		}

		aux = is_partof_choice(find_element_model(child, model));
		if (aux == NULL) {
			child = child->next;
			continue;
		}

		if (aux->parent == choice_branch->parent && aux != choice_branch) {
			/* we have another branch of the same choice -> remove it */
			aux = child->next;

			/* NACM */
			if ((r = nacm_check_data(child, NACM_ACCESS_DELETE, nacm)) == NACM_PERMIT) {
				/* remove the edit node's equivalent from the original document */
				edit_delete(child);
			} else if (r == NACM_DENY) {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_ACCESS_DENIED);
					if (asprintf(&msg, "removing \"%s\" data node is not permitted.", (char*)(child->name)) != -1) {
						nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
						free(msg);
					}
				}
				return (EXIT_FAILURE);
			} else {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_OP_FAILED);
				}
				return (EXIT_FAILURE);
			}

			child = aux;
		} else {
			child = child->next;
		}
	}

	return (EXIT_SUCCESS);
}

/**
 * @brief Create a node and handle the case that the node is one of the choice's
 * case. Then, all nodes from other cases must be deleted.
 *
 * @param[in] parent Parent configuration data node holding the choice content.
 * @param[in] edit_node The configuration node to create.
 * @param[in] model Configuration data model in YIN format.
 * @return 0 in case of success, 1 in case of error
 */
static int edit_create_choice(xmlNodePtr parent, xmlNodePtr edit_node, xmlDocPtr model, const struct nacm_rpc* nacm, struct nc_err** error)
{
	if (edit_choice_clean(parent, edit_node, model, nacm, error) == EXIT_FAILURE) {
		return (EXIT_FAILURE);
	}

	return (edit_create_routine(parent, edit_node));
}

/**
 * \brief Recursive variant of edit_create() function to create the missing parent path of the node to be created.
 *
 * \param[in] orig_doc Original configuration document to edit.
 * \param[in] edit_node Node from the missing parent chain of the element to
 *                      create. If there is no equivalent node in the original
 *                      document, it is created.
 * \param[in] keys  List of key elements from configuration data model.
 *
 * \return Zero on success, non-zero otherwise.
 */
static xmlNodePtr edit_create_recursively(xmlDocPtr orig_doc, xmlNodePtr edit_node, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error)
{
	int r;
	char *msg = NULL;
	xmlNodePtr retval = NULL;
	xmlNodePtr parent = NULL;
	xmlNsPtr ns_aux;

	if (edit_node == NULL || orig_doc == NULL) {
		ERROR("%s: invalid input parameter.", __func__);
		if (error != NULL) {
			*error = nc_err_new(NC_ERR_OP_FAILED);
		}
		return (NULL);
	}

	retval = find_element_equiv(orig_doc, edit_node, model, keys);
	if (retval == NULL) {
		if (defop == NC_EDIT_DEFOP_NONE && !get_operation(edit_node, NC_EDIT_DEFOP_NOTSET, NULL)) {
			/* parent of the node to create does not exist and it is not supposed to be created */
			if (error != NULL) {
				*error = nc_err_new(NC_ERR_DATA_MISSING);
			}
			return NULL;
		}

		/* NACM check */
		if (nacm != NULL) {
			if ((r = nacm_check_data(edit_node->parent, NACM_ACCESS_CREATE, nacm)) != NACM_PERMIT) {
				if (r == NACM_DENY) {
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_ACCESS_DENIED);
						if (asprintf(&msg, "creating \"%s\" data node is not permitted.", (char*)(edit_node->parent->name)) != -1) {
							nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
							free(msg);
						}
					}
				} else {
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_OP_FAILED);
					}
				}
				return (NULL);
			}
		}

		if (edit_node->parent->type == XML_DOCUMENT_NODE) {
			/* original document is empty */
			VERB("Creating the parent %s (%s:%d)", (char*)edit_node->name, __FILE__, __LINE__);
			retval = xmlCopyNode(edit_node, 0);
			if (edit_node->ns) {
				ns_aux = xmlNewNs(retval, edit_node->ns->href, NULL);
				xmlSetNs(retval, ns_aux);
			}
			xmlDocSetRootElement(orig_doc, retval);
			return (retval);
		}

		parent = edit_create_recursively(orig_doc, edit_node->parent, defop, model, keys, nacm, error);
		if (parent == NULL) {
			return (NULL);
		}
		VERB("Creating the parent %s (%s:%d)", (char*)edit_node->name, __FILE__, __LINE__);
		retval = xmlAddChild(parent, xmlCopyNode(edit_node, 0));
		if (edit_node->ns && parent->ns && xmlStrcmp(edit_node->ns->href, parent->ns->href) == 0) {
			xmlSetNs(retval, parent->ns);
		} else if (edit_node->ns) {
			ns_aux = xmlNewNs(retval, edit_node->ns->href, NULL);
			xmlSetNs(retval, ns_aux);
		}
	}
	return retval;
}

/**
 * \brief Perform edit-config's "create" operation.
 *
 * \param[in] orig_doc Original configuration document to edit.
 * \param[in] edit_node Node from the edit-config's \<config\> element with
 * specified "create" operation.
 * \param[in] keys  List of key elements from configuration data model.
 *
 * \return Zero on success, non-zero otherwise.
 */
static int edit_create(xmlDocPtr orig_doc, xmlNodePtr edit_node, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error)
{
	xmlNodePtr parent = NULL, model_node;
	int r;
	char *msg = NULL;

	assert(orig_doc != NULL);
	assert(edit_node != NULL);

	/* NACM */
	if (nacm != NULL) {
		if ((r = nacm_check_data(edit_node, NACM_ACCESS_CREATE, nacm)) != NACM_PERMIT) {
			if (r == NACM_DENY) {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_ACCESS_DENIED);
					if (asprintf(&msg, "creating \"%s\" data node is not permitted.", (char*)(edit_node->name)) != -1) {
						nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
						free(msg);
					}
				}
			} else {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_OP_FAILED);
				}
			}
			return (EXIT_FAILURE);
		}
	}

	if (edit_node->parent->type != XML_DOCUMENT_NODE) {
		parent = edit_create_recursively(orig_doc, edit_node->parent, defop, model, keys, nacm, error);
		if (parent == NULL) {
			return EXIT_FAILURE;
		}
	} else {
		/* we are in the root */
		parent = (xmlNodePtr)(orig_doc->doc);
	}

	/* remove operation attribute */
	xmlRemoveProp(xmlHasNsProp(edit_node, BAD_CAST NC_EDIT_ATTR_OP, BAD_CAST NC_NS_BASE));
	nc_clear_namespaces(edit_node);

	/* handle user-ordered lists */
	model_node = find_element_model(edit_node, model);
	if (is_user_ordered_list(model_node) != 0) {
		if (edit_create_lists(parent, edit_node, model, keys, error) == EXIT_FAILURE) {
			return (EXIT_FAILURE);
		}
	} else if (is_partof_choice(model_node) != NULL) {
		if (edit_create_choice(parent, edit_node, model, nacm, error) == EXIT_FAILURE) {
			return (EXIT_FAILURE);
		}
	} else {
		/* create a new element in the configuration data as a copy of the element from the edit-config */
		if (edit_create_routine(parent, edit_node) == EXIT_FAILURE) {
			return (EXIT_FAILURE);
		}
	}

	/* remove the node from the edit document */
	edit_delete(edit_node);

	return EXIT_SUCCESS;
}

int edit_replace_nacmcheck(xmlNodePtr orig_node, xmlDocPtr edit_doc, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error)
{
	xmlNodePtr aux;
	int r;

	if (orig_node == NULL || edit_doc == NULL) {
		return (-1);
	}

	if (nacm == NULL) {
		/* permit */
		return (NACM_PERMIT);
	}

	if (orig_node->children == NULL || orig_node->children->type == XML_TEXT_NODE) {
		aux = find_element_equiv(edit_doc, orig_node, model, keys);
		if (aux == NULL) {
			/* orig_node has no equivalent in edit, so it will be removed */
			if ((r = nacm_check_data(orig_node, NACM_ACCESS_DELETE, nacm)) != NACM_PERMIT) {
				return (r);
			}
		} else {
			/* orig_node has equivalent in edit, so it will be updated */
			if ((r = nacm_check_data(orig_node, NACM_ACCESS_UPDATE, nacm)) != NACM_PERMIT) {
				return (r);
			}
		}
	} else {
		for (aux = orig_node->children; aux != NULL ; aux = aux->next) {
			/* do a recursion checks */
			if ((r = edit_replace_nacmcheck(aux, edit_doc, model, keys, nacm, error)) != NACM_PERMIT) {
				return (r);
			}
		}

	}

	return (NACM_PERMIT);
}

/**
 * \brief Perform edit-config's "replace" operation on the selected node.
 *
 * \param[in] orig_doc Original configuration document to edit.
 * \param[in] edit_node Node from the edit-config's \<config\> element with
 * the specified "replace" operation.
 * \param[in] keys  List of the key elements from the configuration data model.
 *
 * \return Zero on success, non-zero otherwise.
 */
static int edit_replace(xmlDocPtr orig_doc, xmlNodePtr edit_node, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error)
{
	xmlNodePtr old;
	int r;
	char *msg = NULL;

	if (orig_doc == NULL) {
		return (EXIT_FAILURE);
	}

	if (edit_node == NULL) {
		if ((r = nacm_check_data(orig_doc->children, NACM_ACCESS_DELETE, nacm)) == NACM_PERMIT) {
			return (edit_delete(orig_doc->children));
		} else if (r == NACM_DENY) {
			if (error != NULL) {
				*error = nc_err_new(NC_ERR_ACCESS_DENIED);
				if (asprintf(&msg, "removing \"%s\" data node is not permitted.", (char*)(orig_doc->children->name)) != -1) {
					nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
					free(msg);
				}
			}
		} else {
			if (error != NULL) {
				*error = nc_err_new(NC_ERR_OP_FAILED);
			}
		}
		return (EXIT_FAILURE);
	}

	old = find_element_equiv(orig_doc, edit_node, model, keys);
	if (old == NULL) {
		/* node to be replaced doesn't exist, so create new configuration data */
		return edit_create(orig_doc, edit_node, defop, model, keys, nacm, error);
	} else {
		/* NACM */
		if ((r = edit_replace_nacmcheck(old, edit_node->doc, model, keys, nacm, error)) != NACM_PERMIT) {
			if (r == NACM_DENY) {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_ACCESS_DENIED);
					if (asprintf(&msg, "replacing \"%s\" data node is not permitted.", (char*)(old->name)) != -1) {
						nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
						free(msg);
					}
				}
			} else {
				if (error != NULL) {
					*error = nc_err_new(NC_ERR_OP_FAILED);
				}
			}
			return (EXIT_FAILURE);
		}

		/* remove operation attribute */
		xmlRemoveProp(xmlHasNsProp(edit_node, BAD_CAST NC_EDIT_ATTR_OP, BAD_CAST NC_NS_BASE));
		nc_clear_namespaces(edit_node);

		/*
		 * replace old configuration data with the new data
		 * Do this removing the old node and creating a new one to cover actual
		 * "moving" of the instance of the list/leaf-list using YANG's insert
		 * attribute
		 */
		xmlUnlinkNode(old);
		xmlFreeNode(old);
		return edit_create(orig_doc, edit_node, defop, model, keys, nacm, error);
	}
}

/*
 * @return:
 * 0 - nothing done, node is not a list item
 * 1 - something really bad happened
 * 2 - item moved
 */
static int edit_merge_lists(xmlNodePtr merged_node, xmlNodePtr edit_node, xmlDocPtr model, keyList keys, struct nc_err** error)
{
	xmlNodePtr refnode = NULL, parent;
	int list_type;
	int before_flag = -1;
	char* insert;

	/* if this is a list/leaf-list, moving using insert attribute can be required */
	if ((list_type = is_user_ordered_list(find_element_model(merged_node, model))) != 0) {
		/* get the insert attribute and remove it from the merged node if already placed in */
		if ((insert = (char*)xmlGetNsProp(edit_node, BAD_CAST "insert", BAD_CAST NC_NS_YANG)) != NULL) {
			xmlRemoveProp(xmlHasNsProp(merged_node, BAD_CAST "insert", BAD_CAST NC_NS_YANG));

			VERB("Merging list with insert value \"%s\" (%s:%d)", insert, __FILE__, __LINE__);
			parent = merged_node->parent;

			/* switch according to the insert value */
			if (insert == NULL || strcmp(insert, "last") == 0) {
				/* move aux to the end of the children list */
				if (merged_node->next != NULL) {
					xmlUnlinkNode(merged_node);
					xmlAddChild(parent, merged_node);
				}
			} else if (strcmp(insert, "first") == 0) {
				/* move it to the beginning of the children list */
				if (merged_node->prev != NULL) {
					xmlUnlinkNode(merged_node);
					if (is_user_ordered_list(find_element_model(parent, model)) != 0) {
						/* we are in the list, so the first nodes must be the keys and
						 * we have to place this new node only as the first instance of
						 * it, not as the first child node of its parent
						 */
						for (refnode = parent->children; refnode != NULL; refnode = refnode->next) {
							if (refnode->type != XML_ELEMENT_NODE || xmlStrcmp(refnode->name, merged_node->name) != 0) {
								continue;
							}
							/* we have currently first instance of the edit_node's list */
							break;
						}
						if (refnode != NULL) {
							/* relink th node before the currently first instance of the list */
							xmlAddPrevSibling(refnode, merged_node);
						} else {
							/* re-link the node as last node since there is currently no instance of the list */
							xmlAddChild(parent, merged_node);
						}
					} else {
						/* it is not a list, so simply place it as the first child */
						xmlAddPrevSibling(parent->children, merged_node);
					}
				}
			} else {
				/* check and remembre the operation to avoid code duplication */
				if (strcmp(insert, "before") == 0) {
					before_flag = 1;
				} else if (strcmp(insert, "after") == 0) {
					before_flag = 0;
				} else {
					ERROR("Unknown (%s) leaf-list insert requested.", (char*)insert);
					xmlFree(insert);
					return (1);
				}

				/* do the job */
				if (list_type == 2) { /* leaf-list */
					refnode = get_ref_leaflist(parent, edit_node, error);
				} else if (list_type == 1) {
					refnode = get_ref_list(parent, edit_node, error);
				}
				if (refnode == NULL) {
					/* insert reference node not found */
					if (error != NULL && *error == NULL) {
						*error = nc_err_new(NC_ERR_BAD_ATTR);
						nc_err_set(*error, NC_ERR_PARAM_APPTAG, "missing-instance");
					}
					xmlFree(insert);
					return (1);
				} else {
					if (!matching_elements(merged_node, refnode, keys, (list_type == 2) ? 1 : 0)) {
						if (before_flag == 1) {
							/* place the node before its reference */
							xmlUnlinkNode(merged_node);
							xmlAddPrevSibling(refnode, merged_node);
						} else if (before_flag == 0) {
							/* place the node after its reference */
							xmlUnlinkNode(merged_node);
							xmlAddNextSibling(refnode, merged_node);
						} /* else nonsense */
					} /* else we are referencing the same node as being merged */
				}
			}
		}
		xmlFree(insert);
		return (2);
	}

	return (0);
}

static xmlNodePtr is_list(xmlNodePtr node, xmlDocPtr model)
{
	xmlNodePtr model_node;

	model_node = find_element_model(node, model);
	if (model_node == NULL) {
		WARN("unknown element %s!", (char* )(node->name));
		return (0);
	} else if (xmlStrcmp(model_node->name, BAD_CAST "list") == 0) {
		return (model_node);
	} else {
		return (NULL);
	}
}

static int is_leaf_list(xmlNodePtr node, xmlDocPtr model)
{
	xmlNodePtr model_node;

	model_node = find_element_model(node, model);
	if (model_node == NULL) {
		WARN("unknown element %s!", (char* )(node->name));
		return (0);
	} else if (xmlStrcmp(model_node->name, BAD_CAST "leaf-list") == 0) {
		return (1);
	} else {
		return (0);
	}
}

static int edit_merge_recursively(xmlNodePtr orig_node, xmlNodePtr edit_node, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error)
{
	xmlNodePtr children, aux, next, nextchild, parent;
	int r, access, duplicates;
	int leaf_list;
	char *msg = NULL;

	/* process leaf text nodes - even if we are merging, leaf text nodes are
	 * actually replaced by data specified by the edit configuration data
	 */
	if (edit_node->type == XML_TEXT_NODE) {
		if (orig_node->type == XML_TEXT_NODE) {
			VERB("Merging the node %s (%s:%d)", (char*)edit_node->name, __FILE__, __LINE__);

			/*
			 * check if this is defined as leaf or leaf-list. In case of leaf,
			 * the value will be updated, in case of leaf-list, the item will
			 * be created
			 */
			if (is_leaf_list(edit_node->parent, model)) {
				/*
				 * according to RFC 6020, sec. 7.7.7, leaf-list entries can be
				 * created or deleted, but they can not be modified
				 */
				access = NACM_ACCESS_CREATE;
			} else {
				access = NACM_ACCESS_UPDATE;
			}

			/* NACM */
			if (nacm != NULL) {
				if ((r = nacm_check_data(orig_node->parent, access, nacm)) != NACM_PERMIT) {
					if (r == NACM_DENY) {
						if (error != NULL) {
							*error = nc_err_new(NC_ERR_ACCESS_DENIED);
							if (asprintf(&msg, "updating \"%s\" data node is not permitted.", (char*)(orig_node->parent->name)) != -1) {
								nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
								free(msg);
							}
						}
					} else {
						if (error != NULL) {
							*error = nc_err_new(NC_ERR_OP_FAILED);
						}
					}
					return (EXIT_FAILURE);
				}
			}

			if (access == NACM_ACCESS_UPDATE) {
				if (xmlReplaceNode(orig_node, aux = xmlCopyNode(edit_node, 1)) == NULL) {
					ERROR("Replacing text nodes when merging failed (%s:%d)", __FILE__, __LINE__);
					return EXIT_FAILURE;
				}
				xmlFreeNode(orig_node);
				nc_clear_namespaces(aux);
			} else { /* access == NACM_ACCESS_CREATE */
				duplicates = 0;

				/* check previous existence of exactly the same element in original document */
				if (orig_node->parent != NULL && orig_node->parent->parent != NULL) {
					for (aux = orig_node->parent->parent->children; aux != NULL ; aux = aux->next) {
						/* we don't need keys since this is a leaf-list */
						if (matching_elements(aux, edit_node->parent, NULL, 1) == 1) { /* checks text content */
							duplicates = 1;
							break;
						}
					}
				}
				if (duplicates == 0) {
					/* create a new text element (item in leaf-list) */
					if ((aux = xmlAddNextSibling(orig_node->parent, xmlCopyNode(edit_node->parent, 1))) == NULL ) {
						ERROR("Adding leaf-list node when merging failed (%s:%d)", __FILE__, __LINE__);
						return EXIT_FAILURE;
					}
					nc_clear_namespaces(aux);
				}
			}
		}
	}

	children = edit_node->children;
	while (children != NULL) {
		/* skip checks if the node is text */
		if (children->type == XML_TEXT_NODE) {
			/* find text element to children */
			aux = orig_node->children;
			while (aux != NULL && aux->type != XML_TEXT_NODE) {
				aux = aux->next;
			}
		} else {
			/* skip key elements from merging */
			if (is_key(edit_node, children, keys) != 0) {
				children = children->next;
				continue;
			}
			/* skip comments */
			if (children->type == XML_COMMENT_NODE) {
				children = children->next;
				continue;
			}

			/* find matching element to children */
			leaf_list = is_leaf_list(children, model);
			aux = orig_node->children;
			while (aux != NULL && matching_elements(children, aux, keys, leaf_list) == 0) {
				aux = aux->next;
			}
		}

		nextchild = children->next;
		if (aux == NULL) {
			/*
			 * there is no equivalent element of the children in the
			 * original configuration data, so create it as new
			 */
			VERB("Adding a missing node %s while merging (%s:%d)", (char*)children->name, __FILE__, __LINE__);
			if (edit_create(orig_node->doc, children, defop, model, keys, nacm, error) != 0) {
				ERROR("Adding missing nodes when merging failed (%s:%d)", __FILE__, __LINE__);
				return EXIT_FAILURE;
			}
		} else {
			/* go recursive through all matching elements */
			if (children->type == XML_TEXT_NODE) {
				while (aux != NULL) {
					next = aux->next;
					if (aux->type == XML_TEXT_NODE) {
						if (edit_merge_recursively(aux, children, defop, model, keys, nacm, error) != EXIT_SUCCESS) {
							return EXIT_FAILURE;
						}
					}
					aux = next;
				}
			} else {
				VERB("Merging the node %s (%s:%d)", (char*)children->name, __FILE__, __LINE__);
				parent = aux->parent;

				/*
				 * if the node is leaf-list, we have to reflect it when
				 * searching for matching elements - in such a case we
				 * need exactly the same (with the same value) node for
				 * YANG's  ordered-by user feature, because we are going
				 * to move the node. In other cases (mainly leafs) we
				 * don't care the content, because we want to change it.
				 */
				leaf_list = is_leaf_list(children, model);

				while (aux != NULL) {
					next = aux->next;
					if (matching_elements(children, aux, keys, leaf_list) != 0) {
						if (edit_merge_recursively(aux, children, defop, model, keys, nacm, error) != EXIT_SUCCESS) {
							return EXIT_FAILURE;
						}

						/* update pointer to aux, it could change in edit_merge_recursively() */
						if (next != NULL) {
							aux = next->prev;
						} else {
							aux = parent->last;
						}

						switch (edit_merge_lists(aux, children, model, keys, error)) {
						case 1:
							/* error */
							return (EXIT_FAILURE);
							break;
						case 2:
							/* we've just moved item in the list, so we can skip the rest siblings */
							next = NULL;
							break;
						default:
							break;
						}
						if (edit_choice_clean(parent, children, model, nacm, error) == EXIT_FAILURE) {
							return (EXIT_FAILURE);
						}
					}
					aux = next;
				}
			}
		}

		children = nextchild;
	}

	return EXIT_SUCCESS;
}

int edit_merge(xmlDocPtr orig_doc, xmlNodePtr edit_node, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error)
{
	xmlNodePtr orig_node;
	xmlNodePtr aux, children;
	int r;
	char *msg = NULL;

	assert(edit_node != NULL);

	/* here can be processed only elements or document root */
	if (edit_node->type != XML_ELEMENT_NODE) {
		ERROR("Merge request for unsupported XML node types (%s:%d)", __FILE__, __LINE__);
		return EXIT_FAILURE;
	}

	VERB("Merging the node %s (%s:%d)", (char*)edit_node->name, __FILE__, __LINE__);
	orig_node = find_element_equiv(orig_doc, edit_node, model, keys);
	if (orig_node == NULL) {
		return edit_create(orig_doc, edit_node, defop, model, keys, nacm, error);
	}

	children = edit_node->children;
	while (children != NULL) {
		if (children->type == XML_ELEMENT_NODE) {
			if (is_key(edit_node, children, keys) != 0) {
				/* skip key elements from merging */
				children = children->next;
				continue;
			}

			aux = find_element_equiv(orig_doc, children, model, keys);
		} else if (children->type == XML_TEXT_NODE) {
			aux = find_element_equiv(orig_doc, children->parent, model, keys);
			if (aux) {
				aux = aux->children;
			}
		} else {
			children = children->next;
			continue;
		}

		if (aux == NULL) {
			/*
			 * there is no equivalent element of the children in the
			 * original configuration data, so create it as new
			 */

			/* NACM */
			if (nacm != NULL) {
				if ((r = nacm_check_data(children, NACM_ACCESS_CREATE, nacm)) != NACM_PERMIT) {
					if (r == NACM_DENY) {
						if (error != NULL) {
							*error = nc_err_new(NC_ERR_ACCESS_DENIED);
							if (asprintf(&msg, "creating \"%s\" data node is not permitted.", (char*)(children->name)) != -1) {
								nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
								free(msg);
							}
						}
					} else {
						if (error != NULL) {
							*error = nc_err_new(NC_ERR_OP_FAILED);
						}
					}
					return (EXIT_FAILURE);
				}
			}

			if ((aux = xmlAddChild(orig_node, xmlCopyNode(children, 1))) == NULL) {
				ERROR("Adding missing nodes when merging failed (%s:%d)", __FILE__, __LINE__);
				return EXIT_FAILURE;
			}
		} else {
			/* go recursive */
			VERB("Merging the node %s (%s:%d)", (char*)children->name, __FILE__, __LINE__);
			if (edit_merge_recursively(aux, children, defop, model, keys, nacm, error) != EXIT_SUCCESS) {
				return EXIT_FAILURE;
			}

			if (edit_merge_lists(aux, children, model, keys, error) == 1) {
				return (EXIT_FAILURE);
			}
		}

		if (edit_choice_clean(aux->parent, children, model, nacm, error) == EXIT_FAILURE) {
			return (EXIT_FAILURE);
		}

		children = children->next;
	}
	/* remove the node from the edit document */
	edit_delete(edit_node);

	return EXIT_SUCCESS;
}

/**
 * \brief Perform all the edit-config's operations specified in the edit_doc document.
 *
 * \param[in] orig_doc Original configuration document to edit.
 * \param[in] edit_doc XML document covering edit-config's \<config\> element
 *                     supposed to edit orig_doc configuration data.
 * \param[in] defop Default edit-config's operation for this edit-config call.
 * \param[in] model XML form (YIN) of the configuration data model appropriate
 * to the given configuration data.
 * \param[out] err NETCONF error structure.
 *
 * \return On error, non-zero is returned and err structure is filled. Zero is
 *         returned on success.
 */
static int edit_operations(xmlDocPtr orig_doc, xmlDocPtr edit_doc, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr model, const struct nacm_rpc* nacm, struct nc_err **error)
{
	xmlXPathObjectPtr nodes;
	int i;
	char *msg = NULL;
	xmlNodePtr orig_node, edit_node;
	keyList keys;

	keys = get_keynode_list(model);

	if (error != NULL) {
		*error = NULL;
	}

	/* default replace */
	if (defop == NC_EDIT_DEFOP_REPLACE) {
		/* replace whole document */
		for (edit_node = edit_doc->children; edit_node != NULL; edit_node = edit_doc->children) {
			edit_replace(orig_doc, edit_node, defop, model, keys, nacm, error);
		}

		/* according to RFC 6020 sec. 7.2, default-operation "replace"
		 * completely replaces data in target datastore, so we are done here
		 */
		goto cleanup;
	}

	/* delete operations */
	nodes = get_operation_elements(NC_EDIT_OP_DELETE, edit_doc);
	if (nodes != NULL) {
		if (!xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
			/* something to delete */
			for (i = 0; i < nodes->nodesetval->nodeNr; i++) {
				edit_node = nodes->nodesetval->nodeTab[i];
				orig_node = find_element_equiv(orig_doc, edit_node, model, keys);
				if (orig_node == NULL) {
					xmlXPathFreeObject(nodes);
					if (error != NULL) {
						*error = nc_err_new(NC_ERR_DATA_MISSING);
					}
					goto error;
				}
				for (; orig_node != NULL; orig_node = find_element_equiv(orig_doc, edit_node, model, keys)) {
					/* NACM */
					if (nacm_check_data(orig_node, NACM_ACCESS_DELETE, nacm) == NACM_PERMIT) {
						/* remove the edit node's equivalent from the original document */
						edit_delete(orig_node);
					} else {
						if (error != NULL ) {
							*error = nc_err_new(NC_ERR_ACCESS_DENIED);
							if (asprintf(&msg, "deleting \"%s\" data node is not permitted.", (char*) (orig_node->name)) != -1) {
								nc_err_set(*error, NC_ERR_PARAM_MSG, msg);
								free(msg);
							}
						}
						xmlXPathFreeObject(nodes);
						goto error;
					}
				}
				/* remove the node from the edit document */
				edit_delete(edit_node);
				nodes->nodesetval->nodeTab[i] = NULL;
			}
		}
		xmlXPathFreeObject(nodes);
	}

	/* remove operations */
	nodes = get_operation_elements(NC_EDIT_OP_REMOVE, edit_doc);
	if (nodes != NULL) {
		if (!xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
			/* something to remove */
			for (i = 0; i < nodes->nodesetval->nodeNr; i++) {
				if (edit_remove(orig_doc, nodes->nodesetval->nodeTab[i], model, keys, nacm, error) != EXIT_SUCCESS) {
					xmlXPathFreeObject(nodes);
					goto error;
				}
				nodes->nodesetval->nodeTab[i] = NULL;
			}
		}
		xmlXPathFreeObject(nodes);
	}

	/* replace operations */
	nodes = get_operation_elements(NC_EDIT_OP_REPLACE, edit_doc);
	if (nodes != NULL) {
		if (!xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
			/* something to replace */
			for (i = 0; i < nodes->nodesetval->nodeNr; i++) {
				if (edit_replace(orig_doc, nodes->nodesetval->nodeTab[i], defop, model, keys, nacm, error) != EXIT_SUCCESS) {
					xmlXPathFreeObject(nodes);
					goto error;
				}
				nodes->nodesetval->nodeTab[i] = NULL;
			}
		}
		xmlXPathFreeObject(nodes);
	}

	/* create operations */
	nodes = get_operation_elements(NC_EDIT_OP_CREATE, edit_doc);
	if (nodes != NULL) {
		if (!xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
			/* something to create */
			for (i = 0; i < nodes->nodesetval->nodeNr; i++) {
				if (edit_create(orig_doc, nodes->nodesetval->nodeTab[i], defop, model, keys, nacm, error) != EXIT_SUCCESS) {
					xmlXPathFreeObject(nodes);
					goto error;
				}
				nodes->nodesetval->nodeTab[i] = NULL;
			}
		}
		xmlXPathFreeObject(nodes);
	}

	/* merge operations */
	nodes = get_operation_elements(NC_EDIT_OP_MERGE, edit_doc);
	if (nodes != NULL) {
		if (!xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
			/* something to create */
			for (i = 0; i < nodes->nodesetval->nodeNr; i++) {
				if (edit_merge(orig_doc, nodes->nodesetval->nodeTab[i], defop, model, keys, nacm, error) != EXIT_SUCCESS) {
					xmlXPathFreeObject(nodes);
					goto error;
				}
				nodes->nodesetval->nodeTab[i] = NULL;
			}
		}
		xmlXPathFreeObject(nodes);
	}

	/* default merge */
	if (defop == NC_EDIT_DEFOP_MERGE || defop == NC_EDIT_DEFOP_NOTSET) {
		/* replace whole document */
		if (edit_doc->children != NULL) {
			for (edit_node = edit_doc->children; edit_node != NULL; edit_node = edit_doc->children) {
				if (edit_merge(orig_doc, edit_doc->children, defop, model, keys, nacm, error) != EXIT_SUCCESS) {
					goto error;
				}
			}
		}
	}

cleanup:

	if (keys != NULL) {
		keyListFree(keys);
	}

	return EXIT_SUCCESS;

error:

	if (keys != NULL ) {
		keyListFree(keys);
	}

	if (error != NULL && *error == NULL) {
		*error = nc_err_new(NC_ERR_OP_FAILED);
	}

	return EXIT_FAILURE;
}

static int compact_edit_operations_recursively(xmlNodePtr node, NC_EDIT_OP_TYPE supreme_op)
{
	NC_EDIT_OP_TYPE op;
	xmlNodePtr children;
	int ret;

	assert(node);

	op = get_operation(node, NC_EDIT_DEFOP_NOTSET, NULL);
	switch ((int)op) {
	case NC_EDIT_OP_ERROR:
		return EXIT_FAILURE;
		break;
	case 0:
		/* no operation defined -> go recursively, but use supreme
		 * operation, it may be the default operation and in such a case
		 * remove it */
		op = supreme_op;
		break;
	default:
		/* any operation specified */
		if (op == supreme_op) {
			/* operation duplicity -> remove subordinate duplicated operation */
			/* remove operation attribute */
			xmlRemoveProp(xmlHasNsProp(node, BAD_CAST NC_EDIT_ATTR_OP, BAD_CAST NC_NS_BASE));
			nc_clear_namespaces(node);
		}
		break;
	}

	/* go recursive */
	for (children = node->children; children != NULL; children = children->next) {
		ret = compact_edit_operations_recursively(children, op);
		if (ret == EXIT_FAILURE) {
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

static int compact_edit_operations(xmlDocPtr edit_doc, NC_EDIT_DEFOP_TYPE defop)
{
	xmlNodePtr root;
	NC_EDIT_OP_TYPE op;

	if (edit_doc == NULL) {
		return EXIT_FAILURE;
	}

	/* to start recursive check, use defop as root's supreme operation */
	for (root = edit_doc->children; root != NULL; root = root->next) {
		if (root->type != XML_ELEMENT_NODE) {
			continue;
		}

		switch (defop) {
		case NC_EDIT_DEFOP_NOTSET:
		case NC_EDIT_DEFOP_MERGE:
			op = NC_EDIT_OP_MERGE;
			break;
		case NC_EDIT_DEFOP_REPLACE:
			op = NC_EDIT_OP_REPLACE;
			break;
		case NC_EDIT_DEFOP_NONE:
			op = NC_EDIT_OP_NOTSET;
			break;
		default:
			return EXIT_FAILURE;
		}

		if (compact_edit_operations_recursively(root, op) != EXIT_SUCCESS) {
			return (EXIT_FAILURE);
		}
	}
	return EXIT_SUCCESS;
}

static int check_list_keys(xmlDocPtr edit, xmlDocPtr model, struct nc_err **error)
{
	xmlNodePtr listdef;
	xmlNodePtr *keys = NULL;
	xmlNodePtr node, next;
	keyList modelkeys;
	int ret = EXIT_SUCCESS;
	int i;

	modelkeys = get_keynode_list(model);
	if (!modelkeys) {
		/* no keys in the model */
		return ret;
	}

	node = xmlDocGetRootElement(edit);
	while (node) {
		if ((listdef = is_list(node, model)) != NULL) {
			for (i = 0; i < modelkeys->nodesetval->nodeNr; i++) {
				if (modelkeys->nodesetval->nodeTab[i]->parent == listdef) {
					break;
				}
			}

			if (i < modelkeys->nodesetval->nodeNr) {
				/* find out if all the keys are present in edit data */
				if (find_key_elems(modelkeys->nodesetval->nodeTab[i], node, 1, &keys)) {
					ret = EXIT_FAILURE;
					goto cleanup;
				}
				free(keys);
				keys = NULL;

			} /* else list has no keys */
		}

		/* go to the next element to process (depth-first processing) */
		/* children first */
		next = node->children;
		if (next) {
			/* skip comments and other garbage */
			while(next && next->type != XML_ELEMENT_NODE) {
				next = next->next;
			}
		}

		if (!next) {
			/* try siblings */
			next = node->next;
			while(next && next->type != XML_ELEMENT_NODE) {
				next = next->next;
			}

			while(!next) {
				/* go back through parents */
				if (node->parent == (xmlNodePtr)node->doc) {
					/* we are done */
					goto cleanup;
				}
				node = node->parent;

				/* the parent is already checked, so go to its next sibling */
				next = node->next;
				while(next && next->type != XML_ELEMENT_NODE) {
					next = next->next;
				}
			}
		}
		node = next;
	}

cleanup:

	keyListFree(modelkeys);

	if (ret && error != NULL) {
		*error = nc_err_new(NC_ERR_MISSING_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, (char*)node->name);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "A list key is missing.");
	}

	return ret;
}

/**
 * \brief Perform edit-config changes according to the given parameters
 *
 * \param[in] repo XML document to change (target NETCONF repository).
 * \param[in] edit Content of the edit-config's \<config\> element as an XML
 * document defining the changes to perform.
 * \param[in] ds Datastore structure where the edit-config will be performed.
 * \param[in] defop Default edit-config's operation for this edit-config call.
 * \param[in] errop NETCONF edit-config's error option defining reactions to an error.
 * \param[in] nacm NACM structure of the request RPC to check Access Rights
 * \param[out] err NETCONF error structure.
 * \return On error, non-zero is returned and err structure is filled. Zero is
 * returned on success.
 */
int edit_config(xmlDocPtr repo, xmlDocPtr edit, struct ncds_ds* ds, NC_EDIT_DEFOP_TYPE defop, NC_EDIT_ERROPT_TYPE UNUSED(errop), const struct nacm_rpc* nacm, struct nc_err **error)
{
	if (repo == NULL || edit == NULL) {
		return (EXIT_FAILURE);
	}

	/* check validity - for list instances, all keys must be present */
	if (check_list_keys(edit, ds->ext_model, error) != EXIT_SUCCESS) {
		goto error_cleanup;
	}
	/* check operations */
	if (check_edit_ops(NC_CHECK_EDIT_DELETE, defop, repo, edit, ds->ext_model, error) != EXIT_SUCCESS) {
		goto error_cleanup;
	}
	if (check_edit_ops(NC_CHECK_EDIT_CREATE, defop, repo, edit, ds->ext_model, error) != EXIT_SUCCESS) {
		goto error_cleanup;
	}

	if (compact_edit_operations(edit, defop) != EXIT_SUCCESS) {
		ERROR("Compacting edit-config operations failed.");
		if (error != NULL) {
			*error = nc_err_new(NC_ERR_OP_FAILED);
		}
		goto error_cleanup;
	}

	/* perform operations */
	if (edit_operations(repo, edit, defop, ds->ext_model, nacm, error) != EXIT_SUCCESS) {
		goto error_cleanup;
	}

	/* with defaults capability */
	if (ncdflt_get_basic_mode() == NCWD_MODE_TRIM) {
		/* server work in trim basic mode and therefore all default
		 * values must be removed from the datastore.
		 */
		ncdflt_default_values(repo, ds->ext_model, NCWD_MODE_TRIM);
	}

	return EXIT_SUCCESS;

error_cleanup:

	return EXIT_FAILURE;
}

