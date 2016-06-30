/**
 * \file ssh.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of functions to connect to NETCONF server via SSH2.
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

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <pwd.h>

#ifdef DISABLE_LIBSSH
#	include <sys/ioctl.h>
#	include <ctype.h>
#	include <termios.h>
#	include <pty.h>
#	include <grp.h>
#	include <libxml/xpath.h>
#	include <libxml/xpathInternals.h>

#	define BUFFER_SIZE 4096
#endif

#include "config.h"
#include "libnetconf_ssh.h"
#include "netconf_internal.h"
#include "ssh.h"
#include "session.h"
#include "messages_internal.h"

struct auth_pref_couple
{
	NC_SSH_AUTH_TYPE type;
	short int value;
};

#define AUTH_COUNT 3
static struct auth_pref_couple sshauth_pref[AUTH_COUNT] = {
		{ NC_SSH_AUTH_INTERACTIVE, 3 },
		{ NC_SSH_AUTH_PASSWORD, 2 },
		{ NC_SSH_AUTH_PUBLIC_KEYS, 1 }
};

API void nc_ssh_pref(NC_SSH_AUTH_TYPE type, short int preference)
{
	int dir = 0;
	unsigned short i;
	struct auth_pref_couple new, aux;

	new.type = type;
	new.value = preference;

	for (i = 0; i < AUTH_COUNT; i++) {
		if (sshauth_pref[i].type == new.type) {
			if (sshauth_pref[i].value < new.value) {
				sshauth_pref[i] = new;
				dir = -1;
				/* correct order */
				while ((i + dir) >= 0) {
					if (sshauth_pref[i].value >= sshauth_pref[i + dir].value) {
						aux = sshauth_pref[i + dir];
						sshauth_pref[i + dir] = sshauth_pref[i];
						sshauth_pref[i] = aux;
						i += dir;
					} else {
						break; /* WHILE */
					}
				}
			} else if (sshauth_pref[i].value > new.value) {
				sshauth_pref[i] = new;
				dir = 1;
				/* correct order */
				while ((i + dir) < AUTH_COUNT) {
					if (sshauth_pref[i].value < sshauth_pref[i + dir].value) {
						aux = sshauth_pref[i + dir];
						sshauth_pref[i + dir] = sshauth_pref[i];
						sshauth_pref[i] = aux;
						i += dir;
					} else {
						break; /* WHILE */
					}
				}
			}
			break; /* FOR */
		}
	}
}

#ifndef DISABLE_LIBSSH

/* seconds */
#define SSH_TIMEOUT 10

struct nc_session* _nc_session_accept(const struct nc_cpblts*, const char*, int, int, void*, void*);

API struct nc_session *nc_session_accept_libssh_channel(const struct nc_cpblts* capabilities, const char* username, ssh_channel ssh_chan)
{
	int fd;

	/* we can set fd_input and fd_output too, they are checked after ssh_chan, so the transport will be detected correctly */
	fd = ssh_get_fd(ssh_channel_get_session(ssh_chan));

	return (_nc_session_accept(capabilities, username, fd, fd, ssh_chan, NULL));
}

struct nc_session *nc_session_connect_libssh_socket(const char* username, const char* host, int sock, ssh_session ssh_sess)
{
	struct nc_session *retval = NULL;
	pthread_mutexattr_t mattr;
	int i, j, r, ret_auth, userauthlist;
	int auth = 0;
	const long timeout = SSH_TIMEOUT;
	const char* prompt;
	char *s, *answer, echo;
	ssh_key pubkey, privkey;
	struct passwd *pw;

	if (sock == -1) {
		return (NULL);
	}

	/* get current user if username not explicitely specified */
	if (username == NULL || strisempty(username)) {
		pw = getpwuid(getuid());
		if (pw == NULL) {
			/* unable to get correct username (errno from getpwuid) */
			ERROR("Unable to set a username for the SSH connection (%s).", strerror(errno));
			return (NULL);
		}
		username = pw->pw_name;
	}

