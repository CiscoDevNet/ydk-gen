/**
 * \file datastore.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of NETCONF datastore handling functions.
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

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <libgen.h>
#include <time.h>

#include <libxml/tree.h>

#include "../../netconf_internal.h"
#include "../../error.h"
#include "../../session.h"
#include "../../nacm.h"
#include "../../config.h"
#include "../datastore_internal.h"
#include "datastore_file.h"
#include "../edit_config.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

#define FILEDSFRAME "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<datastores xmlns=\"urn:cesnet:tmc:datastores:file\">\
  <running lock=\"\"/>\
  <startup lock=\"\"/>\
  <candidate modified=\"false\" lock=\"\"/>\
</datastores>"

static struct timespec tv_timeout;
static sigset_t fullsigset;

#define LOCK(file_ds, ret) {\
	sigfillset(&fullsigset);\
	sigprocmask(SIG_SETMASK, &fullsigset, &(file_ds->ds_lock.sigset));\
	tv_timeout.tv_sec += NCDS_LOCK_TIMEOUT;\
	ret = 0;\
	file_ds->ds_lock.holding_lock = 1;\
}
#define UNLOCK(file_ds) {\
	sem_post(file_ds->ds_lock.lock);\
	file_ds->ds_lock.holding_lock = 0;\
	sigprocmask(SIG_SETMASK, &(file_ds->ds_lock.sigset), NULL);\
}

/**
 * @brief Determine if the datastore is accessible (is not NETCONF locked) for the
 * specified session. This function MUST be called between LOCK and UNLOCK
 * macros, which serialize access to the datastore.
 *
 * @param ds Datastore to verify
 * @param target Datastore type
 * @param session Session to test accessibility for
 *
 * @return 0 when all the tests passed and the caller session can work with the target
 * datastore, non-zero else.
 */
static int file_ds_access(struct ncds_ds_file* file_ds, NC_DATASTORE target, const struct nc_session* session)
{
	xmlChar * lock;
	xmlNodePtr target_ds;
	int retval;

	if (file_ds == NULL) {
		ERROR("%s: invalid datastore structure.", __func__);
		return (EXIT_FAILURE);
	}

	switch(target) {
	case NC_DATASTORE_RUNNING:
		target_ds = file_ds->running;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		break;
	default:
		ERROR("%s: invalid target.", __func__);
		return (EXIT_FAILURE);
		break;
	}

	lock = xmlGetProp (target_ds, BAD_CAST "lock");
	if (xmlStrEqual (lock, BAD_CAST "")) {
		retval = EXIT_SUCCESS;
	} else if (session != NULL && xmlStrEqual (lock, BAD_CAST session->session_id)) {
		retval = EXIT_SUCCESS;
	} else {
		retval = EXIT_FAILURE;
	}

	xmlFree (lock);
	return retval;
}

API int ncds_file_set_path(struct ncds_ds* datastore, const char* path)
{
	struct ncds_ds_file * file_ds = (struct ncds_ds_file*)datastore;
	mode_t mask;

	if (datastore == NULL) {
		ERROR ("Invalid datastore.");
		return -1;
	}

	if (path == NULL) {
		ERROR ("Invalid path.");
		return -2;
	}

	if (eaccess (path, F_OK) != 0) {
		/* file does not exist */
		WARN ("Datastore file %s does not exist, creating it.", path);
		/* try to create it */
		mask = umask(MASK_PERM);
		file_ds->file = fopen (path, "w+");
		umask(mask);
		if (file_ds->file == NULL) {
			ERROR ("Datastore file %s cannot be created (%s).", path, strerror(errno));
			return (-2);
		} else {
			VERB ("Datastore file %s was created.", path);
		}
	} else if (eaccess (path, W_OK|R_OK) != 0) {
		ERROR ("Insufficient rights for manipulation with the datastore file %s (%s).", path, strerror(errno));
		return (-2);
	} else {
		/* file exists and it is accessible */
		file_ds->file = fopen (path, "r+");
		if (file_ds->file == NULL) {
			ERROR ("Datastore file %s cannot be opened (%s).", path, strerror(errno));
			return -2;
		}
	}
	file_ds->path = strdup(path);

	return 0;
}

/**
 * @brief Checks if the structure of an XML matches the expected one
 * @param[in] doc Document to check.
 * @return non-zero if matches zero if not.
 */
static int file_structure_check(xmlDocPtr doc)
{
	xmlNodePtr root, ds;
	int running = 0, candidate = 0, startup = 0;

	root = xmlDocGetRootElement (doc);
	if (root == NULL ||!xmlStrEqual (root->name, BAD_CAST "datastores")) {
		return 0;
	}

	for (ds = root->children; ds != NULL; ds = ds->next) {
		if (ds->type != XML_ELEMENT_NODE) {
			continue;
		}
		if (xmlStrEqual (ds->name, BAD_CAST "candidate")) {
			if (candidate) {
				ERROR ("Duplicate datastore candidate found.");
				return 0;
			} else {
				candidate = 1;
			}
		} else if (xmlStrEqual (ds->name, BAD_CAST "running")) {
			if (running) {
				ERROR ("Duplicate datastore running found.");
				return 0;
			} else {
				running = 1;
			}
		} else if (xmlStrEqual (ds->name, BAD_CAST "startup")) {
			if (startup) {
				ERROR ("Duplicate datastore startup found.");
				return 0;
			} else {
				startup = 1;
			}
		} else {
			VERB ("File datastore structure check: ignoring unknown element %s.", ds->name);
		}
	}

	if (candidate && running && startup) {
		return 1;
	}

	return 0;
}

