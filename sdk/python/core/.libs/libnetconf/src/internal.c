/**
 * \file internal.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of internal libnetconf's functions.
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

#define _BSD_SOURCE
#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <grp.h>
#include <pthread.h>
#include <pwd.h>

#ifdef POSIX_SHM
#	include <sys/mman.h>
#	define NC_POSIX_SHM_OBJECT "libnetconfshm"
#else
#	include <sys/shm.h>
#endif

#include <libxslt/xslt.h>
#include <libxml/parser.h>

#include "netconf_internal.h"
#include "nacm.h"
#include "datastore/file/datastore_file.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

/* defined in datastore.c */
int ncds_sysinit(int flags);
void ncds_startup_internal(void);

volatile uint8_t verbose_level = 0;

/* this instance is running as first after reboot or system-wide nc_close */
/* used in nc_device_init to decide if erase running or not */
int first_after_close = 0;

API void nc_verbosity(NC_VERB_LEVEL level)
{
	verbose_level = level;
}

static void prv_vprintf(NC_VERB_LEVEL level, const char *format, va_list args)
{
#define PRV_MSG_SIZE 4096
	char prv_msg[PRV_MSG_SIZE];

	if (callbacks.print != NULL) {
		vsnprintf(prv_msg, PRV_MSG_SIZE - 1, format, args);
		prv_msg[PRV_MSG_SIZE - 1] = '\0';
		callbacks.print(level, prv_msg);

	}
#undef PRV_MSG_SIZE
}

void prv_printf(NC_VERB_LEVEL level, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	prv_vprintf(level, format, ap);
	va_end(ap);
}

API void nc_verb_verbose(const char *format, ...)
{
	va_list argptr;
	if (verbose_level >= NC_VERB_VERBOSE) {
		va_start(argptr, format);
		prv_vprintf(NC_VERB_VERBOSE, format, argptr);
		va_end(argptr);
	}
}

API void nc_verb_warning(const char *format, ...)
{
	va_list argptr;

	if (verbose_level >= NC_VERB_WARNING) {
		va_start(argptr, format);
		prv_vprintf(NC_VERB_WARNING, format, argptr);
		va_end(argptr);
	}
}

API void nc_verb_error(const char *format, ...)
{
	va_list argptr;

	va_start(argptr, format);
	prv_vprintf(NC_VERB_ERROR, format, argptr);
	va_end(argptr);
}

/*
 * thread specific flag for notifications dispatch functions
 */
static pthread_key_t ncntf_dispatch_key;
static pthread_once_t ncntf_dispatch_once = PTHREAD_ONCE_INIT;
int ncntf_dispatch_main = 0;

