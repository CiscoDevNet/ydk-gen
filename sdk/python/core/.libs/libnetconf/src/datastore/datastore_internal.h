/**
 * \file datastore_internal.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief NETCONF datastore internal function prototypes and structures.
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

#ifndef NC_DATASTORE_INTERNAL_H_
#define NC_DATASTORE_INTERNAL_H_

#include <libxml/tree.h>
#include <libxml/xpath.h>

#ifndef DISABLE_VALIDATION
#  include <libxml/relaxng.h>
#  include <libxslt/xsltInternals.h>
#endif

#include "../transapi.h"
#include "../datastore.h"

#define EXIT_RPC_NOT_APPLICABLE -2

struct ncds_lockinfo {
	NC_DATASTORE datastore;
	char* sid;
	char* time;
};

struct ncds_funcs {
	/**
	 * @brief Initialization of datastore
	 *
	 * @param[in] ds Datastore structure
	 * @return 0 on success, non-zero else
	 */
	int (*init) (struct ncds_ds* ds);
	/**
	 * @brief Close the specified datastore and free all the resources.
	 * @param[in] ds Datastore to be closed.
	 */
	void (*free)(struct ncds_ds* ds);
	/**
	 * @brief Test if configuration datastore was changed by another process since
	 * last access of the caller.
	 * @param[in] ds Datastore structure which will be tested.
	 * @return 0 as false if the datastore was not updated, 1 if the datastore was
	 * changed.
	 */
	int (*was_changed)(struct ncds_ds* ds);
	/**
	 * @brief If possible, rollback the last change of the datastore.
	 * @param[in] ds File datastore which will be rolled back.
	 * @return 0 on success, non-zero if the operation can not be performed.
	 */
	int (*rollback)(struct ncds_ds* ds);
	/**
	 * \TODO
	 *
	 * Returned pointer points to a static area that can be changed by any
	 * subsequent call of get_lockinfo(), lock() or unlock() (may vary
	 * according to a specific datastore implementation).
	 */
	const struct ncds_lockinfo* (*get_lockinfo)(struct ncds_ds* ds, NC_DATASTORE target);
	/**
	 * @brief Lock target datastore for single session exclusive write-access
	 *
	 * @param[in] ds Datastore structure where the lock should be applied.
	 * @param[in] session Session originating the request.
	 * @param[in] target Datastore (runnign, startup, candidate) to lock.
	 * @param[out] error NETCONF error structure describing the experienced error.
	 * @return 0 on success, non-zero on error and error structure is filled.
	 */
	int (*lock)(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error);
	/**
	 * @brief Unlock target datastore if it is locked by session
	 *
	 * @param[in] ds Datastore structure where the unlock should be applied.
	 * @param[in] session Session originating the request.
	 * @param[in] target Datastore (runnign, startup, candidate) to unlock.
	 * @param[out] error NETCONF error structure describing the experienced error.
	 * @return 0 on success, non-zero on error and error structure is filled.
	 */
	int (*unlock)(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error);
	/**
	 * @brief Get configuration data stored in target datastore
	 *
	 * @param[in] ds Datastore structure from which the data will be obtained.
	 * @param[in] session Session originating the request.
	 * @param[in] source Datastore (runnign, startup, candidate) to get the data from.
	 * @param[in] filter NETCONF filter to apply on the resulting data.
	 * @param[out] error NETCONF error structure describing the experienced error.
	 * @return NULL on error, resulting data on success.
	*/
	char* (*getconfig)(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error);
	/**
	 * @brief Copy the content of source datastore or externally sent configuration to target datastore
	 *
	 * @param ds Pointer to the datastore structure
	 * @param session Session which the request is a part of
	 * @param rpc RPC message with the request. RPC message is used only for access control. If rpc is NULL access control is skipped.
	 * @param target Target datastore
	 * @param source Source datastore, if the value is NC_DATASTORE_NONE then the next
	 * parameter holds the configration to copy
	 * @param config Configuration to be used as the source in the form of a serialized XML.
	 * @param error	 Netconf error structure.
	 *
	 * @return EXIT_SUCCESS when done without problems
	 * 	   EXIT_FAILURE when error occured
	 */
	int (*copyconfig)(struct ncds_ds* ds, const struct nc_session* session, const nc_rpc* rpc, NC_DATASTORE target, NC_DATASTORE source, char* config, struct nc_err** error);
	/**
	 * @brief Delete the target datastore
	 *
	 * @param ds Datastore to delete
	 * @param session Session requesting the deletion
	 * @param target Datastore type
	 * @param error Netconf error structure
	 *
	 * @return EXIT_SUCCESS or EXIT_FAILURE
	 */
	int (*deleteconfig)(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error);
	/**
	 * @brief Edit configuration in datastore
	 *
	 * @param ds Datastore to edit
	 * @param session Session sending the edit request
	 * @param rpc RPC message with the request. RPC message is used only for access control. If rpc is NULL access control is skipped.
	 * @param target Datastore type
	 * @param config Edit configuration.
	 * @param defop Default edit operation.
	 * @param errop Edit-config's error-option
	 * @param error Netconf error structure
	 *
	 * @return EXIT_SUCCESS or EXIT_FAILURE
	 */
	int (*editconfig)(struct ncds_ds *ds, const struct nc_session * session, const nc_rpc* rpc, NC_DATASTORE target, const char * config, NC_EDIT_DEFOP_TYPE defop, NC_EDIT_ERROPT_TYPE errop, struct nc_err **error);
};

