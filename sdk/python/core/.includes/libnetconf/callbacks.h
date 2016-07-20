/**
 * \file callbacks.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions to set application's callbacks.
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

#ifndef NC_CALLBACKS_H_
#define NC_CALLBACKS_H_

#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set a callback function for printing libnetconf's messages.
 * @ingroup genAPI
 *
 * If the func parameter is NULL, the callback is set back to the default (no)
 * function.
 *
 * @param[in] func Callback function to use.
 */
void nc_callback_print(void (*func)(NC_VERB_LEVEL level, const char* msg));

/**
 * @brief Set a callback function to process (e.g. print) NETCONF \<rpc-error\> message items.
 * @ingroup reply
 *
 * If the func parameter is NULL, the callback is set back to the default (no)
 * function.
 *
 * @param[in] func Callback function to use. Passed parameters are:
 * - tag - error tag,
 * - type - error layer where the error occurred,
 * - severity - error severity,
 * - apptag - the data-model-specific or implementation-specific error condition, if one exists,
 * - path - XPATH expression identifying element with the error,
 * - message - human-readable description of the error,
 * - attribute - name of the data-model-specific XML attribute that caused the error,
 * - element - name of the data-model-specific XML element that caused the error,
 * - ns - name of the unexpected XML namespace that caused the error,
 * - sid - session ID of the session holding the requested lock.
 */
void nc_callback_error_reply(void (*func)(const char* tag,
		const char* type,
		const char* severity,
		const char* apptag,
		const char* path,
		const char* message,
		const char* attribute,
		const char* element,
		const char* ns,
		const char* sid));

#ifdef __cplusplus
}
#endif

#endif /* NC_CALLBACKS_H_ */
