/**
 * \file netconf.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief libnetconf's general public functions and structures definitions.
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

#ifndef NC_NETCONF_H_
#define NC_NETCONF_H_

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct nc_msg;

/**
 * @brief rpc message.
 * @ingroup rpc
 */
typedef struct nc_msg nc_rpc;

/**
 * @brief reply message.
 * @ingroup reply
 */
typedef struct nc_msg nc_reply;

/**
 * @brief Event notification message.
 * @ingroup notifications
 */
typedef struct nc_msg nc_ntf;

/**
 * @ingroup session
 * @brief NETCONF capabilities structure
 */
struct nc_cpblts;

/**
 * @ingroup session
 * @brief Type representing NETCONF message-id attribute.
 *
 * It corresponds to the following typedef:
 * typedef char* nc_msgid;
 *
 * We use a macro to avoid compiler warning of 'const nc_msgid' as return type
 * of functions (because const is applied as 'char* const funct()' which is
 * meaningless).
 *
 * Yes, I know that const char* means "pointer to constant character (not
 * string)", but I want to be clear from the API, that function returns pointer
 * to something that should not be changed.
 */
#define nc_msgid char*

/**
 * @brief NETCONF session description structure
 * @ingroup session
 */
struct nc_session;

/**
 * @ingroup rpc
 * @brief NETCONF filter.
 */
struct nc_filter;

/**
 * @ingroup session
 * @brief Enumeration of the possible states of a NETCONF session.
 */
typedef enum NC_SESSION_STATUS {
	NC_SESSION_STATUS_ERROR = -1, /**< undefined status or the error return code */
	NC_SESSION_STATUS_STARTUP = 0, /**< session is setting up */
	NC_SESSION_STATUS_WORKING = 1, /**< session is established and ready to work */
	NC_SESSION_STATUS_CLOSING = 2, /**< session is being closed */
	NC_SESSION_STATUS_CLOSED = 3, /**< session was closed and could not be used for communication */
	NC_SESSION_STATUS_DUMMY = 4 /**< session is DUMMY, only holds information, does not provide connection */
} NC_SESSION_STATUS;

/**
 * @ingroup session
 * @brief Enumeration of reasons of the NETCONF session termination as defined
 * in RFC 6470.
 */
typedef enum NC_SESSION_TERM_REASON {
	NC_SESSION_TERM_CLOSED, /**< closed by client in a normal fashion */
	NC_SESSION_TERM_KILLED, /**< session was terminated by \<kill-session\> operation */
	NC_SESSION_TERM_DROPPED, /**< transport layer connection was unexpectedly closed */
	NC_SESSION_TERM_TIMEOUT, /**< terminated because of inactivity */
	NC_SESSION_TERM_BADHELLO, /**< \<hello\> message was invalid */
	NC_SESSION_TERM_OTHER /**< terminated for some other reason */
} NC_SESSION_TERM_REASON;

/**
 * @brief Enumeration of NETCONF message types.
 * @ingroup genAPI
 */
typedef enum NC_MSG_TYPE {
	NC_MSG_UNKNOWN, /**< error state */
	NC_MSG_WOULDBLOCK, /**< waiting for another message timed out */
	NC_MSG_NONE, /**< no message at input or message was processed internally */
	NC_MSG_HELLO, /**< \<hello\> message */
	NC_MSG_RPC, /**< \<rpc\> message */
	NC_MSG_REPLY, /**< \<rpc-reply\> message */
	NC_MSG_NOTIFICATION = -5 /**< \<notification\> message */
} NC_MSG_TYPE;

/**
 * @brief Enumeration of \<rpc-reply\> types.
 * @ingroup reply
 */
typedef enum NC_REPLY_TYPE {
	NC_REPLY_UNKNOWN, /**< value describing that no rpc-reply type was detected so far */
	NC_REPLY_HELLO, /**< \<hello\> message type, same as NC_RPC_HELLO */
	NC_REPLY_OK, /**< \<ok\> rpc-reply message type */
	NC_REPLY_ERROR, /**< \<rpc-error\> rpc-reply message type */
	NC_REPLY_DATA /**< rpc-reply message containing \<data\> */
} NC_REPLY_TYPE;

/**
 * @brief Enumeration of \<rpc\> operation types.
 * @ingroup rpc
 */
