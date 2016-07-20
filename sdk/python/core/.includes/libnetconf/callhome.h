/**
 * \file callhome.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions to connect NETCONF server to a NETCONF client (Call Home).
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

#ifndef CALLHOME_H_
#define CALLHOME_H_

#include <stdint.h>

#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup callhome
 * @brief Structure describing a management server where the NETCONF server connects to.
 *
 * Any manipulation with the structure is allowed only via
 * nc_callhome_mngmt_server_*() functions.
 */
struct nc_mngmt_server;

/**
 * @ingroup callhome
 * @brief Add a new management server specification to the end of a list.
 *
 * To make this function available, you have to include libnetconf_ssh.h or
 * libnetconf_tls.h.
 *
 * @param[in] list Current list where the server description will be added. If
 * NULL, a new list is created and returned by the function.
 * @param[in] host Host name of the management server. It specifies either a
 * numerical network address (for IPv4, numbers-and-dots notation as supported
 * by inet_aton(3); for IPv6, hexadecimal string format as supported by
 * inet_pton(3)), or a network host-name, whose network addresses are looked up
 * and resolved.
 * @param[in] port Port of the management server. If this argument is a service
 * name (see services(5)), it is translated to the corresponding port number.
 * @return NULL on error, created/modified management servers list.
 */
struct nc_mngmt_server* nc_callhome_mngmt_server_add(struct nc_mngmt_server* list, const char* host, const char* port);

/**
 * @ingroup callhome
 * @brief Remove the specified management server description from the list.
 *
 * To make this function available, you have to include libnetconf_ssh.h or
 * libnetconf_tls.h.
 *
 * @param[in,out] list Management servers list to be modified.
 * @param[in,out] remove Management server to be removed from the given list.
 * The structure itself is not freed, use nc_callhome_mngmt_server_free()
 * to free it after calling nc_callhome_mngmt_server_rm().
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int nc_callhome_mngmt_server_rm(struct nc_mngmt_server* list, struct nc_mngmt_server* remove);

/**
 * @ingroup callhome
 * @brief Free a management server description structure(s). The function doesn't
 * free only the item refered by given pointer, but the complete list of
 * management servers is freed.
 *
 * To make this function available, you have to include libnetconf_ssh.h or
 * libnetconf_tls.h.
 *
 * @param[in] list List of management servers to be freed.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int nc_callhome_mngmt_server_free(struct nc_mngmt_server* list);

/**
 * @ingroup callhome
 * @brief Searches for the item from the list, which was marked and used by the
 * last call to nc_callhome_connect() to a successfully establish Call Home
 * connection.
 *
 * @param[in] list List of management servers.
 * @return Pointer to the last connected management server.
 */
struct nc_mngmt_server* nc_callhome_mngmt_server_getactive(struct nc_mngmt_server* list);

#ifndef DISABLE_LIBSSH

/**
 * @ingroup callhome
 * @brief Start listening on client side for incoming Call Home connection.
 *
 * To make this function available, you have to include libnetconf_ssh.h or
 * libnetconf_tls.h.
 *
 * @param[in] port Port number where to listen.
 * @return EXIT_SUCCESS or EXIT_FAILURE on error.
 */
int nc_callhome_listen(unsigned int port);

/**
 * @ingroup callhome
 * @brief Stop listening on client side for incoming Call Home connection.
 *
 * To make this function available, you have to include libnetconf_ssh.h or
 * libnetconf_tls.h.
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE if libnetconf is not listening.
 */
int nc_callhome_listen_stop(void);

/**
 * @ingroup callhome
 * @brief Accept incoming Call Home connection and create NETCONF session on it.
 *
 * This function uses transport protocol set by nc_session_transport(). If
 * NC_TRANSPORT_SSH (default value) is set, configure's --disable-libssh option
 * cannot be used. If NC_TRANSPORT_TLS is set, configure's --enable-tls must be
 * used
 *
 * To make this function available, you have to include libnetconf_ssh.h or
 * libnetconf_tls.h.
 *
 * @param[in] username Name of the user to login to the server. The user running the
 * application (detected from the effective UID) is used if NULL is specified.
 * @param[in] cpblts NETCONF capabilities structure with capabilities supported
 * by the client. Client can use nc_session_get_cpblts_default() to get the
 * structure with the list of all the capabilities supported by libnetconf (this is
 * used in case of a NULL parameter).
 * @param[in,out] timeout Timeout for waiting for incoming call home in milliseconds.
 * Negative value means an infinite timeout, zero causes to return immediately.
 * If a positive value is set and timeout is reached, NULL is returned and
 * timeout is changed to 0.
 * @return Structure describing the NETCONF session or NULL in case of an error.
 * NULL is also returned in case of timeout, but in that case also timeout
 * value is changed to 0.
 */
struct nc_session* nc_callhome_accept(const char *username, const struct nc_cpblts* cpblts, int *timeout);

#endif /* not DISABLE_LIBSSH */

/**
 * @ingroup callhome
 * @brief Connect NETCONF server to a management center (NETCONF client) using
 * Call Home mechanism
 *
 * Use nc_session_transport() function to specify which transport protocol
 * should be used.
 *
 * Note that reconnect_secs and reconnect_count parameters are used only before
 * a connection is established, then the function returns. It's up to the caller
 * to reconnect if the session goes down. It can be detected using returned PID.
 *
 * To make this function available, you have to include libnetconf_ssh.h or
 * libnetconf_tls.h.
 *
 * @param[in] host_list List of management servers descriptions where the
 * function will try to connect to.
 * @param[in] reconnect_secs Time delay in seconds between connection attempts
 * (even to the same server but it depends on reconnect_count). See
 * /netconf/ssh/call-home/applications/application/reconnect-strategy/interval-secs
 * value in ietf-netconf-server YANG data model.
 * @param[in] reconnect_count Number times the function tries to connect to a
 * single server before moving on to the next server in the host_list. See
 * /netconf/ssh/call-home/applications/application/reconnect-strategy/count-max
 * value in ietf-netconf-server YANG data model.
 * @param[in] server_path Optional parameter to specify path to the transport server.
 * If not specified, the function get transport protocol according to value
 * set by nc_session_transport() (default value is SSH transport). For the
 * NC_TRANSPORT_SSH the '/usr/sbin/sshd' path is used (OpenSSH server), in case
 * of NC_TRANSPORT_TLS the '/usr/sbin/stunnel' path is used (OpenSSL server).
 * @param[in] argv List of arguments to be used by execv() when starting the server
 * specified in server_path parameter. If server_path not specified (NULL), argv
 * is ignored. Remember, that the server is supposed to read data from stdin and
 * write data to stdout (inetd mode). So, for example sshd is running with -i
 * option.
 * @param[out] com_socket If not NULL, function returns TCP socket used for
 * Call Home connection. Caller is supposed to close returned socket when it is
 * no more needed.
 * @return -1 on error. In case of success, function forks the current process
 * running the transport protocol server and returns its PID.
 */
int nc_callhome_connect(struct nc_mngmt_server *host_list, uint8_t reconnect_secs, uint8_t reconnect_count, const char* server_path, char *const argv[], int *com_socket);

#ifdef __cplusplus
}
#endif

#endif /* CALLHOME_H_ */
