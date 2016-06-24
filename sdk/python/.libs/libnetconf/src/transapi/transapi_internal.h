/**
 * \file transapi_internal.h
 * \author David Kupka <dkupka@cesnet.cz>
 * \author Michal Vasko <mvasko@cesent.cz>
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief TransAPI functions and structures for intenal use.
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

#ifndef NC_TRANSAPI_INTERNAL_H_
#define NC_TRANSAPI_INTERNAL_H_

#include "../transapi.h"
#include "yinparser.h"
#include "../datastore/datastore_internal.h"

/**
 * @ingroup transapi
 * @brief Enum with XML relationships between the nodes
 */
typedef enum
{
	XML_PARENT, /**< Represent XML parent role. */
	XML_CHILD, /**< Represent XML child role. */
	XML_SIBLING /**< Represent XML sibling role. */
} XML_RELATION;

#define PRIORITY_NONE -1

/**
 * @ingroup transapi
 * @brief Top level function of transaction API. Finds differences between old_doc and new_doc and calls specified callbacks.
 *
 * @param[in] ds NETCONF datastore structure for access transAPI connected with this datastore
 * @param[in] old_doc Content of configuration datastore before change.
 * @param[in] new_doc Content of configuration datastore after change.
 * @param[in] libxml2 Specify if the module uses libxml2 API
 *
 * @return EXIT_SUCESS or EXIT_FAILURE
 */
int transapi_running_changed(struct ncds_ds* ds, xmlDocPtr old_doc, xmlDocPtr new_doc, NC_EDIT_ERROPT_TYPE erropt, struct nc_err **error);

#endif /* NC_TRANSAPI_INTERNAL_H_ */
