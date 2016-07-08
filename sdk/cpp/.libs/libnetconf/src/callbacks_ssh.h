/**
 * \file callbacks_ssh.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions to set libssh's callbacks.
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

#ifndef NC_CALLBACKS_SSH_H_
#define NC_CALLBACKS_SSH_H_

#ifndef DISABLE_LIBSSH

#include <libssh/libssh.h>

#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set a callback function for passing user credentials into the libssh's
 * keyboard-interactive authentication method
 *
 * To make this function available, you have to include libnetconf_ssh.h.
 *
 * If the func parameter is NULL, the callback is set back to the default function.
 *
 * @ingroup session
 * @param[in] func Callback function to use for interactive authentication.
 */
void nc_callback_sshauth_interactive(char* (*func)(const char* name,
		const char* instruction,
		const char* prompt,
		int echo));

/**
 * @brief Set a callback function for passing the user password into the libssh's
 * password authentication method when connecting to 'hostname' as 'username'.
 *
 * To make this function available, you have to include libnetconf_ssh.h.
 *
 * If the func parameter is NULL, the callback is set back to the default function.
 *
 * @ingroup session
 * @param[in] func Callback function to use. The callback function should return
 * a password string for the given username and name of the remote host.
 */
void nc_callback_sshauth_password(char* (*func)(const char* username,
		const char* hostname));

/**
 * @brief Set a callback function for passing the user password into the libssh's
 * publickey authentication method when connecting to 'hostname' as 'username'.
 *
 * To make this function available, you have to include libnetconf_ssh.h.
 *
 * If the func parameter is NULL, the callback is set back to the default function.
 *
 * @ingroup session
 * @param[in] func Callback function to use.
 */
void nc_callback_sshauth_passphrase(char* (*func)(const char* username,
		const char* hostname, const char* priv_key_file));

/**
 * @ingroup session
 * @brief Set a callback function to authorize authenticity of the remote host.
 *
 * To make this function available, you have to include libnetconf_ssh.h.
 *
 * If the func parameter is NULL, the callback is set back to the default function.
 *
 * @param[in] func Callback function to use. Expected callback return values are:
 * - EXIT_SUCCESS - hosts and keys match, the SSH session establishment will continue.
 * - EXIT_FAILURE - keys do not match or an error occurred.
 */
void nc_callback_ssh_host_authenticity_check(int (*func)(const char* hostname,
		ssh_session session));

/**
 * @brief Set path to a private and a public key file used in case of SSH authentication via
 * a publickey mechanism.
 *
 * To make this function available, you have to include libnetconf_ssh.h.
 *
 * @ingroup session
 * @param[in] privkey Path to the private key file.
 * @param[in] pubkey Path to the public key file
 *
 * @return EXIT_SUCCES or EXIT_FAILURE
 */
int nc_set_keypair_path(const char* privkey, const char* pubkey);

/**
 * @brief Remove a private and a public key file.
 *
 * To make this function available, you have to include libnetconf_ssh.h.
 *
 * @ingroup session
 * @param[in] privkey Path to the private key file.
 * @param[in] pubkey Path to the public key file
 *
 * @return EXIT_SUCCES or EXIT_FAILURE
 */
int nc_del_keypair_path(const char* privkey, const char* pubkey);

#ifdef __cplusplus
}
#endif

#endif /* DISABLE_LIBSSH */

#endif /* NC_CALLBACKS_SSH_H_ */
