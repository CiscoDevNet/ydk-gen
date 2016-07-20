#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "netconf_internal.h"
#include "xmldiff.h"
#include "../transapi.h"
#include "yinparser.h"
#include "transapi_internal.h"

/* adds a priority into priority buffer structure */
static void xmldiff_add_priority(int prio, struct xmldiff_prio** prios)
{
	int *new_values;

	if (*prios == NULL) {
		*prios = malloc(sizeof(struct xmldiff_prio));
		(*prios)->used = 0;
		(*prios)->alloc = 10;
		(*prios)->values = malloc(10 * sizeof(int));
	} else if ((*prios)->used == (*prios)->alloc) {
		(*prios)->alloc *= 2;
		new_values = realloc((*prios)->values, (*prios)->alloc * sizeof(int));
		if (new_values == NULL) {
			ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
			(*prios)->alloc /= 2;
			return;
		}
		(*prios)->values = new_values;
	}

	(*prios)->values[(*prios)->used] = prio;
	(*prios)->used += 1;
}

/* appends two priority structures, handy for merging all children priorities into one for the parent */
static void xmldiff_merge_priorities(struct xmldiff_prio** old, struct xmldiff_prio* new)
{
	int *new_values;
	size_t alloc;

	if (new == NULL || *old == NULL) {
		if (*old == NULL) {
			*old = new;
		}
		return;
	}

	if ((*old)->alloc - (*old)->used < new->used) {
		alloc = (*old)->alloc;
		(*old)->alloc = (*old)->used + new->used + 10;
		new_values = realloc((*old)->values, (*old)->alloc * sizeof(int));
		if (new_values == NULL) {
			ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
			(*old)->alloc = alloc;
			return;
		}
		(*old)->values = new_values;
	}

	memcpy((*old)->values+(*old)->used, new->values, new->used * sizeof(int));
	(*old)->used += new->used;

	free(new->values);
	free(new);
}

/* the recursive core of xmldiff_set_priorities() function */
static struct xmldiff_prio* xmldiff_set_priority_recursive(struct xmldiff_tree* tree, struct clbk *callbacks, int clbk_count)
{
	int i, min_prio, children_count = 0, children_without_callback = 0;
	struct xmldiff_prio* priorities = NULL, *tmp_prio;
	struct xmldiff_tree* child;

	/* First search for the callbacks of our children */
	child = tree->children;
	while (child != NULL) {
		++children_count;
		tmp_prio = xmldiff_set_priority_recursive(child, callbacks, clbk_count);
		if (tmp_prio == NULL) {
			++children_without_callback;
		}
		xmldiff_merge_priorities(&priorities, tmp_prio);
		child = child->next;
	}

	/* Fix XMLDIFF_OP */
	if (tree->op & XMLDIFF_CHAIN) {
		if (children_count == 0) {
			/* Cannot happen */
		} else if (children_without_callback == 0) {
			/* All of our children have a callback -> XMLDIFF_CHAIN stays */
		} else if (children_count > children_without_callback) {
			/* Some children have a callback, some don't -> XMLDIFF_CHAIN | XMLDIFF_MOD */
			tree->op |= XMLDIFF_MOD;
		} else { /* (children == children_without_callback) */
			/* No child has a callback -> XMLDIFF_MOD */
			tree->op = XMLDIFF_MOD;
		}
	} else { /* XMLDIFF_ADD or XMLDIFF_REM */
		if (children_count == 0) {
			/* XMLDIFF_OP is correct */
		} else if (children_without_callback == 0) {
			/* All of our children have a callback -> XMLDIFF_CHAIN | previous op */
			tree->op |= XMLDIFF_CHAIN;
		} else if (children_count > children_without_callback) {
			/* Some children have a callback, chain should be set -> XMLDIFF_CHAIN | previous op */
			tree->op |= XMLDIFF_CHAIN;
		} else { /* (children == children_without_callback) */
			/* No child has a callback -> only the previous op */
		}
	}

	/* Search for the callback */
	for (i = 0; i < clbk_count; i++) {
		if (strcmp(callbacks[i].path, tree->path) == 0) {
			/* We have a callback */
			tree->callback = callbacks[i].func;
			tree->priority = i;

			/* Save our priority */
			xmldiff_add_priority(tree->priority, &priorities);

			/* stop the loop */
			break;
		}
	}

	if (tree->callback == NULL && priorities != NULL) {
		/* We do not have a callback, so we set the lowest priority from our children callbacks */
		min_prio = priorities->values[0];
		for (i = 1; (unsigned int) i < priorities->used; ++i) {
			if (priorities->values[i] < min_prio) {
				min_prio = priorities->values[i];
			}
		}

		tree->priority = min_prio;
	} else {
		/* We do not have a callback and neither does any of our children, maybe our parent does */
	}

	return priorities;
}

int xmldiff_set_priorities(struct xmldiff_tree* tree, struct clbk *callbacks, int clbk_count)
{
	struct xmldiff_prio* ret;
	struct xmldiff_tree* iter;

	for (iter = tree; iter != NULL; iter = iter->next) {
		ret = xmldiff_set_priority_recursive(iter, callbacks, clbk_count);

		/* There is no callback to call for the configuration change, that probably should not happen */
		if (ret == NULL) {
			return EXIT_FAILURE;
		}

		free(ret->values);
		free(ret);
	}
	return EXIT_SUCCESS;
}

