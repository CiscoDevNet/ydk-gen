/**
 * \file netconf_internal.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief libnetconf's internal functions and structures definitions.
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

#ifndef NC_NETCONF_INTERNAL_H_
#define NC_NETCONF_INTERNAL_H_

#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#ifndef DISABLE_LIBSSH
#	include <libssh/libssh.h>
#	include <libssh/callbacks.h>
#endif

#ifdef ENABLE_TLS
#	include <openssl/bio.h>
#	include <openssl/ssl.h>
#endif

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "config.h"
#include "netconf.h"
#include "callbacks.h"
#include "with_defaults.h"

/* number of characters to store short number */
#define SHORT_INT_LENGTH 6

#define SID_SIZE 	16

#define UTF8 		"UTF-8"
#define XML_VERSION 	"1.0"

/**
 * @brief NETCONF v1.0 message separator.
 * @ingroup internalAPI
 */
#define NC_V10_END_MSG      "]]>]]>"

/**
 * @brief NETCONF v1.1 message separator (part of the chunked framing mechanism).
 * @ingroup internalAPI
 */
#define NC_V11_END_MSG      "\n##\n"

/**
 * @brief Default NETCONF port number assigned by IANA.
 * @ingroup internalAPI
 */
#define NC_PORT             830
#define NC_REVERSE_PORT     6666
#define NC_REVERSE_QUEUE    10

/* NETCONF namespaces */
#define NC_NS_BASE10		"urn:ietf:params:xml:ns:netconf:base:1.0"
#define NC_NS_BASE10_ID		"base10"
#define NC_NS_BASE11		"urn:ietf:params:xml:ns:netconf:base:1.1"
#define NC_NS_BASE11_ID		"base11"

#define NC_NS_BASE NC_NS_BASE10
#define NC_NS_BASE_ID NC_NS_BASE10_ID

#define NC_CAP_BASE10_ID        "urn:ietf:params:netconf:base:1.0"
#define NC_CAP_BASE11_ID        "urn:ietf:params:netconf:base:1.1"
#define NC_CAP_NOTIFICATION_ID  "urn:ietf:params:netconf:capability:notification:1.0"
#define NC_CAP_INTERLEAVE_ID    "urn:ietf:params:netconf:capability:interleave:1.0"
#define NC_CAP_WRUNNING_ID      "urn:ietf:params:netconf:capability:writable-running:1.0"
#define NC_CAP_CANDIDATE_ID     "urn:ietf:params:netconf:capability:candidate:1.0"
#define NC_CAP_STARTUP_ID       "urn:ietf:params:netconf:capability:startup:1.0"
#define NC_CAP_POWERCTL_ID      "urn:liberouter:params:netconf:capability:power-control:1.0"
#define NC_CAP_CONFIRMED_COMMIT_ID "urn:ietf:params:netconf:capability:confirmed-commit:1.1"
#define NC_CAP_ROLLBACK_ID      "urn:ietf:params:netconf:capability:rollback-on-error:1.0"
#define NC_CAP_VALIDATE10_ID    "urn:ietf:params:netconf:capability:validate:1.0"
#define NC_CAP_VALIDATE11_ID    "urn:ietf:params:netconf:capability:validate:1.1"
#define NC_CAP_MONITORING_ID    "urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring"
#define NC_CAP_WITHDEFAULTS_ID  "urn:ietf:params:netconf:capability:with-defaults:1.0"
#define NC_CAP_URL_ID           "urn:ietf:params:netconf:capability:url:1.0"

#define NC_NS_WITHDEFAULTS      "urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults"
#define NC_NS_WITHDEFAULTS_ID   "wd"
#define NC_NS_NOTIFICATIONS     "urn:ietf:params:xml:ns:netconf:notification:1.0"
#define NC_NS_NOTIFICATIONS_ID  "ntf"
#define NC_NS_MONITORING        "urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring"
#define NC_NS_MONITORING_ID     "monitor"
#define NC_NS_NACM              "urn:ietf:params:xml:ns:yang:ietf-netconf-acm"
#define NC_NS_NACM_ID           "nacm"
#define NC_NS_YANG              "urn:ietf:params:xml:ns:yang:1"
#define NC_NS_YANG_ID           "yang"
#define NC_NS_YIN               "urn:ietf:params:xml:ns:yang:yin:1"
#define NC_NS_YIN_ID            "yin"