typedef enum NC_RPC_TYPE {
	NC_RPC_UNKNOWN, /**< value describing that no supported operation type was detected so far */
	NC_RPC_HELLO, /**< \<hello\> message type, same as NC_REPLY_HELLO */
	NC_RPC_DATASTORE_READ, /**< \<rpc\> contains operation reading datastore */
	NC_RPC_DATASTORE_WRITE, /**< \<rpc\> contains operation modifying datastore */
	NC_RPC_SESSION /**< \<rpc\> contains operation affecting the session */
} NC_RPC_TYPE;

typedef enum NC_NOTIF_TYPE {
	NC_NTF_UNKNOWN,
	NC_NTF_BASE
} NC_NOTIF_TYPE;

/**
 * @brief Enumeration of supported \<rpc\> operations
 * @ingroup rpc
 */
typedef enum NC_OP {
	NC_OP_UNKNOWN,		/**< unknown/error value */
	NC_OP_GETCONFIG,	/**< \<get-config\> operation */
	NC_OP_GET,		/**< \<get\> operation */
	NC_OP_EDITCONFIG,	/**< \<edit-config\> operation */
	NC_OP_CLOSESESSION,	/**< \<close-session\> operation */
	NC_OP_KILLSESSION,	/**< \<kill-session\> operation */
	NC_OP_COPYCONFIG,	/**< \<copy-config\> operation */
	NC_OP_DELETECONFIG,	/**< \<delete-config\> operation */
	NC_OP_LOCK,		/**< \<lock\> operation */
	NC_OP_UNLOCK,		/**< \<unlock\> operation */
	NC_OP_COMMIT,		/**< \<commit> operation */
	NC_OP_DISCARDCHANGES,	/**< \<discard-changes> operation */
	NC_OP_CREATESUBSCRIPTION,	/**< \<create-subscription\> operation (RFC 5277) */
	NC_OP_GETSCHEMA,	/**< \<get-schema> operation (RFC 6022) */
	NC_OP_VALIDATE		/**< \<validate\> operation */
} NC_OP;

typedef enum NC_ERR_PARAM {
	/**
	 * error-type - The conceptual layer that the error occurred, accepted
	 * values include 'transport', 'rpc', 'protocol', 'application'.
	 */
	NC_ERR_PARAM_TYPE,
	/**
	 * error-tag - Contains a string identifying the error condition.
	 */
	NC_ERR_PARAM_TAG,
	/**
	 * error-severity - The error severity, accepted values are 'error' and
	 * 'warning'.
	 */
	NC_ERR_PARAM_SEVERITY,
	/**
	 * error-app-tag - Contains a string identifying the data-model-specific
	 * or implementation-specific error condition, if one exists. This
	 * element will not be present if no appropriate application error-tag
	 * can be associated with a particular error condition. If both a
	 * data-model-specific and an implementation-specific error-app-tag
	 * exist then the data-model-specific value MUST be used by the
	 * server.
	 */
	NC_ERR_PARAM_APPTAG,
	/**
	 * error-path - Contains an absolute XPath expression identifying the
	 * element path to the node that is associated with the error being
	 * reported.
	 */
	NC_ERR_PARAM_PATH,
	/**
	 * error-message - A string describing the error.
	 */
	NC_ERR_PARAM_MSG,
	/**
	 * bad-attribute in error-info - name of the attribute, contained in
	 * the 'bad-attribute', 'missing-attribute' and 'unknown-attribute' errors.
	 */
	NC_ERR_PARAM_INFO_BADATTR,
	/**
	 * bad-element in error-info - name of the element, contained in
	 * 'missing-attribute', bad-attribute', 'unknown-attribute',
	 * 'missing-element', 'bad-element', 'unknown-element' and
	 * 'unknown-namespace' errors.
	 */
	NC_ERR_PARAM_INFO_BADELEM,
	/**
	 * bad-namespace in error-info - name of an unexpected namespace,
	 * contained in the 'unknown-namespace' error.
	 */
	NC_ERR_PARAM_INFO_BADNS,
	/**
	 * session-id in error-info - session ID of the session holding the
	 * requested lock, contained in 'lock-denied' error.
	 */
	NC_ERR_PARAM_INFO_SID
} NC_ERR_PARAM;

/**
 * @brief Enumeration of the supported types of datastores defined by NETCONF
 * @ingroup store
 */
typedef enum NC_DATASTORE_TYPE {
	NC_DATASTORE_ERROR, /**< error state of functions returning the datastore type */
	NC_DATASTORE_CONFIG, /**< value describing that the datastore is set as config */
	NC_DATASTORE_URL, /**< value describing that the datastore data should be given from the URL */
	NC_DATASTORE_RUNNING, /**< base NETCONF's datastore containing the current device configuration */
	NC_DATASTORE_STARTUP, /**< separated startup datastore as defined in Distinct Startup Capability */
	NC_DATASTORE_CANDIDATE /**< separated working datastore as defined in Candidate Configuration Capability */
} NC_DATASTORE;

