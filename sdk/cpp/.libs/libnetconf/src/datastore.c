/**
 * \file datastore.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of the NETCONF datastore handling functions.
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
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <dlfcn.h>
#include <dirent.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#ifndef DISABLE_VALIDATION
#  include <libxml/relaxng.h>
#  include <libxslt/transform.h>
#  include <libxslt/xsltInternals.h>
#else
#  ifndef DISABLE_YANGSCHEMA
#    include <libxslt/transform.h>
#    include <libxslt/xsltInternals.h>
#  endif
#endif

#include "netconf_internal.h"
#include "messages.h"
#include "messages_xml.h"
#include "messages_internal.h"
#include "error.h"
#include "with_defaults.h"
#include "session.h"
#include "datastore_xml.h"
#include "nacm.h"
#include "datastore/edit_config.h"
#include "datastore/datastore_internal.h"
#include "datastore/file/datastore_file.h"
#include "datastore/empty/datastore_empty.h"
#include "datastore/custom/datastore_custom_private.h"
#include "transapi/transapi_internal.h"
#include "config.h"

#ifndef DISABLE_URL
#	include "url_internal.h"
#endif

#ifndef DISABLE_NOTIFICATIONS
#  include "notifications.h"
#endif

#include "../models/ietf-netconf-monitoring.xxd"
#include "../models/ietf-netconf-notifications.xxd"
#include "../models/ietf-netconf-with-defaults.xxd"
#include "../models/nc-notifications.xxd"
#include "../models/ietf-netconf-acm.xxd"
#include "../models/ietf-netconf.xxd"
#include "../models/notifications.xxd"
#include "../models/ietf-inet-types.xxd"
#include "../models/ietf-yang-types.xxd"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

/*
 * From internal.c to be used by nc_session_get_cpblts_deault() to detect
 * what part of libnetconf is initiated and can be executed
 */
extern int nc_init_flags;

extern struct nc_shared_info *nc_info;

/* reserve memory for error pointers such as ERROR_POINTER or NCDS_RPC_NOT_APPLICABLE */
API char error_area;
#define ERROR_POINTER ((void*)(&error_area))

char* server_capabilities;
//pthread_spinlock_t server_cpblt_lock;

struct ncds_ds_list {
	struct ncds_ds *datastore;
	struct ncds_ds_list* next;
};

struct ds_desc {
	NCDS_TYPE type;
	char* filename;
};

struct ncds {
	struct ncds_ds_list *datastores;
	ncds_id* datastores_ids;
	int count;
	int array_size;
};

struct ncds_ds *nacm_ds = NULL; /* for NACM subsystem */
static struct ncds ncds = {NULL, NULL, 0, 0};
static struct model_list *models_list = NULL;
static struct transapi_list* augment_tapi_list = NULL;
static char** models_dirs = NULL;

static nc_reply* ncds_apply_rpc(ncds_id id, const struct nc_session* session, const nc_rpc* rpc, struct nc_filter* shared_filter);
static char* get_state_nacm(const char* UNUSED(model), const char* UNUSED(running), struct nc_err ** UNUSED(e));
static char* get_state_monitoring(const char* UNUSED(model), const char* UNUSED(running), struct nc_err ** UNUSED(e));
static int get_model_info(xmlXPathContextPtr model_ctxt, char **name, char **version, char **ns, char **prefix, char ***rpcs, char ***notifs);
static struct data_model* get_model(const char* module, const char* version);
static int ncds_features_parse(struct data_model* model);
static int ncds_update_uses_groupings(struct data_model* model);
static int ncds_update_uses_augments(struct data_model* model);
static void ncds_ds_model_free(struct data_model* model);
static xmlDocPtr ncxml_merge(const xmlDocPtr first, const xmlDocPtr second, const xmlDocPtr data_model);
extern int first_after_close;

static int ncds_update_features();
static int feature_check(xmlNodePtr node, struct data_model* model);
static struct model_feature** get_features_from_prefix(struct data_model* model, char* prefix);

#ifndef DISABLE_NOTIFICATIONS
static char* get_state_notifications(const char* UNUSED(model), const char* UNUSED(running), struct nc_err ** UNUSED(e));
#endif

static struct ncds_ds *datastores_get_ds(ncds_id id);

#ifndef DISABLE_YANGFORMAT
/* XSL stylesheet for transformation from YIN to YANG format */
#define YIN2YANG NC_WORKINGDIR_PATH"/yin2yang.xsl"
static xsltStylesheetPtr yin2yang_xsl = NULL;
#endif

/* Allocate and fill the ncds func structure based on the type. */
static struct ncds_ds* ncds_fill_func(NCDS_TYPE type)
{
	struct ncds_ds* ds;
	switch (type) {
	case NCDS_TYPE_CUSTOM:
		if ((ds = (struct ncds_ds*) calloc(1, sizeof(struct ncds_ds_custom))) == NULL) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		ds->func.init = ncds_custom_init;
		ds->func.free = ncds_custom_free;
		ds->func.was_changed = ncds_custom_was_changed;
		ds->func.rollback = ncds_custom_rollback;
		ds->func.get_lockinfo = ncds_custom_get_lockinfo;
		ds->func.lock = ncds_custom_lock;
		ds->func.unlock = ncds_custom_unlock;
		ds->func.getconfig = ncds_custom_getconfig;
		ds->func.copyconfig = ncds_custom_copyconfig;
		ds->func.deleteconfig = ncds_custom_deleteconfig;
		ds->func.editconfig = ncds_custom_editconfig;
		break;
	case NCDS_TYPE_FILE:
		if ((ds = (struct ncds_ds*) calloc(1, sizeof(struct ncds_ds_file))) == NULL ) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			return (NULL );
		}
		ds->func.init = ncds_file_init;
		ds->func.free = ncds_file_free;
		ds->func.was_changed = ncds_file_changed;
		ds->func.rollback = ncds_file_rollback;
		ds->func.get_lockinfo = ncds_file_lockinfo;
		ds->func.lock = ncds_file_lock;
		ds->func.unlock = ncds_file_unlock;
		ds->func.getconfig = ncds_file_getconfig;
		ds->func.copyconfig = ncds_file_copyconfig;
		ds->func.deleteconfig = ncds_file_deleteconfig;
		ds->func.editconfig = ncds_file_editconfig;
		break;
	case NCDS_TYPE_EMPTY:
		if ((ds = (struct ncds_ds*) calloc(1, sizeof(struct ncds_ds_empty))) == NULL ) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			return (NULL );
		}
		ds->func.init = ncds_empty_init;
		ds->func.free = ncds_empty_free;
		ds->func.was_changed = ncds_empty_changed;
		ds->func.rollback = ncds_empty_rollback;
		ds->func.get_lockinfo = ncds_empty_lockinfo;
		ds->func.lock = ncds_empty_lock;
		ds->func.unlock = ncds_empty_unlock;
		ds->func.getconfig = ncds_empty_getconfig;
		ds->func.copyconfig = ncds_empty_copyconfig;
		ds->func.deleteconfig = ncds_empty_deleteconfig;
		ds->func.editconfig = ncds_empty_editconfig;
		break;
	default:
		ERROR("Unsupported datastore implementation required.");
		return (NULL );
	}
	return (ds);
}

#ifndef DISABLE_NOTIFICATIONS
#define INTERNAL_DS_COUNT 9
#define MONITOR_DS_INDEX 3
#define NOTIF_DS_INDEX_L 4
#define NOTIF_DS_INDEX_H 6
#define WD_DS_INDEX 7
#define NACM_DS_INDEX 8
#else
#define INTERNAL_DS_COUNT 6
#define MONITOR_DS_INDEX 3
#define WD_DS_INDEX 4
#define NACM_DS_INDEX 5
#endif
int internal_ds_count = 0;
int ncds_sysinit(int flags)
{
	int i;
	struct ncds_ds *ds;
	struct ncds_ds_list *dsitem;
	struct model_list *list_item;

	unsigned char* model[INTERNAL_DS_COUNT] = {
			ietf_inet_types_yin,
			ietf_yang_types_yin,
			ietf_netconf_yin,
			ietf_netconf_monitoring_yin,
#ifndef DISABLE_NOTIFICATIONS
			ietf_netconf_notifications_yin,
			nc_notifications_yin,
			notifications_yin,
#endif
			ietf_netconf_with_defaults_yin,
			ietf_netconf_acm_yin
	};
	unsigned int model_len[INTERNAL_DS_COUNT] = {
			ietf_inet_types_yin_len,
			ietf_yang_types_yin_len,
			ietf_netconf_yin_len,
			ietf_netconf_monitoring_yin_len,
#ifndef DISABLE_NOTIFICATIONS
			ietf_netconf_notifications_yin_len,
			nc_notifications_yin_len,
			notifications_yin_len,
#endif
			ietf_netconf_with_defaults_yin_len,
			ietf_netconf_acm_yin_len
	};
	char* (*get_state_funcs[INTERNAL_DS_COUNT])(const char* model, const char* running, struct nc_err ** e) = {
			NULL, /* ietf-inet-types */
			NULL, /* ietf-yang-types */
			NULL, /* ietf-netconf */
			get_state_monitoring, /* ietf-netconf-monitoring */
#ifndef DISABLE_NOTIFICATIONS
			NULL, /* ietf-netconf-notifications */
			get_state_notifications, /* nc-notifications */
			NULL, /* notifications */
#endif
			NULL, /* ietf-netconf-with-defaults */
			get_state_nacm /* NACM status data */
	};
	struct ds_desc internal_ds_desc[INTERNAL_DS_COUNT] = {
			{NCDS_TYPE_EMPTY, NULL},
			{NCDS_TYPE_EMPTY, NULL},
			{NCDS_TYPE_EMPTY, NULL},
			{NCDS_TYPE_EMPTY, NULL},
#ifndef DISABLE_NOTIFICATIONS
			{NCDS_TYPE_EMPTY, NULL}, /* ietf-netconf-notifications */
			{NCDS_TYPE_EMPTY, NULL}, /* nc-notifications */
			{NCDS_TYPE_EMPTY, NULL}, /* notifications */
#endif
			{NCDS_TYPE_EMPTY, NULL},
			{NCDS_TYPE_FILE, NC_WORKINGDIR_PATH"/datastore-acm.xml"}
	};
#ifndef DISABLE_VALIDATION
	char* relaxng_validators[INTERNAL_DS_COUNT] = {
			NULL, /* ietf-inet-types */
			NULL, /* ietf-yang-types */
			NULL, /* ietf-netconf */
			NULL, /* ietf-netconf-monitoring */
#ifndef DISABLE_NOTIFICATIONS
			NULL, /* ietf-netconf-notifications */
			NULL, /* nc-notifications */
			NULL, /* notifications */
#endif
			NULL, /* ietf-netconf-with-defaults */
			NC_WORKINGDIR_PATH"/ietf-netconf-acm-config.rng" /* NACM RelaxNG schema */
	};
	char* schematron_validators[INTERNAL_DS_COUNT] = {
			NULL, /* ietf-inet-types */
			NULL, /* ietf-yang-types */
			NULL, /* ietf-netconf */
			NULL, /* ietf-netconf-monitoring */
#ifndef DISABLE_NOTIFICATIONS
			NULL, /* ietf-netconf-notifications */
			NULL, /* nc-notifications */
			NULL, /* notifications */
#endif
			NULL, /* ietf-netconf-with-defaults */
			NC_WORKINGDIR_PATH"/ietf-netconf-acm-schematron.xsl" /* NACM Schematron XSL stylesheet */
	};
#endif

        //pthread_spin_init(&server_cpblt_lock, PTHREAD_PROCESS_SHARED);

	internal_ds_count = 0;
	for (i = 0; i < INTERNAL_DS_COUNT; i++) {
		if ((i == NACM_DS_INDEX) && !(flags & NC_INIT_NACM)) {
			/* NACM is not enabled */
			continue;
		}

		if ((i == MONITOR_DS_INDEX) && !(flags & NC_INIT_MONITORING)) {
			/* NETCONF monitoring is not enabled */
			continue;
		}

		if ((i == WD_DS_INDEX) && !(flags & NC_INIT_WD)) {
			/* NETCONF with-defaults capability is not enabled */
			continue;
		}

#ifndef DISABLE_NOTIFICATIONS
		if ((i >= NOTIF_DS_INDEX_L && i <= NOTIF_DS_INDEX_H) && !(flags & NC_INIT_NOTIF)) {
			/* Notifications are not enabled */
			continue;
		}
#endif

		ds = ncds_fill_func(internal_ds_desc[i].type);
		if (ds == NULL) {
			/* The error was reported already. */
			return (EXIT_FAILURE);
		}
		ds->id = internal_ds_count++;
		ds->type = internal_ds_desc[i].type;
		if (ds->type == NCDS_TYPE_FILE && ncds_file_set_path(ds, internal_ds_desc[i].filename) != 0) {
			ERROR("Linking internal datastore to a file (%s) failed.", internal_ds_desc[i].filename);
			ncds_free(ds);
			internal_ds_count--;
			return (EXIT_FAILURE);
		}

		ds->data_model = calloc(1, sizeof(struct data_model));
		if (ds->data_model == NULL) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			ncds_free(ds);
			internal_ds_count--;
			return (EXIT_FAILURE);
		}

		ds->data_model->xml = xmlReadMemory ((char*)model[i], model_len[i], NULL, NULL, NC_XMLREAD_OPTIONS);
		if (ds->data_model->xml == NULL ) {
			ERROR("Unable to read the internal monitoring data model.");
			ncds_free(ds);
			internal_ds_count--;
			return (EXIT_FAILURE);
		}

		/* prepare xpath evaluation context of the model for XPath */
		if ((ds->data_model->ctxt = xmlXPathNewContext(ds->data_model->xml)) == NULL) {
			ERROR("%s: Creating XPath context failed.", __func__);
			ncds_free(ds);
			internal_ds_count--;
			/* with-defaults cannot be found */
			return (EXIT_FAILURE);
		}
		if (xmlXPathRegisterNs(ds->data_model->ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
			xmlXPathFreeContext(ds->data_model->ctxt);
			ncds_free(ds);
			internal_ds_count--;
			return (EXIT_FAILURE);
		}

		if (get_model_info(ds->data_model->ctxt,
				&(ds->data_model->name),
				&(ds->data_model->version),
				&(ds->data_model->ns),
				&(ds->data_model->prefix),
				&(ds->data_model->rpcs),
				&(ds->data_model->notifs)) != 0) {
			ERROR("Unable to process internal configuration data model.");
			ncds_free(ds);
			internal_ds_count--;
			return (EXIT_FAILURE);
		}

		asprintf(&ds->data_model->path, "internal_%d", i);
		ncds_features_parse(ds->data_model);
		ds->ext_model = ds->data_model->xml;
		ds->ext_model_tree = NULL;

		/* resolve uses statements in groupings and augments definitions */
		ncds_update_uses_groupings(ds->data_model);
		ncds_update_uses_augments(ds->data_model);

		ds->last_access = 0;
		ds->get_state = get_state_funcs[i];

		/* update internal model lists */
		list_item = malloc(sizeof(struct model_list));
		if (list_item == NULL) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			ncds_free(ds);
			internal_ds_count--;
			return (EXIT_FAILURE);
		}
		list_item->model = ds->data_model;
		list_item->next = models_list;
		models_list = list_item;

#ifndef DISABLE_VALIDATION
		/* set validation */
		if (relaxng_validators[i] != NULL || schematron_validators[i] != NULL) {
			ncds_set_validation(ds, 1, relaxng_validators[i], schematron_validators[i]);
			VERB("Datastore %s initiated with ID %d.", ds->data_model->name, ds->id);
		}
#endif

		/* init */
		ds->func.init(ds);

		/* add to a datastore list */
		if ((dsitem = malloc (sizeof(struct ncds_ds_list))) == NULL ) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			ncds_free(ds);
			internal_ds_count--;
			return (EXIT_FAILURE);
		}
		if (i == NACM_DS_INDEX) {
			/* provide NACM datastore to the NACM subsystem for faster access */
			nacm_ds = ds;
		}
		dsitem->datastore = ds;
		dsitem->next = ncds.datastores;
		ncds.datastores = dsitem;
		ncds.count++;
		if (ncds.count >= ncds.array_size) {
			void *tmp = realloc(ncds.datastores_ids, (ncds.array_size + 10) * sizeof(ncds_id));
			if (tmp == NULL) {
				ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
				ncds_free(ds);
				internal_ds_count--;
				ncds.datastores = NULL;
				ncds.count--;
				return (EXIT_FAILURE);
			}

			ncds.array_size += 10;
			ncds.datastores_ids = tmp;
		}

		ds = NULL;
	}

#ifndef DISABLE_YANGFORMAT
	/* try to get yin2yang XSLT stylesheet */
	errno = 0;
	if (eaccess(YIN2YANG, R_OK) == -1 || (yin2yang_xsl = xsltParseStylesheetFile(BAD_CAST YIN2YANG)) == NULL) {
		WARN("Unable to use %s (%s).", YIN2YANG, errno == 0 ? "XSLT parser failed" : strerror(errno));
		WARN("YANG format data models will not be available via get-schema.");
	}
#endif

	return (EXIT_SUCCESS);
}

void ncds_startup_internal(void)
{
	struct ncds_ds_list *ds_iter;
	struct nc_err *e = NULL;

	for (ds_iter = ncds.datastores; ds_iter != NULL ; ds_iter = ds_iter->next) {
		/* apply startup to running */
		ds_iter->datastore->func.copyconfig(ds_iter->datastore, NULL, NULL, NC_DATASTORE_RUNNING, NC_DATASTORE_STARTUP, NULL, &e);
		nc_err_free(e);
		e = NULL;
	}
}

/**
 * @brief Get ncds_ds structure from the datastore list containing storage
 * information with the specified ID.
 *
 * @param[in] id ID of the storage.
 * @return Pointer to the required ncds_ds structure inside internal
 * datastores variable.
 */
static struct ncds_ds *datastores_get_ds(ncds_id id)
{
	struct ncds_ds_list *ds_iter;

	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next) {
		if (ds_iter->datastore != NULL && ds_iter->datastore->id == id) {
			break;
		}
	}

	if (ds_iter == NULL) {
		return NULL;
	}

	return (ds_iter->datastore);
}

/**
 * @brief Remove datastore with the specified ID from the internal datastore list.
 *
 * @param[in] id ID of the storage.
 * @return Pointer to the required ncds_ds structure detached from the internal
 * datastores variable.
 */
static struct ncds_ds *datastores_detach_ds(ncds_id id)
{
	struct ncds_ds_list *ds_iter;
	struct ncds_ds_list *ds_prev = NULL;
	struct ncds_ds * retval = NULL;

	if (id < internal_ds_count && !(nc_init_flags & NC_INIT_CLOSING)) {
		/* ignore a try to detach some uninitialized or internal datastore */
		return (NULL);
	}

	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_prev = ds_iter, ds_iter = ds_iter->next) {
		if (ds_iter->datastore != NULL && ds_iter->datastore->id == id) {
			break;
		}
	}

	if (ds_iter != NULL) {
		/* required datastore was found */
		if (ds_prev == NULL) {
			/* we're removing the first item of the datastores list */
			ncds.datastores = ds_iter->next;
		} else {
			ds_prev->next = ds_iter->next;
		}
		retval = ds_iter->datastore;
		free(ds_iter);
		ncds.count--;
	}

	return retval;
}

/*
 * type 0 - backup
 * type 1 - restore
 */
static int fmon_cp_file(const char* source, const char* target, uint8_t type)
{
	char buf[4096];
	int source_fd, target_fd;
	mode_t mode;
	uid_t uid;
	gid_t gid;
	struct stat finfo;
	ssize_t r;

	assert(source);
	assert(target);

	if ((source_fd = open(source, O_RDONLY|O_CLOEXEC)) == -1) {
		if (type == 1) {
			ERROR("Unable to open backup file \"%s\" (%s)", source, strerror(errno));
		} else {
			ERROR("Unable to open file \"%s\" to backup (%s)", source, strerror(errno));
		}
		return 1;
	}


	/* get source access rights */
	if (fstat(source_fd, &finfo) == -1) {
		if (type == 1) {
			WARN("Unable to get information about backup file \"%s\" (%s).", source, strerror(errno));
			VERB("Using default protection 0600 for restored file.");
		} else {
			WARN("Unable to get information about \"%s\" file to backup (%s).", source, strerror(errno));
			VERB("Using default protection 0600 for backup file.");
		}
		mode = 00600;
		uid = geteuid();
		gid = getegid();
	} else {
		mode = finfo.st_mode;
		uid = finfo.st_uid;
		gid = finfo.st_gid;
	}

	if ((target_fd = open(target, O_WRONLY|O_CLOEXEC|O_CREAT|O_TRUNC, mode)) == -1) {
		if (type == 1) {
			ERROR("Unable to restore file \"%s\" (%s)", target, strerror(errno));
		} else {
			ERROR("Unable to create backup file \"%s\" (%s)", target, strerror(errno));
		}
		close(source_fd);
		return 1;
	}
	if (fchown(target_fd, uid, gid) != 0) {
		WARN("Failed to change owner of \"%s\" (%s).", target, strerror(errno));
	}
	fchmod(target_fd, mode); /* if not created, but rewriting some existing file */

	for (;;) {
		r = read(source_fd, buf, sizeof(buf));
		if (r == 0) {
			/* EOF */
			break;
		} else if (r < 0) {
			/* ERROR */
			if (type == 1) {
				ERROR("Restoring file \"%s\" failed (%s).", target, strerror(errno));
			} else {
				ERROR("Creating backup file \"%s\" failed (%s).", target, strerror(errno));
			}
			break;
		}
		if (write(target_fd, buf, r) < r) {
			ERROR("Writing into file \"%s\" failed (%s).", target, strerror(errno));
			break;
		}
	}
	close(source_fd);
	close(target_fd);

	return 0;
}

static int fmon_restore_file(const char* target)
{
	char *source = NULL;
	int ret;

	assert(target);

	if (asprintf(&source, "%s.netconf", target) != 0) {
		return 1;
	}
	ret = fmon_cp_file(source, target, 1);
	free(source);

	return ret;
}

static int fmon_backup_file(const char* source)
{
	char *target = NULL;
	int ret;

	assert(source);

	if (asprintf(&target, "%s.netconf", source) == -1) {
		return 1;
	}
	ret = fmon_cp_file(source, target, 0);
	free(target);

	return ret;
}

struct fmon {
	int wd;
	char flags;
};
#define FMON_FLAG_MODIFIED 0x01
#define FMON_FLAG_IGNORED  0x02
#define FMON_FLAG_UPDATE   0x04
struct fmon_arg {
	volatile int flag;
	struct transapi_file_callbacks *fclbks;
	struct ncds_ds *ds;
};
static void* transapi_fmon(void *arg)
{
	return NULL;
}

API int ncds_device_init(ncds_id *id, struct nc_cpblts *cpblts, int force)
{
	nc_rpc * rpc_msg = NULL;
	nc_reply * reply_msg = NULL;
	struct ncds_ds_list * ds_iter, *start = NULL;
	struct ncds_ds * ds;
	struct nc_session * dummy_session = NULL;
	struct nc_err * err = NULL;
	int nocpblts = 0, ret, retval = EXIT_SUCCESS;
	xmlDocPtr running_doc = NULL, aux_doc1 = NULL, aux_doc2;
	xmlNodePtr data_node;
	char* new_running_config = NULL;
	xmlBufferPtr running_buf = NULL;
	struct transapi_list *tapi_iter;
	static struct fmon_arg arg = {0, NULL, NULL};

	if (id != NULL) {
		/* initialize only the device connected with the given datastore ID */
		if ((ds = datastores_get_ds(*id)) == NULL) {
			ERROR("Unable to find module with id %d", *id);
			return (EXIT_FAILURE);
		}
		start = calloc(1, sizeof(struct ncds_ds_list));
		if (start == NULL) {
			ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
			return (EXIT_FAILURE);
		}
		start->datastore = ds;
	} else {
		/* OR if datastore not specified, initialize all transAPI capable modules */
		start = ncds.datastores;
	}

	if (cpblts == NULL) {
		cpblts = nc_session_get_cpblts_default();
		nocpblts = 1;
	}

	/* create dummy session for applying copy-config (startup->running) */
	if ((dummy_session = nc_session_dummy("dummy-internal", "server", NULL, cpblts)) == NULL) {
		ERROR("%s: Creating dummy-internal session failed.", __func__);
		retval = EXIT_FAILURE;
		goto cleanup;
	}

	if (nocpblts) {
		nc_cpblts_free(cpblts);
		cpblts = NULL;
	}

	rpc_msg = nc_rpc_copyconfig(NC_DATASTORE_STARTUP, NC_DATASTORE_RUNNING);
	running_buf = xmlBufferCreate();

	for (ds_iter = start; ds_iter != NULL ; ds_iter = ds_iter->next) {
		for (tapi_iter = ds_iter->datastore->transapis; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
			if (tapi_iter->tapi->init != NULL) {
				/* module can return current configuration of device */
				if (tapi_iter->tapi->init(&aux_doc1)) {
					ERROR("init function from module %s failed.",
					        ds_iter->datastore->data_model->name);
					retval = EXIT_FAILURE;
					goto cleanup;
				}
				if (running_doc == NULL) {
					running_doc = aux_doc1;
				} else {
					aux_doc2 = running_doc;
					running_doc = ncxml_merge(aux_doc2, aux_doc1, ds_iter->datastore->ext_model);
					xmlFreeDoc(aux_doc1);
					xmlFreeDoc(aux_doc2);
				}
			}
		}

		if (first_after_close || force) {
			/* if this process is first after a nc_close(system=1) or reinitialization is forced */

			/* dump running configuration data returned by transapi_init() */
			if (running_doc == NULL) {
				new_running_config = strdup("");
			} else {
				for (data_node = running_doc->children; data_node != NULL; data_node = data_node->next) {
					xmlNodeDump(running_buf, running_doc, data_node, 0, 0);
				}
				new_running_config = strdup((char*)xmlBufferContent(running_buf));
				xmlBufferEmpty(running_buf);
			}

			/* Clean RUNNING datastore. This is important when transAPI is deployed and does not harm when not. */
			/* It is done by calling low level function to avoid invoking transAPI now. */

			/*
			 * If :startup is not supported, running stays persistent between
			 * reboots
			 */
			if (!nc_cpblts_enabled(dummy_session, NC_CAP_STARTUP_ID)) {
				goto cleanup;
			}

			/* replace running datastore with current configuration provided by module, or erase it if none provided
			 * this is done be low level function to bypass transapi */
			ret = ds_iter->datastore->func.copyconfig(ds_iter->datastore, NULL, NULL, NC_DATASTORE_RUNNING, NC_DATASTORE_CONFIG, new_running_config, &err);
			if (ret != 0 && ret != EXIT_RPC_NOT_APPLICABLE) {
				ERROR("Failed to replace running with current configuration (%s).", err ? err->message : "unknown error");
				nc_err_free(err);
				retval = EXIT_FAILURE;
				goto cleanup;
			}

			/* initial copy of startup to running will cause full (re)configuration of module */
			/* Here is used high level function ncds_apply_rpc to apply startup configuration and use transAPI */
			reply_msg = ncds_apply_rpc(ds_iter->datastore->id, dummy_session, rpc_msg, NULL);
			if (reply_msg == NULL || (reply_msg != NCDS_RPC_NOT_APPLICABLE && nc_reply_get_type (reply_msg) != NC_REPLY_OK)) {
				ERROR("Failed perform initial copy of startup to running.");
				nc_reply_free(reply_msg);
				retval = EXIT_FAILURE;
				goto cleanup;
			}
			nc_reply_free(reply_msg);

			/* prepare variable for the next loop */
			free(new_running_config);
			new_running_config = NULL;
		}

		/* start monitoring external files */
		for (tapi_iter = ds_iter->datastore->transapis; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
			if (tapi_iter->tapi->file_clbks != NULL && tapi_iter->tapi->file_clbks->callbacks_count != 0) {
				while(arg.flag) {
					/* let the previous thread store pointers from passed argument */
					usleep(50);
				}
				VERB("Starting FMON thread for %s data model.", ds_iter->datastore->data_model->name);
				arg.flag = 1;
				arg.fclbks = tapi_iter->tapi->file_clbks;
				arg.ds = ds_iter->datastore;
				ret = pthread_create(&(tapi_iter->tapi->fmon_thread), NULL, &transapi_fmon, &arg);
				if (ret != 0) {
					ERROR("Unable to create FMON thread for %s data model (%s)", ds_iter->datastore->data_model->name, strerror(ret));
				}
				pthread_detach(tapi_iter->tapi->fmon_thread);
			}
		}

		/* prepare variable for the next loop */
		xmlFreeDoc(running_doc);
		running_doc = NULL;
	}

cleanup:
	xmlBufferFree(running_buf);
	xmlFreeDoc(running_doc);
	free(new_running_config);

	nc_rpc_free(rpc_msg);
	nc_session_close(dummy_session, NC_SESSION_TERM_OTHER);
	nc_session_free(dummy_session);

	if (id != NULL) {
		free(start);
	}

	return (retval);
}


