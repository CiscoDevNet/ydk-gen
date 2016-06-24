/*
 * commands.c
 * Author Michal Vasko <mvasko@cesnet.cz>
 *
 * Implementation of dev-datastore commands.
 *
 * Copyright (C) 2014 CESNET, z.s.p.o.
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
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libxml/parser.h>

#include "commands.h"
#include "mreadline.h"
#include "libnetconf.h"

#include "datastore.h"
#include "datastore.c"

struct model_hint* model_hints = NULL, *model_hints_end = NULL;

extern int done;

void add_hint(const char* name) {
	if (model_hints == NULL) {
		model_hints = malloc(sizeof(struct model_hint));
		model_hints_end = model_hints;
		model_hints_end->hint = strdup(name);
		model_hints_end->next = NULL;
	} else {
		model_hints_end->next = malloc(sizeof(struct model_hint));
		model_hints_end->next->hint = strdup(name);
		model_hints_end->next->next = NULL;
		model_hints_end = model_hints_end->next;
	}
}

void remove_hint(const char* name) {
	struct model_hint* prev, *cur;

	if (model_hints == NULL) {
		return;
	}

	if (strcmp(model_hints->hint, name) == 0) {
		if (model_hints == model_hints_end) {
			free(model_hints->hint);
			free(model_hints);
			model_hints = NULL;
			model_hints_end = NULL;
		} else {
			prev = model_hints;
			model_hints = model_hints->next;
			free(prev->hint);
			free(prev);
		}
		return;
	}

	prev = model_hints;
	cur = model_hints->next;
	while (cur != NULL) {
		if (strcmp(cur->hint, name) == 0) {
			if (cur == model_hints_end) {
				model_hints_end = prev;
			}
			prev->next = cur->next;
			free(cur->hint);
			free(cur);
			return;
		}
		prev = cur;
		cur = cur->next;
	}
}

void remove_all_hints(void) {
	struct model_hint* prev, *cur;

	if (model_hints == NULL) {
		return;
	}

	cur = model_hints;
	while (cur != NULL) {
		prev = cur;
		cur = cur->next;
		free(prev->hint);
		free(prev);
	}

	model_hints = NULL;
	model_hints_end = NULL;
}

struct ncds_ds_list* find_datastore(const char* name) {
	struct ncds_ds_list* item;

	if (name == NULL) {
		return NULL;
	}

	item = ncds.datastores;

	while (item != NULL) {
		if (strcmp(item->datastore->data_model->name, name) == 0) {
			return item;
		}
		item = item->next;
	}

	return NULL;
}

struct model_list* find_model(const char* name) {
	struct model_list* item;

	if (name == NULL) {
		return NULL;
	}

	item = models_list;

	while (item != NULL) {
		if (strcmp(item->model->name, name) == 0) {
			return item;
		}
		item = item->next;
	}

	return NULL;
}

void cmd_add_datastore_help(void) {
	printf("add-datastore path-to-main-model [ (* | features-to-turn-on ...) ]\n");
}

void cmd_add_model_help(void) {
	printf("add-model path-to-model [ (* | features-to-turn-on ...) ]\n");
}

void cmd_remove_help(void) {
	printf("remove (datastore-name | model-name)\n");
}

void cmd_print_help(void) {
	printf("print [ (datastore-name | model-name) [<output-file>] ]\n");
}

void cmd_feature_help(void) {
	printf("feature (datastore-name | model-name) [ (* | feature-names ...) (on | off) ]\n");
}

void cmd_verb_help(void) {
	printf("verb (error | warning | verbose | debug)\n");
}

int cmd_add_datastore(const char* arg) {
	char* argv, *ptr, *ptr2;
	struct ncds_ds* new_ds;

	argv = strdupa(arg + strlen("add-datastore "));

	if ((ptr = strtok(argv, " ")) == NULL) {
		cmd_add_datastore_help();
		return 1;
	}

	/* remove .yin if specified */
	ptr2 = strstr(ptr, ".yin");
	if (ptr2 != NULL) {
		*ptr2 = '\0';
	}

	if ((new_ds = ncds_new_internal(NCDS_TYPE_EMPTY, ptr)) == NULL) {
		return 1;
	}

	ncds_init(new_ds);

	add_hint(new_ds->data_model->name);

	ptr = strtok(NULL, " ");
	if (ptr != NULL) {
		if (strcmp(ptr, "*") == 0) {
			ncds_features_enableall(new_ds->data_model->name);
			return 0;
		} else {
			ncds_feature_enable(new_ds->data_model->name, ptr);
		}
	}
	while ((ptr = strtok(NULL, " ")) != NULL) {
		ncds_feature_enable(new_ds->data_model->name, ptr);
	}

	return 0;
}

