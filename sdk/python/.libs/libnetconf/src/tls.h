/**
 * \file tls.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions implementing NETCONF over TLS transport.
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

#ifndef NC_TLS_H_
#define NC_TLS_H_

#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char* verify_ret_msg[] __attribute__((__unused__)) = {
	"ok",
	"",
	"unable to get issuer certificate",
	"unable to get certificate CRL",
	"unable to decrypt certificate's signature",
	"unable to decrypt CRL's signature",
	"unable to decode issuer public key",
	"certificate signature failure",
	"CRL signature failure",
	"certificate is not yet valid",
	"certificate has expired",
	"CRL is not yet valid",
	"CRL has expired",
	"format error in certificate's notBefore field",
	"format error in certificate's notAfter field",
	"format error in CRL's lastUpdate field",
	"format error in CRL's nextUpdate field",
	"out of memory",
	"self signed certificate",
	"self signed certificate in certificate chain",
	"unable to get local issuer certificate",
	"unable to verify the first certificate",
	"certificate chain too long",
	"certificate revoked",
	"invalid CA certificate",
	"path length constraint exceeded",
	"unsupported certificate purpose",
	"certificate not trusted",
	"certificate rejected",
	"subject issuer mismatch",
	"authority and subject key identifier mismatch",
	"authority and issuer serial number mismatch",
	"key usage does not include certificate signing"
};

struct nc_session *nc_session_connect_tls(const char* username, const char* host, const char* port);

struct nc_session *nc_session_connect_tls_socket(const char* username, const char* host, int sock);

#ifdef __cplusplus
}
#endif

#endif /* NC_TLS_H_ */
