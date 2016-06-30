/**
 * \file url.c
 * \author Ondrej Vlk <ondrasek.vlk@gmail.com>
 * \brief libnetconf's implementation of the URL capability.
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
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include "url_internal.h"
#include "netconf_internal.h" // for nc_session structure

/* define init flags */
#ifdef CURL_GLOBAL_ACK_EINTR
#define INIT_FLAGS CURL_GLOBAL_SSL|CURL_GLOBAL_ACK_EINTR
#else
#define INIT_FLAGS CURL_GLOBAL_SSL
#endif

int url_tmpfile;

/* Struct for uploading data with curl */
struct nc_url_mem
{
	char *memory;
	size_t size;
};

// default allowed protocols
int nc_url_protocols = NC_URL_FILE | NC_URL_SCP;

API void nc_url_set_protocols(int protocols)
{
	nc_url_protocols = protocols;
}

API void nc_url_enable(NC_URL_PROTOCOLS protocol)
{
	nc_url_protocols = nc_url_protocols | protocol;
}

API void nc_url_disable(NC_URL_PROTOCOLS protocol)
{
	nc_url_protocols = (~protocol) & nc_url_protocols;
}

int nc_url_is_enabled(NC_URL_PROTOCOLS protocol)
{
	return nc_url_protocols & protocol;
}

static xmlChar* url_protocols[] = {
		BAD_CAST "scp",
		BAD_CAST "http",
		BAD_CAST "https",
		BAD_CAST "ftp",
		BAD_CAST "sftp",
		BAD_CAST "ftps",
		BAD_CAST "file"
};

/**< @brief generates url capability string with enabled protocols */
char* nc_url_gencap(void)
{
	char *cpblt = NULL, *cpblt_update = NULL;
	int first = 1;
	int i;
	int protocol = 1;

	if (nc_url_protocols == 0) {
		return (NULL);
	}

	if (asprintf(&cpblt, NC_CAP_URL_ID "?scheme=") < 0) {
		ERROR("%s: asprintf error (%s:%d)", __func__, __FILE__, __LINE__);
		return (NULL);
	}

	for (i = 0, protocol = 1; (unsigned int) i < (sizeof(url_protocols) / sizeof(url_protocols[0])); i++, protocol <<= 1) {
		if (protocol & nc_url_protocols) {
			if (asprintf(&cpblt_update, "%s%s%s", cpblt, first ? "" : ",", url_protocols[i]) < 0) {
				ERROR("%s: asprintf error (%s:%d)", __func__, __FILE__, __LINE__);
			}
			free(cpblt);
			cpblt = cpblt_update;
			cpblt_update = NULL;
			first = 0;
		}
	}

	return (cpblt);
}

/**< @brief gets protocol id from url*/
NC_URL_PROTOCOLS nc_url_get_protocol(const char *url)
{
	int protocol = 1; /* not a SCP, just an init value for bit shift */
	int protocol_set = 0;
	int i;
	char *url_aux = strdup(url);
	char *c;

	c = strchr(url_aux, ':');
	if (c == NULL) {
		free(url_aux);
		ERROR("%s: invalid URL string, missing protocol specification", __func__);
		return (NC_URL_UNKNOWN);
	}

	for (i = 0; (unsigned int) i < (sizeof(url_protocols) / sizeof(url_protocols[0])); i++, protocol <<= 1) {
		if (xmlStrncmp(BAD_CAST url_aux, url_protocols[i], xmlStrlen(url_protocols[i])) == 0) {
			protocol_set = 1;
			break;
		}
	}
	free(url_aux);

	if (protocol_set) {
		return (protocol);
	} else {
		return (NC_URL_UNKNOWN);
	}
}

static size_t nc_url_readdata(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t copied = 0;
	size_t aux_size = size * nmemb;
	struct nc_url_mem *data = (struct nc_url_mem *) userdata;

	if (aux_size < 1 || data->size == 0) {
		/* no space or nothing lefts */
		return 0;
	}

	copied = (data->size > aux_size) ? aux_size : data->size;
	memcpy(ptr, data->memory, copied);
	data->memory = data->memory + copied; /* move pointer */
	data->size = data->size - copied; /* decrease amount of data left */
	return (copied);
}

int nc_url_upload(char *data, const char *url, struct nc_err **e)
{
	CURL * curl;
	CURLcode res;
	struct nc_url_mem mem_data;
	char curl_buffer[CURL_ERROR_SIZE];
	xmlDocPtr doc;
	xmlNodePtr root_element;
	int retval = EXIT_SUCCESS;

	assert(e);

	if (strcmp(data, "") == 0) {
		ERROR("%s: source file is empty", __func__);
		*e = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*e, NC_ERR_PARAM_MSG, "Data to be stored at URL are empty.");
		return EXIT_FAILURE;
	}

	/* check that the content follows RFC */
	doc = xmlParseMemory(data, strlen(data));
	root_element = xmlDocGetRootElement(doc);
	if (strcmp((char *) root_element->name, "config") != 0) {
		ERROR("%s: source file does not contain config element", __func__);
		*e = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*e, NC_ERR_PARAM_MSG, "Data to be stored at URL are invalid.");
		return EXIT_FAILURE;
	}
	xmlFreeDoc(doc);

	DBG("Uploading file to URL: %s (via curl)", url);

	/* fill the structure for libcurl's READFUNCTION */
	mem_data.memory = data;
	mem_data.size = strlen(data);

	/* set up libcurl */
	curl_global_init(INIT_FLAGS);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(curl, CURLOPT_READDATA, &mem_data);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, nc_url_readdata);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_buffer);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		ERROR("%s: curl error: %s", __func__, curl_buffer);
		*e = nc_err_new(NC_ERR_OP_FAILED);
		nc_err_set(*e, NC_ERR_PARAM_MSG, curl_buffer);
		retval = EXIT_FAILURE;
	}

	/* cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return retval;
}

static size_t nc_url_writedata(char *ptr, size_t size, size_t nmemb, void* UNUSED(userdata))
{
	return write(url_tmpfile, ptr, size * nmemb);
}

int nc_url_delete_config(const char *url, struct nc_err **e)
{
	return nc_url_upload("<?xml version=\"1.0\"?><config xmlns=\""NC_NS_BASE10"\"></config>", url, e);
}

int nc_url_check(const char* url)
{
	CURL *curl;
	int retval;

	/* set up libcurl */
	curl_global_init(INIT_FLAGS);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	retval = curl_easy_perform(curl);

	/* cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return (retval);
}

int nc_url_open(const char *url)
{
	CURL * curl;
	CURLcode res;
	char curl_buffer[CURL_ERROR_SIZE];
	char url_tmp_name[(sizeof(NC_WORKINGDIR_PATH) / sizeof(char)) + 19] = NC_WORKINGDIR_PATH"/url_tmpfileXXXXXX";

	/* prepare temporary file ... */
	if ((url_tmpfile = mkstemp(url_tmp_name)) < 0) {
		ERROR("%s: cannot create temporary file (%s)", __func__, strerror(errno));
		return (-1);
	}
	/* and hide it from the file system */
	unlink(url_tmp_name);

	DBG("Getting file from URL: %s (via curl)", url);

	/* set up libcurl */
	curl_global_init(INIT_FLAGS);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, nc_url_writedata);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_buffer);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		ERROR("%s: curl error: %s", __func__, curl_buffer);
		close(url_tmpfile);
		url_tmpfile = -1;
	} else {
		/* move back to the beginning of the output file */
		lseek(url_tmpfile, 0, SEEK_SET);
	}

	/* cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return url_tmpfile;
}
