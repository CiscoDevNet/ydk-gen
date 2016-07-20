/**
 * \file tls.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Implementation of functions to connect to NETCONF server over TLS.
 *
 * Copyright (c) 2014 CESNET, z.s.p.o.
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

#include "config.h"
#include "tls.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <pwd.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include "netconf_internal.h"

/* global SSL context (SSL_CTX*) */
static pthread_key_t tls_ctx_key;
/* global SSL X509 store (X509_STORE*) */
static pthread_key_t tls_store_key;
static pthread_once_t tls_ctx_once = PTHREAD_ONCE_INIT;

static void tls_ctx_init(void)
{
	pthread_key_create(&tls_ctx_key, NULL);

	/* init OpenSSL */
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	SSL_library_init();
}

API void nc_tls_destroy(void)
{
	SSL_CTX* tls_ctx;

	tls_ctx = pthread_getspecific(tls_ctx_key);
	if (tls_ctx) {
		SSL_CTX_free(tls_ctx);
	}
	pthread_setspecific(tls_ctx_key, NULL);
}

/* based on the code of stunnel utility */
int verify_callback(int preverify_ok, X509_STORE_CTX *x509_ctx) {
	X509_STORE* store;
	X509_STORE_CTX store_ctx;
	X509_OBJECT obj;
	X509_NAME* subject;
	X509_NAME* issuer;
	X509* cert;
	X509_CRL* crl;
	X509_REVOKED* revoked;
	EVP_PKEY* pubkey;
	int i, n, rc;
	ASN1_TIME* next_update = NULL;

	if (!preverify_ok) {
		return 0;
	}

	if ((store = pthread_getspecific(tls_store_key)) == NULL) {
		ERROR("Failed to get thread-specific X509 store");
		return 1; /* fail */
	}

	cert = X509_STORE_CTX_get_current_cert(x509_ctx);
	subject = X509_get_subject_name(cert);
	issuer = X509_get_issuer_name(cert);

	/* try to retrieve a CRL corresponding to the _subject_ of
	 * the current certificate in order to verify it's integrity */
	memset((char *)&obj, 0, sizeof obj);
	X509_STORE_CTX_init(&store_ctx, store, NULL, NULL);
	rc = X509_STORE_get_by_subject(&store_ctx, X509_LU_CRL, subject, &obj);
	X509_STORE_CTX_cleanup(&store_ctx);
	crl = obj.data.crl;
	if (rc > 0 && crl) {
		next_update = X509_CRL_get_nextUpdate(crl);

		/* verify the signature on this CRL */
		pubkey = X509_get_pubkey(cert);
		if (X509_CRL_verify(crl, pubkey) <= 0) {
			X509_STORE_CTX_set_error(x509_ctx, X509_V_ERR_CRL_SIGNATURE_FAILURE);
			X509_OBJECT_free_contents(&obj);
			if (pubkey) {
				EVP_PKEY_free(pubkey);
			}
			return 0; /* fail */
		}
		if (pubkey) {
			EVP_PKEY_free(pubkey);
		}

		/* check date of CRL to make sure it's not expired */
		if (!next_update) {
			X509_STORE_CTX_set_error(x509_ctx, X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD);
			X509_OBJECT_free_contents(&obj);
			return 0; /* fail */
		}
		if (X509_cmp_current_time(next_update) < 0) {
			X509_STORE_CTX_set_error(x509_ctx, X509_V_ERR_CRL_HAS_EXPIRED);
			X509_OBJECT_free_contents(&obj);
			return 0; /* fail */
		}
		X509_OBJECT_free_contents(&obj);
	}

	/* try to retrieve a CRL corresponding to the _issuer_ of
	 * the current certificate in order to check for revocation */
	memset((char *)&obj, 0, sizeof obj);
	X509_STORE_CTX_init(&store_ctx, store, NULL, NULL);
	rc = X509_STORE_get_by_subject(&store_ctx, X509_LU_CRL, issuer, &obj);
	X509_STORE_CTX_cleanup(&store_ctx);
	crl = obj.data.crl;
	if (rc > 0 && crl) {
		/* check if the current certificate is revoked by this CRL */
		n = sk_X509_REVOKED_num(X509_CRL_get_REVOKED(crl));
		for (i = 0; i < n; i++) {
			revoked = sk_X509_REVOKED_value(X509_CRL_get_REVOKED(crl), i);
			if (ASN1_INTEGER_cmp(revoked->serialNumber, X509_get_serialNumber(cert)) == 0) {
				ERROR("Certificate revoked");
				X509_STORE_CTX_set_error(x509_ctx, X509_V_ERR_CERT_REVOKED);
				X509_OBJECT_free_contents(&obj);
				return 0; /* fail */
			}
		}
		X509_OBJECT_free_contents(&obj);
	}
	return 1; /* success */
}

