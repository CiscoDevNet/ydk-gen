/**
 * \file callbacks.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Default callbacks and functions to set the application's callbacks.
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
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

#ifdef ENABLE_DNSSEC
#	include <validator/validator.h>
#	include <validator/resolver.h>
#	include <validator/validator-compat.h>
#endif

#include "netconf_internal.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

#ifndef DISABLE_LIBSSH
static char* callback_sshauth_interactive_default (const char* name,
		const char* instruction,
		const char* prompt,
		int echo);

static char* callback_sshauth_publickey_default (const char* username,
		const char* hostname,
		const char* privatekey_filepath);

static char* callback_sshauth_password_default (const char* username, const char* hostname);
static int callback_ssh_hostkey_check_default (const char* hostname, ssh_session session);
#endif

struct callbacks callbacks = {
		NULL, /* message printing callback */
		NULL, /* process_error_reply callback */
#ifndef DISABLE_LIBSSH
		callback_sshauth_interactive_default, /* default keyboard_interactive callback */
		callback_sshauth_password_default, /* default password callback */
		callback_sshauth_publickey_default, /* default publickey (get passphrase) callback */
		callback_ssh_hostkey_check_default, /* default hostkey check callback */
		{ NULL }, /* publickey file path */
		{ NULL },  /* privatekey file path */
		{ 0 }
#endif
};

API void nc_callback_print(void (*func)(NC_VERB_LEVEL level, const char* msg))
{
	callbacks.print = func;
}

API void nc_callback_error_reply(void (*func)(const char* tag,
		const char* type,
		const char* severity,
		const char* apptag,
		const char* path,
		const char* message,
		const char* attribute,
		const char* element,
		const char* ns,
		const char* sid))
{
	callbacks.process_error_reply = func;
}

#ifndef DISABLE_LIBSSH
API void nc_callback_sshauth_interactive(char* (*func)(const char* name,
		const char* instruction,
		const char* prompt,
		int echo))
{
	if (func != NULL) {
		callbacks.sshauth_interactive = func;
	} else {
		callbacks.sshauth_interactive = callback_sshauth_interactive_default;
	}
}

API void nc_callback_sshauth_password(char* (*func)(const char* username,
		const char* hostname))
{
	if (func != NULL) {
		callbacks.sshauth_password = func;
	} else {
		callbacks.sshauth_password = callback_sshauth_password_default;
	}
}

API void nc_callback_sshauth_passphrase(char* (*func)(const char* username,
		const char* hostname, const char* priv_key_file))
{
	if (func != NULL) {
		callbacks.sshauth_passphrase = func;
	} else {
		callbacks.sshauth_passphrase = callback_sshauth_publickey_default;
	}
}

API void nc_callback_ssh_host_authenticity_check(int (*func)(const char* hostname,
		ssh_session session))
{
	if (func != NULL) {
		callbacks.hostkey_check = func;
	} else {
		callbacks.hostkey_check = callback_ssh_hostkey_check_default;
	}
}

static char* callback_sshauth_password_default (const char* username,
		const char* hostname)
{
	char* buf, *newbuf;
	int buflen = 1024, len = 0;
	char c = 0;
	struct termios newterm, oldterm;
	FILE* tty;

	buf = malloc (buflen * sizeof(char));
	if (buf == NULL) {
		ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
		return (NULL);
	}

	if ((tty = fopen("/dev/tty", "r+")) == NULL) {
		ERROR("Unable to open the current terminal (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
		return (NULL);
	}

	if (tcgetattr(fileno(tty), &oldterm) != 0) {
		ERROR("Unable to get terminal settings (%d: %s).", __LINE__, strerror(errno));
		return (NULL);
	}

	fprintf(tty, "%s@%s password: ", username, hostname);
	fflush(tty);

	/* system("stty -echo"); */
	newterm = oldterm;
	newterm.c_lflag &= ~ECHO;
	newterm.c_lflag &= ~ICANON;
	tcflush(fileno(tty), TCIFLUSH);
	if (tcsetattr(fileno(tty), TCSANOW, &newterm) != 0) {
		ERROR("Unable to change terminal settings for hiding password (%d: %s).", __LINE__, strerror(errno));
		return (NULL);
	}

	while (fread(&c, 1, 1, tty) == 1 && c != '\n') {
		if (len >= (buflen-1)) {
			buflen *= 2;
			newbuf = realloc(buf, buflen*sizeof (char));
			if (newbuf == NULL) {
				ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));

				/* remove content of the buffer */
				memset(buf, 0, len);
				free(buf);

				/* restore terminal settings */
				if (tcsetattr(fileno(tty), TCSANOW, &oldterm) != 0) {
					ERROR("Unable to restore terminal settings (%d: %s).", __LINE__, strerror(errno));
				}
				return (NULL);
			} else {
				buf = newbuf;
			}
		}
		buf[len++] = c;
	}
	buf[len++] = 0; /* terminating null byte */

	/* system ("stty echo"); */
	if (tcsetattr(fileno(tty), TCSANOW, &oldterm) != 0) {
		ERROR("Unable to restore terminal settings (%d: %s).", __LINE__, strerror(errno));
		/*
		 * terminal probably still hides input characters, but we have password
		 * and anyway we are unable to set terminal to the previous state, so
		 * just continue
		 */
	}
	fprintf(tty, "\n");

	fclose(tty);
	return (buf);
}

