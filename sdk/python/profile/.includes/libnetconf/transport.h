/**
 * \file transport.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions implementing transport layer for NETCONF.
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

#ifndef NC_TRANSPORT_H_
#define NC_TRANSPORT_H_

#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup session
 * @brief Set transport protocol for the sessions created by subsequent
 * nc_session_connect() calls. By default, transport protocol is set to
 * #NC_TRANSPORT_SSH
 *
 * This function is thread-safe. Change made by calling this function applies
 * only to the current thread.
 */
int nc_session_transport(NC_TRANSPORT proto);

/**
 * @ingroup session
 * @brief Create NETCONF session to the specified server.
 *
 * This function can internally use various callbacks set by the client to perform
 * SSH authentication. It selects authentication mechanism from the list
 * provided by the SSH server and based on the preferences set by the client via
 * nc_ssh_pref(). Then, appropriate callback function (set by
 * nc_callback_sshauth_password(), nc_callback_sshauth_passphrase(),
 * nc_set_publickey_path() or nc_set_privatekey_path()) is used to perform the
 * authentication.
 *
 * @param[in] host Hostname or address (both Ipv4 and IPv6 are accepted). 'localhost'
 * is used by default if NULL is specified.
 * @param[in] port Port number of the server. Default value 830 is used if 0 is
 * specified.
 * @param[in] username Name of the user to login to the server. The user running the
 * application (detected from the effective UID) is used if NULL is specified.
 * @param[in] cpblts NETCONF capabilities structure with capabilities supported
 * by the client. Client can use nc_session_get_cpblts_default() to get the
 * structure with the list of all the capabilities supported by libnetconf (this is
 * used in case of a NULL parameter).
 * @return Structure describing the NETCONF session or NULL in case of an error.
 */
struct nc_session *nc_session_connect(const char *host, unsigned short port, const char *username, const struct nc_cpblts* cpblts);

/**
 * @ingroup session
 * @brief Create another NETCONF session using already established SSH session.
 * No authentication is needed in this case.
 *
 * This function works only if libnetconf is compiled with using libssh.
 *
 * It is not applicable to the sessions created by nc_session_connect_inout().
 *
 * @param[in] session Already established NETCONF session using nc_session_connect().
 * @param[in] cpblts NETCONF capabilities structure with capabilities supported
 * by the client. Client can use nc_session_get_cpblts_default() to get the
 * structure with the list of all the capabilities supported by libnetconf (this is
 * used in case of a NULL parameter).
 * @return Structure describing the NETCONF session or NULL in case of an error.
 *
 */
struct nc_session *nc_session_connect_channel(struct nc_session *session, const struct nc_cpblts* cpblts);

/**
 * @ingroup session
 * @brief Create NETCONF session communicating via given file descriptors. This
 * is an alternative function to nc_session_connect().
 *
 * In this case the initiation of the transport session (SSH, TLS, ...) is done
 * externally. libnetconf just uses provided file descriptors to read data from
 * and write data to that external entity (process, functions,...).
 *
 * Before calling this function, all necessary authentication process must be
 * done so libnetconf can directly start with \<hello\> messages performing the
 * NETCONF handshake.
 *
 * Since connecting to a host and authentication is done before, the provided
 * host, port, username anf transport arguments are only informative and
 * libnetconf use them only for returning value by nc_session_get_*() functions.
 * The cpblts argument is used during the NETCONF handshake in the same way as
 * in the nc_session_connect() function.
 *
 * It is not allowed to use nc_session_connect_channel() on the session created
 * by this function.
 *
 * @param[in] fd_in Opened file desriptor where the (unencrypted) data from the
 * NETCONF server are read.
 * @param[in] fd_out Opened file desriptor where the (unencrypted) data to the
 * NETCONF server are written.
 * @param[in] cpblts NETCONF capabilities structure with capabilities supported
 * by the client. Client can use nc_session_get_cpblts_default() to get the
 * structure with the list of all the capabilities supported by libnetconf (this is
 * used in case of a NULL parameter).
 * @param[in] host Name of the host where we are connected to via the provided
 * file descriptors.
 * @param[in] port The port number of the remote host where we are connected.
 * @param[in] username Name of the user we are connected to the remote host as.
 * @param[in] transport The transport protocol used to connect to the remote host.
 */
