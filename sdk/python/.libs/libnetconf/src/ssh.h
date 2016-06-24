/**
 * \file ssh.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions implementing NETCONF over SSH transport.
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

#ifndef NC_SSH_H_
#define NC_SSH_H_

#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif


struct nc_session *nc_session_connect_ssh(const char* username, const char* host, const char* port, void* ssh_sess);

#ifndef DISABLE_LIBSSH

struct nc_session *nc_session_connect_libssh_socket(const char* username, const char* host, int sock, ssh_session ssh_sess);

struct nc_session *nc_session_connect_libssh_channel(struct nc_session *session);

#else /* DISABLE_LIBSSH */

struct nc_msg* read_hello_openssh(struct nc_session *session);

#endif /* DISABLE_LIBSSH */

#ifdef __cplusplus
}
#endif

#endif /* NC_SSH_H_ */