API int nc_tls_init(const char* peer_cert, const char* peer_key, const char *CAfile, const char *CApath, const char *CRLfile, const char *CRLpath)
{
	const char* key_ = peer_key;
	SSL_CTX* tls_ctx;
	X509_LOOKUP* lookup;
	X509_STORE* tls_store;
	int destroy = 0, ret;

	if (peer_cert == NULL) {
		ERROR("%s: Invalid parameter.", __func__);
		return (EXIT_FAILURE);
	}

	pthread_once(&tls_ctx_once, tls_ctx_init);

	tls_ctx = pthread_getspecific(tls_ctx_key);
	if (tls_ctx) {
		VERB("TLS subsystem reinitiation. Resetting certificates settings");
		/*
		 * continue with creation of a new TLS context, the current will be
		 * destroyed after everything successes
		 */
		destroy = 1;
	}

	/* prepare global SSL context, allow only mandatory TLS 1.2  */
	if ((tls_ctx = SSL_CTX_new(TLSv1_2_client_method())) == NULL) {
		ERROR("Unable to create OpenSSL context (%s)", ERR_reason_error_string(ERR_get_error()));
		return (EXIT_FAILURE);
	}

	/* force peer certificate verification (NO_PEER_CERT and CLIENT_ONCE are ignored when
	 * acting as client, but included just in case) and optionaly set CRL checking callback */
	if (CRLfile != NULL || CRLpath != NULL) {
		/* set the revocation store with the correct paths for the callback */
		tls_store = X509_STORE_new();
		tls_store->cache = 0;

		if (CRLfile != NULL) {
			if ((lookup = X509_STORE_add_lookup(tls_store, X509_LOOKUP_file())) == NULL) {
				ERROR("Failed to add lookup method in CRL checking");
				return (EXIT_FAILURE);
			}
			if (X509_LOOKUP_add_dir(lookup, CRLfile, X509_FILETYPE_PEM) != 1) {
				ERROR("Failed to add revocation lookup file");
				return (EXIT_FAILURE);
			}
		}

		if (CRLpath != NULL) {
			if ((lookup = X509_STORE_add_lookup(tls_store, X509_LOOKUP_hash_dir())) == NULL) {
				ERROR("Failed to add lookup method in CRL checking");
				return (EXIT_FAILURE);
			}
			if (X509_LOOKUP_add_dir(lookup, CRLpath, X509_FILETYPE_PEM) != 1) {
				ERROR("Failed to add revocation lookup directory");
				return (EXIT_FAILURE);
			}
		}

		if ((ret = pthread_key_create(&tls_store_key, (void (*)(void *))X509_STORE_free)) != 0) {
			ERROR("Unable to create pthread key: %s", strerror(ret));
			return (EXIT_FAILURE);
		}
		if ((ret = pthread_setspecific(tls_store_key, tls_store)) != 0) {
			ERROR("Unable to set thread-specific data: %s", strerror(ret));
			return (EXIT_FAILURE);
		}

		SSL_CTX_set_verify(tls_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT | SSL_VERIFY_CLIENT_ONCE, verify_callback);
	} else {
		/* CRL checking will be skipped */
		SSL_CTX_set_verify(tls_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT | SSL_VERIFY_CLIENT_ONCE, NULL);
	}

	/* get peer certificate */
	if (SSL_CTX_use_certificate_file(tls_ctx, peer_cert, SSL_FILETYPE_PEM) != 1) {
		ERROR("Loading a peer certificate from \'%s\' failed (%s).", peer_cert, ERR_reason_error_string(ERR_get_error()));
		return (EXIT_FAILURE);
	}

	if (key_ == NULL) {
		/*
		 * if the file with private key not specified, expect that the private
		 * key is stored altogether with the certificate
		 */
		key_ = peer_cert;
	}
	if (SSL_CTX_use_PrivateKey_file(tls_ctx, key_, SSL_FILETYPE_PEM) != 1) {
		ERROR("Loading a peer certificate from \'%s\' failed (%s).", key_, ERR_reason_error_string(ERR_get_error()));
		return (EXIT_FAILURE);
	}

	if(! SSL_CTX_load_verify_locations(tls_ctx, CAfile, CApath))	{
		WARN("SSL_CTX_load_verify_locations() failed (%s).", ERR_reason_error_string(ERR_get_error()));
	}

	/* store TLS context for thread */
	if (destroy) {
		nc_tls_destroy();
	}
	pthread_setspecific(tls_ctx_key, tls_ctx);

	return (EXIT_SUCCESS);
}

