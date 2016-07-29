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

#ifndef NC_MESSAGES_H_
#define NC_MESSAGES_H_

#include <time.h>
#include "netconf.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup rpc
 * @brief Create a new NETCONF filter of the specified type.
 * @param[in] type Type of the filter.
 * @param[in] ... Filter content:
 * - for #NC_FILTER_SUBTREE type, a single variadic parameter
 * **const char* filter** is accepted.
 * @return Created NETCONF filter structure.
 */
struct nc_filter* nc_filter_new(NC_FILTER_TYPE type, ...);

/**
 * @ingroup rpc
 * @brief Destroy the filter structure.
 * @param[in] filter Filter to destroy.
 */
void nc_filter_free(struct nc_filter *filter);

/**
 * @brief Dump the rpc-reply message into a string.
 * @ingroup reply
 * @param[in] reply rpc-reply message.
 * @return String in XML format containing the NETCONF's \<rpc-reply\> element
 * and all of its content. Caller is responsible for freeing the returned string
 * with free().
 */
char* nc_reply_dump(const nc_reply *reply);

/**
 * @ingroup rpc
 * @brief Duplicate \<rpc\> message.
 * @param[in] rpc \<rpc\> message to replicate.
 * @return Copy of the given \<rpc\> message.
 */
nc_rpc* nc_rpc_dup(const nc_rpc* rpc);

/**
 * @ingroup reply
 * @brief Duplicate \<reply\> message.
 * @param[in] reply \<reply\> message to replicate.
 * @return Copy of the given \<reply\> message.
 */
nc_reply* nc_reply_dup(const nc_reply* reply);

/**
 * @ingroup reply
 * @brief Build \<rpc-reply\> message from the string.
 * This is the reverse function of the nc_reply_dump().
 *
 * @param[in] reply_dump String containing the NETCONF \<rpc-reply\> message.
 * @return Complete reply structure used by libnetconf's functions.
 */
nc_reply* nc_reply_build(const char* reply_dump);

/**
 * @ingroup reply
 * @brief Get message-id of the given rpc-reply.
 * @param[in] reply rpc-reply message.
 * @return message-id of the given rpc-reply message.
 */
const nc_msgid nc_reply_get_msgid(const nc_reply *reply);

/**
 * @brief Dump the rpc message into a string.
 * @ingroup rpc
 * @param[in] rpc rpc message.
 * @return String in XML format containing the NETCONF's \<rpc\> element
 * and all of its content. Caller is responsible for freeing the returned string with
 * free().
 */