/**
 * @brief Destroy and free whole xmldiff_tree structure
 *
 * @param diff	pointer to xmldiff structure
 */
void xmldiff_free(struct xmldiff_tree* diff)
{
	if (diff == NULL) {
		return;
	}

	/* siblings */
	xmldiff_free(diff->next);
	/* children */
	xmldiff_free(diff->children);
	/* itself */
	free(diff->path);
	free(diff);
}

/**
 * @brief Add single diff record
 *
 * @param diff	pointer to xmldiff structure
 * @param[in] path	path in XML document where the change occurs
 * @param[in]	op	change type
 *
 * return EXIT_SUCCESS or EXIT_FAILURE
 */
static void xmldiff_add_diff(struct xmldiff_tree** diff, const char * path, xmlNodePtr old_node, xmlNodePtr new_node, XMLDIFF_OP op, XML_RELATION rel)
{
	struct xmldiff_tree* new, *cur;

	new = malloc(sizeof(struct xmldiff_tree));
	memset(new, 0, sizeof(struct xmldiff_tree));

	new->path = strdup(path);
	new->old_node = old_node;
	new->new_node = new_node;
	new->op = op;
	new->applied = CLBCKS_APPLIED_NONE;
	new->priority = PRIORITY_NONE;

	if (*diff == NULL) {
		*diff = new;
	} else {
		switch (rel) {
		case XML_PARENT:
			/* (*diff) is our parent */
			(*diff)->parent = new;
			cur = (*diff)->next;
			while (cur != NULL) {
				cur->parent = new;
				cur = cur->next;
			}
			new->children = *diff;
			break;

		case XML_CHILD:
			/* (*diff) is our child */
			if ((*diff)->children == NULL) {
				(*diff)->children = new;
			} else {
				cur = (*diff)->children;
				while (cur->next != NULL) {
					cur = cur->next;
				}
				cur->next = new;
			}
			new->parent = *diff;
			break;

		case XML_SIBLING:
			/* (*diff) is our sibling */
			cur = *diff;
			while (cur->next != NULL) {
				cur = cur->next;
			}
			cur->next = new;
			new->parent = cur->parent;
			break;
		}
	}
}

static void xmldiff_addsibling_diff(struct xmldiff_tree** siblings, struct xmldiff_tree** new_sibling)
{
	struct xmldiff_tree* last_sibling;

	if (*siblings == NULL) {
		*siblings = *new_sibling;
		return;
	}

	last_sibling = *siblings;
	while (last_sibling->next != NULL) {
		last_sibling = last_sibling->next;
	}

	last_sibling->next = *new_sibling;
	(*new_sibling)->parent = last_sibling->parent;
}

/**
 * @brief Add diff for all descendants of the node
 */
static void xmldiff_add_diff_recursive(struct xmldiff_tree **diff, const char *path, xmlNodePtr old_node, xmlNodePtr new_node, XMLDIFF_OP op, XML_RELATION rel, struct model_tree * model)
{
	char * tmp_path;
	struct xmldiff_tree * last_diff;
	xmlNodePtr tmp;
	int i, j;
	struct model_tree * model_child = NULL, *empty_child;
	static int level = 0;

	if (level == 0) {
		xmldiff_add_diff(diff, path, old_node, new_node, op, rel);
		switch (rel) {
		case XML_PARENT:
			last_diff = (*diff)->parent;
			break;
		case XML_CHILD:
			last_diff = (*diff)->children;
			while (last_diff->next) {
				last_diff = last_diff->next;
			}
			break;
		case XML_SIBLING:
			last_diff = (*diff);
			while (last_diff->next) {
				last_diff = last_diff->next;
			}
			break;
		}
	} else {
		xmldiff_add_diff(diff, path, old_node, new_node, op, XML_CHILD);
		last_diff = (*diff)->children;
		while (last_diff->next) {
			last_diff = last_diff->next;
		}
	}

	++level;

	/* op is either XMLDIFF_ADD or XMLDIFF_REM => either old_node or new_node are NULL */
	tmp = (old_node == NULL ? new_node->children : old_node->children);
	while(tmp) {
		if (tmp->type == XML_ELEMENT_NODE) {
			for (i = 0; i < model->children_count; i++) {
				if (model->children[i].type == YIN_TYPE_AUGMENT || model->children[i].type == YIN_TYPE_CHOICE) {
					/* AUGMENT and CHOICE nodes are actually empty, go through its children */
					empty_child = model->children[i].children;
					for (j = 0; j < model->children[i].children_count; j++) {
						/* search for the correct model node */
						if (xmlStrEqual(BAD_CAST empty_child[j].name, tmp->name)) {
							if ((empty_child[j].ns_uri == NULL && (tmp->ns == NULL || tmp->ns->href == NULL)) ||
								((empty_child[j].ns_uri != NULL && tmp->ns != NULL && tmp->ns->href != NULL) &&
								 xmlStrEqual(BAD_CAST empty_child[j].ns_uri, tmp->ns->href))) {
								model_child = &empty_child[j];
								break;
							}
						}
					}
				} else if (xmlStrEqual(BAD_CAST model->children[i].name, tmp->name)) {
					/* check for the correct model node */
					if ((model->children[i].ns_uri == NULL && (tmp->ns == NULL || tmp->ns->href == NULL)) ||
						((model->children[i].ns_uri != NULL && tmp->ns != NULL && tmp->ns->href != NULL) &&
						 xmlStrEqual(BAD_CAST model->children[i].ns_uri, tmp->ns->href))) {
						model_child = &model->children[i];
					}
				}

				/* are we done for the current tmp? */
				if (model_child) {
					if (asprintf(&tmp_path, "%s/%s:%s", path, model_child->ns_prefix, model_child->name) == -1) {
						ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
					} else {
						xmldiff_add_diff_recursive(&last_diff, tmp_path, (old_node == NULL ? NULL : tmp), (new_node == NULL ? NULL : tmp), op, XML_CHILD, model_child);
						free(tmp_path);
					}
					model_child = NULL;
					/* yes, we're done, leave the for loop and go for another tmp */
					break;
				}
			}
		}
		tmp = tmp->next;
	}

	--level;

}