/**
 * @brief Create xml frame of the file datastore
 * @return xml document holding the basic structure
 */
static xmlDocPtr file_create_xmlframe(void)
{
	xmlDocPtr doc;

	doc = xmlReadDoc(BAD_CAST FILEDSFRAME, NULL, NULL, NC_XMLREAD_OPTIONS);
	if (doc == NULL) {
		ERROR ("%s: creating an empty file datastore failed.", __func__);
		return (NULL);
	}

	return doc;
}

static int file_fill_dsnodes(struct ncds_ds_file* ds)
{
	xmlNodePtr aux;

	if (ds == NULL || ds->xml == NULL || ds->xml->children == NULL) {
		ERROR("%s: invalid input parameter.", __func__);
		return (EXIT_FAILURE);
	}
	ds->running = NULL;
	ds->startup = NULL;
	ds->candidate = NULL;

	for (aux = ds->xml->children->children; aux != NULL; aux = aux->next) {
		if (xmlStrcmp(aux->name, BAD_CAST "running") == 0) {
			if (ds->running != NULL) {
				goto invalid_ds;
			} else {
				ds->running = aux;
			}
		}else if (xmlStrcmp(aux->name, BAD_CAST "startup") == 0) {
			if (ds->startup != NULL) {
				goto invalid_ds;
			} else {
				ds->startup = aux;
			}
		}else if (xmlStrcmp(aux->name, BAD_CAST "candidate") == 0) {
			if (ds->candidate != NULL) {
				goto invalid_ds;
			} else {
				ds->candidate = aux;
			}
		}
		/* else - ignore such unknown nodes until we get all required nodes */
	}

	if (ds->running == NULL || ds->startup == NULL || ds->candidate == NULL) {
		/* xml structure of the file datastore is invalid */
		goto invalid_ds;
	}

	return (EXIT_SUCCESS);

invalid_ds:
	ds->running = NULL;
	ds->startup = NULL;
	ds->candidate = NULL;
	return (EXIT_FAILURE);
}

int ncds_file_changed(struct ncds_ds* ds)
{
	time_t t;
	struct stat statbuf;

	/* get current time */
	if ((t = time(NULL)) == ((time_t)(-1))) {
		ERROR("time() failed (%s)", strerror(errno));
		/* we do not know, so answer that file was changed */
		return (1);
	}

	/* check when the file was modified */
	if (stat(((struct ncds_ds_file*)ds)->path, &statbuf) == 0) {
		if (statbuf.st_mtime < ds->last_access) {
			/* file was not modified */
			return (0);
		}
	}
	return (1);
}

/**
 * @ingroup store
 * @brief Initialization of the file datastore
 *
 * @file_ds File datastore structure
 *
 * @return 0 on success, non-zero else
 */