API char* ncds_get_model(ncds_id id, int base)
{
	struct ncds_ds * datastore = datastores_get_ds(id);
	xmlBufferPtr buf;
	xmlDocPtr model;
	char * retval = NULL;

	if (datastore == NULL) {
		return (NULL);
	} else if (base) {
		model = datastore->data_model->xml;
	} else {
		model = datastore->ext_model;
	}

	if (model != NULL) {
		buf = xmlBufferCreate();
		xmlNodeDump(buf, model, model->children, 1, 1);
		retval = strdup((char*) xmlBufferContent(buf));
		xmlBufferFree(buf);
	}
	return retval;
}

API const char* ncds_get_model_path(ncds_id id)
{
	struct ncds_ds * datastore = datastores_get_ds(id);

	if (datastore == NULL) {
		return NULL;
	}

	return (datastore->data_model->path);
}

API int ncds_model_info(const char *path, char **name, char **version, char **ns, char **prefix, char ***rpcs, char ***notifs)
{
	int retval;
	xmlXPathContextPtr model_ctxt;
	xmlDocPtr model_xml;

	model_xml = xmlReadFile(path, NULL, NC_XMLREAD_OPTIONS);
	if (model_xml == NULL) {
		ERROR("Unable to read the configuration data model %s.", path);
		return (EXIT_FAILURE);
	}

	/* prepare xpath evaluation context of the model for XPath */
	if ((model_ctxt = xmlXPathNewContext(model_xml)) == NULL) {
		ERROR("%s: Creating XPath context failed.", __func__);
		xmlFreeDoc(model_xml);
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(model_ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
		xmlXPathFreeContext(model_ctxt);
		xmlFreeDoc(model_xml);
		return (EXIT_FAILURE);
	}

	retval = get_model_info(model_ctxt, name, version, ns, prefix, rpcs, notifs);

	xmlFreeDoc(model_xml);
	xmlXPathFreeContext(model_ctxt);

	return (retval);
}

static int get_model_info(xmlXPathContextPtr model_ctxt, char **name, char **version, char **ns, char **prefix, char ***rpcs, char ***notifs)
{
	xmlXPathObjectPtr result = NULL;
	xmlChar *xml_aux;
	int i, j, l;

	if (notifs) {*notifs = NULL;}
	if (rpcs) {*rpcs = NULL;}
	if (ns) { *ns = NULL;}
	if (prefix) { *prefix = NULL;}
	if (name) {*name = NULL;}
	if (version) {*version = NULL;}

	/* get name of the schema */
	if (name != NULL ) {
		result = xmlXPathEvalExpression (BAD_CAST "/yin:module", model_ctxt);
		if (result != NULL ) {
			if (result->nodesetval->nodeNr < 1) {
				xmlXPathFreeObject (result);
				return (EXIT_FAILURE);
			} else {
				*name = (char*) xmlGetProp (result->nodesetval->nodeTab[0], BAD_CAST "name");
			}
			xmlXPathFreeObject (result);
			if (*name == NULL ) {
				return (EXIT_FAILURE);
			}
		}
	}

	/* get version */
	if (version != NULL ) {
		result = xmlXPathEvalExpression (BAD_CAST "/yin:module/yin:revision", model_ctxt);
		if (result != NULL ) {
			if (result->nodesetval->nodeNr < 1) {
				*version = strdup("");
			} else {
				for (i = 0; i < result->nodesetval->nodeNr; i++) {
					xml_aux = xmlGetProp (result->nodesetval->nodeTab[i], BAD_CAST "date");
					if (*version == NULL ) {
						*version = (char*)xml_aux;
					} else if (xml_aux != NULL ) {
						l = strlen (*version); /* should be 10: YYYY-MM-DD */
						if (l != xmlStrlen (xml_aux)) {
							/* something strange happend ?!? - ignore this value */
							continue;
						}
						/* compare with currently the newest version and remember only when it is newer */
						for (j = 0; j < l; j++) {
							if (xml_aux[j] > (*version)[j]) {
								free (*version);
								*version = (char*)xml_aux;
								xml_aux = NULL;
								break;
							} else if (xml_aux[j] < (*version)[j]) {
								break;
							}
						}
						free (xml_aux);
					}
				}
			}
			xmlXPathFreeObject (result);
			if (*version == NULL ) {
				goto errorcleanup;
				return (EXIT_FAILURE);
			}
		}
	}

	/* get namespace of the schema */
	if (ns != NULL ) {
		result = xmlXPathEvalExpression (BAD_CAST "/yin:module/yin:namespace", model_ctxt);
		if (result != NULL ) {
			if (result->nodesetval->nodeNr < 1) {
				xmlXPathFreeObject (result);
				goto errorcleanup;
			} else {
				*ns = (char*) xmlGetProp (result->nodesetval->nodeTab[0], BAD_CAST "uri");
			}
			xmlXPathFreeObject (result);
			if (*ns == NULL ) {
				goto errorcleanup;
			}
		}
	}

	/* get prefix of the schema */
	if (ns != NULL ) {
		result = xmlXPathEvalExpression (BAD_CAST "/yin:module/yin:prefix", model_ctxt);
		if (result != NULL ) {
			if (result->nodesetval->nodeNr < 1) {
				*prefix = strdup("");
			} else {
				*prefix = (char*) xmlGetProp (result->nodesetval->nodeTab[0], BAD_CAST "value");
			}
			xmlXPathFreeObject (result);
			if (*prefix == NULL ) {
				goto errorcleanup;
			}
		}
	}

	if (rpcs != NULL ) {
		result = xmlXPathEvalExpression (BAD_CAST "/yin:module/yin:rpc", model_ctxt);
		if (result != NULL ) {
			if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
				*rpcs = malloc((result->nodesetval->nodeNr + 1) * sizeof(char*));
				if (*rpcs == NULL) {
					ERROR("Memory allocation failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
					xmlXPathFreeObject(result);
					goto errorcleanup;
				}
				for (i = j = 0; i < result->nodesetval->nodeNr; i++) {
					(*rpcs)[j] = (char*)xmlGetProp(result->nodesetval->nodeTab[i], BAD_CAST "name");
					if ((*rpcs)[j] != NULL) {
						j++;
					}
				}
				(*rpcs)[j] = NULL;
			}
			xmlXPathFreeObject (result);
		}
	}

	if (notifs != NULL ) {
		result = xmlXPathEvalExpression (BAD_CAST "/yin:module/yin:notification", model_ctxt);
		if (result != NULL ) {
			if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
				*notifs = malloc((result->nodesetval->nodeNr + 1) * sizeof(char*));
				if (*notifs == NULL) {
					ERROR("Memory allocation failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
					xmlXPathFreeObject(result);
					goto errorcleanup;
				}
				for (i = j = 0; i < result->nodesetval->nodeNr; i++) {
					(*notifs)[j] = (char*)xmlGetProp(result->nodesetval->nodeTab[i], BAD_CAST "name");
					if ((*notifs)[j] != NULL) {
						j++;
					}
				}
				(*notifs)[j] = NULL;
			}
			xmlXPathFreeObject (result);
		}
	}

	return (EXIT_SUCCESS);


errorcleanup:

	xmlFree(*name);
	*name = NULL;
	xmlFree(*version);
	*version = NULL;
	xmlFree(*ns);
	*ns = NULL;
	xmlFree(*prefix);
	*prefix = NULL;
	if (*rpcs != NULL) {
		for (i = 0; (*rpcs)[i] != NULL; i++) {
			free((*rpcs)[i]);
		}
		free(*rpcs);
		*rpcs = NULL;
	}
	if (*notifs != NULL) {
		for (i = 0; (*notifs)[i] != NULL; i++) {
			free((*notifs)[i]);
		}
		free(*notifs);
		*notifs = NULL;
	}

	return (EXIT_FAILURE);
}

/* used in ssh.c and session.c */
char** get_schemas_capabilities(struct nc_cpblts *cpblts)
{
	struct model_list* listitem;
	int i, j, k;
	char **retval = NULL, *auxstr, *comma;

	/* get size of the output */
	for (i = 0, listitem = models_list; listitem != NULL; listitem = listitem->next, i++);

	/* prepare output array */
	if ((retval = malloc(sizeof(char*) * (i + 1))) == NULL) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	for (i = 0, listitem = models_list; listitem != NULL; listitem = listitem->next, i++) {
		if (asprintf(&(retval[i]), "%s?module=%s%s%s%s", listitem->model->ns, listitem->model->name,
				(listitem->model->version != NULL && strnonempty(listitem->model->version)) ? "&amp;revision=" : "",
				(listitem->model->version != NULL && strnonempty(listitem->model->version)) ? listitem->model->version : "",
				(listitem->model->features != NULL) ? "&amp;features=" : "") == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			/* move iterator back, then iterator will go back to the current value and will rewrite it*/
			i--;
			continue;
		}
		if (listitem->model->features != NULL) {
			comma = "";
			for (j = 0; listitem->model->features[j] != NULL; j++) {
				if (listitem->model->features[j]->enabled) {
					if (strcmp(listitem->model->name, "ietf-netconf") == 0) {
						/* for netconf base data model, find the features in the
						 * capabilities list
						 */
						if (cpblts == NULL) {
							/* we don't know capabilities, all features are disabled */
							break;
						}

						for (k = 0; cpblts->list[k] != NULL; k++) {
							if (strstr(cpblts->list[k], listitem->model->features[j]->name) != NULL) {
								/* we got the capability, stop searching and note that we have found it */
								k = -1;
								break;
							}
						}
						if (k >= 0) {
							/* capability not found, continue with the next feature */
							continue;
						}
					}

					if (asprintf(&auxstr, "%s%s%s", retval[i], comma, listitem->model->features[j]->name) == -1) {
						ERROR("asprintf() failed (%s:%d)", __FILE__, __LINE__);
					}
					free(retval[i]);
					retval[i] = auxstr;
					auxstr = NULL;
					comma = ",";
				}
			}
			if (comma[0] == '\0') {
				/* no feature printed, hide features variable */
				retval[i][strlen(retval[i])-14] = '\0';
			}
		}
	}

	retval[i] = NULL;
	return (retval);
}

static char* get_schemas_str(const char* name, const char* version, const char* ns)
{
	char* retval = NULL;
	if (asprintf(&retval,"<schema><identifier>%s</identifier>"
			"<version>%s</version>"
			"<format>yin</format>"
			"<namespace>%s</namespace>"
			"<location>NETCONF</location>"
			"</schema>"
#ifndef DISABLE_YANGFORMAT
			"<schema><identifier>%s</identifier>"
			"<version>%s</version>"
			"<format>yang</format>"
			"<namespace>%s</namespace>"
			"<location>NETCONF</location>"
			"</schema>"
#endif
			,name,version,ns
#ifndef DISABLE_YANGFORMAT
			,name,version,ns
#endif
			) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		retval = NULL;
	}
	return (retval);
}

static char* get_schemas(void)
{
	char *schema = NULL, *schemas = NULL, *aux = NULL;
	struct model_list* listitem;

	for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
		aux = get_schemas_str(listitem->model->name,
				listitem->model->version,
				listitem->model->ns);
		if (schema == NULL) {
			schema = aux;
		} else if (aux != NULL) {
			void *tmp = realloc(schema, strlen(schema) + strlen(aux) + 1);
			if (tmp == NULL) {
				ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
				free(aux);
				/* return what we have */
				break;
			} else {
				schema = tmp;
				strcat(schema, aux);
				free(aux);
			}
		}
	}

	if (schema != NULL) {
		if (asprintf(&schemas, "<schemas>%s</schemas>", schema) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			schemas = NULL;
		}
		free(schema);
	}
	return (schemas);
}

#ifndef DISABLE_NOTIFICATIONS
static char* get_state_notifications(const char* UNUSED(model), const char* UNUSED(running), struct nc_err ** UNUSED(e))
{
	char *retval = NULL;

	/*
	 * notifications streams
	 */
	retval = ncntf_status ();
	if (retval == NULL ) {
		retval = strdup("");
	}

	return (retval);
}
#endif /* DISABLE_NOTIFICATIONS */

static char* get_state_monitoring(const char* UNUSED(model), const char* UNUSED(running), struct nc_err** UNUSED(e))
{
	char *schemas = NULL, *sessions = NULL, *retval = NULL, *ds_stats = NULL, *ds_startup = NULL, *ds_cand = NULL, *stats = NULL, *aux = NULL;
	struct ncds_ds_list* ds = NULL;
	const struct ncds_lockinfo *info;

	/*
	 * datastores
	 */
	/* find non-empty datastore implementation */
	for (ds = ncds.datastores; ds != NULL ; ds = ds->next) {
		if (ds->datastore && ds->datastore->type != NCDS_TYPE_EMPTY) {
			if (ds->datastore->func.get_lockinfo) {
				break;
			}
		}
	}

	if (ds != NULL) {
		/* startup datastore */
		info = ds->datastore->func.get_lockinfo(ds->datastore, NC_DATASTORE_STARTUP);
		if (info != NULL && info->sid != NULL) {
			if (asprintf(&aux, "<locks><global-lock><locked-by-session>%s</locked-by-session>"
					"<locked-time>%s</locked-time></global-lock></locks>", info->sid, info->time) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				aux = NULL;
			}
		}
		if (asprintf(&ds_startup, "<datastore><name>startup</name>%s</datastore>",
		                (aux != NULL) ? aux : "") == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			ds_startup = NULL;
		}
		free(aux);
		aux = NULL;

		/* candidate datastore */
		info = ds->datastore->func.get_lockinfo(ds->datastore, NC_DATASTORE_CANDIDATE);
		if (info != NULL && info->sid != NULL) {
			if (asprintf(&aux, "<locks><global-lock><locked-by-session>%s</locked-by-session>"
					"<locked-time>%s</locked-time></global-lock></locks>", info->sid, info->time) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				aux = NULL;
			}
		}
		if (asprintf(&ds_cand, "<datastore><name>candidate</name>%s</datastore>",
		                (aux != NULL) ? aux : "") == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			ds_cand = NULL;
		}
		free(aux);
		aux = NULL;

		/* running datastore */
		info = ds->datastore->func.get_lockinfo (ds->datastore, NC_DATASTORE_RUNNING);
		if (info != NULL && info->sid != NULL ) {
			if (asprintf (&aux, "<locks><global-lock><locked-by-session>%s</locked-by-session>"
					"<locked-time>%s</locked-time></global-lock></locks>", info->sid, info->time) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				aux = NULL;
			}
		}
		if (asprintf (&ds_stats, "<datastores><datastore><name>running</name>%s</datastore>%s%s</datastores>",
		        (aux != NULL )? aux : "",
		        (ds_startup != NULL) ? ds_startup : "",
		        (ds_cand != NULL) ? ds_cand : "") == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			ds_stats = NULL;
		}
		free (ds_startup);
		free (ds_cand);
		free (aux);
	}

	/*
	 * schemas
	 */
	schemas = get_schemas();

	/*
	 * sessions
	 */
	sessions = nc_session_stats();

	/*
	 * statistics
	 */
	if (nc_info != NULL) {
		pthread_rwlock_rdlock(&(nc_info->lock));
		if (asprintf(&stats, "<statistics><netconf-start-time>%s</netconf-start-time>"
				"<in-bad-hellos>%u</in-bad-hellos>"
				"<in-sessions>%u</in-sessions>"
				"<dropped-sessions>%u</dropped-sessions>"
				"<in-rpcs>%u</in-rpcs>"
				"<in-bad-rpcs>%u</in-bad-rpcs>"
				"<out-rpc-errors>%u</out-rpc-errors>"
				"<out-notifications>%u</out-notifications></statistics>",
				nc_info->stats.start_time,
				nc_info->stats.bad_hellos,
				nc_info->stats.sessions_in,
				nc_info->stats.sessions_dropped,
				nc_info->stats.counters.in_rpcs,
				nc_info->stats.counters.in_bad_rpcs,
				nc_info->stats.counters.out_rpc_errors,
				nc_info->stats.counters.out_notifications) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			stats = NULL;
		}
		pthread_rwlock_unlock(&(nc_info->lock));
	}

	/* get it all together */
        //pthread_spin_lock(&server_cpblt_lock);
	if (asprintf(&retval, "<netconf-state xmlns=\"%s\">%s%s%s%s%s</netconf-state>", NC_NS_MONITORING,
			(server_capabilities != NULL) ? server_capabilities : "",
			(ds_stats != NULL) ? ds_stats : "",
			(sessions != NULL) ? sessions : "",
			(schemas != NULL) ? schemas : "",
			(stats != NULL) ? stats : "") == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		retval = NULL;
	}
        //pthread_spin_unlock(&server_cpblt_lock);
	if (retval == NULL) {
		retval = strdup("");
	}

	free(ds_stats);
	free(sessions);
	free(schemas);
	free(stats);

	return (retval);
}

static char* get_state_nacm(const char* UNUSED(model), const char* UNUSED(running), struct nc_err** UNUSED(e))
{
	char* retval = NULL;

	if (nc_info != NULL ) {
		pthread_rwlock_rdlock(&(nc_info->lock));
		if (asprintf(&retval, "<nacm xmlns=\"%s\">"
				"<denied-operations>%u</denied-operations>"
				"<denied-data-writes>%u</denied-data-writes>"
				"<denied-notifications>%u</denied-notifications>"
				"</nacm>",
				NC_NS_NACM,
				nc_info->stats_nacm.denied_ops,
				nc_info->stats_nacm.denied_data,
				nc_info->stats_nacm.denied_notifs) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			retval = NULL;
		}
		pthread_rwlock_unlock(&(nc_info->lock));
	}
	if (retval == NULL) {
		retval = strdup("");
	}

	return (retval);
}

static char* compare_schemas(struct data_model* model, char* name, char* version)
{
	char* retval = NULL;
	FILE* file;
	struct stat st;
	int size, c, start;

	if (strcmp(name, model->name) == 0) {
		if (version == NULL || strcmp(version, model->version) == 0) {
			/* an internal model */
			if (strncmp(model->path, "internal", 8) == 0) {
				switch (model->path[9]) {
				case '0':
					return strndup((char*)ietf_inet_types_yin + 39, ietf_inet_types_yin_len - 39);
				case '1':
					return strndup((char*)ietf_yang_types_yin + 39, ietf_yang_types_yin_len - 39);
				case '2':
					return strndup((char*)ietf_netconf_yin + 39, ietf_netconf_yin_len - 39);
				case '3':
					return strndup((char*)ietf_netconf_monitoring_yin + 39, ietf_netconf_monitoring_yin_len - 39);
#ifndef DISABLE_NOTIFICATIONS
				case '4':
					return strndup((char*)ietf_netconf_notifications_yin + 39, ietf_netconf_notifications_yin_len - 39);
				case '5':
					return strndup((char*)nc_notifications_yin + 39, nc_notifications_yin_len - 39);
				case '6':
					return strndup((char*)notifications_yin + 39, notifications_yin_len - 39);
				case '7':
					return strndup((char*)ietf_netconf_with_defaults_yin + 39, ietf_netconf_with_defaults_yin_len - 39);
				case '8':
					return strndup((char*)ietf_netconf_acm_yin + 39, ietf_netconf_acm_yin_len - 39);
#else
				case '4':
					return strndup((char*)ietf_netconf_with_defaults_yin + 39, ietf_netconf_with_defaults_yin_len - 39);
				case '5':
					return strndup((char*)ietf_netconf_acm_yin + 39, ietf_netconf_acm_yin_len - 39);
#endif
				default:
					ERROR("%s: internal (%s:%d)", __func__, __FILE__, __LINE__);
					return (ERROR_POINTER);
				}
			}

			/* got the required model, load it */
			if (stat(model->path, &st)) {
				ERROR("%s: failed to stat \"%s\" (%s).", __func__, model->path, strerror(errno));
				return (ERROR_POINTER);
			}
			size = st.st_size;
			file = fopen(model->path, "r");
			if (file == NULL) {
				ERROR("%s: failed to open \"%s\" (%s).", __func__, model->path, strerror(errno));
				return (ERROR_POINTER);
			}
			retval = malloc(size + 1);
			/* perhaps starts with "<?xml"? */
			if (fread(retval, 1, 5, file) < 5) {
				ERROR("%s: failed to read \"%s\" (%s).", __func__, model->path, strerror(errno));
				fclose(file);
				return (ERROR_POINTER);
			}
			size -= 5;
			/* skip the instruction */
			if (strncmp(retval, "<?xml", 5) == 0) {
				do {
					c = fgetc(file);
					--size;
					if (c == '?') {
						c = fgetc(file);
						--size;
						if (c == '>') {
							break;
						}
					}
				} while (c != EOF);
				if (c == EOF) {
					ERROR("%s: failed to read \"%s\" (%s).", __func__, model->path, strerror(errno));
					fclose(file);
					return (ERROR_POINTER);
				}
				start = 0;
			} else {
				start = 5;
			}
			/* read the rest */
			if (fread(retval + start, 1, size, file) < (unsigned)size) {
				ERROR("%s: failed to read \"%s\" (%s).", __func__, model->path, strerror(errno));
				fclose(file);
				return (ERROR_POINTER);
			}
			fclose(file);
			retval[start + size] = '\0';
		}
	}
	return (retval);
}

static char* get_schema(const nc_rpc* rpc, struct nc_err** e)
{
	xmlXPathObjectPtr query_result = NULL;
#ifndef DISABLE_YANGFORMAT
	xmlBufferPtr data_buf;
	xmlDocPtr yang_doc, yin_doc;
	xmlNodePtr node;
#endif
	char *name = NULL, *version = NULL, *format = NULL;
	char *retval = NULL, *r = NULL;
	struct model_list* listitem;

	/* get name of the schema */
	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_MONITORING_ID":get-schema/"NC_NS_MONITORING_ID":identifier", rpc->ctxt)) != NULL &&
			!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
		if (query_result->nodesetval->nodeNr > 1) {
			ERROR("%s: multiple identifier elements found", __func__);
			*e = nc_err_new(NC_ERR_BAD_ELEM);
			nc_err_set(*e, NC_ERR_PARAM_INFO_BADELEM, "identifier");
			nc_err_set(*e, NC_ERR_PARAM_MSG, "Multiple \'identifier\' elements found.");
			xmlXPathFreeObject(query_result);
			return (NULL);
		}
		name = (char*) xmlNodeGetContent(query_result->nodesetval->nodeTab[0]);
		xmlXPathFreeObject(query_result);
	} else {
		if (query_result != NULL) {
			xmlXPathFreeObject(query_result);
		}
		ERROR("%s: missing a mandatory identifier element", __func__);
		*e = nc_err_new(NC_ERR_INVALID_VALUE);
		nc_err_set(*e, NC_ERR_PARAM_INFO_BADELEM, "identifier");
		nc_err_set(*e, NC_ERR_PARAM_MSG, "Missing mandatory \'identifier\' element.");
		return (NULL);
	}

	/* get version of the schema */
	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_MONITORING_ID":get-schema/"NC_NS_MONITORING_ID":version", rpc->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple version elements found", __func__);
				*e = nc_err_new(NC_ERR_BAD_ELEM);
				nc_err_set(*e, NC_ERR_PARAM_INFO_BADELEM, "version");
				nc_err_set(*e, NC_ERR_PARAM_MSG, "Multiple \'version\' elements found.");
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			version = (char*) xmlNodeGetContent(query_result->nodesetval->nodeTab[0]);
		}
		xmlXPathFreeObject(query_result);
	}

	/* get format of the schema */
	if ((query_result = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_MONITORING_ID":get-schema/"NC_NS_MONITORING_ID":format", rpc->ctxt)) != NULL) {
		if (!xmlXPathNodeSetIsEmpty(query_result->nodesetval)) {
			if (query_result->nodesetval->nodeNr > 1) {
				ERROR("%s: multiple version elements found", __func__);
				*e = nc_err_new(NC_ERR_BAD_ELEM);
				nc_err_set(*e, NC_ERR_PARAM_INFO_BADELEM, "version");
				nc_err_set(*e, NC_ERR_PARAM_MSG, "Multiple \'version\' elements found.");
				xmlXPathFreeObject(query_result);
				return (NULL);
			}
			format = (char*) xmlNodeGetContent(query_result->nodesetval->nodeTab[0]);
			char* colon = strrchr(format, ':');
			if (colon) {
				char* old = format;
				format = strdup(colon + 1);
				free(old);
			}
		}
		xmlXPathFreeObject(query_result);
	}

	if (format == NULL) {
		/*
		 * format is missing, use the default format - RFC 6022 specifies YANG
		 * as a default format. Due to intercompatibility, it must be used
		 * even if the YANG format is not available (it was turned off by
		 * --disable-yang-schemas), because clients expects to get YANG format
		 * when they explicitly do not specify the schema format.
		 */
		format = strdup("yang");
	}

	/* process all data models */
	for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
		r = compare_schemas(listitem->model, name, version);
		if (r == ERROR_POINTER) {
			if (e != NULL ) {
				*e = nc_err_new(NC_ERR_OP_FAILED);
			}
			free(retval);
			retval = NULL;
			goto cleanup;
		} else if (r && retval) {
			/* schema is not unique according to request */
			free(r);
			if (e != NULL ) {
				*e = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(*e, NC_ERR_PARAM_APPTAG, "data-not-unique");
				nc_err_set(*e, NC_ERR_PARAM_MSG, "More than one schema matches the requested parameters.");
			}
			free(retval);
			retval = NULL;
			goto cleanup;
		} else if (r != NULL ) {
			/* the first matching schema found */
			retval = r;
			break;
		}
	}

	/* return correct format */
#ifndef DISABLE_YANGFORMAT
	if (retval != NULL && strcmp(format, "yang") == 0) {
		/* convert YIN to YANG */
		yin_doc = xmlReadDoc(BAD_CAST retval, NULL, NULL, NC_XMLREAD_OPTIONS);
		yang_doc = xsltApplyStylesheet(yin2yang_xsl, yin_doc, NULL);
		xmlFreeDoc(yin_doc);
		free(retval);
		if (yang_doc == NULL || yang_doc->children == NULL) {
			if (e != NULL ) {
				*e = nc_err_new(NC_ERR_OP_FAILED);
			}
			retval = NULL;
			goto cleanup;
		}

		data_buf = xmlBufferCreate();
		for (node = yang_doc->children; node != NULL; node = node->next) {
			if (node->type == XML_TEXT_NODE) {
				xmlNodeDump(data_buf, yang_doc, node, 1, 1);
			}
		}
		r = (char*) xmlBufferContent(data_buf);
		if (r != NULL) {
			retval = strdup(r);
		} else {
			if (e != NULL ) {
				*e = nc_err_new(NC_ERR_OP_FAILED);
			}
			retval = NULL;
			goto cleanup;
		}
		xmlBufferFree(data_buf);
		xmlFreeDoc(yang_doc);
	} else