#define NC_NS_LNC_NOTIFICATIONS "urn:cesnet:params:xml:ns:libnetconf:notifications"

/* NETCONF versions identificators */
#define NETCONFV10	0
#define NETCONFV11	1
#define NETCONFVUNK -1

/* RPC model elements */
#define NC_HELLO_MSG        "hello"
#define NC_RPC_MSG          "rpc"
#define NC_RPC_REPLY_MSG    "rpc-reply"
#define NC_RPC_ERROR        "rpc-error"
#define NC_RPC_OK           "ok"
#define NC_RPC_DATA         "data"

#define SSH_KEYS 3 /* the number of supported keys */

/*
 * Special session ID to be used by libnetconf's internal dummy sessions. This
 * kind of dummy sessions does not break datastore locks on a session close.
 */
#define INTERNAL_DUMMY_ID "0"

/*
 * how to send NETCONF XML content:
 * 1 - formatted, i.e. with new lines and spaces
 * 0 - unformatted, only the content without any unnecessary white space formatting characters
 */
#define NC_CONTENT_FORMATTED 1

/**
 * Sleep time in microseconds to wait between unsuccessful reading due to EAGAIN or EWOULDBLOCK
 */
#define NC_READ_SLEEP 100

/*
 * global settings for options passed to xmlRead* functions
 */
#define NC_XMLREAD_OPTIONS XML_PARSE_NOBLANKS|XML_PARSE_NSCLEAN|XML_PARSE_NOERROR|XML_PARSE_NOWARNING|XML_PARSE_HUGE

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

/*
 * libnetconf paths
 */
#ifndef NCNTF_STREAMS_PATH
#  error "NCNTF_STREAMS_PATH not defined, something bad happened to Makefile"
#endif

#ifdef NC_SESSIONFILE_PATH
#  define NC_SESSIONSFILE  NC_SESSIONFILE_PATH"/libnetconf_sessions.bin"
#else
#  error "SESSIONSFILE_PATH not defined, something bad happened to Makefile"
#endif

/*
 * libnetconf permissions for every file or dir creation, process mask used with special cases
 * such as fopen()
 *
 * SETBIT = 1 - SUID
 * SETBIT = 2 - SGID
 * SETBIT = 3 - SUID + SGID
 * SETBIT = 0 - NONE
 */
#if SETBIT == 1
#	define FILE_PERM 0600
#	define DIR_PERM 0700
#	define MASK_PERM 0066
#elif SETBIT == 2
#	define FILE_PERM 0060
# 	define DIR_PERM 0070
#	define MASK_PERM 0606
#elif SETBIT == 3
#	define FILE_PERM 0660
#	define DIR_PERM 0770
#	define MASK_PERM 0006
#elif SETBIT == 0
#	define FILE_PERM 0666
#	define DIR_PERM 0777
#	define MASK_PERM 0000
#endif

/* libnetconf's message printing */
void prv_printf(NC_VERB_LEVEL level, const char *format, ...);
extern volatile uint8_t verbose_level;
#define ERROR(format,args...) prv_printf(NC_VERB_ERROR,format,##args)
#define WARN(format,args...) if(verbose_level>=NC_VERB_WARNING){prv_printf(NC_VERB_WARNING,format,##args);}
#define VERB(format,args...) if(verbose_level>=NC_VERB_VERBOSE){prv_printf(NC_VERB_VERBOSE,format,##args);}
#define DBG(format,args...) if(verbose_level>=NC_VERB_DEBUG){prv_printf(NC_VERB_DEBUG,format,##args);}
#ifdef DEBUG_THREADS
#define DBG_UNLOCK(name) DBG("Unlocking %s in thread %lu (%s:%d)", name, pthread_self(), __FILE__, __LINE__)
#define DBG_LOCK(name) DBG("Locking %s in thread %lu (%s:%d)", name, pthread_self(), __FILE__, __LINE__)
#else
#define DBG_UNLOCK(name)
#define DBG_LOCK(name)
#endif