int ncds_file_init(struct ncds_ds* ds)
{
	struct stat st;
	char* new_path = NULL, *sempath, *dir_name, *file_name, *dup_path;
	struct dirent * file_info;
	DIR * dir;
	int fd;
	mode_t mask;
	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;

	file_ds->xml = xmlReadFile(file_ds->path, NULL, NC_XMLREAD_OPTIONS);
	while (file_ds->xml == NULL || file_structure_check(file_ds->xml) == 0) { /* while is used for break */
		WARN("Failed to parse the datastore (%s).", file_ds->path);
		/*
		 * Try to use a backup datastore that we can use instead the main one
		 */
		if (stat(file_ds->path, &st) == -1) {
			ERROR("Unable to work with datastore file (%s), trying to use a backup datastore.", strerror(errno));
		} else if (st.st_size > 0) {
			/* there are some data in the datastore file */
			WARN("Datastore file contains some data, trying to use a backup datastore...");
		} else { /* st.st_size == 0 */
			/* nothing needed, just use the datastore file and create an empty datastore inside it */
			break;
		}

		/* create a temporary file where an empty datastore will be created */
		dup_path = strdup(file_ds->path);
		file_name = basename(dup_path);
		dir_name = dirname(dup_path);

		/* cleanup so far structures because new will be created */
		fclose(file_ds->file);
		xmlFreeDoc(file_ds->xml);
		file_ds->file = NULL;
		file_ds->xml = NULL;

		/* checking if there are any valid backup datastores */
		if ((dir = opendir(dir_name)) == NULL) {
			ERROR("Unable to open datastore directory %s (%s).", dir_name, strerror(errno));
			free(dup_path);
			return (EXIT_FAILURE);
		}

		for (errno = 0; (file_info = readdir(dir)) != NULL;) {
			if (errno) {
				ERROR("Unable to read datastore directory %s (%s).", dir_name, strerror(errno));
				free(dup_path);
				return (EXIT_FAILURE);
			}

			/* check if readed file contains original file name */
			if (strncmp(file_info->d_name, file_name, strlen(file_name)) != 0) {
				continue;
			}

			/* we have some file that could be a backup datastore from some previous run */
			if (asprintf(&new_path, "%s/%s", dir_name, file_info->d_name) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				free(dup_path);
				return (EXIT_FAILURE);
			}
			nc_clip_occurences_with(new_path, '/', '/');

			file_ds->xml = xmlReadFile(new_path, NULL, NC_XMLREAD_OPTIONS);
			if (file_ds->xml == NULL || file_structure_check(file_ds->xml) == 0) {
				/* bad backup datastore, try another one */
				free(new_path);
				new_path = NULL;
				continue;
			} else {
				/* we found a correct backup datastore, use it */
				WARN("Using %s as a backup datastore.", new_path);
				file_ds->file = fopen(new_path, "r+");
				if (file_ds->file == NULL) {
					/* ok, that is not so correct as we thought, wtf?!? */
					ERROR("Unable to open backup datastore (%s)", strerror(errno));
					xmlFreeDoc(file_ds->xml);
					file_ds->xml = NULL;
					free(new_path);
					new_path = NULL;

					/* try another file in backup datastore directory */
					continue;
				}

				/* now it is surely correct, not like a few lines above :) */
				free(file_ds->path);
				file_ds->path = new_path;
				break;
			}
		}

		free(dup_path);
		closedir(dir);

		if (file_ds->file == NULL) {
			/* no previous backup datastore found, create a new one */

			/* Create file based on original name */
			if (asprintf(&new_path, "%s.XXXXXX", file_ds->path) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				return (EXIT_FAILURE);
			}
			nc_clip_occurences_with(new_path, '/', '/');
			WARN("Using %s as a backup datastore.", new_path);
			if ((fd = mkstemp(new_path)) == -1) {
				ERROR("Unable to create backup datastore (%s).", strerror(errno));
				free(new_path);
				return (EXIT_FAILURE);
			}
			if ((file_ds->file = fdopen(fd, "r+")) == NULL) {
				ERROR("Unable to open backup datastore (%s)", strerror(errno));
				free(new_path);
				close(fd);
				return (EXIT_FAILURE);
			}
			free(file_ds->path);
			file_ds->path = new_path;
		}

		/* end the while loop */
		break;
	}

	/* if necessary, create an empty datastore */
	if (file_ds->xml == NULL) {

		file_ds->xml = file_create_xmlframe();
		if (file_ds->xml == NULL) {
			return (EXIT_FAILURE);
		}
		xmlDocFormatDump(file_ds->file, file_ds->xml, 1);
		WARN("File %s was empty. Basic structure created.", file_ds->path);
	}

	/* init value */
	file_ds->xml_rollback = NULL;

	/* get pointers to running, startup and candidate nodes in xml */
	if (file_fill_dsnodes(file_ds) != EXIT_SUCCESS) {
		return (EXIT_FAILURE);
	}

	/* unlock forgotten locks if any */
	xmlSetProp (file_ds->running, BAD_CAST "lock", BAD_CAST "");
	xmlSetProp (file_ds->startup, BAD_CAST "lock", BAD_CAST "");
	xmlSetProp (file_ds->candidate, BAD_CAST "lock", BAD_CAST "");

	/*
	 * open and eventually create a lock
	 */
	/* first - prepare the path, there must be a separate lock for each
	 * datastore(set), so name it according to the filepath with a special prefix.
	 * Slashes in the path are replaced with underscores.
	 * Sequences of slashes are treated as a single slash character.
	 */
	if (asprintf(&sempath, "%s/%s", NCDS_LOCK, file_ds->path) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (EXIT_FAILURE);
	}
	nc_clip_occurences_with(sempath, '/', '_');
	/* recreate initial backslash in the semaphore name */
	sempath[0] = '/';
	/* and then create the lock (actually it is a semaphore) */
	mask = umask(0000);
	if ((file_ds->ds_lock.lock = sem_open (sempath, O_CREAT, FILE_PERM, 1)) == SEM_FAILED) {
		umask(mask);
		return (EXIT_FAILURE);
	}
	umask(mask);
	free (sempath);

	return (EXIT_SUCCESS);
}

void ncds_file_free(struct ncds_ds* ds)
{
	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;

	if (file_ds != NULL) {
		/* ncds_ds_file specific part */
		if (file_ds->file != NULL) {
			fclose(file_ds->file);
		}
		free(file_ds->path);
		xmlFreeDoc(file_ds->xml);
		xmlFreeDoc(file_ds->xml_rollback);
		if (file_ds->ds_lock.lock != NULL) {
			if (file_ds->ds_lock.holding_lock) {
				sem_post (file_ds->ds_lock.lock);
			}
			sem_close(file_ds->ds_lock.lock);
		}
	}
}

/**
 * @brief Reloads xml configuration from the datastorage file. This function MUST be
 * called ONLY between file_ds_lock() and file_ds_unlock().
 *
 * Tries to read from the datastore and find the datastore root elements.
 * If succussfully, the old xml is freed and replaced with a new one.
 * If it fails, the structure is preserved as it was.
 *
 * @param file_ds Pointer to the datastorage structure
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int file_reload(struct ncds_ds_file* file_ds)
{
	xmlDocPtr new_xml;
	struct stat statbuf;
	time_t t;

	if (file_ds == NULL || !file_ds->ds_lock.holding_lock) {
		ERROR("%s: invalid parameter.", __func__);
		return EXIT_FAILURE;
	}

	/* get current time */
	if ((t = time(NULL)) == ((time_t)(-1))) {
		t = 0;
		WARN("Setting datastore access time failed (%s)", strerror(errno));
	}

	/* check when the file was modified */
	if (stat(file_ds->path, &statbuf) == 0) {
		if (statbuf.st_mtime < file_ds->ds.last_access) {
			/* file was not modified */
			return (EXIT_SUCCESS);
		}
	}

	/* file was modified, it may be necessary to reopen it */
	fclose(file_ds->file);
	file_ds->file = fopen(file_ds->path, "r+");
	if (file_ds->file == NULL) {
		ERROR("%s: reopenening the file %s failed (%s)", __func__, file_ds->path, strerror(errno));
		return EXIT_FAILURE;
	}

	new_xml = xmlReadFile (file_ds->path, NULL, NC_XMLREAD_OPTIONS);
	if (new_xml == NULL) {
		return EXIT_FAILURE;
	}

	xmlFreeDoc (file_ds->xml);
	file_ds->xml = new_xml;

	if (file_fill_dsnodes (file_ds)) {
		xmlFreeDoc (new_xml);
		return EXIT_FAILURE;
	}

	/* update access time */
	file_ds->ds.last_access = t;

	return EXIT_SUCCESS;

}