	/* allocate netconf session structure */
	retval = calloc(1, sizeof(struct nc_session));
	if (retval == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		return (NULL);
	}
	if ((retval->stats = malloc (sizeof (struct nc_session_stats))) == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		free(retval);
		return NULL;
	}
	retval->transport_socket = sock;
	retval->fd_input = -1;
	retval->fd_output = -1;
	retval->username = strdup(username);
	retval->msgid = 1;

	if (pthread_mutexattr_init(&mattr) != 0) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		free(retval);
		return (NULL);
	}
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
	retval->mut_channel = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	if ((r = pthread_mutex_init(retval->mut_channel, &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_mqueue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_equeue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_ntf), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_session), &mattr)) != 0) {
		ERROR("Mutex initialization failed (%s).", strerror(r));
		pthread_mutexattr_destroy(&mattr);
		free(retval);
		return (NULL);
	}
	pthread_mutexattr_destroy(&mattr);

	/* Create a session instance */
	if (ssh_sess) {
		retval->ssh_sess = ssh_sess;
	} else {
		retval->ssh_sess = ssh_new();
		if (retval->ssh_sess == NULL) {
			ERROR("Unable to initialize the SSH session.");
			goto shutdown;
		}
	}

	ssh_options_set(retval->ssh_sess, SSH_OPTIONS_HOST, host);
	ssh_options_set(retval->ssh_sess, SSH_OPTIONS_USER, retval->username);
	ssh_options_set(retval->ssh_sess, SSH_OPTIONS_FD, &retval->transport_socket);
	ssh_options_set(retval->ssh_sess, SSH_OPTIONS_TIMEOUT, &timeout);
    if (ssh_options_set(retval->ssh_sess, SSH_OPTIONS_HOSTKEYS,
                        "ssh-ed25519,ecdsa-sha2-nistp521,ecdsa-sha2-nistp384,"
                        "ecdsa-sha2-nistp256,ssh-rsa,ssh-dss,ssh-rsa1")) {
        /* ecdsa is probably not supported... */
        ssh_options_set(retval->ssh_sess, SSH_OPTIONS_HOSTKEYS, "ssh-ed25519,ssh-rsa,ssh-dss,ssh-rsa1");
    }
    ssh_set_blocking(retval->ssh_sess, 0);

	while ((r = ssh_connect(retval->ssh_sess)) == SSH_AGAIN) {
		usleep(NC_READ_SLEEP);
	}
	if (r != SSH_OK) {
		ERROR("Starting the SSH session failed (%s)", ssh_get_error(retval->ssh_sess));
		DBG("Error code %d.", ssh_get_error_code(retval->ssh_sess));
		goto shutdown;
	}

	if (callbacks.hostkey_check(host, retval->ssh_sess) != 0) {
		ERROR("Checking the host key failed.");
		goto shutdown;
	}

	while ((ret_auth = ssh_userauth_none(retval->ssh_sess, NULL)) == SSH_AUTH_AGAIN) {
		usleep(NC_READ_SLEEP);
	}
	if (ret_auth == SSH_AUTH_ERROR) {
		ERROR("Authentication failed (%s).", ssh_get_error(retval->ssh_sess));
		goto shutdown;
	}

	/* check what authentication methods are available */
	userauthlist = ssh_userauth_list(retval->ssh_sess, NULL);
	if (userauthlist != 0) {
		if (userauthlist & SSH_AUTH_METHOD_PASSWORD) {
			if (callbacks.sshauth_password != NULL) {
				auth |= NC_SSH_AUTH_PASSWORD;
			}
		}
		if (userauthlist & SSH_AUTH_METHOD_PUBLICKEY) {
			if (callbacks.sshauth_passphrase != NULL) {
				auth |= NC_SSH_AUTH_PUBLIC_KEYS;
			}
		}
		if (userauthlist & SSH_AUTH_METHOD_INTERACTIVE) {
			if (callbacks.sshauth_interactive != NULL) {
				auth |= NC_SSH_AUTH_INTERACTIVE;
			}
		}
	}
	if (auth == 0 && ret_auth != SSH_AUTH_SUCCESS) {
		ERROR("Unable to authenticate to the remote server (Authentication methods not supported).");
		goto shutdown;
	}

	/* select authentication according to preferences */
	for (i = 0; i < AUTH_COUNT; i++) {
		if ((sshauth_pref[i].type & auth) == 0) {
			/* method not supported by server, skip */
			continue;
		}

		if (sshauth_pref[i].value < 0) {
			/* all following auth methods are disabled via negative preference value */
			ERROR("Unable to authenticate to the remote server (method disabled or permission denied).");
			goto shutdown;
		}

		/* found common authentication method */
		switch (sshauth_pref[i].type) {
		case NC_SSH_AUTH_PASSWORD:
			VERB("Password authentication (host %s, user %s)", host, username);
			s = callbacks.sshauth_password(username, host);
			while ((ret_auth = ssh_userauth_password(retval->ssh_sess, username, s)) == SSH_AUTH_AGAIN) {
				usleep(NC_READ_SLEEP);
			}
			if (ret_auth != SSH_AUTH_SUCCESS) {
				memset(s, 0, strlen(s));
				VERB("Authentication failed (%s)", ssh_get_error(retval->ssh_sess));
			}
			free(s);
			break;
		case NC_SSH_AUTH_INTERACTIVE:
			VERB("Keyboard-interactive authentication");
			while (((ret_auth = ssh_userauth_kbdint(retval->ssh_sess, NULL, NULL)) == SSH_AUTH_INFO) || (ret_auth == SSH_AUTH_AGAIN)) {
				if (ret_auth == SSH_AUTH_AGAIN) {
					usleep(NC_READ_SLEEP);
					continue;
				}
				for (j = 0; j < ssh_userauth_kbdint_getnprompts(retval->ssh_sess); ++j) {
					prompt = ssh_userauth_kbdint_getprompt(retval->ssh_sess, j, &echo);
					if (prompt == NULL) {
						break;
					}
					answer = callbacks.sshauth_interactive(
						ssh_userauth_kbdint_getname(retval->ssh_sess),
						ssh_userauth_kbdint_getinstruction(retval->ssh_sess),
						prompt, echo);
					if (ssh_userauth_kbdint_setanswer(retval->ssh_sess, j, answer) < 0) {
						free(answer);
						break;
					}
					free(answer);
				}
			}

			if (ret_auth == SSH_AUTH_ERROR) {
				VERB("Authentication failed (%s)", ssh_get_error(retval->ssh_sess));
			}

			break;
		case NC_SSH_AUTH_PUBLIC_KEYS:
			VERB("Publickey athentication");

			for (j = 0; j < SSH_KEYS; ++j) {
				if (callbacks.publickey_filename[j] != NULL && callbacks.privatekey_filename[j] != NULL) {
					break;
				}
			}

			/* if publickeys path not provided, we cannot continue */
			if (j == SSH_KEYS) {
				VERB("No key pair specified.");
				break;
			}

			for (j = 0; j < SSH_KEYS; j++) {
				if (callbacks.privatekey_filename[j] == NULL || callbacks.publickey_filename[j] == NULL) {
					/* key not available */
					continue;
				}

				VERB("Trying to authenticate using %spair %s %s",
						callbacks.key_protected[j] ? "password-protected " : "", callbacks.privatekey_filename[j], callbacks.publickey_filename[j]);

				if (ssh_pki_import_pubkey_file(callbacks.publickey_filename[j], &pubkey) != SSH_OK) {
					WARN("Failed to import the key \"%s\".", callbacks.publickey_filename[j]);
					continue;
				}
				while ((ret_auth = ssh_userauth_try_publickey(retval->ssh_sess, NULL, pubkey)) == SSH_AUTH_AGAIN) {
					usleep(NC_READ_SLEEP);
				}
				if (ret_auth == SSH_AUTH_DENIED || ret_auth == SSH_AUTH_PARTIAL) {
					ssh_key_free(pubkey);
					continue;
				}
				if (ret_auth == SSH_AUTH_ERROR) {
					ERROR("Authentication failed (%s)", ssh_get_error(retval->ssh_sess));
					ssh_key_free(pubkey);
					break;
				}

				if (callbacks.key_protected[j]) {
					s = callbacks.sshauth_passphrase(username, host, callbacks.privatekey_filename[j]);
				} else {
					s = NULL;
				}

				if (ssh_pki_import_privkey_file(callbacks.privatekey_filename[j], s, NULL, NULL, &privkey) != SSH_OK) {
					WARN("Failed to import the key \"%s\".", callbacks.privatekey_filename[j])
					if (s != NULL) {
						memset(s, 0, strlen(s));
						free(s);
					}
					ssh_key_free(pubkey);
					continue;
				}

				if (s != NULL) {
					memset(s, 0, strlen(s));
					free(s);
				}

				while ((ret_auth = ssh_userauth_publickey(retval->ssh_sess, NULL, privkey)) == SSH_AUTH_AGAIN) {
					usleep(NC_READ_SLEEP);
				}
				ssh_key_free(pubkey);
				ssh_key_free(privkey);

				if (ret_auth == SSH_AUTH_ERROR) {
					ERROR("Authentication failed (%s)", ssh_get_error(retval->ssh_sess));
				}
				if (ret_auth == SSH_AUTH_SUCCESS) {
					break;
				}
			}
			break;
		}

		if (ret_auth == SSH_AUTH_SUCCESS) {
			break;
		}
	}

	/* check a state of authentication */
	if (ret_auth != SSH_AUTH_SUCCESS) {
		ERROR("Authentication failed.");
		goto shutdown;
	}

	/* open a channel */
	retval->ssh_chan = ssh_channel_new(retval->ssh_sess);
	while ((r = ssh_channel_open_session(retval->ssh_chan)) == SSH_AGAIN) {
		usleep(NC_READ_SLEEP);
	}
	if (r != SSH_OK) {
		ssh_channel_free(retval->ssh_chan);
		retval->ssh_chan = NULL;
		ERROR("Opening the SSH channel failed (%s)", ssh_get_error(retval->ssh_sess));
		goto shutdown;
	}

	/* execute the NETCONF subsystem on the channel */
	while ((r = ssh_channel_request_subsystem(retval->ssh_chan, "netconf")) == SSH_AGAIN) {
		usleep(NC_READ_SLEEP);
	}
	if (r != SSH_OK) {
		ERROR("Starting the netconf SSH subsystem failed (%s)", ssh_get_error(retval->ssh_sess));
		goto shutdown;
	}

	return (retval);