/**
 * @brief Return EXIT_SUCCESS if node1 and node2 have same name and are in the same namespace.
 *
 * This function is used in xmldiff_recursive() for determining that we have
 * found equivalent node in the other document. Only name and namespace is checked
 * because in xmldiff_recursive() is always assured that we are on the same place in
 * both documents.
 *
 * @param node1	One node to compare.
 * @param node2	Other node to compare.
 *
 * @return EXIT_SUCCESS when equivalent EXIT_FAILURE otherwise
 */
static int node_cmp(xmlNodePtr node1, xmlNodePtr node2)
{
	int ret = EXIT_FAILURE;

	if (node1 != NULL && node2 != NULL) { /* valid nodes */
		if (xmlStrEqual(node1->name, node2->name)) { /*	with same name */
			if (node1->ns == node2->ns) {/* namespace is identical (single object referenced by both nodes) on both NULL */
				ret = EXIT_SUCCESS;
			} else if ((node1->ns == NULL || node2->ns == NULL) || (node1->ns->href == NULL || node2->ns->href == NULL))  { /* one of nodes has no namespace */
				ret = EXIT_FAILURE;
			} else if (xmlStrEqual(node1->ns->href, node2->ns->href)) {
				ret = EXIT_SUCCESS;
			}
		}
	}

	return(ret);
}

/*
 * @brief Return EXIT_SUCCESS if node1 and node2 have same name, are in the same namespace and
 * have the same key values.
 *
 * @param node1	One node to compare.
 * @param node2	Other node to compare.
 *
 * @return EXIT_SUCCESS when equivalent EXIT_FAILURE otherwise
 */
static int list_node_cmp(xmlNodePtr node1, xmlNodePtr node2, struct model_tree * model)
{
	int i, ret = EXIT_FAILURE;
	xmlNodePtr node_tmp;
	xmlChar * tmp_str, *node1_keys, *node2_keys, *new_keys;

	if (node_cmp(node1, node2) == EXIT_SUCCESS) {
		/* For every old node create string holding the concatenated key values */
		node1_keys = BAD_CAST strdup ("");
		node2_keys = BAD_CAST strdup ("");
		for (i=0; i<model->keys_count; i++) { /* For every specified key */
			node_tmp = node1->children;
			while (node_tmp) {
				if (xmlStrEqual(node_tmp->name, BAD_CAST model->keys[i])) { /* Find matching leaf in old document */
					tmp_str = xmlNodeGetContent (node_tmp);
					new_keys  = realloc (node1_keys, sizeof(char) * (strlen((const char*)node1_keys)+strlen((const char*)tmp_str)+1));
					if (new_keys == NULL) {
						ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
						ret = EXIT_FAILURE;
						goto cleanup;
					}
					node1_keys = new_keys;
					strcat((char*)node1_keys, (char*)tmp_str); /* Concatenate key value */
					xmlFree(tmp_str);
					break;
				}
				node_tmp = node_tmp->next;
			}
			node_tmp = node2->children;
			while (node_tmp) {
				if (xmlStrEqual(node_tmp->name, BAD_CAST model->keys[i])) { /* Find matching leaf in old document */
					tmp_str = xmlNodeGetContent (node_tmp);
					new_keys = realloc (node2_keys, sizeof(char) * (strlen((const char*)node2_keys)+strlen((const char*)tmp_str)+1));
					if (new_keys == NULL) {
						ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
						ret = EXIT_FAILURE;
						goto cleanup;
					}
					node2_keys = new_keys;
					strcat ((char*)node2_keys, (char*)tmp_str); /* Concatenate key value */
					xmlFree (tmp_str);
					break;
				}
				node_tmp = node_tmp->next;
			}
		}
		if (xmlStrEqual(node1_keys, node2_keys)) {
			ret = EXIT_SUCCESS;
		}

cleanup:
		xmlFree(node1_keys);
		xmlFree(node2_keys);
	}

	return(ret);
}

static XMLDIFF_OP xmldiff_list(struct xmldiff_tree** diff, char * path, xmlNodePtr old_tmp, xmlNodePtr new_tmp, struct model_tree * model);
static XMLDIFF_OP xmldiff_leaflist(struct xmldiff_tree** diff, char * path, xmlNodePtr old_tmp, xmlNodePtr new_tmp, struct model_tree * model);

