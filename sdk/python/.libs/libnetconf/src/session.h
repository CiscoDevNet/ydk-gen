/**
 * \file session.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions to handle NETCONF sessions.
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

#ifndef NC_SESSION_H_
#define NC_SESSION_H_

#include "transport.h"
#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup session
 * @brief Create a disconnected session structure.
 *
 * This creates a dummy session structure which is not supposed to exchange NETCONF
 * messages between client and server. Instead, it can be successfully used by
 * server (e.g. detached process that doesn't hold the real session structure)
 * to access NETCONF datastores via libnetconf.
 *
 * All the required parameters can be obtained from the real session structure by
 * the session getter functions (nc_session_get_id(), nc_session_get_user() and
 * nc_session_get_cpblts()). NULL values are not allowed.
 *
 * @param[in] sid Session ID.
 * @param[in] username Name of the user holding the session.
 * @param[in] hostname Name (domain name, IP) of the opposite communication side
 * (optional parameter, can be NULL).
 * @param[in] capabilities List of capabilities supported by the session.
 * @return Structure describing a dummy NETCONF session or NULL in case of an error.
 */
struct nc_session* nc_session_dummy(const char* sid, const char* username, const char* hostname, struct nc_cpblts *capabilities);

/**
 * @ingroup session
 * @brief Add the session into the internal list of monitored sessions that are
 * returned as part of netconf-state information defined in RFC 6022.
 * @param session Session to be monitored;
 * @return 0 on success, non-zero on error.
 */
int nc_session_monitor(struct nc_session* session);

/**
 * @ingroup session
 * @brief Cleanup the session structure and free all the allocated resources.
 *
 * Do not use the given session structure after this call. If this session
 * was created with nc_session_accept_tls() or nc_session_accept_libssh_channel(),
 * the library transport structures are not freed or modified in any way.
 *
 * @param[in] session Session to free.
 */
void nc_session_free(struct nc_session* session);

/**
 * @ingroup session
 * @brief Get information about the session current status.
 * @param[in] session NETCONF session.
 * @return NETCONF session status.
 */
