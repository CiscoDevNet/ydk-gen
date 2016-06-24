/**
 * \file with_defaults.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of NETCONF's with-defaults capability defined in RFC 6243
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
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "datastore/edit_config.h"
#include "with_defaults.h"
#include "netconf_internal.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

/*
 * From internal.c to be used by ncdflt_set_*() functions to detect if the
 * with-default capability is enabled
 */
extern int nc_init_flags;

static NCWD_MODE ncdflt_basic_mode = NCWD_MODE_NOTSET;
static NCWD_MODE ncdflt_supported = NCWD_MODE_NOTSET;

API NCWD_MODE ncdflt_get_basic_mode(void)
{
	return (ncdflt_basic_mode);
}

API void ncdflt_set_basic_mode(NCWD_MODE mode)
{
	if (nc_init_flags & NC_INIT_WD) {
		/* if one of valid values, change the value */
		if (mode == NCWD_MODE_ALL
				|| mode == NCWD_MODE_TRIM
				|| mode == NCWD_MODE_EXPLICIT) {
			/* set basic mode */
			ncdflt_basic_mode = mode;

			/* if current basic mode is not in the supported set, add it */
			if ((ncdflt_supported & ncdflt_basic_mode) == 0) {
				ncdflt_supported |= ncdflt_basic_mode;
			}
		}
	}
}

API void ncdflt_set_supported(NCWD_MODE modes)
{
	if (nc_init_flags & NC_INIT_WD) {
		ncdflt_supported = ncdflt_basic_mode;
		ncdflt_supported |= (modes & NCWD_MODE_ALL) ? NCWD_MODE_ALL : 0;
		ncdflt_supported |= (modes & NCWD_MODE_ALL_TAGGED) ? NCWD_MODE_ALL_TAGGED : 0;
		ncdflt_supported |= (modes & NCWD_MODE_TRIM) ? NCWD_MODE_TRIM : 0;
		ncdflt_supported |= (modes & NCWD_MODE_EXPLICIT) ? NCWD_MODE_EXPLICIT : 0;
	}
}

API NCWD_MODE ncdflt_get_supported(void)
{
	return (ncdflt_supported);
}

API NCWD_MODE ncdflt_rpc_get_withdefaults(const nc_rpc* rpc)
{
	return (rpc->with_defaults);
}

/*
 * 0 - no match
 * 1 - match found
 */
static int search_choice_match(xmlNodePtr parent, xmlChar* name)
{
	xmlNodePtr aux;

	/* go through all existing elements on the appropriate level in the
	 * configuration data to check if the currently processed default
	 * value is created and the node with default value is supposed to
	 * be created
	 */
	for (aux = parent->children; aux != NULL; aux = aux->next) {
		if (aux->type != XML_ELEMENT_NODE) {
			continue;
		}

		if (xmlStrcmp(aux->name, name) == 0) {
			/* we have a match */
			return (1);
		}
	}

	return (0);
}

static xmlChar* check_default_case(xmlNodePtr config_choice, xmlNodePtr model_choice)
{
	xmlNodePtr def, aux_model, case_child;
	xmlChar *name;

	/*
	 * for all cases if the model, check that there is no case in config
	 * and then return name of the default case
	 */

	/* first, get the name of default case, if no defined, terminate */
	for (def = model_choice->children; def != NULL; def = def->next) {
		if (def->type != XML_ELEMENT_NODE || xmlStrcmp(def->name, BAD_CAST "default")) {
			continue;
		}
		/* default definition */
		break;
	}
	if (def == NULL) {
		return (NULL);
	}

	/* now search for any existing case */
	for (aux_model = model_choice->children; aux_model != NULL; aux_model = aux_model->next) {
		if (aux_model->type != XML_ELEMENT_NODE || xmlStrcmp(aux_model->name, BAD_CAST "case")) {
			continue;
		}
		for (case_child = aux_model->children; case_child != NULL; case_child = case_child->next) {
			if (case_child->type != XML_ELEMENT_NODE) {
				continue;
			}

			if ((xmlStrcmp(case_child->name, BAD_CAST "anyxml") == 0) ||
					(xmlStrcmp(case_child->name, BAD_CAST "container") == 0) ||
					(xmlStrcmp(case_child->name, BAD_CAST "leaf") == 0) ||
					(xmlStrcmp(case_child->name, BAD_CAST "list") == 0) ||
					(xmlStrcmp(case_child->name, BAD_CAST "leaf-list") == 0)) {
				name = xmlGetProp(case_child, BAD_CAST "name");
				if (search_choice_match(config_choice, name) == 1) {
					xmlFree(name);
					return (NULL);
				}
				xmlFree(name);
			}
		}
	}

	return(xmlGetProp(def, BAD_CAST "value"));
}