/**
 * @brief Write the current version of the configuration to a file. This function MUST be
 * called ONLY between file_ds_lock() and file_ds_unlock().
 *
 * @param file_ds Datastore to sync.
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int file_sync(struct ncds_ds_file* file_ds)
{
	time_t t;

	if (file_ds == NULL || !file_ds->ds_lock.holding_lock) {
		ERROR("%s: invalid parameter.", __func__);
		return EXIT_FAILURE;
	}

	/* erase actual config */
	if (ftruncate (fileno(file_ds->file), 0) == -1) {
		ERROR ("%s: truncate() of file %s failed (%s)", __func__, file_ds->path, strerror(errno));
		return EXIT_FAILURE;
	}
	rewind (file_ds->file);

	if(xmlDocFormatDump(file_ds->file, file_ds->xml, 1) == -1) {
		ERROR("%s: storing repository into the file %s failed.", __func__, file_ds->path);
		return (EXIT_FAILURE);
	}

	/* update last access time */
	if ((t = time(NULL)) == ((time_t)(-1))) {
		WARN("Setting datastore access time failed (%s)", strerror(errno));
	} else {
		file_ds->ds.last_access = t;
	}

	return EXIT_SUCCESS;
}

static int file_rollback_store(struct ncds_ds_file* file_ds)
{
	if (file_ds == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	xmlFreeDoc(file_ds->xml_rollback);
	file_ds->xml_rollback = xmlCopyDoc(file_ds->xml, 1);

	return (EXIT_SUCCESS);
}

static int file_rollback_restore(struct ncds_ds_file* file_ds)
{
	if (file_ds == NULL || !file_ds->ds_lock.holding_lock) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	if (file_ds->xml_rollback == NULL) {
		ERROR("No backup repository for rollback operation (datastore %d).", file_ds->ds.id);
		return (EXIT_FAILURE);
	}

	xmlFreeDoc(file_ds->xml);
	file_ds->xml = file_ds->xml_rollback;
	file_ds->xml_rollback = NULL;
	file_ds->ds.last_access = 0;

	return (file_sync(file_ds));
}

int ncds_file_rollback(struct ncds_ds* ds)
{
	int ret;

	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;

	if (file_ds == NULL || file_ds->ds.type != NCDS_TYPE_FILE) {
		return (EXIT_FAILURE);
	}

	LOCK(file_ds, ret);
	if (ret) {
		return (EXIT_FAILURE);
	}
	ret = file_rollback_restore(file_ds);
	UNLOCK(file_ds);

	return (ret);
}

static struct ncds_lockinfo lockinfo_running = {NC_DATASTORE_RUNNING, NULL, NULL};
static struct ncds_lockinfo lockinfo_startup = {NC_DATASTORE_STARTUP, NULL, NULL};
static struct ncds_lockinfo lockinfo_candidate = {NC_DATASTORE_CANDIDATE, NULL, NULL};
const struct ncds_lockinfo *ncds_file_lockinfo(struct ncds_ds* ds, NC_DATASTORE target)
{
	int ret;
	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;
	xmlNodePtr target_ds;
	struct ncds_lockinfo *info;

	LOCK(file_ds, ret);
	if (ret) {
		return (NULL);
	}

	if (file_reload (file_ds)) {
		UNLOCK(file_ds);
		return (NULL);
	}

	/* check validity of function parameters */
	switch(target) {
	case NC_DATASTORE_RUNNING:
		target_ds = file_ds->running;
		info = &lockinfo_running;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		info = &lockinfo_startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		info = &lockinfo_candidate;
		break;
	default:
		UNLOCK(file_ds);
		return (NULL);
		break;
	}
	free((*info).sid);
	free((*info).time);
	(*info).sid = (char*) xmlGetProp (target_ds, BAD_CAST "lock");
	(*info).time = (char*) xmlGetProp (target_ds, BAD_CAST "locktime");
	if ((*info).sid == NULL) {
		WARN("%s: missing the \"lock\" attribute in the %s datastore.", __func__, file_ds->ds.data_model->name);
	} else if (strisempty((*info).sid)) {
		free((*info).sid);
		free((*info).time);
		(*info).sid = NULL;
		(*info).time = NULL;
	}

	UNLOCK(file_ds);

	return (info);
}

int ncds_file_lock(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error)
{
	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;
	xmlChar* lock, *modified = NULL;
	xmlNodePtr target_ds;
	struct nc_session* no_session;
	int retval = EXIT_SUCCESS, ret;
	char* t;

	assert(error);

	LOCK(file_ds, ret);
	if (ret) {
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Locking datastore file timeouted.");
		return EXIT_FAILURE;
	}

	if (file_reload (file_ds)) {
		UNLOCK(file_ds);
		return EXIT_FAILURE;
	}

	/* check validity of function parameters */
	switch(target) {
	case NC_DATASTORE_RUNNING:
		target_ds = file_ds->running;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		break;
	default:
		UNLOCK(file_ds);
		ERROR("%s: invalid target.", __func__);
		*error = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "target");
		return (EXIT_FAILURE);
		break;
	}

	/* check if repository is locked by anyone including me */
	no_session = nc_session_dummy(INTERNAL_DUMMY_ID, session->username, session->hostname, session->capabilities);
	if (file_ds_access (file_ds, target, no_session) != 0) {
		/* someone is already holding the lock */
		lock = xmlGetProp (target_ds, BAD_CAST "lock");
		*error = nc_err_new(NC_ERR_LOCK_DENIED);
		nc_err_set(*error, NC_ERR_PARAM_INFO_SID, (char*)lock);
		xmlFree(lock);
		retval = EXIT_FAILURE;
	} else {
		if (target == NC_DATASTORE_CANDIDATE &&
				(modified = xmlGetProp(target_ds, BAD_CAST "modified")) != NULL &&
				xmlStrcmp(modified, BAD_CAST "true") == 0) {
			*error = nc_err_new(NC_ERR_LOCK_DENIED);
			nc_err_set(*error, NC_ERR_PARAM_MSG, "Candidate datastore not locked but already modified.");
			retval = EXIT_FAILURE;
		} else {
			xmlSetProp (target_ds, BAD_CAST "lock", BAD_CAST session->session_id);
			xmlSetProp (target_ds, BAD_CAST "locktime", BAD_CAST (t = nc_time2datetime(time(NULL), NULL)));
			free(t);
			if (file_sync(file_ds)) {
				*error = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(*error, NC_ERR_PARAM_MSG, "Datastore file synchronisation failed.");
				retval = EXIT_FAILURE;
			}
		}
		xmlFree(modified);
	}
	UNLOCK(file_ds);

	/* cleanup */
	nc_session_free(no_session);

	return (retval);
}