#endif
		if (retval != NULL && strcmp(format, "yin") == 0) {
		/* default format */
	} else if (retval != NULL) {
		/* unsupported format */
		free(retval);
		retval = NULL;
	}

	if (retval == NULL) {
		*e = nc_err_new(NC_ERR_INVALID_VALUE);
		nc_err_set(*e, NC_ERR_PARAM_TYPE, "protocol");
		nc_err_set(*e, NC_ERR_PARAM_MSG, "The requested schema does not exist.");
	}

cleanup:
	/* cleanup */
	free(version);
	free(name);
	free(format);

	return (retval);
}

static struct transapi_internal* transapi_new_shared(const char* callbacks_path)
{
	void *transapi_module = NULL;
	xmlDocPtr (*get_state)(const xmlDocPtr, const xmlDocPtr, struct nc_err **) = NULL;
	void (*close_func)(void) = NULL;
	int (*init_func)(xmlDocPtr *) = NULL;
	struct transapi_data_callbacks *data_clbks = NULL;
	struct transapi_rpc_callbacks *rpc_clbks = NULL;
	struct transapi_file_callbacks *file_clbks = NULL;
	int *ver, ver_default = 1;
	int *modified;
	NC_EDIT_ERROPT_TYPE *erropt;
	char * ns_mapping = NULL;
	TRANSAPI_CLBCKS_ORDER_TYPE *clbks_order;
	struct transapi_internal* transapi;

	/* load shared library */
	if ((transapi_module = dlopen (callbacks_path, RTLD_NOW)) == NULL) {
		ERROR("Unable to load shared library (%s).", dlerror());
		dlerror(); /* clear memory in dl */
		return (NULL);
	}

	/* check transAPI version used to built the module */
	if ((ver = dlsym (transapi_module, "transapi_version")) == NULL) {
		WARN("transAPI version in module %s not found. Probably version 1, update your module.", callbacks_path);
		ver = &ver_default;
	}
	if (*ver != TRANSAPI_VERSION) {
		ERROR("Wrong transAPI version of the module %s. Have %d, but %d is required.", callbacks_path, *ver, TRANSAPI_VERSION);
		dlclose (transapi_module);
		return (NULL);
	}

	if ((modified = dlsym(transapi_module, "config_modified")) == NULL) {
		ERROR("Missing config_modified variable in %s transAPI module.", callbacks_path);
		dlclose (transapi_module);
		return (NULL);
	}

	if ((erropt = dlsym(transapi_module, "erropt")) == NULL) {
		ERROR("Missing erropt variable in %s transAPI module.", callbacks_path);
		dlclose (transapi_module);
		return (NULL);
	}

	/* find get_state function */
	if ((get_state = dlsym (transapi_module, "get_state_data")) == NULL) {
		ERROR("Missing get_state_data() function in %s transAPI module.", callbacks_path);
		dlclose (transapi_module);
		return (NULL);
	}

	if ((ns_mapping = dlsym(transapi_module, "namespace_mapping")) == NULL) {
		ERROR("Missing mapping of prefixes with URIs in %s transAPI module.", callbacks_path);
		dlclose(transapi_module);
		return(NULL);
	}

	/* find rpc callback functions mapping structure */
	if ((rpc_clbks = dlsym(transapi_module, "rpc_clbks")) == NULL) {
		VERB("No RPC callbacks in %s transAPI module.", callbacks_path);
	}

	if ((clbks_order = dlsym (transapi_module, "callbacks_order")) == NULL) {
		WARN("%s: Unable to find \"callbacks_order\" variable. Guessing Leaf To Root.", __func__);
	}

	if ((file_clbks = dlsym (transapi_module, "file_clbks")) == NULL) {
		VERB("No FMON callback in %s transAPI module.", callbacks_path);
	}

	/* callbacks work with configuration data */
	/* get clbks structure */
	if ((data_clbks = dlsym (transapi_module, "clbks")) == NULL) {
		WARN("%s: No data callbacks in %s transAPI module.", callbacks_path);
		return (NULL);
	}

	if ((init_func = dlsym (transapi_module, "transapi_init")) == NULL) {
		VERB("No transapi_init() function in %s transAPI module.", callbacks_path);
	}

	if ((close_func = dlsym (transapi_module, "transapi_close")) == NULL) {
		VERB("No transapi_close() function in %s transAPI module.", callbacks_path);
	}

	/* allocate transapi structure */
	if ((transapi = malloc(sizeof(struct transapi_internal))) == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	/* fill transapi structure */
	transapi->module = transapi_module;
	transapi->config_modified = modified;
	transapi->erropt = erropt;
	transapi->ns_mapping = (struct ns_pair*)ns_mapping;
	transapi->data_clbks = data_clbks;
	transapi->rpc_clbks = rpc_clbks;
	transapi->file_clbks = file_clbks;
	/* Convert clbks_order to enum */
	transapi->clbks_order = TRANSAPI_CLBCKS_ORDER_DEFAULT;
	if (clbks_order != NULL)
		transapi->clbks_order = *clbks_order;
	transapi->init = init_func;
	transapi->close = close_func;
	transapi->get_state = get_state;

	return (transapi);
}

API struct ncds_ds* ncds_new_transapi(NCDS_TYPE type, const char* model_path, const char* callbacks_path)
{
	struct ncds_ds* ds = NULL;
	struct transapi_list *item;
	struct transapi_internal* transapi;

	if (callbacks_path == NULL) {
		ERROR("%s: missing callbacks path parameter.", __func__);
		return (NULL);
	}

	if ((transapi = transapi_new_shared(callbacks_path)) == NULL) {
		ERROR ("%s: Failed to prepare transAPI structures.", __func__);
		return (NULL);
	}

	/* create basic ncds_ds structure */
	if ((ds = ncds_new2(type, model_path, transapi->get_state)) == NULL) {
		ERROR ("%s: Failed to create ncds_ds structure.", __func__);
		return (NULL);
	}

	/* create transpi list item */
	if ((item = malloc(sizeof(struct transapi_list))) == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	/*
	 * base transAPI module directly connected with the datastore has non-zero
	 * ref_count since it is not stored in the global augment_tapi_list
	 */
	item->tapi = transapi;
	item->ref_count = 1;
	item->next = NULL;
	ds->transapis = item;

	return ds;
}

API struct ncds_ds* ncds_new_transapi_static(NCDS_TYPE type, const char* model_path, const struct transapi* transapi)
{
	struct transapi_list *item;
	struct ncds_ds *ds = NULL;

	/* transAPI module information checks */
	if (transapi == NULL) {
		ERROR("%s: Missing transAPI module description.", __func__);
		return (NULL);
	}

	if (transapi->version != TRANSAPI_VERSION) {
		ERROR("%s: Wrong transAPI static module version (version %d is required).", __func__, TRANSAPI_VERSION);
		return (NULL);
	}

	if (transapi->config_modified == NULL) {
		ERROR("%s: Missing config_modified variable in transAPI module description.", __func__);
		return (NULL);
	}
	if (transapi->erropt == NULL) {
		ERROR("%s: Missing erropt variable in transAPI module description.", __func__);
		return (NULL);
	}
	if (transapi->get_state == NULL) {
		ERROR("%s: Missing get_state() function in transAPI module description.", __func__);
		return (NULL);
	}
	if (transapi->ns_mapping == NULL) {
		ERROR("%s: Missing mapping of prefixes with URIs in transAPI module description.", __func__);
		return (NULL);
	}
	/* empty datastore has no data */
	if (type != NCDS_TYPE_EMPTY) {
		/* get clbks structure */
		if (transapi->data_clbks == NULL) {
			ERROR("%s: Missing data callbacks in transAPI module description.", __func__);
			return (NULL);
		}
	}

	if ((item = malloc(sizeof(struct transapi_list))) == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}
	/* allocate transapi structure */
	if ((item->tapi = malloc(sizeof(struct transapi_internal))) == NULL) {
		ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		free(item);
		return (NULL);
	}
	/* create basic ncds_ds structure */
	if ((ds = ncds_new2(type, model_path, transapi->get_state)) == NULL) {
		ERROR ("%s: Failed to create ncds_ds structure.", __func__);
		free(item->tapi);
		free(item);
		return (NULL);
	}
	/*
	 * base transAPI module directly connected with the datastore has non-zero
	 * ref_count since it is not stored in the global augment_tapi_list
	 */
	item->ref_count = 1;
	item->next = NULL;
	ds->transapis = item;

	/* copy transAPI module info into a internal structure
	 * NOTE: copy only the beginning part common for struct transapi and
	 * struct transapi_internal
	 */
	memcpy(ds->transapis->tapi, transapi, (sizeof(struct transapi)));
	/*
	 * mark it as transAPI (non-NULL), but remember that it is not a dynamically
	 * linked transAPI module
	 */
	ds->transapis->tapi->module = &error_area;



	return (ds);
}

static struct data_model* data_model_new(const char* model_path)
{
	struct data_model *model = NULL;

	if (model_path == NULL ) {
		ERROR("%s: invalid parameter.", __func__);
		return (NULL);
	}

	/* get configuration data model */
	if (eaccess(model_path, R_OK) == -1) {
		ERROR("Unable to access the configuration data model %s (%s).", model_path, strerror(errno));
		return (NULL);
	}

	model = calloc(1, sizeof(struct data_model));
	if (model == NULL) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	model->xml = xmlReadFile(model_path, NULL, NC_XMLREAD_OPTIONS);
	if (model->xml == NULL) {
		ERROR("Unable to read the configuration data model %s.", model_path);
		free(model);
		return (NULL);
	}

	/* prepare xpath evaluation context of the model for XPath */
	if ((model->ctxt = xmlXPathNewContext(model->xml)) == NULL) {
		ERROR("%s: Creating XPath context failed.", __func__);
		xmlFreeDoc(model->xml);
		free(model);
		return (NULL);
	}
	if (xmlXPathRegisterNs(model->ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
		xmlXPathFreeContext(model->ctxt);
		xmlFreeDoc(model->xml);
		free(model);
		return (NULL);
	}

	if (get_model_info(model->ctxt,
			&(model->name),
			&(model->version),
			&(model->ns),
			&(model->prefix),
			&(model->rpcs),
			&(model->notifs)) != 0) {
		ERROR("Unable to process configuration data model %s.", model_path);
		xmlXPathFreeContext(model->ctxt);
		xmlFreeDoc(model->xml);
		free(model);
		return (NULL);
	}
	model->path = strdup(model_path);
	ncds_features_parse(model);

	/* resolve uses statements in groupings and augments */
	ncds_update_uses_groupings(model);
	ncds_update_uses_augments(model);

	return (model);
}

static int data_model_enlink(struct data_model** model)
{
	struct model_list *listitem;

	if (model == NULL || *model == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	/* check duplicity */
	for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
		if (listitem->model &&
			strcmp(listitem->model->name, (*model)->name) == 0 &&
			strcmp(listitem->model->version, (*model)->version) == 0) {
			/* module already found */
			VERB("Module to enlink \"%s\" already exists.", (*model)->name);
			ncds_ds_model_free(*model);
			*model = listitem->model;
			return (EXIT_SUCCESS);
		}
	}

	/* update internal model lists */
	listitem = malloc(sizeof(struct model_list));
	if (listitem == NULL) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (EXIT_FAILURE);
	}
	listitem->model = *model;
	listitem->next = models_list;
	models_list = listitem;

	return (EXIT_SUCCESS);
}

static int match_module_node(char* path_module, char* module, char* name, xmlNodePtr *node)
{
	char* name_aux;

	if (path_module == NULL || module == NULL || name == NULL || node == NULL) {
		return (0);
	}

	if (strcmp(module, path_module) == 0) {
		/* we have the match - move into the specified element */
		while (*node != NULL ) {
			if (xmlStrcmp((*node)->name, BAD_CAST "container") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "list") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "choice") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "case") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "notification") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "leaf") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "leaf-list") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "anyxml") == 0) {
				/* if the target is one of these, check its name attribute */
				if ((name_aux = (char*) xmlGetProp(*node, BAD_CAST "name")) == NULL ) {
					*node = (*node)->next;
					continue;
				}
				if (strcmp(name_aux, name) == 0) {
					free(name_aux);
					return (1);
				}
				free(name_aux);
				*node = (*node)->next;
			} else if (xmlStrcmp((*node)->name, BAD_CAST "input") == 0 ||
			    xmlStrcmp((*node)->name, BAD_CAST "output") == 0) {
				/* if the target is one of these, it has no name attribute, check directly its name (not attribute) */
				if (xmlStrcmp((*node)->name, BAD_CAST name) == 0) {
					return (1);
				}
				*node = (*node)->next;
			} else {
				/* target cannot be anything else (RFC 6020, sec. 7.15) */
				*node = (*node)->next;
			}
		}
	}

	return (0);
}

/* find the prefix in imports */
static char* get_module_with_prefix(const char* prefix, xmlXPathObjectPtr imports)
{
	int j;
	char *val, *module;
	xmlNodePtr node;

	if (prefix == NULL || imports == NULL) {
		return (NULL);
	}

	for (j = 0; j < imports->nodesetval->nodeNr; j++) {
		for (node = imports->nodesetval->nodeTab[j]->children; node != NULL ; node = node->next) {
			if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, BAD_CAST "prefix") == 0) {
				break;
			}
		}
		if (node != NULL ) {
			if ((val = (char*) xmlGetProp(node, BAD_CAST "value")) == NULL ) {
				continue;
			}
			if (strcmp(val, prefix) == 0) {
				free(val);
				if ((module = (char*) xmlGetProp(imports->nodesetval->nodeTab[j], BAD_CAST "module")) == NULL ) {
					continue;
				} else {
					/* we have the module name */
					return (module);
				}
			}
			free(val);
		}
	}

	return (NULL);
}

static struct data_model* get_model2(const char* model_path)
{
	struct model_list* listitem;

	if (model_path == NULL) {
		return (NULL);
	}

	for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
		if (listitem->model && listitem->model->path &&
				strcmp(listitem->model->path, model_path) == 0) {
			/* module found */
			return (listitem->model);
		}
	}

	return (NULL);
}

static struct data_model *read_model(const char* model_path)
{
	struct data_model *model;

	if (model_path == NULL) {
		ERROR("%s: invalid parameter model_path.", __func__);
		return (NULL);
	}

	if ((model = get_model2(model_path)) != NULL) {
		/* model is already loaded */
		return (model);
	}

	/* get configuration data model information */
	if ((model = data_model_new(model_path)) == NULL) {
		return (NULL);
	}

	/* add a new model into the internal lists */
	if (data_model_enlink(&model) != EXIT_SUCCESS) {
		ERROR("Adding new data model failed.");
		ncds_ds_model_free(model);
		return (NULL);
	}

	return (model);
}

static struct data_model* get_model(const char* module, const char* version)
{
	struct model_list* listitem;
	struct data_model* model = NULL;
	int i, r;
	char* aux, *aux2;
	DIR* dir;
	struct dirent* file;

	if (module == NULL) {
		return (NULL);
	}

	for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
		if (listitem->model && strcmp(listitem->model->name, module) == 0) {
			if (version != NULL) {
				if (strcmp(listitem->model->version, version) == 0) {
					/* module found */
					return (listitem->model);
				} else {
					/* module version does not match */
					continue;
				}
			} else {
				/* module found - specific version is not required */
				return (listitem->model);
			}
		}
	}

	/* module not found - try to find it in a models directories */
	if (models_dirs != NULL) {
		for (i = 0; models_dirs[i]; i++) {
			aux = NULL;
			if (asprintf(&aux, "%s/%s.yin", models_dirs[i], module) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				aux = NULL;
			}
			if (access(aux, R_OK) == 0) {
				/* we have found the correct module - probably */
				model = read_model(aux);
				if (model != NULL) {
					if (strcmp(model->name, module) != 0) {
						/* read module is incorrect */
						ncds_ds_model_free(model);
						model = NULL;
					}
				}
			} else {
				/*
				 * filename can contain also revision, so try to open
				 * all suitable files
				 */
				free(aux);
				if (version == NULL) {
					r = asprintf(&aux, "%s@", module);
				} else {
					r = asprintf(&aux, "%s@%s", module, version);
				}
				if (r == -1) {
					ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
					/* try next item in models_dirs list */
					continue;
				}
				dir = opendir(models_dirs[i]);
				while((file = readdir(dir)) != NULL) {
					if (strncmp(file->d_name, aux, strlen(aux)) == 0 &&
					    strcmp(&(file->d_name[strlen(file->d_name)-4]), ".yin") == 0) {
						if (asprintf(&aux2, "%s/%s", models_dirs[i], file->d_name) == -1) {
							ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
							continue;
						}
						model = read_model(aux2);
						free(aux2);
						if (model != NULL) {
							if (strcmp(model->name, module) != 0) {
								/* read module is incorrect */
								ncds_ds_model_free(model);
								model = NULL;
							}
						}
					}
				}
				closedir(dir);
			}
			free(aux);
			if (model != NULL) {
				return (model);
			}
		}
	}

	return (model);
}

/**
 * @param[in] datastore - datastore structure where the other data models will be imported
 * @param[in] model_ctxt - XPath context of the datastore's extended data model
 */
static int import_groupings(const char* module_name, xmlXPathContextPtr model_ctxt)
{
	xmlXPathObjectPtr imports, groupings;
	xmlNodePtr node, node_aux;
	xmlNsPtr ns;
	struct data_model* model;
	char *module, *revision, *prefix, *grouping_name, *aux;
	int i, j, r;

	aux = (char*) xmlGetNsProp(xmlDocGetRootElement(model_ctxt->doc), BAD_CAST "import", BAD_CAST "libnetconf");
	if (aux != NULL && strcmp(aux, "done") == 0) {
		/* import is already done by previous call */
		free(aux);
		return (EXIT_SUCCESS);
	}

	/* copy grouping definitions from imported models */
	if ((imports = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_YIN_ID":module/"NC_NS_YIN_ID":import", model_ctxt)) == NULL ) {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		return (EXIT_FAILURE);
	}else if (!xmlXPathNodeSetIsEmpty(imports->nodesetval)) {
		/* we have something to import */
		for (i = 0; i < imports->nodesetval->nodeNr; i++) {
			revision = NULL;
			prefix = NULL;
			module = (char*) xmlGetProp(imports->nodesetval->nodeTab[i], BAD_CAST "module");
			if (module == NULL) {
				WARN("%s: invalid import statement - missing module reference.", __func__);
				continue;
			}
			for (node = imports->nodesetval->nodeTab[i]->children; node != NULL; node = node->next) {
				if (node->type != XML_ELEMENT_NODE ||
				    node->ns == NULL || node->ns->href == NULL ||
				    xmlStrcmp(node->ns->href, BAD_CAST NC_NS_YIN) != 0) {
					continue;
				}

				if (prefix == NULL && xmlStrcmp(node->name, BAD_CAST "prefix") == 0) {
					prefix = (char*) xmlGetProp(node, BAD_CAST "value");
				} else if (revision == NULL && xmlStrcmp(node->name, BAD_CAST "revision-date") == 0) {
					revision = (char*) xmlGetProp(node, BAD_CAST "value");
				}

				if (prefix != NULL && revision != NULL) {
					break;
				}
			}
			if (prefix == NULL) {
				ERROR("Invalid YIN module \'%s\' - missing prefix for imported \'%s\' module.", module_name, module);
				free(revision);
				free(module);
				return(EXIT_FAILURE);
			}
			model = get_model(module, revision);
			free(revision);
			if (model == NULL) {
				if (strcmp(module, "ietf-netconf-acm") == 0) {
					WARN("NACM turned off, module \'ietf-netconf-acm\' is not available for import from \'%s\'.", module_name);
					free(module);
					free(prefix);
					continue;
				}
				ERROR("Missing YIN module \'%s\' imported from \'%s\'.", module, module_name);
				free(module);
				free(prefix);
				xmlXPathFreeObject(imports);
				return(EXIT_FAILURE);
			}
			free(module);

			/* import grouping definitions */
			if ((groupings = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_YIN_ID":module//"NC_NS_YIN_ID":grouping", model->ctxt)) != NULL ) {
				/* add prefix into the grouping names and add imported grouping into the overall data model */
				r = 0;
				for (j = 0; (r != -1) && (j < groupings->nodesetval->nodeNr); j++) {
					node = xmlCopyNode(groupings->nodesetval->nodeTab[j], 1);
					grouping_name = (char*) xmlGetProp(node, BAD_CAST "name");
					if ((r = asprintf(&aux, "%s:%s", prefix, grouping_name)) == -1) {
						ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
					} else {
						xmlSetProp(node, BAD_CAST "name", BAD_CAST aux);
						xmlAddChild(xmlDocGetRootElement(model_ctxt->doc), node);
					}
					free(aux);
					free(grouping_name);
				}
				free(prefix);
				xmlXPathFreeObject(groupings);

				if (r == -1) {
					/* asprintf() in for loop failed */
					return (EXIT_FAILURE);
				}
			} else {
				ERROR("%s: Evaluating XPath expression failed.", __func__);
				free(prefix);
				xmlXPathFreeObject(imports);
				return (EXIT_FAILURE);
			}
		}
		/* import done - note it */
		ns = xmlNewNs(xmlDocGetRootElement(model_ctxt->doc), BAD_CAST "libnetconf", BAD_CAST "libnetconf");
		xmlSetNsProp(xmlDocGetRootElement(model_ctxt->doc), ns, BAD_CAST "import", BAD_CAST "done");
	}
	xmlXPathFreeObject(imports);

	/* get all grouping statements in the document and remove unneeded nodes */
	if ((groupings = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_YIN_ID":module//"NC_NS_YIN_ID":grouping", model_ctxt)) == NULL ) {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		return (EXIT_FAILURE);
	}
	if (!xmlXPathNodeSetIsEmpty(groupings->nodesetval)) {
		for (j = 0; j < groupings->nodesetval->nodeNr; j++) {
			/* remove unused data from the groupings */
			for (node = groupings->nodesetval->nodeTab[j]->children; node != NULL; ) {
				/*remember the next node */
				node_aux = node->next;

				if (node->type != XML_ELEMENT_NODE) {
					xmlUnlinkNode(node);
					xmlFreeNode(node);
				} else if (xmlStrcmp(node->name, BAD_CAST "description") == 0||
				    xmlStrcmp(node->name, BAD_CAST "reference") == 0 ||
				    xmlStrcmp(node->name, BAD_CAST "status") == 0 ||
				    xmlStrcmp(node->name, BAD_CAST "typedef") == 0) {
					xmlUnlinkNode(node);
					xmlFreeNode(node);
				}

				/* process the next node */
				node = node_aux;
			}
		}
	}

	/* cleanup */
	xmlXPathFreeObject(groupings);

	return (EXIT_SUCCESS);
}

static int ncds_update_uses(const char *module_name, const char *prefix,
		                    xmlXPathContextPtr *model_ctxt, const char* query)
{
	xmlXPathObjectPtr uses, groupings = NULL;
	xmlDocPtr doc;
	xmlNodePtr node, nodenext;
	char *grouping_ref, *grouping_name, *aux;
	int i, j, flag = 0;
	size_t prefix_len = strlen(prefix);

	if (model_ctxt == NULL || *model_ctxt == NULL || query == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}
	doc = (*model_ctxt)->doc;

	/*
	 * Get all uses statements from the data model
	 * Do this before import_groupings to get know if there is any uses
	 * clauses. If not, there is no need to import groupings and we can stop
	 * the processing
	 */
	if ((uses = xmlXPathEvalExpression(BAD_CAST query, *model_ctxt)) != NULL ) {
		if (xmlXPathNodeSetIsEmpty(uses->nodesetval)) {
			/* there is no <uses> part so no work is needed */
			xmlXPathFreeObject(uses);
			return (EXIT_SUCCESS);
		}
	} else {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		return (EXIT_FAILURE);
	}

	/* import grouping definitions from imported data models */
	if (import_groupings(module_name, *model_ctxt) != 0) {
		xmlXPathFreeObject(uses);
		return (EXIT_FAILURE);
	}

	if ((groupings = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_YIN_ID":module//"NC_NS_YIN_ID":grouping", *model_ctxt)) == NULL ) {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		return (EXIT_FAILURE);
	}

	/* process all uses statements */
	while (!xmlXPathNodeSetIsEmpty(uses->nodesetval)) {
		for(i = 0; i < uses->nodesetval->nodeNr; i++) {
			grouping_ref = (char*) xmlGetProp(uses->nodesetval->nodeTab[i], BAD_CAST "name");
			flag = 0;
			/*
			 * we can process even references with prefix, because we
			 * already added imported groupings with changing their name
			 * to include prefix. The only thing we have to fix is references
			 * to local groupings with prefix - such groupings are not imported
			 * so the prefix was not added to their name and they can be (and
			 * usually they are) referenced without prefix.
			 */
			if (!strncmp(grouping_ref, prefix, prefix_len) && grouping_ref[prefix_len] == ':') {
				aux = grouping_ref;
				grouping_ref = strdup(&aux[prefix_len + 1]);
				free(aux);
			}

			for (j = 0; j < groupings->nodesetval->nodeNr; j++) {
				grouping_name = (char*) xmlGetProp(groupings->nodesetval->nodeTab[j], BAD_CAST "name");
				if (strcmp(grouping_name, grouping_ref) == 0) {
					/* used grouping found */

					/* copy grouping content instead of uses statement */
					xmlAddChildList(uses->nodesetval->nodeTab[i]->parent, xmlCopyNodeList(groupings->nodesetval->nodeTab[j]->children));

					/* move uses's content next to the grouping content */
					for (node = uses->nodesetval->nodeTab[i]->children; node != NULL; ) {
						nodenext = node->next;
						xmlUnlinkNode(node);
						xmlAddChild(uses->nodesetval->nodeTab[i]->parent, node);
						node = nodenext;
					}

					/* remove uses statement from the tree */
					xmlUnlinkNode(uses->nodesetval->nodeTab[i]);
					xmlFreeNode(uses->nodesetval->nodeTab[i]);
					uses->nodesetval->nodeTab[i] = NULL;

					free(grouping_name);
					flag = 1;
					break;
				}
				free(grouping_name);
			}
			free(grouping_ref);
		}

		if (flag != 0) {
			/*
			 * repeat uses processing - it could be in grouping, so we could
			 * add another not yet processed uses statement
			 */
			xmlXPathFreeObject(uses);

			/* remember to update model context */
			xmlXPathFreeContext(*model_ctxt);
			if ((*model_ctxt = xmlXPathNewContext(doc)) == NULL) {
				ERROR("%s: Creating XPath context failed.", __func__);
				return (EXIT_FAILURE);
			}
			if (xmlXPathRegisterNs(*model_ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
				xmlXPathFreeContext(*model_ctxt);
				return (EXIT_FAILURE);
			}

			if ((uses = xmlXPathEvalExpression(BAD_CAST query, *model_ctxt)) == NULL ) {
				ERROR("%s: Evaluating XPath expression failed.", __func__);
				return (EXIT_FAILURE);
			}
		} else {
			/* no change since last time */
			break;
		}
	}

	/* cleanup */
	xmlXPathFreeObject(groupings);

	if (!xmlXPathNodeSetIsEmpty(uses->nodesetval)) {
		for (i = 0; i < uses->nodesetval->nodeNr; i++) {
			grouping_ref = (char*) xmlGetProp(uses->nodesetval->nodeTab[i], BAD_CAST "name");
			ERROR("Failed to resolve uses \"%s\" in model \"%s\", could not find such grouping in imports.", grouping_ref, module_name);
			free(grouping_ref);
		}
		xmlXPathFreeObject(uses);
		return (EXIT_FAILURE);
	}

	xmlXPathFreeObject(uses);

	return (EXIT_SUCCESS);
}

