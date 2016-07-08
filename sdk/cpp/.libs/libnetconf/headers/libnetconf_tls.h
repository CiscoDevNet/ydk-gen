/**
 * \file libnetconf_tls.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief libnetconf's header for control openssl.
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

#ifndef LIBNETCONF_TLS_H_
#define LIBNETCONF_TLS_H_

#include <openssl/ssl.h>
#include <openssl/x509.h>

#include "libnetconf/netconf.h"
#include "libnetconf/transport.h"
#include "libnetconf/callhome.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup tls NETCONF over TLS
 * \brief libnetconf's functions to use TLS. More information can be found at
 * \ref transport page.
 *
 * These functions are experimental. It is possible, that TLS transport (and
 * mainly certificates management) is not fully implemented in this version.
 *
 * Remember, that to make these functions available, libnetconf must be
 * compiled with --enable-tls configure's option.
 */

/**
 * @ingroup tls
 * @brief Set paths to the client certificate and its private key
 *
 * This function takes effect only on client side. It must be used before
 * establishing NETCONF session (including call home) over TLS.
 *
 * This function is thread-safe. It is supposed to be part of the process of
 * establishing NETCONF session within a single thread:
 * -# Use nc_tls_init() to set client certificate and CA for server certificate
 * verification. Calling this function repeatedly with different parameters
 * changes those parameter for new NETCONF session created after the call. Any
 * currently used NETCONF session will be still using the settings specified
 * before the creation of the NETCONF session.
 * -# Establish NETCONF session using nc_session_connect(). If you don't need
 * to change parameters set in nc_tls_init(), you can call nc_session_connect()
 * repeatedly.
 * -# To properly clean all resources, call nc_tls_destroy(). It will destroy
 * TLS connection context in the current thread.
 *
 * @param[in] peer_cert Path to the file containing client certificate
 * @param[in] peer_key Path to the file containing private key for the client
 * certificate. If NULL, key is expected to be stored in the file specified in
 * cert parameter.
 * @param[in] CAfile Location of the CA certificate used to verify the server
 * certificates. For More info, see documentation for
 * SSL_CTX_load_verify_locations() function from OpenSSL.
 * @param[in] CApath Location of the CA certificates used to verify the server
 * certificates. For More info, see documentation for
 * SSL_CTX_load_verify_locations() function from OpenSSL.
 * @param[in] CRLfile Location of the CRL certificate used to check for
 * revocated certificates.
 * @param[in] CRLpath Locarion of the CRL certificates used to check for
 * revocated certificates.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int nc_tls_init(const char* peer_cert, const char* peer_key, const char *CAfile, const char *CApath, const char *CRLfile, const char *CRLpath);

struct nc_session *nc_session_accept_tls(const struct nc_cpblts* capabilities, const char* username, SSL* tls_sess);

/**
 * @ingroup tls
 * @brief Destroy all resources allocated for preparation of TLS connections.
 *
 * See nc_tls_init() for more information about NETCONF session preparation.
 *
 * To make this function available, you have to include libnetconf_tls.h header
 * file.
 */
void nc_tls_destroy(void);

#ifdef __cplusplus
}
#endif

#endif /* LIBNETCONF_H_ */