shutdown:

	/* cleanup */
	nc_session_close(retval, NC_SESSION_TERM_OTHER);
	nc_session_free(retval);

	return (NULL);
}

/* definition in transport.c */
int transport_connect_socket(const char* host, const char* port);

/*
 * libssh variant - use internal SSH client implementation using libssh
 */
struct nc_session *nc_session_connect_ssh(const char* username, const char* host, const char* port, void* ssh_sess)
{
	struct nc_session *retval = NULL;
	int sock = -1;

	sock = transport_connect_socket(host, port);
	if (sock == -1) {
		return (NULL);
	}

	retval = nc_session_connect_libssh_socket(username, host, sock, ssh_sess);
	if (retval != NULL) {
		retval->hostname = strdup(host);
		retval->port = strdup(port);
	} else {
		close(sock);
	}

	return (retval);
}

struct nc_session *nc_session_connect_libssh_channel(struct nc_session *session)
{
	struct nc_session *retval;
	pthread_mutexattr_t mattr;
	int r;

	/* allocate netconf session structure */
	retval = calloc(1, sizeof(struct nc_session));
	if (retval == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		return (NULL);
	}
	if ((retval->stats = malloc (sizeof (struct nc_session_stats))) == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		free(retval);
		return NULL;
	}

	retval->transport_socket = session->transport_socket;
	retval->fd_input = -1;
	retval->fd_output = -1;
	retval->hostname = session->hostname;
	retval->username = session->username;
	retval->port = session->port;
	retval->msgid = 1;