static xmlNodePtr* fill_default(xmlDocPtr config, xmlNodePtr node, const char* namespace, NCWD_MODE mode)
{
	xmlNodePtr *parents = NULL, *retvals = NULL, *aux_nodeptr;
	xmlNodePtr aux = NULL;
	xmlNsPtr ns;
	xmlChar* value = NULL, *name, *value2;
	int i, j, k, size = 0, first_call = 0;
	static xmlNodePtr *created_local = NULL;
	static int created_count = 0;
	static int created_size = 0;

	if (mode == NCWD_MODE_NOTSET || mode == NCWD_MODE_EXPLICIT) {
		return (NULL);
	}

	if (xmlStrcmp(node->name, BAD_CAST "default") == 0 && xmlStrcmp(node->parent->name, BAD_CAST "choice") == 0) {
		/* skip default branches of a choice */
		return (NULL);
	}

	if (created_local == NULL) {
		/* initial (not recursive) call */
		first_call = 1;
		created_count = 0;
		created_size = 32;
		created_local = malloc(created_size * sizeof(xmlNodePtr));
		created_local[created_count] = NULL; /* list terminating byte */
	}

	/* do recursion */
	if (node->parent == NULL) {
		if (first_call) {
			if (retvals == NULL) {
				for(i = created_count-1; i >= 0; i--) {
					if (created_local[i]->children == NULL) {
						/* created parent element, but default value was not finally
						 * created and no other children element exists -> remove
						 * the created element
						 */
						xmlUnlinkNode(created_local[i]);
						xmlFreeNode(created_local[i]);
					}
				}
			}
			/* free in last recursion call */
			created_count = 0;
			free(created_local);
			created_local = NULL;
		}
		return (NULL);
	} else if (xmlStrcmp(node->parent->name, BAD_CAST "module") != 0) {
		/* we will get parent of the config's equivalent of the node */
		parents = fill_default(config, node->parent, namespace, mode);

		if (parents && xmlStrcmp(node->parent->name, BAD_CAST "choice") == 0) {
			/* process choices */

			if (xmlStrcmp(node->name, BAD_CAST "case") == 0) {
				/*
				 * if there is case defined, we have to skip it and search for
				 * its subelements since case node itself is not present in
				 * configuration data
				 */

				/* so do it for all given parent nodes */
				for (i = 0; parents[i] != NULL; i++) {
					/*
					 * at least one of the case children is supposed to be
					 * present, if no such element is found, the parent is
					 * removed from further processing
					 */
					for (aux = node->children; aux != NULL; aux = aux->next) {
						if (aux->type != XML_ELEMENT_NODE) {
							continue;
						}

						if ((xmlStrcmp(aux->name, BAD_CAST "anyxml") == 0) ||
								(xmlStrcmp(aux->name, BAD_CAST "container") == 0) ||
								(xmlStrcmp(aux->name, BAD_CAST "leaf") == 0) ||
								(xmlStrcmp(aux->name, BAD_CAST "list") == 0) ||
								(xmlStrcmp(aux->name, BAD_CAST "leaf-list") == 0)) {
							value = xmlGetProp(aux, BAD_CAST "name");
							if (search_choice_match(parents[i], value) != 0) {
								xmlFree(value);
								break;
							}
							xmlFree(value);
						}
					}
					if (aux == NULL) {
						/* match not found */

						/* check default case */
						if ((value = check_default_case(parents[i], node->parent)) != NULL) {
							/*
							 * there is no case in config data and default case
							 * is specified in the model (named value), check
							 * if it is the currently processed node
							 */
							name = xmlGetProp(node, BAD_CAST "name");
							if (xmlStrcmp(value, name) != 0) {
								/*
								 * we are in a different case, remove parent
								 * from further processing
								 */
								parents[i] = NULL;
							}

							/* cleanup */
							xmlFree(value);
							xmlFree(name);

						} else {
							/*
							 * there is already some case in config data or no
							 * default case is specified. Remove parent from
							 * further processing
							 */
							parents[i] = NULL;
						}
					}
				}
			} else {
				/*
				 * if there is no case, the element itself is supposed to be
				 * present in the configuration data, so search for it
				 */
				value = xmlGetProp(node, BAD_CAST "name");
				for (i = 0; parents[i] != NULL; i++) {
					if (search_choice_match(parents[i], value) == 0) {
						name = check_default_case(parents[i], node->parent);
						if (name == NULL || xmlStrcmp(name, value) != 0) {
							/* we are not in a default branch */
							parents[i] = NULL;
						}
					}
				}
				xmlFree(value);
			}

			/* consolidate parents */
			for (j = 0, k = 0; k < i; k++) {
				if (parents[k]) {
					parents[j] = parents[k];
					j++;
				}
			}
			if (j == 0) {
				free(parents);
				parents = NULL;
			}

		}

		/* if we are in augment or choice node, just go through */
		if (((xmlStrcmp(node->name, BAD_CAST "augment") == 0) ||
				(xmlStrcmp(node->name, BAD_CAST "choice") == 0) ||
				(xmlStrcmp(node->name, BAD_CAST "case") == 0)) &&
				(xmlStrcmp(node->ns->href, BAD_CAST NC_NS_YIN) == 0)) {
			return (parents);
		}
	} else {
		/* we are in the root */
		aux = config->children;
		switch (mode) {
		case NCWD_MODE_ALL:
		case NCWD_MODE_IMPL_TAGGED:
		case NCWD_MODE_ALL_TAGGED:
			/* return root element, create it if it does not exist */
			retvals = (xmlNodePtr*) malloc(2 * sizeof(xmlNodePtr));
			if (retvals == NULL) {
				ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				goto cleanup;
			}
			retvals[1] = NULL;
			/* create root element */
			name = xmlGetProp(node, BAD_CAST "name");
			/* search in all the root elements */
			for (; aux != NULL; aux = aux->next) {
				if (xmlStrcmp(aux->name, name) == 0) {
					break;
				}
			}
			if (aux == NULL) {
				aux = xmlNewNode(NULL, name);
				if (config->children == NULL) {
					xmlDocSetRootElement(config, aux);
				} else {
					xmlAddSibling(config->children, aux);
				}
				/* set namespace */
				ns = xmlNewNs(aux, BAD_CAST namespace, NULL);
				xmlSetNs(aux, ns);

				/* remember created node, for later remove if no default child will be created */
				if (created_count == created_size-1) {
					/* (re)allocate created list */
					created_size += 32;
					aux_nodeptr = realloc(created_local, created_size * sizeof(xmlNodePtr));
					if (aux_nodeptr == NULL) {
						ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
						free(retvals);
						goto cleanup;
					}
					created_local = aux_nodeptr;
				}
				created_local[created_count++] = aux;
				created_local[created_count] = NULL; /* list terminating byte */
			}
			xmlFree(name);

			/* if report-all-tagged, add namespace for default attribute into the whole doc */
			if (mode == NCWD_MODE_ALL_TAGGED || mode == NCWD_MODE_IMPL_TAGGED) {
				xmlNewNs(aux, BAD_CAST "urn:ietf:params:xml:ns:netconf:default:1.0", BAD_CAST "wd");
			}

			retvals[0] = aux;
			return (retvals);
			break;
		case NCWD_MODE_TRIM:
			/* search in all the root elements */
			name = xmlGetProp(node, BAD_CAST "name");
			for (; aux != NULL; aux = aux->next) {
				if (xmlStrcmp(aux->name, name) == 0) {
					break;
				}
			}
			/* return root element, do not create it if it does not exist */
			if (aux != NULL) {
				retvals = (xmlNodePtr*) malloc(2 * sizeof(xmlNodePtr));
				if (retvals == NULL) {
					ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
					xmlFree(name);
					goto cleanup;
				}
				retvals[0] = aux;
				retvals[1] = NULL;
			}
			xmlFree(name);
			return (retvals);
			break;
		default:
			/* remove compiler warnings, but do nothing */
			break;
		}
	}
	if (parents == NULL) {
		if (first_call) {
			if (retvals == NULL) {
				for(i = created_count-1; i >= 0; i--) {
					if (created_local[i]->children == NULL) {
						/* created parent element, but default value was not finally
						 * created and no other children element exists -> remove
						 * the created element
						 */
						xmlUnlinkNode(created_local[i]);
						xmlFreeNode(created_local[i]);
					}
				}
			}
			/* free in last recursion call */
			free(created_local);
			created_local = NULL;
			created_count = 0;
		}
		return (NULL);
	}

	for (i = 0, j = 0; parents[i] != NULL; i++) {
		if (xmlStrcmp(node->name, BAD_CAST "default") == 0) {
			switch (mode) {
			case NCWD_MODE_ALL:
			case NCWD_MODE_IMPL_TAGGED:
			case NCWD_MODE_ALL_TAGGED:
				/* we are at the end - set default content if needed */
				value = xmlGetProp(node, BAD_CAST "value");
				if (parents[i]->children == NULL) {
					/* element is empty -> fill it with the default value */
					xmlNodeSetContent(parents[i], value);
				} /* else do nothing, configuration data contain (non-)default value */

				if (mode == NCWD_MODE_ALL_TAGGED ||
						(mode == NCWD_MODE_IMPL_TAGGED && created_count)) { /* the default node is not explicit from datastore */
					value2 = xmlNodeGetContent(parents[i]);
					if (xmlStrcmp(value, value2) == 0) {
						/* add default attribute if element has default value */
						aux = xmlDocGetRootElement(config);
						for (ns = aux->nsDef; ns != NULL; ns = ns->next) {
							if (xmlStrcmp(ns->href, BAD_CAST "urn:ietf:params:xml:ns:netconf:default:1.0") == 0) {
								break;
							}
						}
						xmlNewNsProp(parents[i], ns, BAD_CAST "default", BAD_CAST "true");
					}
					xmlFree(value2);
				}

				xmlFree(value);
				/* continue to another parent node in the list to process */
				break;
			case NCWD_MODE_TRIM:
				/* we are at the end - remove element if it contains default value */
				if (parents[i]->children != NULL) {
					value = xmlGetProp(node, BAD_CAST "value");
					value2 = xmlNodeGetContent(parents[i]);
					if (xmlStrcmp(value, value2) == 0) {
						/* element contains default value, remove it */
						xmlUnlinkNode(parents[i]);
						xmlFreeNode(parents[i]);
					}
					xmlFree(value);
					xmlFree(value2);
				}
				break;
			default:
				/* remove compiler warnings, but do nothing */
				break;
			}
		} else {
			/* remember the number of retvals */
			k = j;
			/* find node's equivalents in config */
			name = xmlGetProp(node, BAD_CAST "name");
			for(aux = parents[i]->children; aux != NULL; aux = aux->next) {
				if (xmlStrcmp(aux->name, name) == 0) {
					/* remember the node */
					if (size <= j+1) {
						/* (re)allocate retvals list */
						size += 32;
						aux_nodeptr = (xmlNodePtr*) realloc(retvals, size * sizeof(xmlNodePtr));
						if (aux_nodeptr == NULL) {
							ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
							/* we're in real troubles here */
							free(retvals);
							return (NULL);
						}
						retvals = aux_nodeptr;
					}

					retvals[j] = aux;
					j++;
					retvals[j] = NULL; /* list terminating NULL */
				}
			}

			switch (mode) {
			case NCWD_MODE_ALL:
			case NCWD_MODE_IMPL_TAGGED:
			case NCWD_MODE_ALL_TAGGED:
				if (k == j && xmlStrcmp(node->name, BAD_CAST "list") != 0) {
					/* check, that new node is not a "presence" element */
					for (aux = node->children; aux != NULL; aux = aux->next) {
						if (xmlStrcmp(aux->name, BAD_CAST "presence") == 0) {
							break;
						}
					}
					if (aux != NULL) {
						/* presence definition found -> leave the case */
						break;
					}
					/* no new equivalent node found -> create one, but only if it is not supposed to be list */
					if (size <= j + 1) {
						/* (re)allocate retvals list */
						size += 32;
						aux_nodeptr = (xmlNodePtr*) realloc(retvals, size * sizeof(xmlNodePtr));
						if (aux_nodeptr == NULL) {
							ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
							/* we're in real troubles here */
							free(retvals);
							return (NULL);
						}
						retvals = aux_nodeptr;
					}
					/* no new equivalent node found -> create one */
					retvals[j] = xmlNewChild(parents[i], parents[i]->ns, name, NULL);
                    /* augment need update the namespace */
                    if (xmlStrcmp(node->parent->name, BAD_CAST "augment") == 0) {
                        xmlChar *uri = xmlGetProp(node->parent, BAD_CAST "ns");
                        if (uri) {
                            xmlSetNs(retvals[j], xmlNewNs(retvals[j], BAD_CAST uri, NULL));
                            free(uri);
                        }
                    }
					j++;
					retvals[j] = NULL; /* list terminating NULL */

					/* remember created node, for later remove if no default child will be created */
					if (created_count == created_size-1) {
						/* (re)allocate created list */
						created_size += 32;
						aux_nodeptr = realloc(created_local, created_size * sizeof(xmlNodePtr));
						if (aux_nodeptr == NULL) {
							ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
							free(retvals);
							goto cleanup;
						}
						created_local = aux_nodeptr;
					}
					created_local[created_count++] = retvals[j-1];
					created_local[created_count] = NULL; /* list terminating byte */
				}
				break;
			case NCWD_MODE_TRIM:
				/* nothing needed */
				break;
			default:
				/* remove compiler warnings, but do nothing */
				break;
			}
			xmlFree(name);
		}
	}
	if (parents != NULL) {
		free(parents);
	}

cleanup:
	if (first_call) {
		if (retvals == NULL) {
			for(i = created_count-1; i >= 0; i--) {
				if (created_local[i]->children == NULL) {
					/* created parent element, but default value was not finally
					 * created and no other children element exists -> remove
					 * the created element
					 */
					xmlUnlinkNode(created_local[i]);
					xmlFreeNode(created_local[i]);
				}
			}
		}
		/* free in last recursion call */
		free(created_local);
		created_local = NULL;
		created_count = 0;
		created_size = 0;
	}

	return (retvals);
}