/* Tests whether string is empty or non-empty. */
#define strisempty(str) ((str)[0] == '\0')
#define strnonempty(str) ((str)[0] != '\0')

/**
 * @brief Callbacks structure for all the callback functions that can be set by an application
 * @ingroup internalAPI
 */
struct callbacks {
	/**< @brief Message printing function, if not set, all the messages are suppressed */
	void (*print)(NC_VERB_LEVEL level, const char* msg);
	/**< @brief Function processing \<rpc-error\> replies on the client side. If no callback function is set, the error details are ignored */
	void (*process_error_reply)(const char* tag,
			const char* type,
			const char* severity,
			const char* apptag,
			const char* path,
			const char* message,
			const char* attribute,
			const char* element,
			const char* ns,
			const char* sid);
#ifndef DISABLE_LIBSSH
	/**< @brief Callback for libssh's 'keyboard-interactive' authentication method */
	char* (*sshauth_interactive)(const char* name,
			const char* instruction,
			const char* prompt,
			int echo);
	/**< @brief Callback for passing the password for libssh's 'password' authentication method */
	char* (*sshauth_password)(const char* username, const char* hostname);
	/**< @brief Callback for passing the passphrase for libssh's 'publickey' authentication method */
	char* (*sshauth_passphrase)(const char* username, const char* hostname, const char* privatekey_filepath);
	/**< @brief Callback to check the host authenticity: 0 ok, 1 failed */
	int (*hostkey_check)(const char* hostname, ssh_session session);
	/**< @brief */
	char *publickey_filename[SSH_KEYS];
	/**< @brief */
	char *privatekey_filename[SSH_KEYS];
	/**< @brief is private key protected by password */
	int key_protected[SSH_KEYS];
#endif
};

/**
 * @ingroup internalAPI
 * @brief Header instance of a callback structure storing all the callbacks set by the application
 *
 * Real instance of the callbacks structure is placed in callbacks.c where
 * the default values are also set.
 */
extern struct callbacks callbacks;

/**
 * @ingroup internalAPI
 * @brief NETCONF session statistics as defined in RFC 6022 (as common-counters)
 */
struct nc_session_stats {
	unsigned int in_rpcs;
	unsigned int in_bad_rpcs;
	unsigned int out_rpc_errors;
	unsigned int out_notifications;
};

/**
 * @ingroup internalAPI
 * @brief NETCONF statistics section as defined in RFC 6022
 */
struct nc_statistics {
	unsigned int participants;
#define TIME_LENGTH 21
	char start_time[TIME_LENGTH];
	unsigned int bad_hellos;
	unsigned int sessions_in;
	unsigned int sessions_dropped;
	struct nc_session_stats counters;
};

struct nacm_stats {
	unsigned int denied_ops;
	unsigned int denied_data;
	unsigned int denied_notifs;
};

/* concurrently running (crashed) libnetconf apps */
#define NC_APPS_MAX 10
/* command length (without ending '\0') */
#define NC_APPS_COMM_MAX 40
struct nc_apps {
	unsigned char valid[NC_APPS_MAX];
	pid_t pids[NC_APPS_MAX];
	char comms[NC_APPS_MAX][NC_APPS_COMM_MAX+1];
};

/**
 * @ingroup internalAPI
 * @brief Information structure shared between all libnetconf's processes.
 */
struct nc_shared_info {
	pthread_rwlock_t lock;
	unsigned long last_session_id;
	struct nc_statistics stats;
	struct nacm_stats stats_nacm;
	struct nc_apps apps;
};

/**
 * @ingroup internalAPI
 * @brief NETCONF session description structure
 *
 * No one outside libnetconf can access members of this structure.
 */