	/* shared resources with the original session */
	retval->ssh_sess = session->ssh_sess;
	/*
	 * libssh is quite stupid - it provides multiple channels inside a single
	 * session, but it does not allow multiple threads to work with these
	 * channels, so we have to share mutex of the master
	 * session to control access to each SSH channel
	 */
	retval->mut_channel = session->mut_channel;

	if (pthread_mutexattr_init(&mattr) != 0) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
	if ((r = pthread_mutex_init(&(retval->mut_mqueue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_equeue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_ntf), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_session), &mattr)) != 0) {
		ERROR("Mutex initialization failed (%s).", strerror(r));
		pthread_mutexattr_destroy(&mattr);
		return (NULL);
	}
	pthread_mutexattr_destroy(&mattr);

	/* open a separated channel */
	DBG_LOCK("mut_channel");
	pthread_mutex_lock(session->mut_channel);
	retval->ssh_chan = ssh_channel_new(retval->ssh_sess);
	if (ssh_channel_open_session(retval->ssh_chan) != SSH_OK) {
		DBG_UNLOCK("mut_channel");
		pthread_mutex_unlock(session->mut_channel);
		ERROR("Opening the SSH channel failed (%s)", ssh_get_error(retval->ssh_sess));
		goto shutdown;
	}

	/* execute the NETCONF subsystem on the channel */
	if (ssh_channel_request_subsystem(retval->ssh_chan, "netconf") != SSH_OK) {
		DBG_UNLOCK("mut_channel");
		pthread_mutex_unlock(session->mut_channel);
		ERROR("Starting the netconf SSH subsystem failed (%s)", ssh_get_error(retval->ssh_sess));
		goto shutdown;
	}
	DBG_UNLOCK("mut_channel");
	pthread_mutex_unlock(session->mut_channel);

	retval->status = NC_SESSION_STATUS_WORKING;

	return (retval);
