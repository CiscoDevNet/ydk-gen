/**
 * \file session.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of functions to handle NETCONF sessions.
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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h>
#include <ctype.h>

#ifndef DISABLE_LIBSSH
#	include <libssh/libssh.h>
#endif

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "config.h"

#include "netconf_internal.h"
#include "messages.h"
#include "messages_internal.h"
#include "session.h"
#include "datastore.h"
#include "nacm.h"
#include "transport.h"

#ifdef ENABLE_TLS
#	include <openssl/ssl.h>
#	include <openssl/err.h>
#endif

#ifndef DISABLE_NOTIFICATIONS
#  include "notifications.h"
#endif

#ifndef DISABLE_URL
#	include "url_internal.h"
	extern int nc_url_protocols;
#endif

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

/* definition in datastore.c */
char** get_schemas_capabilities(struct nc_cpblts *cpblts);

extern struct nc_shared_info *nc_info;

/*
 * From internal.c to be used by nc_session_get_cpblts_deault() to detect
 * what part of libnetconf is initiated and can be provided in hello messages
 * as a supported capability/module
 */
extern int nc_init_flags;

/**
 * @brief List of possible NETCONF transportation supported by libnetconf
 */
enum nc_transport {
	NC_TRTANSPORT_SSH /* netconf-ssh */
};

struct session_list_item {
	int offset_prev;
	int offset_next;
	int size;
	int active; /* flag if the non-dummy session is connected to this record */
	int scounter; /* number of sessions connected with this record */
	char session_id[SID_SIZE];
	pid_t pid;
	enum nc_transport transport;
	struct nc_session_stats stats;
	char login_time[TIME_LENGTH];
	 /*
	  * variable length part - data actually contain 2 strings (username and
	  * source-host whose lengths can differ for each session)
	  */
	pthread_rwlock_t lock;
	char data[1];
};

struct session_list_map {
	/* start of the mapped file with session list */
	int size; /* current file size (may include gaps) */
	int count; /* current number of sessions */
	int first_offset;
	pthread_rwlock_t lock; /* lock for the all file - for resizing */
	struct session_list_item record[1]; /* first record of the session records list */
};

static int session_list_fd = -1;
static struct session_list_map *session_list = NULL;

