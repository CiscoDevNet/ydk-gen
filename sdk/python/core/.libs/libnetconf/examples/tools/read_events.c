#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libnetconf.h>
#include "../../src/netconf_internal.h"

#include <libxml/tree.h>
#include <libxml/parser.h>

#define ARGUMENTS "hls:e:v:"

void clb_print(NC_VERB_LEVEL level, const char* msg)
{
	switch (level) {
	case NC_VERB_ERROR:
		fprintf(stderr, "libnetconf ERROR: %s\n", msg);
		break;
	case NC_VERB_WARNING:
		fprintf(stderr, "libnetconf WARNING: %s\n", msg);
		break;
	case NC_VERB_VERBOSE:
		fprintf(stderr, "libnetconf VERBOSE: %s\n", msg);
		break;
	case NC_VERB_DEBUG:
		fprintf(stderr, "libnetconf DEBUG: %s\n", msg);
		break;
	}
}

void usage(char* progname)
{
	printf("Usage: %s [-hl] [-s time] [-e time] [-v level] stream\n", progname);
	printf("-h         Show this help\n");
	printf("-l         List available streams\n");
	printf("-s time    Start time of the events time range\n");
	printf("-e time    End time of the events time range\n");
	printf("-v level   Set verbose level (0-3)\n\n");
	printf("Note: time is accepted in a form printed by the -l option.\n\n");
}

int main(int argc, char* argv[])
{
	char* event, *stream, **list, *desc, *start;
	int i, c;
	int verbosity = NC_VERB_ERROR;
	int listing = 0;
	int corrupted = 0;
	int time_start = 0, time_end = -1;
	xmlDocPtr eventDoc;

	while ((c = getopt(argc, argv, ARGUMENTS)) != -1) {
		switch (c) {
		case 'h': /* Show help */
			usage(argv[0]);
			return EXIT_SUCCESS;

		case 'l': /* list the streams */
			listing = 1;
			break;

		case 's': /* time range - start */
			time_start = nc_datetime2time(optarg);
			break;

		case 'e': /* time range - end */
			time_end = nc_datetime2time(optarg);
			break;

		case 'v': /* Verbose operation */
			verbosity = atoi(optarg);
			if (verbosity < NC_VERB_ERROR) {
				verbosity = NC_VERB_ERROR;
			} else if (verbosity > NC_VERB_DEBUG) {
				verbosity = NC_VERB_DEBUG;
			}
			break;

		default:
			fprintf(stderr, "unknown argument -%c", optopt);
			break;
		}
	}

	if (!listing && (argc - optind) != 1) {
		if ((argc - optind) < 1) {
			fprintf(stderr, "Missing stream name\n\n");
		} else { /* (argc - optind) > 1 */
			fprintf(stderr, "Only a single stream name is allowed\n\n");
		}
		usage(argv[0]);
		return (EXIT_FAILURE);
	}
	stream = argv[optind];

	nc_verbosity(verbosity);
	nc_callback_print(clb_print);

	c = nc_init(NC_INIT_NOTIF);
	if (c == -1) {
		fprintf(stderr, "libnetconf initiation failed.");
		return (EXIT_FAILURE);
	}

	if (listing) {
		list = ncntf_stream_list();
		if (list == NULL || list[0] == NULL) {
			fprintf(stderr, "There is no NETCONF Event Stream.\n");
			return (EXIT_FAILURE);
		}
		fprintf(stdout, "NETCONF Event Stream list:\n");
		for (i = 0; list[i] != NULL; i++) {
			if (ncntf_stream_info(list[i], &desc, &start) == 0) {
				fprintf(stdout, "\t%s\n\t\t%s\n\t\t%s\n", list[i], desc, start);
				free(desc);
				free(start);
			}
			free(list[i]);
		}
		fprintf(stdout, "\n");
		free(list);

		return(EXIT_SUCCESS);
	}

	i = 0;
	ncntf_stream_iter_start(stream);
	while((event = ncntf_stream_iter_next(stream, time_start, time_end, NULL)) != NULL) {
		if ((eventDoc = xmlReadMemory(event, strlen(event), NULL, NULL, 0)) != NULL) {
			fprintf(stdout, "Event:\n");
			xmlDocFormatDump(stdout, eventDoc, 1);
			xmlFreeDoc(eventDoc);
			i++;
		} else {
			fprintf(stdout, "Invalid event format.\n");
			corrupted = 1;
		}
		free(event);
	}
	ncntf_stream_iter_finish(stream);

	/* print summary */
	fprintf(stdout, "\nSummary:\n\tNumber of records: %d\n", i);
	if (corrupted) {
		fprintf(stdout, "\tSTREAM FILE IS CORRUPTED!\n");
	}

	ncntf_close();

	return (EXIT_SUCCESS);
}