static int ncds_update_uses_groupings(struct data_model* model)
{
	char* query;

	if (model == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	query = "/"NC_NS_YIN_ID":module//"NC_NS_YIN_ID":grouping//"NC_NS_YIN_ID":uses";
	return(ncds_update_uses(model->name, model->prefix, &(model->ctxt), query));
}

static int ncds_update_uses_augments(struct data_model* model)
{
	char* query;

	if (model == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	query = "//"NC_NS_YIN_ID":augment//"NC_NS_YIN_ID":uses";
	return(ncds_update_uses(model->name, model->prefix, &(model->ctxt), query));
}

static int ncds_update_uses_ds(struct ncds_ds* datastore)
{
	xmlXPathContextPtr model_ctxt;
	char* query;
	int ret;

	if (datastore == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	/*
	 * check that the extended model is already separated from the datastore's
	 * base model
	 */
	if (datastore->ext_model == datastore->data_model->xml) {
		datastore->ext_model = xmlCopyDoc(datastore->data_model->xml, 1);
	}

	/* prepare xpath evaluation context of the model for XPath */
	if ((model_ctxt = xmlXPathNewContext(datastore->ext_model)) == NULL) {
		ERROR("%s: Creating XPath context failed.", __func__);
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(model_ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
		xmlXPathFreeContext(model_ctxt);
		return (EXIT_FAILURE);
	}

	query = "/"NC_NS_YIN_ID":module//"NC_NS_YIN_ID":uses";
	ret = ncds_update_uses(datastore->data_model->name,
			               datastore->data_model->prefix, &model_ctxt, query);
	xmlXPathFreeContext(model_ctxt);

	return (ret);
}

static int ncds_transapi_enlink(struct ncds_ds* ds, struct transapi_internal* tapi)
{
	struct transapi_list *tapi_item, *tapi_iter;

	if (ds == NULL || tapi == NULL) {
		return (EXIT_FAILURE);
	}

	/* search for the transapi module in the internal list */
	for (tapi_iter = augment_tapi_list; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
		if (tapi_iter->tapi == tapi) {
			break;
		}
	}
	if (tapi_iter == NULL) {
		ERROR("%s: Unknown transAPI module. libnetconf internal error.");
		return (EXIT_FAILURE);
	}

	tapi_item = malloc(sizeof(struct transapi_list));
	if (tapi_item == NULL) {
		ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
		return (EXIT_FAILURE);
	}
	tapi_item->next = NULL;
	tapi_item->tapi = tapi;
	/* ref_count in datastore's transAPIs list MUST be always 0 */
	tapi_item->ref_count = 0;

	/* update reference count */
	tapi_iter->ref_count++;

	/* enlink into the datastore's list */
	if (ds->transapis == NULL) {
		ds->transapis = tapi_item;
	} else {
		for (tapi_iter = ds->transapis; tapi_iter->next != NULL; tapi_iter = tapi_iter->next);
		tapi_iter->next = tapi_item;
	}


	return (EXIT_SUCCESS);
}

static xmlNodePtr model_node_path(xmlNodePtr current, const char* current_prefix, const char* current_module_name, char *path, xmlXPathObjectPtr imports, struct ncds_ds** ds)
{
	char *token, *name, *module_inpath = NULL, *module;
	const char *prefix;
	struct ncds_ds_list *ds_iter;
	xmlNodePtr path_node = NULL, node;
	int match, path_type;

	if (path == NULL) {
		return (NULL);
	}

	if (path[0] == '/') {
		path_type = 0; /* absolute */
	} else {
		path_type = 1; /* relative */
	}
	*ds = NULL;

	/* path processing - check that we already have such an element */
	for (token = strtok(path, "/"); token != NULL; token = strtok(NULL, "/")) {
		if ((name = strchr(token, ':')) == NULL) {
			name = token;
			prefix = NULL;
		} else {
			name[0] = 0;
			name = &(name[1]);
			prefix = token;
		}

		if (*ds == NULL) {
			/* locate corresponding datastore - we are at the beginning of the path */
			module = NULL;
			if (prefix == NULL || !strcmp(prefix, current_prefix)) {
				/* model is augmenting itself - get the module's name to be able to find it */
				module = (char*) xmlGetProp(xmlDocGetRootElement(current->doc), BAD_CAST "name");
			} else { /* (prefix != NULL) */
				/* find the prefix in imports */
				module = get_module_with_prefix(prefix, imports);
			}
			if (module == NULL ) {
				/* unknown name of the module to augment */
				return (NULL);
			}

			/* locate the correct datastore to augment */
			for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next) {
				if (ds_iter->datastore != NULL && strcmp(ds_iter->datastore->data_model->name, module) == 0) {
					*ds = ds_iter->datastore;
					break;
				}
			}
			if (*ds == NULL) {
				/* no such a datastore containing model with this path */
				free(module);
				return (NULL);
			}

			if ((*ds)->ext_model == (*ds)->data_model->xml) {
				/* we have the first augment model */
				(*ds)->ext_model = xmlCopyDoc((*ds)->data_model->xml, 1);
			}

			if (path_type == 0) { /* absolute path */
				/* start path parsing with module root */
				path_node = (*ds)->ext_model->children;
			} else {
				/* start relative path parsing with the node's parent */
				path_node = current->parent;

				/* move it if needed according to the start of the relative path */
				if (strcmp("..", name) == 0) {
					path_node = path_node->parent;
				}
			}
			module_inpath = strdup((*ds)->data_model->name);
		} else {
			/* we are somewhere in the path and we need to connect declared prefix with the corresponding module */
			if (prefix == NULL) {
				prefix = current_prefix;
			}

			if (strcmp(prefix, current_prefix) == 0) {
				/* the path is augmenting the self module */
				module = strdup(current_module_name);
			} else {
				/* find the prefix in imports */
				module = get_module_with_prefix(prefix, imports);
			}
			if (module == NULL ) {
				/* unknown name of the module to augment */
				free(module_inpath);
				return (NULL);
			}
		}

		match = 0;
		if (strcmp("..", name) == 0) {
			path_node = path_node->parent;
			match = 1;
		} else if (strcmp(".", name) == 0) {
			/* do nothing */
			match = 1;
		} else {
			/* go into children */
			path_node = path_node->children;
			if (module_inpath != NULL && strcmp(module, module_inpath) != 0) {
				/* the prefix is changing, so there must be an augment element */
				for (node = path_node; node != NULL && match == 0; node = node->next) {
					if (xmlStrcmp(node->name, BAD_CAST "augment") != 0) {
						continue;
					}
					/* some augment found, now check it */
					free(module_inpath);
					module_inpath = (char*) xmlGetNsProp(node, BAD_CAST "module", BAD_CAST "libnetconf");

					path_node = node->children;
					match = match_module_node(module_inpath, module, name, &path_node);
				}
			} else if (module_inpath != NULL && strcmp(module, module_inpath) == 0) {
				/* we have the match - move into the specified element */
				match = match_module_node(module_inpath, module, name, &path_node);
			}
		}
		free(module);

		if (match == 0) {
			/* we didn't find the matching path */
			free(module_inpath);
			return (NULL);
		}
	}

	free(module_inpath);

	return (path_node);
}

/**
 * supported types:
 *  'augment' 1
 *  'refine'  2
 *
 * supported path_types:
 *  'absolute' 0
 *  'relative' 1
 *  'both'
 *
 * return:
 *  error       -1
 *  no changes  0
 *  some change 1
 */
static int _update_model(int type, xmlXPathContextPtr model_ctxt, const char* model_prefix, const char* model_name, const char* model_ns, struct transapi_internal* aug_transapi, int path_type)
{
	xmlXPathObjectPtr imports = NULL, nodes = NULL;
	xmlNodePtr node, node_aux, path_node;
	xmlNsPtr ns;
	int i, ret = 0;
	char *path, *resolved_path, *resolved_ns, *to_resolve_path;
	struct ncds_ds* ds = NULL;

	/* get all definitions of nodes to modify */
	switch (type) {
	case 1: /* augment */
		nodes = xmlXPathEvalExpression(BAD_CAST "//"NC_NS_YIN_ID":augment", model_ctxt);
		break;
	case 2: /* refine */
		nodes = xmlXPathEvalExpression(BAD_CAST "//"NC_NS_YIN_ID":refine", model_ctxt);
		break;
	default: /* wtf */
		return (-1);
	}

	if (nodes != NULL ) {
		if (xmlXPathNodeSetIsEmpty(nodes->nodesetval)) {
			/* there is no element modifying the model so we have nothing to do */
			xmlXPathFreeObject(nodes);
			return (0);
		}
	} else {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		goto error_cleanup;
	}

	/* get all <import> nodes for their prefix specification to be used with augment statement */
	if ((imports = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_YIN_ID":module/"NC_NS_YIN_ID":import", model_ctxt)) == NULL ) {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		return (-1);
	}

	/* process all modifying nodes from this model */
	for (i = 0; i < nodes->nodesetval->nodeNr; i++) {

		/* get path to the target element */
		if ((path = (char*) xmlGetProp (nodes->nodesetval->nodeTab[i], BAD_CAST "target-node")) == NULL) {
			ERROR("%s: Missing 'target-node' attribute in <augment>.", __func__);
			goto error_cleanup;
		}

		/* according to set option, skip relative/absolute paths */
		if ((path[0] == '/' && path_type == 1) || /* we are processing only absolute paths */
			(path[0] != '/' && path_type == 0)) {
			free(path);
			continue;
		}

		to_resolve_path = strdup(path);
		path_node = model_node_path(nodes->nodesetval->nodeTab[i], model_prefix, model_name, path, imports, &ds);
		free(path);
		if (path_node != NULL) {
			/* path is correct, process the requested modification */
			switch (type) {
			case 1: /* augment */

				//we must add the attributes to the candidate node before comparing
				node = xmlCopyNode(nodes->nodesetval->nodeTab[i], 1);
				ns = xmlNewNs(node, BAD_CAST "libnetconf", BAD_CAST "libnetconf");
				xmlSetNsProp(node, ns, BAD_CAST "module", BAD_CAST model_name);
				xmlSetNsProp(node, ns, BAD_CAST "ns", BAD_CAST model_ns);

				/* check whether this augment was not already resolved */
				for (node_aux = path_node->children; node_aux != NULL; node_aux = node_aux->next) {
					if (xmlStrcmp(node_aux->name, BAD_CAST "augment") == 0) {
						resolved_path = (char*) xmlGetProp (node_aux, BAD_CAST "target-node");
						resolved_ns = (char*) xmlGetNsProp (node_aux, BAD_CAST "ns", BAD_CAST "libnetconf");
						if (strcmp(to_resolve_path, resolved_path) == 0 && strcmp(model_ns, resolved_ns) == 0) {
							xmlBufferPtr content1 = xmlBufferCreate(), content2 = xmlBufferCreate();

							//The same module can have multiple augment statetements of the same node.
							//The only way to check if it has already been processed is to check the contents.
							if (content1 && content2)
							{
								xmlNodeDump(content1, node_aux->doc, node_aux, 0, 0);
								xmlNodeDump(content2, node->doc,	 node, 0, 0);
								if (xmlStrcmp(xmlBufferContent(content1), xmlBufferContent(content2)) == 0) {
									xmlBufferFree(content1);
									xmlBufferFree(content2);
									free(resolved_path);
									free(resolved_ns);
									break;
								}
							}

							xmlBufferFree(content1);
							xmlBufferFree(content2);
						}
						free(resolved_path);
						free(resolved_ns);
					}
				}

				if (node_aux != NULL) {
					/* already resolved, discard the copy node */
					xmlUnlinkNode(node);
					xmlFreeNode(node);
					break;
				}

				xmlAddChild(path_node, node);

				/*
				 * if the model is connected with the transAPI module, add it to the
				 * list of transAPI modules of the datastore
				 */
				if (path_type == 0 && aug_transapi != NULL) {
					ncds_transapi_enlink(ds, aug_transapi);
				}

				/* remember the change */
				ret = 1;
				break;
			case 2: /* refine */
				for (node = nodes->nodesetval->nodeTab[i]->children; node != NULL; node = node->next) {
					if (xmlStrcmp(node->name, BAD_CAST "must") == 0) {
						/* always add to the target_node */
						xmlAddChild(path_node, xmlCopyNode(node, 1));
					} else {
						/* detect if the node exists, then replace it, add it otherwise */
						for (node_aux = path_node->children; node_aux != NULL; node_aux = node_aux->next) {
							if (node_aux->type != XML_ELEMENT_NODE) {
								continue;
							}
							if (xmlStrcmp(node_aux->name, node->name) == 0) {
								xmlUnlinkNode(node_aux);
								xmlFreeNode(node_aux);
								break;
							}
						}
						xmlAddChild(path_node, xmlCopyNode(node, 1));
					}
				}
				/* remove refine definition */
				xmlUnlinkNode(nodes->nodesetval->nodeTab[i]);
				xmlFreeNode(nodes->nodesetval->nodeTab[i]);
				nodes->nodesetval->nodeTab[i] = NULL;

				/* remember the change */
				ret = 1;
				break;
			default: /* wtf */
				return (-1);
			}
		}
		free(to_resolve_path);
	}
	xmlXPathFreeObject(nodes);
	xmlXPathFreeObject(imports);

	return (ret);

error_cleanup:

	xmlXPathFreeObject(imports);
	xmlXPathFreeObject(nodes);

	return (-1);
}

static int ncds_update_refine(struct ncds_ds *ds)
{
	int ret;
	xmlXPathContextPtr ext_model_ctxt;

	if (ds == NULL) {
		ERROR("%s: invalid parameter ds.", __func__);
		return (EXIT_FAILURE);
	}

	if ((ext_model_ctxt = xmlXPathNewContext(ds->ext_model)) == NULL) {
		ERROR("%s: Creating XPath context failed.", __func__);
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(ext_model_ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
		xmlXPathFreeContext(ext_model_ctxt);
		return (EXIT_FAILURE);
	}

	ret = _update_model(2, ext_model_ctxt, ds->data_model->prefix, ds->data_model->name, ds->data_model->ns, NULL, 2);

	xmlXPathFreeContext(ext_model_ctxt);
	return (ret);
}

static int ncds_update_augment_absolute(struct data_model *augment)
{
	if (augment == NULL) {
		ERROR("%s: invalid parameter augment.", __func__);
		return (EXIT_FAILURE);
	}

	return(_update_model(1, augment->ctxt, augment->prefix, augment->name, augment->ns, augment->transapi, 0));
}

static int ncds_update_augment_relative(struct ncds_ds *ds)
{
	int ret;
	xmlXPathContextPtr ext_model_ctxt;

	if (ds == NULL) {
		ERROR("%s: invalid parameter ds.", __func__);
		return (EXIT_FAILURE);
	}

	if ((ext_model_ctxt = xmlXPathNewContext(ds->ext_model)) == NULL) {
		ERROR("%s: Creating XPath context failed.", __func__);
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(ext_model_ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
		xmlXPathFreeContext(ext_model_ctxt);
		return (EXIT_FAILURE);
	}

	ret = _update_model(1, ext_model_ctxt, ds->data_model->prefix, ds->data_model->name, ds->data_model->ns, NULL, 1);

	xmlXPathFreeContext(ext_model_ctxt);
	return (ret);
}

static int ncds_update_augment_cleanup(struct ncds_ds *ds)
{
	int i;
	xmlXPathContextPtr ext_model_ctxt;
	xmlXPathObjectPtr augments;

	if (ds == NULL) {
		ERROR("%s: invalid parameter ds.", __func__);
		return (EXIT_FAILURE);
	}

	if ((ext_model_ctxt = xmlXPathNewContext(ds->ext_model)) == NULL) {
		ERROR("%s: Creating XPath context failed.", __func__);
		return (EXIT_FAILURE);
	}
	if (xmlXPathRegisterNs(ext_model_ctxt, BAD_CAST NC_NS_YIN_ID, BAD_CAST NC_NS_YIN) != 0) {
		xmlXPathFreeContext(ext_model_ctxt);
		return (EXIT_FAILURE);
	}

	/* get all augment definitions */
	if ((augments = xmlXPathEvalExpression(BAD_CAST "//"NC_NS_YIN_ID":augment", ext_model_ctxt)) != NULL ) {
		if (xmlXPathNodeSetIsEmpty(augments->nodesetval)) {
			/* there is no <augment> part so we have nothing to do */
			xmlXPathFreeObject(augments);
			xmlXPathFreeContext(ext_model_ctxt);
			return (EXIT_SUCCESS);
		}
	} else {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		xmlXPathFreeContext(ext_model_ctxt);
		return (EXIT_FAILURE);
	}

	for (i = 0; i < augments->nodesetval->nodeNr; i++) {
		if (xmlHasNsProp(augments->nodesetval->nodeTab[i], BAD_CAST "module", BAD_CAST "libnetconf") != NULL) {
			/* substituted augment, do not remove it */
			continue;
		} else {
			/* no more needed augment definition, remove it */
			xmlUnlinkNode(augments->nodesetval->nodeTab[i]);
			xmlFreeNode(augments->nodesetval->nodeTab[i]);
			augments->nodesetval->nodeTab[i] = NULL;
		}
	}

	xmlXPathFreeObject(augments);
	xmlXPathFreeContext(ext_model_ctxt);

	return (EXIT_SUCCESS);
}

API int ncds_add_models_path(const char* path)
{
	static int list_size = 0;
	static int list_records = 0;

	if (models_dirs == NULL) {
		/* the list was cleaned */
		list_size = 0;
		list_records = 0;
	}

	if (path == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	if (access(path, R_OK | X_OK) != 0) {
		ERROR("Configuration data models directory \'%s\' is not accessible (%s).", path, strerror(errno));
		return (EXIT_FAILURE);
	}

	if (list_records + 1 >= list_size) {
		void *tmp = realloc(models_dirs, (list_size + 5) * sizeof(char*));
		if (tmp == NULL) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			return (EXIT_FAILURE);
		}

		models_dirs = tmp;
		list_size += 5;
	}

	models_dirs[list_records] = strdup(path);
	if (models_dirs[list_records] == NULL) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (EXIT_FAILURE);
	}

	list_records++;
	models_dirs[list_records] = NULL; /* terminating NULL byte */

	return (EXIT_SUCCESS);
}

API int ncds_add_augment_transapi(const char* model_path, const char* callbacks_path)
{
	struct data_model *model;
	struct transapi_internal* transapi;
	struct transapi_list *tapi_item;

	if (model_path == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	/* get model */
	if ((model = read_model(model_path)) == NULL) {
		return (EXIT_FAILURE);
	}

	/* load transapi module */
	if (model->transapi == NULL) {
		tapi_item = malloc(sizeof(struct transapi_list));
		if (tapi_item == NULL) {
			ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}

		transapi = transapi_new_shared(callbacks_path);
		if (transapi == NULL) {
			ncds_ds_model_free(model);
			free(tapi_item);
			return (EXIT_FAILURE);
		}

		/* link model with transapi module (and vice versa) */
		transapi->model = model;
		model->transapi = transapi;

		/* add transapi module into internal list of loaded modules */
		tapi_item->tapi = transapi;
		tapi_item->ref_count = 0;
		tapi_item->next = augment_tapi_list;
		augment_tapi_list = tapi_item;
	}

	return (EXIT_SUCCESS);
}

API int ncds_add_augment_transapi_static(const char* model_path, const struct transapi* transapi)
{
	struct data_model *model;
	struct transapi_list *tapi_item;

	if (model_path == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	/* get model */
	if ((model = read_model(model_path)) == NULL) {
		return (EXIT_FAILURE);
	}

	if (model->transapi == NULL) {
		/* transAPI module information checks */
		if (transapi == NULL) {
			ERROR("%s: Missing transAPI module description.", __func__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}
		if (transapi->config_modified == NULL) {
			ERROR("%s: Missing config_modified variable in transAPI module description.", __func__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}
		if (transapi->erropt == NULL) {
			ERROR("%s: Missing erropt variable in transAPI module description.", __func__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}
		if (transapi->get_state == NULL) {
			ERROR("%s: Missing get_state() function in transAPI module description.", __func__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}
		if (transapi->ns_mapping == NULL) {
			ERROR("%s: Missing mapping of prefixes with URIs in transAPI module description.", __func__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}
		/* ok, checks passed */

		tapi_item = malloc(sizeof(struct transapi_list));
		if (tapi_item == NULL) {
			ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}

		/* allocate transapi structure */
		if ((model->transapi = malloc(sizeof(struct transapi_internal))) == NULL) {
			ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
			ncds_ds_model_free(model);
			return (EXIT_FAILURE);
		}
		/* fill created transapi structure with given pointers to transAPI functions */

		/* copy transAPI module info into a internal structure
		 * NOTE: copy only the beginning part common for struct transapi and
		 * struct transapi_internal
		 */
		memcpy(model->transapi, transapi, (sizeof(struct transapi)));
		/*
		 * mark it as transAPI (non-NULL), but remember that it is not a dynamically
		 * linked transAPI module
		 */
		model->transapi->module = &error_area;

		/* link created transapi with the model */
		model->transapi->model = model;

		/* add transapi module into internal list of loaded modules */
		tapi_item->tapi = model->transapi;
		tapi_item->ref_count = 0;
		tapi_item->next = augment_tapi_list;
		augment_tapi_list = tapi_item;
	}

	return (EXIT_SUCCESS);
}

API int ncds_add_model(const char* model_path)
{
	if (model_path == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	if (read_model(model_path) == NULL) {
		return (EXIT_FAILURE);
	} else {
		return (EXIT_SUCCESS);
	}
}

static int ncds_features_parse(struct data_model* model)
{
	xmlXPathObjectPtr features = NULL;
	int i;

	if (model == NULL || model->ctxt == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	/* get all top-level's augment definitions */
	if ((features = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_YIN_ID":module/"NC_NS_YIN_ID":feature", model->ctxt)) != NULL ) {
		if (xmlXPathNodeSetIsEmpty(features->nodesetval)) {
			/* there is no <feature> part so feature list will be empty */
			model->features = NULL;

			VERB("%s: no feature definitions found in data model %s.", __func__, model->name);
			xmlXPathFreeObject(features);
			return (EXIT_SUCCESS);
		}

		model->features = malloc((features->nodesetval->nodeNr + 1) * sizeof(struct model_feature*));
		if (model->features == NULL) {
			ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
			xmlXPathFreeObject(features);
			return (EXIT_FAILURE);
		}
		for(i = 0; i < features->nodesetval->nodeNr; i++) {
			model->features[i] = malloc(sizeof(struct model_feature));
			if (model->features[i] == NULL) {
				ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
				xmlXPathFreeObject(features);
				return (EXIT_FAILURE);
			}
			if ((model->features[i]->name = (char*) xmlGetProp(features->nodesetval->nodeTab[i], BAD_CAST "name")) == NULL ) {
				ERROR("xmlGetProp failed (%s:%d).", __FILE__, __LINE__);
				free(model->features[i]);
				model->features = NULL;
				xmlXPathFreeObject(features);
				return (EXIT_FAILURE);
			}
			/* by default, all features are disabled */
			model->features[i]->enabled = 0;
		}
		model->features[i] = NULL; /* list terminating NULL byte */

		xmlXPathFreeObject(features);
	} else {
		ERROR("%s: Evaluating XPath expression failed.", __func__);
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

API int ncds_feature_isenabled(const char* module, const char* feature)
{
	struct data_model* model;
	int i;

	if (module == NULL || feature == NULL) {
		ERROR("%s: invalid parameter %s", __func__, (module==NULL)?"module":"feature");
		return (-1);
	}

	if ((model = get_model(module, NULL)) == NULL) {
		return (-1);
	}

	if (model->features != NULL) {
		for (i = 0; model->features[i] != NULL; i++) {
			if (strcmp(model->features[i]->name, feature) == 0) {
				return(model->features[i]->enabled);
			}
		}
	}
	return (-1);
}

static inline int _feature_switch(const char* module, const char* feature, int value)
{
	struct data_model* model;
	int i;

	if (module == NULL || feature == NULL) {
		ERROR("%s: invalid parameter %s", __func__, (module==NULL)?"module":"feature");
		return (EXIT_FAILURE);
	}

	if ((model = get_model(module, NULL)) == NULL) {
		return (EXIT_FAILURE);
	}

	if (model->features != NULL) {
		for (i = 0; model->features[i] != NULL; i++) {
			if (strcmp(model->features[i]->name, feature) == 0) {
				model->features[i]->enabled = value;
				return (EXIT_SUCCESS);
			}
		}
	}

	return (EXIT_FAILURE);
}

API int ncds_feature_enable(const char* module, const char* feature)
{
	return (_feature_switch(module, feature, 1));
}

API int ncds_feature_disable(const char* module, const char* feature)
{
	return (_feature_switch(module, feature, 0));
}

static inline int _features_switchall(const char* module, int value)
{
	struct data_model* model;
	int i;

	if (module == NULL) {
		ERROR("%s: invalid parameter", __func__);
		return (EXIT_FAILURE);
	}

	if ((model = get_model(module, NULL)) == NULL) {
		return (EXIT_FAILURE);
	}

	if (model->features != NULL) {
		for (i = 0; model->features[i] != NULL; i++) {
			model->features[i]->enabled = value;
		}
	}

	return (EXIT_SUCCESS);
}

API int ncds_features_enableall(const char* module)
{
	return (_features_switchall(module, 1));
}

API int ncds_features_disableall(const char* module)
{
	return (_features_switchall(module, 1));
}

/**
 * @brief Replace substr in str by replacement
 *
 * Remember to free the returned string. Even if no replacement is done, new
 * (copy of the original) string is returned.
 *
 * @param[in] str String to modify.
 * @param[in] substr Substring to be replaced.
 * @param[in] replacement Replacement for the substr.
 *
 * return Resulting string or NULL on error.
 */
static char* nc_str_replace(const char *str, const char *substr, const char *replacement)
{
	int i, j, len;
	const char *aux;
	char *ret;

	if ((len = strlen(replacement) - strlen(substr) ) > 0) {
		/* we are going to enlarge the string - get to know how much */
		for (i = 0, aux = strstr(str, substr); aux != NULL; aux = strstr(aux, substr)) {
			i++;
			aux = &(aux[strlen(substr)]);
		}
		if (i == 0) {
			/* there is no occurrence of the needle, return just a copy of str */
			return (strdup(str));
		}

		/* length of original string +
		 * (# of needle occurrence * difference between needle and replacement) +
		 * terminating NULL byte
		 */
		ret = malloc((strlen(str) + (i * len) + 1) * sizeof(char));
	} else {
		/* it's not going to be longer than original string */
		ret = malloc((strlen(str) + 1) * sizeof(char));
	}
	if (ret == NULL) {
		return (NULL);
	}

	for (i = j = 0, aux = strstr(str, substr); aux != NULL; aux = strstr(aux, substr)) {
		while (&(str[i]) != aux) {
			ret[j] = str[i];
			i++;
			j++;
		}
		strcpy(&(ret[j]), replacement);
		j += strlen(replacement);
		i += strlen(substr);
		aux = &(str[i]);
	}
	/* copy the rest of the string */
	strcpy(&(ret[j]), &(str[i]));

	return(ret);
}

static int ncds_update_callbacks(struct ncds_ds* ds)
{
	struct transapi_list *tapi_iter;
	int i, j, k, l, clbk_count, len;
	char *path, *path_aux;
#define PREFIX_BUFFER_SIZE 128
	char buffer[PREFIX_BUFFER_SIZE];

#define MAPPING_SIZE 26+1 /* # of letters used as prefixes + terminating item */
	char ext_ns_mapping_buffer[4] = {'/', ' ',':','\0'};
	struct ns_pair ext_ns_mapping[MAPPING_SIZE] = {
			{"A",NULL},{"B",NULL},{"C",NULL},{"D",NULL},{"E",NULL},{"F",NULL},
			{"G",NULL},{"H",NULL},{"I",NULL},{"J",NULL},{"K",NULL},{"L",NULL},{"M",NULL},
			{"N",NULL},{"O",NULL},{"P",NULL},{"Q",NULL},{"R",NULL},{"S",NULL},{"T",NULL},
			{"U",NULL},{"V",NULL},{"W",NULL},{"X",NULL},{"Y",NULL},{"Z",NULL},{NULL,NULL},};

	i = 0;
	clbk_count = 0;
	/* compound namespace mappings from all transapi modules connected with this datastore */
	for (tapi_iter = ds->transapis; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
		for (j = 0; tapi_iter->tapi->ns_mapping[j].href != NULL; j++) {
			/* check that we don't have already such a namespace mapping */
			for (k = 0; k < i; k++) {
				if (strcmp(ext_ns_mapping[k].href, tapi_iter->tapi->ns_mapping[j].href) == 0) {
					break;
				}
			}

			/* allocate array for namespace mapping */
			if (i >= MAPPING_SIZE) {
				ERROR("Too many namespaces to process. Limit is %d.", MAPPING_SIZE);
				return (EXIT_FAILURE);
			}
			ext_ns_mapping[i].href = tapi_iter->tapi->ns_mapping[j].href;
			i++;
		}

		/* as a side effect of the loop, count the callbacks */
		clbk_count += tapi_iter->tapi->data_clbks->callbacks_count;
	}
	/* terminating item */
	ext_ns_mapping[i].prefix = NULL;
	ext_ns_mapping[i].href = NULL;

	if (ds->tapi_callbacks_count != 0) {
		for (i = 0; i < ds->tapi_callbacks_count; i++) {
			free(ds->tapi_callbacks[i].path);
		}
		free(ds->tapi_callbacks);
		ds->tapi_callbacks = NULL;
	}
	/* create list of callbacks */
	ds->tapi_callbacks_count = clbk_count;
	if (clbk_count > 0) {
		ds->tapi_callbacks = malloc(clbk_count * sizeof(struct clbk));
	} else {
		ds->tapi_callbacks = NULL;
	}
	for (i = 0, tapi_iter = ds->transapis; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
		for (j = 0; j < tapi_iter->tapi->data_clbks->callbacks_count; j++) {
			ds->tapi_callbacks[i].func = tapi_iter->tapi->data_clbks->callbacks[j].func;
			/* correct prefixes in path */
			path = strdup(tapi_iter->tapi->data_clbks->callbacks[j].path);
			for (k = 0; tapi_iter->tapi->ns_mapping[k].href != NULL; k++) {
				/* search for remapped prefix of the namespace */
				for (l = 0; ext_ns_mapping[l].href != NULL; l++) {
					if (strcmp(ext_ns_mapping[l].href, tapi_iter->tapi->ns_mapping[k].href) == 0) {
						break;
					}
				}
				if (ext_ns_mapping[l].href == NULL) {
					ERROR("Processing unknown namespace, internal error.");
					free(path);
					return (EXIT_FAILURE);
				}

				path_aux = path;
				len = strlen(tapi_iter->tapi->ns_mapping[k].prefix);
				if (len+1 >= PREFIX_BUFFER_SIZE) {
					ERROR("Namespace prefix \'%s\' is too long. libnetconf is able to process prefixes up to %d characters.",
							tapi_iter->tapi->ns_mapping[k].prefix, PREFIX_BUFFER_SIZE - 1);
					free(path);
					return (EXIT_FAILURE);
				}
				/* magic */
				buffer[0] = '/';
				strcpy(buffer+1, tapi_iter->tapi->ns_mapping[k].prefix);
				buffer[len+1] = ':';
				buffer[len+2] = '\0';
				ext_ns_mapping_buffer[1] = ext_ns_mapping[l].prefix[0];
				path = nc_str_replace(path_aux, buffer, ext_ns_mapping_buffer);
				free(path_aux);
			}
			ds->tapi_callbacks[i].path = path;
			i++;
		}
	}

	/* rewrite previously created model */
	yinmodel_free(ds->ext_model_tree);

	/* parse model, if there are no callbacks, we don't need the model parsed and it would fail anyway */
	if (ds->tapi_callbacks_count == 0) {
		VERB("transAPI module for the model \"%s\" does not have any callbacks.", ds->data_model->name);
	} else if ((ds->ext_model_tree = yinmodel_parse(ds->ext_model, ext_ns_mapping)) == NULL) {
		WARN("Failed to parse the model \"%s\". Callbacks of transAPI modules using this model will not be executed.", ds->data_model->name);
	}

	return (EXIT_SUCCESS);
}

static void transapi_unload(struct transapi_internal* tapi)
{
	/* stop the thread monitoring the files */
	if (tapi->file_clbks != NULL && tapi->file_clbks->callbacks_count > 0 && tapi->fmon_thread != 0) {
		VERB("Stopping FMON thread.");
        pthread_cancel(tapi->fmon_thread);
        usleep(50000);
		/* just hope the thread was canceled */
	}
	/* if close function is defined */
	if (tapi->close != NULL) {
		tapi->close();
	}
	if (tapi->module != &error_area && dlclose(tapi->module)) {
		ERROR("%s: Unloading transAPI module failed: %s:", __func__, dlerror());
	}
}

static void transapis_cleanup(struct transapi_list **list, int force)
{
	struct transapi_list *iter, *prev = NULL;

	if (list == NULL || *list == NULL) {
		return;
	}

	for (iter = *list; iter != NULL; ) {
		if (force || iter->ref_count == 0) {
			transapi_unload(iter->tapi);
			free(iter->tapi);
			if (prev == NULL) {
				*list = iter->next;
				free(iter);
				iter = *list;
			} else {
				prev->next = iter->next;
				free(iter);
				iter = prev->next;
			}
		} else {
			prev = iter;
			iter = iter->next;
		}
	}
}

API int ncds_consolidate(void)
{
	int ret, changes;
	struct ncds_ds_list *ds_iter;
	struct model_list* listitem;
	struct transapi_list *tapi_iter;

	/* cleanup all datastore's properties built by previous ncds_consolidate() */
	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next) {
		transapis_cleanup(&(ds_iter->datastore->transapis), 0);

		if (ds_iter->datastore->ext_model != ds_iter->datastore->data_model->xml) {
			xmlFreeDoc(ds_iter->datastore->ext_model);
			ds_iter->datastore->ext_model = ds_iter->datastore->data_model->xml;
		}

		yinmodel_free(ds_iter->datastore->ext_model_tree);
		ds_iter->datastore->ext_model_tree = NULL;
	}
	/* set ref_count of all transAPIs to 0 to recount it in ncds_update_augment() */
	for (tapi_iter = augment_tapi_list; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
		tapi_iter->ref_count = 0;
	}

	ncds_update_features();

	/* process uses statements in the configuration datastores */
	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next) {
		if (ds_iter->datastore != NULL && ncds_update_uses_ds(ds_iter->datastore) != EXIT_SUCCESS) {
			ERROR("Preparing configuration data models failed.");
			return (EXIT_FAILURE);
		}
	}

	/* augment statement processing - absolute paths to modify other (datastore's extended models) data models */
	do {
		ret = 0;
		changes = 0;
		for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
			if (listitem->model != NULL && (ret = ncds_update_augment_absolute(listitem->model)) == -1) {
				ERROR("Augmenting configuration data models failed.");
				return (EXIT_FAILURE);
			}

			if (ret == 1) {
				changes = 1;
			}
		}
	} while (changes);

	/* augment statement processing - relative paths to modify always the data model (datastore's extended model) itself */
	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next) {
		if (ds_iter->datastore->ext_model != NULL && ncds_update_augment_relative(ds_iter->datastore) == -1) {
			ERROR("Augmenting configuration data models failed.");
			return (EXIT_FAILURE);
		}
		/* clean datastore's extended models from augment definitions */
		ncds_update_augment_cleanup(ds_iter->datastore);

		/* resolve refines */
		ncds_update_refine(ds_iter->datastore);
	}

	/* parse models to get aux structure for TransAPI's internal purposes */
	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next) {
		/* when using transapi */
		if (ds_iter->datastore->transapis != NULL) {
			if (ncds_update_callbacks(ds_iter->datastore) != EXIT_SUCCESS) {
				ERROR("Preparing transAPI failed.");
				return (EXIT_FAILURE);
			}
		}
	}

	transapis_cleanup(&(augment_tapi_list), 0);
	return (EXIT_SUCCESS);
}

/**
 * @brief Check if the given root element is part of the given data model.
 *
 * Currently, the checking is based only on namespace!
 *
 * @param[in] root The node to be checked.
 * @param[in] data_model The data model structure to check if the root belongs to it
 * @return 1 if root belongs to the data model namespace, 0 if not
 */
static int is_model_root(xmlNodePtr root, struct data_model *data_model)
{
	assert(root);
	assert(data_model);

	if (root->type != XML_ELEMENT_NODE) {
		return 0;
	}

	if (data_model->ns == NULL) {
		ERROR("Invalid configuration data model '%s'- namespace is missing.", data_model->name);
		return 0;
	}

	if (root->ns == NULL || xmlStrcmp(root->ns->href, BAD_CAST (data_model->ns)) != 0) {
		return 0;
	} else {
		return 1;
	}
}

static xmlDocPtr read_datastore_data(ncds_id id, const char *data)
{
	char *config = NULL;
	const char *datap = data;
	xmlDocPtr doc, ret = NULL;
	xmlNodePtr node;

	if (data == NULL || strcmp(data, "") == 0) {
		/* config is empty */
		return xmlNewDoc (BAD_CAST "1.0");
	} else {
		if (strncmp(data, "<?xml", 5) == 0) {
			/* We got a "real" XML document. We strip off the
			 * declaration, so the thing below works.
			 *
			 * We just skip it and use the rest of the xml. Data are untouched.
			 */
			datap = index(data, '>');
			if (datap == NULL) {
				/* content is corrupted */
				ERROR("Invalid datastore configuration data (datastore %d).", id);
				return (NULL);
			}
			++datap;
		}

		if (asprintf(&config, "<config>%s</config>", datap) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			return (NULL);
		}
		doc = xmlReadDoc(BAD_CAST config, NULL, NULL, NC_XMLREAD_OPTIONS);
		free(config);

		if (doc == NULL || doc->children == NULL) {
			xmlFreeDoc(doc);
			ERROR("Invalid datastore configuration data (datastore %d).", id);
			return (NULL);
		}

		for (node = doc->children->children; node != NULL; node = node->next) {
			if (node->type != XML_ELEMENT_NODE) {
				continue;
			}

			if (ret) {
				xmlAddNextSibling(ret->last, xmlCopyNode(node, 1));
			} else {
				ret = xmlNewDoc(BAD_CAST "1.0");
				xmlDocSetRootElement(ret, xmlCopyNode(node, 1));
			}
		}
		xmlFreeDoc(doc);
		return ret;
	}
}

#ifndef DISABLE_VALIDATION
static void relaxng_error_callback(void *error, const char * msg, ...)
{
	struct nc_err **e = (struct nc_err**)(error);
	struct nc_err *err_aux;
	va_list args;
	char* s = NULL, *m = NULL;
	int r;

	if (e != NULL) {
		va_start(args, msg);
		r = vasprintf(&s, msg, args);
		va_end(args);

		if (r == -1) {
			ERROR("vasprintf() failed (%s:%d).", __FILE__, __LINE__);
			return;
		}

		err_aux = nc_err_new(NC_ERR_OP_FAILED);
		if (*e != NULL) {
			err_aux->next = *e;
		}
		*e = err_aux;

		if (s[strlen(s)-1] == '\n') {
			s[strlen(s)-1] = '\0';
		}
		if (asprintf(&m, "Datastore fails to validate (%s)", s) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			m = NULL;

			nc_err_set(*e, NC_ERR_PARAM_MSG, "Datastore fails to validate");
			err_aux = nc_err_new(NC_ERR_OP_FAILED);
			nc_err_set(err_aux, NC_ERR_PARAM_MSG, s);
			err_aux->next = (*e)->next;
			(*e)->next = err_aux;
		} else {
			nc_err_set(*e, NC_ERR_PARAM_MSG, m);
		}

		free(s);
		free(m);
	}
}

/*
 * EXIT_SUCCESS - validation ok
 * EXIT_FAILURE - validation failed
 * EXIT_RPC_NOT_APPLICABLE - RelaxNG scheme not defined
 */
static int validate_ds(struct ncds_ds *ds, xmlDocPtr doc, struct nc_err **error)
{
	int ret = 0;
	int retval = EXIT_RPC_NOT_APPLICABLE;
	xmlDocPtr sch_result;
	xmlXPathContextPtr ctxt = NULL;
	xmlXPathObjectPtr result = NULL;
	char* schematron_error = NULL, *error_string = NULL;
	struct nc_err *err_aux;
	int i;

	assert(error != NULL);

	if (ds == NULL || doc == NULL) {
		ERROR("%s: invalid input parameter", __func__);
		return (EXIT_FAILURE);
	}

	if (ds->validators.rng) {
		/* RelaxNG validation */
		DBG("RelaxNG validation on subdatastore %d", ds->id);

		xmlRelaxNGSetValidErrors(ds->validators.rng,
			(xmlRelaxNGValidityErrorFunc) relaxng_error_callback,
			(xmlRelaxNGValidityWarningFunc) relaxng_error_callback,
			error);

		ret = xmlRelaxNGValidateDoc(ds->validators.rng, doc);
		if (ret > 0) {
			VERB("subdatastore %d fails to validate", ds->id);
			if (*error == NULL) {
				*error = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(*error, NC_ERR_PARAM_MSG, "Datastore fails to validate.");
			}
			return (EXIT_FAILURE);
		} else if (ret < 0) {
			ERROR("validation generated an internal error", ds->id);
			if (*error == NULL) {
				*error = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(*error, NC_ERR_PARAM_MSG, "Validation generated an internal error.");
			}
			return (EXIT_FAILURE);
		} else { /* ret == 0 -> success */
			retval = EXIT_SUCCESS;
		}
	}

	if (ds->validators.schematron) {
		/* schematron */
		DBG("Schematron validation on subdatastore %d", ds->id);

		sch_result = xsltApplyStylesheet(ds->validators.schematron, doc, NULL);
		if (sch_result == NULL) {
			ERROR("Applying Schematron stylesheet on subdatastore %d failed", ds->id);
			*error = nc_err_new(NC_ERR_OP_FAILED);
			nc_err_set(*error, NC_ERR_PARAM_MSG, "Schematron validation internal error.");
			return (EXIT_FAILURE);
		}

		/* parse SVRL result */
		if ((ctxt = xmlXPathNewContext(sch_result)) == NULL) {
			ERROR("%s: Creating the XPath context failed.", __func__);
			xmlFreeDoc(sch_result);
			*error = nc_err_new(NC_ERR_OP_FAILED);
			return (EXIT_FAILURE);
		}
		if (xmlXPathRegisterNs(ctxt, BAD_CAST "svrl", BAD_CAST "http://purl.oclc.org/dsdl/svrl") != 0) {
			ERROR("Registering SVRL namespace for the xpath context failed.");
			xmlXPathFreeContext(ctxt);
			xmlFreeDoc(sch_result);
			*error = nc_err_new(NC_ERR_OP_FAILED);
			return (EXIT_FAILURE);
		}
		if ((result = xmlXPathEvalExpression(BAD_CAST "/svrl:schematron-output/svrl:failed-assert/svrl:text | /svrl:schematron-output/svrl:successful-report/svrl:text", ctxt)) != NULL) {
			if (!xmlXPathNodeSetIsEmpty(result->nodesetval)) {
				for (i = 0; i < result->nodesetval->nodeNr; i++) {
					schematron_error = (char*)xmlNodeGetContent(result->nodesetval->nodeTab[i]);
					ERROR("Datastore fails to validate: %s", schematron_error);
					err_aux = nc_err_new(NC_ERR_OP_FAILED);
					if (asprintf(&error_string, "Datastore fails to validate: %s", schematron_error) == -1) {
						nc_err_set(err_aux, NC_ERR_PARAM_MSG, "Datastore fails to validate");
					} else {
						nc_err_set(err_aux, NC_ERR_PARAM_MSG, error_string);
						free(error_string);
					}

					if (*error != NULL) {
						err_aux->next = *error;
					}
					*error = err_aux;

					free(schematron_error);
				}

				xmlXPathFreeObject(result);
				xmlXPathFreeContext(ctxt);
				xmlFreeDoc(sch_result);

				return (EXIT_FAILURE);
			} else {
				/* there is no error */
				retval = EXIT_SUCCESS;
			}
			xmlXPathFreeObject(result);
		} else {
			WARN("Evaluating Schematron output failed");
		}
		xmlXPathFreeContext(ctxt);
		xmlFreeDoc(sch_result);
	}

	if (ds->validators.callback) {
		/* datastore specific validation function */
		DBG("Datastore-specific validation on subdatastore %d", ds->id);

		retval = ds->validators.callback(doc, error);
		if (retval != EXIT_SUCCESS) {
			VERB("subdatastore %d fails to validate with datastore-specific validation", ds->id);
			if (*error == NULL) {
				*error = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(*error, NC_ERR_PARAM_MSG,
				    "Datastore fails to validate via registered callback.");
			}
			return (EXIT_FAILURE);
		}
	}

	return (retval);
}

static int apply_rpc_validate_(struct ncds_ds* ds, const struct nc_session* session, NC_DATASTORE source, const char* config, struct nc_err** e)
{
	int ret = EXIT_FAILURE;
	char *data_cfg = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr root, node;
	xmlNsPtr ns;

	if (!ds->validators.rng && !ds->validators.rng_schema && !ds->validators.schematron) {
		/* validation not supported by this datastore */
		return (EXIT_RPC_NOT_APPLICABLE);
	}

	/* init/clean error information */
	*e = NULL;

	switch (source) {
	case NC_DATASTORE_RUNNING:
	case NC_DATASTORE_STARTUP:
	case NC_DATASTORE_CANDIDATE:
		if ((data_cfg = ds->func.getconfig(ds, session, source, e)) == NULL ) {
			if (*e == NULL ) {
				ERROR("%s: Failed to get data from the datastore (%s:%d).", __func__, __FILE__, __LINE__);
				*e = nc_err_new(NC_ERR_OP_FAILED);
			}
			return (EXIT_FAILURE);
		}
		break;
	case NC_DATASTORE_CONFIG:
		/*
		 * config can contain multiple elements on the root level, so
		 * cover it with the <config> element to allow the creation of xml
		 * document
		 */
		data_cfg = (char*)config;
		break;
	default:
		*e = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*e, NC_ERR_PARAM_INFO_BADELEM, "source");
		return (EXIT_FAILURE);
	}

	doc = read_datastore_data(ds->id, data_cfg);
	if (doc == NULL || doc->children == NULL) {
		/* config is empty */
		xmlFreeDoc(doc);
		doc = NULL;
	}
	if (source != NC_DATASTORE_CONFIG) {
		free(data_cfg);
	}

	if (!doc) {
		/*
		 * there are no data to validate - datastore is empty and
		 * it is a valid state of the datastore
		 */
		ret = EXIT_SUCCESS;
	} else {
		/*
		 * reconnect root elements from datastore data under the <data>
		 * element required by validators
		 */
		root = xmlNewNode(NULL, BAD_CAST "config");
		ns = xmlNewNs(root, (xmlChar *) NC_NS_BASE10, NULL);
		xmlSetNs(root, ns);
		for (node = doc->children; node != NULL; node = doc->children) {
			xmlUnlinkNode(node);

			/* keep only data relevant to this datastore module */
			if (node->ns && node->ns->href &&
					!strcmp(ds->data_model->ns, (char*) node->ns->href)) {
				xmlAddChild(root, node);
			} else {
				xmlFreeNode(node);
			}
		}
		xmlDocSetRootElement(doc, root);

		ret = validate_ds(ds, doc, e);

		xmlFreeDoc(doc);
	}

	return (ret);

}

static int apply_rpc_validate(struct ncds_ds* ds, const struct nc_session* session, const nc_rpc* rpc, struct nc_err** e)
{
	int ret;
	char *config;
	NC_DATASTORE source;

	if (!ds->validators.rng && !ds->validators.rng_schema && !ds->validators.schematron) {
		/* validation not supported by this datastore */
		return (EXIT_RPC_NOT_APPLICABLE);
	}

	switch (source = nc_rpc_get_source(rpc)) {
	case NC_DATASTORE_RUNNING:
	case NC_DATASTORE_STARTUP:
	case NC_DATASTORE_CANDIDATE:
		ret = apply_rpc_validate_(ds, session, source, NULL, e);
		break;
	case NC_DATASTORE_URL:
	case NC_DATASTORE_CONFIG:
		/*
		 * config can contain multiple elements on the root level, so
		 * cover it with the <config> element to allow the creation of xml
		 * document
		 */
		config = nc_rpc_get_config(rpc);
		ret = apply_rpc_validate_(ds, session, NC_DATASTORE_CONFIG, config, e);
		free(config);
		break;
	default:
		*e = nc_err_new(NC_ERR_BAD_ELEM);
		nc_err_set(*e, NC_ERR_PARAM_INFO_BADELEM, "source");
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}

#endif /* not DISABLE_VALIDATION */

#ifdef DISABLE_VALIDATION
API int ncds_set_validation(struct ncds_ds* UNUSED(ds), int UNUSED(enable), const char* UNUSED(relaxng), const char* UNUSED(schematron))
{
	return (EXIT_SUCCESS);
}
#else
API int ncds_set_validation(struct ncds_ds* ds, int enable, const char* relaxng, const char* schematron)
{
	int ret = EXIT_SUCCESS;
	xmlRelaxNGParserCtxtPtr rng_ctxt = NULL;
	xmlRelaxNGPtr rng_schema = NULL;
	xmlRelaxNGValidCtxtPtr rng = NULL;
	xsltStylesheetPtr schxsl = NULL;

	if (enable == 0) {
		/* disable validation on this datastore */
		xmlRelaxNGFreeValidCtxt(ds->validators.rng);
		xmlRelaxNGFree(ds->validators.rng_schema);
		xsltFreeStylesheet(ds->validators.schematron);
		memset(&(ds->validators), 0, sizeof(struct model_validators));
	} else if (nc_init_flags & NC_INIT_VALIDATE) { /* && enable == 1 */
		/* enable and reset validators */
		if (relaxng != NULL) {
			/* prepare validators - Relax NG */
			if (eaccess(relaxng, R_OK) == -1) {
				ERROR("%s: Unable to access RelaxNG schema for validation (%s - %s).", __func__, relaxng, strerror(errno));
				ret = EXIT_FAILURE;
				goto cleanup;
			} else {
				rng_ctxt = xmlRelaxNGNewParserCtxt(relaxng);
				if ((rng_schema = xmlRelaxNGParse(rng_ctxt)) == NULL) {
					ERROR("Failed to parse Relax NG schema (%s)", relaxng);
					ret = EXIT_FAILURE;
					goto cleanup;
				} else if ((rng = xmlRelaxNGNewValidCtxt(rng_schema)) == NULL) {
					ERROR("Failed to create validation context (%s)", relaxng);
					ret = EXIT_FAILURE;
					goto cleanup;
				}
				xmlRelaxNGFreeParserCtxt(rng_ctxt);
				rng_ctxt = NULL;
			}
		}

		if (schematron != NULL) {
			/* prepare validators - Schematron */
			if (eaccess(schematron, R_OK) == -1) {
				ERROR("%s: Unable to access Schematron stylesheet for validation (%s - %s).", __func__, schematron, strerror(errno));
				ret = EXIT_FAILURE;
				goto cleanup;
			} else {
				if ((schxsl = xsltParseStylesheetFile(BAD_CAST schematron)) == NULL) {
					ERROR("Failed to parse Schematron stylesheet (%s)", schematron);
					ret = EXIT_FAILURE;
					goto cleanup;
				}
			}
		}

		/* replace previous validators */
		if (rng_schema && rng) {
			xmlRelaxNGFree(ds->validators.rng_schema);
			ds->validators.rng_schema = rng_schema;
			rng_schema = NULL;
			xmlRelaxNGFreeValidCtxt(ds->validators.rng);
			ds->validators.rng = rng;
			rng = NULL;
			DBG("%s: Relax NG validator set (%s)", __func__, relaxng);
		}
		if (schxsl) {
			xsltFreeStylesheet(ds->validators.schematron);
			ds->validators.schematron = schxsl;
			schxsl = NULL;
			DBG("%s: Schematron validator set (%s)", __func__, schematron);
		}

	}

cleanup:
	xmlRelaxNGFreeValidCtxt(rng);
	xmlRelaxNGFree(rng_schema);
	xmlRelaxNGFreeParserCtxt(rng_ctxt);
	xsltFreeStylesheet(schxsl);

	return (ret);
}
#endif


#ifdef DISABLE_VALIDATION
API int ncds_set_validation2(struct ncds_ds* UNUSED(ds), int UNUSED(enable),
	const char* UNUSED(relaxng), const char* UNUSED(schematron),
    int (*valid_func)(const xmlDocPtr config, struct nc_err **err) __attribute__((__unused__)))
{
	return (EXIT_SUCCESS);
}
#else
API int ncds_set_validation2(struct ncds_ds* ds, int enable, const char* relaxng,
    const char* schematron,
    int (*valid_func)(const xmlDocPtr config, struct nc_err **err))
{
	int ret;

	ret = ncds_set_validation(ds, enable, relaxng, schematron);
	if (ret != EXIT_SUCCESS) {
		return (ret);
	}

	ds->validators.callback = valid_func;

	return (ret);
}
#endif

static struct ncds_ds* ncds_new_internal(NCDS_TYPE type, const char * model_path)
{
	int ret;
	struct ncds_ds* ds = NULL;
	struct ncds_ds_list *ds_iter;
	char *basename, *path_yin;

#ifndef DISABLE_VALIDATION
	char *path_rng = NULL, *path_sch = NULL;
	xmlRelaxNGParserCtxtPtr rng_ctxt;
#endif

	if (model_path == NULL) {
		ERROR("%s: missing the model path parameter.", __func__);
		return (NULL);
	}

	/* check the form of model_path - for backward compatibility
	 * model_path.yin is supported and validation file names will be also
	 * derived
	 */
	basename = strdup(model_path);
	nc_clip_occurences_with(basename, '/', '/');
	if (strcmp(&(basename[strlen(basename)-4]), ".yin") == 0) {
		path_yin = strdup(model_path);
		basename[strlen(basename)-4] = 0; /* remove .yin suffix */
	} else {
		if (asprintf(&path_yin, "%s.yin", basename) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			path_yin = NULL;
		}
	}
#ifndef DISABLE_VALIDATION
	if (asprintf(&path_rng, "%s-config.rng", basename) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		path_rng = NULL;
	}
	if (asprintf(&path_sch, "%s-schematron.xsl", basename) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		path_sch = NULL;
	}
#endif

	ds = ncds_fill_func(type);
	if (ds == NULL) {
		/* The error was reported already. */
		goto cleanup;
	}

	ds->type = type;

	/* get configuration data model */
	ds->data_model = read_model(path_yin);
	if (ds->data_model == NULL) {
		free(ds);
		ds = NULL;
		goto cleanup;
	}
	ds->ext_model = ds->data_model->xml;
	ds->ext_model_tree = NULL;

	/* check if there is already datastore with this model */
	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next) {
		if (ds_iter->datastore->data_model == ds->data_model) {
			/* this datastore already exists */
			free(ds);
			ds = NULL;
			ERROR("Creating datastore failed (Datastore already exists).");
			goto cleanup;
		}
	}

#ifndef DISABLE_VALIDATION
	if (nc_init_flags & NC_INIT_VALIDATE) {
		/* prepare validation - Relax NG */
		if (eaccess(path_rng, R_OK) == -1) {
			WARN("Missing RelaxNG schema for validation (%s - %s).", path_rng, strerror(errno));
		} else {
			rng_ctxt = xmlRelaxNGNewParserCtxt(path_rng);
			if ((ds->validators.rng_schema = xmlRelaxNGParse(rng_ctxt)) == NULL) {
				WARN("Failed to parse Relax NG schema (%s)", path_rng);
			} else if ((ds->validators.rng = xmlRelaxNGNewValidCtxt(ds->validators.rng_schema)) == NULL) {
				WARN("Failed to create validation context (%s)", path_rng);
				xmlRelaxNGFree(ds->validators.rng_schema);
				ds->validators.rng_schema = NULL;
			} else {
				DBG("%s: Relax NG validator set (%s)", __func__, path_rng);
			}
			xmlRelaxNGFreeParserCtxt(rng_ctxt);
		}

		/* prepare validation - Schematron */
		if (eaccess(path_sch, R_OK) == -1) {
			WARN("Missing Schematron stylesheet for validation (%s - %s).", path_sch, strerror(errno));
		} else {
			if ((ds->validators.schematron = xsltParseStylesheetFile(BAD_CAST path_sch)) == NULL) {
				WARN("Failed to parse Schematron stylesheet (%s)", path_sch);
			} else {
				DBG("%s: Schematron validator set (%s)", __func__, path_sch);
			}
		}
	}
#endif /* not DISABLE_VALIDATION */

	/* TransAPI structure is set to NULLs */

	ret = pthread_mutex_init(&ds->lock, NULL);
	if (ret != 0) {
		free(ds);
		ds = NULL;
		ERROR("Initialization of a mutex failed (%s).", strerror(errno));
		goto cleanup;
	}

	ds->last_access = 0;

	/* ds->id is -1 to indicate, that datastore is still not fully configured */
	ds->id = -1;

cleanup:
	free(basename);
	free(path_yin);

#ifndef DISABLE_VALIDATION
	free(path_rng);
	free(path_sch);
#endif

	return (ds);
}

API struct ncds_ds* ncds_new2(NCDS_TYPE type, const char * model_path, xmlDocPtr (*get_state)(const xmlDocPtr model, const xmlDocPtr running, struct nc_err **e))
{
	struct ncds_ds * ds;

	if ((ds = ncds_new_internal(type, model_path)) != NULL) {
		ds->get_state_xml = get_state;
		ds->get_state = NULL;
	}

	return(ds);
}

API struct ncds_ds* ncds_new(NCDS_TYPE type, const char* model_path, char* (*get_state)(const char* model, const char* running, struct nc_err** e))
{
	struct ncds_ds * ds;

	if ((ds = ncds_new_internal(type, model_path)) != NULL) {
		ds->get_state_xml = NULL;
		ds->get_state = get_state;
	}

	return(ds);
}

static ncds_id generate_id(void)
{
	ncds_id current_id;

	do {
		/* generate id */
		current_id = (rand() + 1) % INT_MAX;
		/* until it's unique */
	} while (datastores_get_ds(current_id) != NULL);

	return current_id;
}

static void ncds_ds_model_free(struct data_model* model)
{
	int i;
	struct model_list *listitem, *listprev;

	if (model == NULL) {
		return;
	}

	/* models_list */
	listprev = NULL;
	for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
		if (listitem->model == model) {
			if (listprev != NULL) {
				listprev->next = listitem->next;
			} else {
				models_list = listitem->next;
			}
			free(listitem);
			break;
		}
		listprev = listitem;
	}

	free(model->path);
	free(model->name);
	free(model->version);
	free(model->ns);
	free(model->prefix);
	if (model->rpcs != NULL) {
		for (i = 0; model->rpcs[i] != NULL; i++) {
			free(model->rpcs[i]);
		}
		free(model->rpcs);
	}
	if (model->notifs != NULL) {
		for (i = 0; model->notifs[i] != NULL; i++) {
			free(model->notifs[i]);
		}
		free(model->notifs);
	}
	if (model->xml != NULL) {
		xmlFreeDoc(model->xml);
	}
	if (model->ctxt != NULL) {
		xmlXPathFreeContext(model->ctxt);
	}
	if (model->features != NULL) {
		for (i = 0; model->features[i] != NULL ; i++) {
			free(model->features[i]->name);
			free(model->features[i]);
		}
		free(model->features);
	}

	free(model);
}

API ncds_id ncds_init(struct ncds_ds* datastore)
{
	struct ncds_ds_list * item;

	/* not initiated datastores have id set to -1 */
	if (datastore == NULL || datastore->id != -1) {
		return -1;
	}

	/* check the size of datastore list */
	if ((ncds.count + 1) >= ncds.array_size) {
		void *tmp = realloc(ncds.datastores_ids, (ncds.array_size + 10) * sizeof(ncds_id));
		if (tmp == NULL) {
			ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
			return (-4);
		}
		ncds.datastores_ids = tmp;
		ncds.array_size += 10;
	}
	/* prepare slot for the datastore in the list */
	item = malloc(sizeof(struct ncds_ds_list));
	if (item == NULL) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return -4;
	}

	/** \todo data model validation */

	/* call implementation-specific datastore init() function */
	if (datastore->func.init(datastore) != 0) {
		free(item);
		return -2;
	}

	/* acquire unique id */
	datastore->id = generate_id();

	VERB("Datastore %s initiated with ID %d.", datastore->data_model->name, datastore->id);

	/* add to list */
	item->datastore = datastore;
	item->next = ncds.datastores;
	ncds.datastores = item;
	ncds.count++;

	return datastore->id;
}

void ncds_cleanall()
{
	struct ncds_ds_list *ds_item, *dsnext;
	struct model_list *listitem, *listnext;
	int i;

        //pthread_spin_destroy(&server_cpblt_lock);

	ds_item = ncds.datastores;
	while (ds_item != NULL) {
		dsnext = ds_item->next;
		ncds_free(ds_item->datastore);
		ds_item = dsnext;
	}
	free(ncds.datastores_ids);
	ncds.datastores = NULL;
	ncds.datastores_ids = NULL;
	ncds.count = 0;
	ncds.array_size = 0;

	for (listitem = models_list; listitem != NULL; ) {
		listnext = listitem->next;
		ncds_ds_model_free(listitem->model);
		/* listitem is actually also freed by ncds_ds_model_free() */
		listitem = listnext;
	}

	for (i = 0; models_dirs != NULL && models_dirs[i] != NULL; i++) {
		free(models_dirs[i]);
	}
	free(models_dirs);
	models_dirs = NULL;

	transapis_cleanup(&(augment_tapi_list), 1);

#ifndef DISABLE_YANGFORMAT
	xsltFreeStylesheet(yin2yang_xsl);
	yin2yang_xsl = NULL;
#endif
}

API void ncds_free(struct ncds_ds* datastore)
{
	struct ncds_ds *ds = NULL;
	struct transapi_list* tapi_iter;
	int i;

	if (datastore == NULL) {
		return;
	}

	if (datastore->id != -1) {
		/* datastore is initialized and must be in the datastores list */
		ds = datastores_detach_ds(datastore->id);
	} else {
		/* datastore to free is uninitialized and will be only freed */
		ds = datastore;
	}

	/* close and free the datastore itself */
	if (ds != NULL) {
		/* if using transapi */
		if (ds->transapis != NULL) {
			/*
			 * free the list, but not directly the transAPI structures, they are
			 * still part of internal list
			 */
			while (ds->transapis != NULL) {
				tapi_iter = ds->transapis->next;
				if (ds->transapis->ref_count != 0) {
					transapi_unload(ds->transapis->tapi);
					free(ds->transapis->tapi);
				}
				free(ds->transapis);
				ds->transapis = tapi_iter;
			}
			if (ds->tapi_callbacks != NULL) {
				for (i = 0; i < ds->tapi_callbacks_count; i++) {
					free(ds->tapi_callbacks[i].path);
				}
				free(ds->tapi_callbacks);
			}
		}

#ifndef DISABLE_VALIDATION
		/* validators */
		xmlRelaxNGFreeValidCtxt(ds->validators.rng);
		xmlRelaxNGFree(ds->validators.rng_schema);
		xsltFreeStylesheet(ds->validators.schematron);
#endif
		/* free all implementation specific resources */
		ds->func.free(ds);

		/* free models */
		if (ds->data_model == NULL || (ds->data_model->xml != ds->ext_model)) {
			xmlFreeDoc(ds->ext_model);
		}
		ncds_ds_model_free(ds->data_model);
		yinmodel_free(ds->ext_model_tree);

		free (ds);
	}
}

API void ncds_free2(ncds_id datastore_id)
{
	struct ncds_ds *del;

	/* empty list */
	if (ncds.datastores == NULL) {
		return;
	}

	/* invalid id */
	if (datastore_id <= 0) {
		WARN("%s: invalid datastore ID to free.", __func__);
		return;
	}

	/* get datastore from the internal datastores list */
	del = datastores_get_ds(datastore_id);

	/* free if any found */
	if (del != NULL) {
		/*
		 * ncds_free() detaches the item from the internal datastores
		 * list and also the whole list item (del variable here) is freed
		 * by ncds_free(), so do not do it here!
		 */
		ncds_free(del);
	}
}

/**
 * \brief Decide if the given child is a key element of the parent.
 *
 * \param[in] parent Parent element which key node is checked.
 * \param[in] child Element to decide if it is a key element of the parent
 * \param[in] keys List of key elements from the configuration data model.
 * \return Zero if the given child is NOT the key element of the parent.
 */
int is_key(xmlNodePtr parent, xmlNodePtr child, keyList keys)
{
	xmlChar *str = NULL;
	char *s, *token;
	int i, match;
	xmlNodePtr key_parent, node_parent;

	assert(parent != NULL);
	assert(child != NULL);

	if (keys == NULL) {
		/* there are no keys */
		return 0;
	}

	for (i = 0; i < keys->nodesetval->nodeNr; i++) {
		match = 1;
		key_parent = keys->nodesetval->nodeTab[i]->parent;
		node_parent = parent;

		while (1) {
			if ((str = xmlGetProp(key_parent, BAD_CAST "name")) == NULL) {
				match = 0;
				break;
			}
			if (xmlStrcmp(str, node_parent->name)) {
				xmlFree(str);
				match = 0;
				break;
			}
			xmlFree(str);

			do {
				key_parent = key_parent->parent;
			} while (key_parent && ((xmlStrcmp(key_parent->name, BAD_CAST "augment") == 0)
					|| (xmlStrcmp(key_parent->name, BAD_CAST "choice") == 0)
					|| (xmlStrcmp(key_parent->name, BAD_CAST "case") == 0)));
			node_parent = node_parent->parent;

			if ((!key_parent && node_parent) || (key_parent && !node_parent)) {
				match = 0;
				break;
			}

			if (!xmlStrcmp(key_parent->name, BAD_CAST "module") && (node_parent->type == XML_DOCUMENT_NODE)) {
				/* we have match */
				break;
			}
		}

		if (!match) {
			continue;
		}

		/* so the node is in a key's place (list), but is it a key?
		 * btw, now we don't want to iterate through next key specs,
		 * so do not use continue from here
		 */

		/* get the name of the key node(s) from the 'value' attribute in key element in data model */
		if ((str = xmlGetProp(keys->nodesetval->nodeTab[i], BAD_CAST "value")) == NULL) {
			continue;
		}

		/* attribute have the form of space-separated list of key nodes */
		/* compare all the key node names with the specified child */
		for (token = s = (char*)str; token != NULL ; s = NULL) {
			token = strtok(s, " ");
			if (token == NULL) {
				break;
			}

			if (xmlStrcmp(BAD_CAST token, child->name) == 0) {
				xmlFree(str);
				return 1;
			}
		}
		xmlFree(str);
		break;
	}

	return 0;
}

static xmlDocPtr ncxml_merge(const xmlDocPtr first, const xmlDocPtr second, const xmlDocPtr data_model)
{
	int ret = EXIT_FAILURE;
	keyList keys;
	xmlDocPtr result;
	xmlNodePtr node;

	/* return NULL if both docs are NULL, or the other doc in case on of them is NULL */
	if (first == NULL) {
		return (second ? xmlCopyDoc(second, 1) : NULL);
	} else if (second == NULL) {
		return (xmlCopyDoc(first, 1));
	}

	result = xmlCopyDoc(first, 1);
	if (result == NULL) {
		return (NULL);
	}

	/* get all keys from data model */
	keys = get_keynode_list(data_model);

	/* merge the documents */
	for (node = second->children; node != NULL; node = second->children) {
		if ((ret = edit_merge(result, second->children, NC_EDIT_DEFOP_MERGE, data_model, keys, NULL, NULL)) != EXIT_SUCCESS) {
			break;
		}
	}

	if (keys != NULL) {
		keyListFree(keys);
	}

	if (ret != EXIT_SUCCESS) {
		xmlFreeDoc(result);
		return (NULL);
	} else {
		return (result);
	}
}

/**
 * \brief compare the node properties against the reference node properties
 *
 * \param reference     reference node, compared node must have all the
 *                      properties (and the same values) as reference node
 * \param node          compared node
 *
 * \return              0 if compared node contains all the properties (with
 *						the same values) as reference node, 1 otherwise
 */
static int attrcmp(xmlNodePtr reference, xmlNodePtr node)
{
	xmlAttrPtr attr = reference->properties;
	xmlChar *value = NULL, *refvalue = NULL;

	while (attr != NULL) {
		if ((value = xmlGetProp(node, attr->name)) == NULL) {
			return 1;
		} else {
			refvalue = xmlGetProp(reference, attr->name);
			if (strcmp((char *) refvalue, (char *) value)) {
				free(refvalue);
				free(value);
				return 1;
			}
			free(refvalue);
			free(value);
		}
		attr = attr->next;
	}

	return 0;
}

/**
 * \brief NETCONF subtree filtering, stolen from old old netopeer
 *
 * \param config        pointer to xmlNode tree to filter
 * \param filter        pointer to NETCONF filter xml tree
 *
 * \return              1 if config satisfies the output filter, 0 otherwise
 */

static int ncxml_subtree_filter(xmlNodePtr config, xmlNodePtr filter, keyList keys)
{
	xmlNodePtr config_node;
	xmlNodePtr filter_node;
	xmlNodePtr delete = NULL, delete2 = NULL;
	char *content1 = NULL, *content2 = NULL;
	int nomatch = 0;
	int filter_in = 0, sibling_in = 0, end_node = 0, sibling_selection = 0;

	/* normalize filter */
	if (!config->prev) {
		for (filter_node = filter; filter_node; ) {
			delete = filter_node;
			filter_node = filter_node->next;

			/* remove comments from filter */
			if (delete->type != XML_ELEMENT_NODE) {
				if (delete == filter) {
					filter = filter_node;
				}
				xmlUnlinkNode(delete);
				xmlFreeNode(delete);
				continue;
			}
		}
	}

	/* check if this filter level is last */
	filter_node = filter;
	while (filter_node) {
		if ((filter_node->children) && (filter_node->children->type == XML_TEXT_NODE) &&
				!xmlIsBlankNode(filter_node->children)) {
			end_node = 1;
			break;
		}
		filter_node = filter_node->next;
	}

	if (end_node) {

		/* 0 means that all the sibling nodes will be in the filter result - this is a default
		 * behavior when there are no selection or containment nodes in the filter sibling set.
		 * If 1 is set, sibling nodes for the filter result will be selected according to the
		 * rules in RFC 6241, sec. 6.2.5
		 */
		sibling_selection = 0;

		/* init */
		if ((content1 = nc_clrwspace((char *) filter_node->children->content)) == NULL) {
			/* internal error - memory allocation failed, do not continue! */
			return 0;
		}

		/* try to find required node */
		config_node = config;
		while (config_node && config_node->children) {
			if (!strcmp((char *) filter_node->name, (char *) config_node->name) &&
					!nc_nscmp(filter_node, config_node) &&
					!attrcmp(filter_node, config_node)) {
				/* init */
				content2 = NULL;
				/* get node's content without leading and trailing spaces */
				if ((content2 = nc_clrwspace((char *) config_node->children->content)) == NULL) {
					free(content1);
					free(content2);
					/* internal error - memory allocation failed, do not continue! */
					return 0;
				}

				if (strisempty(content1)) {
					/* we have an empty content match node, so interpret it as a selection node,
					 * which means that we will be selecting sibling nodes that will be in the
					 * filter result
					 */
					filter_in = 1;
					sibling_selection = 1;
				} else if (strcmp(content1, content2) == 0) {
					filter_in = 1;
				}
				free(content2);

				if (filter_in) {
					free(content1);
					content1 = NULL;

					/* we have the matching node, now decide what to do */
					if (filter_node->next || filter_node->prev || sibling_selection == 1) {
						/* check if all filter sibling nodes are content match nodes -> then no config sibling node will be removed */
						/*go to the first filter sibling node */
						filter_node = filter;
						/* pass all filter sibling nodes */
						while (sibling_selection == 0 && filter_node) {
							if (!filter_node->children || (filter_node->children->type != XML_TEXT_NODE) ||
									xmlIsBlankNode(filter_node->children)) {
								sibling_selection = 1; /* filter result will be selected */
								break;
							}
							filter_node = filter_node->next;
						}

						/* select and remove all unwanted nodes */
						config_node = config;
						while (config_node) {
							/* init */
							sibling_in = 0;

							/* go to the first filter sibing node */
							filter_node = filter;

filter:
							/* pass all filter sibling nodes */
							while (filter_node) {
								if (!strcmp((char *) filter_node->name, (char *) config_node->name) &&
										!nc_nscmp(filter_node, config_node) && !attrcmp(filter_node, config_node)) {
									/* content match node check */
									if (filter_node->children && (filter_node->children->type == XML_TEXT_NODE) && !xmlIsBlankNode(filter_node->children) &&
											config_node->children && (config_node->children->type == XML_TEXT_NODE) && !xmlIsBlankNode(config_node->children)) {
										/* get filter's text node content ignoring whitespaces */
										if ((content2 = nc_clrwspace((char *) filter_node->children->content)) == NULL ||
												(content1 = nc_clrwspace((char *) config_node->children->content)) == NULL) {
											free(content1);
											free(content2);
											/* internal error - memory allocation failed, do not continue! */
											return 0;
										}
										if (strcmp(content1, content2) != 0) {
											free(content1);
											free(content2);
											filter_node = filter_node->next;
											nomatch = 1;
											continue;
										}
										free(content1);
										free(content2);
									}
									sibling_in = 1;
									break;
								}
								filter_node = filter_node->next;
							}
							content1 = NULL;

							if (!filter_node) {
								if (nomatch) {
									/* instance does not follow restrictions */
									return 0;
								} else if (is_key(config_node->parent, config_node, keys)) {
									/* go to the next sibling */
									config_node = config_node->next;
									continue;
								}
							}

							/* if this config node is not in filter, remove it */
							if (sibling_selection && !sibling_in) {
								if (filter_node) {
									/* try another filter node */
									filter_node = filter_node->next;
									goto filter;
								}

								delete = config_node;
								config_node = config_node->next;
								xmlUnlinkNode(delete);
								xmlFreeNode(delete);
							} else {
								/* recursively process subtree filter */
								if (filter_node && filter_node->children && (filter_node->children->type == XML_ELEMENT_NODE) && config_node->children && (config_node->children->type == XML_ELEMENT_NODE)) {
									sibling_in = ncxml_subtree_filter(config_node->children, filter_node->children, keys);
								}
								if (sibling_selection && sibling_in == 0) {
									if (filter_node) {
										/* try another filter node */
										filter_node = filter_node->next;
										goto filter;
									}

									/* subtree is not a content of the filter output */
									delete = config_node;

									/* remeber where to go next */
									config_node = config_node->next;

									/* and remove unwanted subtree */
									xmlUnlinkNode(delete);
									xmlFreeNode(delete);
								} else {
									/* go to the next sibling */
									config_node = config_node->next;
								}
							}
						}
					} else {
						/* only content match node present - all sibling nodes stays */
					}
					break;
				}
			}
			config_node = config_node->next;
		}
		free(content1);
	} else {
		/* this is containment node (no sibling node is content match node */
		filter_node = filter;
		while (filter_node) {
			if (!strcmp((char *)filter_node->name, (char *)config->name) &&
					!nc_nscmp(filter_node, config) &&
					!attrcmp(filter_node, config)) {
				filter_in = 1;
				break;
			}
			filter_node = filter_node->next;
		}

		if (filter_in == 1) {
			while (config->children && filter_node && filter_node->children && !xmlIsBlankNode(filter_node->children) &&
					((filter_in = ncxml_subtree_filter(config->children, filter_node->children, keys)) == 0)) {
				filter_node = filter_node->next;
				while (filter_node) {
					if (!strcmp((char *)filter_node->name, (char *)config->name) &&
							!nc_nscmp(filter_node, config) &&
							!attrcmp(filter_node, config)) {
						filter_in = 1;
						break;
					}
					filter_node = filter_node->next;
				}
			}
			if (filter_in == 0) {
				/* subtree is not a content of the filter output */
				delete = config->children;
				xmlUnlinkNode(delete);
				xmlFreeNode(delete);
				delete2 = config;
			}
		} else {
			delete2 = config;
		}
		/* filter next sibling node */
		if (config->next != NULL) {
			if (ncxml_subtree_filter(config->next, filter, keys) == 0) {
				delete = config->next;
				xmlUnlinkNode(delete);
				xmlFreeNode(delete);
			} else {
				filter_in = 1;
			}
		}
		if (delete2) {
			xmlUnlinkNode(delete2);
			xmlFreeNode(delete2);
		}
	}

	return filter_in;
}

int ncxml_filter(xmlNodePtr old, const struct nc_filter* filter, xmlNodePtr *new, const xmlDocPtr data_model)
{
	xmlDocPtr result, data_filtered[2] = {NULL, NULL};
	xmlNodePtr filter_item, node;
	keyList keys;
	int ret = EXIT_FAILURE;

	if (new == NULL || old == NULL || filter == NULL) {
		return EXIT_FAILURE;
	}

	switch (filter->type) {
	case NC_FILTER_SUBTREE:
		if (filter->subtree_filter == NULL) {
			ERROR("%s: invalid filter (%s:%d).", __func__, __FILE__, __LINE__);
			return EXIT_FAILURE;
		}

		/* get all keys from data model */
		keys = get_keynode_list(data_model);

		data_filtered[0] = xmlNewDoc(BAD_CAST "1.0");
		data_filtered[1] = xmlNewDoc(BAD_CAST "1.0");
		for (filter_item = filter->subtree_filter->children; filter_item != NULL; filter_item = filter_item->next) {
			xmlAddChildList((xmlNodePtr)(data_filtered[0]), xmlCopyNodeList(old));

			/* modify filter doc to deny ncxml_subtree_filter processing
			 * siblings that are (on this top level) meaningless and they
			 * will be processed separatelly in the next run of the loop.
			 */
			node = filter_item->next;
			filter_item->next = NULL;
			ncxml_subtree_filter(data_filtered[0]->children, filter_item, keys);
			/* revert change made to the filter doc */
			filter_item->next = node;

			if (data_filtered[1]->children == NULL) {
				/* there are no data so far */
				if (data_filtered[0]->children != NULL) {
					/* we have some result, move it to [1] */
					node = data_filtered[0]->children;
					xmlUnlinkNode(node);
					xmlDocSetRootElement(data_filtered[1], node);
				}
			} else if (data_filtered[0]-> children != NULL) {
				/* there are some data already filtered */
				/* and we have some new data, so merge them */
				if (data_model != NULL) {
					result = ncxml_merge(data_filtered[0], data_filtered[1], data_model);
				} else {
					result = data_filtered[1];
					data_filtered[1] = NULL;
					xmlDocCopyNodeList(result, data_filtered[0]->children);
				}

				node = data_filtered[0]->children;
				xmlUnlinkNode(node);
				xmlFreeNode(node);
				xmlFreeDoc(data_filtered[1]);
				data_filtered[1] = result;
			}
		}

		if (keys != NULL) {
			keyListFree(keys);
		}

		if (filter->subtree_filter->children != NULL) {
			if(data_filtered[1] != NULL && data_filtered[1]->children != NULL) {
				*new = xmlCopyNodeList(data_filtered[1]->children);
			} else {
				*new = NULL;
			}
		} else { /* empty filter -> RFC 6241, sec. 6.4.2 - result is empty */
			*new = NULL;
		}
		xmlFreeDoc(data_filtered[0]);
		xmlFreeDoc(data_filtered[1]);
		ret = EXIT_SUCCESS;
		break;
	default:
		ret = EXIT_FAILURE;
		break;
	}

	return ret;
}

API int ncds_rollback(ncds_id id)
{
	struct ncds_ds *datastore = datastores_get_ds(id);

	if (datastore == NULL) {
		return (EXIT_FAILURE);
	}

	return (datastore->func.rollback(datastore));
}

/**
 * @brief Check if source and target are same. If url is enabled, checks if source and target urls are same
 * @param rpc
 * @param session
 * @return
 */
static int ncds_is_conflict(const nc_rpc * rpc, const struct nc_session * session)
{
	NC_DATASTORE source, target;
#ifndef DISABLE_URL
	xmlXPathObjectPtr query_source = NULL;
	xmlXPathObjectPtr query_target = NULL;
	xmlChar *nc1 = NULL, *nc2 = NULL;
	int ret;
#else /* notDISABLE_URL */
	(void)session; /* supress unused parameter warning */
#endif

	source = nc_rpc_get_source(rpc);
	target = nc_rpc_get_target(rpc);

	if (source == target) {
		/* source and target datastore are the same */
#ifndef DISABLE_URL
		/* if they are URLs, check if both URLs point to a single resource */
		if (source == NC_DATASTORE_URL && nc_cpblts_enabled(session, NC_CAP_URL_ID)) {
			query_source = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":source/"NC_NS_BASE10_ID":url", rpc->ctxt);
			query_target = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":url", rpc->ctxt);
			if ((query_source == NULL || query_target == NULL )) {
				return 1;
			}

			nc1 = xmlNodeGetContent(query_source->nodesetval->nodeTab[0]);
			nc2 = xmlNodeGetContent(query_target->nodesetval->nodeTab[0]);

			if ((nc1 == NULL) || (nc2 == NULL)) {
				ERROR("Empty source or target in ncds_is_conflict");
				return 1;
			}
			ret = xmlStrcmp(nc1, nc2);

			/* cleanup */
			xmlFree(nc1);
			xmlFree(nc2);
			xmlXPathFreeObject(query_source);
			xmlXPathFreeObject(query_target);
			return (ret);
		} else {
#else
		{
#endif /* DISABLE_URL */
			/* rpc targets local datastores */
			return 1;
		}
	}

	return 0;
}

/**
 * \return NULL on success, error reply with error info else
 */
static nc_reply* ncds_apply_transapi(struct ncds_ds* ds, const struct nc_session* session, xmlDocPtr old, NC_EDIT_ERROPT_TYPE erropt, nc_reply *reply)
{
	char *new_data;
	xmlDocPtr new;
	xmlChar *config;
	int ret;
	struct nc_err *e = NULL, *e_new;
	nc_reply *new_reply = NULL;
	int modified;
	struct transapi_list* tapi_iter;

	if (reply != NULL && nc_reply_get_type(reply) == NC_REPLY_ERROR) {
		/* use some reply to add new error messages */
		new_reply = reply;
	}

	/* find differences and call functions */
	new_data = ds->func.getconfig(ds, session, NC_DATASTORE_RUNNING, &e);
	new = read_datastore_data(ds->id, new_data);
	free(new_data);

	/* add default values */
	ncdflt_default_values(new, ds->ext_model, NCWD_MODE_IMPL_TAGGED);

	if (new == NULL ) { /* cannot get or parse data */
		e = nc_err_new(NC_ERR_OP_FAILED);
		if (new_reply != NULL) {
			/* second try, add the error info */
			nc_err_set(e, NC_ERR_PARAM_MSG, "TransAPI: Failed to get data from RUNNING datastore. Configuration is probably inconsistent.");
			nc_reply_error_add(new_reply, e);
		} else {
			nc_err_set(e, NC_ERR_PARAM_MSG, "TransAPI: Failed to get data from RUNNING datastore.");
			new_reply = nc_reply_error(e);
		}
	} else {
		/* announce error-option to the TransAPI module, if error-option not set, announce default stop-on-error */
		for (tapi_iter = ds->transapis; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
			*(tapi_iter->tapi->erropt) = (erropt != NC_EDIT_ERROPT_NOTSET) ? erropt : NC_EDIT_ERROPT_STOP;
		}

		/* add default values */
		ncdflt_default_values(old, ds->ext_model, NCWD_MODE_IMPL_TAGGED);

		/* perform TransAPI transactions */
		ret = transapi_running_changed(ds, old, new, erropt, &e);
		if (ret) {
			e_new = nc_err_new(NC_ERR_OP_FAILED);
			if (e != NULL) {
				/* remember the error description from TransAPI */
				e_new->next = e;
				e = NULL;
			}
			if (new_reply != NULL ) {
				/* second try, add the error info */
				nc_err_set(e_new, NC_ERR_PARAM_MSG, "Failed to rollback configuration changes to device. Configuration is probably inconsistent.");
				nc_reply_error_add(new_reply, e_new);
			} else {
				nc_err_set(e_new, NC_ERR_PARAM_MSG, "Failed to apply configuration changes to device.");
				new_reply = nc_reply_error(e_new);

				if (erropt == NC_EDIT_ERROPT_ROLLBACK) {
					/* do the rollback on datastore */
					ds->func.rollback(ds);
				}

			}
		} /* else success */

		modified = 0;
		for (tapi_iter = ds->transapis; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
			if (*(tapi_iter->tapi->config_modified)) {
				*tapi_iter->tapi->config_modified = 0;
				modified = 1;
			}
		}
		if (ret || modified) {
			DBG("Updating XML tree after TransAPI callbacks");
			if (!modified) { /* ret only */
				/* remove default nodes */
				ncdflt_default_clear(old);
				/* revert changes */
				xmlDocDumpMemory(old, &config, NULL);
			} else { /* ret and/or modified */
				/* remove default nodes */
				ncdflt_default_clear(new);
				/* update config data according to changes made by transAPI module */
				xmlDocDumpMemory(new, &config, NULL);
			}
			if (ds->func.copyconfig(ds, session, NULL, NC_DATASTORE_RUNNING, NC_DATASTORE_CONFIG, (char*)config, &e) == EXIT_FAILURE) {
				ERROR("Updating XML tree after transAPI callbacks failed (%s)", e->message);
				nc_err_free(e);
			}
			xmlFree(config);
		}
		xmlFreeDoc(new);
	}

	return (new_reply);
}

/*
 * returns:
 *  0 - filter removes data from this datastore, do not continue
 *  1 - filter includes data from this datastore, continue with processing
 */
static int rpc_get_prefilter(struct nc_filter **filter, const struct ncds_ds* ds, const nc_rpc* rpc, struct nc_filter* shared_filter)
{
	xmlNodePtr filter_node;
	int retval = 1;
	char* s;

	/* get filter if specified for this request */
	if (shared_filter == NULL) {
		*filter = nc_rpc_get_filter(rpc);
	} else {
		*filter = shared_filter;
	}

	/* check root element according to the filter (if any) */
	if (*filter != NULL && (*filter)->type == NC_FILTER_SUBTREE &&
			ds->data_model && ds->data_model->ns) {
		retval = 0;
		for (filter_node = (*filter)->subtree_filter->children; filter_node != NULL; filter_node = filter_node->next) {
			/* XML namespace wildcard mechanism:
			 * 1) no namespace defined and namespace is inherited from message so it
			 *    is NETCONF base namespace
			 * 2) namespace is empty: xmlns=""
			 */
			s = NULL;
			if (filter_node->ns == NULL || filter_node->ns->href == NULL ||
					strcmp((char *)filter_node->ns->href, NC_NS_BASE10) == 0 ||
					strlen(s = nc_clrwspace((char*)(filter_node->ns->href))) == 0) {
				free(s);
				return (1);
			}
			free(s);

			if (filter_node->ns && xmlStrcmp(BAD_CAST ds->data_model->ns, filter_node->ns->href) == 0) {
				return (1);
			}
		}
	}

	if (retval == 0 && shared_filter == NULL) {
		/* we will not continue, so filter structure can be removed if not shared */
		nc_filter_free(*filter);
		*filter = NULL;
	}

	return (retval);
}

/**
 * @ingroup store
 * @brief Perform the requested RPC operation on the datastore.
 *
 * @param[in] id Datastore ID. Use #NCDS_INTERNAL_ID (0) to apply request
 * (typically \<get\>) onto the libnetconf's internal datastore.
 * @param[in] session NETCONF session (a dummy session is acceptable) where the
 * \<rpc\> came from. Capabilities checks are done according to this session.
 * @param[in] rpc NETCONF \<rpc\> message specifying requested operation.
 * @return NULL in case of a non-NC_RPC_DATASTORE_* operation type or invalid
 * parameter session or rpc, else \<rpc-reply\> with \<ok\>, \<data\> or
 * \<rpc-error\> according to the type and the result of the requested
 * operation. When the requested operation is not applicable to the specified
 * datastore (e.g. the namespace does not match), NCDS_RPC_NOT_APPLICABLE
 * is returned.
 */
static nc_reply* ncds_apply_rpc(ncds_id id, const struct nc_session* session, const nc_rpc* rpc, struct nc_filter* shared_filter)
{
	struct nc_err* e = NULL;
	struct ncds_ds* ds = NULL;
	struct nc_filter *filter = NULL;
	char* data = NULL, *config, *model = NULL, *data2, *op_name;
	xmlDocPtr doc1, doc2, doc_merged = NULL;
	int len, dsid, i;
	int ret = EXIT_FAILURE;
	nc_reply* reply = NULL, *old_reply = NULL, *new_reply;
	xmlBufferPtr resultbuffer;
	xmlNodePtr aux_node, node;
	NC_OP op;
	xmlDocPtr old = NULL;
	char * old_data = NULL;
	NC_DATASTORE source_ds = 0, target_ds = 0;
	struct nacm_rpc *nacm_aux;
	nc_rpc *rpc_aux;
	xmlNodePtr op_node;
	xmlNodePtr op_input;
	struct transapi_list* tapi_iter;
	const char * rpc_name;
	const char *data_ns = NULL;
	char *aux = NULL;
	NC_EDIT_ERROPT_TYPE erropt;
#ifndef DISABLE_VALIDATION
	NC_EDIT_TESTOPT_TYPE testopt;
#endif

#ifndef DISABLE_URL
	xmlXPathObjectPtr url_path = NULL;
	xmlNodePtr root;
	xmlChar *url;
	char url_test_empty;
	int url_tmpfile;
	xmlNsPtr ns;
	NC_URL_PROTOCOLS protocol;
#endif /* DISABLE_URL */

	if (rpc == NULL || session == NULL) {
		ERROR("%s: invalid parameter %s", __func__, (rpc==NULL)?"rpc":"session");
		return (NULL);
	}

	dsid = id;

process_datastore:

	ds = datastores_get_ds(dsid);
	if (ds == NULL) {
		return (nc_reply_error(nc_err_new(NC_ERR_OP_FAILED)));
	}

	op = nc_rpc_get_op(rpc);
	/* if transapi used AND operation will affect running repository => store current running content */

	i = pthread_mutex_lock(&ds->lock);
	if (i != 0) {
		ERROR("Failed to lock datastore (%s).", strerror(errno));
		return (NULL);
	}

	if (ds->transapis != NULL
		&& (op == NC_OP_COMMIT || op == NC_OP_COPYCONFIG || (op == NC_OP_EDITCONFIG && (nc_rpc_get_testopt(rpc) != NC_EDIT_TESTOPT_TEST))) &&
		(nc_rpc_get_target(rpc) == NC_DATASTORE_RUNNING)) {

		old_data = ds->func.getconfig(ds, session, NC_DATASTORE_RUNNING, &e);
		old = read_datastore_data(ds->id, old_data);
		if (old == NULL) {/* cannot get or parse data */
			pthread_mutex_unlock(&ds->lock);
			if (e == NULL) { /* error not set */
				e = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(e, NC_ERR_PARAM_MSG, "TransAPI: Failed to get data from RUNNING datastore.");
			}
			return nc_reply_error(e);
		}
		free(old_data);
	}

	filter = NULL;

	switch (op) {
	case NC_OP_LOCK:
	case NC_OP_UNLOCK:
		if (op == NC_OP_LOCK) {
			op_name = "lock";
			ret = ds->func.lock(ds, session, target_ds = nc_rpc_get_target(rpc), &e);
		} else { /* NC_OP_UNLOCK */
			op_name = "unlock";
			ret = ds->func.unlock(ds, session, target_ds = nc_rpc_get_target(rpc), &e);
		}
#ifndef DISABLE_NOTIFICATIONS
		/* log the event */
		if (dsid == NCDS_INTERNAL_ID && ret == EXIT_SUCCESS) {
			switch (target_ds) {
			case NC_DATASTORE_RUNNING:
				aux = "running";
				break;
			case NC_DATASTORE_CANDIDATE:
				aux = "candidate";
				break;
			case NC_DATASTORE_STARTUP:
				aux = "startup";
				break;
			default:
				/* wtf, (un)lock had to fail already */
				aux = "unknown";
			}
			if (asprintf(&data, "<datastore-%s xmlns=\"%s\"><datastore>%s</datastore><session-id>%s</session-id></datastore-%s>",
					op_name, NC_NS_LNC_NOTIFICATIONS, aux, session->session_id, op_name) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				ERROR("Generating datastore-(un)lock event failed.");
			} else {
				ncntf_event_new(-1, NCNTF_GENERIC, data);
				free(data);
				data = NULL;
			}
		}
#endif /* DISABLE_NOTIFICATIONS */
		break;
	case NC_OP_GET:
		/* pre-filter the request for the current datastore part */
		if (!rpc_get_prefilter(&filter, ds, rpc, shared_filter)) {
			/*
			 * filter completely removes content of this repository, so do not
			 * continue with the following operations
			 */
			doc_merged = xmlNewDoc(BAD_CAST "1.0");
			break;
		}

		if ((data = ds->func.getconfig(ds, session, NC_DATASTORE_RUNNING, &e)) == NULL ) {
			if (e == NULL ) {
				ERROR("%s: Failed to get data from the datastore (%s:%d).", __func__, __FILE__, __LINE__);
				e = nc_err_new(NC_ERR_OP_FAILED);
			}
			break;
		}

		if (ds->get_state_xml != NULL || ds->get_state != NULL) {
			/* caller provided callback function to retrieve status data */

			/* convert configuration data into XML structure */
			doc1 = read_datastore_data(ds->id, data);
			if (doc1 == NULL || doc1->children == NULL) {
				/* empty */
				xmlFreeDoc(doc1);
				doc1 = NULL;
			}

			if (ds->get_state_xml != NULL) {
				/* status data are directly in XML format */
				doc2 = ds->get_state_xml(ds->ext_model, doc1, &e);
			} else if (ds->get_state != NULL) {
				/* status data are provided as string, convert it into XML structure */
				xmlDocDumpMemory(ds->ext_model, (xmlChar**) (&model), &len);
				data2 = ds->get_state(model, data, &e);
				doc2 = read_datastore_data(ds->id, data2);
				if (doc2 == NULL || doc2->children == NULL) {
					/* empty */
					xmlFreeDoc(doc2);
					doc2 = NULL;
				}
				free(model);
				free(data2);
			} else {
				/* we have no status data */
				doc2 = NULL;
			}

			if (e != NULL) {
				/* state data retrieval error */
				free(data);
				break;
			}

			/* merge status and config data */
			/* if merge fail (probably one of docs NULL)*/
			if ((doc_merged = ncxml_merge(doc1, doc2, ds->ext_model)) == NULL) {
				/* use only config if not null*/
				if (doc1 != NULL) {
					doc_merged = doc1;
					xmlFreeDoc(doc2);
					/* or only state if not null*/
				} else if (doc2 != NULL) {
					doc_merged = doc2;
					xmlFreeDoc(doc1);
					/* or create empty document to allow further processing */
				} else {
					doc_merged = xmlNewDoc(BAD_CAST "1.0");
					xmlFreeDoc(doc1);
					xmlFreeDoc(doc2);
				}
			} else {
				/* cleanup */
				xmlFreeDoc(doc1);
				xmlFreeDoc(doc2);
			}
		} else {
			doc_merged = read_datastore_data(ds->id, data);
		}
		free(data);

		if (doc_merged == NULL) {
			ERROR("Reading the configuration datastore failed.");
			e = nc_err_new(NC_ERR_OP_FAILED);
			nc_err_set(e, NC_ERR_PARAM_MSG, "Invalid datastore content.");
			break;
		}

		/* process default values */
		if (ds && ds->data_model->xml) {
			ncdflt_default_values(doc_merged, ds->ext_model, rpc->with_defaults);
		}

		/* NACM */
		nacm_check_data_read(doc_merged, rpc->nacm);

		/* if filter specified, now is good time to apply it */
		node = NULL;
		if (doc_merged->children != NULL) {
			if (filter != NULL) {
				if (ncxml_filter(doc_merged->children, filter, &node, ds->ext_model) != 0) {
					ERROR("Filter failed.");
					e = nc_err_new(NC_ERR_BAD_ELEM);
					nc_err_set(e, NC_ERR_PARAM_TYPE, "protocol");
					nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "filter");
					xmlFreeDoc(doc_merged);
					break;
				}
				xmlFreeDoc(doc_merged);
				doc_merged = xmlNewDoc(BAD_CAST "1.0");
				xmlAddChildList((xmlNodePtr)doc_merged, node);
			}
		}

		break;
	case NC_OP_GETCONFIG:
		/* pre-filter the request for the current datastore part */
		if (!rpc_get_prefilter(&filter, ds, rpc, shared_filter)) {
			/*
			 * filter completely removes content of this repository, so do not
			 * continue with the following operations
			 */
			doc_merged = xmlNewDoc(BAD_CAST "1.0");
			break;
		}

		if ((data = ds->func.getconfig(ds, session, nc_rpc_get_source(rpc), &e)) == NULL) {
			if (e == NULL) {
				ERROR ("%s: Failed to get data from the datastore (%s:%d).", __func__, __FILE__, __LINE__);
				e = nc_err_new(NC_ERR_OP_FAILED);
			}
			break;
		}
		doc_merged = read_datastore_data(ds->id, data);
		free(data);

		if (doc_merged == NULL) {
			ERROR("Reading configuration datastore failed.");
			e = nc_err_new(NC_ERR_OP_FAILED);
			nc_err_set(e, NC_ERR_PARAM_MSG, "Invalid datastore content.");
			break;
		}

		/* process default values */
		if (ds && ds->data_model->xml) {
			ncdflt_default_values(doc_merged, ds->ext_model, rpc->with_defaults);
		}

		/* NACM */
		nacm_check_data_read(doc_merged, rpc->nacm);

		/* if filter specified, now is good time to apply it */
		node = NULL;
		if (doc_merged->children != NULL) {
			if (filter != NULL) {
				if (ncxml_filter(doc_merged->children, filter, &node, ds->ext_model) != 0) {
					ERROR("Filter failed.");
					e = nc_err_new(NC_ERR_BAD_ELEM);
					nc_err_set(e, NC_ERR_PARAM_TYPE, "protocol");
					nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "filter");
					xmlFreeDoc(doc_merged);
					break;
				}
				xmlFreeDoc(doc_merged);
				doc_merged = xmlNewDoc(BAD_CAST "1.0");
				xmlAddChildList((xmlNodePtr)doc_merged, node);
			}
		}
		break;
	case NC_OP_EDITCONFIG:
	case NC_OP_COPYCONFIG:
		if (ds->type == NCDS_TYPE_EMPTY) {
			/* there is nothing to edit in empty datastore type */
			ret = EXIT_RPC_NOT_APPLICABLE;
			break;
		}

		/* check target element */
		if ((target_ds = nc_rpc_get_target(rpc)) == NC_DATASTORE_ERROR) {
			e = nc_err_new(NC_ERR_BAD_ELEM);
			nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "target");
			break;
		}
		/* check source element */
		if (op == NC_OP_COPYCONFIG && (source_ds = nc_rpc_get_source(rpc)) == NC_DATASTORE_ERROR) {
			e = nc_err_new(NC_ERR_BAD_ELEM);
			nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "source");
			break;
		}

		if (op == NC_OP_COPYCONFIG && ((source_ds != NC_DATASTORE_CONFIG) && (source_ds != NC_DATASTORE_URL ))) {
			/* <copy-config> with a standard datastore as a source */
			/* check possible conflicts */
			if (ncds_is_conflict(rpc, session) ) {
				e = nc_err_new(NC_ERR_INVALID_VALUE);
				nc_err_set(e, NC_ERR_PARAM_MSG, "Both the target and the source identify the same datastore.");
				break;
			}
			config = NULL;
		} else {
			/* source is url or config, here starts woodo magic */
			/*
			 * if configuration data are provided as operation's config,
			 * just return <config> element content. If it is url,
			 * download remote file and return its content
			 */
			config = nc_rpc_get_config(rpc);
			if (config == NULL) {
				e = nc_err_new(NC_ERR_OP_FAILED);
				break;
			}
			if (strcmp(config, "") == 0) {
				/* config is empty -> ignore rest of magic here,
				 * go to application of the operation and do
				 * delete of the datastore (including running)!
				 */
				goto apply_editcopyconfig;
			}

			/*
			 * config can contain multiple elements on the root level, so
			 * cover it with the <config> element to allow the creation of
			 * xml document
			 */
			if (strncmp(config, "<config", 7) == 0) {
				data = strdup(config);
			} else if (asprintf(&data, "<config>%s</config>", config) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				e = nc_err_new(NC_ERR_OP_FAILED);
				break;
			}
			free(config);
			config = NULL;
			doc1 = xmlReadDoc(BAD_CAST data, NULL, NULL, NC_XMLREAD_OPTIONS);
			free(data);

			if (doc1 == NULL || doc1->children == NULL || doc1->children->children == NULL) {
				if (doc1 != NULL) {
					xmlFreeDoc(doc1);
				}
				e = nc_err_new(NC_ERR_INVALID_VALUE);
				nc_err_set(e, NC_ERR_PARAM_MSG, "Invalid <config> parameter of the rpc request.");
				break;
			}

			/* keep only root elements applicable to the currently processed datastore */
			for (doc2 = NULL, aux_node = doc1->children->children; aux_node != NULL; aux_node = aux_node->next) {
				if (is_model_root(aux_node, ds->data_model)) {
					if (!doc2) {
						doc2 = xmlNewDoc(BAD_CAST "1.0");
						xmlDocSetRootElement(doc2, xmlCopyNode(aux_node, 1));
					} else {
						xmlAddNextSibling(doc2->last, xmlCopyNode(aux_node, 1));
					}
				}
			}
			xmlFreeDoc(doc1);
			if (!doc2) {
				/* request is not intended for this device */
				/* this makes copy-config behavior a little bit magic - if we
				 * copy data from a standard datastore (e.g. startup), and some
				 * part (e.g. internal NACM) of the datastore is empty, such
				 * part in the target datastore is rewritten (removed). But if
				 * the source is config data in RPC or URL, we ignore empty
				 * parts and do copy only of the parts that rewrites the target
				 * with some data.
				 */
				ret = EXIT_RPC_NOT_APPLICABLE;
				break;
			}

			if (NCWD_MODE_ALL_TAGGED & ncdflt_get_supported()) {
				/* if report-all-tagged mode is supported, 'default'
				 * attribute with 'true' or '1' value can appear and we
				 * have to check that the element's value is equal to the
				 * default value. If it is, the element is removed and
				 * the item is supposed to be set to the default value. If the
				 * value is not equal to the default value, the invalid-value
				 * error reply must be returned.
				 */
				if (ncdflt_edit_remove_default(doc2, ds->ext_model) != EXIT_SUCCESS) {
					e = nc_err_new(NC_ERR_INVALID_VALUE);
					nc_err_set(e, NC_ERR_PARAM_MSG, "with-defaults capability failure");
					break;
				}
			}

			/* dump the data to string */
			resultbuffer = xmlBufferCreate();
			if (resultbuffer == NULL) {
				ERROR("%s: xmlBufferCreate failed (%s:%d).", __func__, __FILE__, __LINE__);
				e = nc_err_new(NC_ERR_OP_FAILED);
				break;
			}
			for (aux_node = doc2->children; aux_node != NULL; aux_node = aux_node->next) {
				xmlNodeDump(resultbuffer, NULL, aux_node, 2, 1);
			}
			config = strdup((char *) xmlBufferContent(resultbuffer));
			xmlBufferFree(resultbuffer);
			xmlFreeDoc(doc2);
		}
apply_editcopyconfig:
		/* perform the operation */
		if (op == NC_OP_EDITCONFIG) {
			ret = ds->func.editconfig(ds, session, rpc, target_ds, config, nc_rpc_get_defop(rpc), nc_rpc_get_erropt(rpc), &e);
#ifndef DISABLE_VALIDATION
			if (ret == EXIT_SUCCESS && (nc_cpblts_enabled(session, NC_CAP_VALIDATE11_ID) || nc_cpblts_enabled(session, NC_CAP_VALIDATE10_ID))) {
				/* process test option if set */
				switch (testopt = nc_rpc_get_testopt(rpc)) {
				case NC_EDIT_TESTOPT_TEST:
				case NC_EDIT_TESTOPT_TESTSET:
				/* default value if test option not set is test-then-set, RFC 6241 sec. 7.2 */
				case NC_EDIT_TESTOPT_NOTSET:
					/* validate the result */
					ret = apply_rpc_validate_(ds, session, target_ds, NULL, &e);

					if (ret == EXIT_RPC_NOT_APPLICABLE) {
						/*
						 * we don't have enough information to do
						 * validation (validators are missing), so
						 * just say that it is ok and allow to
						 * perform the required changes. In validate
						 * operation we return RPC not applicable if
						 * no datastore provides validation, but
						 * in edit-config, we don't know if there
						 * is any datastore with validation since
						 * it doesn't need to affect all datastores.
						 */
						ret = EXIT_SUCCESS;
					}

					if (testopt == NC_EDIT_TESTOPT_TEST || ret == EXIT_FAILURE) {
						/*
						 * revert changes in the datastore:
						 * only test was required or error occurred
						 */
						ds->func.rollback(ds);
					}

					break;
				default:
					/* continue without validation */
					break;
				}
			}
#endif
		} else if (op == NC_OP_COPYCONFIG) {
#ifndef DISABLE_URL
			if ((source_ds == NC_DATASTORE_URL) || (source_ds == NC_DATASTORE_CONFIG)) {
				/* if source is url, change source type to config */
				source_ds = NC_DATASTORE_CONFIG;
				if (target_ds == NC_DATASTORE_URL) {
					/* if target is url, prepare document content */
					data = config;
					config = NULL;
					if (asprintf(&config, "<?xml version=\"1.0\"?><config xmlns=\""NC_NS_BASE10"\">%s</config>", data) == -1) {
						ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
						e = nc_err_new(NC_ERR_OP_FAILED);
						nc_err_set(e, NC_ERR_PARAM_MSG, "libnetconf server internal error, see error log.");
						free(data);
						break; /* main switch */
					}
					free(data);
				}
			}
			if (target_ds == NC_DATASTORE_URL && nc_cpblts_enabled(session, NC_CAP_URL_ID)) {
				/* get target url */
				url_path = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/*/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":url", rpc->ctxt);
				if (url_path == NULL || xmlXPathNodeSetIsEmpty(url_path->nodesetval)) {
					ERROR("%s: unable to get URL path from <copy-config> request.", __func__);
					e = nc_err_new(NC_ERR_BAD_ELEM);
					nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "target");
					nc_err_set(e, NC_ERR_PARAM_MSG, "Unable to get URL path from the <copy-config> request.");
					xmlXPathFreeObject(url_path);
					break; /* main switch */
				}
				url = xmlNodeGetContent(url_path->nodesetval->nodeTab[0]);
				xmlXPathFreeObject(url_path);

				protocol = nc_url_get_protocol((char*)url);
				if (protocol == 0 || !nc_url_is_enabled(protocol)) {
					ERROR("%s: protocol (%s - %d) not supported", __func__, url, protocol);
					e = nc_err_new(NC_ERR_OP_FAILED);
					nc_err_set(e, NC_ERR_PARAM_MSG, "Specified URL protocol not supported.");
					xmlFree(url);
					break; /* main switch */
				}

				switch (source_ds) {
				case NC_DATASTORE_CONFIG:
					/* source datastore is config (or url), so just upload file */
					ret = nc_url_upload(config, (char*)url, &e);
					break;
				case NC_DATASTORE_RUNNING:
				case NC_DATASTORE_STARTUP:
				case NC_DATASTORE_CANDIDATE:
					/* Woodoo magic.
					 * If target is URL we have problem, because ncds_apply_rpc2all is calling ncds_apply_rpc for
					 * each datastore -> remote file would be overwriten everytime. So solution is to download
					 * remote file, make document from it and add current datastore configuration data to documtent and
					 * then upload it. Problem is if remote file is not empty (it contains data from datastores we does not have).
					 * Then data would merge and we will have merged wanted data with non-wanted data from remote file before editing.
					 * Thats FEATURE, not bug!!!. I recommend to call ncds_apply_rpc2all and before that use delete-config on remote file.
					 */
					url_tmpfile = -1;
					if (nc_url_check((char*)url) == 0) {
						url_tmpfile = nc_url_open((char*) url);
						/* check that the file has some content */
						if (read(url_tmpfile, &url_test_empty, 1) <= 0) {
							close(url_tmpfile);
							url_tmpfile = -1;
						} else {
							/* go back to the beginning of the file */
							lseek(url_tmpfile, 0, SEEK_SET);
						}
					}
					if (url_tmpfile == -1) {
						/*
						 * remote file is empty or does not exists,
						 * so create empty document with <config> root element
						 */
						doc1 = xmlNewDoc(BAD_CAST "1.0");
						root = xmlNewNode(NULL, BAD_CAST "config");
						ns = xmlNewNs(root, BAD_CAST NC_NS_BASE10, NULL);
						xmlSetNs(root, ns);
						xmlDocSetRootElement(doc1, root);
					} else {
						/* read content of the remote file */
						if ((doc1 = xmlReadFd(url_tmpfile, NULL, NULL, NC_XMLREAD_OPTIONS)) == NULL) {
							close(url_tmpfile);
							ERROR("%s: error reading XML data from the URL file", __func__);
							e = nc_err_new(NC_ERR_OP_FAILED);
							nc_err_set(e, NC_ERR_PARAM_MSG, "libnetconf internal server error, see error log.");
							break;
						}
						close(url_tmpfile);
						root = xmlDocGetRootElement(doc1);
						if (xmlStrcmp(BAD_CAST "config", root->name) != 0) {
							ERROR("%s: no config data in remote file (%s)", __func__, url);
							e = nc_err_new(NC_ERR_OP_FAILED);
							nc_err_set(e, NC_ERR_PARAM_MSG, "Invalid remote configuration file, missing top level <config> element.");
							break;
						}

						for (node = root->children; node != NULL; node = aux_node) {
							aux_node = node->next;
							if (node->type != XML_ELEMENT_NODE) {
								continue;
							}

							if (is_model_root(node, ds->data_model)){
								xmlUnlinkNode(node);
								xmlFreeNode(node);
							}
						}
					}

					data = ds->func.getconfig(ds, session, source_ds, &e);
					if (data == NULL ) {
						if (e == NULL ) {
							ERROR("%s: Failed to get data from the datastore (%s:%d).", __func__, __FILE__, __LINE__);
							e = nc_err_new(NC_ERR_OP_FAILED);
						}
						xmlFreeDoc(doc1);
						break;
					}
					doc2 = read_datastore_data(ds->id, data);
					free(data);
					if (doc2 == NULL) {
						if (e == NULL ) {
							ERROR("%s: Unable to process datastore data (%s:%d).", __func__, __FILE__, __LINE__);
							e = nc_err_new(NC_ERR_OP_FAILED);
						}
						xmlFreeDoc(doc1);
						break;
					}

					/* copy local data to "remote" document */
					xmlAddChildList(root, xmlCopyNodeList(doc2->children));

					xmlDocDumpFormatMemory(doc1, (xmlChar**) (&data), NULL, 1);
					nc_url_upload(data, (char*) url, &e);
					free(data);
					xmlFreeDoc(doc1);
					xmlFreeDoc(doc2);
					break;
				default:
					ERROR("%s: invalid source datastore for URL target", __func__);
					e = nc_err_new(NC_ERR_BAD_ELEM);
					nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "source");
					nc_err_set(e, NC_ERR_PARAM_MSG, "Invalid source element value for use with URL target.");
					break;
				}
				xmlFree(url);

				if (e == NULL) {
					ret = EXIT_SUCCESS;
				} else {
					free(config);
					break; /* main switch */
				}
			} else {
#else
			{
#endif /* DISABLE_URL */
				ret = ds->func.copyconfig(ds, session, rpc, target_ds, source_ds, config, &e);
			}
		} else {
			ret = EXIT_FAILURE;
		}
		free(config);

		break;
	case NC_OP_DELETECONFIG:
		if (ds->type == NCDS_TYPE_EMPTY) {
			/* there is nothing to edit in empty datastore type */
			ret = EXIT_RPC_NOT_APPLICABLE;
			break;
		}

		if (nc_rpc_get_target(rpc) == NC_DATASTORE_RUNNING) {
			/* can not delete running */
			e = nc_err_new(NC_ERR_OP_FAILED);
			nc_err_set(e, NC_ERR_PARAM_MSG, "Cannot delete a running datastore.");
			break;
		}
		target_ds  = nc_rpc_get_target(rpc);
#ifndef DISABLE_URL
		if (target_ds == NC_DATASTORE_URL && nc_cpblts_enabled(session, NC_CAP_URL_ID)) {
			url_path = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_BASE10_ID":rpc/"NC_NS_BASE10_ID":delete-config/"NC_NS_BASE10_ID":target/"NC_NS_BASE10_ID":url", rpc->ctxt);
			if (url_path == NULL || xmlXPathNodeSetIsEmpty(url_path->nodesetval)) {
				ERROR("%s: unable to get URL path from <delete-config> request.", __func__);
				e = nc_err_new(NC_ERR_BAD_ELEM);
				nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "target");
				nc_err_set(e, NC_ERR_PARAM_MSG, "Unable to get URL path from the <delete-config> request.");
				xmlXPathFreeObject(url_path);
				ret = EXIT_FAILURE;
				break;
			}
			url = xmlNodeGetContent(url_path->nodesetval->nodeTab[0]);
			xmlXPathFreeObject(url_path);

			protocol = nc_url_get_protocol((char*) url);
			if (protocol == 0 || !nc_url_is_enabled(protocol)) {
				ERROR("%s: protocol (%s - %d) not supported", __func__, url, protocol);
				e = nc_err_new(NC_ERR_OP_FAILED);
				nc_err_set(e, NC_ERR_PARAM_MSG, "Specified URL protocol not supported.");
				xmlFree(url);
				break; /* main switch */
			}

			ret = nc_url_delete_config((char*) url, &e);
			xmlFree(url);
		} else {
#else
		{
#endif /* DISABLE_URL */
			ret = ds->func.deleteconfig(ds, session, target_ds, &e);
		}

		break;
	case NC_OP_COMMIT:
		if (ds->type == NCDS_TYPE_EMPTY) {
			/* there is nothing to edit in empty datastore type */
			ret = EXIT_RPC_NOT_APPLICABLE;
			break;
		}

		if (nc_cpblts_enabled (session, NC_CAP_CANDIDATE_ID)) {
			ret = ds->func.copyconfig (ds, session, rpc, NC_DATASTORE_RUNNING, NC_DATASTORE_CANDIDATE, NULL, &e);
		} else {
			e = nc_err_new (NC_ERR_OP_NOT_SUPPORTED);
			ret = EXIT_FAILURE;
		}
		break;
	case NC_OP_DISCARDCHANGES:
		if (ds->type == NCDS_TYPE_EMPTY) {
			/* there is nothing to edit in empty datastore type */
			ret = EXIT_RPC_NOT_APPLICABLE;
			break;
		}

		if (nc_cpblts_enabled (session, NC_CAP_CANDIDATE_ID)) {
			/* NACM - no datastore permissions are needed,
			 * so create a copy of the rpc and remove NACM structure
			 */
			rpc_aux = nc_msg_dup((struct nc_msg*)rpc);
			nacm_aux = rpc_aux->nacm;
			rpc_aux->nacm = NULL;
			ret = ds->func.copyconfig(ds, session, rpc_aux, NC_DATASTORE_CANDIDATE, NC_DATASTORE_RUNNING, NULL, &e);
			rpc_aux->nacm = nacm_aux;
			nc_rpc_free(rpc_aux);
		} else {
			e = nc_err_new (NC_ERR_OP_NOT_SUPPORTED);
			ret = EXIT_FAILURE;
		}
		break;
	case NC_OP_GETSCHEMA:
		data_ns = NC_NS_MONITORING;
		if (nc_cpblts_enabled (session, NC_CAP_MONITORING_ID)) {
			if (dsid == NCDS_INTERNAL_ID) {
				if ((data = get_schema (rpc, &e)) == NULL) {
					ret = EXIT_FAILURE;
				} else {
					reply = nc_reply_data_ns(data, data_ns);
					free(data);
				}
			} else {
				doc_merged = xmlNewDoc(BAD_CAST "1.0");
				ret = EXIT_SUCCESS;
			}
		} else {
			e = nc_err_new (NC_ERR_OP_NOT_SUPPORTED);
			ret = EXIT_FAILURE;
		}
		break;
#ifndef DISABLE_VALIDATION
	case NC_OP_VALIDATE:
		ret = apply_rpc_validate(ds, session, rpc, &e);
		break;
#endif
	case NC_OP_UNKNOWN:
		/* get operation name */
		op_name = nc_rpc_get_op_name (rpc);
		/* prepare for case RPC is not supported by this datastore */
		reply = NCDS_RPC_NOT_APPLICABLE;
		/* go through all RPC implemented by datastore's transAPI modules */
		for (tapi_iter = ds->transapis; tapi_iter != NULL; tapi_iter = tapi_iter->next) {
			for (i = 0; i < tapi_iter->tapi->rpc_clbks->callbacks_count; i++) {
				/* find matching rpc and call rpc callback function */
				rpc_name = tapi_iter->tapi->rpc_clbks->callbacks[i].name;
				if (strcmp(op_name, rpc_name) == 0) {
					/* get operation node */
					op_node = ncxml_rpc_get_op_content(rpc);
					op_input = xmlCopyNodeList(op_node->children);
					xmlFreeNode(op_node);

					/* call RPC callback function */
					VERB("Calling %s RPC function\n", rpc_name);
					reply = tapi_iter->tapi->rpc_clbks->callbacks[i].func(op_input);
					xmlFreeNodeList(op_input);

					/* end RPC search, there can be only one RPC with name == op_name */
					break;
				}
			}
			if (i >= tapi_iter->tapi->rpc_clbks->callbacks_count) {
				/* propagate break to outer for loop */
				break;
			}
		}

		free(op_name);
		break;
	default:
		ERROR("%s: unsupported NETCONF operation requested.", __func__);
		pthread_mutex_unlock(&ds->lock);
		return (nc_reply_error (nc_err_new (NC_ERR_OP_NOT_SUPPORTED)));
		break;
	}

	/*
	 * remove various unneeded variables from the switch
	 */
	/* filter from <get> and <get-config> */
	if (shared_filter == NULL) {
		/* filter is not shared, free it */
		nc_filter_free(filter);
	}

	/* if reply was not already created */
	if (reply == NULL) {
		if (e != NULL) {
			/* operation failed and error is filled */
			reply = nc_reply_error(e);
		} else if (doc_merged == NULL && ret != EXIT_SUCCESS) {
			if (ret == EXIT_RPC_NOT_APPLICABLE) {
				/* operation can not be performed on this datastore */
				reply = NCDS_RPC_NOT_APPLICABLE;
			} else {
				/* operation failed, but no additional information is provided */
				reply = nc_reply_error(nc_err_new(NC_ERR_OP_FAILED));
			}
		} else {
			if (doc_merged != NULL) {
				if (data_ns != NULL) {
					reply = ncxml_reply_data_ns(doc_merged->children, data_ns);
				} else {
					reply = ncxml_reply_data(doc_merged->children);
				}
				xmlFreeDoc(doc_merged);
				if (!reply) {
					return nc_reply_error(nc_err_new(NC_ERR_OP_FAILED));
				}
			} else {
				reply = nc_reply_ok();
			}
		}
	}

	/* if transapi used, rpc affected running and succeeded get its actual content */
	/*
	 * skip transapi if <edit-config> was performed with test-option set
	 * to test-only value
	 */
	if (ds->transapis != NULL && ds->tapi_callbacks_count
		&& (op == NC_OP_COMMIT || op == NC_OP_COPYCONFIG || (op == NC_OP_EDITCONFIG && (nc_rpc_get_testopt(rpc) != NC_EDIT_TESTOPT_TEST))) &&
		(nc_rpc_get_target(rpc) == NC_DATASTORE_RUNNING && nc_reply_get_type(reply) == NC_REPLY_OK)) {

		if (op == NC_OP_EDITCONFIG) {
			erropt = nc_rpc_get_erropt(rpc);
		} else { /* commit or copy-config */
			/* try rollback to keep transactions atomic */
			erropt = NC_EDIT_ERROPT_ROLLBACK;
		}

		if ((new_reply = ncds_apply_transapi(ds, session, old, erropt, NULL)) != NULL) {
			nc_reply_free(reply);
			reply = new_reply;
		}
	}
	xmlFreeDoc (old);
	old = NULL;

	pthread_mutex_unlock(&ds->lock);

	if (id == NCDS_INTERNAL_ID) {
		if (old_reply == NULL) {
			old_reply = reply;
		} else if (old_reply != NCDS_RPC_NOT_APPLICABLE || reply != NCDS_RPC_NOT_APPLICABLE){
			if ((new_reply = nc_reply_merge(2, old_reply, reply)) == NULL) {
				if (nc_reply_get_type(old_reply) == NC_REPLY_ERROR) {
					return (old_reply);
				} else if (nc_reply_get_type(reply) == NC_REPLY_ERROR) {
					return (reply);
				} else {
					return (nc_reply_error(nc_err_new(NC_ERR_OP_FAILED)));
				}
			}
			old_reply = reply = new_reply;
		}
		dsid++;
		if (dsid < internal_ds_count) {
			e = NULL;
			reply = NULL;
			goto process_datastore;
		}
	}

	return (reply);
}

