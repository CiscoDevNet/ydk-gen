/**
 * \file transport.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of functions implementing transport layer for NETCONF.
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

#include "config.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>

#ifdef HAVE_UTMPX_H
#	include <utmpx.h>
#endif

#include <libxml/tree.h>
#include <libxml/xpathInternals.h>

#include "netconf_internal.h"
#include "session.h"
#include "messages_internal.h"
#include "transport.h"
#include "callhome.h"
#include "ssh.h"

#ifdef ENABLE_TLS
#	include "tls.h"
#	include "openssl/pem.h"
#	include "openssl/x509.h"
#endif

#ifndef DISABLE_URL
#	include "url_internal.h"
#endif

#ifndef DISABLE_NOTIFICATIONS
#  include "notifications.h"
#endif

struct nc_mngmt_server {
	int active;
	struct addrinfo *addr;
	struct nc_mngmt_server* next;
};

/* definition in session.c */
void parse_wdcap(struct nc_cpblts *capabilities, NCWD_MODE *basic, int *supported);

/* definition in datastore.c */
char** get_schemas_capabilities(struct nc_cpblts *cpblts);

extern struct nc_shared_info *nc_info;
extern int nc_init_flags;

static pthread_key_t transproto_key;
static pthread_once_t transproto_key_once = PTHREAD_ONCE_INIT;
static NC_TRANSPORT proto_ssh = NC_TRANSPORT_SSH;
static NC_TRANSPORT proto_tls = NC_TRANSPORT_TLS;
static void transproto_init(void)
{
	pthread_key_create(&transproto_key, NULL);
}

