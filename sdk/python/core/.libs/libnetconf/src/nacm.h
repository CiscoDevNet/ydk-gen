/**
 * \file nacm.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief NETCONF Access Control Module functions (according to RFC 6536).
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

#ifndef NC_NACM_H_
#define NC_NACM_H_

#include "netconf.h"

#define NACM_DENY true
#define NACM_PERMIT false

#define NACM_ACCESS_CREATE 0x01
#define NACM_ACCESS_READ   0x02
#define NACM_ACCESS_UPDATE 0x04
#define NACM_ACCESS_DELETE 0x08
#define NACM_ACCESS_EXEC   0x10
#define NACM_ACCESS_ALL    0xff

/**
 * @brief Init NACM subsystem
 * @return 0 on success, -1 on error
 */
int nacm_init(void);

/**
 * @brief Close NACM internal structures
 */
void nacm_close(void);

/**
 * @brief Connect current NACM rules with the specified NETCONF RPC
 *
 * This function only prepares NACM structures to be used with the given RPC.
 * No NACM check is done by this function, it only enables further NACM checks
 * according to the NACM rules available at the time of calling nacm_start().
 *
 * @param[in] rpc RPC message where NACM rules will be applied
 * @param[in] session NETCONF session where the RPC came from.
 * @return 0 on success, -1 on error (invalid parameters or memory allocation
 * failure). rpc is not affected in case of error.
 */
int nacm_start(nc_rpc* rpc, const struct nc_session* session);

/**
 * @brief Check if there is a permission to invoke requested protocol operation.
 *
 * @param[in] rpc RPC specifying requested operation
 * @return 0 for permit the operation<br/>
 *         1 for deny the operation<br/>
 *        -1 in case of error
 */
int nacm_check_operation(const nc_rpc* rpc);

/**
 * @brief Check if there is a permission to send the given notification
 * message via the specified session.
 *
 * @param[in] ntf Notification message to be checked
 * @param[in] session NETCONF session where the Notification is going to be sent
 * @return 0 for permit the operation<br/>
 *         1 for deny the operation<br/>
 *        -1 in case of error
 */
#ifndef DISABLE_NOTIFICATIONS
int nacm_check_notification(const nc_ntf* ntf, const struct nc_session* session);
#endif

/**
 * @brief Check if there is a permission to access (read/create/delete/update)
 * the given configuration data node.
 *
 * @param[in] node Configuration data node to be checked
 * @param[in] access Specific access right for manipulation with the node.
 * Value should be one of the NACM_ACCESS_* macros. NACM_ACCESS_ALL and
 * NACM_ACCESS_EXEC are meaningless and the result is unspecified.
 * @param[in] nacm NACM structure from the RPC
 * @return 0 for permit the operation<br/>
 *         1 for deny the operation<br/>
 *        -1 in case of error
 */
int nacm_check_data(const xmlNodePtr node, const int access, const struct nacm_rpc* nacm);

/**
 * @brief Check given document for read access and remove node that are not
 * allowed to be read.
 *
 * @param[in] doc Configuration data document to be checked and modified.
 * @param[in] nacm NACM structure from the RPC
 * @return 0 on success, -1 on error
 */
int nacm_check_data_read(xmlDocPtr doc, const struct nacm_rpc* nacm);

struct rule_list** nacm_rule_lists_dup(struct rule_list** list);
void nacm_rule_list_free(struct rule_list* rl);

#endif /* NC_NACM_H_ */