static char* serialize_cpblts(const struct nc_cpblts *capabilities)
{
	char *aux = NULL, *retval = NULL;
	int i;

	if (capabilities == NULL) {
		return (NULL);
	}

	for (i = 0; i < capabilities->items; i++) {
		if (asprintf(&retval, "%s<capability>%s</capability>",
				(aux == NULL) ? "" : aux,
				capabilities->list[i]) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			continue;
		}
		free(aux);
		aux = retval;
		retval = NULL;
	}
	if (asprintf(&retval, "<capabilities>%s</capabilities>", aux) == -1) {
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		retval = NULL;
	}
	free(aux);
	return(retval);
}

API nc_reply* ncds_apply_rpc2all(struct nc_session* session, const nc_rpc* rpc, ncds_id* ids[])
{
	struct ncds_ds_list* ds, *ds_rollback;
	nc_reply *old_reply = NULL, *new_reply = NULL, *reply = NULL;
	int id_i = 0, transapi = 0;
	char *op_name, *op_namespace, *data;
	xmlDocPtr old;
	NC_OP op;
	NC_DATASTORE target;
	NC_EDIT_ERROPT_TYPE erropt = NC_EDIT_ERROPT_NOTSET;
	NC_RPC_TYPE req_type;
	struct nc_err *e = NULL;
	struct nc_filter *shared_filter = NULL;

	if (rpc == NULL || session == NULL) {
		ERROR("%s: invalid parameter %s", __func__, (rpc==NULL)?"rpc":"session");
		return (NULL);
	}

	/* check that we have a valid definition of the requested RPC */
	op_name = nc_rpc_get_op_name(rpc);
	op_namespace = nc_rpc_get_op_namespace(rpc);
	if (ncds_get_model_operation(op_name, op_namespace) == NULL) {
		/* rpc operation is not defined in any known module */
		ERROR("%s: unsupported NETCONF operation (%s) requested.", __func__, op_name);
		free(op_name);
		free(op_namespace);
		return (nc_reply_error(nc_err_new (NC_ERR_OP_NOT_SUPPORTED)));
	}
	free(op_namespace);
	free(op_name);

	if (ids != NULL) {
		*ids = ncds.datastores_ids;
	}

	/* get flags and data for the following loop */
	req_type = nc_rpc_get_type(rpc);
	op = nc_rpc_get_op(rpc);
	switch (op) {
	case NC_OP_EDITCONFIG:
		erropt = nc_rpc_get_erropt(rpc);
		break;
	case NC_OP_GET:
		data = serialize_cpblts(session->capabilities);
                //pthread_spin_lock(&server_cpblt_lock);
		server_capabilities = data;
                //pthread_spin_unlock(&server_cpblt_lock);
		/* no break */
	case NC_OP_GETCONFIG:
		shared_filter = nc_rpc_get_filter(rpc);
		break;
	default:
		/* do nothing */
		break;
	}

	for (ds = ncds.datastores; ds != NULL; ds = ds->next) {
		/* skip internal datastores */
		if (ds->datastore->id > 0 && ds->datastore->id < internal_ds_count) {
			continue;
		}

		/* apply RPC on a single datastore */
		reply = ncds_apply_rpc(ds->datastore->id, session, rpc, shared_filter);
		if (ids != NULL && reply != NCDS_RPC_NOT_APPLICABLE) {
			ncds.datastores_ids[id_i] = ds->datastore->id;
			id_i++;
			ncds.datastores_ids[id_i] = -1; /* terminating item */
		}

		/* merge results from the previous runs */
		if (old_reply == NULL) {
			old_reply = reply;
		} else if (old_reply != NCDS_RPC_NOT_APPLICABLE || reply != NCDS_RPC_NOT_APPLICABLE) {
			if ((new_reply = nc_reply_merge(2, old_reply, reply)) == NULL) {
				nc_filter_free(shared_filter);
				shared_filter = NULL;
                                //pthread_spin_lock(&server_cpblt_lock);
				free(server_capabilities);
				server_capabilities = NULL;
                                //pthread_spin_unlock(&server_cpblt_lock);

				if (nc_reply_get_type(old_reply) == NC_REPLY_ERROR) {
					return (old_reply);
				} else if (nc_reply_get_type(reply) == NC_REPLY_ERROR) {
					return (reply);
				} else {
					return (nc_reply_error(nc_err_new(NC_ERR_OP_FAILED)));
				}
			}
			old_reply = reply = new_reply;
		}

		if (reply != NCDS_RPC_NOT_APPLICABLE && nc_reply_get_type(reply) == NC_REPLY_ERROR) {
			if (req_type == NC_RPC_DATASTORE_WRITE) {
				if (erropt == NC_EDIT_ERROPT_NOTSET || erropt == NC_EDIT_ERROPT_STOP) {
					goto cleanup;
				} else if (erropt == NC_EDIT_ERROPT_ROLLBACK) {
					/* rollback previously changed datastores */
					/* do not skip internal datastores */
					target = nc_rpc_get_target(rpc);
					for (ds_rollback = ncds.datastores; ds_rollback != ds; ds_rollback = ds_rollback->next) {
						if (ds_rollback->datastore->transapis != NULL && ds_rollback->datastore->tapi_callbacks_count
								&& (op == NC_OP_COMMIT || op == NC_OP_COPYCONFIG || (op == NC_OP_EDITCONFIG && (nc_rpc_get_testopt(rpc) != NC_EDIT_TESTOPT_TEST)))
								&& ( target == NC_DATASTORE_RUNNING)) {
							transapi = 1;
						} else {
							transapi = 0;
						}

						if (transapi) {
							/* remeber data for transAPI diff */
							data = ds_rollback->datastore->func.getconfig(ds_rollback->datastore, session, NC_DATASTORE_RUNNING, &e);
							nc_err_free(e);
							e = NULL;

							old = read_datastore_data(ds_rollback->datastore->id, data);
							free(data);
						}

						ds_rollback->datastore->func.rollback(ds_rollback->datastore);

						/* transAPI rollback */
						if (transapi) {
							reply = ncds_apply_transapi(ds_rollback->datastore, session, old, erropt, reply);
							xmlFreeDoc(old);
						}

					}
					goto cleanup;
				} /* else if (erropt == NC_EDIT_ERROPT_CONT)
				   * just continue
				   */
			} else if (req_type == NC_RPC_DATASTORE_READ) {
				goto cleanup;
			}
		}
	}

#ifndef DISABLE_NOTIFICATIONS
	if (op == NC_OP_EDITCONFIG || op == NC_OP_COPYCONFIG || op == NC_OP_DELETECONFIG || op == NC_OP_COMMIT) {
		/* log the event */
		target = nc_rpc_get_target(rpc);
		if (nc_reply_get_type(reply) == NC_REPLY_OK && (target == NC_DATASTORE_RUNNING || target == NC_DATASTORE_STARTUP)) {
			ncntf_event_new(-1, NCNTF_BASE_CFG_CHANGE, target, NCNTF_EVENT_BY_USER, session);
		}
	}
#endif /* DISABLE_NOTIFICATIONS */

cleanup:
	/* clean up the common data for calling nc_apply_rpc() */
	nc_filter_free(shared_filter);
	shared_filter = NULL;

        //pthread_spin_lock(&server_cpblt_lock);
	free(server_capabilities);
	server_capabilities = NULL;
        //pthread_spin_unlock(&server_cpblt_lock);

	return (reply);
}