/**
 * @brief Recursively go through documents and search for differences. Build
 *		a difference tree starting with leaves.
 *
 * @param diff	returned difference tree, should be NULL when first passed
 * @param path	path of the model node passed
 * @param old_node	current node (or sibling) in the old configuration
 * @param new_node	current node (or sibling) in the new configuration
 * @param model	current node in the model
 */
static XMLDIFF_OP xmldiff_recursive(struct xmldiff_tree** diff, char * path, xmlNodePtr old_node, xmlNodePtr new_node, struct model_tree * model)
{
	char * next_path;
	xmlNodePtr old_tmp, new_tmp;
	XMLDIFF_OP tmp_op, ret_op = XMLDIFF_NONE;
	xmlChar * old_content, * new_content;
	xmlChar * old_str, *new_str;
	xmlBufferPtr buf;
	struct xmldiff_tree** tmp_diff;
	int i;

	/* Some of the required documents are missing */
	if (model == NULL) {
		ERROR("%s: invalid parameter \"model\".", __func__);
		return XMLDIFF_ERR;
	}

	/* Find the node from the model in the old configuration */
	for (old_tmp = old_node; old_tmp != NULL; old_tmp = old_tmp->next) {
		if (xmlStrEqual(old_tmp->name, BAD_CAST model->name)) {
			if (old_tmp->ns == NULL) {
				WARN("Node \"%s\" from the current config does not have any namespace!", (char*)old_tmp->name);
				break;
			}

			if (old_tmp->ns->href != NULL && model->ns_uri != NULL && xmlStrEqual(old_tmp->ns->href, BAD_CAST model->ns_uri)) {
				break;
			}
		}
	}

	/* Find the node from the model in the new configuration */
	for (new_tmp = new_node; new_tmp != NULL; new_tmp = new_tmp->next) {
		if (xmlStrEqual(new_tmp->name, BAD_CAST model->name)) {
			if (new_tmp == NULL) {
				WARN("Node \"%s\" from the new config does not have any namespace!", (char*)new_tmp->name);
				break;
			}

			if (new_tmp->ns->href != NULL && model->ns_uri != NULL && xmlStrEqual(new_tmp->ns->href, BAD_CAST model->ns_uri)) {
				break;
			}
		}
	}

	if (new_tmp == NULL && old_tmp == NULL && model->type != YIN_TYPE_CHOICE && model->type != YIN_TYPE_AUGMENT) {
		return XMLDIFF_NONE;
	}

	/* Check for internal changes */
	switch (model->type) {
	/* -- CONTAINER -- */
	case YIN_TYPE_CONTAINER:
		if (old_tmp == NULL) {
			ret_op = XMLDIFF_ADD;
		} else if (new_tmp == NULL) {
			ret_op = XMLDIFF_REM;
		} else {
			ret_op = XMLDIFF_NONE;
		}
		tmp_diff = malloc(sizeof(struct xmldiff_tree*));
		*tmp_diff = NULL;
		tmp_op = XMLDIFF_NONE;
		for (i = 0; i < model->children_count; i++) {
			if (asprintf(&next_path, "%s/%s:%s", path, model->children[i].ns_prefix, model->children[i].name) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				free(tmp_diff);
				return (XMLDIFF_ERR);
			}
			tmp_op = xmldiff_recursive(tmp_diff, next_path, (old_tmp ? old_tmp->children : NULL), (new_tmp ? new_tmp->children : NULL), &model->children[i]);
			free(next_path);

			if (tmp_op == XMLDIFF_ERR) {
				free(tmp_diff);
				return (XMLDIFF_ERR);
			}
			if (tmp_op & XMLDIFF_SIBLING) {
				ret_op |= XMLDIFF_REORDER;
			}
			if ((tmp_op & (XMLDIFF_ADD | XMLDIFF_REM | XMLDIFF_MOD | XMLDIFF_REORDER | XMLDIFF_CHAIN)) && !(ret_op & (XMLDIFF_ADD | XMLDIFF_REM))) {
				ret_op |= XMLDIFF_CHAIN;
			}
		}
		if (ret_op != XMLDIFF_NONE) {
			if (ret_op & XMLDIFF_REM) {
				xmldiff_add_diff(tmp_diff, path, old_tmp, new_tmp, ret_op, XML_PARENT);
			} else {
				xmldiff_add_diff(tmp_diff, path, old_tmp, new_tmp, ret_op, XML_PARENT);
			}
			if ((*tmp_diff) && (*tmp_diff)->parent) {
				*tmp_diff = (*tmp_diff)->parent;
			}
			xmldiff_addsibling_diff(diff, tmp_diff);
		}
		free(tmp_diff);
		break;

	/* -- CHOICE -- */
	case YIN_TYPE_CHOICE:
	case YIN_TYPE_AUGMENT:
		ret_op = XMLDIFF_NONE;
		/* Trim the choice path, replace it with the children directly */
		*strrchr(path, '/') = '\0';

		for (i = 0; i < model->children_count; i++) {
			if (asprintf(&next_path, "%s/%s:%s", path, model->children[i].ns_prefix, model->children[i].name) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				return (XMLDIFF_ERR);
			}
			/* We are moving down the model only (not in the configuration) */
			tmp_op = xmldiff_recursive(diff, next_path, old_node, new_node, &model->children[i]);
			free(next_path);

			if (tmp_op == XMLDIFF_ERR) {
				return (XMLDIFF_ERR);
			} else if (tmp_op != XMLDIFF_NONE) {
				ret_op |= tmp_op;
			}
		}

		break;

	/* -- LEAF -- */
	case YIN_TYPE_LEAF:
		if (old_tmp == NULL) {
			ret_op = XMLDIFF_ADD;
			xmldiff_add_diff(diff, path, old_tmp, new_tmp, XMLDIFF_ADD, XML_SIBLING);
			break;
		} else if (new_tmp == NULL) {
			ret_op = XMLDIFF_REM;
			xmldiff_add_diff(diff, path, old_tmp, new_tmp, XMLDIFF_REM, XML_SIBLING);
			break;
		}
		old_content = xmlNodeGetContent(old_tmp);
		new_content = xmlNodeGetContent(new_tmp);
		if (xmlStrEqual(old_content, new_content)) {
			ret_op = XMLDIFF_NONE;
		} else {
			ret_op = XMLDIFF_MOD;
			xmldiff_add_diff(diff, path, old_tmp, new_tmp, XMLDIFF_MOD, XML_SIBLING);
		}
		xmlFree(old_content);
		xmlFree(new_content);
		break;

	/* -- LIST -- */
	case YIN_TYPE_LIST:
		ret_op = xmldiff_list(diff, path, old_tmp, new_tmp, model);
		break;

	/* -- LEAFLIST -- */
	case YIN_TYPE_LEAFLIST:
		ret_op = xmldiff_leaflist(diff, path, old_tmp, new_tmp, model);
		break;

	/* -- ANYXML -- */
	case YIN_TYPE_ANYXML:
		/* Serialize and compare strings */
		/* TODO: find better solution in future */
		if (old_tmp == NULL) {
			ret_op = XMLDIFF_ADD;
			xmldiff_add_diff(diff, path, old_tmp, new_tmp, XMLDIFF_ADD, XML_SIBLING);
		} else if (new_tmp == NULL) {
			ret_op = XMLDIFF_REM;
			xmldiff_add_diff(diff, path, old_tmp, new_tmp, XMLDIFF_REM, XML_SIBLING);
		}

		buf = xmlBufferCreate();
		xmlNodeDump(buf, old_tmp->doc, old_tmp, 0, 0);
		old_str = xmlStrdup(xmlBufferContent(buf));
		xmlBufferEmpty(buf);
		xmlNodeDump(buf, new_tmp->doc, new_tmp, 0, 0);
		new_str = xmlStrdup(xmlBufferContent(buf));
		xmlBufferFree(buf);

		if (xmlStrEqual(old_str, new_str)) {
			ret_op = XMLDIFF_NONE;
		} else {
			xmldiff_add_diff(diff, path, old_tmp, new_tmp, XMLDIFF_MOD, XML_SIBLING);
			ret_op = XMLDIFF_CHAIN;
		}
		xmlFree(old_str);
		xmlFree(new_str);
		break;

	default:
		/* No other type is supported now */
		break;
	}

	return ret_op;
}

