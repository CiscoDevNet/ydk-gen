/**
 * \file messages.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions to create NETCONF messages.
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

#ifndef NC_MESSAGES_XML_H_
#define NC_MESSAGES_XML_H_

#include <time.h>

#include <libxml/tree.h>

#include "netconf.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup rpc_xml
 * @brief Create a new NETCONF filter of the specified type.
 * @param[in] type Type of the filter.
 * @param[in] ... Filter content:
 * - for #NC_FILTER_SUBTREE type, single variadic parameter
 * **const xmlNodePtr filter** with the content for the \<filter\> element is
 * accepted. The filter parameter is not supposed to contain \<filter\> element,
 * but only its content. The node is taken as a node list, so the sibling nodes
 * are also added into the filter definition. If NULL is specified, the Empty filter
 * (RFC 6241 sec 6.4.2) is created.
 * @return Created NETCONF filter structure.
 */
struct nc_filter* ncxml_filter_new(NC_FILTER_TYPE type, ...);

/**
 * @ingroup reply_xml
 * @brief Dump the rpc-reply message into a libxml2 document structure.
 * @param[in] reply rpc-reply message.
 * @return XML document of the NETCONF's \<rpc-reply\> message. Caller is
 * supposed to free the returned structure with xmlFreeDoc().
 */
xmlDocPtr ncxml_reply_dump(const nc_reply *reply);

/**
 * @ingroup reply_xml
 * @brief Build \<rpc-reply\> message from the libxml2 document structure.
 * This is the reverse function of the ncxml_reply_dump().
 *
 * @param[in] reply_dump XML document structure with the NETCONF \<rpc-reply\>
 * message. The structure is integrated into the internal rpc structure and
 * caller should not access (or free) the given XML document anymore.
 * @return Complete reply structure used by libnetconf's functions.
 */
nc_reply* ncxml_reply_build(xmlDocPtr reply_dump);

/**
 * @ingroup rpc_xml
 * @brief Dump the rpc message into a libxml2 document structure.
 * @param[in] rpc rpc message.
 * @return XML document of the NETCONF's \<rpc\> message. Caller is supposed
 * to free the returned structure with xmlFreeDoc().
 */
xmlDocPtr ncxml_rpc_dump(const nc_rpc *rpc);

/**
 * @ingroup rpc_xml
 * @brief Build \<rpc\> message from the libxml2 document structure.
 * This is the reverse function of the ncxml_rpc_dump().
 *
 * @param[in] rpc_dump XML document structure with the NETCONF \<rpc\> message.
 * The structure is integrated into the internal rpc structure and caller should
 * not access (or free) given XML document anymore.
 * @param[in] session Session information needed for ACM. If NULL, ACM structure
 * is not prepared and no ACM rules will be applied to the created RPC message.
 * @return Complete rpc structure used by libnetconf's functions.
 */
nc_rpc* ncxml_rpc_build(xmlDocPtr rpc_dump, const struct nc_session* session);

/**
 * @ingroup rpc_xml
 * @brief Get the content of the operation specification from the given rpc.
 * @param[in] rpc rpc message.
 * @return libxml2 node structure with the NETCONF operation element(s) and its
 * content. Caller is supposed to free the returned structure with xmlFreeNodeList().
 */
xmlNodePtr ncxml_rpc_get_op_content(const nc_rpc *rpc);

/**
 * @ingroup rpc_xml
 * @brief Get \<config\> element from the RPC operation including its content.
 * This function is valid only for \<copy-config\> and \<edit-config\> RPCs.
 *
 * @param[in] rpc \<copy-config\> or \<edit-config\> rpc message.
 *
 * @return XML node \<config\> with its content or NULL on error. Note that
 * \<config\> can contain no data. Caller is supposed to free the returned
 * structure with xmlFreeNode().
 */
xmlNodePtr ncxml_rpc_get_config(const nc_rpc *rpc);

/**
 * @ingroup reply_xml
 * @brief Get \<data\> element in \<rpc-reply\> including its content.
 * @param reply rpc-reply message.
 * @return XML node \<data\> with its content. \<data\> is included to provide
 * a valid (non-NULL) output in case of empty data returned (e.g. when all the data
 * are filterred out). Caller is supposed to free the returned structure with
 * xmlFreeNode().
 */
xmlNodePtr ncxml_reply_get_data(const nc_reply *reply);