API void ncds_break_locks(const struct nc_session* session)
{
	struct ncds_ds_list * ds;
	struct nc_err * e = NULL;
	/* maximum is 3 locks (one for every datastore type) */
	struct nc_session * sessions[3];
	const struct ncds_lockinfo * lockinfo;
	int number_sessions = 0, i, j;
	NC_DATASTORE ds_type[3] = {NC_DATASTORE_CANDIDATE, NC_DATASTORE_RUNNING, NC_DATASTORE_STARTUP};
	struct nc_cpblts * cpblts;

#ifndef DISABLE_NOTIFICATIONS
	char *ds_name, *data = NULL;
	int *flag, flag_r, flag_s, flag_c;
#endif

	if (session == NULL) {
		/* if session NULL, get all sessions that hold lock from first file datastore */
		ds = ncds.datastores;
		/* find first file datastore */
		while (ds != NULL && ds->datastore != NULL && ds->datastore->type != NCDS_TYPE_FILE) {
			ds = ds->next;
		}
		if (ds != NULL) {
			/* if there is one */
			/* get default capabilities for dummy sessions */
			cpblts = nc_session_get_cpblts_default();
			for (i=0; i<3; i++) {
				if ((lockinfo = ncds_file_lockinfo (ds->datastore, ds_type[i])) != NULL) {
					if (lockinfo->sid != NULL && strcmp (lockinfo->sid, "") != 0) {
						/* create dummy session with session ID used to lock datastore */
						sessions[number_sessions++] = nc_session_dummy(lockinfo->sid, "dummy", NULL, cpblts);
					}
				}
			}
			nc_cpblts_free(cpblts);
		}
	} else {
		/* plain old single session break locks */
		number_sessions = 1;
		sessions[0] = (struct nc_session*)session;
	}

	/* for all prepared sessions */
	for (i=0; i<number_sessions; i++) {
		ds = ncds.datastores;
#ifndef DISABLE_NOTIFICATIONS
		flag_r = 0;
		flag_s = 0;
		flag_c = 0;
#endif /* DISABLE_NOTIFICATIONS */
		/* every datastore */
		while (ds) {
			if (ds->datastore && ds->datastore->type != NCDS_TYPE_EMPTY) {
				/* and every datastore type */
				for (j=0; j<3; j++) {
					/* try to unlock datastore */
					ds->datastore->func.unlock(ds->datastore, sessions[i], ds_type[j], &e);
					if (e) {
						nc_err_free(e);
						e = NULL;
#ifndef DISABLE_NOTIFICATIONS
					} else {
						/* log the event */
						if (ds->datastore->type == NCDS_TYPE_FILE) {
							switch (ds_type[j]) {
							case NC_DATASTORE_RUNNING:
								ds_name = "running";
								flag = &flag_r;
								break;
							case NC_DATASTORE_CANDIDATE:
								ds_name = "candidate";
								flag = &flag_c;
								break;
							case NC_DATASTORE_STARTUP:
								ds_name = "startup";
								flag = &flag_s;
								break;
							default:
								/* wtf, (un)lock had to fail already */
								flag = NULL;
							}

							if (flag && !(*flag)) {
								if (asprintf(&data, "<datastore-unlock xmlns=\"%s\"><datastore>%s</datastore><session-id>%s</session-id></datastore-unlock>",
										NC_NS_LNC_NOTIFICATIONS, ds_name, session->session_id) == -1) {
									ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
									ERROR("Generating datastore-unlock event failed.");
								} else {
									ncntf_event_new(-1, NCNTF_GENERIC, data);
									free(data);
									data = NULL;
								}
								*flag = 1;
							}
						}
#endif /* DISABLE_NOTIFICATIONS */
					}
				}
			}
			ds = ds->next;
#ifndef DISABLE_NOTIFICATIONS
			flag = 0;
#endif /* DISABLE_NOTIFICATIONS */
		}
	}

	/* clean created dummy sessions */
	if (session == NULL) {
		for (i=0; i<number_sessions; i++) {
			nc_session_free(sessions[i]);
		}
	}

	return;
}