/**
 * @brief Default callback for the \"keyboard-interactive\" authentication method
 */
static char* callback_sshauth_interactive_default (const char* UNUSED(name),
		const char* UNUSED(instruction),
		const char* prompt,
		int echo)
{
	unsigned int buflen = 8, response_len;
	char c = 0;
	struct termios newterm, oldterm;
	char* newtext, *response;
	FILE* tty;

	if ((tty = fopen("/dev/tty", "r+")) == NULL) {
		ERROR("Unable to open the current terminal (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
		return NULL;
	}

	if (tcgetattr(fileno(tty), &oldterm) != 0) {
		ERROR("Unable to get terminal settings (%d: %s).", __LINE__, strerror(errno));
		return NULL;
	}

	if (fwrite (prompt, sizeof(char), strlen(prompt), tty) == 0) {
		ERROR("Writing the authentication prompt into stdout failed.");
		return NULL;
	}
	fflush(tty);
	if (!echo) {
		/* system("stty -echo"); */
		newterm = oldterm;
		newterm.c_lflag &= ~ECHO;
		tcflush(fileno(tty), TCIFLUSH);
		if (tcsetattr(fileno(tty), TCSANOW, &newterm) != 0) {
			ERROR("Unable to change terminal settings for hiding password (%d: %s).", __LINE__, strerror(errno));
			return NULL;
		}
	}

	response = malloc (buflen*sizeof(char));
	response_len = 0;
	if (response == NULL) {
		ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
		/* restore terminal settings */
		if (tcsetattr(fileno(tty), TCSANOW, &oldterm) != 0) {
			ERROR("Unable to restore terminal settings (%d: %s).", __LINE__, strerror(errno));
		}
		return NULL;
	}

	while (fread(&c, 1, 1, tty) == 1 && c != '\n') {
		if (response_len >= (buflen-1)) {
			buflen *= 2;
			newtext = realloc(response, buflen*sizeof (char));
			if (newtext == NULL) {
				ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				free(response);

				/* restore terminal settings */
				if (tcsetattr(fileno(tty), TCSANOW, &oldterm) != 0) {
					ERROR("Unable to restore terminal settings (%d: %s).", __LINE__, strerror(errno));
				}
				return NULL;
			} else {
				response = newtext;
			}
		}
		response[response_len++] = c;
	}
	/* terminating null byte */
	response[response_len++] = '\0';

	/* system ("stty echo"); */
	if (tcsetattr(fileno(tty), TCSANOW, &oldterm) != 0) {
		ERROR("Unable to restore terminal settings (%d: %s).", __LINE__, strerror(errno));
		/*
		 * terminal probably still hides input characters, but we have password
		 * and anyway we are unable to set terminal to the previous state, so
		 * just continue
		 */
	}

	fprintf(tty, "\n");
	fclose(tty);
	return response;
}

static char* callback_sshauth_publickey_default (const char*  UNUSED(username),
		const char*  UNUSED(hostname),
		const char* privatekey_filepath)
{
	char c, *buf, *newbuf;
	int buflen = 1024, len = 0;
	struct termios newterm, oldterm;
	FILE* tty;

	buf = malloc (buflen * sizeof(char));
	if (buf == NULL) {
		ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
		return (NULL);
	}

	if ((tty = fopen("/dev/tty", "r+")) == NULL) {
		ERROR("Unable to open the current terminal (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
		return (NULL);
	}

	if (tcgetattr(fileno(tty), &oldterm) != 0) {
		ERROR("Unable to get terminal settings (%d: %s).", __LINE__, strerror(errno));
		return (NULL);
	}

	fprintf(tty, "Enter passphrase for the key '%s':", privatekey_filepath);
	fflush(tty);

	/* system("stty -echo"); */
	newterm = oldterm;
	newterm.c_lflag &= ~ECHO;
	newterm.c_lflag &= ~ICANON;
	tcflush(fileno(tty), TCIFLUSH);
	if (tcsetattr(fileno(tty), TCSANOW, &newterm) != 0) {
		ERROR("Unable to change terminal settings for hiding password (%d: %s).", __LINE__, strerror(errno));
		return (NULL);
	}

	while (fread(&c, 1, 1, tty) == 1 && c != '\n') {
		if (len >= (buflen-1)) {
			buflen *= 2;
			newbuf = realloc (buf, buflen*sizeof (char));
			if (newbuf == NULL) {
				ERROR("Memory allocation failed (%s:%d - %s).", __FILE__, __LINE__, strerror(errno));
				/* remove content of the buffer */
				memset(buf, 0, len);
				free(buf);

				/* restore terminal settings */
				if (tcsetattr(fileno(tty), TCSANOW, &oldterm) != 0) {
					ERROR("Unable to restore terminal settings (%d: %s).", __LINE__, strerror(errno));
				}

				return (NULL);
			}
			buf = newbuf;
		}
		buf[len++] = (char)c;
	}
	buf[len++] = 0; /* terminating null byte */

	/* system ("stty echo"); */
	if (tcsetattr(fileno(tty), TCSANOW, &oldterm) != 0) {
		ERROR("Unable to restore terminal settings (%d: %s).", __LINE__, strerror(errno));
		/*
		 * terminal probably still hides input characters, but we have password
		 * and anyway we are unable to set terminal to the previous state, so
		 * just continue
		 */
	}
	fprintf(tty, "\n");

	fclose(tty);
	return (buf);
}

#ifdef ENABLE_DNSSEC

/* return 0 (DNSSEC + key valid), 1 (unsecure DNS + key valid), 2 (key not found or an error) */
/* type - 1 (RSA), 2 (DSA), 3 (ECDSA); alg - 1 (SHA1), 2 (SHA-256) */
static int callback_ssh_hostkey_hash_dnssec_check(const char* hostname, const char* sha1hash, int type, int alg) {
	ns_msg handle;
	ns_rr rr;
	val_status_t val_status;
	const unsigned char* rdata;
	unsigned char buf[4096];
	int buf_len = 4096;
	int ret = 0, i, j, len;

	/* class 1 - internet, type 44 - SSHFP */
	len = val_res_query(NULL, hostname, 1, 44, buf, buf_len, &val_status);

	if (len < 0 || !val_istrusted(val_status)) {
		ret = 2;
		goto finish;
	}

	if (ns_initparse(buf, len, &handle) < 0) {
		ERROR("Failed to initialize DNSSEC response parser.");
		ret = 2;
		goto finish;
	}

	if ((i = libsres_msg_getflag(handle, ns_f_rcode)) != 0) {
		ERROR("DNSSEC query returned %d.", i);
		ret = 2;
		goto finish;
	}

	if (!libsres_msg_getflag(handle, ns_f_ad)) {
		/* response not secured by DNSSEC */
		ret = 1;
	}

	/* query section */
	if (ns_parserr(&handle, ns_s_qd, 0, &rr)) {
		ERROR("DNSSEC query section parser fail.");
		ret = 2;
		goto finish;
	}

	if (strcmp(hostname, ns_rr_name(rr)) != 0 || ns_rr_type(rr) != 44 || ns_rr_class(rr) != 1) {
		ERROR("DNSSEC query in the answer does not match the original query.");
		ret = 2;
		goto finish;
	}

	/* answer section */
	i = 0;
	while (ns_parserr(&handle, ns_s_an, i, &rr) == 0) {
		if (ns_rr_type(rr) != 44) {
			++i;
			continue;
		}

		rdata = ns_rr_rdata(rr);
		if (rdata[0] != type) {
			++i;
			continue;
		}
		if (rdata[1] != alg) {
			++i;
			continue;
		}

		/* we found the correct SSHFP entry */
		rdata += 2;
		for (j = 0; j < 20; ++j) {
			if (rdata[j] != (unsigned char)sha1hash[j]) {
				ret = 2;
				goto finish;
			}
		}

		/* server fingerprint is supported by a DNS entry,
		 * we have already determined if DNSSEC was used or not
		 */
		goto finish;
	}

	/* no match */
	ret = 2;
finish:
	val_free_validator_state();
	return ret;
}

#endif

static int callback_ssh_hostkey_check_default (const char* hostname, ssh_session session)
{
	char* hexa;
	int c, state, ret;
	ssh_key srv_pubkey;
	unsigned char *hash_sha1 = NULL;
	size_t hlen;
	enum ssh_keytypes_e srv_pubkey_type;
	char answer[5];

	state = ssh_is_server_known(session);

	ret = ssh_get_publickey(session, &srv_pubkey);
	if (ret < 0) {
		ERROR("Unable to get server public key.");
		return EXIT_FAILURE;
	}

	srv_pubkey_type = ssh_key_type(srv_pubkey);
	ret = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash_sha1, &hlen);
	ssh_key_free(srv_pubkey);
	if (ret < 0) {
		ERROR("Failed to calculate SHA1 hash of the server public key.");
		return EXIT_FAILURE;
	}

	hexa = ssh_get_hexa(hash_sha1, hlen);

	switch (state) {
	case SSH_SERVER_KNOWN_OK:
		break; /* ok */

	case SSH_SERVER_KNOWN_CHANGED:
		ERROR("Remote host key changed, the connection will be terminated!");
		goto fail;

	case SSH_SERVER_FOUND_OTHER:
		ERROR("The remote host key was not found but another type of key was, the connection will be terminated.");
		goto fail;

	case SSH_SERVER_FILE_NOT_FOUND:
		WARN("Could not find the known hosts file.");
		/* fallback to SSH_SERVER_NOT_KNOWN behavior */

	case SSH_SERVER_NOT_KNOWN:
#ifdef ENABLE_DNSSEC
		if (srv_pubkey_type != SSH_KEYTYPE_UNKNOWN || srv_pubkey_type != SSH_KEYTYPE_RSA1) {
			if (srv_pubkey_type == SSH_KEYTYPE_DSS) {
				ret = callback_ssh_hostkey_hash_dnssec_check(hostname, hash_sha1, 2, 1);
			} else if (srv_pubkey_type == SSH_KEYTYPE_RSA) {
				ret = callback_ssh_hostkey_hash_dnssec_check(hostname, hash_sha1, 1, 1);
			} else if (srv_pubkey_type == SSH_KEYTYPE_ECDSA) {
				ret = callback_ssh_hostkey_hash_dnssec_check(hostname, hash_sha1, 3, 1);
			}

			/* DNSSEC SSHFP check successful, that's enough */
			if (ret == 0) {
				DBG("DNSSEC SSHFP check successful");
				ssh_write_knownhost(session);
				ssh_clean_pubkey_hash(&hash_sha1);
				ssh_string_free_char(hexa);
				return EXIT_SUCCESS;
			}
		}
#endif

		/* try to get result from user */
		fprintf(stdout, "The authenticity of the host \'%s\' cannot be established.\n", hostname);
		fprintf(stdout, "%s key fingerprint is %s.\n", ssh_key_type_to_char(srv_pubkey_type), hexa);

#ifdef ENABLE_DNSSEC
		if (ret == 2) {
			fprintf(stdout, "No matching host key fingerprint found in DNS.\n");
		} else if (ret == 1) {
			fprintf(stdout, "Matching host key fingerprint found in DNS.\n");
		}
#endif

		fprintf(stdout, "Are you sure you want to continue connecting (yes/no)? ");

		do {
			if (fscanf(stdin, "%4s", answer) == EOF) {
				ERROR("fscanf() failed (%s).", strerror(errno));
				goto fail;
			}
			while ((c = getchar()) != EOF && c != '\n');

			fflush(stdin);
			if (strcmp("yes", answer) == 0) {
				/* store the key into the host file */
				ret = ssh_write_knownhost(session);
				if (ret < 0) {
					WARN("Adding the known host %s failed (%s).", hostname, strerror(errno));
				}
			} else if (strcmp("no", answer) == 0) {
				goto fail;
			} else {
				fprintf(stdout, "Please type 'yes' or 'no': ");
			}
		} while (strcmp(answer, "yes") != 0 && strcmp(answer, "no") != 0);

		break;

	case SSH_SERVER_ERROR:
		ssh_clean_pubkey_hash(&hash_sha1);
		fprintf(stderr,"%s",ssh_get_error(session));
		return -1;
	}

	ssh_clean_pubkey_hash(&hash_sha1);
	ssh_string_free_char(hexa);
	return EXIT_SUCCESS;

fail:
	ssh_clean_pubkey_hash(&hash_sha1);
	ssh_string_free_char(hexa);
	return EXIT_FAILURE;
}

/* op = 1 (add), 2 (remove) */
static int nc_publickey_path (const char* path, int op)
{
	int i;

	if (path == NULL) {
		return EXIT_FAILURE;
	}

	for (i = 0; i < SSH_KEYS; ++i) {
		if (op == 1 && callbacks.publickey_filename[i] == NULL) {
			callbacks.publickey_filename[i] = strdup(path);
			break;
		}
		if (op == 2 && callbacks.publickey_filename[i] != NULL && strcmp(callbacks.publickey_filename[i], path) == 0) {
			free(callbacks.publickey_filename[i]);
			callbacks.publickey_filename[i] = NULL;
			break;
		}
	}

	if (i == SSH_KEYS) {
		if (op == 1) {
			ERROR("Too many SSH public keys.");
			return EXIT_FAILURE;
		}
		if (op == 2) {
			ERROR("The SSH public key to delete was not found.");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/* op = 1 (add), 2 (remove) */
static int nc_privatekey_path (const char* path, int op)
{
	FILE* key;
	char line[128];
	int i;

	if (path == NULL) {
		return EXIT_FAILURE;
	}

	for (i = 0; i < SSH_KEYS; ++i) {
		if (op == 1 && callbacks.privatekey_filename[i] == NULL) {
			callbacks.privatekey_filename[i] = strdup(path);
			break;
		}
		if (op == 2 && callbacks.privatekey_filename[i] != NULL && strcmp(callbacks.privatekey_filename[i], path) == 0) {
			free(callbacks.privatekey_filename[i]);
			callbacks.privatekey_filename[i] = NULL;
			callbacks.key_protected[i] = 0;
			break;
		}
	}

	if (i == SSH_KEYS) {
		if (op == 1) {
			ERROR("Too many SSH private keys.");
			return EXIT_FAILURE;
		}
		if (op == 2) {
			ERROR("The SSH private key to delete was not found.");
			return EXIT_FAILURE;
		}
	}

	if (op == 1) {
		if ((key = fopen (path, "r")) != NULL) {
			/* Key type line */
			if (fgets(line, sizeof(line), key) == NULL) {
				ERROR("fgets() on %s failed.", path);
				return EXIT_FAILURE;
			}
			/* encryption information or key */
			if (fgets(line, sizeof(line), key) == NULL) {
				ERROR("fgets() on %s failed.", path);
				return EXIT_FAILURE;
			}
			if (strcasestr (line, "encrypted") != NULL) {
				callbacks.key_protected[i] = 1;
			}
		}
	}

	return EXIT_SUCCESS;
}

API int nc_set_keypair_path(const char* privkey, const char* pubkey)
{
	if (nc_privatekey_path(privkey, 1) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	if (nc_publickey_path(pubkey, 1) != EXIT_SUCCESS) {
		nc_privatekey_path(privkey, 2);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

API int nc_del_keypair_path(const char* privkey, const char* pubkey)
{
	if (nc_privatekey_path(privkey, 2) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}
	if (nc_publickey_path(pubkey, 2) != EXIT_SUCCESS) {
		nc_privatekey_path(privkey, 1);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif /* not DISABLE_LIBSSH */