shutdown:

	/* cleanup */
	if (retval->ssh_chan != NULL) {
		DBG_LOCK("mut_channel");
		pthread_mutex_lock(session->mut_channel);
		ssh_channel_free(retval->ssh_chan);
		DBG_UNLOCK("mut_channel");
		pthread_mutex_unlock(session->mut_channel);
	}
	if (retval) {
		free(retval->stats);
		pthread_mutex_destroy(&(retval->mut_mqueue));
		pthread_mutex_destroy(&(retval->mut_equeue));
		pthread_mutex_destroy(&(retval->mut_ntf));
		pthread_mutex_destroy(&(retval->mut_session));
		free(retval);
	}
	return (NULL);
}

#else /* DISABLE_LIBSSH */

struct nc_msg* read_hello_openssh(struct nc_session *session)
{
	struct nc_msg *retval;
	nc_reply* reply;
	xmlNodePtr root;
	unsigned long long int i, size = BUFFER_SIZE;
	char *buffer = NULL, c, *aux_buffer;

	if (!(buffer = (char *) malloc(size * sizeof (char)))) {
		return NULL;
	}
	memset(buffer, '\0', size * sizeof (char));

	/* initial reading */
	while(isspace(buffer[0] = (char) fgetc(session->f_input)));
	for (i = 1; i < strlen(NC_V10_END_MSG); i++) {
		if ((!feof(session->f_input)) && (!ferror(session->f_input))) {
			buffer[i] = (char) fgetc(session->f_input);
		} else {
			free(buffer);
			return NULL;
		}
	}

	/*
	 * read next character and check ending character sequence for
	 * NC_V10_END_MSG
	 */
	if (strcmp(NC_V10_END_MSG, &buffer[i - (unsigned int)strlen(NC_V10_END_MSG)])) {
		while ((!feof(session->f_input)) && (!ferror(session->f_input)) && ((c = (char) fgetc(session->f_input)) != EOF)) {
			if (i == size - 1) { /* buffer is too small */
				/* allocate larger buffer */
				size = 2 * size;
				if (!(aux_buffer = (char *) realloc(buffer, size))) {
					free(buffer); /* free buffer that was too small and reallocation failed */
					return NULL;
				}
				buffer = aux_buffer;
			}
			/* store read character */
			buffer[i] = c;
			i++;

			/* check if the ending character sequence was read */
			if (!(strncmp(NC_V10_END_MSG, &buffer[i - (unsigned int)strlen(NC_V10_END_MSG)], strlen(NC_V10_END_MSG)))) {
				buffer[i - strlen(NC_V10_END_MSG)] = '\0';
				break;
			}
		}
	} else {
		/* message is empty and contains only END-MESSAGE marker */
		/* one option is to return NULL as error:
		 free(buffer);
		 return NULL;
		 * or return empty string (buffer): */
		buffer[0] = '\0';
	}
	fclose(session->f_input);
	session->f_input = NULL;