struct nc_session {
	/**< @brief Session ID */
	char session_id[SID_SIZE];
	/**< @brief Last message ID */
	long long unsigned int msgid;
	/**< @brief only for clients using libssh for communication */
	int transport_socket;
#ifdef ENABLE_TLS
	/**< @brief TLS handler */
	SSL *tls;
#endif
	/**< @brief Input file descriptor for communication with (reading from) the other side of the NETCONF session */
	int fd_input;
#ifdef DISABLE_LIBSSH
	/**< @brief FILE structure for the fd_input file descriptor. This is used only if libssh is not used */
	FILE *f_input;
#endif
	/**< @brief Output file descriptor for communication with (writing to) the other side of the NETCONF session */
	int fd_output;
	/**< @brief Transport protocol identifier */
	NC_TRANSPORT transport;
#ifndef DISABLE_LIBSSH
	/**< @brief */
	ssh_session ssh_sess;
	/**< @brief */
	ssh_channel ssh_chan;
#else
	void *ssh_sess;
	void *ssh_chan;
#endif
	/**< @brief Am I the server endpoint? */
	int is_server;
	/**< @brief netopeer-agent's hostname */
	char *hostname;
	/**< @brief netopeer-agent's port */
	char *port;
	/**< @brief name of the user holding the session */
	char *username;
	/**< @brief NULL-terminated list of external (system) groups for NACM */
	char **groups;
	/**< @brief login time in the yang:date-and-time format */
	char *logintime;
	/**< @brief number of confirmed capabilities */
	struct nc_cpblts *capabilities;
	/**< @brief NETCONF protocol version */
	int version;
	/**< @brief session's with-defaults basic mode */
	NCWD_MODE wd_basic;
	/**< @brief session's with-defaults ORed supported modes */
	int wd_modes;
	/**< @brief status of the NETCONF session */
	volatile uint8_t status;
	/**< @brief thread lock for accessing session items */
	pthread_mutex_t mut_session;
	/**< @brief thread lock for communication channel */
	pthread_mutex_t *mut_channel;
	/**< @brief flag for mut_channel, partially it works as conditional variable */
	volatile uint8_t mut_channel_flag;
	/**< @brief thread lock for accessing queue_event */
	pthread_mutex_t mut_equeue;
	/**< @brief thread lock for accessing queue_msg */
	pthread_mutex_t mut_mqueue;
	/**< @brief queue for received, but not processed, NETCONF messages */
	struct nc_msg* queue_msg;
	/**< @brief queue for received, but not processed, NETCONF Event Notifications */
	struct nc_msg* queue_event;
	/**< @brief flag for active notification subscription on the session */
	int ntf_active;
	/**< @brief flag for stopping notification subscription on the session */
	int ntf_stop;
	/**< @brief thread lock for notifications */
	pthread_mutex_t mut_ntf;
	/**< @brief flag for NACM Recovery session - set if session user ID is 0 */
	int nacm_recovery;
	/**< @brief Flag if the session is monitored and connected to the shared memory segment */
	int monitored;
	/**< @brief NETCONF session statistics as defined in RFC 6022 */
	struct nc_session_stats *stats;
	/**< @brief pointer to the next NETCONF session on the shared SSH session, but different SSH channel */
	struct nc_session *next;
	/**< @brief pointer to the previous NETCONF session on the shared SSH session, but different SSH channel */
	struct nc_session *prev;
};

/**
 * @brief NETCONF error structure representation
 * @ingroup internalAPI
 */