const struct data_model* ncds_get_model_data(const char* namespace)
{
	struct model_list* listitem;
	struct data_model *model = NULL;

	if (namespace == NULL) {
		return (NULL);
	}

	for (listitem = models_list; listitem != NULL; listitem = listitem->next) {
		if (listitem->model->ns != NULL && strcmp(listitem->model->ns, namespace) == 0) {
			/* namespace matches */
			model = listitem->model;
			break;
		}
	}

	if (model != NULL) {
		/* model found */
		return (model);
	}

	/* model not found */
	return (NULL);
}

const struct data_model* ncds_get_model_operation(const char* operation, const char* namespace)
{
	const struct data_model *model = NULL;
	int i;

	if (operation == NULL || namespace == NULL) {
		return (NULL);
	}

	model = ncds_get_model_data(namespace);
	if (model != NULL && model->rpcs != NULL ) {
		for (i = 0; model->rpcs[i] != NULL ; i++) {
			if (strcmp(model->rpcs[i], operation) == 0) {
				/* operation definition found */
				return (model);
			}
		}
	}

	/* oepration definition not found */
	return (NULL);
}

static int ncds_update_features()
{
	struct model_list* listitem;
	xmlNodePtr node, next;
	struct ncds_ds_list *ds_iter;

	for (listitem = models_list; listitem != NULL; listitem = listitem->next){
		for (node = xmlDocGetRootElement(listitem->model->xml)->children; node != NULL; node = next) {
			next = node->next;
			if (feature_check(node, listitem->model) == 1) {
				/* remove the node */
				xmlUnlinkNode(node);
				xmlFreeNode(node);
			}
		}
	}

	for (ds_iter = ncds.datastores; ds_iter != NULL; ds_iter = ds_iter->next){
		if (ds_iter->datastore->ext_model == ds_iter->datastore->data_model->xml){
			ds_iter->datastore->ext_model = xmlCopyDoc(ds_iter->datastore->data_model->xml, 1);
		}

		for (node = xmlDocGetRootElement(ds_iter->datastore->ext_model)->children; node != NULL; node = next) {
			next = node->next;
			if (feature_check(node, ds_iter->datastore->data_model) == 1) {
				/* remove the node */
				xmlUnlinkNode(node);
				xmlFreeNode(node);
			}
		}
	}

	return (EXIT_SUCCESS);
}

