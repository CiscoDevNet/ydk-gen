/**
 * \file edit-config.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Header file for NETCONF edit-config implementation.
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

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "datastore_internal.h"
#include "../netconf.h"
#include "../netconf_internal.h"
#include "../error.h"

#ifndef NC_EDIT_CONFIG_H_
#define NC_EDIT_CONFIG_H_

typedef xmlXPathObjectPtr keyList;
#define keyListFree(x) xmlXPathFreeObject((xmlXPathObjectPtr)x)

keyList get_keynode_list(xmlDocPtr model);

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
 * \param[in] keys List of the key elements from the configuration data model.
 *
 * \return 0 - false, 1 - true (matching elements).
 */
int matching_elements(xmlNodePtr node1, xmlNodePtr node2, keyList keys, int leaf);

/**
 * \brief Find an equivalent of the given node in orig_doc document.
 *
 * \param[in] orig_doc Original configuration document to edit.
 * \param[in] node Element whose equivalent in orig_doc should be found.
 * \param[in] model Configuration data model.
 * \param[in] keys List of the key elements from the configuration data model.
 * \return Found equivalent element, NULL if no such element exists.
 */
xmlNodePtr find_element_equiv(xmlDocPtr orig_doc, xmlNodePtr edit, xmlDocPtr model, keyList keys);

/**
 * @brief Go recursively in the YIN model and find model's equivalent of the node
 * @param[in] node XML element which we want to find in the model
 * @param[in] model Configuration data model (YIN format)
 * @return model's equivalent of the node, NULL if no such element is found.
 */
xmlNodePtr find_element_model(xmlNodePtr node, xmlDocPtr model);

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
int edit_config(xmlDocPtr repo, xmlDocPtr edit, struct ncds_ds* ds, NC_EDIT_DEFOP_TYPE defop, NC_EDIT_ERROPT_TYPE UNUSED(errop), const struct nacm_rpc* nacm, struct nc_err **error);

int edit_replace_nacmcheck(xmlNodePtr orig_node, xmlDocPtr edit_doc, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error);
int edit_merge(xmlDocPtr orig_doc, xmlNodePtr edit_node, NC_EDIT_DEFOP_TYPE defop, xmlDocPtr model, keyList keys, const struct nacm_rpc* nacm, struct nc_err** error);

/**
 * \todo: stolen from old netopeer, verify function
 * \brief compare the node namespace against the reference node namespace
 *
 * \param reference     reference node, compared node must have the same namespace as the reference node
 * \param node          compared node
 *
 * \return              0 if compared node is in the same namespace as the reference
 *                      node, 1 otherwise
 */
int nc_nscmp(xmlNodePtr reference, xmlNodePtr node);

#endif /* NC_EDIT_CONFIG_H_ */