API int nc_session_transport(NC_TRANSPORT proto)
{
#ifndef ENABLE_TLS
	if (proto == NC_TRANSPORT_TLS) {
		ERROR("NETCONF over TLS is not supported, recompile libnetconf with --enable-tls option");
		return (EXIT_FAILURE);
	}
#endif

	pthread_once(&transproto_key_once, transproto_init);

	switch(proto) {
	case NC_TRANSPORT_SSH:
		pthread_setspecific(transproto_key, &proto_ssh);
		break;
	case NC_TRANSPORT_TLS:
		pthread_setspecific(transproto_key, &proto_tls);
		break;
	default:
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

int transport_connect_socket(const char* host, const char* port)
{
	int i, flags, sock = -1;
	struct addrinfo hints, *res_list, *res;

	/* Connect to a server */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	i = getaddrinfo(host, port, &hints, &res_list);
	if (i != 0) {
		ERROR("Unable to translate the host address (%s).", gai_strerror(i));
		return (-1);
	}

	for (i = 0, res = res_list; res != NULL; res = res->ai_next) {
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sock == -1) {
			/* socket was not created, try another resource */
			i = errno;
			goto errloop;
		}

		if (connect(sock, res->ai_addr, res->ai_addrlen) == -1) {
			/* network connection failed, try another resource */
			i = errno;
			close(sock);
			sock = -1;
			goto errloop;
		}

		/* make the socket non-blocking */
		if (((flags = fcntl(sock, F_GETFL)) == -1) || (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)) {
			ERROR("Fcntl failed (%s).", strerror(errno));
			close(sock);
			return -1;
		}

		/* we're done, network connection established */
		break;
errloop:
		VERB("Unable to connect to %s:%s over %s (%s).", host, port,
				(res->ai_family == AF_INET6) ? "IPv6" : "IPv4", strerror(i));
		continue;
	}
	freeaddrinfo(res_list);

	if (sock == -1) {
		ERROR("Unable to connect to %s:%s.", host, port);
	}

	return (sock);
}

static char** nc_parse_hello(struct nc_msg *msg, struct nc_session *session)
{
	xmlNodePtr node, capnode;
	char *cap = NULL, *str = NULL;
	char **capabilities = NULL;
	int c;

	if ((node = xmlDocGetRootElement(msg->doc)) == NULL) {
		ERROR("Parsing a <hello> message failed - the document is empty.");
		return (NULL);
	}

	if (xmlStrcmp(node->name, BAD_CAST "hello")) {
		ERROR("Parsing a <hello> message failed - received a non-<hello> message.");
		return (NULL);
	}

	for (node = node->children; node != NULL; node = node->next) {
		if (xmlStrcmp(node->name, BAD_CAST "capabilities") == 0) {
			/* <capabilities> node */

			/* count capabilities */
			for (capnode = node->children, c = 0; capnode != NULL;
			                capnode = capnode->next) {
				c++;
			}

			/* allocate memory for the capability list */
			if ((capabilities = malloc((c + 1) * sizeof(char*))) == NULL) {
				ERROR("Memory allocation failed - %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
				return (NULL);
			}

			/* fill up capabilities list */
			for (capnode = node->children, c = 0; capnode != NULL;
			                capnode = capnode->next) {
				str = (char*) xmlNodeGetContent(capnode);
				if ((str == NULL) || (cap = nc_clrwspace(str)) == NULL) {
					ERROR("Parsing a <hello> message failed - unable to read the capabilities.");
					return (NULL);
				}
				xmlFree(BAD_CAST str);
				if (strnonempty(cap)) {
					capabilities[c++] = cap;
				}
			}
			/* list termination NULL */
			capabilities[c] = NULL;
		} else if (xmlStrcmp(node->name, BAD_CAST "session-id") == 0) {
			/* session-id - available only if the caller of this parsing is a client */
			if (session->session_id[0] == '\0') {
				str = (char*) xmlNodeGetContent(node);
				if (strlen(str) >= (size_t) SID_SIZE) {
					/* Session ID is too long and we cannot store it */
					ERROR("Received <session-id> is too long - terminating the session.");
					return (NULL);
				}
				strncpy(session->session_id, str, SID_SIZE - 1);
				xmlFree(BAD_CAST str);
			} else {
				/* as defined by RFC, we have to terminate the session */
				ERROR("Received <hello> message with <session-id> - terminating the session.");
				return (NULL);
			}
		} else {
			/* something unknown - log it now, maybe in the future we will be more strict and will be returning error */
			WARN("Unknown content of the <hello> message (%s), ignoring and trying to continue.", (char*) node->name);
		}
	}

	if (capabilities == NULL || capabilities[0] == NULL) {
		/* no capability received */
		ERROR("Parsing a <hello> message failed - no capabilities detected.");
		return (NULL);
	}

	/* everything OK, return the received list of supported capabilities */
	return (capabilities);
}

static char** nc_accept_server_cpblts(char ** server_cpblts_list, char ** client_cpblts_list, int *version)
{
	int i, j, c;
	char **result = NULL;

	if (server_cpblts_list == NULL || client_cpblts_list == NULL) {
		ERROR("%s: Invalid parameters.", __func__);
		return (NULL);
	}

	if (version != NULL) {
		(*version) = NETCONFVUNK;
	}

	/* count max size of the resulting list */
	for (c = 0; server_cpblts_list[c] != NULL; c++);

	if ((result = malloc((c + 1) * sizeof(char*))) == NULL) {
		ERROR("Memory allocation failed: %s (%s:%d).", strerror (errno), __FILE__, __LINE__);
		return (NULL);
	}

	c = 0;
	for (i = 0; server_cpblts_list[i] != NULL; i++) {
		if (strstr(server_cpblts_list[i], "urn:ietf:params:netconf:base:") == NULL) {
			result[c++] = strdup(server_cpblts_list[i]);
		} else {
			/* some of the base capability detected - check that both sides support it */
			for (j = 0; client_cpblts_list[j] != NULL; j++) {
				if (strcmp(server_cpblts_list[i], client_cpblts_list[j]) == 0) {
					result[c++] = strdup(server_cpblts_list[i]);
					break;
				}
			}
		}
	}
	result[c] = NULL;

	for (i = 0; result[i] != NULL; i++) { /* Try to find one of the netconf base capability */
		if (strcmp(NC_CAP_BASE11_ID, result[i]) == 0) {
			(*version) = NETCONFV11;
			break;
			/* v 1.1 is preferred */
		}
		if (strcmp(NC_CAP_BASE10_ID, result[i]) == 0) {
			(*version) = NETCONFV10;
			/* continue in searching for higher version */
		}
	}

	if ((*version) == NETCONFVUNK) {
		ERROR("No base capability found in the capabilities intersection.");
		free(result);
		return (NULL);
	}

	return (result);
}

/**
 * @brief Create the client \<hello\> message.
 * @ingroup internalAPI
 * @param caps List of client capabilities.
 * @return rpc structure with the created client \<hello\> message.
 */
static nc_rpc* nc_msg_client_hello(char** cpblts)
{
	nc_rpc *msg;
	xmlNodePtr node;
	int i;
	xmlNsPtr ns;

	if (cpblts == NULL || cpblts[0] == NULL) {
		ERROR("hello: no capability specified");
		return (NULL);
	}

	msg = calloc(1, sizeof(nc_rpc));
	if (msg == NULL) {
		ERROR("Memory reallocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}

	msg->error = NULL;
	msg->doc = xmlNewDoc(BAD_CAST "1.0");
	msg->doc->encoding = xmlStrdup(BAD_CAST UTF8);
	msg->msgid = NULL;
	msg->with_defaults = NCWD_MODE_NOTSET;
	msg->type.rpc = NC_RPC_HELLO;

	/* create root element */
	msg->doc->children = xmlNewDocNode(msg->doc, NULL, BAD_CAST NC_HELLO_MSG, NULL);

	/* set namespace */
	ns = xmlNewNs(msg->doc->children, (xmlChar *) NC_NS_BASE10, NULL);
	xmlSetNs(msg->doc->children, ns);

	/* create capabilities node */
	node = xmlNewChild(msg->doc->children, ns, BAD_CAST "capabilities", NULL);
	for (i = 0; cpblts[i] != NULL; i++) {
		xmlNewChild(node, ns, BAD_CAST "capability", BAD_CAST cpblts[i]);
	}

	/* create xpath evaluation context */
	if ((msg->ctxt = xmlXPathNewContext(msg->doc)) == NULL) {
		ERROR("%s: rpc message XPath context cannot be created.", __func__);
		nc_msg_free(msg);
		return NULL;
	}

	/* register base namespace for the rpc */
	if (xmlXPathRegisterNs(msg->ctxt, BAD_CAST NC_NS_BASE10_ID, BAD_CAST NC_NS_BASE10) != 0) {
		ERROR("Registering base namespace for the message xpath context failed.");
		nc_msg_free(msg);
		return NULL;
	}

	return (msg);
}

/**
 * @brief Create the server \<hello\> message.
 * @ingroup internalAPI
 * @param caps List of server capabilities.
 * @param session_id Generated NETCONF session ID string.
 * @return rpc structure with the created server \<hello\> message.
 */
static nc_rpc *nc_msg_server_hello(char **cpblts, char* session_id)
{
	nc_rpc *msg;

	msg = nc_msg_client_hello(cpblts);
	if (msg == NULL) {
		return (NULL);
	}
	msg->error = NULL;

	/* assign session-id */
	/* check if session-id is prepared */
	if (session_id == NULL || strisempty(session_id)) {
		/* no session-id set */
		ERROR("Hello: session ID is empty");
		xmlFreeDoc(msg->doc);
		free(msg);
		return (NULL);
	}

	/* create <session-id> node */
	xmlNewChild(msg->doc->children, msg->doc->children->ns, BAD_CAST "session-id", BAD_CAST session_id);

	return (msg);
}

static int hello_timeout = -1;
API void nc_hello_timeout(int timeout)
{
	if (timeout < 0) {
		/* normalize negative value */
		hello_timeout = -1;
	} else {
		hello_timeout = timeout;
	}
}

#define HANDSHAKE_SIDE_SERVER 1
#define HANDSHAKE_SIDE_CLIENT 2
static int nc_handshake(struct nc_session *session, char** cpblts, nc_rpc *hello, int side)
{
	int retval = EXIT_SUCCESS;
	int i;
	nc_reply *recv_hello = NULL;
	char **recv_cpblts = NULL, **merged_cpblts = NULL;
	NC_MSG_TYPE reply = NC_MSG_UNKNOWN;

	if (nc_session_send_rpc(session, hello) == 0) {
		return (EXIT_FAILURE);
	}

#ifdef DISABLE_LIBSSH
	if (side == HANDSHAKE_SIDE_CLIENT) {
		recv_hello = read_hello_openssh(session);
		if (recv_hello) {
			reply = NC_MSG_HELLO;
		}
	} else {
		reply = nc_session_recv_reply(session, hello_timeout, &recv_hello);
	}
#else
	reply = nc_session_recv_reply(session, hello_timeout, &recv_hello);
#endif
	if (reply != NC_MSG_HELLO || recv_hello == NULL) {
		if (reply == NC_MSG_WOULDBLOCK) {
			ERROR("Hello timeout expired.");
		}
		return (EXIT_FAILURE);
	}

	if ((recv_cpblts = nc_parse_hello((struct nc_msg*) recv_hello, session)) == NULL) {
		nc_reply_free(recv_hello);
		return (EXIT_FAILURE);
	}
	nc_reply_free(recv_hello);

	if (side == HANDSHAKE_SIDE_CLIENT) {
		merged_cpblts = nc_accept_server_cpblts(recv_cpblts, cpblts, &(session->version));
	} else if (side == HANDSHAKE_SIDE_SERVER) {
		merged_cpblts = nc_accept_server_cpblts(cpblts, recv_cpblts, &(session->version));
	}
	if (merged_cpblts == NULL) {
		retval = EXIT_FAILURE;
	} else if ((session->capabilities = nc_cpblts_new((const char* const*) merged_cpblts)) == NULL) {
		retval = EXIT_FAILURE;
	}

	if (recv_cpblts) {
		i = 0;
		while (recv_cpblts[i]) {
			free(recv_cpblts[i]);
			i++;
		}
		free(recv_cpblts);
	}

	if (merged_cpblts) {
		i = 0;
		while (merged_cpblts[i]) {
			free(merged_cpblts[i]);
			i++;
		}
		free(merged_cpblts);
	}

	return (retval);
}

static int nc_client_handshake(struct nc_session *session, char** cpblts)
{
	nc_rpc *hello;
	int retval;

	/* just to be sure, it should be already done */
	memset(session->session_id, '\0', SID_SIZE);

	/* create client's <hello> message */
	hello = nc_msg_client_hello(cpblts);
	if (hello == NULL) {
		return (EXIT_FAILURE);
	}

	retval = nc_handshake(session, cpblts, hello, HANDSHAKE_SIDE_CLIENT);
	nc_rpc_free(hello);

	return (retval);
}

extern int nc_session_is_monitored(const char* session_id);

static int nc_server_handshake(struct nc_session *session, char** cpblts)
{
	nc_rpc *hello;
	int retval;

	/* set session ID */
	if (nc_info == NULL) {
		ERROR("Unable to generate the NETCONF session ID.");
		return (EXIT_FAILURE);
	}

	pthread_rwlock_wrlock(&(nc_info->lock));
	do {
		snprintf(session->session_id, SID_SIZE, "%lu", ++nc_info->last_session_id);
	} while (nc_session_is_monitored(session->session_id));
	pthread_rwlock_unlock(&(nc_info->lock));

	/* create server's <hello> message */
	hello = nc_msg_server_hello(cpblts, session->session_id);
	if (hello == NULL) {
		return (EXIT_FAILURE);
	}

	retval = nc_handshake(session, cpblts, hello, HANDSHAKE_SIDE_SERVER);
	nc_rpc_free(hello);

	if (retval != EXIT_SUCCESS) {
		if (nc_info) {
			pthread_rwlock_wrlock(&(nc_info->lock));
			nc_info->stats.bad_hellos++;
			pthread_rwlock_unlock(&(nc_info->lock));
		}
	}

	return (retval);
}

API struct nc_session* nc_session_connect_inout(int fd_in, int fd_out, const struct nc_cpblts* cpblts, const char *host, const char *port, const char *username, NC_TRANSPORT transport)
{
	struct nc_session *retval = NULL;
	pthread_mutexattr_t mattr;
	int r = 0;
	struct nc_cpblts *client_cpblts = NULL;

	/*
	 * host, port and username are only informative, connecting to the server
	 * is expected to be done by the process providing in/out file descriptors.
	 * Similarly, the set transport protocol is ignored and informatively the
	 * provided value is stored
	 */

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
	retval->fd_input = fd_in;
	retval->fd_output = fd_out;

	retval->transport_socket = -1;
	retval->transport = transport;
	retval->hostname = host ? strdup(host) : NULL;
	retval->port = port ? strdup(port) : NULL;
	retval->username = username ? strdup(username) : NULL;
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

	retval->status = NC_SESSION_STATUS_WORKING;

	if (cpblts == NULL) {
		if ((client_cpblts = nc_session_get_cpblts_default()) == NULL) {
			VERB("Unable to set the client's NETCONF capabilities.");
			goto error_cleanup;
		}
	} else {
		client_cpblts = nc_cpblts_new((const char* const*)(cpblts->list));
	}

	if (nc_client_handshake(retval, client_cpblts->list) != 0) {
		goto error_cleanup;
	}

	/* set with-defaults capability flags */
	parse_wdcap(retval->capabilities, &(retval->wd_basic), &(retval->wd_modes));

	/* cleanup */
	nc_cpblts_free(client_cpblts);

	return (retval);

error_cleanup:
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

		free(retval);
	}
	return (NULL);
}

struct nc_session* _nc_session_connect(const char* host, unsigned short port, const char* username, const struct nc_cpblts* cpblts, void* ssh_sess)
{
	struct nc_session *retval = NULL;
	struct nc_cpblts *client_cpblts = NULL;
	char port_s[SHORT_INT_LENGTH];
	NC_TRANSPORT *transport_proto = NULL;

	/* set default values */
	if (host == NULL || strisempty(host)) {
		host = "localhost";
	}
	if (port == 0) {
		port = NC_PORT;
	}

	if (snprintf(port_s, SHORT_INT_LENGTH, "%d", port) < 0) {
		/* converting short int to the string failed */
		ERROR("Unable to convert the port number to a string.");
		return (NULL);
	}

#ifdef ENABLE_TLS
	pthread_once(&transproto_key_once, transproto_init);
	if ((transport_proto = pthread_getspecific(transproto_key)) == NULL) {
		pthread_setspecific(transproto_key, &proto_ssh);
		transport_proto = &proto_ssh;
	}

	if (*transport_proto == NC_TRANSPORT_TLS) {
		retval = nc_session_connect_tls(username, host, port_s);
	} else {
		retval = nc_session_connect_ssh(username, host, port_s, ssh_sess);
	}
#else  /* not ENABLE_TLS */
	retval = nc_session_connect_ssh(username, host, port_s, ssh_sess);
#endif /* not ENABLE_TLS */

	if (retval == NULL) {
		return (NULL);
	}

	retval->transport = (transport_proto == NULL) ? NC_TRANSPORT_SSH : *transport_proto;
	retval->status = NC_SESSION_STATUS_WORKING;

	if (cpblts == NULL) {
		if ((client_cpblts = nc_session_get_cpblts_default()) == NULL) {
			VERB("Unable to set the client's NETCONF capabilities.");
			goto shutdown;
		}
	} else {
		client_cpblts = nc_cpblts_new((const char* const*)(cpblts->list));
	}

	if (nc_client_handshake(retval, client_cpblts->list) != 0) {
		goto shutdown;
	}

	/* set with-defaults capability flags */
	parse_wdcap(retval->capabilities, &(retval->wd_basic), &(retval->wd_modes));

	/* cleanup */
	nc_cpblts_free(client_cpblts);

	return (retval);

shutdown:

	/* cleanup */
	nc_session_close(retval, NC_SESSION_TERM_OTHER);
	nc_session_free(retval);
	nc_cpblts_free(client_cpblts);

	return (NULL);
}

API struct nc_session* nc_session_connect(const char* host, unsigned short port, const char* username, const struct nc_cpblts* cpblts)
{
    return _nc_session_connect(host, port, username, cpblts, NULL);
}

#ifdef DISABLE_LIBSSH

API struct nc_session *nc_session_connect_channel(struct nc_session* UNUSED(session), const struct nc_cpblts* UNUSED(cpblts))
{
	ERROR("%s: SSH channels are provided only with libssh.", __func__);
	return (NULL);
}

#else

API struct nc_session *nc_session_connect_channel(struct nc_session* session, const struct nc_cpblts* cpblts)
{
	struct nc_session *retval, *session_aux;
	struct nc_cpblts *client_cpblts = NULL;

#ifdef ENABLE_TLS
	if (session == NULL || session->is_server || session->tls) {
		/* we cannot open SSH channel in TLS connection */
#else /* not ENABLE_TLS */
	if (session == NULL || session->is_server) {
#endif
		/* we can open channel only for client-side, no-dummy sessions */
		ERROR("Invalid session for opening another channel");
		return (NULL);
	}

	retval = nc_session_connect_libssh_channel(session);
	if (retval == NULL) {
		return (NULL);
	}

	if (cpblts == NULL) {
		if ((client_cpblts = nc_session_get_cpblts_default()) == NULL) {
			VERB("Unable to set the client's NETCONF capabilities.");
			goto shutdown;
		}
	} else {
		client_cpblts = nc_cpblts_new((const char* const*)(cpblts->list));
	}

	if (nc_client_handshake(retval, client_cpblts->list) != 0) {
		goto shutdown;
	}

	/* set with-defaults capability flags */
	parse_wdcap(retval->capabilities, &(retval->wd_basic), &(retval->wd_modes));

	/* cleanup */
	nc_cpblts_free(client_cpblts);

	/*
	 * link sessions:
	 * session <-> retval <-> session_aux
	 */
	session_aux = session->next;
	if (session_aux != NULL) {
		session_aux->prev = retval;
	}
	session->next = retval;
	retval->next = session_aux;
	retval->prev = session;

	return (retval);

shutdown:

	/* cleanup */
	nc_session_close(retval, NC_SESSION_TERM_OTHER);
	nc_session_free(retval);
	nc_cpblts_free(client_cpblts);

	return (NULL);
}

#endif /* not DISABLE_LIBSSH */

struct nc_session* _nc_session_accept(const struct nc_cpblts* capabilities, const char* username, int input, int output, void* ssh_chan, void* tls_sess)
{
	int r, i;
	struct nc_session *retval = NULL;
	struct nc_cpblts *server_cpblts = NULL;
	struct passwd *pw;
	char *wdc, *wdc_aux, *straux;
	char list[255];
	NCWD_MODE mode;
	char** nslist;
	pthread_mutexattr_t mattr;
#ifdef HAVE_UTMPX_H
	struct utmpx protox, *utp;
#endif

	if (username == NULL) {
		/*
		 * get username - we are running as SSH Subsystem (or some other process)
		 * which was started under the user which was connecting to NETCONF server
		 */
		pw = getpwuid(getuid());
		if (pw == NULL) {
			/* unable to get correct username */
			ERROR("Unable to get username for the NETCONF session (%s).", strerror(errno));
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
	memset(retval, 0, sizeof(struct nc_session));
	if ((retval->stats = malloc (sizeof (struct nc_session_stats))) == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		free(retval);
		return NULL;
	}
	retval->is_server = 1;
	retval->transport_socket = -1;
	retval->fd_input = input;
	retval->fd_output = output;
#ifdef ENABLE_TLS
	retval->tls = tls_sess;
#endif
#ifndef DISABLE_LIBSSH
	retval->ssh_chan = ssh_chan;
#endif
	retval->msgid = 1;
	retval->queue_event = NULL;
	retval->queue_msg = NULL;
	retval->monitored = 0;
	retval->stats->in_rpcs = 0;
	retval->stats->in_bad_rpcs = 0;
	retval->stats->out_rpc_errors = 0;
	retval->stats->out_notifications = 0;

	if (pthread_mutexattr_init(&mattr) != 0) {
		ERROR("Memory allocation failed (%s:%d).", __FILE__, __LINE__);
		return (NULL);
	}
	retval->mut_channel = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
	if ((r = pthread_mutex_init(retval->mut_channel, &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_mqueue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_equeue), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_ntf), &mattr)) != 0 ||
			(r = pthread_mutex_init(&(retval->mut_session), &mattr)) != 0) {
		ERROR("Mutex initialization failed (%s).", strerror(r));
		pthread_mutexattr_destroy(&mattr);
		return (NULL);
	}
	pthread_mutexattr_destroy(&mattr);

	retval->username = strdup(username);
	retval->groups = nc_get_grouplist(retval->username);
	/* detect if user ID is nacm_recovery_uid -> then the session is recovery */
	pw = getpwnam(retval->username);
	if (pw && pw->pw_uid == NACM_RECOVERY_UID) {
		retval->nacm_recovery = 1;
	} else {
		retval->nacm_recovery = 0;
	}

	if (capabilities == NULL) {
		if ((server_cpblts = nc_session_get_cpblts_default()) == NULL) {
			VERB("Unable to set the client's NETCONF capabilities.");
			nc_session_close(retval, NC_SESSION_TERM_OTHER);
			return (NULL);
		}
	} else {
		server_cpblts = nc_cpblts_new((const char* const*)(capabilities->list));
	}
	/* set with-defaults capability announcement */
	if ((nc_cpblts_get(server_cpblts, NC_CAP_WITHDEFAULTS_ID) != NULL)
         && ((mode = ncdflt_get_basic_mode()) != NCWD_MODE_NOTSET)) {
		switch(mode) {
		case NCWD_MODE_ALL:
			wdc_aux = "?basic-mode=report-all";
			break;
		case NCWD_MODE_TRIM:
			wdc_aux = "?basic-mode=trim";
			break;
		case NCWD_MODE_EXPLICIT:
			wdc_aux = "?basic-mode=explicit";
			break;
		default:
			wdc_aux = NULL;
			break;
		}
		if (wdc_aux != NULL) {
			mode = ncdflt_get_supported();
			list[0] = 0;
			if ((mode & NCWD_MODE_ALL) != 0) {
				strcat(list, ",report-all");
			}
			if ((mode & NCWD_MODE_ALL_TAGGED) != 0) {
				strcat(list, ",report-all-tagged");
			}
			if ((mode & NCWD_MODE_TRIM) != 0) {
				strcat(list, ",trim");
			}
			if ((mode & NCWD_MODE_EXPLICIT) != 0) {
				strcat(list, ",explicit");
			}

			if (strnonempty(list)) {
				list[0] = '='; /* replace initial comma */
				r = asprintf(&wdc, "urn:ietf:params:netconf:capability:with-defaults:1.0%s&amp;also-supported%s", wdc_aux, list);
			} else {
				/* no also-supported */
				r = asprintf(&wdc, "urn:ietf:params:netconf:capability:with-defaults:1.0%s", wdc_aux);
			}

			if (r != -1) {
				/* add/update capabilities list */
				nc_cpblts_add(server_cpblts, wdc);
				free(wdc);
			} else {
				WARN("asprintf() failed - with-defaults capability parameters may not be set properly (%s:%d).", __FILE__, __LINE__);
			}
		}
	}

#ifndef DISABLE_URL
	if (nc_cpblts_get(server_cpblts, NC_CAP_URL_ID) != NULL) {
		/* update URL capability with enabled protocols */
		straux = nc_url_gencap();
		nc_cpblts_add(server_cpblts, straux);
		free(straux);
	}
#endif

	retval->status = NC_SESSION_STATUS_WORKING;

	/* add namespaces of used datastores as announced capabilities */
	if ((nslist = get_schemas_capabilities(server_cpblts)) != NULL) {
		for(i = 0; nslist[i] != NULL; i++) {
			nc_cpblts_add(server_cpblts, nslist[i]);
			free(nslist[i]);
		}
		free(nslist);
	}

	if (nc_server_handshake(retval, server_cpblts->list) != 0) {
		nc_session_close(retval, NC_SESSION_TERM_BADHELLO);
		nc_session_free(retval);
		nc_cpblts_free(server_cpblts);
		return (NULL);
	}

	/* monitor the session if on */
	if ((nc_init_flags & NC_INIT_MONITORING) && nc_session_monitor(retval) != EXIT_SUCCESS) {
		return (NULL);
	}

	/* get client's hostname */
	/*
	 * This is not a critical information, so only warnings are generated if
	 * the following process fails. The hostname of the client is used by
	 * server to generate NETCONF base notifications.
	 */
	if ((straux = getenv("SSH_CLIENT")) != NULL) {
		/* OpenSSH implementation provides SSH_CLIENT environment variable */
		retval->hostname = strdup(straux);
		if ((straux = strchr(retval->hostname, ' ')) != NULL ) {
			*straux = 0; /* null byte after IP in $SSH_CLIENT */
		}
#ifdef HAVE_UTMPX_H
	} else {
		/*
		 * in other cases, we will try to get information from the utmpx
		 * file of this session
		 */
		if ((straux = ttyname(fileno(stdin))) == 0) {
			WARN("Unable to get tty (%s) to get the client's hostname (session %s).", strerror(errno), retval->session_id);
		} else {
			if (strncmp(straux, "/dev/", 5) == 0) {
				straux += 5;
			}
			memset(&protox, 0, sizeof protox);
			strcpy(protox.ut_line, straux);

			if ((utp = getutxline(&protox)) == 0) {
				WARN("Unable to locate UTMPX for \'%s\' to get the client's hostname (session %s).", straux, retval->session_id);
			} else {
				retval->hostname = malloc(sizeof(char) * (1 + sizeof(utp->ut_host)));
				memcpy(retval->hostname, utp->ut_host, sizeof(utp->ut_host));
				retval->hostname[sizeof(utp->ut_host)] = 0;
			}
		}
#endif
	}

	/* set with-defaults capability flags */
	parse_wdcap(retval->capabilities, &(retval->wd_basic), &(retval->wd_modes));

	/* cleanup */
	nc_cpblts_free(server_cpblts);

#ifndef DISABLE_NOTIFICATIONS
	/* log start of the session */
	ncntf_event_new(-1, NCNTF_BASE_SESSION_START, retval);
#endif

	retval->logintime = nc_time2datetime(time(NULL), NULL);
	if (nc_info) {
		pthread_rwlock_wrlock(&(nc_info->lock));
		nc_info->stats.sessions_in++;
		pthread_rwlock_unlock(&(nc_info->lock));
	}

	if (pw) {
		VERB("Created session %s for user \'%s\' (UID %d)%s",
			retval->session_id,
			retval->username,
			pw->pw_uid,
			retval->nacm_recovery ? " (recovery)" : "");
	}

	return (retval);
}

API struct nc_session *nc_session_accept_inout(const struct nc_cpblts* capabilities, const char* username, int input, int output)
{
	return (_nc_session_accept(capabilities, username, input, output, NULL, NULL));
}

API struct nc_session *nc_session_accept_username(const struct nc_cpblts* capabilities, const char* username)
{
	/*
	 * just a wrapper (backward compatibility) for the
	 * nc_session_accept_username_inout(), which allows explicitely set the
	 * input/output file descriptors for reading/writing NETCONF data.
	 */
	return (_nc_session_accept(capabilities, username, STDIN_FILENO, STDOUT_FILENO, NULL, NULL));
}

API struct nc_session *nc_session_accept(const struct nc_cpblts* capabilities)
{
	/*
	 * just a wrapper (backward compatibility) for the
	 * nc_session_accept_username_inout(), which gets the current user of the
	 * running process in case the username argument is NULL.
	 */
	return (_nc_session_accept(capabilities, NULL, STDIN_FILENO, STDOUT_FILENO, NULL, NULL));
}

/*
 * CALL HOME PART
 */

#ifndef DISABLE_LIBSSH

/* 0 is IPv4, 1 is IPv6 */
static struct pollfd reverse_listen_socket[2] = {{-1, POLLIN, 0}, {-1, POLLIN, 0}};

static int get_socket(const char* port, int family)
{
	struct addrinfo hints, *res_list, *res;
	int sock = -1;
	int i = 1;
	int optval;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = family;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	i = getaddrinfo(NULL, port, &hints, &res_list);
	if (i != 0) {
		ERROR("Unable to translate the host address (%s).", gai_strerror(i));
		return (EXIT_FAILURE);
	}

	for (i = 1, res = res_list; res != NULL; res = res->ai_next) {
		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sock == -1) {
			/* socket was not created, try another resource */
			i = errno;
			continue;
		}

		/* allow both IPv4 and IPv6 sockets to listen on the same port */
		optval = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
			ERROR("Unable to set SO_REUSEADDR (%s)", strerror(errno));
		}
		if (family == AF_INET6 && setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof(optval)) == -1) {
			ERROR("Unable to limit IPv6 socket only to IPv6 (IPV6_V6ONLY) (%s)", strerror(errno));
		}

		if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
			/* binding the port failed, try another resource */
			i = errno;
			close(sock);
			sock = -1;
			continue;
		}

		/* we're done, network connection established */
		break;
	}
	freeaddrinfo(res_list);

	if (sock == -1) {
		ERROR("Unable to start prepare socket on %s port %s (%s).", (family == AF_INET ? "IPv4" : "IPv6"), port, strerror(i));
	} else {
		VERB("Socket %d on port %s.", sock, port);
	}
	return (sock);
}