int ncdflt_default_values(xmlDocPtr config, const xmlDocPtr model, NCWD_MODE mode)
{
	xmlXPathContextPtr model_ctxt = NULL;
	xmlXPathObjectPtr defaults = NULL, query;
	xmlNodePtr root;
	xmlChar* namespace = NULL;
	int i;

	if (config == NULL || model == NULL) {
		return (EXIT_FAILURE);
	}

	if (mode == NCWD_MODE_NOTSET || mode == NCWD_MODE_EXPLICIT) {
		/* nothing to do */
		return (EXIT_SUCCESS);
	}

	/* create xpath evaluation context */
	if ((model_ctxt = xmlXPathNewContext(model)) == NULL) {
		WARN("%s: Creating the XPath context failed.", __func__);
		/* with-defaults cannot be found */
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(model_ctxt, BAD_CAST "yin", BAD_CAST NC_NS_YIN) != 0) {
		ERROR("%s: Registering yin namespace for the model xpath context failed.", __func__);
		xmlXPathFreeContext(model_ctxt);
		return (EXIT_FAILURE);
	}
	if ((query = xmlXPathEvalExpression(BAD_CAST "/yin:module/yin:namespace", model_ctxt)) == NULL) {
		ERROR("%s: Unable to get namespace from the data model.", __func__);
		xmlXPathFreeContext(model_ctxt);
		return (EXIT_FAILURE);
	}
	if (xmlXPathNodeSetIsEmpty(query->nodesetval) || (namespace = xmlGetProp(query->nodesetval->nodeTab[0], BAD_CAST "uri")) == NULL) {
		ERROR("%s: Unable to get namespace from the data model.", __func__);
		xmlFree(namespace);
		xmlXPathFreeObject(query);
		xmlXPathFreeContext(model_ctxt);
		return (EXIT_FAILURE);
	}
	xmlXPathFreeObject(query);

	if ((defaults = xmlXPathEvalExpression(BAD_CAST "/yin:module/yin:container//yin:default", model_ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(defaults->nodesetval)) {
			/* if report-all-tagged, add namespace for default attribute into the whole doc */
			root = xmlDocGetRootElement(config);
			if ((mode & (NCWD_MODE_ALL_TAGGED | NCWD_MODE_IMPL_TAGGED)) && root != NULL) {
				xmlNewNs(root, BAD_CAST "urn:ietf:params:xml:ns:netconf:default:1.0", BAD_CAST "wd");
			}
			/* process all defaults elements */
			for (i = 0; i < defaults->nodesetval->nodeNr; i++) {
				if (xmlStrcmp(defaults->nodesetval->nodeTab[i]->parent->name, BAD_CAST "choice") == 0) {
					/* skip defaults for choices */
					continue;
				}
				fill_default(config, defaults->nodesetval->nodeTab[i], (char*)namespace, mode);
			}
		}
		xmlXPathFreeObject(defaults);
	}
	xmlFree(namespace);
	xmlXPathFreeContext(model_ctxt);

	return (EXIT_SUCCESS);
}

int ncdflt_default_clear(xmlDocPtr config)
{
	xmlXPathContextPtr ctxt = NULL;
	xmlXPathObjectPtr defaults = NULL;
	xmlNodePtr aux;
	int i;

	if (config == NULL) {
		return (EXIT_FAILURE);
	}

	if (xmlDocGetRootElement(config) == NULL) {
		/* nothing to do */
		return (EXIT_SUCCESS);
	}

	/* create xpath evaluation context */
	if ((ctxt = xmlXPathNewContext(config)) == NULL) {
		WARN("%s: Creating the XPath context failed.", __func__);
		/* with-defaults cannot be found */
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(ctxt, BAD_CAST "wd", BAD_CAST "urn:ietf:params:xml:ns:netconf:default:1.0") != 0) {
		xmlXPathFreeContext(ctxt);
		return (EXIT_FAILURE);
	}
	defaults = xmlXPathEvalExpression(BAD_CAST "//*[@wd:default=\"true\"]", ctxt);
	if (defaults != NULL) {
		/* remove them */
		for (i = 0; i < defaults->nodesetval->nodeNr; i++) {
			/* element contain default value, remove it */
			for (aux = defaults->nodesetval->nodeTab[i]; aux->parent != NULL && (void*)aux->parent != (void*)aux->doc; aux = aux->parent) {
				if (aux->next || aux->prev) {
					/* the node has a sibling, so we don't remove also its parent */
					break;
				}
			}
			xmlUnlinkNode(aux);
			xmlFreeNode(aux);
			defaults->nodesetval->nodeTab[i] = NULL;
		}

		xmlXPathFreeObject(defaults);
	}
	xmlXPathFreeContext(ctxt);

	return (EXIT_SUCCESS);
}

int ncdflt_edit_remove_default(xmlDocPtr config, const xmlDocPtr model)
{
	xmlXPathContextPtr ctxt = NULL;
	xmlXPathObjectPtr defaults = NULL;
	xmlNodePtr model_node, aux;
	xmlChar* value_data, *value_model;
	xmlNsPtr ns;
	int i, retval = EXIT_SUCCESS;

	if (config == NULL || model == NULL) {
		return (EXIT_FAILURE);
	}

	/* create xpath evaluation context */
	if ((ctxt = xmlXPathNewContext(config)) == NULL) {
		WARN("%s: Creating the XPath context failed.", __func__);
		/* with-defaults cannot be found */
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(ctxt, BAD_CAST "wd", BAD_CAST "urn:ietf:params:xml:ns:netconf:default:1.0") != 0) {
		xmlXPathFreeContext(ctxt);
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(ctxt, BAD_CAST "data", BAD_CAST (xmlDocGetRootElement(config))->ns->href) != 0) {
		xmlXPathFreeContext(ctxt);
		return (EXIT_FAILURE);
	}
	defaults = xmlXPathEvalExpression(BAD_CAST "//data:*[@wd:default=\"true\"]", ctxt);
	if (defaults != NULL) {
		/* RFC 6243 requires us to check, that the data contains correct default values */
		for (i = 0; i < defaults->nodesetval->nodeNr; i++) {
			if ((model_node = find_element_model(defaults->nodesetval->nodeTab[i], model)) == NULL) {
				/* node with default attribute not defined in data model */
				return (EXIT_FAILURE);
			}
			/* get default value from model ... */
			value_model = NULL;
			for (aux = model_node->children; aux != NULL; aux = aux->next) {
				if (aux->type != XML_ELEMENT_NODE || xmlStrcmp(aux->name, BAD_CAST "default") != 0) {
					continue;
				}
				value_model = xmlGetProp(aux, BAD_CAST "value");
				break;
			}
			if (value_model == NULL) {
				return (EXIT_FAILURE);
			}
			/* ... and value from configuration data ... */
			value_data = xmlNodeGetContent(defaults->nodesetval->nodeTab[i]);
			if (value_data == NULL) {
				xmlFree(value_data);
				return (EXIT_FAILURE);
			}
			/* ... and compare them */
			if (xmlStrcmp(value_data, value_model) != 0) {
				xmlFree(value_data);
				xmlFree(value_model);
				return (EXIT_FAILURE);
			}

			/*
			 * all checks passed so transform it to edit-config's operation
			 * 'delete', to delete the node from the configuration data and
			 * put them back to the default values.
			 */
			xmlRemoveProp(xmlHasNsProp(defaults->nodesetval->nodeTab[i], BAD_CAST "default", BAD_CAST "urn:ietf:params:xml:ns:netconf:default:1.0"));
			ns = xmlNewNs(defaults->nodesetval->nodeTab[i], BAD_CAST NC_NS_BASE10, BAD_CAST NC_NS_BASE10_ID);
			xmlNewNsProp(defaults->nodesetval->nodeTab[i], ns, BAD_CAST "operation", BAD_CAST "remove");
		}

		xmlXPathFreeObject(defaults);
	}
	xmlXPathFreeContext(ctxt);

	return (retval);
}