static void ncntf_dispatch_free(void *ptr)
{
#ifdef __linux__
	/* in __linux__ we use static memory in the main thread,
	 * so this check is for programs terminating the main()
	 * function by pthread_exit() :)
	 */
	if (ptr != &ncntf_dispatch_main) {
#else
	{
#endif
		free(ptr);
	}
}

static void ncntf_dispatch_createkey(void)
{
	int r;
	/* initiate */
	while ((r = pthread_key_create(&ncntf_dispatch_key, ncntf_dispatch_free)) == EAGAIN);
	pthread_setspecific(ncntf_dispatch_key, NULL);
}

int *ncntf_dispatch_location(void)
{
	int *flag;

	pthread_once(&ncntf_dispatch_once, ncntf_dispatch_createkey);
	flag = pthread_getspecific(ncntf_dispatch_key);
	if (!flag) {
		/* prepare ly_err storage */
#ifdef __linux__
		if (getpid() == syscall(SYS_gettid)) {
			/* main thread - use global variable instead of thread-specific variable. */
			flag = &ncntf_dispatch_main;
		} else {
#else
		{
#endif /* __linux__ */
			flag = calloc(1, sizeof *flag);
		}
		pthread_setspecific(ncntf_dispatch_key, flag);
	}

	return flag;
}

struct nc_shared_info *nc_info = NULL;
#ifndef POSIX_SHM
static int shmid = -1;
#endif

int nc_init_flags = 0;

static void nc_apps_add(const char* comm, struct nc_apps* apps) {
	int i;

	if (comm[0] == '\0') {
		return;
	}

	for (i = 0; i < NC_APPS_MAX; ++i) {
		if (apps->valid[i] == 0) {
			break;
		}
	}

	if (i == NC_APPS_MAX) {
		VERB("Too many running/crashed libnetconf apps.");
		return;
	}

	apps->valid[i] = 1;
	apps->pids[i] = getpid();
	strcpy(apps->comms[i], comm);
}

/* return flags - 1 (your entry was found and deleted (meaning you crashed, unless used in nc_close),
 * 2 (there are actually some libnetconf apps running, not just stored crash info) */
static int nc_apps_check(const char* comm, struct nc_apps* apps) {
	int i, j = 0, ret = 0, fd = -1, readcount;
	char procpath[64], runcomm[NC_APPS_COMM_MAX+1];

	for (i = 0; i < NC_APPS_MAX; ++i) {
		if (apps->valid[i] == 0) {
			continue;
		}

		if (sprintf(procpath, "/proc/%d/comm", apps->pids[i]) == -1 || ((fd = open(procpath, O_RDONLY)) == -1 && errno != ENOENT)) {
			/* not much to do on error */
			continue;
		}

		/* the process is not running - it crashed */
		if (fd == -1) {
			/* it was this process */
			if (strcmp(apps->comms[i], comm) == 0) {
				ret |= 1;
				j = i;
			}
			continue;
		}

		readcount = read(fd, runcomm, NC_APPS_COMM_MAX);
		close(fd);
		if (readcount < 0) {
			continue;
		}
		if (runcomm[readcount-1] == '\n') {
			runcomm[readcount-1] = '\0';
		} else {
			runcomm[readcount] = '\0';
		}

		/* PID got recycled - the process crashed */
		if (strcmp(runcomm, apps->comms[i]) != 0) {
			/* it was this process */
			if (strcmp(apps->comms[i], comm) == 0) {
				ret |= 1;
				j = i;
			}
			continue;
		}

		/* a process with the same PID and command did not use nc_close(),
		 * it must have been this process, because it did not add its
		 * information into this structure yet
		 */
		if (strcmp(comm, apps->comms[i]) == 0 && getpid() == apps->pids[i]) {
			ret |= 1;
			j = i;
			continue;
		}

		/* we know that the process is running, so just remember that
		 * there is a valid running libnetconf application
		 */
		ret |= 2;
	}

	if (ret & 1) {
		apps->valid[j] = 0;
	}

	return ret;
}

static int nc_shared_cleanup(int del_shm) {
	char path[256], lock_prefix[32];
	struct dirent* dr;
	DIR* dir;

#ifndef POSIX_SHM
	struct shmid_ds ds;
#endif

	/* remove the global session information file */
	if (unlink(NC_SESSIONSFILE) == -1 && errno != ENOENT) {
		ERROR("Unable to remove the session information file (%s)", strerror(errno));
		return (-1);
	}

	/* remove semaphores */
	strcpy(lock_prefix, NCDS_LOCK);
	memmove(lock_prefix+4, lock_prefix+1, strlen(lock_prefix));
	memcpy(lock_prefix, "sem.", 4);

	if ((dir = opendir("/dev/shm")) == NULL) {
		/* let's just ignore this fail */
		DBG("Failed to open semaphore directory \"/dev/shm\" (%s).", strerror(errno));
	} else {
		while ((dr = readdir(dir))) {
			if (strncmp(dr->d_name, lock_prefix, strlen(lock_prefix)) == 0) {
				sprintf(path, "/dev/shm/%s", dr->d_name);
				if (unlink(path) == -1) {
					DBG("Failed to remove semaphore \"%s\" (%s).", path, strerror(errno));
				}
			}
		}
		closedir(dir);
	}

	/* remove shared memory */
	if (del_shm && nc_info) {
#ifndef POSIX_SHM
		if (shmctl(shmid, IPC_STAT, &ds) == -1) {
			ERROR("Unable to get the status of shared memory (%s).", strerror(errno));
			return (-1);
		}
		if (ds.shm_nattch == 1 && (nc_init_flags & NC_INIT_MULTILAYER)) {
			shmctl(shmid, IPC_RMID, NULL);
		} else {
			return (1);
		}
#else
		if (nc_init_flags & NC_INIT_MULTILAYER) {
			shm_unlink(NC_POSIX_SHM_OBJECT);
		}
#endif /* #ifndef POSIX_SHM */
	}

	return (0);
}

API int nc_init(int flags)
{
	int retval = 0, r, init_shm = 1, fd;
	char* t, my_comm[NC_APPS_COMM_MAX+1];
	pthread_rwlockattr_t rwlockattr;
	mode_t mask;
#ifndef POSIX_SHM
	key_t key = -4;
#endif

	if (nc_init_flags & NC_INIT_DONE) {
		ERROR("libnetconf already initiated!");
		return (-1);
	}

#ifndef DISABLE_LIBSSH
	if (flags & NC_INIT_LIBSSH_PTHREAD) {
		ssh_threads_set_callbacks(ssh_threads_get_pthread());
		ssh_init();
		nc_init_flags |= NC_INIT_LIBSSH_PTHREAD;
	}
#endif

	if (flags == NC_INIT_CLIENT) {
		nc_init_flags |= NC_INIT_CLIENT;
		return (retval);
	}

	if ((flags & (NC_INIT_MULTILAYER | NC_INIT_SINGLELAYER)) != NC_INIT_MULTILAYER &&
			(flags & (NC_INIT_MULTILAYER | NC_INIT_SINGLELAYER)) != NC_INIT_SINGLELAYER) {
		ERROR("Either single-layer or multi-layer flag must be used in initialization.");
		return (-1);
	}

	/* some flags need other flags, so check that all dependencies are fullfilled */
	if (flags & NC_INIT_NACM) {
		flags |= NC_INIT_DATASTORES;
	}
	if (flags & NC_INIT_KEEPALIVECHECK) {
		flags |= NC_INIT_MONITORING;
	}

	if (flags & (NC_INIT_DATASTORES | NC_INIT_MONITORING | NC_INIT_NACM)) {
#ifndef POSIX_SHM
		DBG("Shared memory key: %d", key);
		mask = umask(MASK_PERM);
		shmid = shmget(key, sizeof(struct nc_shared_info), IPC_CREAT | IPC_EXCL | FILE_PERM);
		umask(mask);
		if (shmid == -1) {
			if (errno == EEXIST) {
				shmid = shmget(key, sizeof(struct nc_shared_info), 0);
				init_shm = 0;
			}
			if (shmid == -1) {
				ERROR("Accessing System V shared memory failed (%s).", strerror(errno));
				return (-1);
			}
		}
		DBG("Shared memory ID: %d", shmid);

		/* attach memory */
		nc_info = shmat(shmid, NULL, 0);
		if (nc_info == (void*) -1) {
			ERROR("Attaching System V shared memory failed (%s). You can try removing the memory by \"ipcrm -m %d\".", strerror(errno), shmid);
			nc_info = NULL;
			return (-1);
		}

#else

		DBG("Shared memory location: /dev/shm/"NC_POSIX_SHM_OBJECT);
		mask = umask(MASK_PERM);
		fd = shm_open(NC_POSIX_SHM_OBJECT, O_CREAT | O_EXCL | O_RDWR, FILE_PERM);
		umask(mask);
		if (fd == -1) {
			if (errno == EEXIST) {
				DBG("Shared memory file %s already exists - opening", NC_POSIX_SHM_OBJECT);
				fd = shm_open(NC_POSIX_SHM_OBJECT, O_RDWR, 0);
				init_shm = 0;
			}
			if (fd == -1) {
				ERROR("Accessing POSIX shared memory failed (%s).", strerror(errno));
				return (-1);
			}
		}
		DBG("POSIX SHM File Descriptor: %d (%dB).", fd, sizeof(struct nc_shared_info));

		if (ftruncate(fd,sizeof(struct nc_shared_info)) == -1 )  {
			ERROR("Truncating POSIX shared memory failed (%s).", strerror(errno));
			shm_unlink(NC_POSIX_SHM_OBJECT);
			return (-1);
		}

		nc_info = mmap(NULL, sizeof(struct nc_shared_info), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (nc_info == MAP_FAILED) {
			ERROR("Mapping POSIX shared memory failed (%s).", strerror(errno));
			shm_unlink(NC_POSIX_SHM_OBJECT);
			return (-1);
		}

#endif /* #ifndef POSIX_SHM */

		/* get my comm */
		my_comm[0] = '\0';
		fd = open("/proc/self/comm", O_RDONLY);
		if (fd != -1) {
			r = read(fd, my_comm, NC_APPS_COMM_MAX);
			close(fd);
			if (r > 0) {
				if (my_comm[r-1] == '\n') {
					my_comm[r-1] = '\0';
				} else {
					my_comm[r] = '\0';
				}
			}
		}

		if (init_shm) {
			/* we created the shared memory, consider first even for single-layer */
			first_after_close = 1;

			/* clear the apps structure */
			memset(nc_info->apps.valid, 0, NC_APPS_MAX * sizeof(unsigned char));

			/* set last session id */
			nc_info->last_session_id = 0;

			/* init lock */
			pthread_rwlockattr_init(&rwlockattr);
			pthread_rwlockattr_setpshared(&rwlockattr, PTHREAD_PROCESS_SHARED);
			if ((r = pthread_rwlock_init(&(nc_info->lock), &rwlockattr)) != 0) {
				ERROR("Shared information lock initialization failed (%s)", strerror(r));
#ifdef POSIX_SHM
				munmap(nc_info, sizeof(struct nc_shared_info));
				shm_unlink(NC_POSIX_SHM_OBJECT);
#else
				shmdt(nc_info);
#endif /* #ifdef POSIX_SHM */
				return (-1);
			}
			pthread_rwlockattr_destroy(&rwlockattr);

			/* LOCK */
			r = pthread_rwlock_wrlock(&(nc_info->lock));
			memset(nc_info->apps.valid, 0, NC_APPS_MAX * sizeof(unsigned char));
		} else {
			/* LOCK */
			r = pthread_rwlock_wrlock(&(nc_info->lock));

			/* check if I didn't crash before */
			r = nc_apps_check(my_comm, &(nc_info->apps));

			if (r & 1) {
				/* I crashed */
				retval |= NC_INITRET_RECOVERY;
				nc_info->stats.participants--;
			}

			if (r & 2) {
				/* shared memory existed and there are actually some running libnetconf apps */
				first_after_close = 0;
				retval |= NC_INITRET_NOTFIRST;
			} else if (flags & NC_INIT_MULTILAYER) {
				/* shared memory contained only crash info, multi-layer is considered first, ... */
				first_after_close = 1;
			} else {
				/* ... single-layer not */
				first_after_close = 0;
			}
		}

		if (first_after_close) {
			/* we are certain we can do this */
			nc_shared_cleanup(0);

			/* init the information structure */
			strncpy(nc_info->stats.start_time, t = nc_time2datetime(time(NULL), NULL), TIME_LENGTH);
			free(t);
		}

		/* update shared memory with this process's information */
		nc_info->stats.participants++;
		nc_apps_add(my_comm, &(nc_info->apps));

		/* UNLOCK */
		r = pthread_rwlock_unlock(&(nc_info->lock));

	}

	/*
	 * check used flags according to a compile time settings
	 */
	if (flags & NC_INIT_MULTILAYER) {
		nc_init_flags |= NC_INIT_MULTILAYER;
	} else {
		nc_init_flags |= NC_INIT_SINGLELAYER;
	}
#ifndef DISABLE_NOTIFICATIONS
	if (flags & NC_INIT_NOTIF) {
		nc_init_flags |= NC_INIT_NOTIF;
	}
#endif /* DISABLE_NOTIFICATIONS */
	if (flags & NC_INIT_NACM) {
		nc_init_flags |= NC_INIT_NACM;
	}
	if (flags & NC_INIT_MONITORING) {
		nc_init_flags |= NC_INIT_MONITORING;
	}
	if (flags & NC_INIT_DATASTORES) {
		nc_init_flags |= NC_INIT_DATASTORES;
	}
	if (flags & NC_INIT_WD) {
		nc_init_flags |= NC_INIT_WD;
	}
#ifndef DISABLE_VALIDATION
	if (flags & NC_INIT_VALIDATE) {
		nc_init_flags |= NC_INIT_VALIDATE;
	}
#endif
#ifndef DISABLE_URL
	if (flags & NC_INIT_URL) {
		nc_init_flags |= NC_INIT_URL;
	}
#endif
	if (flags & NC_INIT_KEEPALIVECHECK) {
		nc_init_flags |= NC_INIT_KEEPALIVECHECK;
	}

	if (nc_init_flags & NC_INIT_DATASTORES) {
		/*
		 * init internal datastores - they have to be initiated before they are
		 * used by their subsystems initiated below
		 */
		if (ncds_sysinit(nc_init_flags) != EXIT_SUCCESS) {
			nc_init_flags &= !(NC_INIT_NOTIF & NC_INIT_NACM & NC_INIT_MONITORING & NC_INIT_DATASTORES);
			return (-1);
		}

		if (first_after_close) {
			/* break any locks forgotten from the previous run */
			ncds_break_locks(NULL);

			/* apply startup to running in internal datastores */
			ncds_startup_internal();
		}

		/* set features for ietf-netconf */
		ncds_feature_enable("ietf-netconf", "writable-running");
		ncds_feature_enable("ietf-netconf", "startup");
		ncds_feature_enable("ietf-netconf", "candidate");
		ncds_feature_enable("ietf-netconf", "rollback-on-error");
		if (nc_init_flags & NC_INIT_VALIDATE) {
			ncds_feature_enable("ietf-netconf", "validate");
		}
		if (nc_init_flags & NC_INIT_URL) {
			ncds_feature_enable("ietf-netconf", "url");
		}
	}

	/* init NETCONF sessions statistics */
	if (nc_init_flags & NC_INIT_MONITORING) {
		if (nc_session_monitoring_init() != EXIT_SUCCESS) {
			nc_init_flags &= !(NC_INIT_MONITORING & NC_INIT_NOTIF & NC_INIT_NACM);
			nc_close();
			return -1;
		}
	}

	/* init NETCONF with-defaults capability */
	if (nc_init_flags & NC_INIT_WD) {
		ncdflt_set_basic_mode(NCWD_MODE_EXPLICIT);
		ncdflt_set_supported(NCWD_MODE_ALL
		    | NCWD_MODE_ALL_TAGGED
		    | NCWD_MODE_TRIM
		    | NCWD_MODE_EXPLICIT);
	}

#ifndef DISABLE_NOTIFICATIONS
	/* init Notification subsystem */
	if (nc_init_flags & NC_INIT_NOTIF) {
		if (ncntf_init() != EXIT_SUCCESS) {
			/* remove flags of uninitiated subsystems */
			nc_init_flags &= !(NC_INIT_NOTIF & NC_INIT_NACM);

			nc_close();
			return (-1);
		}
	}
#endif

	/* init Access Control subsystem */
	if (nc_init_flags & NC_INIT_NACM) {
		if (nacm_init() != EXIT_SUCCESS) {
			/* remove flags of uninitiated subsystems */
			nc_init_flags &= !NC_INIT_NACM;

			nc_close();
			return (-1);
		}
	}

	nc_init_flags |= NC_INIT_DONE;
	return (retval);
}

API int nc_close(void)
{
	int retval = 0, i, fd;
	char my_comm[NC_APPS_COMM_MAX+1];

#ifndef DISABLE_LIBSSH
	if (nc_init_flags & NC_INIT_LIBSSH_PTHREAD) {
		ssh_finalize();
	}
#endif

	if (nc_init_flags & NC_INIT_CLIENT) {
		return (retval);
	}

	/* get my comm */
	my_comm[0] = '\0';
	fd = open("/proc/self/comm", O_RDONLY);
	if (fd != -1) {
		i = read(fd, my_comm, NC_APPS_COMM_MAX);
		close(fd);
		if (i > 0) {
			if (my_comm[i-1] == '\n') {
				my_comm[i-1] = '\0';
			} else {
				my_comm[i] = '\0';
			}
		}
	}

	nc_init_flags |= NC_INIT_CLOSING;

	/* nc_apps_check() also deletes this process's info from the shared memory */
	if (nc_info != NULL) {
		/* LOCK */
		pthread_rwlock_wrlock(&(nc_info->lock));
		if (nc_apps_check(my_comm, &(nc_info->apps)) == 1 && nc_init_flags & NC_INIT_MULTILAYER) {
			/* UNLOCK */
			pthread_rwlock_unlock(&(nc_info->lock));
			/* delete the shared memory */
			retval = nc_shared_cleanup(1);
		} else {
			nc_info->stats.participants--;
			/* UNLOCK */
			pthread_rwlock_unlock(&(nc_info->lock));
		}

#ifdef POSIX_SHM
		munmap(nc_info, sizeof(struct nc_shared_info));
#else
		shmdt(nc_info);
#endif /* #ifdef POSIX_SHM */
		nc_info = NULL;
	}

	if (retval == -1) {
		nc_init_flags &= ~NC_INIT_CLOSING;
		return (retval);
	}

	/* close NETCONF session statistics */
	if (nc_init_flags & NC_INIT_MONITORING) {
		nc_session_monitoring_close();
	}

	/* close all remaining datastores */
	if (nc_init_flags & NC_INIT_DATASTORES) {
		ncds_cleanall();
	}

#ifndef DISABLE_NOTIFICATIONS
	/* close Notification subsystem */
	if (nc_init_flags & NC_INIT_NOTIF) {
		ncntf_close();
	}
#endif
	/* close Access Control subsystem */
	if (nc_init_flags & NC_INIT_NACM) {
		nacm_close();
	}

	xsltCleanupGlobals();
	xmlCleanupParser();

	nc_init_flags = 0;
	return (retval);
}

char* nc_clrwspace (const char* in)
{
	int i, j = 0, len = strlen(in);
	char* retval = strdup(in);
	if (retval == NULL) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	if (isspace(retval[0])) {
		/* remove leading whitespace characters */
		for (i = 0, j = 0; i < len ; i++, j++) {
			while (retval[i] != '\0' && isspace(retval[i])) {
				i++;
			}
			retval[j] = retval[i];
		}
	}

	/* remove trailing whitespace characters */
	while (j >= 0 && isspace(retval[j])) {
		retval[j] = '\0';
		j--;
	}

	return (retval);
}

void nc_clip_occurences_with(char *str, char sought, char replacement)
{
	int adjacent = 0;
	int clipped = 0;

	if (str == NULL) {
		return;
	}

	while (*str != '\0') {
		if (*str != sought) {
			if (clipped != 0) {
				/* Hurl together. */
				*(str - clipped) = *str;
			}
			adjacent = 0;
		} else if (adjacent == 0) {
			/*
			 * Found first character from a possible sequence of
			 * characters. The whole sequence is going to be
			 * replaced by only one replacement character.
			 */
			*(str - clipped) = replacement;
			/* Next occurrence will be adjacent. */
			adjacent = 1;
		} else {
			++clipped;
		}

		/* Next character. */
		++str;
	}

	if (clipped != 0) {
		/* New string end. */
		*(str - clipped) = '\0';
	}
}

char** nc_get_grouplist(const char* username)
{
	struct passwd p, *pp;
	struct group g, *gg;
	int i, j, k;
	gid_t *glist;
	char** retval = NULL, buf[256];

	if (!username) {
		return NULL;
	}

	getpwnam_r(username, &p, buf, 256, &pp);

	/* get system groups for the username */
	if (pp != NULL) {
		i = 0;
		/* this call end with -1, but sets i to contain count of groups */
		getgrouplist(username, pp->pw_gid, NULL, &i);
		if (i != 0) {
			glist = malloc(i * sizeof (gid_t));
			retval = malloc((i+1) * sizeof(char*));
			if (glist == NULL || retval == NULL) {
				ERROR("Memory reallocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				free(retval);
				free(glist);
				return NULL;
			}

			if (getgrouplist(username, pp->pw_gid, glist, &i) != -1) {
				for (j = 0, k = 0; j < i; j++) {
					getgrgid_r(glist[j], &g, buf, 256, &gg);
					if (gg != NULL && gg->gr_name) {
						retval[k++] = strdup(gg->gr_name);
					}
				}
				retval[k] = NULL; /* list termination */
			} else {
				WARN("%s: unable to get list of groups (getgrouplist() failed)", __func__);
			}
			free(glist);
		}
	}

	return (retval);
}

API time_t nc_datetime2time(const char* datetime)
{
	struct tm time;
	char* dt;
	int i;
	long int shift, shift_m;
	time_t retval;

	if (datetime == NULL) {
		return (-1);
	} else {
		dt = strdup(datetime);
	}

	if (strlen(dt) < 20 || dt[4] != '-' || dt[7] != '-' || dt[13] != ':' || dt[16] != ':') {
		ERROR("Wrong date time format not compliant to RFC 3339.");
		free(dt);
		return (-1);
	}

	memset(&time, 0, sizeof(struct tm));
	time.tm_year = atoi(&dt[0]) - 1900;
	time.tm_mon = atoi(&dt[5]) - 1;
	time.tm_mday = atoi(&dt[8]);
	time.tm_hour = atoi(&dt[11]);
	time.tm_min = atoi(&dt[14]);
	time.tm_sec = atoi(&dt[17]);

	retval = timegm(&time);

	/* apply offset */
	i = 19;
	if (dt[i] == '.') { /* we have fractions to skip */
		for (i++; isdigit(dt[i]); i++);
	}
	if (dt[i] == 'Z' || dt[i] == 'z') {
		/* zero shift */
		shift = 0;
	} else if (dt[i+3] != ':') {
		/* wrong format */
		ERROR("Wrong date time shift format not compliant to RFC 3339.");
		free(dt);
		return (-1);
	} else {
		shift = strtol(&dt[i], NULL, 10);
		shift = shift * 60 * 60; /* convert from hours to seconds */
		shift_m = strtol(&dt[i+4], NULL, 10) * 60; /* includes conversion from minutes to seconds */
		/* correct sign */
		if (shift < 0) {
			shift_m *= -1;
		}
		/* connect hours and minutes of the shift */
		shift = shift + shift_m;
	}
	/* we have to shift to the opposite way to correct the time */
	retval -= shift;

	free(dt);
	return (retval);
}

API char* nc_time2datetime(time_t time, const char* tz)
{
	char* date = NULL;
	char* zoneshift = NULL;
	int zonediff, zonediff_h, zonediff_m;
	struct tm tm, *tm_ret;
	char *tz_origin;

	if (tz) {
		tz_origin = getenv("TZ");
		setenv("TZ", tz, 1);
		tm_ret = localtime_r(&time, &tm);
		setenv("TZ", tz_origin, 1);

		if (tm_ret == NULL) {
			return (NULL);
		}
	} else {
		if (gmtime_r(&time, &tm) == NULL) {
			return (NULL);
		}
	}

	if (tm.tm_isdst < 0) {
		zoneshift = NULL;
	} else {
		if (tm.tm_gmtoff == 0) {
			/* time is Zulu (UTC) */
			if (asprintf(&zoneshift, "Z") == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				return (NULL);
			}
		} else {
			zonediff = tm.tm_gmtoff;
			zonediff_h = zonediff / 60 / 60;
			zonediff_m = zonediff / 60 % 60;
			if (asprintf(&zoneshift, "%s%02d:%02d",
			                (zonediff < 0) ? "-" : "+",
			                zonediff_h,
			                zonediff_m) == -1) {
				ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
				return (NULL);
			}
		}
	}
	if (asprintf(&date, "%04d-%02d-%02dT%02d:%02d:%02d%s",
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec,
	                (zoneshift == NULL) ? "" : zoneshift) == -1) {
		free(zoneshift);
		ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	free (zoneshift);

	return (date);
}