struct nc_err {
	/**
	 * @brief Error tag
	 *
	 * Expected values are
	 * <br/>#NC_ERR_TAG_IN_USE,<br/>#NC_ERR_TAG_INVALID_VALUE,
	 * <br/>#NC_ERR_TAG_TOO_BIG,<br/>#NC_ERR_TAG_MISSING_ATTR,
	 * <br/>#NC_ERR_TAG_BAD_ATTR,<br/>#NC_ERR_TAG_UNKN_ATTR,
	 * <br/>#NC_ERR_TAG_MISSING_ELEM,<br/>#NC_ERR_TAG_BAD_ELEM,
	 * <br/>#NC_ERR_TAG_UNKN_ELEM,<br/>#NC_ERR_TAG_UNKN_NAMESPACE,
	 * <br/>#NC_ERR_TAG_ACCESS_DENIED,<br/>#NC_ERR_TAG_LOCK_DENIED,
	 * <br/>#NC_ERR_TAG_RES_DENIED,<br/>#NC_ERR_TAG_ROLLBCK,
	 * <br/>#NC_ERR_TAG_DATA_EXISTS,<br/>#NC_ERR_TAG_DATA_MISSING,
	 * <br/>#NC_ERR_TAG_OP_NOT_SUPPORTED,<br/>#NC_ERR_TAG_OP_FAILED,
	 * <br/>#NC_ERR_TAG_PARTIAL_OP,<br/>#NC_ERR_TAG_MALFORMED_MSG.
	 */
	char *tag;
	/**
	 * @brief Error layer where the error occurred
	 *
	 * Expected values are
	 * <br/>#NC_ERR_TYPE_RPC,<br/>#NC_ERR_TYPE_PROT,
	 * <br/>#NC_ERR_TYPE_APP,<br/>#NC_ERR_TYPE_TRANS.
	 */
	char *type;
	/**
	 * @brief Error severity.
	 *
	 * Expected values are
	 * <br/>#NC_ERR_SEV_ERR,<br/>#NC_ERR_SEV_WARN.
	 */
	char *severity;
	/**
	 * @brief The data-model-specific or implementation-specific error condition, if one exists.
	 */
	char *apptag;
	/**
	 * @brief XPATH expression identifying the element with the error.
	 */
	char *path;
	/**
	 * @brief Human-readable description of the error.
	 */
	char *message;
	/**
	 * @brief Name of the data-model-specific XML attribute that caused the error.
	 * \todo: The model defines this as a sequence, so we have to support storing of multiple values for this
	 * This information is a part of the error-info element.
	 */
	char *attribute;
	/**
	 * @brief Name of the data-model-specific XML element that caused the error.
	 * \todo: The model defines this as a sequence, so we have to support storing of multiple values for this
	 * This information is a part of the error-info element.
	 */
	char *element;
	/**
	 * @brief Name of the unexpected XML namespace that caused the error.
	 * \todo: The model defines this as a sequence, so we have to support storing of multiple values for this
	 * This information is a part of the error-info element.
	 */
	char *ns;
	/**
	 * @brief Session ID of the session holding the requested lock.
	 *
	 * This information is a part of the error-info element.
	 */
	char *sid;
	/**
	 * @brief Pointer to the next error in the list
	 */
	struct nc_err* next;
};

struct nacm_rpc {
	bool default_read; /* false (0) for permit, true (1) for deny */
	bool default_write; /* false (0) for permit, true (1) for deny */
	bool default_exec; /* false (0) for permit, true (1) for deny */
	struct rule_list** rule_lists;
};

/**
 * @brief generic message structure covering both a rpc and a reply.
 * @ingroup internalAPI
 */
struct nc_msg {
	xmlDocPtr doc;
	xmlXPathContextPtr ctxt;
	char* msgid;
	union {
		NC_REPLY_TYPE reply;
		NC_RPC_TYPE rpc;
		NC_NOTIF_TYPE ntf;
	} type;
	NCWD_MODE with_defaults;
	struct nacm_rpc *nacm;
	struct nc_err* error;
	struct nc_msg* next;
	struct nc_session * session;
	/* rpc-specific fields */
	NC_OP op;
	NC_DATASTORE source;
	NC_DATASTORE target;
};

struct nc_filter {
	NC_FILTER_TYPE type;
	xmlNodePtr subtree_filter;
};

struct nc_cpblts {
	int iter;
	int list_size;
	int items;
	char **list;
};

/**
 * @brief Get a copy of the given string without whitespaces.
 *
 * @param[in] in string to clear.
 *
 * return Copy of the given string without whitespaces. The caller is supposed to free it.
 */
char* nc_clrwspace (const char* in);

/**
 * @brief Replace (repeated) character in string with another one.
 *
 * Remember that given str parameter is being modified, so it cannot be a static
 * string! Function replaces single as well as a sequence of the specified
 * character with another (or the same - to eliminate sequences) character.
 *
 * @param[in,out] str String to modify.
 * @param[in] sought Character to find and to replace.
 * @param[in] replacement Character to be used as the replacement for sought.
 */
void nc_clip_occurences_with(char *str, char sought, char replacement);

