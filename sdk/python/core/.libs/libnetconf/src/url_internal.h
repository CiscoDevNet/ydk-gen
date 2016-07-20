/**
 * \file url_internal.h
 * \author Ondrej Vlk <ondrasek.vlk@gmail.com>
 * \brief libnetconf's internal API to use the URL capability.
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

#ifndef NC_URL_INTERNAL_H_
#define NC_URL_INTERNAL_H_

#include "url.h"
#include "error.h"

/**
 * @brief Get config file from remote source and store it to temporary file
 *
 * Caller HAVE TO close file descriptor returned by this function. Tmp file is
 * unliked by nc_url_open.
 *
 * @param url source url
 * @return fd on tmp file with remote rpc or -1 on error
 */
int nc_url_open(const char* url);

/**
 * @brief Check if the URL exists (can be opened)
 */
int nc_url_check(const char* url);
/**
 * @brief Replaces target file with empty <config> element.
 * @param url target url
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int nc_url_delete_config(const char* url, struct nc_err **e);

/**
 * @brief Uploads data to remote target
 * @param data configuration data enclosed in <config> element
 * @param url target url
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int nc_url_upload(char* data, const char* url, struct nc_err **e);

/**
 * @brief Generate URL capability string from enabled protocols
 * @return capability string, NULL on error
 */
char* nc_url_gencap(void);

/**
 * @brief Check if protocol is enabled
 * @param protocol protocol ID
 * @return 0 if not supported, nonzero otherwise
 */
int nc_url_is_enabled(NC_URL_PROTOCOLS protocol);

/**
 * @brief Get protocol ID from url string
 * @param url url to check
 * @return protocol ID
 */
NC_URL_PROTOCOLS nc_url_get_protocol(const char* url);

#endif /* NC_URL_INTERNAL_H_ */
