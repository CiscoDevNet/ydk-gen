/**
 * \file datastore_file.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief NETCONF datastore handling function prototypes and structures for file datastore implementation.
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

#ifndef NC_DATASTORE_FILE_H_
#define NC_DATASTORE_FILE_H_

#include "../../netconf_internal.h"
#include "../datastore_internal.h"
#include <semaphore.h>

/* Unique name prefix of every semaphore created */
#define NCDS_LOCK "/NCDS_FLOCK"

/* Number of seconds waiting for a semaphore increment before
 * giving up and cancelling the locking
 */
#define NCDS_LOCK_TIMEOUT 5

/**
 * @brief File datastore implementation-specific ncds_ds structure.
 */
struct ncds_ds_file {
	/* common part from datastore_internal.h */
	struct ncds_ds ds;

	/* specific part */
	/**
	 * @brief Path to the file containing the configuration data, a single file is
	 * used for all the datastore types (running, startup, candidate).
	 */
	char* path;
	/**
	 * @brief File descriptor of an opened file containing the configuration data
	 */
	FILE* file;
	/**
	 * libxml2's document structure of the datastore
	 */
	xmlDocPtr xml;
	/**
	 * backup libxml2's document structure of the datastore for rollback
	 */
	xmlDocPtr xml_rollback;
	/**
	 * libxml2 Node pointers providing access to individual datastores
	 */
	xmlNodePtr candidate, running, startup;
	/**
	 * locking structure
	 */
	struct ds_lock_s {
		/**
		 * semaphore pointer
		 */
		sem_t * lock;
		/**
		 * signal set before locked
	 	 */
		sigset_t sigset;
		/**
		 * Am I holding the lock
		 */
		int holding_lock;
	} ds_lock;
};

/**
 * @brief Initialization of a file datastore
 *
 * @param[in] ds File datastore structure
 * @return 0 on success, non-zero else
 */
int ncds_file_init(struct ncds_ds* ds);

/**
 * @brief Test if configuration datastore was changed by another process since
 * last access of the caller.
 * @param[in] ds File datastore structure which will be tested.
 * @return 0 as false if the datastore was not updated, 1 if the datastore was
 * changed.
 */
int ncds_file_changed(struct ncds_ds* ds);

/**
 * @brief If possible, rollback the last change of the datastore.
 * @param[in] ds File datastore which will be rolled back.
 * @return 0 on success, non-zero if the operation can not be performed.
 */
int ncds_file_rollback(struct ncds_ds* ds);

/**
 * @brief Perform get-config on the specified repository.
 *
 * @param[in] ds File datastore structure from which the data will be obtained.
 * @param[in] session Session originating the request.
 * @param[in] source Datastore (running, startup, candidate) to get the data from.
 * @param[out] error NETCONF error structure describing the experienced error.
 * @return NULL on error, resulting data on success.
*/
char* ncds_file_getconfig(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE source, struct nc_err** error);

/**
 * @brief Get lock information about the specified NETCONF datastore
 * @param[in] ds File datastore structure that will be checked.
 * @param[in] target NETCONF datastore (runnign, startup, candidate) to be analyzed.
 * @return NULL on error, filled lock information structure on success.
 */
const struct ncds_lockinfo *ncds_file_lockinfo(struct ncds_ds* ds, NC_DATASTORE target);

/**
 * @brief Perform locking of the specified datastore for the specified session.
 *
 * @param[in] ds File datastore structure where the lock should be applied.
 * @param[in] session Session originating the request.
 * @param[in] target Datastore (runnign, startup, candidate) to lock.
 * @param[out] error NETCONF error structure describing the experienced error.
 * @return 0 on success, non-zero on error and error structure is filled.
 */
int ncds_file_lock(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error);

/**
 * @brief Perform unlocking of the specified datastore for the specified session.
 *
 * @param[in] ds File datastore structure where the unlock should be applied.
 * @param[in] session Session originating the request.
 * @param[in] target Datastore (runnign, startup, candidate) to unlock.
 * @param[out] error NETCONF error structure describing the experienced error.
 * @return 0 on success, non-zero on error and error structure is filled.
 */
int ncds_file_unlock(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error);

/**
 * @brief Close the specified datastore and free all the resources.
 * @param[in] datastore Datastore to be closed.
 */
void ncds_file_free(struct ncds_ds* ds);

/**
 * @brief Copy the content of a datastore or externally sent configuration to the other datastore
 *
 * @param ds File datastore structure where the changes will be applied.
 * @param session Session originating the request.
 * @param rpc RPC message with the request. RPC message is used only for access control. If rpc is NULL access control is skipped.
 * @param target Target datastore
 * @param source Source datastore, if the value is NC_DATASTORE_CONFIG then
 * config parameter holds the configration to be copy into the target datastore.
 * @param config Configuration in the form of a serialized XML. The config is
 * used only in case of NC_DATASTORE_CONFIG value of source parameter.
 * @param error NETCONF error structure describing the experienced error.
 * @return 0 on success, non-zero on error and error structure is filled.
 */
int ncds_file_copyconfig(struct ncds_ds *ds, const struct nc_session *session, const nc_rpc* rpc, NC_DATASTORE target, NC_DATASTORE source, char *config, struct nc_err **error);

/**
 * @brief Delete the target datastore
 *
 * @param[in] ds File datastore to be deleted
 * @param[in] session Session requesting the deletion
 * @param[in] target Datastore type (running, startup, candidate)
 * @param[out] error NETCONF error structure
 *
 * @return 0 on success, non-zero on error and error structure is filled.
 */
int ncds_file_deleteconfig(struct ncds_ds * ds, const struct nc_session * session, NC_DATASTORE target, struct nc_err **error);

/**
 * @brief Perform the edit-config operation
 *
 * @param[in] ds File datastore to edit
 * @param[in] session Session sending the edit request
 * @param[in] rpc RPC message with the request. RPC message is used only for access control. If rpc is NULL access control is skipped.
 * @param[in] target Datastore type
 * @param[in] config Edit configuration.
 * @param[in] defop Default edit operation.
 * @param[out] error NETCONF error structure describing the experienced error.
 * @return 0 on success, non-zero on error and error structure is filled.
 */
int ncds_file_editconfig(struct ncds_ds *ds, const struct nc_session * session, const nc_rpc* rpc, NC_DATASTORE target, const char * config, NC_EDIT_DEFOP_TYPE defop, NC_EDIT_ERROPT_TYPE errop, struct nc_err **error);

#endif /* NC_DATASTORE_FILE_H_ */