/**
 * @brief Process config data according to with-defaults' mode and data model
 * @param[in] config XML configuration data document in which the default values will
 * be modified (added for report-all and removed for trim mode).
 * @param[in] model Configuration data model for the data given in the config parameter.
 * @param[in] mode With-defaults capability mode for the configuration data modification.
 * @return 0 on success, non-zero else.
 */
int ncdflt_default_values(xmlDocPtr config, const xmlDocPtr model, NCWD_MODE mode);

/**
 * @breaf Remove the defaults nodes from the configuration data.
 *
 * Removes nodes with attribute wd:defaultt="true"
 *
 * @param[in] config XML configuration data document from which the default nodes
 * will removed
 * @return 0 on success, non-zero else.
 */
int ncdflt_default_clear(xmlDocPtr config);

/**
 * @brief Replace tagged default values in edit-config's configuration data with
 * operation 'delete' - RFC 6243 sec 4.5.2.
 *
 * @param config Configuration data to be modified
 * @param model Data model for the given configuration data
 * @return EXIT_SUCCESS if the data pass, EXIT_FAILURE if a constrain is broken
 */
int ncdflt_edit_remove_default(xmlDocPtr config, const xmlDocPtr model);

/**
 * @brief Parse the given reply message and create a NETCONF error structure
 * describing the error from the reply. The reply must be of #NC_REPLY_ERROR type.
 * @param[in] reply \<rpc-reply\> message to be parsed.
 * @return Filled error structure according to given rpc-reply, returned value
 * is automatically connected with the given rpc-reply and should not be freed
 * separately. This behaviour should be changed when this function will be a part
 * of the public API and used by applications.
 */
struct nc_err* nc_err_parse(nc_reply* reply);

/**
 * @brief Apply filter on the given XML document.
 * @param data XML document to be filtered.
 * @param filter Filter to apply. Only 'subtree' filters are supported.
 * @param data_model Data model of the filtered document.
 * @return 0 on success,\n non-zero else
 */
int ncxml_filter(xmlNodePtr old, const struct nc_filter * filter, xmlNodePtr *new, const xmlDocPtr data_model);

/**
 * @brief Get state information about sessions. Only information about monitored
 * sessions added by nc_session_monitor() is provided.
 * @return Serialized XML describing session as defined in RFC 6022, NULL on
 * error (or if no session is monitored).
 */
char* nc_session_stats(void);

/**
 * @brief Get human-readable description to the specific type of the session
 * termination reason.
 * @param[in] reason Type of the session termination reason.
 * @return String describing the given termination reason value.
 */
const char* nc_session_term_string(NC_SESSION_TERM_REASON reason);

/**
 * @brief Close NETCONF connection with the server.
 *
 * Only nc_session_free() and nc_session_get_status() functions are allowed
 * after this call.
 *
 * @param[in] session Session to close.
 * @param[in] reason Type of the session termination reason.
 */
void nc_session_close (struct nc_session* session, NC_SESSION_TERM_REASON reason);

#ifndef DISABLE_NOTIFICATIONS

/* sleep time in dispatch loops in microseconds */
#define NCNTF_DISPATCH_SLEEP 10000

/**
 * @brief Initiate the NETCONF Notifications environment
 * @return 0 on success, non-zero value else
 */
int ncntf_init(void);

/**
 * @brief Close all the NETCONF Event Streams and other parts of the Notification
 * environment.
 */
void ncntf_close(void);

int *ncntf_dispatch_location(void);
#define ncntf_dispatch (*ncntf_dispatch_location())

#endif /* DISABLE_NOTIFICATIONS */

/**
 * @brief internal values for nc_init_flags variable
 */
#define NC_INIT_DONE     0x00000001
#define NC_INIT_CLOSING  0x80000000

/**
 * @brief remove all internal datastore structures
 */
void ncds_cleanall();

int nc_session_monitoring_init(void);
void nc_session_monitoring_close(void);

const struct data_model* ncds_get_model_data(const char* namespace);
const struct data_model* ncds_get_model_operation(const char* operation, const char* namespace);
const struct data_model* ncds_get_model_notification(const char* notification, const char* namespace);

char** nc_get_grouplist(const char* username);

#endif /* NC_NETCONF_INTERNAL_H_ */
