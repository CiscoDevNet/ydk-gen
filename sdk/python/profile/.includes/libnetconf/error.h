/**
 * \file error.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief NETCONF error handling functions.
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

#ifndef NC_ERROR_H_
#define NC_ERROR_H_

#include "netconf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief NETCONF error structure representation
 * @ingroup genAPI
 */
struct nc_err;

typedef enum {
	NC_ERR_EMPTY,
	NC_ERR_IN_USE,
	NC_ERR_INVALID_VALUE,
	NC_ERR_TOO_BIG,
	NC_ERR_MISSING_ATTR,
	NC_ERR_BAD_ATTR,
	NC_ERR_UNKNOWN_ATTR,
	NC_ERR_MISSING_ELEM,
	NC_ERR_BAD_ELEM,
	NC_ERR_UNKNOWN_ELEM,
	NC_ERR_UNKNOWN_NS,
	NC_ERR_ACCESS_DENIED,
	NC_ERR_LOCK_DENIED,
	NC_ERR_RES_DENIED,
	NC_ERR_ROLLBACK_FAILED,
	NC_ERR_DATA_EXISTS,
	NC_ERR_DATA_MISSING,
	NC_ERR_OP_NOT_SUPPORTED,
	NC_ERR_OP_FAILED,
	NC_ERR_MALFORMED_MSG
}NC_ERR;

/**
 * @ingroup genAPI
 * @brief Create a new error description structure.
 * @param[in] error Predefined NETCONF error (according to RFC 6241 Appendix A).
 * @return Created NETCONF error structure on success, NULL on an error.
 */
struct nc_err* nc_err_new(NC_ERR error);

/**
 * @ingroup genAPI
 * @brief Duplicate an error description structure.
 * @param[in] error Existing NETCONF error description structure to be duplicated.
 * @return Duplicated NETCONF error structure on success, NULL on an error.
 */
struct nc_err* nc_err_dup(const struct nc_err* error);

/**
 * @ingroup genAPI
 * @brief Free NETCONF error structure.
 * @param err NETCONF error structure to free.
 */
void nc_err_free(struct nc_err* err);

/**
 * @ingroup genAPI
 * @brief Set selected parameter of the NETCONF error structure to the specified value.
 * @param err NETCONF error structure to be modified.
 * @param param NETCONF error structure's parameter to be modified.
 * @param value New value for the specified parameter.
 * @return 0 on success\n non-zero on error
 */
int nc_err_set(struct nc_err* err, NC_ERR_PARAM param, const char* value);

/**
 * @ingroup genAPI
 * @brief Get value of the specified parameter of the NETCONF error structure.
 * @param err NETCONF error structure from which the value will be read.
 * @param param NETCONF error structure's parameter to be returned.
 * @return Constant string value of the requested parameter, NULL if the
 * specified parameter is not set.
 */
const char* nc_err_get(const struct nc_err* err, NC_ERR_PARAM param);

#ifdef __cplusplus
}
#endif

#endif /* NC_ERROR_H_ */