int ncds_file_unlock(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE target, struct nc_err** error)
{
	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;
	xmlNodePtr target_ds, del;
	struct nc_session* no_session;
	int retval = EXIT_SUCCESS, ret;

	assert(error);

	LOCK(file_ds, ret);
	if (ret) {
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Locking datastore file timeouted.");
		return EXIT_FAILURE;
	}

	if (file_reload (file_ds)) {
		UNLOCK(file_ds);
		return EXIT_FAILURE;
	}

	/* check validity of function parameters */
	switch(target) {
	case NC_DATASTORE_RUNNING:
		target_ds = file_ds->running;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		break;
	default:
		UNLOCK(file_ds);
		ERROR("%s: invalid target.", __func__);
		*error = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "target");
		return (EXIT_FAILURE);
		break;
	}

	/* check if repository is locked */
	no_session = nc_session_dummy(INTERNAL_DUMMY_ID, session->username, session->hostname, session->capabilities);
	if (file_ds_access (file_ds, target, no_session) == 0) {
		/* not locked */
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Target datastore is not locked.");
		retval = EXIT_FAILURE;
	} else if (file_ds_access (file_ds, target, session) != 0) {
		/* the datastore is locked by somebody else */
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Target datastore is locked by another session.");
		retval = EXIT_FAILURE;
	} else {
		/* the datastore is locked by request originating session */

		if (target == NC_DATASTORE_CANDIDATE) {
			/* drop current candidate configuration */
			while ((del = file_ds->candidate->children) != NULL) {
				xmlUnlinkNode (file_ds->candidate->children);
				xmlFreeNode (del);
			}

			/* copy running into candidate configuration */
			xmlAddChildList(file_ds->candidate, xmlCopyNodeList(file_ds->running->children));

			/* mark candidate as not modified */
			xmlSetProp (target_ds, BAD_CAST "modified", BAD_CAST "false");
		}

		/* unlock datastore */
		xmlSetProp (target_ds, BAD_CAST "lock", BAD_CAST "");
		xmlSetProp (target_ds, BAD_CAST "locktime", BAD_CAST "");
		if (file_sync(file_ds)) {
			*error = nc_err_new(NC_ERR_OP_FAILED);
			nc_err_set(*error, NC_ERR_PARAM_MSG, "Datastore file synchronisation failed.");
			retval = EXIT_FAILURE;
		}
	}

	UNLOCK(file_ds);

	/* cleanup */
	if (no_session != NULL) {
		nc_session_free(no_session);
	}

	return (retval);
}

char* ncds_file_getconfig(struct ncds_ds* ds, const struct nc_session* UNUSED(session), NC_DATASTORE source, struct nc_err** error)
{
	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;
	xmlNodePtr target_ds, aux_node;
	xmlBufferPtr resultbuffer;
	char* data = NULL;
	int ret;

	assert(error);

	LOCK(file_ds, ret);
	if (ret) {
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Locking datastore file timeouted.");
		return NULL;
	}

	if (file_reload (file_ds)) {
		UNLOCK(file_ds);
		return NULL;
	}

	/* check validity of function parameters */
	switch(source) {
	case NC_DATASTORE_RUNNING:
		target_ds = file_ds->running;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		break;
	default:
		UNLOCK(file_ds);
		ERROR("%s: invalid target.", __func__);
		*error = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "source");
		return (NULL);
		break;
	}

	resultbuffer = xmlBufferCreate();
	if (resultbuffer == NULL) {
		UNLOCK(file_ds);
		ERROR("%s: xmlBufferCreate failed (%s:%d).", __func__, __FILE__, __LINE__);
		*error = nc_err_new(NC_ERR_OP_FAILED);
		return (NULL);
	}
	for (aux_node = target_ds->children; aux_node != NULL; aux_node = aux_node->next) {
		xmlNodeDump(resultbuffer, file_ds->xml, aux_node, 2, 1);
	}
	data = nc_clrwspace((char *) xmlBufferContent(resultbuffer));
	xmlBufferFree(resultbuffer);

	UNLOCK(file_ds);
	return (data);
}

