/**
 * \file with_defaults.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of NETCONF's with-defaults capability defined in RFC 6243
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


#ifndef NC_WITH_DEFAULTS_H_
#define NC_WITH_DEFAULTS_H_

#include "netconf.h"
#include "messages.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup withdefaults
 * @brief Set the basic mode of the with-defaults capability for a NETCONF server.
 *
 * The default basic mode used by libnetconf is 'explicit'. This function should be
 * used before establishing a NETCONF session to settle on a common set of
 * capabilities between a client and a server.
 *
 * On the client side, this function does not have any effect of setting up a
 * specific mode. It only disables (NCDFLT_MODE_DISABLED) or enables (any other
 * valid value) support for the with-defaults capability.
 *
 * @param[in] mode One of the with-defaults capability basic modes,
 * NCDFLT_MODE_ALL_TAGGED is not a basic mode and such value is ignored.
 */
void ncdflt_set_basic_mode(NCWD_MODE mode);

/**
 * @ingroup withdefaults
 * @brief Disable support for the with-defaults capability. This can be done on
 * either a client or a server.
 *
 * This is an alternative for ncdflt_set_basic_mode(NCDFLT_MODE_DISABLED). To enable
 * the with-defaults capability, use ncdflt_set_basic_mode() to set the with-defaults'
 * basic mode.
 */
#define NCDFLT_DISABLE ncdflt_set_basic_mode(NCWD_MODE_DISABLED)

/**
 * @ingroup withdefaults
 * @brief Get the current set basic mode of the with-defaults capability.
 * @return Current value of the with-defaults' basic mode.
 */
NCWD_MODE ncdflt_get_basic_mode(void);

/**
 * @ingroup withdefaults
 * @brief Set with-defaults modes that are supported.
 *
 * This function should be used before establishing a NETCONF session to settle
 * on a common set of capabilities between a client and a server. On the client side,
 * this function has no effect.
 *
 * @param[in] modes ORed set of the supported NCDFLT_MODE modes. The basic mode
 * is always supported automatically.
 */
void ncdflt_set_supported(NCWD_MODE modes);

/**
 * @ingroup withdefaults
 * @brief Get ORed value containing the currently supported with-defaults modes.
 * @return ORed value containing the currently supported with-defaults modes.
 */
NCWD_MODE ncdflt_get_supported(void);

/**
 * @ingroup withdefaults
 * @brief Get value of the \<with-defaults\> element from the rpc message.
 * @param[in] rpc RPC message to be parsed.
 * @return with-defaults mode of the NETCONF rpc message.
 */
NCWD_MODE ncdflt_rpc_get_withdefaults(const nc_rpc* rpc);

#ifdef __cplusplus
}
#endif

#endif /* NC_WITH_DEFAULTS_H_ */