struct model_feature {
	char* name;
	int enabled;
};

#ifndef DISABLE_VALIDATION
struct model_validators {
	xmlRelaxNGValidCtxtPtr rng;
	xmlRelaxNGPtr rng_schema;
	xsltStylesheetPtr schematron;
	int (*callback)(const xmlDocPtr, struct nc_err **);
};
#endif

struct transapi_list {
	struct transapi_internal *tapi;
	int ref_count;
	struct transapi_list *next;
};

struct data_model {
	/**
	 * @brief Path to the file containing YIN configuration data model
	 */
	char* path;
	/**
	 * @brief Name of the model
	 */
	char* name;
	/**
	 * @brief Revision of the model
	 */
	char* version;
	/**
	 * @brief Namespace of the model
	 */
	char* ns;
	/**
	 * @brief Prefix of the model
	 */
	char* prefix;
	/**
	 * @brief List of defined RPCs
	 */
	char** rpcs;
	/**
	 * @brief List of defined notifications
	 */
	char** notifs;
	/**
	 * @brief YIN configuration data model in the libxml2's document form.
	 */
	xmlDocPtr xml;
	/**
	 * @brief XPath context for model processing
	 */
	xmlXPathContextPtr ctxt;
	/**
	 * @brief The list of enabled features defined in the model
	 */
	struct model_feature** features;
	/**
	 * @brief Link with the appropriate transAPI module, if exists
	 */
	struct transapi_internal* transapi;
};

/*
 * internal transAPI structure covering both ways how to handle transAPI
 * modules:
 * 1) dynamic linking using ncds_new_transapi()
 * 2) static linking using ncds_new_transapi_static()
 *
 * For the 2) there is a public struct transapi that is the same (it can be
 * mapped to this structure) except the last item module which refers to a
 * dynamically loaded object (dlopen()).
 */
struct transapi_internal {
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
	xmlDocPtr (*get_state)(const xmlDocPtr, const xmlDocPtr, struct nc_err **);
	/**
	 * @brief Callbacks order settings.
	 */
	TRANSAPI_CLBCKS_ORDER_TYPE clbks_order;
	/**
	 * @brief Transapi callback mapping structure.
	 */
	struct transapi_data_callbacks * data_clbks;
	/**
	 * @brief Transapi rpc callbacks mapping structure.
	 */
	struct transapi_rpc_callbacks * rpc_clbks;
	/**
	 * @brief Mapping prefixes with URIs
	 */
	struct ns_pair *ns_mapping;
	/**
	 * @brief Flag if configuration data passed to callbacks were modified
	 */
	int *config_modified;
	/**
	 * @brief edit-config's error-option for the current transaction
	 */
	NC_EDIT_ERROPT_TYPE *erropt;
	/**
	 * @brief Transapi file monitoring structure.
	 */
	struct transapi_file_callbacks* file_clbks;

	/* internal specific part */
	/**
	 * @brief Loaded shared library with transapi callbacks.
	 */
	void *module;
	/**
	 * @brief Link with the appropriate data_model structure
	 */
	struct data_model* model;
	/**
	 * @brief File monitoring thread, connected with the file_clbks.
	 */
	pthread_t fmon_thread;
};

struct model_list {
	struct data_model *model;
	struct model_list* next;
};

struct ncds_ds {
	/**
	 * @brief Datastore implementation type
	 */
	NCDS_TYPE type;
	/**
	 * @brief Datastore ID: 0 - uninitiated datastore, positive value - valid ID
	 */
	ncds_id id;
	/**
	 * @brief Time of the last access to the configuration datastore.
	 */
	time_t last_access;
	/**
	 * @brief Lock for serialized access/modification of the datastore.
	 */
	pthread_mutex_t lock;
	/**
	 * @brief Pointer to a callback function implementing the retrieval of the
	 * device status data.
	 */
	char* (*get_state)(const char* model, const char* running, struct nc_err ** e);
	/**
	 * @brief Pointer to a xml version of callback function implementing the
	 * retrieval of the device status data.
	 */
	xmlDocPtr (*get_state_xml)(const xmlDocPtr model, const xmlDocPtr running, struct nc_err **e);
	/**
	 * @brief Datastore implementation functions.
	 */
	struct ncds_funcs func;
	/**
	 * @brief Compounded data model containing base data model extended by
	 * all augment models
	 */
	xmlDocPtr ext_model;
	/**
	 * @brief Parsed extended data model structure.
	 */
	struct model_tree* ext_model_tree;

#ifndef DISABLE_VALIDATION
	/**
	 * @brief Configuration data model validators
	 */
	struct model_validators validators;
#endif

	/**
	 * @brief Information about base data model linked with the datastore
	 */
	struct data_model* data_model;
	/**
	 * @brief TransAPI information
	 */
	struct transapi_list* transapis;
	/**
	 * @brief Compound list of all transAPI callbacks applicable to this datastore
	 */
	struct clbk *tapi_callbacks;
	int tapi_callbacks_count;
};

#endif /* NC_DATASTORE_INTERNAL_H_ */