static XMLDIFF_OP xmldiff_list(struct xmldiff_tree** diff, char * path, xmlNodePtr old_tmp, xmlNodePtr new_tmp, struct model_tree * model)
{
	XMLDIFF_OP item_ret_op, tmp_op, ret_op = XMLDIFF_NONE;
	xmlNodePtr* list_added = NULL, *list_removed = NULL, *realloc_tmp;
	xmlNodePtr list_old_tmp, list_new_tmp, list_old_inter, list_new_inter;
	xmlChar* old_keys, *new_keys, *old_str, *new_str, *aux_str;
	struct xmldiff_tree** tmp_diff;
	int i, list_added_cnt = 0, list_removed_cnt = 0;
	char* next_path;

	/* Find matches according to the key elements, process all the elements inside recursively */
	/* Not matching are _ADD or _REM */
	/* Maching are _NONE or _CHAIN, according to the return values of the recursive calls */

	/* ---REM--- Go through the old nodes and search for matching nodes in the new document*/
	list_old_tmp = old_tmp;
	while (list_old_tmp) {
		/* We have to make sure that this really is a list node we are checking now */
		if (node_cmp(old_tmp, list_old_tmp)) {
			list_old_tmp = list_old_tmp->next;
			continue;
		}

		item_ret_op = XMLDIFF_NONE;
		/* For every old node create string holding the concatenated key values */
		old_keys = BAD_CAST strdup("");
		for (i=0; i<model->keys_count; i++) { /* For every specified key */
			list_old_inter = list_old_tmp->children;
			while (list_old_inter) {
				if (xmlStrEqual(list_old_inter->name, BAD_CAST model->keys[i])) { /* Find matching leaf in old document */
					old_str = xmlNodeGetContent(list_old_inter);
					aux_str  = realloc(old_keys, sizeof(xmlChar) * (xmlStrlen(old_keys)+xmlStrlen(old_str)+1));
					if (aux_str == NULL) {
						ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
						xmlFree(old_str);
						xmlFree(old_keys);
						return (XMLDIFF_ERR);
					}
					old_keys = aux_str;
					strcat((char*)old_keys, (char*)old_str); /* Concatenate key value */
					xmlFree(old_str);
					break;
				}
				list_old_inter = list_old_inter->next;
			}
		}

		/* Go through the new list */
		list_new_tmp = new_tmp;
		while (list_new_tmp) {
			if (node_cmp(old_tmp, list_new_tmp)) {
				list_new_tmp = list_new_tmp->next;
				continue;
			}

			new_keys = BAD_CAST strdup("");
			for (i = 0; i < model->keys_count; i++) {
				list_new_inter = list_new_tmp->children;
				while (list_new_inter) {
					if (xmlStrEqual(list_new_inter->name, BAD_CAST model->keys[i])) {
						new_str = xmlNodeGetContent(list_new_inter);
						aux_str  = realloc(new_keys, sizeof(xmlChar) * (xmlStrlen(new_keys)+xmlStrlen(new_str)+1));
						if (aux_str == NULL) {
							ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
							xmlFree(new_keys);
							xmlFree(new_str);
							return (XMLDIFF_ERR);
						}
						new_keys = aux_str;
						strcat((char*)new_keys, (char*)new_str);
						xmlFree(new_str);
						break;
					}
					list_new_inter = list_new_inter->next;
				}
			}
			if (strcmp((const char*)old_keys, (const char*)new_keys) == 0) { /* Matching item found */
				xmlFree(new_keys);
				break;
			}
			xmlFree(new_keys);
			list_new_tmp = list_new_tmp->next;
		}
		xmlFree(old_keys);

		if (list_new_tmp == NULL) { /* Item NOT found in the new document -> removed */
			xmldiff_add_diff_recursive(diff, path, list_old_tmp, list_new_tmp, XMLDIFF_REM, XML_SIBLING, model);
			ret_op = XMLDIFF_REM;
			/* Remember that the node was removed */
			if ((realloc_tmp = realloc(list_removed, ++list_removed_cnt * sizeof(xmlNodePtr))) == NULL) {
				ERROR("Memory reallocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				free(list_removed);
				return (XMLDIFF_ERR);
			} else {
				list_removed = realloc_tmp;
				list_removed[list_removed_cnt-1] = list_old_tmp;
			}
		} else { /* Item found -> check for changes recursively */
			tmp_diff = malloc(sizeof(struct xmldiff_tree*));
			*tmp_diff = NULL;
			for (i = 0; i < model->children_count; i++) {
				if (asprintf(&next_path, "%s/%s:%s", path, model->children[i].ns_prefix, model->children[i].name) == -1) {
					ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
					free(tmp_diff);
					return (XMLDIFF_ERR);
				}
				tmp_op = xmldiff_recursive(tmp_diff, next_path, list_old_tmp->children, list_new_tmp->children, &model->children[i]);
				free(next_path);

				if (tmp_op == XMLDIFF_ERR) {
					free(tmp_diff);
					return (XMLDIFF_ERR);
				} else {
					item_ret_op |= tmp_op;
				}
			}

			if (item_ret_op != XMLDIFF_NONE) {
				/* There actually was a change, so we append those changes as our children and add our change as a sibling */
				if (item_ret_op & XMLDIFF_SIBLING) {
					ret_op |= XMLDIFF_REORDER;
				}
				if (item_ret_op & (XMLDIFF_ADD | XMLDIFF_REM | XMLDIFF_MOD | XMLDIFF_REORDER | XMLDIFF_CHAIN)) {
					ret_op |= XMLDIFF_CHAIN;
				}
				if (item_ret_op & XMLDIFF_REM) {
					xmldiff_add_diff(tmp_diff, path, list_old_tmp, list_new_tmp, ret_op, XML_PARENT);
				} else {
					xmldiff_add_diff(tmp_diff, path, list_old_tmp, list_new_tmp, ret_op, XML_PARENT);
				}
				*tmp_diff = (*tmp_diff)->parent;
				xmldiff_addsibling_diff(diff, tmp_diff);
			}
			free(tmp_diff);
		}
		list_old_tmp = list_old_tmp->next;
	}

	/* ---ADD--- Go through the new nodes and search for matching nodes in the old document */
	list_new_tmp = new_tmp;
	while (list_new_tmp) {
		if (node_cmp(new_tmp, list_new_tmp)) {
			list_new_tmp = list_new_tmp->next;
			continue;
		}

		item_ret_op = XMLDIFF_NONE;
		/* For every new node create string holding the concatenated key values */
		new_keys = BAD_CAST strdup ("");
		for (i = 0; i < model->keys_count; i++) { /* For every specified key */
			list_new_inter = list_new_tmp->children;
			while (list_new_inter) {
				if (xmlStrEqual(list_new_inter->name, BAD_CAST model->keys[i])) { /* Find matching leaf in the old document */
					new_str = xmlNodeGetContent(list_new_inter);
					aux_str = realloc(new_keys, sizeof(xmlChar*) * (xmlStrlen(new_keys)+xmlStrlen(new_str)+1));
					if (aux_str == NULL) {
						ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
						xmlFree(new_keys);
						xmlFree(new_str);
						return (XMLDIFF_ERR);
					}
					new_keys = aux_str;
					strcat((char*)new_keys, (char*)new_str); /* Concatenate key value */
					xmlFree(new_str);
					break;
				}
				list_new_inter = list_new_inter->next;
			}
		}
		/* Go through the new list */
		list_old_tmp = old_tmp;
		while (list_old_tmp) {
			if (node_cmp(new_tmp, list_old_tmp)) {
				list_old_tmp = list_old_tmp->next;
				continue;
			}

			old_keys = BAD_CAST strdup ("");
			for (i = 0; i < model->keys_count; i++) {
				list_old_inter = list_old_tmp->children;
				while (list_old_inter) {
					if (xmlStrEqual(list_old_inter->name, BAD_CAST model->keys[i])) {
						old_str = xmlNodeGetContent(list_old_inter);
						aux_str = realloc(old_keys, sizeof(xmlChar) * (xmlStrlen(old_keys)+xmlStrlen(old_str)+1));
						if (aux_str == NULL) {
							ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
							xmlFree(old_keys);
							xmlFree(old_str);
							return (XMLDIFF_ERR);
						}
						old_keys = aux_str;
						strcat((char*)old_keys, (char*)old_str);
						xmlFree(old_str);
						break;
					}
					list_old_inter = list_old_inter->next;
				}
			}
			if (strcmp((const char*)old_keys, (const char*)new_keys) == 0) { /* Matching item found */
				free(old_keys);
				break;
			}
			xmlFree(old_keys);
			list_old_tmp = list_old_tmp->next;
		}
		xmlFree(new_keys);

		if (list_old_tmp == NULL) { /* Item NOT found in the old document -> added */
			xmldiff_add_diff_recursive(diff, path, list_old_tmp, list_new_tmp, XMLDIFF_ADD, XML_SIBLING, model);
			ret_op = XMLDIFF_ADD;
			/* Remember that the node was added */
			if ((realloc_tmp = realloc(list_added, ++list_added_cnt * sizeof(xmlNodePtr))) == NULL) {
				ERROR("Memory reallocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				free(list_added);
				return (XMLDIFF_ERR);
			} else {
				list_added = realloc_tmp;
				list_added[list_added_cnt-1] = list_new_tmp;
			}
		} else {
			/* We already checked for changes in these nodes */
		}
		list_new_tmp = list_new_tmp->next;
	}

	/* list is ordered by user */
	if (model->ordering == YIN_ORDER_USER) {

		/* Go through old and new list and compare pairs */
		list_old_tmp = old_tmp;
		list_new_tmp = new_tmp;

		while (list_old_tmp && list_new_tmp) {
			/* Nodes are not part of the list we are now processing */
			if (!xmlStrEqual(list_old_tmp->name, BAD_CAST model->name)) {
				list_old_tmp = list_old_tmp->next;
				continue;
			}
			if (!xmlStrEqual(list_new_tmp->name, BAD_CAST model->name)) {
				list_new_tmp = list_new_tmp->next;
				continue;
			}

			/* Wasn't the old node removed and that's why it isn't in the new config? */
			for (i = 0; i < list_removed_cnt; ++i) {
				if (list_old_tmp == list_removed[i]) {
					break;
				}
			}
			if (i != list_removed_cnt) {
				list_old_tmp = list_old_tmp->next;
				continue;
			}

			/* Wasn't the new node added and that's why it isn't in the old config? */
			for (i = 0; i < list_added_cnt; ++i) {
				if (list_new_tmp == list_added[i]) {
					break;
				}
			}
			if (i != list_added_cnt) {
				list_new_tmp = list_new_tmp->next;
				continue;
			}

			/* We have to make sure these two nodes are not equal */
			if (list_node_cmp(list_old_tmp, list_new_tmp, model) != 0) {
				ret_op |= XMLDIFF_SIBLING;
				xmldiff_add_diff(diff, path, list_old_tmp, list_new_tmp, XMLDIFF_SIBLING, XML_SIBLING);
			}

			list_old_tmp = list_old_tmp->next;
			list_new_tmp = list_new_tmp->next;
		}
	}

	free(list_added);
	free(list_removed);
	return ret_op;
}

static XMLDIFF_OP xmldiff_leaflist(struct xmldiff_tree** diff, char * path, xmlNodePtr old_tmp, xmlNodePtr new_tmp, struct model_tree * model)
{
	XMLDIFF_OP ret_op = XMLDIFF_NONE;
	char* list_name = strrchr(path, ':')+1;
	xmlNodePtr* list_added = NULL, *list_removed = NULL, *realloc_tmp;
	xmlNodePtr list_old_tmp, list_new_tmp;
	xmlChar* new_str, *old_str;
	int i, list_added_cnt = 0, list_removed_cnt = 0;

	/* Search for matches, only _ADD and _REM will be here */
	/* For each in the old node find one from the new nodes or log as _REM */
	list_old_tmp = old_tmp;
	while (list_old_tmp) {
		if (!xmlStrEqual(BAD_CAST list_name, list_old_tmp->name)) {
			list_old_tmp = list_old_tmp->next;
			continue;
		}
		old_str = xmlNodeGetContent(list_old_tmp);
		list_new_tmp = new_tmp;
		while (list_new_tmp) {
			if (!xmlStrEqual(BAD_CAST list_name, list_new_tmp->name)) {
				list_new_tmp = list_new_tmp->next;
				continue;
			}
			new_str = xmlNodeGetContent(list_new_tmp);
			if (xmlStrEqual(old_str, new_str)) {
				xmlFree(new_str);
				/* Equivalent found */
				break;
			}
			xmlFree(new_str);
			list_new_tmp = list_new_tmp->next;
		}
		xmlFree(old_str);
		if (list_new_tmp == NULL) {
			xmldiff_add_diff(diff, path, list_old_tmp, list_new_tmp, XMLDIFF_REM, XML_SIBLING);
			ret_op = XMLDIFF_REM;
			/* Remember that the node was removed */
			if ((realloc_tmp = realloc(list_removed, ++list_removed_cnt * sizeof(xmlNodePtr))) == NULL) {
				ERROR("Memory reallocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				free(list_removed);
				return (XMLDIFF_ERR);
			} else {
				list_removed = realloc_tmp;
				list_removed[list_removed_cnt-1] = list_old_tmp;
			}
		}
		list_old_tmp = list_old_tmp->next;
	}

	/* For each in the new node find one from the old nodes or log as _ADD */
	list_new_tmp = new_tmp;
	while (list_new_tmp) {
		if (!xmlStrEqual(BAD_CAST list_name, list_new_tmp->name)) {
			list_new_tmp = list_new_tmp->next;
			continue;
		}
		new_str = xmlNodeGetContent(list_new_tmp);
		list_old_tmp = old_tmp;
		while (list_old_tmp) {
			if (!xmlStrEqual(BAD_CAST list_name, list_old_tmp->name)) {
				list_old_tmp = list_old_tmp->next;
				continue;
			}
			old_str = xmlNodeGetContent(list_old_tmp);
			if (xmlStrEqual(old_str, new_str)) {
				xmlFree(old_str);
				/* Equivalent found */
				break;
			}
			xmlFree(old_str);
			list_old_tmp = list_old_tmp->next;
		}
		xmlFree(new_str);
		if (list_old_tmp == NULL) {
			xmldiff_add_diff(diff, path, list_old_tmp, list_new_tmp, XMLDIFF_ADD, XML_SIBLING);
			ret_op = XMLDIFF_ADD;
			/* remeber that the node was added*/
			if ((realloc_tmp = realloc(list_added, ++list_added_cnt * sizeof(xmlNodePtr))) == NULL) {
				ERROR("Memory reallocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				free(list_added);
				return (XMLDIFF_ERR);
			} else {
				list_added = realloc_tmp;
				list_added[list_added_cnt-1] = list_new_tmp;
			}
		}
		list_new_tmp = list_new_tmp->next;
	}

	/* leaf-list is ordered by user */
	if (model->ordering == YIN_ORDER_USER) {

		/* Go through old and new list and compare pairs */
		list_old_tmp = old_tmp;
		list_new_tmp = new_tmp;

		while (list_old_tmp && list_new_tmp) {
			/* Nodes are not part of the leaf-list we are now processing */
			if (!xmlStrEqual(list_old_tmp->name, BAD_CAST model->name)) {
				list_old_tmp = list_old_tmp->next;
				continue;
			}
			if (!xmlStrEqual(list_new_tmp->name, BAD_CAST model->name)) {
				list_new_tmp = list_new_tmp->next;
				continue;
			}

			/* Wasn't the old node removed and that's why it isn't in the new config? */
			for (i = 0; i < list_removed_cnt; ++i) {
				if (list_old_tmp == list_removed[i]) {
					break;
				}
			}
			if (i != list_removed_cnt) {
				list_old_tmp = list_old_tmp->next;
				continue;
			}

			/* Wasn't the new node added and that's why it isn't in the old config? */
			for (i = 0; i < list_added_cnt; ++i) {
				if (list_new_tmp == list_added[i]) {
					break;
				}
			}
			if (i != list_added_cnt) {
				list_new_tmp = list_new_tmp->next;
				continue;
			}

			/* We have to make sure these two nodes are not equal */
			if (xmlStrcmp(list_old_tmp->children->content, list_new_tmp->children->content) != 0) {
				ret_op |= XMLDIFF_SIBLING;
				xmldiff_add_diff(diff, path, list_old_tmp, list_new_tmp, XMLDIFF_SIBLING, XML_SIBLING);
			}

			list_old_tmp = list_old_tmp->next;
			list_new_tmp = list_new_tmp->next;
		}
	}

	free(list_added);
	free(list_removed);
	return ret_op;
}

/**
 * @brief Module top level function
 *
 * @param old		old version of XML document
 * @param new		new version of XML document
 * @param model	data model in YANG format
 *
 * @return xmldiff structure holding all differences between XML documents or NULL
 */
XMLDIFF_OP xmldiff_diff(struct xmldiff_tree** diff, xmlDocPtr old, xmlDocPtr new, struct model_tree * model)
{
	char* path;
	XMLDIFF_OP ret_op = XMLDIFF_NONE;
	int i;

	if (old == NULL || new == NULL || diff == NULL || model == NULL) {
		ERROR("%s: invalid parameter \"%s\".", __func__, !old ? "old" : !new ? "new" : !diff ? "diff" : "model");
		return XMLDIFF_ERR;
	}

	for (i = 0; i < model->children_count; i++) {
		if (asprintf(&path, "/%s:%s", model->children[i].ns_prefix, model->children[i].name) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (XMLDIFF_ERR);
		}
		ret_op = xmldiff_recursive(diff, path, old->children, new->children, &model->children[i]);
		free(path);
	}

	return (ret_op);
}