NC_SESSION_STATUS nc_session_get_status(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Get NETCONF protocol version used in the given session.
 * @param[in] session NETCONF session structure
 * @return NETCONF protocol version, 0 for 1.0, 1 for 1.1
 */
int nc_session_get_version(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Get the input file descriptor to asynchronous control of input events.
 *
 * The caller must avoid direct reading from the returned file descriptor. It is
 * supposed to be used only by select, poll, epoll or an event library (e.g.
 * libevent).
 *
 * @param[in] session NETCONF session structure
 * @return Input file descriptor of the communication channel.
 */
int nc_session_get_eventfd(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Get NETCONF session ID
 * @param[in] session NETCONF session structure
 * @return Constant string identifying NETCONF session.
 */
const char* nc_session_get_id(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Get NETCONF session host
 * @param[in] session NETCONF session structure
 * @return Constant string identifying NETCONF session server host.
 */
const char* nc_session_get_host(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Get NETCONF session port number
 * @param[in] session NETCONF session structure
 * @return Constant string identifying NETCONF session server host.
 */
const char* nc_session_get_port(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Get NETCONF session username
 * @param[in] session NETCONF session structure
 * @return Constant string identifying NETCONF session server host.
 */
const char* nc_session_get_user(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Get transport protocol used for the NETCONF session
 * @param[in] session NETCONF session structure
 * @return one of NC_TRANSPORT enumeration values.
 */
NC_TRANSPORT nc_session_get_transport(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Tell me if the notification subscription is allowed on the given session.
 * @param[in] session NETCONF session structure
 * @return 0 if not, 1 if subscription is currently allowed.
 */
int nc_session_notif_allowed(struct nc_session *session);

/**
 * @ingroup session
 * @brief Get list of capabilities associated with the session.
 *
 * Returned structure is connected with the session. Do not free or modify it.
 *
 * @param[in] session NETCONF session structure
 * @return NETCONF capabilities structure containing capabilities associated
 * with the given session. NULL is returned on error.
 */
struct nc_cpblts* nc_session_get_cpblts(const struct nc_session* session);

/**
 * @ingroup session
 * @brief Create a new NETCONF capabilities structure.
 * @param list NULL terminated list of capabilities strings to initially add
 * into the NETCONF capabilities structure.
 * @return Created NETCONF capabilities structure.
 */
struct nc_cpblts *nc_cpblts_new(const char* const list[]);

/**
 * @ingroup session
 * @brief Free NETCONF capabilities structure.
 *
 * This function is NOT thread safe.
 *
 * @param c Capabilities structure to free.
 */
void nc_cpblts_free(struct nc_cpblts *c);

/**
 * @ingroup session
 * @brief Add another capability string into the NETCONF capabilities structure.
 *
 * This function is NOT thread safe.
 *
 * @param capabilities Current NETCONF capabilities structure.
 * @param capability_string Capability string to add.
 * @return 0 on success\n non-zero on error
 */
int nc_cpblts_add(struct nc_cpblts *capabilities, const char* capability_string);

/**
 * @ingroup session
 * @brief Remove the specified capability string from the NETCONF capabilities structure.
 *
 * This function is NOT thread safe.
 *
 * @param capabilities Current NETCONF capabilities structure.
 * @param capability_string Capability string to remove.
 * @return 0 on success\n non-zero on error
 */
int nc_cpblts_remove(struct nc_cpblts *capabilities, const char* capability_string);

/**
 * @ingroup session
 * @brief Check if the given capability is supported by the session.
 * @param session Established session where the given capability support will
 * be checked.
 * @param capability_string NETCONF capability string to check.
 * @return 0 for false result, 1 if the given capability is supported.
 */
int nc_cpblts_enabled(const struct nc_session* session, const char* capability_string);

/**
 * @ingroup session
 * @brief Get complete capability string including parameters
 * @param[in] c Capabilities structure to be examined
 * @param[in] capability_string Capability identifier, parameters are ignored
 * and only basic identifier is used to retrieve specific identifier including
 * parameters from the given capability structure.
 * @return Constant capability identifier including parameters
 */
const char* nc_cpblts_get(const struct nc_cpblts *c, const char* capability_string);

/**
 * @ingroup session
 * @brief Move NETCONF capabilities structure iterator to the beginning of the capability strings list.
 *
 * This function is NOT thread safe.
 *
 * @param c NETCONF capabilities structure to be iterated.
 */
void nc_cpblts_iter_start(struct nc_cpblts *c);

/**
 * @ingroup session
 * @brief Get the next capability string from the given NETCONF capabilities structure.
 *
 * To move iterator to the beginning of the capability strings list, use
 * nc_cpblts_iter_start().
 *
 * This function is NOT thread safe.
 *
 * @param c NETCONF capabilities structure to be iterated.
 * @return Another capability string, NULL if all strings were already returned.
 */
const char *nc_cpblts_iter_next(struct nc_cpblts *c);

/**
 * @ingroup session
 * @brief Get the number of capabilities in the structure.
 *
 * Use this function to get the count of capabilities held by nc_cpblts structure.
 *
 * @param c NETCONF capabilities structure.
 * @return Number of capabilities held by structure c.
 */
int nc_cpblts_count(const struct nc_cpblts *c);

/**
 * @ingroup session
 * @brief Get NULL terminated list of the default capabilities supported by
 * libnetconf including the list of namespaces provided by the datastores
 * created with ncds_new() and initialized by ncds_init().
 *
 * The caller is supposed to free the returned structure with nc_cpblts_free().
 *
 * @return NETCONF capabilities structure containing capabilities supported by
 * libnetconf.
 */
struct nc_cpblts *nc_session_get_cpblts_default(void);

/**
 * @ingroup rpc
 * @brief Send \<rpc\> request via specified NETCONF session.
 * This function is supposed to be performed only by NETCONF clients.
 *
 * This function IS thread safe.
 *
 * @param[in] session NETCONF session to use.
 * @param[in] rpc \<rpc\> message to send.
 * @return 0 on error,\n message-id of sent message on success.
 */
const nc_msgid nc_session_send_rpc(struct nc_session* session, nc_rpc *rpc);

/**
 * @ingroup reply
 * @brief Send \<rpc-reply\> response via specified NETCONF session.
 * This function is supposed to be performed only by NETCONF servers.
 *
 * This function IS thread safe.
 *
 * @param[in] session NETCONF session to use.
 * @param[in] rpc \<rpc\> message which is request for the sending reply
 * @param[in] reply \<repc-reply\> message to send.
 * @return 0 on error,\n message-id of sent message on success.
 */
const nc_msgid nc_session_send_reply(struct nc_session* session, const nc_rpc* rpc, const nc_reply *reply);

/**
 * @ingroup notifications
 * @brief Send \<notification\> message from server to client
 *
 * @param[in] session NETCONF session to use.
 * @param[in] ntf \<notification\> message to send.
 * @return 0 on success,\n non-zero on error.
 */
int nc_session_send_notif(struct nc_session* session, const nc_ntf* ntf);

/**
 * @ingroup rpc
 * @brief Receive \<rpc\> request from the specified NETCONF session.
 * This function is supposed to be performed only by NETCONF servers.
 *
 * @param[in] session NETCONF session to use.
 * @param[in] timeout Timeout in microseconds, -1 for infinite timeout, 0 for
 * non-blocking
 * @param[out] rpc Received \<rpc\>
 * @return
 * - #NC_MSG_RPC - success, *rpc points to the received \<rpc\> message.
 * - #NC_MSG_HELLO - success, *rpc points to the received \<hello\> message.
 * - #NC_MSG_UNKNOWN - error occurred
 * - #NC_MSG_WOULDBLOCK - receiving timeouted without any received message.
 */
NC_MSG_TYPE nc_session_recv_rpc(struct nc_session* session, int timeout, nc_rpc** rpc);

/**
 * @ingroup reply
 * @brief Receive \<rpc-reply\> response from the specified NETCONF session.
 * This function is supposed to be performed only by NETCONF clients.
 *
 * @param[in] session NETCONF session to use.
 * @param[in] timeout Timeout in microseconds, -1 for infinite timeout, 0 for
 * non-blocking
 * @param[out] reply Received \<rpc-reply\>
 * @return
 * - #NC_MSG_REPLY - success, *reply points to the received \<rpc-reply\> message.
 * - #NC_MSG_HELLO - success, *reply points to the received \<hello\> message.
 * - #NC_MSG_NONE - success, but \<rpc-reply\> with error information was
 *   processed automatically using callback specified with nc_callback_error_reply()
 *   function. *reply was not changed.
 * - #NC_MSG_UNKNOWN - error occurred
 * - #NC_MSG_NOTIFICATION - \<notification\> message was received and enqueued
 *   to the internal queue until the nc_session_recv_notif() function is called.
 *   Caller is supposed to repeat the function call to receive another
 *   \<rpc-reply\> message.
 * - #NC_MSG_WOULDBLOCK - receiving timeouted without any received message.
 */
NC_MSG_TYPE nc_session_recv_reply(struct nc_session* session, int timeout, nc_reply** reply);

/**
 * @ingroup notifications
 * @brief Receive a \<notification\> message from the specified NETCONF session.
 * This function is supposed to be performed only by NETCONF clients.
 *
 * @param[in] session NETCONF session to use.
 * @param[in] timeout Timeout in microseconds, -1 for infinite timeout, 0 for
 * non-blocking
 * @param[out] ntf Received \<notification\> message
 * @return
 * - #NC_MSG_NOTIFICATION - success, *ntf points to the received \<notification\>
 * - #NC_MSG_UNKNOWN - error occurred
 * - #NC_MSG_REPLY - \<rpc-reply\> to some request received and enqueued to the
 *   internal queue until the nc_session_recv_reply() function is called. Caller
 *   is supposed to repeat the function call to receive another \<notification\>
 *   message.
 * - #NC_MSG_WOULDBLOCK - receiving timeouted without any received message.
 */
NC_MSG_TYPE nc_session_recv_notif(struct nc_session* session, int timeout, nc_ntf** ntf);

/**
 * @ingroup genAPI
 * @brief Compare two message IDs if they are the same.
 *
 * @param[in] id1 First message ID to compare.
 * @param[in] id2 Second message ID to compare.
 * @return 0 if both IDs are the same.
 */
int nc_msgid_compare(const nc_msgid id1, const nc_msgid id2);

/**
 * @ingroup rpc
 * @brief Send \<rpc\> and receive \<rpc-reply\> via the specified NETCONF session.
 * @param[in] session NETCONF session to use.
 * @param[in] rpc RPC message to send.
 * @param[out] reply Received \<rpc-reply\>
 * @return
 * - #NC_MSG_REPLY - success, *reply points to the received \<rpc-reply\> message.
 * - #NC_MSG_NONE - success, but \<rpc-reply\> with error information was
 *   processed automatically using callback specified with nc_callback_error_reply()
 *   function. *reply was not changed.
 * - #NC_MSG_UNKNOWN - error occurred
 */
NC_MSG_TYPE nc_session_send_recv(struct nc_session* session, nc_rpc *rpc, nc_reply** reply);

#ifdef __cplusplus
}
#endif

#endif /* NC_SESSION_H_ */