static int set_socket_listening(int sock)
{
	if (sock == -1) {
		return (0);
	}

	if (listen(sock, NC_REVERSE_QUEUE) == -1) {
		ERROR("Unable to start listening (%s).", strerror(errno));
		return (-1);
	}

	VERB("Listening on socket %d.", sock);
	return (0);
}

API int nc_callhome_listen(unsigned int port)
{
	char port_s[SHORT_INT_LENGTH];

	if (reverse_listen_socket[0].fd != -1 || reverse_listen_socket[1].fd != -1) {
		ERROR("%s: libnetconf is already listening for incoming call home.", __func__);
		return (EXIT_FAILURE);
	}

	/* set default values */
	if (port == 0) {
		port = NC_REVERSE_PORT;
	}

	if (snprintf(port_s, SHORT_INT_LENGTH, "%d", port) < 0) {
		/* converting short int to the string failed */
		ERROR("Unable to convert the port number to a string.");
		return (EXIT_FAILURE);
	}

	reverse_listen_socket[0].fd = get_socket(port_s, AF_INET);
	reverse_listen_socket[1].fd = get_socket(port_s, AF_INET6);
	if (set_socket_listening(reverse_listen_socket[0].fd) ||
			set_socket_listening(reverse_listen_socket[1].fd)) {
		close(reverse_listen_socket[0].fd);
		close(reverse_listen_socket[1].fd);
		reverse_listen_socket[0].fd = -1;
		reverse_listen_socket[1].fd = -1;
		return (EXIT_FAILURE);
	}

	if (reverse_listen_socket[0].fd == -1 && reverse_listen_socket[1].fd == -1) {
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}

API int nc_callhome_listen_stop(void)
{
	if (reverse_listen_socket[0].fd == -1 && reverse_listen_socket[1].fd == -1) {
		ERROR("%s: libnetconf is not listening for incoming call home.", __func__);
		return (EXIT_FAILURE);
	}

	close(reverse_listen_socket[0].fd);
	close(reverse_listen_socket[1].fd);
	reverse_listen_socket[0].fd = -1;
	reverse_listen_socket[1].fd = -1;

	return (EXIT_SUCCESS);
}


API int nc_callhome_connect(struct nc_mngmt_server *host_list, uint8_t reconnect_secs, uint8_t reconnect_count, const char* server_path, char *const argv[], int *com_socket)
{
	struct nc_mngmt_server *srv_iter;
	struct addrinfo *addr;
	void *addr_p;
	char addr_buf[INET6_ADDRSTRLEN];
	unsigned short port;
	int sock, sock6, sock4;
	int i;
	int pid = -1;
	char* const *server_argv;
	char* const sshd_argv[] = {"/usr/sbin/sshd", "-ddd", "-i", NULL};
	char* const stunnel_argv[] = {"/usr/sbin/stunnel", NULL};
	NC_TRANSPORT *transport_proto;

	if (server_path == NULL) {
		pthread_once(&transproto_key_once, transproto_init);
		if ((transport_proto = pthread_getspecific(transproto_key)) == NULL) {
			pthread_setspecific(transproto_key, &proto_ssh);
			transport_proto = &proto_ssh;
		}

		switch(*transport_proto) {
		case NC_TRANSPORT_SSH:
			server_path = "/usr/sbin/sshd";
			server_argv = sshd_argv;
			break;
		case NC_TRANSPORT_TLS:
			server_path = "/usr/sbin/stunnel";
			server_argv = stunnel_argv;
			break;
		default:
			ERROR("%s: Unknown transport protocol (%d)", __func__, *transport_proto);
			return (-1);
		}
	} else {
		server_argv = argv;
	}
	VERB("Call home using \'%s\' server.", server_path);

	/* prepare a socket */
	if ((sock4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		ERROR("%s: creating IPv4 socket failed (%s)", __func__, strerror(errno));
		WARN("%s: IPv4 connection to management servers will not be available.", __func__);
	}
	if ((sock6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		ERROR("%s: creating IPv6 socket failed (%s)", __func__, strerror(errno));
		WARN("%s: IPv6 connection to management servers will not be available.", __func__);
	}
	if (sock4 == -1 && sock6 == -1) {
		ERROR("%s: Unable to connect to any management server, creating sockets failed.", __func__);
		return (-1);
	}

	/* remove active flag from the last connected management server information */
	srv_iter = nc_callhome_mngmt_server_getactive(host_list);
	if (srv_iter) {
		srv_iter->active = 0;
	}

	/* since host_list is supposed to be ring list, this is potentially never ending loop */
	for (srv_iter = host_list; srv_iter != NULL; srv_iter = srv_iter->next) {
		for (addr = srv_iter->addr; addr != NULL; addr = addr->ai_next) {
			switch (addr->ai_family) {
			case AF_INET:
				if (sock4 == -1) {
					continue;
				} else {
					sock = sock4;
					addr_p = &(((struct sockaddr_in *)addr->ai_addr)->sin_addr);
					port = ntohs(((struct sockaddr_in *)addr->ai_addr)->sin_port);
				}
				break;
			case AF_INET6:
				if (sock6 == -1) {
					continue;
				} else {
					sock = sock6;
					addr_p = &(((struct sockaddr_in6 *)addr->ai_addr)->sin6_addr);
					port = ntohs(((struct sockaddr_in6 *)addr->ai_addr)->sin6_port);
				}
				break;
			default:
				continue;
			}

			inet_ntop(addr->ai_family, addr_p, addr_buf, INET6_ADDRSTRLEN);
			for (i = 0; i < reconnect_count; i++) {
				if (connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
					WARN("Connecting to %s:%u failed (%s)", addr_buf, port, strerror(errno));
					sleep(reconnect_secs);
					continue;
				}
				VERB("Connected to %s:%u.", addr_buf, port);
				/* close unused socket */
				if (sock == sock4) {
					close(sock6);
				} else {
					close(sock4);
				}

				/* go to start SSH daemon */
				goto connected;
			}
		}
	}

	close(sock4);
	close(sock6);
	return(-1);

connected:
	/* execute a transport protocol server */
	pid = fork();
	if (pid == -1) {
		ERROR("Forking process for a transport server failed (%s)", strerror(errno));
		close(sock);
	} else if (pid == 0) {
		/* child (future sshd) process */
		int log = open("/tmp/netconf_callhome.log", O_RDWR | O_CREAT, 0666);
		/* redirect stdin/stdout to the communication socket */
		dup2(sock, STDIN_FILENO);
		dup2(sock, STDOUT_FILENO);
		dup2(log, STDERR_FILENO);

		/* start the sshd */
		execv(server_path, server_argv);

		/* you never should be here! */
		ERROR("Executing transport server (%s) failed (%s).", server_path, strerror(errno));
		exit(1);
	} else {
		/* parent (current app) */
		if (com_socket) {
			/*
			 * do not close sock, it can be used by caller
			 * to monitor communication activities
			 */
			*com_socket = sock;
		} else {
			close(sock);
		}
	}

	/* mark the management server as connected */
	srv_iter->active = 1;

	return (pid);
}

API struct nc_session *nc_callhome_accept(const char *username, const struct nc_cpblts* cpblts, int *timeout)
{
	struct nc_session* retval = NULL;
	struct nc_cpblts *client_cpblts;
	int sock;
	struct sockaddr_storage remote;
	socklen_t addr_size = sizeof(remote);
	char port[SHORT_INT_LENGTH];
	char host[INET6_ADDRSTRLEN];
	int status, i, flags;
	NC_TRANSPORT *transport_proto;

	pthread_once(&transproto_key_once, transproto_init);
	if ((transport_proto = pthread_getspecific(transproto_key)) == NULL) {
		pthread_setspecific(transproto_key, &proto_ssh);
		transport_proto = &proto_ssh;
	}

#ifndef ENABLE_TLS
	if (*transport_proto == NC_TRANSPORT_TLS) {
		ERROR("%s: call home via TLS is provided only with --enable-tls option.", __func__);
		return (NULL);
	}
#endif

	if (reverse_listen_socket[0].fd == -1 && reverse_listen_socket[1].fd == -1) {
		ERROR("No listening socket, use nc_session_reverse_listen() first.");
		return (NULL);
	}

	reverse_listen_socket[0].revents = 0;
	reverse_listen_socket[1].revents = 0;
	while (1) {
		DBG("Waiting %dms for incoming call home connections...", *timeout);
		status = poll(reverse_listen_socket, 2, *timeout);

		if (status == 0) {
			/* timeout */
			*timeout = 0;
			return (NULL);
		} else if ((status == -1) && (errno == EINTR)) {
			/* poll was interrupted - try it again */
			continue;
		} else if (status < 0) {
			/* poll failed - something wrong happened */
			ERROR("Polling call home sockets failed (%s)", strerror(errno));
			return (NULL);
		} else if (status > 0) {
			for (i = 0; i < 2; i++) {
				if ((reverse_listen_socket[i].revents & POLLHUP) || (reverse_listen_socket[i].revents & POLLERR)) {
					/* close pipe/fd - other side already did it */
					ERROR("Listening socket is down.");
					close(reverse_listen_socket[i].fd);
					return (NULL );
				} else if (reverse_listen_socket[i].revents & POLLIN) {
					/* accept call home */
					sock = accept(reverse_listen_socket[i].fd, (struct sockaddr*) &remote, &addr_size);
					goto netconf_connect;
				}
			}
		}
	}

netconf_connect:
	if (sock == -1) {
		ERROR("Accepting call home failed (%s)", strerror(errno));
		return (NULL);
	}
	/* make the socket non-blocking */
	if (((flags = fcntl(sock, F_GETFL)) == -1) || (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)) {
		ERROR("Fcntl failed (%s).", strerror(errno));
		close(sock);
		return NULL;
	}

	port[0] = '\0';
	host[0] = '\0';
	if (remote.ss_family == AF_INET) {
		struct sockaddr_in* remote_in = (struct sockaddr_in*)&remote;
		snprintf(port, SHORT_INT_LENGTH, "%5u", ntohs(remote_in->sin_port));
		inet_ntop(AF_INET, &(remote_in->sin_addr), host, INET6_ADDRSTRLEN);
	} else if (remote.ss_family == AF_INET6) {
		struct sockaddr_in6* remote_in = (struct sockaddr_in6*)&remote;
		snprintf(port, SHORT_INT_LENGTH, "%5u", ntohs(remote_in->sin6_port));
		inet_ntop(AF_INET6, &(remote_in->sin6_addr), host, INET6_ADDRSTRLEN);
	} else {
		/* wtf?!? */
	}

#ifdef ENABLE_TLS
	/* we can choose from transport protocol according to nc_session_transport() */
	if (*transport_proto == NC_TRANSPORT_TLS) {
		retval = nc_session_connect_tls_socket(username, host, sock);
	} else {
#else
	{
#endif
		retval = nc_session_connect_libssh_socket(username, host, sock, NULL);
	}

	if (retval != NULL) {
		retval->hostname = strdup(host);
		retval->port = strdup(port);
	} else {
		close(sock);
		sock = -1;

		return(NULL);
	}

	retval->status = NC_SESSION_STATUS_WORKING;

	if (cpblts == NULL) {
		if ((client_cpblts = nc_session_get_cpblts_default()) == NULL) {
			VERB("Unable to set the client's NETCONF capabilities.");
			goto shutdown;
		}
	} else {
		client_cpblts = nc_cpblts_new((const char* const*)(cpblts->list));
	}

	if (nc_client_handshake(retval, client_cpblts->list) != 0) {
		goto shutdown;
	}

	/* set with-defaults capability flags */
	parse_wdcap(retval->capabilities, &(retval->wd_basic), &(retval->wd_modes));

	/* cleanup */
	nc_cpblts_free(client_cpblts);

	return (retval);

shutdown:

	/* cleanup */
	nc_session_close(retval, NC_SESSION_TERM_OTHER);
	nc_session_free(retval);
	nc_cpblts_free(client_cpblts);

	return (NULL);
}

API struct nc_session* nc_session_connect_libssh_sess(const char* host, unsigned short port, const char* username, const struct nc_cpblts* cpblts, ssh_session ssh_sess)
{
    return _nc_session_connect(host, port, username, cpblts, ssh_sess);
}

#endif

API struct nc_mngmt_server *nc_callhome_mngmt_server_add(struct nc_mngmt_server* list, const char* host, const char* port)
{
	struct nc_mngmt_server *item, *start, *end;
	struct addrinfo hints;
	int r;

	if (host == NULL || port == NULL) {
		return (NULL);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	item = malloc(sizeof(struct nc_mngmt_server));
	if ((r = getaddrinfo(host, port, &hints, &(item->addr))) != 0) {
		ERROR("Unable to get information about remote server %s (%s)", host, gai_strerror(r));
		free(item);
		return (NULL);
	}
	item->active = 0;

	if (list == NULL) {
		start = item;
		end = item;
	} else {
		start = list;
		/* find end of the ring list */
		for(end = list; end->next != list; end = end->next) {
			if (end->next == NULL) {
				/* it was not ring list, make it */
				end->next = list;
				break;
			}
		}
	}

	/* add the new item into the ring list */
	end->next = item;
	item->next = start;

	return (start);
}

API int nc_callhome_mngmt_server_rm(struct nc_mngmt_server* list, struct nc_mngmt_server* remove)
{
	struct nc_mngmt_server *iter;

	/* locate the item to remove */
	for(iter = list; iter != NULL && iter->next != remove && iter->next != list; iter = iter->next);

	if (iter == NULL) {
		/* list is empty or remove is not present in the list (and list is not ring list) */
		return (EXIT_FAILURE);
	} else if (iter->next == list) {
		/* remove was not found in the list, yet check the first item in the list */
		if (list != remove) {
			return (EXIT_FAILURE);
		}
	}
	/* else remove found, modify the list */
	iter->next = remove->next;
	remove->next = remove; /* keep it ring */

	return (EXIT_SUCCESS);
}

API int nc_callhome_mngmt_server_free(struct nc_mngmt_server* list)
{
	struct nc_mngmt_server *iter, *aux;

	if (list == NULL) {
		return (EXIT_FAILURE);
	} else if (list->next == NULL) {
		freeaddrinfo(list->addr);
		free(list);
	} else {
		for(iter = list->next, list->next = NULL; iter != NULL; ) {
			if (iter->next == NULL && iter != list) {
				/* the list was not ring, so we have to free the start of the list */
				freeaddrinfo(list->addr);
				free(list);
			}
			aux = iter->next;
			freeaddrinfo(iter->addr);
			free(iter);
			iter = aux;
		}
	}

	return (EXIT_SUCCESS);
}

API struct nc_mngmt_server *nc_callhome_mngmt_server_getactive(struct nc_mngmt_server* list)
{
	struct nc_mngmt_server* srv_iter;

	for (srv_iter = list; srv_iter != NULL && srv_iter->next != list && srv_iter->active == 0; srv_iter = srv_iter->next);

	if (srv_iter && srv_iter->active == 1) {
		return (srv_iter);
	} else {
		return (NULL);
	}
}