struct nc_session* _nc_session_accept(const struct nc_cpblts*, const char*, int, int, void*, void*);

API struct nc_session *nc_session_accept_tls(const struct nc_cpblts* capabilities, const char* username, SSL* tls_sess)
{
	return (_nc_session_accept(capabilities, username, -1, -1, NULL, tls_sess));
}

struct nc_session *nc_session_connect_tls_socket(const char* username, const char* UNUSED(host), int sock)
{
	struct nc_session *retval;
	struct passwd *pw;
	pthread_mutexattr_t mattr;
	int verify, r;
	SSL_CTX* tls_ctx;

	tls_ctx = pthread_getspecific(tls_ctx_key);
	if (tls_ctx == NULL) {
		ERROR("TLS subsystem not initiated.");
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
	memset(retval, 0, sizeof(struct nc_session));
	if ((retval->stats = malloc (sizeof (struct nc_session_stats))) == NULL) {
		ERROR("Memory allocation failed (%s)", strerror(errno));
		free(retval);
		return NULL;
	}

	/* prepare a new TLS structure */
	if ((retval->tls = SSL_new(tls_ctx)) == NULL) {
		ERROR("%s: Unable to prepare TLS structure (%s)", __func__, ERR_reason_error_string(ERR_get_error()));
		free(retval->stats);
		free(retval);
		return (NULL);
	}

	/* connect SSL with existing socket */
	SSL_set_fd(retval->tls, sock);

	/* Set the SSL_MODE_AUTO_RETRY flag to allow OpenSSL perform re-handshake automatically */
	SSL_set_mode(retval->tls, SSL_MODE_AUTO_RETRY);

	/* connect and perform the handshake */
	while (((r = SSL_connect(retval->tls)) == -1) && (SSL_get_error(retval->tls, r) == SSL_ERROR_WANT_READ)) {
		usleep(NC_READ_SLEEP);
	}
	if (r != 1) {
		ERROR("Connecting over TLS failed (%s).", ERR_reason_error_string(ERR_get_error()));
		SSL_free(retval->tls);
		free(retval->stats);
		free(retval);
		return (NULL);
	}

	/* check certificate checking */
	verify = SSL_get_verify_result(retval->tls);
	switch (verify) {
	case X509_V_OK:
		VERB("Server certificate successfully verified.");
		break;
	default:
		WARN("I'm not happy with the server certificate (%s).", verify_ret_msg[verify]);
	}

	/* fill session structure */
	retval->transport_socket = sock;
	retval->fd_input = -1;
	retval->fd_output = -1;
	retval->username = strdup(username);
	retval->groups = NULL; /* client side does not need this information */
	retval->msgid = 1;
	retval->queue_event = NULL;
	retval->queue_msg = NULL;
	retval->logintime = NULL;
	retval->monitored = 0;
	retval->nacm_recovery = 0; /* not needed/decidable on the client side */
	retval->stats->in_rpcs = 0;
	retval->stats->in_bad_rpcs = 0;
	retval->stats->out_rpc_errors = 0;
	retval->stats->out_notifications = 0;

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

	return (retval);
}

/* definition in transport.c */
int transport_connect_socket(const char* host, const char* port);

struct nc_session *nc_session_connect_tls(const char* username, const char* host, const char* port)
{
	struct nc_session *retval = NULL;
	int sock = -1;

	sock = transport_connect_socket(host, port);
	if (sock == -1) {
		return (NULL);
	}

	retval = nc_session_connect_tls_socket(username, host, sock);
	if (retval != NULL) {
		retval->hostname = strdup(host);
		retval->port = strdup(port);
	} else {
		close(sock);
	}

	return (retval);
}