int cmd_add_model(const char* arg) {
	char* argv, *ptr, *ptr2, *model;
	struct data_model* mdl;

	argv = strdupa(arg + strlen("add-model "));

	if ((ptr = strtok(argv, " ")) == NULL) {
		cmd_add_model_help();
		return 1;
	}

	/* add .yin if not specified */
	ptr2 = strstr(ptr, ".yin");
	if (ptr2 == NULL) {
		asprintf(&ptr2, "%s.yin", ptr);
		model = ptr2;
	} else {
		ptr2 = NULL;
		model = ptr;
	}

	mdl = read_model(model);
	free(ptr2);
	if (mdl == NULL) {
		return 1;
	}

	add_hint(mdl->name);

	ptr = strtok(NULL, " ");
	if (ptr != NULL) {
		if (strcmp(ptr, "*") == 0) {
			ncds_features_enableall(mdl->name);
			return 0;
		} else {
			ncds_feature_enable(mdl->name, ptr);
		}
	}
	while ((ptr = strtok(NULL, " ")) != NULL) {
		ncds_feature_enable(mdl->name, ptr);
	}

	return 0;
}

int cmd_remove(const char* arg) {
	char* ptr, *argv;
	struct ncds_ds_list* ds;
	struct model_list* model;

	if (strlen(arg) < 7) {
		cmd_remove_help();
		return 1;
	}

	argv = strdupa(arg + strlen("remove "));

	ptr = strtok(argv, " ");

	ds = find_datastore(ptr);
	if (ds == NULL) {
		model = find_model(ptr);
		if (model == NULL) {
			nc_verb_error("No datastore or model \"%s\" found", ptr);
			return 1;
		} else {
			ncds_ds_model_free(model->model);
		}
	} else {
		ncds_free(ds->datastore);
	}

	remove_hint(ptr);
	return 0;
}

int cmd_print(const char* arg) {
	int fd;
	char* ptr, *argv;
	xmlDocPtr doc;
	struct ncds_ds_list* ds;
	struct model_list* model;

	argv = strdupa(arg);
	strtok(argv, " ");
	ptr = strtok(NULL, " ");

	if (ptr == NULL) {
		ds = ncds.datastores;
		model = models_list;

		printf("Datastores:\n");
		if (ds == NULL) {
			printf("\tnone\n");
		}
		for (; ds != NULL; ds = ds->next) {
			printf("\t%s\n", ds->datastore->data_model->name);
		}

		printf("Models:\n");
		if (model == NULL) {
			printf("\tnone\n");
		}
		for (; model != NULL; model = model->next) {
			printf("\t%s\n", model->model->name);
		}
	} else {
		char* buf = NULL;
		int buf_len = 0;

		ds = find_datastore(ptr);
		if (ds == NULL) {
			model = find_model(ptr);
			if (model == NULL) {
				nc_verb_error("No datastore or model \"%s\" found", ptr);
				return 1;
			} else {
				doc = model->model->xml;
			}
		} else {
			doc = ds->datastore->ext_model;
		}

		xmlDocDumpFormatMemory(doc, (xmlChar**)&buf, &buf_len, 1);
		ptr = strtok(NULL, " ");
		if (ptr == NULL) {
			fwrite(buf, 1, buf_len, stdout);
		} else {
			fd = creat(ptr, 00660);
			if (fd == -1) {
				nc_verb_error("Failed to open file \"%s\" (%s)", ptr, strerror(errno));
				free(buf);
				return 1;
			}
			if (write(fd, buf, buf_len) < buf_len) {
				nc_verb_error("Failed to write into file (%s)", strerror(errno));
				free(buf);
				close(fd);
				return 1;
			}
			close(fd);
		}
		free(buf);
	}

	return 0;
}

int cmd_consolidate(const char* UNUSED(arg)) {
	if (ncds.datastores == NULL) {
		nc_verb_warning("No datastores to consolidate");
		return 1;
	}
	return ncds_consolidate();
}