/**
 * @ingroup rpc
 * @brief Enumeration of supported NETCONF filter types.
 */
typedef enum NC_FILTER_TYPE {
	NC_FILTER_UNKNOWN, /**< unsupported filter type */
	NC_FILTER_SUBTREE  /**< subtree filter according to RFC 6241, sec. 6 */
} NC_FILTER_TYPE;

/**
 * @ingroup rpc
 * @brief Enumeration of edit-config's operation attribute values.
 */
typedef enum NC_EDIT_OP_TYPE {
	NC_EDIT_OP_ERROR = -1,  /**< for internal purposes, not defined by NETCONF */
	NC_EDIT_OP_MERGE = 1,   /**< merge */
	NC_EDIT_OP_NOTSET = 0,  /**< compatibility value for NC_EDIT_DEFOP_TYPE */
	NC_EDIT_OP_REPLACE = 2, /**< replace */
	NC_EDIT_OP_CREATE,      /**< create */
	NC_EDIT_OP_DELETE,      /**< delete */
	NC_EDIT_OP_REMOVE       /**< remove */
} NC_EDIT_OP_TYPE;

/**
 * @ingroup rpc
 * @brief Enumeration of edit-config's \<default-operation\> element values.
 */
typedef enum NC_EDIT_DEFOP_TYPE {
	NC_EDIT_DEFOP_ERROR = -1,  /**< for internal purposes, not defined by NETCONF */
	NC_EDIT_DEFOP_NOTSET = 0,  /**< follow NETCONF defined default behavior (merge) */
	NC_EDIT_DEFOP_MERGE = 1,   /**< merge (RFC 6241, sec. 7.2) */
	NC_EDIT_DEFOP_REPLACE = 2, /**< replace (RFC 6241, sec. 7.2) */
	NC_EDIT_DEFOP_NONE = 3     /**< none (RFC 6241, sec. 7.2) */
} NC_EDIT_DEFOP_TYPE;

/**
 * @ingroup rpc
 * @brief Enumeration of edit-config's \<error-option\> element values.
 */
typedef enum NC_EDIT_ERROPT_TYPE {
	NC_EDIT_ERROPT_ERROR = -1,   /**< for internal purposes, not defined by NETCONF */
	NC_EDIT_ERROPT_NOTSET = 0,   /**< follow NETCONF defined default behavior (stop-on-error) */
	NC_EDIT_ERROPT_STOP = 1,     /**< stop-on-error (RFC 6241, sec. 7.2) */
	NC_EDIT_ERROPT_CONT = 2,     /**< continue-on-error (RFC 6241, sec. 7.2) */
	NC_EDIT_ERROPT_ROLLBACK = 3  /**< rollback-on-error (RFC 6241, sec. 7.2), valid only when :rollback-on-error capability is enabled */
} NC_EDIT_ERROPT_TYPE;

/**
 * @ingroup rpc
 * @brief Enumeration of edit-config's \<test-option\> element values.
 *
 * Valid only with enabled :validate:1.1 capability.
 */
typedef enum NC_EDIT_TESTOPT_TYPE {
	NC_EDIT_TESTOPT_ERROR = -1,  /**< for internal purposes, not defined by NETCONF */
	NC_EDIT_TESTOPT_NOTSET = 0,  /**< follow NETCONF defined default behavior (test-then-set) */
	NC_EDIT_TESTOPT_TESTSET = 1, /**< test-then-set */
	NC_EDIT_TESTOPT_SET = 2,     /**< set */
	NC_EDIT_TESTOPT_TEST = 3     /**< test-only */
} NC_EDIT_TESTOPT_TYPE;

/**
 * @ingroup withdefaults
 * @brief Enumeration of \<with-defaults\> element values.
 *
 * Valid only with enabled :with-defaults capability
 */
typedef enum NCWD_MODE {
	NCWD_MODE_NOTSET = 0,     /**< follow NETCONF defined default behavior (mode selected by server as its basic mode) */
	NCWD_MODE_ALL = 1,        /**< report-all mode (RFC 6243, sec. 3.1) */
	NCWD_MODE_TRIM = 2,       /**< trim mode (RFC 6243, sec. 3.2) */
	NCWD_MODE_EXPLICIT = 4,   /**< explicit mode (RFC 6243, sec. 3.3) */
	NCWD_MODE_ALL_TAGGED = 8, /**< report-all-tagged mode (RFC 6243, sec. 3.4) */
	NCWD_MODE_IMPL_TAGGED = 16/**< non-standard mode, tags are added only to implicit default nodes */
} NCWD_MODE;

