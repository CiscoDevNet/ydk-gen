/**
 * \file datastore_xml.h
 * \author David Kupka <dkupka@cesnet.cz>
 * \brief NETCONF datastore handling function prototypes and structures - XML variants.
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

#ifndef NC_DATASTORE_XML_H_
#define NC_DATASTORE_XML_H_

#include <libxml/tree.h>

#include "datastore.h"
#include "transapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup store
 * @brief Create a new datastore structure of the specified implementation type with get_state function using libxml2.
 *
 * To make this function available, you have to include libnetconf_xml.h.
 *
 * @param[in] type Datastore implementation type for the new datastore structure.
 * @param[in] model_path Base name of the configuration data model files.
 * libnetconf expects model_path.yin as a data model, model_path.rng for
 * grammar and data types validation, model_path.dsrl for default values
 * validation and model_path.sch for semantic validation.
 * @param[in] get_state Pointer to a callback function that returns a  XML document
 * containing the state data of the device. The parameters it receives are
 * a configuration data model in YIN format and the current content of the running
 * datastore. If NULL is set, \<get\> operation is performed in the same way
 * as \<get-config\>.
 * @return Prepared (not configured) datastore structure. To configure the
 * structure, caller must use the parameter setters of the specific datastore
 * implementation type. Then, the datastore can be initiated (ncds_init()) and
 * used to access the configuration data.
 */
struct ncds_ds* ncds_new2(NCDS_TYPE type, const char * model_path, xmlDocPtr (*get_state)(const xmlDocPtr model, const xmlDocPtr running, struct nc_err **e));

/**
 * @ingroup transapi
 * @brief Create new datastore structure with transaction API support
 *
 * To make this function available, you have to include libnetconf_xml.h.
 *
 * @param[in] type Datastore implementation type for the new datastore structure.
 * @param[in] model_path Base name of the configuration data model files.
 * libnetconf expects model_path.yin as a data model, model_path.rng for
 * grammar and data types validation, model_path.dsrl for default values
 * validation and model_path.sch for semantic validation.
 * @param[in] transapi Structure describing transAPI module. This way the module
 * can be connected with the libnetconf library statically. The structure itself
 * can be freed after the call, but the structure contains only pointers to
 * other structures and variables that will be accessed directly from the
 * subsequent functions using the returned datastore structure. These objects
 * must not be freed during the existence of the returned datastore structure.
 * After ncds_free(), these transAPI variables/structures are not freed.
 * @return Prepared (not configured) datastore structure. To configure the
 * structure, caller must use the parameter setters of the specific datastore
 * implementation type. Then, the datastore can be initiated (ncds_init()) and
 * used to access the configuration data.
 */
struct ncds_ds* ncds_new_transapi_static(NCDS_TYPE type, const char* model_path, const struct transapi* transapi);

/**
 * @ingroup transapi
 * @brief Extend datastore(s) with an augment model and its transAPI callbacks.
 *
 * To make this function available, you have to include libnetconf_xml.h.
 *
 * The function must be called before ncds_consolidate().
 *
 * @param[in] model_path Path of the file containing augment data model in the
 * YIN format. libnetconf accepts data model augmenting multiple base data
 * models linked with the datastores using ncds_new_transapi() function.
 * @param[in] transapi Structure describing transAPI module. This way the module
 * can be connected with the libnetconf library statically. The structure itself
 * can be freed after the call, but the structure contains only pointers to
 * other structures and variables that will be accessed directly from the
 * subsequent functions using the returned datastore structure. These objects
 * must not be freed during the existence of the returned datastore structure.
 * However, these transAPI variables/structures are not freed by libnetconf.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int ncds_add_augment_transapi_static(const char* model_path, const struct transapi* transapi);

/**
 * @ingroup store
 * @brief Set validators (or disable validation) on the specified datastore.
 *
 * To make this function available, you have to include libnetconf_xml.h.
 *
 * @param[in] ds Datastore structure to be configured.
 * @param[in] enable 1 to enable validation on the datastore according to the
 * following parameters, 0 to disable validation (following parameters will be
 * ignored as well as automatically or previously set validators).
 * @param[in] relaxng Path to the Relax NG schema for validation of the
 * datastore content syntax. To generate it, use the lnctool(1) script. NULL
 * if syntactic validation not required.
 * @param[in] schematron Path to the Schematron XSLT stylesheet for validation of
 * the datastore content semantics. To generate it, use the lnctool(1) script.
 * NULL if semantic validation not required.
 * @param[in] valid_func Pointer to a callback function that is used for
 * additional validation of the configuration data in the datastore. It can
 * perform any specific check for the datastore (e.g. check for presence of
 * referred files). If no such check is needed, parameter can be set to NULL.
 * <BR>
 * Validation callback function receives configuration data as a libxml2's
 * xmlDocPtr. As a result it returns EXIT_SUCCESS if validation checks passed
 * and EXIT_FAILURE when an error occurred. An error description may be
 * returned via the \p err parameter.
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int ncds_set_validation2(struct ncds_ds* ds, int enable, const char* relaxng,
    const char* schematron,
    int (*valid_func)(const xmlDocPtr config, struct nc_err **err));

#ifdef __cplusplus
}
#endif

#endif /* NC_DATASTORE_XML_H_ */