int cmd_feature(const char* arg) {
	char* argv, *ptr, *state_str, *saveptr;
	int i, ret = 0;
	struct model_list* list;
	struct data_model* model;

	if (strlen(arg) < 8) {
		cmd_feature_help();
		return 1;
	}

	argv = strdupa(arg + strlen("feature "));

	ptr = strtok_r(argv, " ", &saveptr);
	if (ptr == NULL) {
		cmd_feature_help();
		return 1;
	}

	list = find_model(ptr);
	if (list == NULL) {
		nc_verb_error("No model \"%s\" found", ptr);
		return 1;
	}
	model = list->model;

	ptr = strtok_r(NULL, " ", &saveptr);

	/* we are done, no more arguments */
	if (ptr == NULL) {
		printf("Features:\n");
		if (model->features == NULL) {
			printf("\tnone\n");
			return 0;
		}
		for (i = 0; model->features[i] != NULL; ++i) {
			printf("\t%s %s\n", model->features[i]->name, (model->features[i]->enabled ? "ON" : "OFF"));
		}
		return 0;
	}

	do {
		state_str = strtok_r(NULL, " ", &saveptr);
	} while (state_str != NULL && strcmp(state_str, "on") != 0 && strcmp(state_str, "off") != 0);
	/* there was no "yes" or "no" at the end */
	if (state_str == NULL) {
		cmd_feature_help();
		return 1;
	}

	if (model->features == NULL) {
		nc_verb_error("Model does not have any features");
		return 1;
	}

	/* all features */
	if (strcmp(ptr, "*") == 0) {
		for (i = 0; model->features[i] != NULL; ++i) {
			if (strcmp(state_str, "on") == 0) {
				model->features[i]->enabled = 1;
			} else {
				model->features[i]->enabled = 0;
			}
		}
	} else {
		/* one or more features */
		ptr = argv + strlen(argv)+1;
		while (ptr != state_str) {
			for (i = 0; model->features[i] != NULL; ++i) {
				if (strcmp(model->features[i]->name, ptr) == 0) {
					if ((model->features[i]->enabled && strcmp(state_str, "on") == 0) || (!model->features[i]->enabled && strcmp(state_str, "off") == 0)) {
						nc_verb_verbose("Feature \"%s\" is already %s", ptr, state_str);
					} else if (strcmp(state_str, "on") == 0) {
						model->features[i]->enabled = 1;
					} else {
						model->features[i]->enabled = 0;
					}
					break;
				}
			}

			if (model->features[i] == NULL) {
				nc_verb_error("Model does not have the feature \"%s\"", ptr);
				ret = 1;
			}

			ptr = ptr + strlen(ptr)+1;
		}
	}

	return ret;
}

int cmd_verb(const char* arg) {
	const char* verb;

	verb = arg + 5;
	if (strcmp(verb, "error") == 0) {
		nc_verbosity(NC_VERB_ERROR);
	} else if (strcmp(verb, "warning") == 0) {
		nc_verbosity(NC_VERB_WARNING);
	} else if (strcmp(verb, "verbose") == 0) {
		nc_verbosity(NC_VERB_VERBOSE);
	} else if (strcmp(verb, "debug") == 0) {
		nc_verbosity(NC_VERB_DEBUG);
	} else {
		nc_verb_error("Unknown verbosity \"%s\"", verb);
		return 1;
	}

	return 0;
}

int cmd_quit(const char* UNUSED(arg)) {
	done = 1;
	ncds_cleanall();
	remove_all_hints();
	return 0;
}

int cmd_help(const char* arg) {
	int i;
	char* args = strdupa(arg);
	char* cmd = NULL;

	strtok(args, " ");
	if ((cmd = strtok(NULL, " ")) == NULL) {

generic_help:
		fprintf(stdout, "Available commands:\n");

		for (i = 0; commands[i].name; i++) {
			if (commands[i].helpstring != NULL) {
				fprintf(stdout, "  %-15s %s\n", commands[i].name, commands[i].helpstring);
			}
		}
	} else {
		/* print specific help for the selected command */

		/* get the command of the specified name */
		for (i = 0; commands[i].name; i++) {
			if (strcmp(cmd, commands[i].name) == 0) {
				break;
			}
		}

		/* execute the command's help if any valid command specified */
		if (commands[i].name) {
			if (commands[i].help_func != NULL) {
				commands[i].help_func();
			} else {
				printf("%s\n", commands[i].helpstring);
			}
		} else {
			/* if unknown command specified, print the list of commands */
			printf("Unknown command \'%s\'\n", cmd);
			goto generic_help;
		}
	}

	return 0;
}

COMMAND commands[] = {
		{"help", cmd_help, NULL, "Display commands description"},
		{"add-datastore", cmd_add_datastore, cmd_add_datastore_help, "Add a new datastore"},
		{"add-model", cmd_add_model, cmd_add_model_help, "Add a new model"},
		{"remove", cmd_remove, cmd_remove_help, "Remove a datastore/model"},
		{"print", cmd_print, cmd_print_help, "Print datastore/model"},
		{"consolidate", cmd_consolidate, NULL, "Consolidate datastores"},
		{"feature", cmd_feature, cmd_feature_help, "Manage datastore/model features"},
		{"verb", cmd_verb, cmd_verb_help, "Change verbosity"},
		{"quit", cmd_quit, NULL, "Quit the program"},
/* synonyms for previous commands */
		{"?", cmd_help, NULL, "Display commands description"},
		{"exit", cmd_quit, NULL, "Quit the program"},
		{NULL, NULL, NULL, NULL}
};