/**
 * @ingroup rpc
 * @brief RPC attributes list
 *
 * List of specific attributes that can be added to selected RPC operations.
 * The attributes can be set by (possibly repeated) call of the
 * nc_rpc_capability_attr() function.
 */
typedef enum NC_CAP_ATTR {
	NC_CAP_ATTR_WITHDEFAULTS_MODE = 1  /**< Set \<with-default\> attribute of the operation */
} NC_CAP_ATTR;

/**
 * @brief Verbosity levels.
 * @ingroup genAPI
 */
typedef enum NC_VERB_LEVEL {
	NC_VERB_ERROR,  /**< Print only error messages. */
	NC_VERB_WARNING,/**< Print error and warning messages. */
	NC_VERB_VERBOSE,/**< Besides errors and warnings, print some other verbose messages. */
	NC_VERB_DEBUG   /**< Print all messages including some development debug messages. */
} NC_VERB_LEVEL;

/**
 * @brief Set libnetconf's verbosity level.
 * @param[in] level Enabled verbosity level (includes all the levels with higher priority).
 * @ingroup genAPI
 */
void nc_verbosity(NC_VERB_LEVEL level);

/**
 * @brief Function for logging error messages.
 * @param[in] format	printf's format string
 * @param[in] ...	list of arguments specified in format
 * @ingroup genAPI
 */
void nc_verb_error(const char * format, ...);

/**
 * @brief Function for logging warning messages.
 * @param[in] format	printf's format string
 * @param[in] ...	list of arguments specified in format
 * @ingroup genAPI
 */
void nc_verb_warning(const char * format, ...);

/**
 * @brief Function for logging verbose messages.
 * @param[in] format	printf's format string
 * @param[in] ...	list of arguments specified in format
 * @ingroup genAPI
 */
void nc_verb_verbose(const char * format, ...);

/**
 * @ingroup session
 * @brief Supported NETCONF transport protocols enumeration. To change currently
 * used transport protocol, call nc_session_transport().
 *
 * Note that NC_TRANSPORT_TLS is supported only when libnetconf is compiled
 * with --enable-tls configure's option. If the option is not used,
 * nc_session_transport() returns EXIT_FAILURE with NC_TRANSPORT_TLS value.
 *
 * This setting is valuable only for client side NETCONF applications.
 */
typedef enum NC_TRANSPORT {
	NC_TRANSPORT_UNKNOWN = -1, /**< Unknown transport protocol, this is not acceptable as input value */
	NC_TRANSPORT_SSH, /**< NETCONF over SSH, this value is used by default */
	NC_TRANSPORT_TLS /**< NETCONF over TLS */
} NC_TRANSPORT;

/**
 * @ingroup genAPI
 * @brief Initialize libnetconf for system-wide usage. This initialization is
 * shared across all the processes
 * @param[in] flags ORed flags for libnetconf initialization. Must include
 * *NC_INIT_CLIENT*, *NC_INIT_MULTILAYER*, or *NC_INIT_SINGLELAYER* with
 * other accepted values including:
 *    - *NC_INIT_LIBSSH_PTHREAD* The application uses libssh and pthread threads
 *    - *NC_INIT_ALL* Enable all available subsystems
 *    - *NC_INIT_MONITORING* Enable ietf-netconf-monitoring module
 *    - *NC_INIT_WD* Enable With-default capability
 *    - *NC_INIT_NOTIF* Enable Notification subsystem
 *    - *NC_INIT_NACM* Enable NETCONF Access Control subsystem
 *
 * Clients call init just for libssh initialization, if it is used.
 * The difference between the multi-layer and single-layer flag is strictly in
 * the behaviour when cleaning shared library resources, either during
 * nc_close() or if the calling process crashed before (equals did not call
 * nc_close()). On multi-layer crash/close, if it was/is the only running
 * libnetconf application, full cleanup is performed, unlike single-layer
 * crash, when this situation is reflected just in the return flag or
 * single-layer close, when only the local resources are released.
 *
 * @return -1 on fatal error\n 0 on success with some possible flags:\n NC_INITRET_NOTFIRST when someone else already called
 * nc_init() since last system-wide nc_close() or system reboot.\n NC_INITRET_RECOVERY when after last init and before this
 * init this application crashed (based on same commands - executable binary names).
 */