#ifdef DISABLE_LIBSSH
#ifdef ENABLE_TLS
#define NC_WRITE(session,buf,c,ret) \
	if (session->fd_output != -1) {ret = write (session->fd_output, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else if (session->tls){ \
		ret = SSL_write(session->tls, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else { \
		ret = -1; \
	}
#else /* not ENABLE_TLS (but DISABLE_LIBSSH) */
#define NC_WRITE(session,buf,c,ret) \
	if (session->fd_output != -1) {ret = write (session->fd_output, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else { \
		ret = -1; \
	}
#endif /* not ENABLE_TLS */
#else /* not DISABLE_LIBSSH */
#ifdef ENABLE_TLS
#define NC_WRITE(session,buf,c,ret) \
	if(session->ssh_chan){ \
		ret = ssh_channel_write (session->ssh_chan, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else if (session->tls){ \
		ret = SSL_write(session->tls, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else if (session->fd_output != -1) { \
		ret = write (session->fd_output, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else { \
		ret = -1; \
	}
#else /* not ENABLE_TLS */
#define NC_WRITE(session,buf,c,ret) \
	if(session->ssh_chan){ \
		ret = ssh_channel_write (session->ssh_chan, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else if (session->fd_output != -1) { \
		ret = write (session->fd_output, (buf), strlen(buf)); \
		if (ret > 0) {c += ret;} \
	} else { \
		ret = -1; \
	}
#endif /* not ENABLE_TLS */
#endif /* not DISABLE_LIBSSH */

#define SIZE_STEP (1024*16)
int nc_session_monitoring_init(void)
{
	struct stat fdinfo;
	int first = 0, c;
	size_t size;
	pthread_rwlockattr_t rwlockattr;
	mode_t um;

	if (session_list != NULL) {
		ERROR("%s: session list already exists.", __func__);
		return (EXIT_FAILURE);
	}

	if (session_list_fd != -1) {
		close(session_list_fd);
	}

	um = umask(0000);
	session_list_fd = open(NC_SESSIONSFILE, O_CREAT | O_RDWR, FILE_PERM);
	umask(um);
	if (session_list_fd == -1) {
		ERROR("Opening the sessions monitoring file failed (%s).", strerror(errno));
		return (EXIT_FAILURE);
	}

	/* get the file size */
	if (fstat(session_list_fd, &fdinfo) == -1) {
		ERROR("Unable to get the sessions monitoring file information (%s)", strerror(errno));
		close(session_list_fd);
		session_list_fd = -1;
		return (EXIT_FAILURE);
	}

	if (fdinfo.st_size == 0) {
		/* we have a new file, create some initial size using file gaps */
		first = 1;
		lseek(session_list_fd, SIZE_STEP - 1, SEEK_SET);
		while (((c = write(session_list_fd, "", 1)) == -1) && (errno == EAGAIN || errno == EINTR));
		if (c == -1) {
			WARN("%s: Preparing the session list file failed (%s).", __func__, strerror(errno));
		}
		lseek(session_list_fd, 0, SEEK_SET);
		size = SIZE_STEP;
	} else {
		size = fdinfo.st_size;
	}

	session_list = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, session_list_fd, 0);
	if (session_list == MAP_FAILED) {
		ERROR("Accessing the shared sessions monitoring file failed (%s)", strerror(errno));
		close(session_list_fd);
		session_list = NULL;
		session_list_fd = -1;
		return (EXIT_FAILURE);
	}

	if (first) {
		pthread_rwlockattr_init(&rwlockattr);
		pthread_rwlockattr_setpshared(&rwlockattr, PTHREAD_PROCESS_SHARED);
		pthread_rwlock_init(&(session_list->lock), &rwlockattr);
		pthread_rwlockattr_destroy(&rwlockattr);
		pthread_rwlock_wrlock(&(session_list->lock));
		session_list->size = SIZE_STEP;
		session_list->count = 0;
		pthread_rwlock_unlock(&(session_list->lock));
	}

	return (EXIT_SUCCESS);
}

void nc_session_monitoring_close(void)
{
	if (session_list) {
		munmap(session_list, session_list->size);
		close(session_list_fd);
		session_list = NULL;
		session_list_fd = -1;
	}
}

int nc_session_is_monitored(const char* session_id)
{
	struct session_list_item *litem;

	if (session_list == NULL || session_list->count == 0) {
		return 0;
	}

	for(litem = (struct session_list_item*)((char*)(session_list->record) + session_list->first_offset);
				litem != NULL;
				litem = (struct session_list_item*)((char*)litem + litem->offset_next)) {
		if (strcmp(litem->session_id, session_id) == 0) {
			return 1;
		}

		if (litem->offset_next == 0) {
			/* we are at the end of the list */
			break;
		}
	}

	return 0;
}

API int nc_session_monitor(struct nc_session* session)
{
	struct session_list_item *litem = NULL, *litem_aux;
	pthread_rwlockattr_t rwlockattr;
	int prev, next, size, totalsize = 0;

#define UNKN_USER "UNKNOWN"
#define UNKN_USER_SIZE 8
#define UNKN_HOST "UNKNOWN"
#define UNKN_HOST_SIZE 8

	if (session->monitored) {
		return (EXIT_SUCCESS);
	}

	if (session == NULL || session_list == NULL) {
		return (EXIT_FAILURE);
	}

	if (session->status != NC_SESSION_STATUS_WORKING && session->status != NC_SESSION_STATUS_DUMMY) {
		ERROR("%s: specified session is in invalid state and cannot be monitored.", __func__);
		return (EXIT_FAILURE);
	}

	/* critical section */
	pthread_rwlock_wrlock(&(session_list->lock));
	if (session_list->count > 0) {
		/* find possible duplicity of the record */
		for(litem = (struct session_list_item*)((char*)(session_list->record) + session_list->first_offset);
				litem != NULL;
				litem = (struct session_list_item*)((char*)litem + litem->offset_next)) {
			if (strcmp(session->session_id, litem->session_id) == 0) {
				/* session is already monitored */

				/*
				 * allow to add the session only if the connecting
				 * session is dummy or if there is no real session
				 * connected with this record
				 */
				if (session->status == NC_SESSION_STATUS_DUMMY) {
					litem->scounter++;
					/*
					 * PID is not updated, since the keep-alive check should
					 * focus on processes holding the real session, not the dummies
					 */
					pthread_rwlock_unlock(&(session_list->lock));

					/* connect session statistics to the shared memory segment */
					free(session->stats);
					session->stats = &(litem->stats);
					session->monitored = 1;
					return (EXIT_SUCCESS);
				} else if (session->status == NC_SESSION_STATUS_WORKING && litem->active == 0) {
					litem->scounter++;
					litem->active = 1;
					/* update PID for keep-alive check */
					litem->pid = getpid();
					pthread_rwlock_unlock(&(session_list->lock));

					/* connect session statistics to the shared memory segment */
					free(session->stats);
					session->stats = &(litem->stats);
					session->monitored = 1;
					return (EXIT_SUCCESS);
				} else if (litem->active == 1) {
					/* update PID for keep-alive check */
					litem->pid = getpid();
					pthread_rwlock_unlock(&(session_list->lock));
					return (EXIT_SUCCESS);
				} else {
					ERROR("%s: specified session is in invalid state and cannot be monitored.", __func__);
					pthread_rwlock_unlock(&(session_list->lock));
					return (EXIT_FAILURE);
				}
			}

			if (litem->offset_next == 0) {
				/* we are on the end of the list */
				break;
			}
		}
	}

	/* find the place for a new record, try to place it into some gap */
	size = (sizeof(struct session_list_item) - 1) + (
	                (session->username != NULL) ? (strlen(session->username) + 1) : UNKN_USER_SIZE) + (
	                (session->hostname != NULL) ? (strlen(session->hostname) + 1) : UNKN_HOST_SIZE);
	if (session_list->count == 0) {
		/* we add the first item into the list */
		litem = &(session_list->record[0]);
		litem->offset_prev = 0;
		litem->offset_next = 0;
		session_list->first_offset = 0;
	} else if (session_list->first_offset >= size) {
		/* the gap is in the beginning of the list */
		litem = &(session_list->record[0]);
		litem->offset_prev = 0;
		litem->offset_next = session_list->first_offset;
		litem_aux = (struct session_list_item*)((char*)(session_list->record) + session_list->first_offset);
		session_list->first_offset = 0;
		litem_aux->offset_prev = litem_aux->size;
	} else {
		totalsize = session_list->first_offset;

		/* search for a gap inside the list */
		for (litem = (struct session_list_item*) ((char*) (session_list->record) + session_list->first_offset);
		                ;
		                litem = (struct session_list_item*) ((char*) litem + litem->offset_next)) {
			/* check if there is enough space to add new record */
			if ((totalsize + litem->size + size) > session_list->size) {
				ERROR("There is not enough space to monitor another NETCONF session.");
				pthread_rwlock_unlock(&(session_list->lock));
				return (EXIT_FAILURE);
			} else {
				totalsize += litem->offset_next;
			}

			if (litem->offset_next >= (size + litem->size)) {
				/* we have the efficient gap */

				/* correct links from predecessor */
				prev = litem->size;
				next = litem->offset_next - litem->size;
				litem->offset_next = litem->size;

				/* now go into the new record and create connection with sibling records */
				litem = (struct session_list_item*)((char*)litem + litem->offset_next);
				litem->offset_prev = prev;
				litem->offset_next = next;

				/* also correct links in successor */
				litem_aux = (struct session_list_item*)((char*)litem + litem->offset_next);
				litem_aux->offset_prev = litem->offset_next;
				break;
			} else if (litem->offset_next == 0) {
				/* we are on the end of the list */
				prev = litem->size;
				litem->offset_next = litem->size;

				/* now go into the new record */
				litem = (struct session_list_item*)((char*)litem + litem->offset_next);
				litem->offset_prev = prev;
				litem->offset_next = 0;
				break;
			}
			/* move to another item in the list */
		}
	}
	session_list->count++;

	/* fill new structure */
	litem->size = size;
	strncpy(litem->session_id, session->session_id, SID_SIZE);
	litem->pid = getpid();
	litem->transport = NC_TRTANSPORT_SSH;
	if (session->stats != NULL) {
		memcpy(&(litem->stats), session->stats, sizeof(struct nc_session_stats));
		free(session->stats);
	}
	session->stats = &(litem->stats);
	strncpy(litem->login_time, (session->logintime == NULL) ? "0000-01-01T00:00:00Z" : session->logintime, TIME_LENGTH);
	litem->login_time[TIME_LENGTH - 1] = 0; /* terminating null byte */

	strcpy(litem->data, (session->username == NULL) ? UNKN_USER : session->username);
	strcpy(litem->data + 1 + strlen(litem->data), (session->hostname == NULL) ? UNKN_HOST : session->hostname);

	pthread_rwlockattr_init(&rwlockattr);
	pthread_rwlockattr_setpshared(&rwlockattr, PTHREAD_PROCESS_SHARED);
	pthread_rwlock_init(&(litem->lock), &rwlockattr);
	pthread_rwlockattr_destroy(&rwlockattr);

	if (session->status == NC_SESSION_STATUS_WORKING) {
		litem->active = 1;
	}

	litem->scounter = 1;
	session->monitored = 1;

	/* end of critical section, other processes now can access new record */
	pthread_rwlock_unlock(&(session_list->lock));

	return (EXIT_SUCCESS);
}

static void nc_session_monitor_remove(struct session_list_item *litem)
{
	struct session_list_item *aux;

	/* reconnect the list */
	if (litem->offset_prev == 0) { /* first item in the list */
		session_list->first_offset += litem->offset_next;
	} else {
		aux = (struct session_list_item*) ((char*) litem - litem->offset_prev);
		aux->offset_next = (litem->offset_next == 0) ? 0 : (aux->offset_next + litem->offset_next);
	}
	aux = (struct session_list_item*) ((char*) litem + litem->offset_next);
	aux->offset_prev = (litem->offset_prev == 0) ? 0 : (aux->offset_prev + litem->offset_prev);
	session_list->count--;
}

/* length of path of /proc/<PID>/fd/<FDNUM> */
#define ALIVECHECK_PATH_LENGTH 32
static void nc_session_monitor_alive_check(void)
{
	struct session_list_item *litem;
	char dirpath[ALIVECHECK_PATH_LENGTH];
	char linkpath[ALIVECHECK_PATH_LENGTH];
	char linkname[sizeof(NC_SESSIONSFILE) + 1];
	char* aux = NULL;
	int len;
	DIR *dir;
	struct dirent* pfd;

	if (session_list != NULL) {
		aux = strdup(NC_SESSIONSFILE);
		nc_clip_occurences_with(aux, '/', '/');

		pthread_rwlock_wrlock(&(session_list->lock));

		/* check the whole list of monitored sessions */
		for (litem = (struct session_list_item*) ((char*) (session_list->record) + session_list->first_offset);
				;
		        litem = (struct session_list_item*) ((char*) litem + litem->offset_next)) {

			/* check that the monitored process is still alive */
			snprintf(dirpath, ALIVECHECK_PATH_LENGTH, "/proc/%d/fd", litem->pid);
			if (access(dirpath, F_OK) == -1) {
				/* no such a process exists, remove not alive session item */
				litem->scounter = 0;
				nc_session_monitor_remove(litem);
			} else {
				/* check that the process is still the same (is using libnetconf) */
				dir = opendir(dirpath);
				if (dir == NULL) {
					if (errno == ENOENT) {
						/* process /proc directory actually does not exist */
						litem->scounter = 0;
						nc_session_monitor_remove(litem);
					} /* else we cannot do more checks */
					goto alivecheck_next;
				}

				/* search in all file descriptors for the sessions stats file */
				errno = 0;
				while((pfd = readdir(dir)) != NULL) {
					snprintf(linkpath, ALIVECHECK_PATH_LENGTH, "%s/%s", dirpath, pfd->d_name);
					if ((len = readlink(linkpath, linkname, sizeof(linkname))) > 0) {
						linkname[len] = 0;
						if (strcmp(linkname, aux) == 0) {
							/* we have match, the process uses libnetconf */
							break;
						}
					} /* not a symlink or other problem - simply it doesn't match, continue */
				}
				if (pfd == NULL) {
					/* the process does not use libnetconf, remove not alive session item */
					litem->scounter = 0;
					nc_session_monitor_remove(litem);
				}
				closedir(dir);
			}

alivecheck_next:
			/* end loop condition */
			if (litem->offset_next == 0) {
				/* no other item in the list */
				break;
			}
		}

		pthread_rwlock_unlock(&(session_list->lock));
		free(aux);
	}

}

char* nc_session_stats(void)
{
	char *aux, *sessions = NULL, *session = NULL;
	struct session_list_item *litem;

	if (session_list == NULL) {
		return (NULL);
	}
	if (nc_init_flags & NC_INIT_KEEPALIVECHECK) {
		nc_session_monitor_alive_check();
	}
	pthread_rwlock_rdlock(&(session_list->lock));
	for (litem = (struct session_list_item*)((char*)(session_list->record) + session_list->first_offset); session_list->count > 0 && litem != NULL;) {
		aux = NULL;
		if (asprintf(&aux, "<session><session-id>%s</session-id>"
				"<transport>netconf-ssh</transport>"
				"<username>%s</username>"
				"<source-host>%s</source-host>"
				"<login-time>%s</login-time>"
				"<in-rpcs>%u</in-rpcs><in-bad-rpcs>%u</in-bad-rpcs>"
				"<out-rpc-errors>%u</out-rpc-errors>"
				"<out-notifications>%u</out-notifications></session>",
				litem->session_id,
				litem->data, /* username */
				litem->data + (strlen(litem->data) + 1), /* hostname */
				litem->login_time,
				litem->stats.in_rpcs,
				litem->stats.in_bad_rpcs,
				litem->stats.out_rpc_errors,
				litem->stats.out_notifications) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
		} else {
			if (session == NULL) {
				session = aux;
			} else {
				void *tmp = realloc(session, strlen(session) + strlen(aux) + 1);
				if (tmp == NULL) {
					ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
					free(aux);
					/* return what we already have */
					break;
				} else {
					session = tmp;
					strcat(session, aux);
					free(aux);
				}
			}
		}

		/* move to the next record */
		if (litem->offset_next == 0) {
			litem = NULL;
		} else {
			litem = (struct session_list_item*)((char*)litem + litem->offset_next);
		}
	}
	pthread_rwlock_unlock(&(session_list->lock));

	if (session != NULL) {
		if (asprintf(&sessions, "<sessions>%s</sessions>", session) == -1) {
			ERROR("asprintf() failed (%s:%d).", __FILE__, __LINE__);
			free(session);
			return (NULL);
		}
		free(session);
	}
	return (sessions);
}

API const char* nc_session_get_id (const struct nc_session *session)
{
	if (session == NULL) {
		return (NULL);
	}
	return (session->session_id);
}

API const char* nc_session_get_host(const struct nc_session* session)
{
	if (session == NULL) {
		return (NULL);
	}
	return (session->hostname);
}

API const char* nc_session_get_port(const struct nc_session* session)
{
	if (session == NULL) {
		return (NULL);
	}
	return (session->port);
}

API const char* nc_session_get_user(const struct nc_session* session)
{
	if (session == NULL) {
		return (NULL);
	}
	return (session->username);
}

API NC_TRANSPORT nc_session_get_transport(const struct nc_session* session)
{
	if (session == NULL) {
		return (NC_TRANSPORT_UNKNOWN);
	}

	return (session->transport);
}

API int nc_session_get_version(const struct nc_session *session)
{
	if (session == NULL) {
		return (-1);
	}
	return (session->version);
}

API int nc_session_get_eventfd(const struct nc_session *session)
{
	if (session == NULL) {
		return -1;
	}

	if (session->transport_socket != -1) {
		return (session->transport_socket);
	} else if (session->fd_input != -1) {
		return (session->fd_input);
	} else {
		return (-1);
	}
}

API int nc_session_notif_allowed(struct nc_session *session)
{
#ifndef DISABLE_NOTIFICATIONS
	int ret;

	if (session == NULL) {
		/* notification subscription is not allowed */
		return 0;
	}

	/* check capabilities */
	if (nc_cpblts_enabled(session, NC_CAP_NOTIFICATION_ID) == 1) {
		/* subscription is allowed only if another subscription is not active */
		DBG_LOCK("mut_ntf");
		pthread_mutex_lock(&(session->mut_ntf));
		ret = (session->ntf_active == 0) ? 1 : 0;
		DBG_UNLOCK("mut_ntf");
		pthread_mutex_unlock(&(session->mut_ntf));

		return (ret);
	} else
#endif
	{
		/* notifications are not supported at all */
		return (0);
	}
}

API void nc_cpblts_free(struct nc_cpblts *c)
{
	int i;

	if (c == NULL) {
		return;
	}

	if (c->list != NULL) {
		if (c->items <= c->list_size) {
			for(i = 0; i < c->items; i++) {
				if (c->list[i] != NULL) {
					free(c->list[i]);
				}
			}
		} else {
			WARN("nc_cpblts_free: invalid capabilities structure, some memory may not be freed.");
		}
		free(c->list);
	}
	free(c);
}

API struct nc_cpblts* nc_cpblts_new(const char* const list[])
{
	struct nc_cpblts *retval;
	int i;

	retval = calloc(1, sizeof(struct nc_cpblts));
	if (retval == NULL) {
		ERROR("Memory allocation failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	retval->list_size = 10; /* initial value */
	retval->list = malloc(retval->list_size * sizeof(char*));
	if (retval->list == NULL) {
		ERROR("Memory allocation failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		free(retval);
		return (NULL);
	}
	retval->list[0] = NULL;

	if (list != NULL) {
		for (i = 0; list[i] != NULL; i++) {
			retval->list[i] = strdup (list[i]);
			retval->items++;
			if (retval->items == retval->list_size) {
				/* resize the capacity of the capabilities list */
				void *tmp = realloc (retval->list, retval->list_size * 2 * sizeof (char*));
				if (tmp == NULL) {
					nc_cpblts_free (retval);
					return (NULL);
				}
				retval->list = tmp;
				retval->list_size *= 2;
			}
			retval->list[i + 1] = NULL;
		}
	}

	return (retval);
}

API int nc_cpblts_add(struct nc_cpblts* capabilities, const char* capability_string)
{
	int i;
	size_t len;
	char *s, *p = NULL;

	if (capabilities == NULL || capability_string == NULL) {
		return (EXIT_FAILURE);
	}

	/* get working copy of capability_string where the parameters will be ignored */
	s = strdup(capability_string);
	if ((p = strchr(s, '?')) != NULL) {
		/* in following comparison, ignore capability's parameters */
		len = p - s;
	} else {
		len = strlen(s);
	}

	/* find duplicities */
	for (i = 0; i < capabilities->items; i++) {
		if (!strncmp(capabilities->list[i], s, len) &&
				(capabilities->list[i][len] == '?' || capabilities->list[i][len] == '\0')) {
			/* capability is already in the capabilities list, but
			 * parameters can differ, so substitute the current instance
			 * with the new one
			 */
			free(capabilities->list[i]);
			capabilities->list[i] = s;
			return (EXIT_SUCCESS);
		}
	}

	/* check size of the capabilities list */
	if ((capabilities->items + 1) >= capabilities->list_size) {
		/* resize the capacity of the capabilities list */
		void *tmp = realloc(capabilities->list, capabilities->list_size * 2 * sizeof (char*));
		if (tmp == NULL) {
			free(s);
			return (EXIT_FAILURE);
		}
		capabilities->list = tmp;
		capabilities->list_size *= 2;
	}

	/* add capability into the list */
	capabilities->list[capabilities->items] = s;
	capabilities->items++;
	/* set list terminating NULL item */
	capabilities->list[capabilities->items] = NULL;

	return (EXIT_SUCCESS);
}

API int nc_cpblts_remove(struct nc_cpblts* capabilities, const char* capability_string)
{
	int i;
	char* s, *p;

	if (capabilities == NULL || capability_string == NULL) {
		return (EXIT_FAILURE);
	}

	if (capabilities->items > capabilities->list_size) {
		WARN("nc_cpblts_add: structure inconsistency! Some data may be lost.");
		return (EXIT_FAILURE);
	}

	s = strdup(capability_string);
	if ((p = strchr(s, '?')) != NULL) {
		/* in comparison, ignore capability's parameters */
		p = 0;
	}

	for (i = 0; i < capabilities->items; i++) {
		if (capabilities->list[i] != NULL && strncmp(capabilities->list[i], s, strlen(s)) == 0) {
			break;
		}
	}
	free(s);

	if (i < capabilities->items) {
		free(capabilities->list[i]);
		/* move here the last item from the list */
		capabilities->list[i] = capabilities->list[capabilities->items - 1];
		/* and then set the last item in the list to NULL */
		capabilities->list[capabilities->items - 1] = NULL;
		capabilities->items--;
	}

	return (EXIT_SUCCESS);
}

API const char* nc_cpblts_get(const struct nc_cpblts* c, const char* capability_string)
{
	int i;
	char* s, *p;

	if (capability_string == NULL || c == NULL || c->list == NULL) {
		return (NULL);
	}

	s = strdup(capability_string);
	if ((p = strchr(s, '?')) != NULL) {
		/* in comparison, ignore capability's parameters */
		p = 0;
	}

	for (i = 0; c->list[i]; i++) {
		if (strncmp(s, c->list[i], strlen(s)) == 0) {
			free(s);
			return (c->list[i]);
		}
	}
	free(s);
	return (NULL);

}

API int nc_cpblts_enabled(const struct nc_session* session, const char* capability_string)
{
	int i;
	char* s, *p;

	if (capability_string == NULL || session == NULL || session->capabilities == NULL) {
		return (0);
	}

	s = strdup(capability_string);
	if ((p = strchr(s, '?')) != NULL) {
		/* in comparison, ignore capability's parameters */
		p = 0;
	}

	for (i = 0; session->capabilities->list[i]; i++) {
		if (strncmp(s, session->capabilities->list[i], strlen(s)) == 0) {
			free(s);
			return (1);
		}
	}
	free(s);
	return (0);
}

API void nc_cpblts_iter_start(struct nc_cpblts* c)
{
	if (c == NULL) {
		return;
	}
	c->iter = 0;
}

API const char* nc_cpblts_iter_next(struct nc_cpblts* c)
{
	if (c == NULL || c->list == NULL) {
		return (NULL);
	}

	if (c->iter > (c->items - 1)) {
		return NULL;
	}

	return (c->list[c->iter++]);
}

API int nc_cpblts_count(const struct nc_cpblts* c)
{
	if (c == NULL || c->list == NULL) {
		return 0;
	}
	return c->items;
}

API struct nc_cpblts* nc_session_get_cpblts_default(void)
{
	struct nc_cpblts *retval;
	char** nslist;
	int i;

	retval = nc_cpblts_new(NULL);
	if (retval == NULL) {
		return (NULL);
	}

	nc_cpblts_add(retval, NC_CAP_BASE10_ID);
	nc_cpblts_add(retval, NC_CAP_BASE11_ID);
	nc_cpblts_add(retval, NC_CAP_WRUNNING_ID);
	nc_cpblts_add(retval, NC_CAP_CANDIDATE_ID);
	nc_cpblts_add(retval, NC_CAP_STARTUP_ID);
	nc_cpblts_add(retval, NC_CAP_ROLLBACK_ID);

#ifndef DISABLE_NOTIFICATIONS
	if (nc_init_flags & NC_INIT_NOTIF) {
		nc_cpblts_add(retval, NC_CAP_INTERLEAVE_ID);
		nc_cpblts_add(retval, NC_CAP_NOTIFICATION_ID);
	}
#endif
#ifndef DISABLE_VALIDATION
	if (nc_init_flags & NC_INIT_VALIDATE) {
		nc_cpblts_add(retval, NC_CAP_VALIDATE10_ID);
		nc_cpblts_add(retval, NC_CAP_VALIDATE11_ID);
	}
#endif
	if ((nc_init_flags & NC_INIT_WD) && (ncdflt_get_basic_mode() != NCWD_MODE_NOTSET)) {
		nc_cpblts_add(retval, NC_CAP_WITHDEFAULTS_ID);
	}
#ifndef DISABLE_URL
	if ((nc_init_flags & NC_INIT_URL)) {
		nc_cpblts_add(retval, NC_CAP_URL_ID);
	}
#endif

	/* add namespaces of used datastores as announced capabilities */
	if ((nslist = get_schemas_capabilities(retval)) != NULL) {
		for(i = 0; nslist[i] != NULL; i++) {
			nc_cpblts_add(retval, nslist[i]);
			free(nslist[i]);
		}
		free(nslist);
	}

	return (retval);
}

API struct nc_cpblts* nc_session_get_cpblts(const struct nc_session* session)
{
	if (session == NULL) {
		return (NULL);
	}

	return (session->capabilities);
}

/**
 * @brief Parse with-defaults capability
 * This function is used in ssh.c
 */
void parse_wdcap(struct nc_cpblts *capabilities, NCWD_MODE *basic, int *supported)
{
	const char* cpblt;
	char* s;

	if ((cpblt = nc_cpblts_get(capabilities, NC_CAP_WITHDEFAULTS_ID)) != NULL) {
		if ((s = strstr(cpblt, "report-all")) != NULL) {
			if (s[-1] == '=' && s[-2] == 'e') {
				/* basic mode: basic-mode=report-all */
				*basic = NCWD_MODE_ALL;
			}
			*supported = *supported | NCWD_MODE_ALL;
		}
		if ((s = strstr(cpblt, "trim")) != NULL) {
			if (s[-1] == '=' && s[-2] == 'e') {
				/* basic mode: basic-mode=trim */
				*basic = NCWD_MODE_TRIM;
			}
			*supported = *supported | NCWD_MODE_TRIM;
		}
		if ((s = strstr(cpblt, "explicit")) != NULL) {
			if (s[-1] == '=' && s[-2] == 'e') {
				/* basic mode: basic-mode=explicit */
				*basic = NCWD_MODE_EXPLICIT;
			}
			*supported = *supported | NCWD_MODE_EXPLICIT;
		}
		if ((s = strstr(cpblt, "report-all-tagged")) != NULL) {
			*supported = *supported | NCWD_MODE_ALL_TAGGED;
		}
	} else {
		*basic = NCWD_MODE_NOTSET;
		*supported = 0;
	}
}

API struct nc_session* nc_session_dummy(const char* sid, const char* username, const char* hostname, struct nc_cpblts *capabilities)
{
	struct nc_session * session;
	struct passwd p, *pp;
	const char* cpblt;
	char buf[256];

	if (sid == NULL || username == NULL || capabilities == NULL) {
		return NULL;
	}

	if ((session = malloc (sizeof (struct nc_session))) == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		return NULL;
	}
	memset (session, 0, sizeof (struct nc_session));
	if ((session->stats = malloc (sizeof (struct nc_session_stats))) == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		free(session);
		return NULL;
	}

	/* do not send <close-session> on nc_session_close() */
	session->is_server = 1;

	/* set invalid fd values to prevent comunication */
	session->fd_input = -1;
	session->fd_output = -1;
	session->transport_socket = -1;

	/* init stats values */
	session->logintime = nc_time2datetime(time(NULL), NULL);
	session->monitored = 0;
	session->stats->in_rpcs = 0;
	session->stats->in_bad_rpcs = 0;
	session->stats->out_rpc_errors = 0;
	session->stats->out_notifications = 0;

	/*
	 * mutexes and queues fields are not initialized since dummy session
	 * cannot send or receive any data
	 */

	/* session is DUMMY */
	session->status = NC_SESSION_STATUS_DUMMY;
	/* copy session id */
	strncpy (session->session_id, sid, SID_SIZE);
	/* get system groups for the username */
	session->groups = nc_get_grouplist(username);
	/* if specified, copy hostname */
	if (hostname != NULL) {
		session->hostname = strdup (hostname);
	}
	/* copy user name */
	session->username = strdup (username);
	/* detect if user ID is 0 -> then the session is recovery */
	session->nacm_recovery = 0;
	getpwnam_r(username, &p, buf, 256, &pp);
	if (pp != NULL) {
		if (pp->pw_uid == NACM_RECOVERY_UID) {
			session->nacm_recovery = 1;
		}
	}
	/* create empty capabilities list */
	session->capabilities = nc_cpblts_new (NULL);
	/* initialize capabilities iterator */
	nc_cpblts_iter_start (capabilities);
	/* copy all capabilities */
	while ((cpblt = nc_cpblts_iter_next (capabilities)) != NULL) {
		nc_cpblts_add (session->capabilities, cpblt);
	}

	session->wd_basic = NCWD_MODE_NOTSET;
	session->wd_modes = 0;
	/* set with defaults capability flags */
	parse_wdcap(session->capabilities, &(session->wd_basic), &(session->wd_modes));

	if (pp) {
		VERB("Created dummy session %s for user \'%s\' (UID %d)%s",
			session->session_id,
			session->username,
			pp->pw_uid,
			session->nacm_recovery ? " - recovery session" : "");
	}

	return session;
}

static void announce_nc_session_closing(struct nc_session* session)
{
	nc_rpc *rpc_close = NULL;
	nc_reply *reply = NULL;

	/* prevent infinite recursion when socket is corrupted -> stack overflow */
	session->status = NC_SESSION_STATUS_CLOSING;

	if (!session->is_server) {
		/* close NETCONF session */
		rpc_close = nc_rpc_closesession();
		if (rpc_close != NULL) {
			if (nc_session_send_rpc(session, rpc_close) != 0) {
				nc_session_recv_reply(session, 10000, &reply); /* wait max 10 seconds */
				if (reply != NULL) {
					nc_reply_free(reply);
				}
			}
			if (rpc_close != NULL) {
				nc_rpc_free(rpc_close);
			}
		}
	}
}

/**
 * @brief Stop the running ncntf_dispatch_*()
 *
 * When we are going to close an active session and receiving/sending
 * notifications is active, we should properly stop it before freeing session
 * structure. This should be called after nc_session_close() but before
 * doing stuff in nc_session_free().
 *
 */
static void ncntf_dispatch_stop(struct nc_session *session)
{
	DBG_LOCK("mut_ntf");
	pthread_mutex_lock(&(session->mut_ntf));
	if (session != NULL && session->ntf_active) {
		session->ntf_stop = 1;
		if (session->status == NC_SESSION_STATUS_WORKING) {
			announce_nc_session_closing(session);
		}
		while (session->ntf_active) {
			DBG_UNLOCK("mut_ntf");
			pthread_mutex_unlock(&(session->mut_ntf));
			DBG_UNLOCK("mut_session");
			pthread_mutex_unlock(&(session->mut_session));
			usleep(NCNTF_DISPATCH_SLEEP);
			DBG_LOCK("mut_session");
			pthread_mutex_lock(&(session->mut_session));
			DBG_LOCK("mut_ntf");
			pthread_mutex_lock(&(session->mut_ntf));
		}
	}
	DBG_UNLOCK("mut_ntf");
	pthread_mutex_unlock(&(session->mut_ntf));
}

void nc_session_close(struct nc_session* session, NC_SESSION_TERM_REASON reason)
{
	int i;
	struct nc_msg *qmsg, *qmsg_aux;
	NC_SESSION_STATUS sstatus = session->status;

	/* lock session due to accessing its status and other items */
	if (sstatus != NC_SESSION_STATUS_DUMMY) {
		DBG_LOCK("mut_session");
		pthread_mutex_lock(&(session->mut_session));
	}

	/* close the SSH session */
	if (session != NULL && session->status != NC_SESSION_STATUS_CLOSING && session->status != NC_SESSION_STATUS_CLOSED) {

#ifndef DISABLE_NOTIFICATIONS
		if (!ncntf_dispatch) {
			/* let notification receiving/sending function stop, if any */
			ncntf_dispatch_stop(session);
		}

		/* log closing of the session */
		if (sstatus != NC_SESSION_STATUS_DUMMY) {
			ncntf_event_new(-1, NCNTF_BASE_SESSION_END, session, reason, NULL);
		}
#endif

		if (strcmp(session->session_id, INTERNAL_DUMMY_ID) != 0) {
			/*
			 * break all datastore locks held by the session,
			 * libnetconf's internal dummy sessions are excluded
			 */
			ncds_break_locks(session);
		}

		/* close NETCONF session */
#ifdef DISABLE_LIBSSH
		if (session->fd_output != -1) {
			if (session->status == NC_SESSION_STATUS_WORKING && !session->is_server) {
				announce_nc_session_closing(session);
			}
		} else
#else
		if (session->ssh_chan != NULL) {
			DBG_LOCK("mut_channel");
			pthread_mutex_lock(session->mut_channel);
			i = ssh_channel_is_eof(session->ssh_chan);
			DBG_UNLOCK("mut_channel");
			pthread_mutex_unlock(session->mut_channel);
			if (session->status == NC_SESSION_STATUS_WORKING &&  i == 0) {
				/* prevent infinite recursion when socket is corrupted */
				announce_nc_session_closing(session);
			}

			DBG_LOCK("mut_channel");
			pthread_mutex_lock(session->mut_channel);
			/* server SSH channel, do not close or free */
			if (!session->is_server) {
				ssh_channel_free(session->ssh_chan);
			}
			session->ssh_chan = NULL;
			DBG_UNLOCK("mut_channel");
			pthread_mutex_unlock(session->mut_channel);
		} else
#endif
#ifdef ENABLE_TLS
		if (session->tls != NULL) {
			if (session->status == NC_SESSION_STATUS_WORKING) {
				announce_nc_session_closing(session);
			}
			/* server TLS session, do not close or free */
			if (!session->is_server) {
				SSL_shutdown(session->tls);
				SSL_free(session->tls);
			}
			session->tls = NULL;
		}
#else
		{} /* close else which is not needed */
#endif
#ifndef DISABLE_LIBSSH
		if (!session->is_server) {
			if (session->ssh_sess != NULL && session->next == NULL && session->prev == NULL) {
				/* close and free only if there is no other session using it */
				ssh_disconnect(session->ssh_sess);
				ssh_free(session->ssh_sess);
				session->ssh_sess = NULL;

				close(session->transport_socket);
			}
			session->transport_socket = -1;
		}
#endif

		free(session->logintime);
		session->logintime = NULL;

		if (session->next == NULL && session->prev == NULL) {
			/* free only if there is no other session using it */
			free(session->hostname);
			free(session->username);
			free(session->port);

			/* also destroy shared mutexes */
			if (session->mut_channel != NULL) {
				pthread_mutex_destroy(session->mut_channel);
				free(session->mut_channel);
				session->mut_channel = NULL;
			}
		}
		session->username = NULL;
		session->hostname = NULL;
		session->port = NULL;

		/* remove messages from the queues */
		for (i = 0, qmsg = session->queue_event; i < 2; i++, qmsg = session->queue_msg) {
			while (qmsg != NULL) {
				qmsg_aux = qmsg->next;
				nc_msg_free(qmsg);
				qmsg = qmsg_aux;
			}
		}

		/*
		 * capabilities, session_id and shared monitoring structure are untouched
		 */

		/* successfully closed */
	}

	if (sstatus != NC_SESSION_STATUS_DUMMY) {
		session->status = NC_SESSION_STATUS_CLOSED;
		DBG_UNLOCK("mut_session");
		pthread_mutex_unlock(&(session->mut_session));
	} else {
		session->status = NC_SESSION_STATUS_CLOSED;
	}

	/* unlink the session from the list of related sessions */
	if (session->next != NULL) {
		session->next->prev = session->prev;
	}
	if (session->prev != NULL) {
		session->prev->next = session->next;
	}
	session->next = NULL;
	session->prev = NULL;
}


API void nc_session_free(struct nc_session* session)
{
	struct session_list_item* litem;
	int i;

	if (session == NULL) {
		return;
	}

	if (session->status != NC_SESSION_STATUS_CLOSED) {
		nc_session_close(session, NC_SESSION_TERM_CLOSED);
	}

	if (session->groups != NULL) {
		for (i = 0; session->groups[i] != NULL; i++) {
			free(session->groups[i]);
		}
		free(session->groups);
	}
	if (session->capabilities != NULL) {
		nc_cpblts_free(session->capabilities);
	}

	/* destroy mutexes */
	pthread_mutex_destroy(&(session->mut_mqueue));
	pthread_mutex_destroy(&(session->mut_equeue));
	pthread_mutex_destroy(&(session->mut_ntf));
	pthread_mutex_destroy(&(session->mut_session));

	if (session_list != NULL && session->monitored == 1) {
		/* remove from internal list if session is monitored */
		pthread_rwlock_wrlock(&(session_list->lock));
		if (session_list->count > 0) {
			for (litem = (struct session_list_item*) ((char*) (session_list->record) + session_list->first_offset);
					;
					litem = (struct session_list_item*) ((char*) litem + litem->offset_next)) {
				if (strcmp(litem->session_id, session->session_id) == 0) {
					/* we have matching record */
					litem->scounter--;
					if (litem->scounter == 0) {
						nc_session_monitor_remove(litem);
					}

					/* remove link from session statistics into the mapped file */
					session->stats = NULL;

					/* we are done */
					break;
				}

				/* end loop condition */
				if (litem->offset_next == 0) {
					/* no other item in the list */

					/* if the session's stats were not connected
					 * with internal monitoring list, so free it
					 */
					free(session->stats);

					/* end the loop */
					break; /* for loop */
				}
			}
		}
		pthread_rwlock_unlock(&(session_list->lock));
	} else {
		/* there is no internal session monitoring list so session's
		 * stats cannot be connected with it - free the structure
		 * directly from the session structure
		 */
		free(session->stats);
	}

	free (session);
}

API NC_SESSION_STATUS nc_session_get_status (const struct nc_session* session)
{
	if (session == NULL) {
		return (NC_SESSION_STATUS_ERROR);
	}

	return (session->status);
}

static int nc_session_send(struct nc_session* session, struct nc_msg *msg)
{
	ssize_t c = 0;
	int len, status;
	char *text;
#ifndef DISABLE_LIBSSH
	const char *emsg;
#endif
	char buf[1024];
	struct pollfd fds;
	int ret;

	if (session->fd_output == -1 && session->transport_socket == -1
#ifndef DISABLE_LIBSSH
			&& session->ssh_chan == NULL
#endif
#ifdef ENABLE_TLS
			&& session->tls == NULL
#endif
			&& 1) {
		return (EXIT_FAILURE);
	}

	/*
	 * maybe the previous check can be replaced by the following one, but
	 * using both cannot be wrong
	 */
	if (session->status != NC_SESSION_STATUS_WORKING &&
			session->status != NC_SESSION_STATUS_CLOSING) {
		return (EXIT_FAILURE);
	}

	/* check that we are able to write data */
	while (1) {
		fds.fd = -1;

		if (session->transport_socket != -1) {
			fds.fd = session->transport_socket;
		} else if (session->fd_output != -1) {
			fds.fd = session->fd_output;
		}
#ifndef DISABLE_LIBSSH
		else if (session->ssh_chan != NULL) {
			fds.fd = ssh_get_fd(ssh_channel_get_session(session->ssh_chan));
		}
#endif
#ifdef ENABLE_TLS
		else if (session->tls != NULL) {
			fds.fd = SSL_get_fd(session->tls);
		}
#endif

		if (fds.fd == -1) {
			ERROR("Invalid transport channel.");
			nc_session_close(session, NC_SESSION_TERM_DROPPED);
			return (EXIT_FAILURE);
		}

		fds.events = POLLOUT;
		fds.revents = 0;
		status = poll(&fds, 1, 0);

		if ((status == -1) && (errno == EINTR)) {
			/* poll was interrupted - try it again */
			continue;
		} else if (status < 0) {
			/* poll failed - something wrong happened */
			ERROR("Poll on output communication file descriptor failed (%s)", strerror(errno));
			return (EXIT_FAILURE);

		} else if (status > 0 && ((fds.revents & POLLHUP) || (fds.revents & POLLERR))) {
			/* close pipe/fd - other side already did it */
			ERROR("Communication dropped.");
			nc_session_close(session, NC_SESSION_TERM_DROPPED);
			return (EXIT_FAILURE);
		}
		break;
	}

	/* lock the session for sending the data */

	xmlDocDumpFormatMemory (msg->doc, (xmlChar**) (&text), &len, NC_CONTENT_FORMATTED);
	DBG("Writing message (session %s): %s", session->session_id, text);

	DBG_LOCK("mut_channel");
	session->mut_channel_flag = 1;
	pthread_mutex_lock(session->mut_channel);
	/* if v1.1 send chunk information before message */
	if (session->version == NETCONFV11) {
		snprintf (buf, 1024, "\n#%d\n", (int) strlen (text));
		c = 0;
		do {
			NC_WRITE(session, &(buf[c]), c, ret);
			if (ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				usleep(10);
				continue;
			}
#ifndef DISABLE_LIBSSH
			if (ret == SSH_ERROR) {
				DBG_UNLOCK("mut_channel");
				session->mut_channel_flag = 0;
				pthread_mutex_unlock(session->mut_channel);
				if (!session->ssh_chan) {
					emsg = strerror(errno);
				} else if (session->ssh_chan && session->ssh_sess) {
					emsg = ssh_get_error(session->ssh_sess);
				} else {
					emsg = "description not available";
				}
				VERB("Writing data into the communication channel failed (%s).", emsg);
				free (text);
				return (EXIT_FAILURE);
			}
#endif
			if (ret <= 0) {
				DBG_UNLOCK("mut_channel");
				session->mut_channel_flag = 0;
				pthread_mutex_unlock(session->mut_channel);
				free (text);
				return (EXIT_FAILURE);
			}
		} while (c < (ssize_t) strlen (buf));
	}

	/* write the message */
	c = 0;
	do {
		NC_WRITE(session, &(text[c]), c, ret);
		if (ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			usleep(10);
			continue;
		}
#ifndef DISABLE_LIBSSH
		if (ret == SSH_ERROR) {
			DBG_UNLOCK("mut_channel");
			session->mut_channel_flag = 0;
			pthread_mutex_unlock(session->mut_channel);
			if (!session->ssh_chan) {
				emsg = strerror(errno);
			} else if (session->ssh_chan && session->ssh_sess) {
				emsg = ssh_get_error(session->ssh_sess);
			} else {
				emsg = "description not available";
			}
			VERB("Writing data into the communication channel failed (%s).", emsg);
			free (text);
			return (EXIT_FAILURE);
		}
#endif
		if (ret <= 0) {
			DBG_UNLOCK("mut_channel");
			session->mut_channel_flag = 0;
			pthread_mutex_unlock(session->mut_channel);
			free (text);
			return (EXIT_FAILURE);
		}
	} while (c < (ssize_t) strlen (text));
	free (text);

	/* close message */
	if (session->version == NETCONFV11) {
		text = NC_V11_END_MSG;
	} else { /* NETCONFV10 */
		text = NC_V10_END_MSG;
	}
	c = 0;
	do {
		NC_WRITE(session, &(text[c]), c, ret);
		if (ret < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			usleep(10);
			continue;
		}
#ifndef DISABLE_LIBSSH
		if (ret == SSH_ERROR) {
			DBG_UNLOCK("mut_channel");
			session->mut_channel_flag = 0;
			pthread_mutex_unlock(session->mut_channel);
			if (!session->ssh_chan) {
				emsg = strerror(errno);
			} else if (session->ssh_chan && session->ssh_sess) {
				emsg = ssh_get_error(session->ssh_sess);
			} else {
				emsg = "description not available";
			}
			VERB("Writing data into the communication channel failed (%s).", emsg);
			return (EXIT_FAILURE);
		}
#endif
		if (ret <= 0) {
			DBG_UNLOCK("mut_channel");
			session->mut_channel_flag = 0;
			pthread_mutex_unlock(session->mut_channel);
			return (EXIT_FAILURE);
		}
	} while (c < (ssize_t) strlen (text));

	/* unlock the session's output */
	DBG_UNLOCK("mut_channel");
	session->mut_channel_flag = 0;
	pthread_mutex_unlock(session->mut_channel);

	return (EXIT_SUCCESS);
}

static int nc_session_read_len(struct nc_session* session, size_t chunk_length, char **text, size_t *len)
{
	char *buf;
	ssize_t c;
	size_t rd = 0;
	long sleep_count = 0;
#ifdef ENABLE_TLS
	int r;
#endif

	/* check if we can work with the session */
	if (session->status != NC_SESSION_STATUS_WORKING &&
			session->status != NC_SESSION_STATUS_CLOSING) {
		return (EXIT_FAILURE);
	}

	buf = malloc ((chunk_length + 1) * sizeof(char));
	if (buf == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		*len = 0;
		*text = NULL;
		return (EXIT_FAILURE);
	}

	while (rd < chunk_length) {
		if ((READ_TIMEOUT * 1000000) / NC_READ_SLEEP == sleep_count) {
			ERROR("Reading timeout elapsed.");
			free(buf);
			*len = 0;
			*text = NULL;
			return (EXIT_FAILURE);
		}
#ifndef DISABLE_LIBSSH
		if (session->ssh_chan) {
			/* read via libssh */
			c = ssh_channel_read(session->ssh_chan, &(buf[rd]), chunk_length - rd, 0);
			if (c == SSH_AGAIN) {
				usleep (NC_READ_SLEEP);
				++sleep_count;
				continue;
			} else if (c == SSH_ERROR) {
				ERROR("Reading from the SSH channel failed (%zd: %s)", ssh_get_error_code(session->ssh_sess), ssh_get_error(session->ssh_sess));
				free (buf);
				*len = 0;
				*text = NULL;
				return (EXIT_FAILURE);
			} else if (c == 0) {
				if (ssh_channel_is_eof(session->ssh_chan)) {
					ERROR("Server has closed the communication socket");
					free (buf);
					*len = 0;
					*text = NULL;
					return (EXIT_FAILURE);
				}
				usleep (NC_READ_SLEEP);
				++sleep_count;
				continue;
			}
		} else
#endif
#ifdef ENABLE_TLS
		if (session->tls) {
			/* read via OpenSSL */
			c = SSL_read(session->tls, &(buf[rd]), chunk_length - rd);
			if (c <= 0 && (r = SSL_get_error(session->tls, c))) {
				if (r == SSL_ERROR_WANT_READ) {
					usleep(NC_READ_SLEEP);
					++sleep_count;
					continue;
				} else {
					if (r == SSL_ERROR_SYSCALL) {
						ERROR("Reading from the TLS session failed (%s)", strerror(errno));
					} else if (r == SSL_ERROR_SSL) {
						ERROR("Reading from the TLS session failed (%s)", ERR_error_string(r, NULL));
					} else {
						ERROR("Reading from the TLS session failed (SSL code %d)", r);
					}
					free (buf);
					if (len != NULL) {
						*len = 0;
					}
					if (text != NULL) {
						*text = NULL;
					}
					return (EXIT_FAILURE);
				}
			}
		} else
#endif
		if (session->fd_input != -1) {
			/* read via file descriptor */
			c = read (session->fd_input, &(buf[rd]), chunk_length - rd);
			if (c == -1) {
				if (errno == EAGAIN) {
					usleep (NC_READ_SLEEP);
					++sleep_count;
					continue;
				} else {
					ERROR("Reading from an input file descriptor failed (%s)", strerror(errno));
					free (buf);
					*len = 0;
					*text = NULL;
					return (EXIT_FAILURE);
				}
			}
		} else {
			ERROR("No way to read the input, fatal error.");
			free (buf);
			*len = 0;
			*text = NULL;
			return (EXIT_FAILURE);
		}

		rd += c;
	}

	/* add terminating null byte */
	buf[rd] = 0;

	*len = rd;
	*text = buf;
	return (EXIT_SUCCESS);
}

static int nc_session_read_until(struct nc_session* session, const char* endtag, unsigned int limit, char **text, size_t *len)
{
	size_t rd = 0;
	ssize_t c;
	char *buf = NULL;
	size_t buflen = 0;
	long sleep_count = 0;
#ifdef ENABLE_TLS
	int r;
#endif

	/* check if we can work with the session */
	if (session->status != NC_SESSION_STATUS_WORKING &&
			session->status != NC_SESSION_STATUS_CLOSING) {
		return (EXIT_FAILURE);
	}

	if (endtag == NULL) {
		return (EXIT_FAILURE);
	}

	/* set starting buffer size */
	buflen = 1024;
	buf = (char*) malloc (buflen * sizeof(char));
	if (buf == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (EXIT_FAILURE);
	}

	for (rd = 0;;) {
		if (limit > 0 && rd > limit) {
			free(buf);
			WARN("%s: reading limit reached.", __func__);
			return (EXIT_FAILURE);
		}

		if ((READ_TIMEOUT * 1000000) / NC_READ_SLEEP == sleep_count) {
			free(buf);
			ERROR("Reading timeout elapsed.");
			return (EXIT_FAILURE);
		}
#ifndef DISABLE_LIBSSH
		if (session->ssh_chan) {
			/* read via libssh */
			c = ssh_channel_read(session->ssh_chan, &(buf[rd]), 1, 0);
			if (c == SSH_AGAIN) {
				usleep (NC_READ_SLEEP);
				++sleep_count;
				continue;
			} else if (c == SSH_ERROR) {
				if (session->ssh_sess != NULL) {
					ERROR("Reading from the SSH channel failed (%zd: %s)", ssh_get_error_code(session->ssh_sess), ssh_get_error(session->ssh_sess));
				} else {
					ERROR("Reading from the SSH channel failed");
				}
				free (buf);
				if (len != NULL) {
					*len = 0;
				}
				if (text != NULL) {
					*text = NULL;
				}
				return (EXIT_FAILURE);
			} else if (c == 0) {
				if (ssh_channel_is_eof (session->ssh_chan)) {
					ERROR("Server has closed the communication socket");
					free (buf);
					if (len != NULL) {
						*len = 0;
					}
					if (text != NULL) {
						*text = NULL;
					}
					return (EXIT_FAILURE);
				}
				usleep (NC_READ_SLEEP);
				++sleep_count;
				continue;
			}
		} else
#endif
#ifdef ENABLE_TLS
		if (session->tls) {
			/* read via OpenSSL */
			c = SSL_read(session->tls, &(buf[rd]), 1);
			if (c <= 0 && (r = SSL_get_error(session->tls, c))) {
				if (r == SSL_ERROR_WANT_READ) {
					usleep(NC_READ_SLEEP);
					++sleep_count;
					continue;
				} else {
					if (r == SSL_ERROR_SYSCALL) {
						ERROR("Reading from the TLS session failed (%s)", strerror(errno));
					} else if (r == SSL_ERROR_SSL) {
						ERROR("Reading from the TLS session failed (%s)", ERR_error_string(r, NULL));
					} else {
						ERROR("Reading from the TLS session failed (SSL code %d)", r);
					}
					free (buf);
					if (len != NULL) {
						*len = 0;
					}
					if (text != NULL) {
						*text = NULL;
					}
					return (EXIT_FAILURE);
				}
			}
		} else
#endif
		if (session->fd_input != -1) {
			/* read via file descriptor */
			c = read (session->fd_input, &(buf[rd]), 1);
			if (c == -1) {
				if (errno == EAGAIN) {
					usleep (NC_READ_SLEEP);
					++sleep_count;
					continue;
				} else {
					ERROR("Reading from an input file descriptor failed (%s)", strerror(errno));
					free (buf);
					if (len != NULL) {
						*len = 0;
					}
					if (text != NULL) {
						*text = NULL;
					}
					return (EXIT_FAILURE);
				}
			} else if (c == 0) {
				ERROR("EOF received (%s)", strerror(errno));
				free(buf);
				if (len != NULL ) {
					*len = 0;
				}
				if (text != NULL ) {
					*text = NULL;
				}
				return (EXIT_FAILURE);
			}
		} else {
			ERROR("No way to read the input, fatal error.");
			free (buf);
			if (len != NULL) {
				*len = 0;
			}
			if (text != NULL) {
				*text = NULL;
			}
			return (EXIT_FAILURE);
		}

		rd += c; /* should be rd++ */
		buf[rd] = '\0';

		if ((rd) < strlen (endtag)) {
			/* not enough data to compare with endtag */
			continue;
		} else {
			/* compare with endtag */
			if (strcmp (endtag, &(buf[rd - strlen (endtag)])) == 0) {
				/* end tag found */
				if (len != NULL) {
					*len = rd;
				}
				if (text != NULL) {
					*text = buf;
				} else {
					free(buf);
				}
				return (EXIT_SUCCESS);
			}
		}

		/* resize buffer if needed */
		if (rd == (buflen-1)) {
			/* get more memory for the text */
			void *tmp = realloc (buf, (2 * buflen) * sizeof(char));
			if (tmp == NULL) {
				ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
				if (len != NULL) {
					*len = 0;
				}
				if (text != NULL) {
					*text = NULL;
				}
				free(buf);
				return (EXIT_FAILURE);
			}
			buf = tmp;
			buflen = 2 * buflen;
		}
	}

	/* no one could be here */
	ERROR("Reading NETCONF message fatal failure");
	free (buf);
	if (len != NULL) {
		*len = 0;
	}
	if (text != NULL) {
		*text = NULL;
	}
	return (EXIT_FAILURE);
}

/**
 * @brief Get the message id string from the NETCONF message
 *
 * @param[in] msg NETCONF message to parse.
 * @return 0 on error,\n message-id of the message on success.
 */
const nc_msgid nc_msg_parse_msgid(const struct nc_msg *msg)
{
	nc_msgid ret = NULL;
	xmlAttrPtr prop;

	/* parse and store message-id */
	prop = xmlHasProp(xmlDocGetRootElement(msg->doc), BAD_CAST "message-id");
	if (prop != NULL && prop->children != NULL && prop->children->content != NULL) {
		ret = (char*)prop->children->content;
	}
	if (ret == NULL) {
		if (xmlStrcmp (xmlDocGetRootElement(msg->doc)->name, BAD_CAST "hello") != 0) {
			WARN("Missing message-id in %s.", (char*)xmlDocGetRootElement(msg->doc)->name);
			ret = NULL;
		} else {
			ret = "hello";
		}
	}

	return (ret);
}

static NC_MSG_TYPE nc_session_receive(struct nc_session* session, int timeout, struct nc_msg** msg)
{
	struct nc_msg *retval;
	nc_reply* reply;
	const char* id;
	const char *emsg;
	char *text = NULL, *tmp_text, *chunk = NULL;
	size_t len;
	unsigned long long int text_size = 0, total_len = 0;
	size_t chunk_length;
	struct pollfd fds;
	int status;
	unsigned long int revents;
	NC_MSG_TYPE msgtype;
	xmlNodePtr root;

	if (session == NULL || (session->status != NC_SESSION_STATUS_WORKING && session->status != NC_SESSION_STATUS_CLOSING)) {
		ERROR("Invalid session to receive data.");
		return (NC_MSG_UNKNOWN);
	}

	/* if there is waiting sending thread, sleep for timeout and return
	 * with NC_MSG_WOULDBLOCK because we probably relock the mut_channel quite
	 * fast, so we are trying to avoid starvation this way
	 */
	if (session->mut_channel_flag) {
		usleep(timeout ? timeout : 1);
		return (NC_MSG_WOULDBLOCK);
	}

	/* lock the session for receiving */
	DBG_LOCK("mut_channel");
	pthread_mutex_lock(session->mut_channel);

	/* use while for possibility of repeating test */
	while(1) {
		revents = 0;
#ifndef DISABLE_LIBSSH
		if (session->ssh_chan != NULL) {
			/* we are getting data from libssh's channel */
			status = ssh_channel_poll_timeout(session->ssh_chan, timeout, 0);
			if (status > 0) {
				revents = POLLIN;
			}
		} else
#endif
#ifdef ENABLE_TLS
		if (session->tls != NULL) {
			/* we are getting data from TLS session using OpenSSL */
			fds.fd = SSL_get_fd(session->tls);
			fds.events = POLLIN;
			fds.revents = 0;
			status = poll(&fds, 1, timeout);

			revents = (unsigned long int) fds.revents;
		} else
#endif
		if (session->fd_input != -1) {
			/* we are getting data from standard file descriptor */
			fds.fd = session->fd_input;
			fds.events = POLLIN;
			fds.revents = 0;
			status = poll(&fds, 1, timeout);

			revents = (unsigned long int) fds.revents;
		} else {
			ERROR("Invalid session to receive data.");
			return (NC_MSG_UNKNOWN);
		}

		/* process the result */
		if (status == 0) {
			/* timed out */
			DBG_UNLOCK("mut_channel");
			pthread_mutex_unlock(session->mut_channel);
			return (NC_MSG_WOULDBLOCK);
		} else if (((status == -1) && (errno == EINTR))
#ifndef DISABLE_LIBSSH
				|| (status == SSH_AGAIN)
#endif
				) {
			/* poll was interrupted */
			continue;
		} else if (status < 0) {
			/* poll failed - something wrong happend, close this socket and wait for another request */
			DBG_UNLOCK("mut_channel");
			pthread_mutex_unlock(session->mut_channel);
#ifndef DISABLE_LIBSSH
			if (status == SSH_EOF) {
				emsg = "end of file";
			} else if (!session->ssh_chan) {
				emsg = strerror(errno);
			} else if (session->ssh_sess) {
				emsg = ssh_get_error(session->ssh_sess);
			} else {
				emsg = "description not available";
			}
#else
			emsg = strerror(errno);
#endif
			ERROR("Input channel error (%s)", emsg);
			nc_session_close(session, NC_SESSION_TERM_DROPPED);
			if (nc_info) {
				pthread_rwlock_wrlock(&(nc_info->lock));
				nc_info->stats.sessions_dropped++;
				pthread_rwlock_unlock(&(nc_info->lock));
			}
			return (NC_MSG_UNKNOWN);

		}
		/* status > 0 */
		/* check the status of the socket */
		/* if nothing to read and POLLHUP (EOF) or POLLERR set */
		if ((revents & POLLHUP) || (revents & POLLERR)) {
			/* close client's socket (it's probably already closed by client */
			DBG_UNLOCK("mut_channel");
			pthread_mutex_unlock(session->mut_channel);
			ERROR("Input channel closed");
			nc_session_close(session, NC_SESSION_TERM_DROPPED);
			if (nc_info) {
				pthread_rwlock_wrlock(&(nc_info->lock));
				nc_info->stats.sessions_dropped++;
				pthread_rwlock_unlock(&(nc_info->lock));
			}
			return (NC_MSG_UNKNOWN);
		}

		/* we have something to read */
		break;
	}

	switch (session->version) {
	case NETCONFV10:
		if (nc_session_read_until (session, NC_V10_END_MSG, 0, &text, &len) != 0) {
			goto malformed_msg_channels_unlock;
		}
		text[len - strlen (NC_V10_END_MSG)] = 0;
		DBG("Received message (session %s): %s", session->session_id, text);
		break;
	case NETCONFV11:
		do {
			if (nc_session_read_until (session, "\n#", 2, NULL, NULL) != 0) {
				if (total_len > 0) {
					free (text);
				}
				goto malformed_msg_channels_unlock;
			}
			if (nc_session_read_until (session, "\n", 0, &chunk, &len) != 0) {
				if (total_len > 0) {
					free (text);
				}
				goto malformed_msg_channels_unlock;
			}
			if (strcmp (chunk, "#\n") == 0) {
				/* end of chunked framing message */
				free (chunk);
				break;
			}

			/* convert string to the size of the following chunk */
			chunk_length = strtoul (chunk, (char **) NULL, 10);
			if (chunk_length == 0) {
				ERROR("Invalid frame chunk size detected, fatal error.");
				goto malformed_msg_channels_unlock;
			}
			free (chunk);
			chunk = NULL;

			/* now we have size of next chunk, so read the chunk */
			if (nc_session_read_len (session, chunk_length, &chunk, &len) != 0) {
				if (total_len > 0) {
					free (text);
				}
				goto malformed_msg_channels_unlock;
			}

			/*
			 * realloc resulting text buffer if needed (always needed now)
			 * don't forget count terminating null byte
			 * */
			if (text_size < (total_len + len + 1)) {
				char *tmp = realloc (text, total_len + len + 1);
				if (tmp == NULL) {
					ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
					free(text);
					goto malformed_msg_channels_unlock;
				}
				text = tmp;
				text[total_len] = '\0';
				text_size = total_len + len + 1;
			}
			memcpy(text + total_len, chunk, len);
			total_len += len;
			text[total_len] = '\0';
			free (chunk);
			chunk = NULL;

		} while (1);
		DBG("Received message (session %s): %s", session->session_id, text);
		break;
	default:
		ERROR("Unsupported NETCONF protocol version (%d)", session->version);
		goto malformed_msg_channels_unlock;
		break;
	}

	DBG_UNLOCK("mut_channel");
	pthread_mutex_unlock(session->mut_channel);

	if (text == NULL) {
		ERROR("Empty message received (session %s)", session->session_id);
		goto malformed_msg;
	}

	retval = calloc (1, sizeof(struct nc_msg));
	if (retval == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		free (text);
		goto malformed_msg;
	}

	/* skip leading whitespaces */
	tmp_text=text;
	while (isspace(*tmp_text)) {
		tmp_text++;
	}
	/* store the received message in libxml2 format */
	retval->doc = xmlReadDoc (BAD_CAST tmp_text, NULL, NULL, NC_XMLREAD_OPTIONS);
	if (retval->doc == NULL) {
		free (retval);
		free (text);
		ERROR("Invalid XML data received.");
		goto malformed_msg;
	}
	free (text);

	/* create xpath evaluation context */
	if ((retval->ctxt = xmlXPathNewContext(retval->doc)) == NULL) {
		ERROR("%s: rpc message XPath context cannot be created.", __func__);
		nc_msg_free(retval);
		goto malformed_msg;
	}

	/* register base namespace for the rpc */
	if (xmlXPathRegisterNs(retval->ctxt, BAD_CAST NC_NS_BASE10_ID, BAD_CAST NC_NS_BASE10) != 0) {
		ERROR("Registering base namespace for the message xpath context failed.");
		nc_msg_free(retval);
		goto malformed_msg;
	}
	if (xmlXPathRegisterNs(retval->ctxt, BAD_CAST NC_NS_NOTIFICATIONS_ID, BAD_CAST NC_NS_NOTIFICATIONS) != 0) {
		ERROR("Registering notifications namespace for the message xpath context failed.");
		nc_msg_free(retval);
		goto malformed_msg;
	}
	if (xmlXPathRegisterNs(retval->ctxt, BAD_CAST NC_NS_WITHDEFAULTS_ID, BAD_CAST NC_NS_WITHDEFAULTS) != 0) {
		ERROR("Registering with-defaults namespace for the message xpath context failed.");
		nc_msg_free(retval);
		goto malformed_msg;
	}
	if (xmlXPathRegisterNs(retval->ctxt, BAD_CAST NC_NS_MONITORING_ID, BAD_CAST NC_NS_MONITORING) != 0) {
		ERROR("Registering monitoring namespace for the message xpath context failed.");
		nc_msg_free(retval);
		goto malformed_msg;
	}

	/* parse and store message type */
	root = xmlDocGetRootElement(retval->doc);
	/* check namespace */
	if (!root->ns || !root->ns->href || xmlStrcmp(root->ns->href, BAD_CAST NC_NS_BASE10)) {
		/* event notification ? */
		if (root->ns && root->ns->href && !xmlStrcmp(root->ns->href, BAD_CAST NC_NS_NOTIFICATIONS) &&
				!xmlStrcmp(root->name, BAD_CAST "notification")) {
			/* we have notification */
			msgtype = NC_MSG_NOTIFICATION;
		} else {
			/* bad namespace */
			WARN("Unknown (unsupported) namespace of the received message root element.");
			retval->type.rpc = NC_RPC_UNKNOWN;
			msgtype = NC_MSG_UNKNOWN;
		}

	} else 	if (xmlStrcmp (root->name, BAD_CAST "rpc-reply") == 0) {
		msgtype = NC_MSG_REPLY;

		/* set reply type flag */
		nc_reply_parse_type(retval);

	} else if (xmlStrcmp (root->name, BAD_CAST "rpc") == 0) {
		msgtype = NC_MSG_RPC;

		/* set with-defaults if any */
		nc_rpc_parse_withdefaults(retval, NULL);
	} else if (xmlStrcmp (root->name, BAD_CAST "hello") == 0) {
		/* set message type, we have <hello> message */
		retval->type.reply = NC_REPLY_HELLO;
		msgtype = NC_MSG_HELLO;
	} else {
		WARN("Unknown (unsupported) type of received message detected.");
		retval->type.rpc = NC_RPC_UNKNOWN;
		msgtype = NC_MSG_UNKNOWN;
	}

	if (msgtype == NC_MSG_RPC || msgtype == NC_MSG_REPLY) {
		/* parse and store message-id */
		if ((id = nc_msg_parse_msgid(retval)) == NULL) {
			retval->msgid = NULL;
		} else {
			retval->msgid = strdup(id);
		}
	} else {
		retval->msgid = NULL;
	}

	/* return the result */
	*msg = retval;
	(*msg)->session = session;
	return (msgtype);

malformed_msg_channels_unlock:
	DBG_UNLOCK("mut_channel");
	pthread_mutex_unlock(session->mut_channel);

malformed_msg:
	if (session->version == NETCONFV11 && session->ssh_sess == NULL) {
		/* NETCONF version 1.1 define sending error reply from the server */
		reply = nc_reply_error(nc_err_new(NC_ERR_MALFORMED_MSG));
		if (reply == NULL) {
			ERROR("Unable to create the \'Malformed message\' reply");
			nc_session_close(session, NC_SESSION_TERM_OTHER);
			return (NC_MSG_UNKNOWN);
		}

		if (nc_session_send_reply(session, NULL, reply) == 0) {
			ERROR("Unable to send the \'Malformed message\' reply");
			nc_session_close(session, NC_SESSION_TERM_OTHER);
			return (NC_MSG_UNKNOWN);
		}
		nc_reply_free(reply);
	}

	ERROR("Malformed message received, closing the session %s.", session->session_id);
	nc_session_close(session, NC_SESSION_TERM_OTHER);

	return (NC_MSG_UNKNOWN);
}

static NC_MSG_TYPE nc_session_recv_msg(struct nc_session* session, int timeout, struct nc_msg** msg)
{
	NC_MSG_TYPE ret;

	ret = nc_session_receive (session, timeout, msg);
	switch (ret) {
	case NC_MSG_REPLY: /* regular reply received */
	case NC_MSG_HELLO:
	case NC_MSG_NOTIFICATION:
	case NC_MSG_WOULDBLOCK:
		/* do nothing, just return the type */
		break;
	default:
		ret = NC_MSG_UNKNOWN;
		break;
	}

	return (ret);
}

#define LOCAL_RECEIVE_TIMEOUT 100
API NC_MSG_TYPE nc_session_recv_reply(struct nc_session* session, int timeout, nc_reply** reply)
{
	struct nc_msg *msg_aux, *msg = NULL;
	NC_MSG_TYPE ret;
	struct nc_err* error;

	/* use local timeout to avoid continual long time blocking */
	int local_timeout;

	if (timeout == 0) {
		local_timeout = 0;
	} else {
		local_timeout = LOCAL_RECEIVE_TIMEOUT;
	}

	DBG_LOCK("mut_mqueue");
	pthread_mutex_lock(&(session->mut_mqueue));

try_again:
	if (session->queue_msg != NULL) {
		/* pop the oldest reply from the queue */
		*reply = (nc_reply*)(session->queue_msg);
		session->queue_msg = (*reply)->next;
		DBG_UNLOCK("mut_mqueue");
		pthread_mutex_unlock(&(session->mut_mqueue));
		(*reply)->next = NULL;
		return (NC_MSG_REPLY);
	}

	ret = nc_session_recv_msg(session, local_timeout, &msg);

	switch (ret) {
	case NC_MSG_REPLY: /* regular reply received */
		/* if specified callback for processing rpc-error, use it */
		if (nc_reply_get_type (msg) == NC_REPLY_ERROR &&
				callbacks.process_error_reply != NULL) {
			/* process rpc-error msg */
			for (error = msg->error; error != NULL; error = error->next) {
				callbacks.process_error_reply(error->tag,
						error->type,
						error->severity,
						error->apptag,
						error->path,
						error->message,
						error->attribute,
						error->element,
						error->ns,
						error->sid);
			}
			/* free the data */
			nc_reply_free(msg);
			ret = NC_MSG_NONE;
		} else {
			*reply = (nc_reply*)msg;
		}
		break;
	case NC_MSG_HELLO: /* hello message received */
		*reply = (nc_reply*)msg;
		break;
	case NC_MSG_NONE:
		/* <rpc-reply> with error information was processed automatically */
		break;
	case NC_MSG_WOULDBLOCK:
		if ((timeout == -1) || ((timeout > 0) && ((timeout = timeout - local_timeout) > 0))) {
			goto try_again;
		}
		break;
	case NC_MSG_NOTIFICATION:
		/* add event notification into the session's list of notification messages */
		DBG_LOCK("mut_equeue");
		pthread_mutex_lock(&(session->mut_equeue));
		msg_aux = session->queue_event;
		if (msg_aux == NULL) {
			msg->next = session->queue_event;
			session->queue_event = msg;
		} else {
			for (; msg_aux->next != NULL; msg_aux = msg_aux->next);
			msg_aux->next = msg;
		}
		DBG_UNLOCK("mut_equeue");
		pthread_mutex_unlock(&(session->mut_equeue));

		break;
	default:
		nc_msg_free(msg);
		ret = NC_MSG_UNKNOWN;
		break;
	}

	/* session lock is no more needed */
	DBG_UNLOCK("mut_mqueue");
	pthread_mutex_unlock(&(session->mut_mqueue));

	return (ret);
}

API int nc_session_send_notif(struct nc_session* session, const nc_ntf* ntf)
{
	int ret;
	struct nc_msg *msg;

	DBG_LOCK("mut_session");
	pthread_mutex_lock(&(session->mut_session));

	if (session == NULL || (session->status != NC_SESSION_STATUS_WORKING && session->status != NC_SESSION_STATUS_CLOSING)) {
		ERROR("Invalid session to send <notification>.");
		DBG_UNLOCK("mut_session");
		pthread_mutex_unlock(&(session->mut_session));
		return (EXIT_FAILURE);
	}

	msg = nc_msg_dup ((struct nc_msg*) ntf);

	/* send message */
	ret = nc_session_send (session, msg);

	DBG_UNLOCK("mut_session");
	pthread_mutex_unlock(&(session->mut_session));

	nc_msg_free (msg);

	if (ret == EXIT_SUCCESS) {
		/* update stats */
		session->stats->out_notifications++;
		if (nc_info) {
			pthread_rwlock_wrlock(&(nc_info->lock));
			nc_info->stats.counters.out_notifications++;
			pthread_rwlock_unlock(&(nc_info->lock));
		}
	}

	return (ret);
}

API NC_MSG_TYPE nc_session_recv_notif(struct nc_session* session, int timeout, nc_ntf** ntf)
{
	struct nc_msg *msg_aux, *msg=NULL;
	NC_MSG_TYPE ret;

	/* use local timeout to avoid continual long time blocking */
	int local_timeout;

	if (timeout == 0) {
		local_timeout = 0;
	} else {
		local_timeout = LOCAL_RECEIVE_TIMEOUT;
	}

	DBG_LOCK("mut_equeue");
	pthread_mutex_lock(&(session->mut_equeue));

try_again:
	if (session->queue_event != NULL) {
		/* pop the oldest reply from the queue */
		*ntf = (nc_reply*)(session->queue_event);
		session->queue_event = (*ntf)->next;
		DBG_UNLOCK("mut_equeue");
		pthread_mutex_unlock(&(session->mut_equeue));
		(*ntf)->next = NULL;
		return (NC_MSG_NOTIFICATION);
	}

	ret = nc_session_recv_msg(session, local_timeout, &msg);

	switch (ret) {
	case NC_MSG_REPLY: /* regular reply received */
		/* add reply into the session's list of reply messages */
		msg_aux = session->queue_msg;
		if (msg_aux == NULL) {
			msg->next = session->queue_msg;
			session->queue_msg = msg;
		} else {
			for (; msg_aux->next != NULL; msg_aux = msg_aux->next);
			msg_aux->next = msg;
		}
		break;
	case NC_MSG_NONE:
		/* <rpc-reply> with error information was processed
		 * automatically, but we are waiting for a notification
		 */
		goto try_again;
		break;
	case NC_MSG_WOULDBLOCK:
		if ((timeout == -1) || ((timeout > 0) && ((timeout = timeout - local_timeout) > 0))) {
			goto try_again;
		}
		break;
	case NC_MSG_NOTIFICATION:
		*ntf = (nc_reply*)msg;
		break;
	default:
		ret = NC_MSG_UNKNOWN;
		break;
	}

	DBG_UNLOCK("mut_equeue");
	pthread_mutex_unlock(&(session->mut_equeue));

	return (ret);
}

API NC_MSG_TYPE nc_session_recv_rpc(struct nc_session* session, int timeout, nc_rpc** rpc)
{
	NC_MSG_TYPE ret;
	NC_OP op;
	struct nc_err* e = NULL;
	nc_reply* reply;
	unsigned int *counter = NULL;

	/* use local timeout to avoid continual long time blocking */
	int local_timeout;

	if (timeout == 0) {
		local_timeout = 0;
	} else {
		local_timeout = LOCAL_RECEIVE_TIMEOUT;
	}

try_again:
	ret = nc_session_receive (session, local_timeout, (struct nc_msg**) rpc);
	switch (ret) {
	case NC_MSG_RPC:
		(*rpc)->with_defaults = nc_rpc_parse_withdefaults(*rpc, session);

		/* check for with-defaults capability */
		if ((*rpc)->with_defaults != NCWD_MODE_NOTSET) {
			/* check if the session support this */
			if (session->wd_basic == NCWD_MODE_NOTSET) {
				ERROR("rpc requires the with-defaults capability, but the session does not support it.");
				e = nc_err_new(NC_ERR_INVALID_VALUE);
				nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "with-defaults");
				nc_err_set(e, NC_ERR_PARAM_MSG, "rpc requires the with-defaults capability, but the session does not support it.");
			} else {
				switch ((*rpc)->with_defaults) {
				case NCWD_MODE_ALL:
					if ((session->wd_modes & NCWD_MODE_ALL) == 0) {
						ERROR("rpc requires the with-defaults capability report-all mode, but the session does not support it.");
						e = nc_err_new(NC_ERR_INVALID_VALUE);
						nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "with-defaults");
						nc_err_set(e, NC_ERR_PARAM_MSG, "rpc requires the with-defaults capability report-all mode, but the session does not support it.");
					}
					break;
				case NCWD_MODE_ALL_TAGGED:
					if ((session->wd_modes & NCWD_MODE_ALL_TAGGED) == 0) {
						ERROR("rpc requires the with-defaults capability report-all-tagged mode, but the session does not support it.");
						e = nc_err_new(NC_ERR_INVALID_VALUE);
						nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "with-defaults");
						nc_err_set(e, NC_ERR_PARAM_MSG, "rpc requires the with-defaults capability report-all-tagged mode, but the session does not support it.");
					}
					break;
				case NCWD_MODE_TRIM:
					if ((session->wd_modes & NCWD_MODE_TRIM) == 0) {
						ERROR("rpc requires the with-defaults capability trim mode, but the session does not support it.");
						e = nc_err_new(NC_ERR_INVALID_VALUE);
						nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "with-defaults");
						nc_err_set(e, NC_ERR_PARAM_MSG, "rpc the requires with-defaults capability trim mode, but the session does not support it.");
					}
					break;
				case NCWD_MODE_EXPLICIT:
					if ((session->wd_modes & NCWD_MODE_EXPLICIT) == 0) {
						ERROR("rpc requires the with-defaults capability explicit mode, but the session does not support it.");
						e = nc_err_new(NC_ERR_INVALID_VALUE);
						nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "with-defaults");
						nc_err_set(e, NC_ERR_PARAM_MSG, "rpc requires the with-defaults capability explicit mode, but the session does not support it.");
					}
					break;
				default: /* something weird */
					ERROR("rpc requires the with-defaults capability with an unknown mode.");
					e = nc_err_new(NC_ERR_INVALID_VALUE);
					nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "with-defaults");
					nc_err_set(e, NC_ERR_PARAM_MSG, "rpc requires the with-defaults capability with an unknown mode.");
					break;
				}
			}

			if (e != NULL) {
				counter = &nc_info->stats.counters.in_bad_rpcs;
				goto replyerror;
			}
		}
		/* update statistics */
		session->stats->in_rpcs++;
		if (nc_info) {
			pthread_rwlock_wrlock(&(nc_info->lock));
			nc_info->stats.counters.in_rpcs++;
			pthread_rwlock_unlock(&(nc_info->lock));
		}

		/* NACM init */
		nacm_start(*rpc, session);

		/* NACM - check operation access */
		if (nacm_check_operation(*rpc) != NACM_PERMIT) {
			e = nc_err_new(NC_ERR_ACCESS_DENIED);
			nc_err_set(e, NC_ERR_PARAM_MSG, "Operation not permitted.");
			counter = &nc_info->stats_nacm.denied_ops;
			goto replyerror;
		}

		/* assign operation value */
		op = nc_rpc_assign_op(*rpc);

		/* set rpc type flag */
		nc_rpc_parse_type(*rpc);

		/* assign source/target datastore types */
		if (op == NC_OP_GETCONFIG || op == NC_OP_COPYCONFIG || op == NC_OP_VALIDATE) {
			nc_rpc_assign_ds(*rpc, "source");
			if (!(*rpc)->source) {
				e = nc_err_new(NC_ERR_MISSING_ELEM);
				nc_err_set(e, NC_ERR_PARAM_TYPE, "protocol");
				nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "source");

				session->stats->in_bad_rpcs++;
				counter = &nc_info->stats.counters.in_bad_rpcs;
				goto replyerror;
			}
		}
		if (op == NC_OP_EDITCONFIG || op == NC_OP_COPYCONFIG || op == NC_OP_COMMIT
				|| op == NC_OP_DELETECONFIG || op == NC_OP_LOCK || op == NC_OP_UNLOCK) {
			nc_rpc_assign_ds(*rpc, "target");
			if (!(*rpc)->target) {
				e = nc_err_new(NC_ERR_MISSING_ELEM);
				nc_err_set(e, NC_ERR_PARAM_TYPE, "protocol");
				nc_err_set(e, NC_ERR_PARAM_INFO_BADELEM, "target");

				session->stats->in_bad_rpcs++;
				counter = &nc_info->stats.counters.in_bad_rpcs;
				goto replyerror;
			}
		}

		break;
	case NC_MSG_HELLO:
		/* do nothing, just return the type */
		break;
	case NC_MSG_WOULDBLOCK:
		if ((timeout == -1) || ((timeout > 0) && ((timeout = timeout - local_timeout) > 0))) {
			goto try_again;
		}
		break;
	default:
		ret = NC_MSG_UNKNOWN;

		/* update stats */
		session->stats->in_bad_rpcs++;
		if (nc_info) {
			pthread_rwlock_wrlock(&(nc_info->lock));
			nc_info->stats.counters.in_bad_rpcs++;
			pthread_rwlock_unlock(&(nc_info->lock));
		}

		break;
	}

	return (ret);

replyerror:

	reply = nc_reply_error(e);
	if (nc_session_send_reply(session, *rpc, reply) == 0) {
		ERROR("Failed to send reply.");
	}
	nc_rpc_free(*rpc);
	*rpc = NULL;
	nc_reply_free(reply);
	/* update stats */
	if (nc_info) {
		pthread_rwlock_wrlock(&(nc_info->lock));
		(*counter)++;
		pthread_rwlock_unlock(&(nc_info->lock));
	}

	return (NC_MSG_NONE); /* message processed internally */
}

API const nc_msgid nc_session_send_rpc(struct nc_session* session, nc_rpc *rpc)
{
	int ret;
	char msg_id_str[16];
	const char* wd;
	struct nc_msg *msg;
	NC_OP op;

	if (session == NULL || (session->status != NC_SESSION_STATUS_WORKING && session->status != NC_SESSION_STATUS_CLOSING)) {
		ERROR("Invalid session to send <rpc>.");
		return (NULL); /* failure */
	}

	if (rpc->type.rpc != NC_RPC_HELLO) {
		/* check for capabilities operations */
		op = nc_rpc_get_op(rpc);
		/* :notifications */
		switch (op) {
#ifndef DISABLE_NOTIFICATIONS
		case NC_OP_CREATESUBSCRIPTION:
			if (nc_cpblts_enabled(session, NC_CAP_NOTIFICATION_ID) == 0) {
				ERROR("RPC requires :notifications capability, but the session does not support it.");
				return (NULL); /* failure */
			}
			break;
#endif
		case NC_OP_COMMIT:
		case NC_OP_DISCARDCHANGES:
			if (nc_cpblts_enabled(session, NC_CAP_CANDIDATE_ID) == 0) {
				ERROR("RPC requires :candidate capability, but the session does not support it.");
				return (NULL); /* failure */
			}
			break;
		case NC_OP_GETSCHEMA:
			if (nc_cpblts_enabled(session, NC_CAP_MONITORING_ID) == 0) {
				ERROR("RPC requires :monitoring capability, but the session does not support it.");
				return (NULL); /* failure */
			}
			break;
		default:
			/* no check is needed */
			break;
		}

		/* check for with-defaults capability */
		if (rpc->with_defaults != NCWD_MODE_NOTSET) {
			/* check if the session support this */
			if ((wd = nc_cpblts_get(session->capabilities, NC_CAP_WITHDEFAULTS_ID)) == NULL) {
				ERROR("RPC requires :with-defaults capability, but the session does not support it.");
				return (NULL); /* failure */
			}
			switch (rpc->with_defaults) {
			case NCWD_MODE_ALL:
				if (strstr(wd, "report-all") == NULL) {
					ERROR("RPC requires the with-defaults capability report-all mode, but the session does not support it.");
					return (NULL); /* failure */
				}
				break;
			case NCWD_MODE_ALL_TAGGED:
				if (strstr(wd, "report-all-tagged") == NULL) {
					ERROR("RPC requires the with-defaults capability report-all-tagged mode, but the session does not support it.");
					return (NULL); /* failure */
				}
				break;
			case NCWD_MODE_TRIM:
				if (strstr(wd, "trim") == NULL) {
					ERROR("RPC requires the with-defaults capability trim mode, but the session does not support it.");
					return (NULL); /* failure */
				}
				break;
			case NCWD_MODE_EXPLICIT:
				if (strstr(wd, "explicit") == NULL) {
					ERROR("RPC requires the with-defaults capability explicit mode, but the session does not support it.");
					return (NULL); /* failure */
				}
				break;
			default: /* NCDFLT_MODE_DISABLED */
				/* nothing to check */
				break;
			}
		}
	}

	msg = nc_msg_dup ((struct nc_msg*) rpc);
	/* set message id */
	if (xmlStrcmp (xmlDocGetRootElement(msg->doc)->name, BAD_CAST "rpc") == 0) {
		/* lock the session due to accessing msgid item */
		DBG_LOCK("mut_session");
		pthread_mutex_lock(&(session->mut_session));
		sprintf (msg_id_str, "%llu", session->msgid++);
		DBG_UNLOCK("mut_session");
		pthread_mutex_unlock(&(session->mut_session));
		if (xmlNewProp(xmlDocGetRootElement(msg->doc), BAD_CAST "message-id", BAD_CAST msg_id_str) == NULL) {
			ERROR("xmlNewProp failed (%s:%d).", __FILE__, __LINE__);
			nc_msg_free (msg);
			return (NULL);
		}
	} else {
		/* hello message */
		sprintf (msg_id_str, "hello");
	}

	/* send message */
	ret = nc_session_send (session, msg);

	nc_msg_free (msg);

	if (ret != EXIT_SUCCESS) {
		if (rpc->type.rpc != NC_RPC_HELLO) {
			DBG_LOCK("mut_session");
			pthread_mutex_lock(&(session->mut_session));
			session->msgid--;
			DBG_UNLOCK("mut_session");
			pthread_mutex_unlock(&(session->mut_session));
		}
		return (NULL);
	} else {
		rpc->msgid = strdup(msg_id_str);
		return (rpc->msgid);
	}
}

API const nc_msgid nc_session_send_reply(struct nc_session* session, const nc_rpc* rpc, const nc_reply *reply)
{
	int ret;
	struct nc_msg *msg;
	const nc_msgid retval = NULL;
	xmlNsPtr ns;
	xmlNodePtr msg_root, rpc_root;

	if (reply == NULL) {
		ERROR("%s: Invalid <reply> message to send.", __func__);
		return (0); /* failure */
	}

	DBG_LOCK("mut_session");
	pthread_mutex_lock(&(session->mut_session));

	if (session == NULL || (session->status != NC_SESSION_STATUS_WORKING && session->status != NC_SESSION_STATUS_CLOSING)) {
		DBG_UNLOCK("mut_session");
		pthread_mutex_unlock(&(session->mut_session));

		ERROR("Invalid session to send <rpc-reply>.");
		return (0); /* failure */
	}

	msg = nc_msg_dup ((struct nc_msg*) reply);

	if (rpc != NULL) {
		/* get message id */
		if (rpc->msgid == 0) {
			/* parse and store message-id */
			retval = nc_msg_parse_msgid(rpc);
		} else {
			retval = rpc->msgid;
		}

		/* set message id */
		if (retval != NULL) {
			msg->msgid = strdup(retval);
		} else {
			msg->msgid = NULL;
		}
		msg_root = xmlDocGetRootElement(msg->doc);
		rpc_root = xmlDocGetRootElement(rpc->doc);
		if (xmlStrEqual(msg_root->name, BAD_CAST "rpc-reply") &&
				xmlStrEqual(msg_root->ns->href, BAD_CAST NC_NS_BASE10)) {
			/* copy attributes from the rpc */
			msg_root->properties = xmlCopyPropList(msg_root, rpc_root->properties);
			if (msg_root->properties == NULL && msg->msgid != NULL) {
				xmlNewProp(msg_root, BAD_CAST "message-id", BAD_CAST msg->msgid);
			}

			/* copy additional namespace definitions from rpc */
			for (ns = rpc_root->nsDef; ns != NULL; ns = ns->next) {
				if (ns->prefix == NULL) {
					/* skip default namespace */
					continue;
				}
				xmlNewNs(msg_root, ns->href, ns->prefix);
			}

		}
	} else {
		retval = ""; /* dummy message id for return */
		msg_root = xmlDocGetRootElement(msg->doc);
		/* unknown message ID, send reply without it */
		if (xmlStrcmp(msg_root->name, BAD_CAST "rpc-reply") == 0) {
			xmlRemoveProp(xmlHasProp(msg_root, BAD_CAST "message-id"));
		}
	}

	/* send message */
	ret = nc_session_send (session, msg);

	DBG_UNLOCK("mut_session");
	pthread_mutex_unlock(&(session->mut_session));

	nc_msg_free (msg);

	if (ret != EXIT_SUCCESS) {
		return (0);
	} else {
		if (reply->type.reply == NC_REPLY_ERROR) {
			/* update stats */
			session->stats->out_rpc_errors++;
			if (nc_info) {
				pthread_rwlock_wrlock(&(nc_info->lock));
				nc_info->stats.counters.out_rpc_errors++;
				pthread_rwlock_unlock(&(nc_info->lock));
			}
		}
		return (retval);
	}
}

API int nc_msgid_compare(const nc_msgid id1, const nc_msgid id2)
{
	if (id1 == NULL || id2 == NULL) {
		return (-1);
	} else {
		return (strcmp(id1, id2));
	}
}

API NC_MSG_TYPE nc_session_send_recv(struct nc_session* session, nc_rpc *rpc, nc_reply** reply)
{
	const nc_msgid msgid;
	NC_MSG_TYPE replytype;
	struct nc_msg* queue = NULL, *msg, *p = NULL;

	msgid = nc_session_send_rpc(session, rpc);
	if (msgid == NULL) {
		return (NC_MSG_UNKNOWN);
	}

	DBG_LOCK("mut_mqueue");
	pthread_mutex_lock(&(session->mut_mqueue));

	/* first, look into the session's list of previously received messages */
	if ((queue = session->queue_msg) != NULL) {
		/* search in the queue for the reply with required message ID */
		for (msg = queue; msg != NULL; msg = msg->next) {
			/* test message IDs */
			if (nc_msgid_compare(msgid, nc_reply_get_msgid((nc_reply*) msg)) == 0) {
				break;
			}

			/* store the predecessor */
			p = msg;
		}

		if (msg != NULL) {
			/* we have found the reply in the queue */
			(*reply) = (nc_reply*) msg;

			/* detach the reply from session's queue */
			if (p == NULL) {
				/* no predecessor - we detach the first item in the queue */
				session->queue_msg = msg->next;
			} else {
				p->next = msg->next;
			}
			msg->next = NULL;
			DBG_UNLOCK("mut_mqueue");
			pthread_mutex_unlock(&(session->mut_mqueue));
			return (NC_MSG_REPLY);
		} else {
			/*
			 * the queue does not contain required reply, we have to
			 * read it from the input, but first we have to hide
			 * session's queue from nc_session_recv_reply()
			 */
			session->queue_msg = NULL;
		}
	}
	DBG_UNLOCK("mut_mqueue");
	pthread_mutex_unlock(&(session->mut_mqueue));

	while (1) {
		replytype = nc_session_recv_reply(session, -1, reply);
		if (replytype == NC_MSG_REPLY) {
			/* compare message ID */
			if (nc_msgid_compare(msgid, nc_reply_get_msgid(*reply)) != 0) {
				/* reply with different message ID is expected */
				DBG_LOCK("mut_mqueue");
				pthread_mutex_lock(&(session->mut_mqueue));
				/* store this reply for the later use of someone else */
				if (queue == NULL) {
					queue = (struct nc_msg*) (*reply);
				} else {
					for (msg = queue; msg->next != NULL; msg = msg->next);
					msg->next = (struct nc_msg*) (*reply);
				}
				/*The message is now owned by the queue, do not return it to the caller*/
				*reply = NULL;
				DBG_UNLOCK("mut_mqueue");
				pthread_mutex_unlock(&(session->mut_mqueue));
			} else {
				/* we have it! */
				break;
			}
		} else if (replytype == NC_MSG_UNKNOWN || replytype == NC_MSG_NONE) {
			/* some error occured */
			break;
		}
		/*
		 * else (e.g. Notification or an unexpected reply was received)
		 * read another message in the loop
		 */
	}

	if (queue != NULL) {
		DBG_LOCK("mut_mqueue");
		pthread_mutex_lock(&(session->mut_mqueue));
		/* reconnect hidden queue back to the session */
		session->queue_msg = queue;
		DBG_UNLOCK("mut_mqueue");
		pthread_mutex_unlock(&(session->mut_mqueue));
	}

	return (replytype);
}

const char* nc_session_term_string(NC_SESSION_TERM_REASON reason)
{
	switch(reason) {
	case NC_SESSION_TERM_CLOSED:
		return ("closed");
		break;
	case NC_SESSION_TERM_KILLED:
		return ("killed");
		break;
	case NC_SESSION_TERM_DROPPED:
		return ("dropped");
		break;
	case NC_SESSION_TERM_TIMEOUT:
		return ("timeout");
		break;
	case NC_SESSION_TERM_BADHELLO:
		return ("bad-hello");
		break;
	default:
		return ("other");
		break;
	}
	return (NULL);
}