/**
 * @ingroup reply_xml
 * @brief Create rpc-reply response with \<data\> content.
 * @param data Content (possibly a node list) for the \<rpc-reply\>'s \<data\>
 * element.
 * @return Created \<rpc-reply\> message.
 */
nc_reply* ncxml_reply_data(const xmlNodePtr data);

/**
 * @ingroup reply_xml
 * @brief Create rpc-reply response with \<data\> content in the specified namespace.
 * @param data Content (possibly a node list) for the \<rpc-reply\>'s \<data\>
 * element.
 * @param[in] ns Default namespace for the data element.
 * @return Created \<rpc-reply\> message.
 */
nc_reply* ncxml_reply_data_ns(const xmlNodePtr data, const char* ns);

/**
 * @ingroup rpc_xml
 * @brief Create \<copy-config\> NETCONF rpc message.
 *
 * ### Variadic parameters:
 * - the source is specified as #NC_DATASTORE_CONFIG:
 *  - nc_rpc_copyconfig() accepts as the first variadic parameter
 *  **const xmlNodePtr source_config** providing the complete configuration data
 *  to copy.
 * - the source is specified as #NC_DATASTORE_URL:
 *  - nc_rpc_copyconfig() accepts as the first variadic parameter
 *  **const char* source_url** providing the URL to the file
 * - target is specified as #NC_DATASTORE_URL:
 *  - nc_rpc_copyconfig() accepts as another (first or second according to
 *  eventual previous variadic parameter) variadic parameter
 *  **const char* target_url** providing the URL to the target file.
 *
 * The file that the url refers to contains the complete datastore, encoded in
 * XML under the element \<config\> in the
 * "urn:ietf:params:xml:ns:netconf:base:1.0" namespace.
 *
 * @param[in] source Source configuration datastore type. If the
 * NC_DATASTORE_NONE is specified, data parameter is used as the complete
 * configuration to copy.
 * @param[in] target Target configuration datastore type to be replaced.
 * @param[in] ... Specific parameters according to the source and target parameters.
 * @return Created rpc message.
 */
nc_rpc* ncxml_rpc_copyconfig(NC_DATASTORE source, NC_DATASTORE target, ...);

/**
 * @ingroup rpc_xml
 * @brief Create \<edit-config\> NETCONF rpc message.
 *
 * @param[in] target Target configuration datastore type to be edited.
 * @param[in] source Specifies the type of the source data taken from the
 * variadic parameter. Only #NC_DATASTORE_CONFIG (variadic parameter contains
 * the \<config\> data) and #NC_DATASTORE_URL (variadic parameter contains URL
 * for \<url\> element) values are accepted.
 * @param[in] default_operation Default operation for this request, 0 to skip
 * the setting of this parameter and use default server ('merge') behavior.
 * @param[in] error_option Set the response to an error, 0 for the server default
 * behavior.
 * @param[in] test_option Set test-option element according to :validate:1.1
 * capability specified in RFC 6241.
 * @param[in] ... According to the source parameter, variadic parameter can be
 * one of the following:
 * - **const xmlNodePtr config** defining the content of the \<config\> element
 * in case the source parameter is specified as #NC_DATASTORE_CONFIG. The config
 * parameter can points to the node list.
 * - **const char* source_url** specifying URL, in case the source parameter is
 * specified as #NC_DATASTORE_URL. The URL must refer to the file containing
 * configuration data hierarchy to be modified, encoded in XML under the element
 * \<config\> in the "urn:ietf:params:xml:ns:netconf:base:1.0" namespace.
 *
 * @return Created rpc message.
 */
nc_rpc* ncxml_rpc_editconfig(NC_DATASTORE target, NC_DATASTORE source, NC_EDIT_DEFOP_TYPE default_operation, NC_EDIT_ERROPT_TYPE error_option, NC_EDIT_TESTOPT_TYPE test_option, ...);

/**
 * @ingroup rpc_xml
 * @brief Create a generic NETCONF rpc message with the specified content.
 *
 * The function recieves the data parameter and envelopes it into an \<rpc\> container. Caller
 * is fully responsible for the correctness of the given data.
 *
 * @param[in] data XML content of the \<rpc\> request to be sent.
 * @return Created rpc message.
 */
nc_rpc* ncxml_rpc_generic(const xmlNodePtr data);

#ifdef __cplusplus
}
#endif

#endif /* NC_MESSAGES_XML_H_ */