/**
 * @brief Copy the content of the datastore or externally send
 * the configuration to another datastore
 *
 * @param ds Pointer to a datastore structure
 * @param session Session which the request is a part of
 * @param rpc RPC message with the request
 * @param target Target datastore.
 * @param source Source datastore, if the value is NC_DATASTORE_NONE
 * then the next parameter holds the configration to copy
 * @param config Configuration to be used as the source in the form of a serialized XML.
 * @param error	 Netconf error structure.
 *
 * @return EXIT_SUCCESS when done without problems
 * 	   EXIT_FAILURE when error occured
 * 	   EXIT_RPC_NOT_APPLICABLE when rpc is not applicable
 */
int ncds_file_copyconfig(struct ncds_ds *ds, const struct nc_session *session, const nc_rpc* rpc, NC_DATASTORE target, NC_DATASTORE source, char * config, struct nc_err **error)
{
	struct ncds_ds_file* file_ds = (struct ncds_ds_file*)ds;
	xmlDocPtr config_doc = NULL, aux_doc;
	xmlNodePtr target_ds, source_ds, aux_node, root;
	keyList keys;
	char *aux = NULL, *configp;
	int r, ret = 0;

	assert(error);

	LOCK(file_ds, ret);
	if (ret) {
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Locking datastore file timeouted.");
		return EXIT_FAILURE;
	}

	if (file_reload (file_ds)) {
		UNLOCK(file_ds);
		return EXIT_FAILURE;
	}
	file_rollback_store(file_ds);

	switch(target) {
	case NC_DATASTORE_RUNNING:
		target_ds = file_ds->running;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		break;
	default:
		UNLOCK(file_ds);
		ERROR("%s: invalid target.", __func__);
		*error = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "target");
		return EXIT_FAILURE;
		break;
	}

	/* isn't target locked? */
	if (file_ds_access (file_ds, target, session) != 0) {
		UNLOCK(file_ds);
		*error = nc_err_new (NC_ERR_IN_USE);
		return EXIT_FAILURE;
	}
	if (source == NC_DATASTORE_CANDIDATE && target == NC_DATASTORE_RUNNING) {
		/* commit - check also the lock on source (i.e. candidate) datastore */
		if (file_ds_access (file_ds, source, session) != 0) {
			UNLOCK(file_ds);
			*error = nc_err_new (NC_ERR_IN_USE);
			return EXIT_FAILURE;
		}
	}

	switch(source) {
	case NC_DATASTORE_RUNNING:
		source_ds = file_ds->running->children;
		break;
	case NC_DATASTORE_STARTUP:
		source_ds = file_ds->startup->children;
		break;
	case NC_DATASTORE_CANDIDATE:
		source_ds = file_ds->candidate->children;
		break;
	case NC_DATASTORE_CONFIG:
		if (config == NULL) {
			UNLOCK(file_ds);
			ERROR("%s: invalid source config.", __func__);
			*error = nc_err_new(NC_ERR_BAD_ELEM);
			nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "config");
			return EXIT_FAILURE;
		}
		if (strncmp(config, "<?xml", 5) == 0) {
			if ((configp = strchr(config, '>')) == NULL) {
				UNLOCK(file_ds);
				ERROR("%s: invalid source config.", __func__);
				*error = nc_err_new(NC_ERR_BAD_ELEM);
				nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "config");
				return EXIT_FAILURE;
			}
			++configp;
			while (*configp == ' ' || *configp == '\n' || *configp == '\t') {
				++configp;
			}
		} else {
			configp = config;
		}
		if (asprintf(&aux, "<config>%s</config>", configp) == -1) {
			UNLOCK(file_ds);
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			*error = nc_err_new(NC_ERR_OP_FAILED);
			return EXIT_FAILURE;
		}
		if ((config_doc = xmlReadMemory (aux, strlen(aux), NULL, NULL, NC_XMLREAD_OPTIONS)) == NULL) {
			free(aux);
			UNLOCK(file_ds);
			ERROR("%s: reading source config failed.", __func__);
			*error = nc_err_new(NC_ERR_OP_FAILED);
			return EXIT_FAILURE;
		}
		free(aux);
		source_ds = config_doc->children->children;
		break;
	default:
		UNLOCK(file_ds);
		ERROR("%s: invalid source.", __func__);
		*error = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "target");
		return EXIT_FAILURE;
		break;
	}

	/* we could still do something with candidate datastore,
	 * so we have to change the "modified" attribute
	 */
	if (source_ds == NULL && target_ds->children == NULL) {
		ret = EXIT_RPC_NOT_APPLICABLE;
		goto finish;
	}

	aux_doc = xmlNewDoc (BAD_CAST "1.0");
	if (source_ds) {
		xmlDocSetRootElement(aux_doc, xmlCopyNode(source_ds, 1));
		for (root = source_ds->next; root != NULL; root = aux_node) {
			aux_node = root->next;
			xmlAddNextSibling(aux_doc->last, xmlCopyNode(root, 1));
		}
	}

	if (rpc != NULL && rpc->nacm != NULL ) {
		/* NACM */
		/* RFC 6536, sec. 3.2.4., paragraph 2
		 * If the source of the <copy-config> protocol operation is the running
		 * configuration datastore and the target is the startup configuration
		 * datastore, the client is only required to have permission to execute
		 * the <copy-config> protocol operation.
		 */
		if (!(source == NC_DATASTORE_RUNNING && target == NC_DATASTORE_STARTUP)) {
			keys = get_keynode_list(file_ds->ds.ext_model);
			if (source == NC_DATASTORE_RUNNING || source == NC_DATASTORE_STARTUP || source == NC_DATASTORE_CANDIDATE) {
				/* RFC 6536, sec 3.2.4., paragraph 3
				 * If the source of the <copy-config> operation is a datastore,
				 * then data nodes to which the client does not have read access
				 * are silently omitted
				 */
				nacm_check_data_read(aux_doc, rpc->nacm);
			}

			/* RFC 6536, sec. 3.2.4., paragraph 4
			 * If the target of the <copy-config> operation is a datastore,
			 * the client needs access to the modified nodes according to
			 * the effective access operation of the each modified node.
			 */
			if (target_ds->children == NULL) {
				/* creating a completely new configuration data */
				r = nacm_check_data(aux_doc->children, NACM_ACCESS_CREATE, rpc->nacm);
			} else {
				/* replacing an old configuration data */
				r = edit_replace_nacmcheck(target_ds->children, aux_doc, file_ds->ds.ext_model, keys, rpc->nacm, error);
			}

			if (r != NACM_PERMIT) {
				if (r == NACM_DENY) {
					if (error != NULL ) {
						*error = nc_err_new(NC_ERR_ACCESS_DENIED);
					}
				} else {
					if (error != NULL ) {
						*error = nc_err_new(NC_ERR_OP_FAILED);
					}
				}
				UNLOCK(file_ds);
				xmlFreeDoc(aux_doc);
				keyListFree(keys);
				xmlFreeDoc (config_doc);
				return (EXIT_FAILURE);
			}
			keyListFree(keys);
		}
	}

	/* drop current target configuration */
	while ((aux_node = target_ds->children) != NULL) {
		xmlUnlinkNode (target_ds->children);
		xmlFreeNode (aux_node);
	}

	/* copy new target configuration */
	xmlAddChildList(target_ds, xmlCopyNodeList(aux_doc->children));
	xmlFreeDoc(aux_doc);

