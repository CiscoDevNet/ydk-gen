/**
 * \file transapi.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \author David Kupka <david.kupka@cesent.cz>
 * \author Michal Vasko <mvasko@cesnet.cz>
 * \brief Functions implementing libnetconf TransAPI mechanism.
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

#ifndef NC_TRANSAPI_H
#define NC_TRANSAPI_H

//#include <sys/inotify.h>
#include <libxml/tree.h>
#include "netconf.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Current transAPI version */
#define TRANSAPI_VERSION 6

/* maximal number of input arguments every defined RPC can have */
#ifndef MAX_RPC_INPUT_ARGS
#	define MAX_RPC_INPUT_ARGS 64
#endif

/**
 * @ingroup transapi
 * @brief Enum specifying states of node in document
 */
typedef enum
{
	XMLDIFF_ERR = -1 /**< Error while creating XML difftree. */,
	XMLDIFF_NONE = 0 /**< Last operation did not cause any change in configuration. */,
	XMLDIFF_ADD = 1 /**< Element was added to configuration. */,
	XMLDIFF_REM = 2 /**< Element was removed from configuration. */,
	XMLDIFF_MOD = 4/**< Element was modified. */,
	XMLDIFF_CHAIN = 8/**< Some of children of element was modified/added/removed. */,
	XMLDIFF_SIBLING = 16 /**< Some sibling nodes were added/removed/changed position. Only for LEAF and LEAF-LIST. */,
	XMLDIFF_REORDER = 32 /**< Some of the children nodes changed theirs position. None was added/removed. Only for LEAF and LEAF-LIST. */,
} XMLDIFF_OP;

typedef enum TRANSAPI_CLBCKS_ORDER_TYPE {
	TRANSAPI_CLBCKS_LEAF_TO_ROOT,
	TRANSAPI_CLBCKS_ROOT_TO_LEAF,
	TRANSAPI_CLBCKS_ORDER_DEFAULT = TRANSAPI_CLBCKS_LEAF_TO_ROOT,
} TRANSAPI_CLBCKS_ORDER_TYPE;

typedef enum
{
	CLBCKS_APPLIED_NONE,
	CLBCKS_APPLYING_CHILDREN,
	CLBCKS_APPLIED_ERROR,
	CLBCKS_APPLIED_NO_ERROR,
	CLBCKS_APPLIED_CHILDREN_ERROR,
	CLBCKS_APPLIED_CHILDREN_NO_ERROR,
	CLBCKS_APPLIED_NOT_FULLY,
	CLBCKS_APPLIED_FULLY
} CLBCKS_APPLIED;


struct ns_pair {
	const char* prefix;
	const char* href;
};

/**
 * @ingroup transapi
 * @brief Structure to describe transAPI module and connect it statically with
 * libnetconf using ncds_new_transapi_static().
 */
struct transapi {
	/**
	 * @brief transapi version of the module
	 */
	int version;
	/**
	 * @brief Module initialization.
	 */
	int (*init)(xmlDocPtr *);
	/**
	 * @brief Free module resources and prepare for closing.
	 */
	void (*close)(void);
	/**
	 * @brief Function returning status information
	 */
	xmlDocPtr (*get_state)(const xmlDocPtr, const xmlDocPtr, struct nc_err**);
	/**
	 * @brief Callbacks order settings.
	 */
	TRANSAPI_CLBCKS_ORDER_TYPE clbks_order;
	/**
	 * @brief Transapi callback mapping structure.
	 */
	struct transapi_data_callbacks* data_clbks;
	/**
	 * @brief Transapi rpc callbacks mapping structure.
	 */
	struct transapi_rpc_callbacks* rpc_clbks;
	/**
	 * @brief Mapping prefixes with URIs
	 */
	struct ns_pair* ns_mapping;
	/**
	 * @brief Flag if configuration data passed to callbacks were modified
	 */
	int* config_modified;
	/**
	 * @brief edit-config's error-option for the current transaction
	 */
	NC_EDIT_ERROPT_TYPE *erropt;
	/**
	 * @brief Transapi file monitoring structure.
	 */
	struct transapi_file_callbacks* file_clbks;
};

/**
 * @ingroup transapi
 * @brief Structure describing callback - path + function
 */
struct clbk {
	char* path;
	int (*func)(void**, XMLDIFF_OP, xmlNodePtr, xmlNodePtr, struct nc_err**);
};

/**
 * @ingroup transapi
 * @brief Same as transapi_data_callbacks. Using libxml2 structures for callbacks parameters.
 */
struct transapi_data_callbacks {
	int callbacks_count;
	void* data;
	struct clbk callbacks[];
};

/**
 * @ingroup transapi
 * @brief Same as transapi_rpc_callbacks. Using libxml2 structures for callbacks parameters.
 */
struct transapi_rpc_callbacks {
	int callbacks_count;
	struct {
		char* name;
		nc_reply* (*func)(xmlNodePtr);
	} callbacks[];
};

/**
 * @ingroup transapi
 * @brief Functions to call if the specified file is modified.
 *
 * Description of the callback parameters:
 * const char *filename[in] - name of the changed file
 * xmlDocPtr *edit_config[out] - return parameter with edit-config data to
 * to change running datastore. The data are supposed to be enclosed in
 * \<config/\> root element.
 */
struct transapi_file_callbacks {
	int callbacks_count;
	struct {
		const char* path;
		int (*func)(const char*, xmlDocPtr*, int*);
	} callbacks[];
};

#ifdef __cplusplus
}
#endif

#endif /* NC_TRANSAPI_H */
