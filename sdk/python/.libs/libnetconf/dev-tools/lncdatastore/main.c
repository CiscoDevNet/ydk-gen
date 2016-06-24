#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "netconf.h"
#include "callbacks.h"

#include "commands.h"
#include "mreadline.h"

volatile int done;

void mprint(NC_VERB_LEVEL level, const char* msg) {
	switch (level) {
	case NC_VERB_ERROR:
		fprintf(stderr, "ERROR: %s\n", msg);
		break;
	case NC_VERB_WARNING:
		fprintf(stderr, "WARNING: %s\n", msg);
		break;
	case NC_VERB_VERBOSE:
		fprintf(stderr, "VERBOSE: %s\n", msg);
		break;
	case NC_VERB_DEBUG:
		fprintf(stderr, "DEBUG: %s\n", msg);
		break;
	}
}

int main(int UNUSED(argc), char** UNUSED(argv)) {
	char* cmd, *cmdline, *cmdstart;
	int i, j;

	initialize_readline();

	nc_verbosity(NC_VERB_VERBOSE);
	nc_callback_print(mprint);

	while (!done) {
		/* get the command from user */
		cmdline = readline(PROMPT);

		/* EOF -> exit */
		if (cmdline == NULL) {
			done = 1;
			cmdline = strdup("quit");
		}

		/* empty line -> wait for another command */
		if (*cmdline == '\0') {
			free(cmdline);
			continue;
		}

		/* Isolate the command word. */
		for (i = 0; cmdline[i] && whitespace(cmdline[i]); i++);
		cmdstart = cmdline + i;
		for (j = 0; cmdline[i] && !whitespace(cmdline[i]); i++, j++);
		cmd = strndup(cmdstart, j);

		/* parse the command line */
		for (i = 0; commands[i].name; i++) {
			if (strcmp(cmd, commands[i].name) == 0) {
				break;
			}
		}

		/* execute the command if any valid specified */
		if (commands[i].name) {
			/* display help */
			if (strchr(cmdstart, ' ') != NULL && (strncmp(strchr(cmdstart, ' ')+1, "-h", 2) == 0 ||
					strncmp(strchr(cmdstart, ' ')+1, "--help", 6) == 0)) {
				if (commands[i].help_func != NULL) {
					commands[i].help_func();
				} else {
					printf("%s\n", commands[i].helpstring);
				}
			} else {
				commands[i].func((const char*)cmdstart);
			}
		} else {
			/* if unknown command specified, tell it to user */
			printf("%s: no such command, type 'help' for more information.\n", cmd);
		}
		add_history(cmdline);

		free(cmd);
		free(cmdline);
	}

	return 0;
}