finish:
	/*
	 * if we are changing candidate, mark it as modified, since we need
	 * this information for locking - according to RFC, candidate cannot
	 * be locked since it has been modified and not committed.
	 */
	if (target == NC_DATASTORE_CANDIDATE) {
		if (source == NC_DATASTORE_RUNNING) {
			xmlSetProp (target_ds, BAD_CAST "modified", BAD_CAST "false");
		} else {
			xmlSetProp (target_ds, BAD_CAST "modified", BAD_CAST "true");
		}
	}

	if (file_sync (file_ds)) {
		UNLOCK(file_ds);
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Datastore file synchronisation failed.");
		return EXIT_FAILURE;
	}
	UNLOCK(file_ds);

	xmlFreeDoc (config_doc);
	return ret;
}

/**
 * @brief Delete target datastore
 *
 * @param ds Datastore to delete
 * @param session Session requesting the deletion
 * @param target Datastore type
 * @param error Netconf error structure
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int ncds_file_deleteconfig(struct ncds_ds * ds, const struct nc_session * session, NC_DATASTORE target, struct nc_err **error)
{
	struct ncds_ds_file * file_ds = (struct ncds_ds_file*)ds;
	xmlNodePtr target_ds, del;
	int ret;

	assert(error);

	LOCK(file_ds, ret);
	if (ret) {
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Locking datastore file timeouted.");
		return EXIT_FAILURE;
	}

	if (file_reload(file_ds)) {
		UNLOCK(file_ds);
		return EXIT_FAILURE;
	}
	file_rollback_store(file_ds);

	switch(target) {
	case NC_DATASTORE_RUNNING:
		UNLOCK(file_ds);
		*error = nc_err_new (NC_ERR_OP_FAILED);
		nc_err_set (*error, NC_ERR_PARAM_MSG, "Cannot delete a running datastore.");
		return EXIT_FAILURE;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		break;
	default:
		UNLOCK(file_ds);
		ERROR("%s: invalid target.", __func__);
		*error = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "target");
		return EXIT_FAILURE;
		break;
	}

	if (file_ds_access (file_ds, target, session) != 0) {
		UNLOCK(file_ds);
		*error = nc_err_new (NC_ERR_IN_USE);
		return EXIT_FAILURE;
	}

	while ((del = target_ds->children) != NULL) {
		xmlUnlinkNode (target_ds->children);
		xmlFreeNode (del);
	}

	/*
	 * if we are changing the candidate, mark it as modified, since we need
	 * this information for locking - according to RFC, candidate cannot
	 * be locked since it has been modified and not committed.
	 */
	if (target == NC_DATASTORE_CANDIDATE) {
		xmlSetProp (target_ds, BAD_CAST "modified", BAD_CAST "true");
	}

	if (file_sync (file_ds)) {
		UNLOCK(file_ds);
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Datastore file synchronisation failed.");
		return EXIT_FAILURE;
	}
	UNLOCK(file_ds);

	return EXIT_SUCCESS;
}

