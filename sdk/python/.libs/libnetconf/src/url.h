/**
 * \file url.h
 * \author Ondrej Vlk <ondrasek.vlk@gmail.com>
 * \brief libnetconf's public API to use the URL capability.
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

#ifndef DISABLE_URL
#ifndef NC_URL_H_
#define NC_URL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup url
 * @brief List of protocol IDs supported by URL capability implementation.
 * Values are used to enable/disable server support of these protocols
 * (nc_url_enable(), nc_url_disable()).
 */
typedef enum NC_URL_PROTOCOLS {
	NC_URL_UNKNOWN =   0, /**< No protocol. */
	NC_URL_SCP     =   1, /**< SCP (Secure Copy Protocol). */
	NC_URL_HTTP    =   2, /**< HTTP (Hypertext Transfer Protocol). */
	NC_URL_HTTPS   =   4, /**< HTTPS (Hypertext Transfer Protocol Secure). */
	NC_URL_FTP     =   8, /**< FTP (File Transfer Protocol). */
	NC_URL_SFTP    =  16, /**< SFTP (SSH File Transfer Protocol) */
	NC_URL_FTPS    =  32, /**< FTPS (FTP/SSL) */
	NC_URL_FILE    =  64, /**< local file */
	NC_URL_ALL     = 127  /**< All supported protocols */
} NC_URL_PROTOCOLS;

/**
 * @ingroup url
 * @brief Overwrite enabled protocols for URL capability
 * @param protocols binary array of protocol IDs (ORed NC_URL_PROTOCOLS) to be
 * enabled.
 */
void nc_url_set_protocols(int protocols);

/**
 * @ingroup url
 * @brief Enable specific protocol for use in URL capability.
 * @param protocol ID of the protocol to enable.
 */
void nc_url_enable(NC_URL_PROTOCOLS protocol);

/**
 * @ingroup url
 * @brief Disable specific protocol for use in URL capability.
 * @param protocol ID of the protocol to disable.
 */
void nc_url_disable(NC_URL_PROTOCOLS protocol);

#ifdef __cplusplus
}
#endif

#endif /* NC_URL_H_ */
#endif /* DISABLE_URL */