int nc_init(int flags);

/*
 * The difference between single and multi-layer server affects only
 * nc_init() and nc_close() calls. Generally, single-layer application
 * can be executed and finish several times, even during a single
 * NETCONF session. A multi-layer application is expected to call
 * nc_close() only after it actually finished all its work and no
 * client is connected.
 */
#define NC_INIT_MULTILAYER 0x00001000  /**< nc_init()'s flag for multi-layer server architecture */
#define NC_INIT_SINGLELAYER 0x00002000 /**< nc_init()'s flag for single-layer server architecture */
#define NC_INIT_CLIENT 0x00004000      /**< nc_init()'s flag for NETCONF clients */

#define NC_INIT_ALL        0xffff8fff /**< nc_init()'s flag to enable all optional features/subsystems */
#define NC_INIT_NOTIF      0x00000002 /**< nc_init()'s flag to enable Notification subsystem. */
#define NC_INIT_NACM       0x00000004 /**< nc_init()'s flag to enable Acccess Control subsystem */
#define NC_INIT_MONITORING 0x00000008 /**< nc_init()'s flag to enable ietf-netconf-monitoring module */
#define NC_INIT_WD         0x00000010 /**< nc_init()'s flag to enable with-default capability */
#define NC_INIT_VALIDATE   0x00000020 /**< nc_init()'s flag to enable server's validation capability */
#define NC_INIT_URL        0x00000040 /**< nc_init()'s flag to enable server's URL capability */
#define NC_INIT_KEEPALIVECHECK  0x00000080 /**< nc_init()'s flag to enable check of monitored sessions.
 * Sometimes the process holding a monitored session crashes and status information
 * of the session is not properly removed from the monitored sessions list.
 * If this option is used, libnetconf checks if the process holding the session
 * is still alive. To do this properly, the session is connected with the PID
 * of the nc_session_monitor() caller. If the PID changes (e.g. after fork() or
 * daemon()), the process is supposed to call nc_session_monitor() againg.
 */
#define NC_INIT_DATASTORES 0x00000100 /**< nc_init()'s flag to use internal datastores */
#define NC_INIT_LIBSSH_PTHREAD 0x00000200 /**< nc_init()'s flag to initialize libssh pthread callbacks */

#define NC_INITRET_NOTFIRST 0x00000001 /**< nc_init()'s return flag for this process not calling nc_init() first */
#define NC_INITRET_RECOVERY 0x00000002 /**< nc_init()'s return flag for this process crashing before (not calling nc_close()) */

/**
 * @ingroup genAPI
 * @brief Release libnetconf resources. Init flag is used to determine if close
 * should be applied as system-wide (NC_INIT_MULTILAYER) or not
 * (NC_INIT_SINGLELAYER). System-wide nc_close() closes all the shared
 * structures if no other libnetconf participant is currently running. Local
 * release of the calling instance from the shared structures is done in both
 * cases.
 * @return -1 on error\n 0 on success\n 1 in case of system-wide when there is
 * another participant using shared structures and system-wide close cannot be
 * done.
 */
int nc_close(void);

/**
 * @ingroup genAPI
 * @brief Transform given time_t (seconds since the epoch) into the RFC 3339 format
 * accepted by NETCONF functions.
 *
 * This is a reverse function to nc_datetime2time().
 *
 * @param[in] time time_t type value returned e.g. by time().
 * @param[in] tz timezone name for the result. See tzselect(1) for list of
 * correct values. If not specified (NULL), the result is provided in UTC (Zulu).
 * @return Printed string in a format compliant to RFC 3339. It is up to the
 * caller to free the returned string.
 */
char* nc_time2datetime(time_t time, const char* tz);

/**
 * @ingroup genAPI
 * @brief Transform given string in RFC 3339 compliant format to the time_t
 * (seconds since the epoch) accepted by most Linux functions.
 *
 * This is a reverse function to nc_time2datetime().
 *
 * @param[in] datetime Time structure returned e.g. by localtime().
 * @return time_t value of the given string.
 */
time_t nc_datetime2time(const char* datetime);

/**
 * @ingroup genAPI
 * @brief Set \<hello\> timeout - how long libnetconf will wait for the \<hello\>
 * message from the other side. Default value is -1 (infinite timeout).
 *
 * @param[in] timeout Timeout in milliseconds, -1 for infinite timeout, 0 for non-blocking.
 */
void nc_hello_timeout(int timeout);

#ifdef __cplusplus
}
#endif

#endif /* NC_NETCONF_H_ */