	/* create the message structure */
	retval = calloc (1, sizeof(struct nc_msg));
	if (retval == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		free (buffer);
		goto malformed_msg;
	}

	/* store the received message in libxml2 format */
	retval->doc = xmlReadDoc (BAD_CAST buffer, NULL, NULL, NC_XMLREAD_OPTIONS);
	if (retval->doc == NULL) {
		free (retval);
		free (buffer);
		ERROR("Invalid XML data received.");
		goto malformed_msg;
	}
	free (buffer);

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

	/* parse and store message type */
	root = xmlDocGetRootElement(retval->doc);
	if (xmlStrcmp (root->name, BAD_CAST "hello") != 0) {
		ERROR("Unexpected (non-hello) message received.");
		nc_msg_free(retval);
		goto malformed_msg;
	} else {
		/* set message type, we have <hello> message */
		retval->type.reply = NC_REPLY_HELLO;
		retval->msgid = NULL;
	}

	return (retval);

malformed_msg:

	if (session->version == NETCONFV11 && session->ssh_sess == NULL) {
		/* NETCONF version 1.1 define sending error reply from the server */
		reply = nc_reply_error(nc_err_new(NC_ERR_MALFORMED_MSG));
		if (reply == NULL) {
			ERROR("Unable to create a \'Malformed message\' reply");
			nc_session_close(session, NC_SESSION_TERM_OTHER);
			return (NULL);
		}

		if (nc_session_send_reply(session, NULL, reply) == 0) {
			ERROR("Unable to send a \'Malformed message\' reply");
			nc_session_close(session, NC_SESSION_TERM_OTHER);
			return (NULL);
		}
		nc_reply_free(reply);
	}

	ERROR("Malformed message received, closing the session %s.", session->session_id);
	nc_session_close(session, NC_SESSION_TERM_OTHER);

	return (NULL);
}

/*
 * OpenSSH variant - use a standalone ssh client from OpenSSH
 */
struct nc_session *nc_session_connect_ssh(const char* username, const char* host, const char* port, void* ssh_sess)
{
	struct nc_session *retval = NULL;
	struct passwd *pw;
	pthread_mutexattr_t mattr;
	pid_t sshpid; /* child's PID */
	struct termios termios;
	struct winsize win;
	int pout[2] = {-1, -1}, ssh_in;
	int ssh_fd, count = 0;
	char buffer[BUFFER_SIZE];
	char tmpchar[2];
	char line[81];
	char *s;
	int r = 0;
	int forced = 0; /* force connection to unknown destinations */
	size_t n;
	gid_t newgid, oldgid;
	uid_t newuid, olduid;

	if (access(SSH_PROG, X_OK) != 0) {
		ERROR("Unable to locate or execute ssh(1) application \'%s\' (%s).", SSH_PROG, strerror(errno));
		return (NULL);
	}

	/* get current user if not specified */
	if (username == NULL) {
		pw = getpwuid(getuid());
		if (pw == NULL) {
			/* unable to get correct username (errno from getpwuid) */
			ERROR("Unable to set the username for the SSH connection (%s).", strerror(errno));
			return (NULL);
		} else {
			username = pw->pw_name;
		}
	}

	/* allocate netconf session structure */
	retval = calloc(1, sizeof(struct nc_session));
	if (retval == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		return (NULL);
	}
	if ((retval->stats = malloc(sizeof(struct nc_session_stats))) == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		free(retval);
		return NULL;
	}
	retval->transport_socket = -1;
	retval->fd_input = -1;
	retval->hostname = strdup(host);
	retval->username = strdup(username);
	retval->port = strdup(port);
	retval->msgid = 1;