/*
 *  1 - remove the node
 *  0 - do not remove the node
 * -1 - error
 */
static int feature_check(xmlNodePtr node, struct data_model* model)
{
	xmlNodePtr child, next;
	char* fname;
	char* name;
	char* prefix;
	char* feature_str;
	int i;

	struct model_feature **features = NULL;

	if (node == NULL || model == NULL) {
		ERROR("%s: invalid parameter.", __func__);
		return (-1);
	}

	for (child = node->children; child != NULL; child = child->next){
		if (child->type == XML_ELEMENT_NODE && xmlStrcmp(child->name, BAD_CAST "if-feature") == 0){
			if ((fname = (char*) xmlGetProp (child, BAD_CAST "name")) == NULL){
				WARN("Invalid if-feature statement");
				continue;
			}

			features = NULL;
			name = NULL;
			prefix = NULL;
			feature_str = NULL;
			if ((name = strchr(fname, ':')) == NULL){
				feature_str = fname;
				features = model->features;
			}
			else{
				prefix = fname;
				name[0] = 0;
				feature_str = &(name[1]);
				features = get_features_from_prefix(model,prefix);
			}

			if (!(features == NULL || features[0] == NULL)){
				/* check if the feature is enabled or not */
				for (i = 0; features[i] != NULL; i++){
					if (strcmp(features[i]->name, feature_str) == 0){
						if (features[i]->enabled == 0){
							free(fname);
							/* remove the node */
							return (1);
						}
						break;
					}
				}
			}
			free(fname);
			/* ignore any following if-feature statements */
			break;
		}
	}

	/* recursion check */
	for (child = node->children; child != NULL; child = next){
		next = child->next;
		if (feature_check(child, model) == 1){
			/* remove the node */
			xmlUnlinkNode(child);
			xmlFreeNode(child);
		}
	}

	return (0);
}

static struct model_feature** get_features_from_prefix(struct data_model* model, char* prefix)
{
	char* import_model_str = NULL;
	xmlXPathObjectPtr imports = NULL;
	struct data_model* import_model = NULL;

	if (prefix == NULL || model == NULL){
		ERROR("%s: invalid parameter.", __func__);
		return NULL;
	}

	if (strcmp(prefix, model->prefix) == 0){
		return model->features;
	}
	else{
		/* get all <import> nodes for their prefix specification to be used with augment statement */
		if ((imports = xmlXPathEvalExpression(BAD_CAST "/"NC_NS_YIN_ID":module/"NC_NS_YIN_ID":import", model->ctxt)) == NULL ){
			ERROR("%s: Evaluating XPath expression failed.", __func__);
			return NULL;
		}

		/* find the prefix in imports */
		import_model_str = get_module_with_prefix(prefix, imports);
		xmlXPathFreeObject(imports);
		if (import_model_str == NULL ){
			/* unknown name of the import model */
			return (NULL);
		}

		import_model = get_model(import_model_str, NULL);
		free(import_model_str);
		if(import_model == NULL){
			return NULL;
		}
		return import_model->features;
	}
}

const struct data_model* ncds_get_model_notification(const char* notification, const char* namespace)
{
	const struct data_model *model = NULL;
	int i;

	if (notification == NULL || namespace == NULL) {
		return (NULL);
	}

	model = ncds_get_model_data(namespace);
	if (model != NULL && model->notifs != NULL ) {
		for (i = 0; model->notifs[i] != NULL ; i++) {
			if (strcmp(model->notifs[i], notification) == 0) {
				/* notification definition found */
				return (model);
			}
		}
	}

	/* notification definition not found */
	return (NULL);
}