char* nc_rpc_dump(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Build \<rpc\> message from the string.
 * This is the reverse function of the nc_rpc_dump().
 *
 * @param[in] rpc_dump String containing the NETCONF \<rpc\> message.
 * @param[in] session Session information needed for ACM. If NULL, ACM structure
 * is not prepared and no ACM rules will be applied to the created RPC message.
 * @return Complete rpc structure used by libnetconf's functions.
 */
nc_rpc* nc_rpc_build(const char* rpc_dump, const struct nc_session* session);

/**
 * @ingroup rpc
 * @brief Get message-id of the given rpc.
 * @param[in] rpc rpc message.
 * @return message-id of the given rpc message.
 */
const nc_msgid nc_rpc_get_msgid(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get user's rpc operation namespace
 * @param rpc rpc message.
 * @return Namespace URI, NULL in case of error.
 */
char* nc_rpc_get_ns(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get operation of the given rpc.
 * @param[in] rpc rpc message.
 * @return Operation identification of the given rpc message.
 */
NC_OP nc_rpc_get_op(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get operation name of the given rpc.
 * @param[in] rpc rpc message.
 * @return Name of operation in the given rpc message. Caller
 * is responsible for freeing the returned string with free().
 */
char* nc_rpc_get_op_name(const nc_rpc* rpc);

/**
 * @ingroup rpc
 * @brief Get operation namespace of the given rpc.
 * @param[in] rpc rpc message.
 * @return Namespace of operation in the given rpc message. Caller
 * is responsible for freeing the returned string with free().
 */
char* nc_rpc_get_op_namespace(const nc_rpc* rpc);

/**
 * @ingroup rpc
 * @brief Get content of the operation specification from the given rpc.
 * @param[in] rpc rpc message.
 * @return String in XML form starting with the operation name element. Caller
 * is responsible for freeing the returned string with free().
 */
char* nc_rpc_get_op_content(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get type of the rpc message.
 *
 * \<rpc\> message can affect the datastore, the session or it can be unknown for the
 * libnetconf (defined by an unsupported capability or device configuration
 * model)
 *
 * @param[in] rpc rpc message
 * @return One of the #NC_RPC_TYPE.
 */
NC_RPC_TYPE nc_rpc_get_type(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get the target datastore type (running, startup, candidate) of the rpc request.
 *
 * For \<rpc\> message that does not affect datastore (e.g. kill-session), the
 * #NC_DATASTORE_ERROR is returned.
 *
 * @param[in] rpc rpc message
 * @return One of the #NC_DATASTORE.
 */
NC_DATASTORE nc_rpc_get_target(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get the source datastore type (running, startup, candidate) of the rpc request.
 *
 * For \<rpc\> message that does not affect datastore (e.g. kill-session), the
 * #NC_DATASTORE_ERROR is returned.
 *
 * @param[in] rpc rpc message
 * @return One of the #NC_DATASTORE.
 */
NC_DATASTORE nc_rpc_get_source(const nc_rpc *rpc);

/**
 * @ingroup rpc
 *
 * @brief Get serialized content of the config parameter (\<config\> itself is
 * not a part of the returned data). This function is valid only for
 * \<copy-config\> and \<edit-config\> RPCs.
 *
 * @param[in] rpc \<copy-config\> or \<edit-config\> rpc message.
 *
 * @return Serialized XML or NULL if not available. Caller is responsible for
 * freeing the returned string with free().
 */
char* nc_rpc_get_config(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get default-operation type, valid only for \<edit-config\> RPCs.
 *
 * @param[in] rpc \<edit-config\> rpc message
 *
 * @return One of the #NC_EDIT_DEFOP_TYPE, #NC_EDIT_DEFOP_ERROR in case of error.
 */
NC_EDIT_DEFOP_TYPE nc_rpc_get_defop(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get error-option type, valid only for \<edit-config\> RPCs.
 * @param[in] rpc \<edit-config\> rpc message
 *
 * @return One of the #NC_EDIT_ERROPT_TYPE, #NC_EDIT_ERROPT_ERROR in case of an error
 */
NC_EDIT_ERROPT_TYPE nc_rpc_get_erropt(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get test-option type, valid only for \<edit-config\> RPCs supporting
 * :validate:1.1 capability.
 * @param[in] rpc \<edit-config\> rpc message
 *
 * @return One of the #NC_EDIT_TESTOPT_TYPE, #NC_EDIT_TESTOPT_ERROR in case of
 * an error
 */
NC_EDIT_TESTOPT_TYPE nc_rpc_get_testopt(const nc_rpc *rpc);

/**
 * @ingroup rpc
 * @brief Get filter from \<get\> or \<get-config\> RPC
 *
 * @param[in] rpc \<get\> or \<get-config\> rpc message
 *
 * @return pointer to the struct nc_filter or NULL if no filter specified
 */
struct nc_filter* nc_rpc_get_filter(const nc_rpc * rpc);

/**
 * @ingroup reply
 * @brief Get the type of the rpc-reply message.
 *
 * \<rpc-reply\> message can contain \<ok\>, \<rpc-error\> or \<data\>
 *
 * @param[in] reply rpc-reply message
 * @return One of the #NC_REPLY_TYPE.
 */
NC_REPLY_TYPE nc_reply_get_type(const nc_reply *reply);

/**
 * @ingroup reply
 * @brief Get content of the \<data\> element in \<rpc-reply\>.
 * @param reply rpc-reply message.
 * @return String with the content of the \<data\> element. Caller is
 * responsible for freeing the returned string with free().
 */
char* nc_reply_get_data(const nc_reply *reply);

/**
 * @ingroup reply
 * @brief Get namespace of the \<data\> element in \<rpc-reply\>.
 * @param reply rpc-reply message.
 * @return Namespace of the \<data\> element. Do not free.
 */
const char* nc_reply_get_data_ns(const nc_reply *reply);

/**
 * @ingroup reply
 * @brief Get error-message from the server's \<rpc-error\> reply.
 * @param reply rpc-reply message of the #NC_REPLY_ERROR type.
 * @return String with the content of the \<error-message\> element. Referenced
 * string is a part of the reply, so it can not be used after freeing the
 * given reply.
 */
const char* nc_reply_get_errormsg(const nc_reply *reply);

/**
 * @brief Free rpc message.
 * @ingroup rpc
 * @param[in] rpc rpc message to free.
 */
void nc_rpc_free(nc_rpc *rpc);

/**
 * @brief Free reply message.
 * @ingroup reply
 * @param[in] reply reply message to free.
 */
void nc_reply_free(nc_reply *reply);

/**
 * @ingroup reply
 * @brief Create \<ok\> rpc-reply response.
 * @return Created \<rpc-reply\> message.
 */
nc_reply* nc_reply_ok(void);

/**
 * @ingroup reply
 * @brief Create rpc-reply response with \<data\> content (in
 * urn:ietf:params:xml:ns:netconf:base:1.0 namespace).
 * @param[in] data Serialized XML content of the \<data\> element for the
 * \<rpc-reply\> message being created.
 * @return Created \<rpc-reply\> message.
 */
nc_reply* nc_reply_data(const char* data);

/**
 * @ingroup reply
 * @brief Create rpc-reply response with \<data\> content in the specified namespace.
 * @param[in] data Serialized XML content of the \<data\> element for the
 * \<rpc-reply\> message being created.
 * @param[in] ns Default namespace for the data element.
 * @return Created \<rpc-reply\> message.
 */
nc_reply* nc_reply_data_ns(const char* data, const char* ns);

/**
 * @ingroup reply
 * @brief Create rpc-reply response with \<rpc-error\> content.
 * @param[in] error NETCONF error description structure for the reply message. From
 * now, the error is connected with the reply and should not be used by the caller.
 * @return Created \<rpc-reply\> message.
 */
nc_reply* nc_reply_error(struct nc_err* error);

/**
 * @ingroup reply
 * @brief Add another error description into the existing rpc-reply with \<rpc-error\> content.
 *
 * This function can be applied only to reply messages created by nc_reply_error().
 *
 * @param[in,out] reply Reply structure to which the given error description will
 * be added.
 * @param[in] error NETCONF error description structure for the reply message. From
 * now, the error is connected with the reply and should not be used by the caller.
 * @return 0 on success, non-zero else.
 */
int nc_reply_error_add(nc_reply *reply, struct nc_err* error);

/**
 * @ingroup reply
 * @brief Merge reply messages. All messages MUST be of the same type.
 *
 * Function merges a number of \<rpc-reply\> specified by the count parameter (at
 * least 2) into one \<rpc-reply\> message which is returned as the result. When
 * the merge is successful, all input messages are freed and MUST NOT be used
 * after this call. Merge can fail only because of an invalid input parameter. In
 * such a case, NULL is returned and input messages are left unchanged.
 *
 * @param[in] count Number of messages to merge
 * @param[in] ... Messages to merge (all are of nc_reply* type). Total number of
 * messages MUST be equal to count.
 *
 * @return Pointer to a new reply message with the merged content of the messages to
 * merge. If an error occurs (due to the invalid input parameters), NULL is
 * returned and the messages to merge are not freed.
 */
nc_reply* nc_reply_merge(int count, ...);

/**
 * @ingroup rpc
 * @brief Create \<copy-config\> NETCONF rpc message.
 *
 * ### Variadic parameters:
 * - source is specified as #NC_DATASTORE_CONFIG:
 *  - nc_rpc_copyconfig() accepts as the first variadic parameter
 *  **const char* source_config** providing the complete configuration data to copy.
 * - source is specified as #NC_DATASTORE_URL:
 *  - nc_rpc_copyconfig() accepts as the first variadic parameter
 *  **const char* source_url** providing the URL to the file
 * - target is specified as #NC_DATASTORE_URL:
 *  - nc_rpc_copyconfig() accepts as another (first or second according to an
 *  eventual previous variadic parameter) variadic parameter
 *  **const char* target_url** providing the URL to the target file.
 *
 * The file that the url refers to contains the complete datastore, encoded in
 * XML under the element \<config\> in the
 * "urn:ietf:params:xml:ns:netconf:base:1.0" namespace.
 *
 * @param[in] source Source configuration of the datastore type. If the
 * #NC_DATASTORE_CONFIG is specified, data parameter is used as the complete
 * configuration to copy.
 * @param[in] target Target configuration datastore type to be replaced.
 * @param[in] ... Specific parameters according to the source and target parameters.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_copyconfig(NC_DATASTORE source, NC_DATASTORE target, ...);

/**
 * @ingroup rpc
 * @brief Create \<delete-config\> NETCONF rpc message.
 *
 * @param[in] target Target configuration datastore type to be deleted.
 * @param[in] ... URL as **const char* url** if the target parameter is
 * specified as #NC_DATASTORE_URL.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_deleteconfig(NC_DATASTORE target, ...);

/**
 * @ingroup rpc
 * @brief Create \<edit-config\> NETCONF rpc message.
 *
 * @param[in] target Target configuration datastore type to be edited.
 * @param[in] source Specifies the type of the source data taken from the
 * variadic parameter. Only #NC_DATASTORE_CONFIG (variadic parameter contains
 * the \<config\> data) and #NC_DATASTORE_URL (variadic parameter contains URL
 * for \<url\> element) values are accepted.
 * @param[in] default_operation Default operation for this request, 0 to skip
 * setting this parameter and use the default server ('merge') behavior.
 * @param[in] error_option Set the response to an error, 0 for the server default
 * behavior.
 * @param[in] test_option Set test-option element according to :validate:1.1
 * capability specified in RFC 6241.
 * @param[in] ... According to the source parameter, variadic parameter can be
 * one of the following:
 * - **const char* config** defining the content of the \<config\> element
 * in case the source parameter is specified as #NC_DATASTORE_CONFIG
 * - **const char* source_url** specifying URL, in case the source parameter is
 * specified as #NC_DATASTORE_URL. The URL must refer to the file containing
 * configuration data hierarchy to be modified, encoded in XML under the element
 * \<config\> in the "urn:ietf:params:xml:ns:netconf:base:1.0" namespace.
 *
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_editconfig(NC_DATASTORE target, NC_DATASTORE source, NC_EDIT_DEFOP_TYPE default_operation, NC_EDIT_ERROPT_TYPE error_option, NC_EDIT_TESTOPT_TYPE test_option, ...);

/**
 * @ingroup rpc
 * @brief Create \<get\> NETCONF rpc message.
 *
 * @param[in] filter NETCONF filter or NULL if no filter is required.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_get(const struct nc_filter *filter);

/**
 * @ingroup rpc
 * @brief Create \<get-config\> NETCONF rpc message.
 *
 * @param[in] source Source configuration datastore type being queried.
 * @param[in] filter NETCONF filter or NULL if no filter required.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_getconfig(NC_DATASTORE source, const struct nc_filter *filter);

/**
 * @ingroup rpc
 * @brief Create \<kill-session\> NETCONF rpc message.
 *
 * @param[in] kill_sid ID of session to kill.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_killsession(const char *kill_sid);

/**
 * @ingroup rpc
 * @brief Create \<lock\> NETCONF rpc message.
 *
 * @param[in] target Target configuration datastore type to be locked.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_lock(NC_DATASTORE target);

/**
 * @ingroup rpc
 * @brief Create \<unlock\> NETCONF rpc message.
 *
 * @param[in] target Target configuration datastore type to be unlocked.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_unlock(NC_DATASTORE target);

/**
 * @ingroup rpc
 * @brief Create \<validate\> NETCONF rpc message.
 *
 * ### Variadic parameters:
 * - source is specified as #NC_DATASTORE_URL:
 *  - nc_rpc_validate() accepts the first variadic parameter
 *  **const char* source_url** providing the url to the file.
 * - source is specified as #NC_DATASTORE_CONFIG:
 *  - nc_rpc_validate() accepts as the first variadic parameter
 *  **const char* source_config** providing the complete configuration data to copy.
 *
 * @param[in] source Name of the configuration datastore to validate.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_validate(NC_DATASTORE source, ...);

/**
 * @ingroup notifications
 * @brief Create \<create-subsciption\> NETCONF rpc message.
 *
 * Detailed description of this operation can be found in RFC 5277, section 2.1.1.
 *
 * @param[in] stream Name of the stream of events that is of interest. Optional
 * parameter (NULL is accepted), if not specified, the default NETCONF stream is
 * subscribed.
 * @param[in] filter Specify the subset of all possible events to be received.
 * Optional parameter (NULL is accepted).
 * @param[in] start Start time to trigger the replay feature from the specified
 * time. Optional parameter (NULL is accepted). Format of the date is of the type
 * dateTime according to RFC 3339.
 * @param[in] stop Stop time to stop the replay of event notifications. Optional
 * parameter (NULL is accepted). Format of the date is of the type dateTime
 * according to RFC 3339.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_subscribe(const char* stream, const struct nc_filter *filter, const time_t* start, const time_t* stop);

/**
 * @ingroup rpc
 * @brief Create \<commit\> NETCONF rpc message.
 *
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_commit(void);

/**
 * @ingroup rpc
 * @brief Create \<discard-changes\> NETCONF rpc message.
 *
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_discardchanges(void);

/**
 * @ingroup rpc
 * @brief Create \<get-schema\> NETCONF rpc message (RFC 6022).
 * @param[in] name Identifier for the schema list entry.
 * @param[in] version Optional parameter specifying version of the requested
 * schema.
 * @param[in] format Optional parameter specifying the data modeling language
 * of the schema.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_getschema(const char* name, const char* version, const char* format);

/**
 * @ingroup rpc
 * @brief Create a generic NETCONF rpc message with the specified content.
 *
 * Function gets the data parameter and envelopes it into \<rpc\> container. Caller
 * is fully responsible for the correctness of the given data.
 *
 * @param[in] data XML content of the \<rpc\> request to be sent.
 * @return Created rpc message.
 */
nc_rpc* nc_rpc_generic(const char* data);

/**
 * @ingroup rpc
 * @brief Set the attribute of the given \<rpc\> which is specific for some
 * NETCONF capability.
 *
 * ### Parameters for specific capability attributes:
 * - #NC_CAP_ATTR_WITHDEFAULTS_MODE
 *  - applicable to \<get\>, \<get-config\> and \<copy-config\> operations.
 *  - Accepts one parameter of #NCWD_MODE type, specifying mode of the
 *  :with-defaults capability (RFC 6243).
 *
 * ### Examples:
 * - nc_rpc_capability_attr(rpc, #NC_CAP_ATTR_WITHDEFAULTS_MODE, #NCWD_MODE_ALL);
 *
 * @param[in] rpc RPC to be modified. This RPC must be created by one of the
 * nc_rpc* functions. RPC received by the server side's nc_session_recv_rpc() is
 * not accepted.
 * @param[in] attr RPC's attribute defined by a capability to be set or changed.
 * The list of accepted operations can be found in the description of this function.
 * @return 0 on success,\n non-zero on error.
 *
 */
int nc_rpc_capability_attr(nc_rpc* rpc, NC_CAP_ATTR attr, ...);

#ifdef __cplusplus
}
#endif

#endif /* NC_MESSAGES_H_ */