	if (pthread_mutexattr_init(&mattr) != 0) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		goto error_cleanup;
	}
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
	if (((retval->mut_channel = calloc(1, sizeof(pthread_mutex_t))) == NULL) ||
			(r = pthread_mutex_init(retval->mut_channel, &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_mqueue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_equeue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_ntf), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_session), &mattr)) != 0) {
		ERROR("Mutex initialization failed (%s).", strerror(r));
		pthread_mutexattr_destroy(&mattr);
		goto error_cleanup;
	}
	pthread_mutexattr_destroy(&mattr);

	/* create communication pipes */
	if (pipe(pout) == -1) {
		ERROR("%s: Unable to create communication pipes", __func__);
		goto error_cleanup;
	}
	retval->fd_output = pout[1];
	ssh_in = pout[0];

	/* Get current properties of tty */
	ioctl(0, TIOCGWINSZ, &win);
	if (tcgetattr(STDIN_FILENO, &termios) < 0) {
		ERROR("%s", strerror(errno));
		goto error_cleanup;
	}

	/* create child process */
	if ((sshpid = forkpty(&ssh_fd, NULL, &termios, &win)) == -1) {
		ERROR("%s", strerror(errno));
		goto error_cleanup;
	} else if (sshpid == 0) { /* child process*/
		/* close unused ends of communication pipes */
		close(retval->fd_output);

		/* set ends of communication pipes to standard input/output */
		if (dup2(ssh_in, STDIN_FILENO) == -1) {
			ERROR("%s", strerror(errno));
			exit(-1);
		}

		/* drop privileges if any set by the application */
		newgid = getgid();
		oldgid = getegid();
		newuid = getuid();
		olduid = geteuid();
		/* if root privileges are to be dropped, pare down the ancillary groups */
		if (olduid == 0) {
			setgroups(1, &newgid);
		}
		/* drop group privileges */
		if (newgid != oldgid) {
#if !defined(linux)
			setegid(newgid);
			setgid(newgid);
#else
			/*
			 * ignore result using empty if(...) that get rid of a compiler
			 * warning on unused-result
			 */
			if (setregid(newgid, newgid)){}
#endif
		}
		/* drop user privileges */
		if (newuid != olduid) {
#if !defined(linux)
			seteuid(newuid);
			setuid(newuid);
#else
			/*
			 * ignore result using empty if(...) that get rid of a compiler
			 * warning on unused-result
			 */
			if (setreuid(newuid, newuid)){}
#endif
		}

		/* run ssh with parameters to start ssh subsystem on the server */
		execl(SSH_PROG, SSH_PROG, "-l", username, "-p", port, "-s", host, "netconf", NULL);
		ERROR("Executing ssh failed");
		exit(-1);
	} else { /* parent process*/
		DBG("child proces with PID %d forked", (int) sshpid);
		close(ssh_in); pout[0] = -1;
		/* open stream to ssh pseudo terminal */
		/* write there only password/commands for ssh, commands for
		 netopeer-agent are written only through communication pipes
		 (i.e. communication->out file stream). Output from
		 netopeer-agent is then read from ssh pseudo terminal
		 (i.e. communication->in file stream).
		 */
		retval->fd_input = ssh_fd;
		retval->f_input = fdopen(dup(ssh_fd), "a+");
		buffer[0] = '\0';
		/* Read 1 char at a time and build up a string */
		/* This will wait forever until "<" as xml message start is found... */
		DBG("waiting for a password request");
		while ((count++ < BUFFER_SIZE) && (fgets(tmpchar, 2, retval->f_input) != NULL)) {
			strcat(buffer, tmpchar);
			if (((int *) strcasestr(buffer, "password") != NULL) || ((int *) strcasestr(buffer, "enter passphrase"))) {
				/* read rest of the line */
				while ((count++ < BUFFER_SIZE) && (buffer[strlen(buffer) - 1] != ':')) {
					if (fgets(tmpchar, 2, retval->f_input) == NULL) {
						break;
					}
					strcat(buffer, tmpchar);
				}
				DBG("writing the password to ssh");
				//s = callbacks.sshauth_password(username, host);

				fprintf(stdout, "%s ", buffer);
				s = NULL;
				if (system("stty -echo") == -1) {
					ERROR("system() call failed (%s:%d).", __FILE__, __LINE__);
					goto error_cleanup;
				}
				if (getline(&s, &n, stdin) == -1) {
					ERROR("getline() failed (%s:%d).", __FILE__, __LINE__);
					goto error_cleanup;
				}
				if (system("stty echo") == -1) {
					ERROR("system() call failed (%s:%d).", __FILE__, __LINE__);
					goto error_cleanup;
				}

				if (s == NULL) {
					ERROR("Unable to get the password from a user (%s)", strerror(errno));
					goto error_cleanup;
				}
				fprintf(retval->f_input, "%s", s);
				//fprintf(retval->f_input, "\n");
				fflush(retval->f_input);

				/* remove password from the memory */
				memset(s, 0, strlen(s));
				free(s);

				strcpy(buffer, "\0");
				count = 0; /* reset search string */
			}
			if (((int *) strcasestr(buffer, "connecting (yes/no)?") != NULL) || ((int *) strcasestr(buffer, "'yes' or 'no':") != NULL)) {
				switch (forced) {
				case 1:
					fprintf(retval->f_input, "yes");
					DBG("connecting to an unauthenticated host");
					break;
				case 0:
					fprintf(stdout, "%s ", buffer);
					if (fgets(line, 81, stdin) == NULL) {
						WARN("fgets() failed (%s:%d).", __FILE__, __LINE__);
						fprintf(retval->f_input, "no");
						VERB("connecting to an unauthenticated host disabled");
					} else {
						fprintf(retval->f_input, "%s", line);
					}
					break;
				case -1:
					fprintf(stdout, "%s ", buffer);
					fprintf(retval->f_input, "no");
					VERB("connecting to an unauthenticated host disabled");
					break;
				default:
					goto error_cleanup;
				}
				fprintf(retval->f_input, "\n");
				fflush(retval->f_input);
				/*
				 * read written line from terminal to remove it from read buffer,
				 * if(...) just checks results and do nothing to get rid of
				 * a compiler warning on unused-result
				 */
				if (fgets(line, 81, retval->f_input)){}
				line[0] = '\0';                   /* and forget what we read */
				strcpy(buffer, "\0"); count = 0;  /* finally, reset search string */
			}
			if ((int *) strcasestr(buffer, "to the list of known hosts.") != NULL) {
				if (forced != 1) {
					fprintf(stdout, "%s\n", buffer);
					fflush(stdout);
				}
				strcpy(buffer, "\0");
				count = 0; /* reset search string */
			}
			if ((int *) strcasestr(buffer, "No route to host") != NULL) {
				ERROR("%s", buffer);
				goto error_cleanup;
			}
			if ((int *) strcasestr(buffer, "Permission denied") != NULL) {
				ERROR("%s", buffer);
				goto error_cleanup;
			}
			if ((int *) strcasestr(buffer, "Connection refused") != NULL) {
				ERROR("%s", buffer);
				goto error_cleanup;
			}
			if ((int *) strcasestr(buffer, "Connection closed") != NULL) {
				ERROR("%s", buffer);
				goto error_cleanup;
			}
			if ((int *) strcasestr(buffer, "<") != NULL) {
				DBG("XML message begin found, waiting for the password finished");
				ungetc(buffer[strlen(buffer) - 1], retval->f_input);
				break; /* while */
			}
			/* print out other messages */
			if ((int *) strcasestr(buffer, "\n") != NULL) {
				fprintf(stdout, "%s", buffer);
				fflush(stdout);
				strcpy(buffer, "\0");
				count = 0; /* reset search string */
			}
		}
		termios.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
		termios.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR | INPCK | ISTRIP | IXON | PARMRK);
		termios.c_oflag &= ~OPOST;
		termios.c_cc[VMIN] = 1;
		termios.c_cc[VTIME] = 0;

		tcsetattr(retval->fd_input, TCSANOW, &termios);
	}

	return (retval);

error_cleanup:

	close(pout[0]);
	close(pout[1]);
	if (retval) {
		free(retval->hostname);
		free(retval->username);
		free(retval->port);

		if (retval->mut_channel) {
			pthread_mutex_destroy(retval->mut_channel);
			free(retval->mut_channel);
		}
		pthread_mutex_destroy(&(retval->mut_mqueue));
		pthread_mutex_destroy(&(retval->mut_equeue));
		pthread_mutex_destroy(&(retval->mut_ntf));
		pthread_mutex_destroy(&(retval->mut_session));

		close(retval->fd_input);
		fclose(retval->f_input);

		free(retval);
	}
	return (NULL);
}

#endif /* DISABLE_LIBSSH */