struct nc_session* nc_session_connect_inout(int fd_in, int fd_out, const struct nc_cpblts* cpblts, const char *host, const char *port, const char *username, NC_TRANSPORT transport);

/**
 * @ingroup session
 * @brief Accept NETCONF session from a client.
 *
 * The caller process of this function is supposed to be launched as a
 * subprocess of the transport protocol server (in case of SSH, it is called
 * SSH Subsystem). Username assigned to the NETCONF session is guessed from the
 * process's UID. This approach supposes that the transport protocol server
 * launches the caller process with the changed UID according to the user
 * logged in (OpenSSH's sshd does this, stunnel does not - see
 * nc_session_accept_username() instead of this function).
 *
 * Only one NETCONF session can be accepted in a single caller since it
 * communicates with the transport protocol server directly via (redirected)
 * stdin and stdout streams.
 *
 * @param[in] capabilities NETCONF capabilities structure with the capabilities supported
 * by the server. The caller can use nc_session_get_cpblts_default() to get the
 * structure with the list of all the capabilities supported by libnetconf (this is
 * used in case of a NULL parameter).
 * @return Structure describing the accepted NETCONF session or NULL in case of an error.
 */
struct nc_session *nc_session_accept(const struct nc_cpblts* capabilities);

/**
 * @ingroup session
 * @brief Accept NETCONF session from a client and assign it to the specified
 * username.
 *
 * The same as nc_session_accept() except that instead of guessing username
 * from the process's UID, the specified username is assigned to the NETCONF
 * session. This can be used especially in case that the transport protocol
 * server (sshd, stunnel,...) does not change process's UID automatically.
 *
 * @param[in] capabilities NETCONF capabilities structure with the capabilities supported
 * by the server. The caller can use nc_session_get_cpblts_default() to get the
 * structure with the list of all the capabilities supported by libnetconf (this is
 * used in case of a NULL parameter).
 * @param[in] username Name of the user which will be assigned to the NETCONF
 * session. This information is used for example by NACM subsystem. If NULL,
 * the function act the same way as the nc_session_accept() function.
 * @return Structure describing the accepted NETCONF session or NULL in case of an error.
 */
struct nc_session *nc_session_accept_username(const struct nc_cpblts* capabilities, const char* username);

/**
 * @ingroup session
 * @brief Accept NETCONF session from a client. It allows to assign the
 * specified username to it and set file descriptors for reading/writing NETCONF
 * data.
 *
 * The same as nc_session_accept_username() except that it allows caller to set
 * file descriptors where the libnetconf will read/write NETCONF (unencrypted)
 * data.
 *
 * @param[in] capabilities NETCONF capabilities structure with the capabilities supported
 * by the server. The caller can use nc_session_get_cpblts_default() to get the
 * structure with the list of all the capabilities supported by libnetconf (this is
 * used in case of a NULL parameter).
 * @param[in] username Name of the user which will be assigned to the NETCONF
 * session. This information is used for example by NACM subsystem. If NULL,
 * the function act the same way as the nc_session_accept() function.
 * @param[in] input File descriptor from which the NETCONF data will be read.
 * @param[in] output File descriptor to which the NETCONF data will be written.
 * @return Structure describing the accepted NETCONF session or NULL in case of an error.
 */
struct nc_session *nc_session_accept_inout(const struct nc_cpblts* capabilities, const char* username, int input, int output);

#ifdef __cplusplus
}
#endif

#endif /* NC_TRANSPORT_H_ */