/**
 * @brief Perform the edit-config operation
 *
 * @param ds Datastore to edit
 * @param session Session sending the edit request
 * @param rpc
 * @param target Datastore type
 * @param config Edit configuration.
 * @param defop Default edit operation.
 * @param error Netconf error structure
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int ncds_file_editconfig(struct ncds_ds *ds, const struct nc_session * session, const nc_rpc* rpc, NC_DATASTORE target, const char * config, NC_EDIT_DEFOP_TYPE defop, NC_EDIT_ERROPT_TYPE errop, struct nc_err **error)
{
	struct ncds_ds_file * file_ds = (struct ncds_ds_file *)ds;
	xmlDocPtr config_doc, datastore_doc;
	xmlNodePtr target_ds, aux_node, root;
	int retval = EXIT_SUCCESS, ret;
	char* aux = NULL;
	const char* configp;

	assert(error);

	/* lock the datastore */
	LOCK(file_ds, ret);
	if (ret) {
		*error = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*error, NC_ERR_PARAM_MSG, "Locking datastore file timeouted.");
		return EXIT_FAILURE;
	}

	/* reload the datastore content */
	if (file_reload (file_ds)) {
		UNLOCK(file_ds);
		return EXIT_FAILURE;
	}
	file_rollback_store(file_ds);

	switch(target) {
	case NC_DATASTORE_RUNNING:
		target_ds = file_ds->running;
		break;
	case NC_DATASTORE_STARTUP:
		target_ds = file_ds->startup;
		break;
	case NC_DATASTORE_CANDIDATE:
		target_ds = file_ds->candidate;
		break;
	default:
		UNLOCK(file_ds);
		ERROR("%s: invalid target.", __func__);
		*error = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "target");
		return EXIT_FAILURE;
		break;
	}

	if (file_ds_access (file_ds, target, session) != 0) {
		UNLOCK(file_ds);
		*error = nc_err_new (NC_ERR_IN_USE);
		return EXIT_FAILURE;
	}

	if (strncmp(config, "<?xml", 5) == 0) {
		if ((configp = strchr(config, '>')) == NULL) {
			UNLOCK(file_ds);
			ERROR("%s: invalid config.", __func__);
			*error = nc_err_new(NC_ERR_BAD_ELEM);
			nc_err_set(*error, NC_ERR_PARAM_INFO_BADELEM, "config");
			return EXIT_FAILURE;
		}
		++configp;
		while (*configp == ' ' || *configp == '\n' || *configp == '\t') {
			++configp;
		}
	} else {
		configp = config;
	}
	if (asprintf(&aux, "<config>%s</config>", configp) == -1) {
		UNLOCK(file_ds);
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		*error = nc_err_new(NC_ERR_OP_FAILED);
		return EXIT_FAILURE;
	}

	/* read config to XML doc */
	if ((config_doc = xmlReadMemory (aux, strlen(aux), NULL, NULL, NC_XMLREAD_OPTIONS)) == NULL) {
		UNLOCK(file_ds);
		free(aux);
		ERROR("%s: Reading xml data failed!", __func__);
		return EXIT_FAILURE;
	}
	free(aux);
	/* magic - get off the root config element and move all children to the 1st level */
	root = xmlDocGetRootElement(config_doc);
	for (aux_node = root->children; aux_node != NULL; aux_node = root->children) {
		xmlUnlinkNode(aux_node);
		xmlAddNextSibling(config_doc->last, aux_node);
	}
	aux_node = root->next;
	xmlUnlinkNode(root);
	xmlFreeNode(root);

	/* create an XML doc with a copy of the datastore configuration */
	datastore_doc = xmlNewDoc (BAD_CAST "1.0");
	xmlDocSetRootElement(datastore_doc, xmlCopyNode(target_ds->children, 1));
	if (target_ds->children) {
		for (root = target_ds->children->next; root != NULL; root = aux_node) {
			aux_node = root->next;
			xmlAddNextSibling(datastore_doc->last, xmlCopyNode(root, 1));
		}
	}

	/* preform edit config */
	if (edit_config(datastore_doc, config_doc, (struct ncds_ds*)file_ds, defop, errop, (rpc != NULL) ? rpc->nacm : NULL, error)) {
		retval = EXIT_FAILURE;
	} else {
		/* replace datastore by edited configuration */
		while ((aux_node = target_ds->children) != NULL) {
			xmlUnlinkNode(aux_node);
			xmlFreeNode(aux_node);
		}
		xmlAddChildList(target_ds, xmlCopyNodeList(datastore_doc->children));

		/*
		 * if we are changing candidate, mark it as modified, since we need
		 * this information for locking - according to RFC, candidate cannot
		 * be locked since it has been modified and not committed.
		 */
		if (target == NC_DATASTORE_CANDIDATE) {
			xmlSetProp(target_ds, BAD_CAST "modified", BAD_CAST "true");
		}

		/* sync xml tree with file on the hdd */
		if (file_sync(file_ds)) {
			*error = nc_err_new(NC_ERR_OP_FAILED);
			nc_err_set(*error, NC_ERR_PARAM_MSG, "Datastore file synchronisation failed.");
			retval = EXIT_FAILURE;
		}
	}
	UNLOCK(file_ds);

	xmlFreeDoc(datastore_doc);
	xmlFreeDoc(config_doc);

	return retval;
}
