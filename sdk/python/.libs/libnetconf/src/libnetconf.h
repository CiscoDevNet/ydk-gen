/**
 * \file libnetconf.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief libnetconf's main header.
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

#ifndef LIBNETCONF_H_
#define LIBNETCONF_H_

/**
 * \mainpage About
 *
 * [netconfwg]: http://trac.tools.ietf.org/wg/netconf/trac/wiki "NETCONF WG"
 * [mlist]: https://groups.google.com/forum/#!forum/libnetconf
 * [issues]: https://code.google.com/p/libnetconf/issues/list "Issues"
 * [TMC]: https://www.liberouter.org/ "Tools for Monitoring and Configuration"
 * [CESNET]: http://www.ces.net/ "CESNET"
 * [RFC6241]: http://tools.ietf.org/html/rfc6241 "RFC 6241"
 * [wrunning]: http://tools.ietf.org/html/rfc6241#section-8.2
 * [candidate]: http://tools.ietf.org/html/rfc6241#section-8.3
 * [validate]: http://tools.ietf.org/html/rfc6241#section-8.6
 * [startup]: http://tools.ietf.org/html/rfc6241#section-8.7
 * [url]: http://tools.ietf.org/html/rfc6241#section-8.8
 * [RFC6242]: http://tools.ietf.org/html/rfc6242 "RFC 6242"
 * [RFC5277]: http://tools.ietf.org/html/rfc5277 "RFC 5277"
 * [RFC5539bis]: http://tools.ietf.org/html/draft-ietf-netconf-rfc5539bis-05 "RFC 5539bis"
 * [RFC6470]: http://tools.ietf.org/html/rfc6470 "RFC 6470"
 * [RFC6243]: http://tools.ietf.org/html/rfc6243 "RFC 6243"
 * [RFC6536]: http://tools.ietf.org/html/rfc6536 "RFC 6536"
 * [reversessh]: http://tools.ietf.org/html/draft-ietf-netconf-reverse-ssh-05 "Reverse SSH draft"
 * [interopevent]: http://www.internetsociety.org/articles/successful-netconf-interoperability-testing-announced-ietf-85
 * [netopeer]: https://code.google.com/p/netopeer
 *
 * libnetconf is a NETCONF library in C intended for building NETCONF clients
 * and servers. It provides basic functions to connect NETCONF client and server
 * to each other via SSH or TLS, to send and receive NETCONF messages and to
 * store and work with the configuration data in a datastore.
 *
 * libnetconf implements the NETCONF protocol introduced by IETF. More
 * information about the NETCONF protocol can be found at [NETCONF WG][netconfwg].
 *
 * libnetconf is currently under development at the [TMC] department of [CESNET].
 * Any testing of the library is welcome. Please inform us about your
 * experiences with libnetconf via our [mailing list][mlist] or the
 * [Google Code's Issue section][issues]. Any feature suggestion or bugreport
 * is also appreciated.
 *
 * In November 2012, CESNET attended the NETCONF Interoperability Event held in
 * Atlanta, prior to the IETF 85 meeting. We went to the event with the
 * libnetconf-based client and server and successfully tested interoperability
 * with other implementations. The notes from the event can be found
 * [here][interopevent].
 *
 * \section about-features Features
 *
 * - NETCONF v1.0 and v1.1 compliant ([RFC 6241][RFC6241])
 * - NETCONF over SSH ([RFC 6242][RFC6242]) including Chunked Framing Mechanism
 * - NETCONF over TLS ([RFC 5539bis][RFC5539bis])
 * - NETCONF Writable-running capability ([RFC 6241][wrunning])
 * - NETCONF Candidate configuration capability ([RFC 6241][candidate])
 * - NETCONF Validate capability ([RFC 6241][validate])
 * - NETCONF Distinct startup capability ([RFC 6241][startup])
 * - NETCONF URL capability ([RFC 6241][url])
 * - NETCONF Event Notifications ([RFC 5277][RFC5277] and [RFC 6470][RFC6470])
 * - NETCONF With-defaults capability ([RFC 6243][RFC6243])
 * - NETCONF Access Control ([RFC 6536][RFC6536])
 * - NETCONF Call Home ([Reverse SSH draft][reversessh], [RFC 5539bis][RFC5539bis])
 *
 * \section about-apps Example Applications
 *
 * Examples of applications built on top of the libnetconf library can be
 * found in the [Netopeer project][netopeer].
 *
 * \section about-license BSD License
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
 * This software is provided "as is", and any express or implied
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
 *
 * ![CESNET, z.s.p.o.](../../img/cesnet-logo-125.png)
 *
 */

/**
 * \page install Compilation and Installation
 *
 * \section install-getting Cloning git repository
 *
 * As written on libnetconf's [Google Code page](https://code.google.com/p/libnetconf/source/checkout),
 * the repository can be cloned using the following command:
 *
 * ~~~~~~~
 * $ git clone https://code.google.com/p/libnetconf/
 * ~~~~~~~
 *
 * \note In case that git fails to clone the repository make sure your git is at least version 1.6.6.
 *
 * \section install-reqs Requirements
 *
 * Before compiling the source code make sure that your system provides the
 * following libraries or applications. Some of them are optional or can be
 * avoided in cost of missing of some feature - see the notes for the specific
 * item. All requirements are checked by the `configure` script.
 *
 * - compiler (_gcc_, _clang_, ...) and standard headers
 * - _pkg-config_
 * - _libpthreads_
 * - _libxml2_ (including headers from the devel package)
 * - _libxslt_ (including headers from the devel package)
 * - _libssh_ (including headers from the devel package)
 *  - can be omitted by `--disable-libssh` option, but in that case a
 *    standalone SSH client (usually from the openSSH) is required. For more
 *    details, see \ref configure-disable-libssh "--disable-libssh" description.
 * - _libcurl_ (including headers from the devel package)
 *  - can be omitted by `--disable-url` option, but in that case the NETCONF
 *    :url capability is disabled.
 * - _libopenssl_ (including headers from the devel package)
 *  - required only when the TLS transport is enabled by `--enable-tls` option.
 *    More information about the TLS transport can be found in \ref transport
 *    section.
 * - _doxygen_
 *  - optional, required to (re)build documentation (`make doc`)
 * - _rpmbuild_
 *  - optional, required to build RPM package (`make rpm`)
 *
 * \section install-compilation Compilation
 *
 * libnetconf uses standard GNU Autotools toolchain. To compile and install
 * libnetconf you have to go through the following three steps:
 *
 * ~~~~~~~~~~~~~~
 * $ ./configure
 * $ make
 * # make install
 * ~~~~~~~~~~~~~~
 *
 * This way the library will be installed in `/usr/local/lib/` (or lib64) and
 * `/usr/local/include/` respectively.
 *
 * \subsection install-compilation-options Configure Options
 *
 * `configure` script supports the following options. The full list of the
 * accepted options can be shown by `--help` option.
 *
 * - `--disable-libssh` \anchor configure-disable-libssh
 *  - Remove dependency on the `libssh` library. By default,
 *    the `libssh` library is used by the client side functions to create SSH
 *    connection to a remote host. If the usage of the `libssh` is disabled,
 *    libnetconf will use a standalone `ssh(1)` client located in a system path.
 *    `ssh(1)` client is, for example, part of the OpenSSH. This option has no
 *    effect for server-side functionality.
 *
 * - `--disable-notifications`
 *  - Remove support for the NETCONF Notifications.
 *
 * - `--disable-url`
 *  - Remove support for NETCONF :url capability. cURL dependency is also
 *    removed.
 *
 * - `--disable-validation`
 *  - Remove support for NETCONF :validate capability.
 *
 * - `--disable-yang-schemas`
 *  - Remove support for YANG data model format for <get-schema> opration. With
 *    this option, only YIN format is available.
 *
 * - `--enable-debug`
 *  - Add debugging information for a debugger.
 *
 * - `--enable-tls`
 *  - Enable experimental support for TLS transport. More information about the
 *    TLS transport can be found in \ref transport section.
 *
 * - `--with-pyapi[=path_to_python3]`
 *  - Build also the libnetconf Python API. This requires python3, so if it is
 *    installed in some non-standard location, specify the complete path to the
 *    binary. For more information about Python API, see a separated doxygen
 *    documentation accessible from the project main page.
 *
 * - `--with-nacm-recovery-uid=<uid>` \anchor configure-nacm-recovery
 *  - Specify user ID to be used for the identification of the \ref
 *  nacm-recovery "NACM Recovery Session".
 *
 * - `--with-workingdir=<path>`
 *  - Change location of libnetconf's working directory. Default path is
 *    `/var/lib/libnetconf/`. Note that applications using libnetconf should
 *    have permissions to read/write to this path, with `--with-suid` and
 *    `--with-sgid` this is set automatically.
 *
 * - `--with-suid=<user>`
 *  - Limit usage of libnetconf to the specific _user_. With this option,
 *    libnetconf creates shared files and other resources with access rights
 *    limited to the specified _user_. This option can be freely combined with
 *    the `--with-sgid` option. If neither `--with-suid` nor `--with-sgid`
 *    option is specified, full access rights for all users are granted.
 *
 * - `--with-sgid=<group>`
 *  - Limit usage of libnetconf to the specific _group_. With this option,
 *    libnetconf creates shared files and other resources with access rights
 *    limited to the specified _group_. This option can be freely combined with
 *    the `--with-suid` option. If neither `--with-suid` nor `--with-sgid`
 *    option is specified, full access rights for all users are granted.
 *
 * \note
 * If the library is built with `--with-suid` or `--with-sgid` options,
 * the proper suid or/and sgid bit should be set to the server-side
 * application binaries that use the libnetconf library.
 *
 */

/**
 * \page nacm NETCONF Access Control Module (NACM)
 *
 * [RFC6536]: http://tools.ietf.org/html/rfc6536 "RFC 6536"
 * [NACMExamples]: http://tools.ietf.org/html/rfc6536#appendix-A
 *
 * NACM is a transparent subsystem of libnetconf. It is activated using
 * #NC_INIT_NACM flag in the nc_init() function. No other action is required
 * to use NACM in libnetconf. All NACM rules and settings are controlled via
 * standard NETCONF operations since NACM subsystem provides implicit datastore
 * accessible with the ncds_apply_rpc2all() function.
 *
 * libnetconf supports usage of the system groups (/etc/group) in the access
 * control rule-lists. To disable this feature, <enable-external-groups> value
 * must be set to false:
 *
 * ~~~~~~~
 * <nacm xmlns="urn:ietf:params:xml:ns:yang:ietf-netconf-acm">
 *   <enable-external-groups>false</enable-external-groups>
 * </nacm>
 * ~~~~~~~
 *
 *
 * \section nacm-recovery Recovery Session
 *
 * Recovery session serves for setting up initial access rules or to repair a broken
 * access control configuration. If a session is recognized as recovery, NACM
 * subsystem is completely bypassed.
 *
 * By default, libnetconf considers all sessions of the user with the system UID
 * equal zero as recovery. To change this default value to a UID of any user,
 * use configure's \ref configure-nacm-recovery "--with-nacm-recovery-uid"
 * option.
 *
 *
 * \subsection nacm-recovery-init Initial operation
 *
 * According to [RFC 6536][RFC6536], libnetconf's NACM subsystem is initially
 * set to allow reading (permitted read-default), refuse writing (denied
 * write-default) and allow operation execution (permitted exec-default).
 *
 * \note Some operations or data have their specific access control settings
 * defined in their data models. These settings override the described default
 * settings.
 *
 * To change this initial settings, user has to access NACM datastore via
 * a recovery session (since any write operation is denied) and set required
 * access control rules.
 *
 * For example, to change default write rule from deny to permit, use
 * edit-config operation to create (merge) the following configuration data:
 *
 * ~~~~~~~
 * <nacm xmlns="urn:ietf:params:xml:ns:yang:ietf-netconf-acm">
 *   <write-default>permit</write-default>
 * </nacm>
 * ~~~~~~~
 *
 * To guarantee all access rights to a specific users group, use edit-config
 * operation to create (merge) the following rule:
 *
 * ~~~~~~~
 * <nacm xmlns="urn:ietf:params:xml:ns:yang:ietf-netconf-acm">
 *   <rule-list>
 *     <name>admin-acl</name>
 *     <group>admin</group>
 *     <rule>
 *       <name>permit-all</name>
 *       <module-name>*</module-name>
 *       <access-operations>*</access-operations>
 *       <action>permit</action>
 *     </rule>
 *   </rule-list>
 * </nacm>
 * ~~~~~~~
 *
 * More examples can be found in the [Appendix A. of RFC 6536][NACMExamples].
 */

/**
 * \page usage Using libnetconf
 *
 * [netopeer]: https://code.google.com/p/netopeer
 *
 * Useful notes on using the libnetconf library can be found in the following
 * articles. However, the source codes of the example applications in the
 * [Netopeer project][netopeer] are supposed to be the most accurate, up-to-date
 * and generally the best source of information.
 *
 * - \subpage client
 * - \subpage server
 * - \subpage transport
 * - \subpage callhome
 * - \subpage datastores
 * - \subpage validation
 * - \subpage misc
 *   - \ref misc-datetime
 *   - \ref misc-errors
 *   - \ref misc-logging
 * - \subpage troubles
 *
 * \section usage-glossary GLOSSARY
 *
 * - **message** - all the types of messages passing through NETCONF. It
 * includes rpc, rpc-reply and notification.
 *
 */

/**
 * \page client Client
 *
 * \section client-workflow Client Workflow
 *
 * Here is a description of using libnetconf functions in a NETCONF client:
 * -# **Set verbosity (optional)**.\n
 * The verbosity of the libnetconf can be set by nc_verbosity(). By default,
 * libnetconf is completely silent.\n
 * There is a default message-printing function that writes messages on stderr.
 * The application's specific message printing function can be set via
 * nc_callback_print() function.
 * -# **Set SSH authentication methods priorities (optional)**.\n
 * libnetconf supports several SSH authentication methods for connecting to
 * a NETCONF server over SSH. However, the used method is selected from a
 * list of supported authentication methods provided by the server. Client is
 * allowed to specify the priority of each supported authentication method via
 * nc_ssh_pref() function. The authentication method can also be disabled using
 * a negative priority value.\n
 * Default priorities are following:
 *    - *Interactive* (value 3)
 *    - *Password* (value 2)
 *    - *Public keys* (value 1)
 * -# **Set your own callback(s) for the SSH authentication methods (optional)**.\n
 * User credentials are received via the callback functions specific for each
 * authentication method. There are default callbacks, but application can set
 * their own via:
 *    - *Interactive* - nc_callback_sshauth_interactive()
 *    - *Password* - nc_callback_sshauth_password()
 *    - *Publuc keys* - nc_callback_sshauth_passphrase(). Here can
 *    the paths to the key files be also specified by nc_set_publickey_path() and
 *    nc_set_privatekey_path(). If not set, libnetconf tries to find them in
 *    the default paths.
 * -# **Connect to the NETCONF server(s)**.\n
 * Simply call nc_session_connect() to connect to the specified host via SSH.
 * Authentication method is selected according to the default values or the previous
 * steps.
 * -# **Prepare NETCONF rpc message(s)**.\n
 * Creating NETCONF rpc messages is covered by the functions described in the section
 * \ref rpc. The application prepares NETCONF rpc messages according to the specified
 * attributes. These messages can be then repeatedly used for communication
 * over any of the created NETCONF sessions.
 * -# **Send the message to the selected NETCONF server**.\n
 * To send created NETCONF rpc message to the NETCONF server, use
 * nc_session_send_rpc() function. nc_session_send_recv() function connects
 * sending and receiving the reply (see the next step) into one blocking call.
 * -# **Get the server's rpc-reply message**.
 * When the NETCONF rpc is sent, use nc_session_recv_reply() to receive the
 * reply. To learn when the reply is coming, a file descriptor of the
 * communication channel can be checked by poll(), select(), ... This descriptor
 * can be obtained via nc_session_get_eventfd() function.
 * -# **Close the NETCONF session**.\n
 * When the communication is done, the NETCONF session should be freed (session
 * is also properly closed) via  nc_session_free() function.
 * -# **Free all created objects**.\n
 * Do not forget to free created rpc messages (nc_rpc_free()),
 * \link nc_filter_new() filters\endlink (nc_filter_free()) or received NETCONF
 * rpc-replies (nc_reply_free()).
 *
 */

/**
 * \page server Server
 *
 * \section server-arch Server Architecture
 *
 * It is __strongly__ advised to set SUID (or SGID) bit on every application that is
 * built on libnetconf for a user (or group) created for this purpose, as several
 * internal functions behaviour is based on this precondition. libnetconf uses a number
 * of files which pose a security risk if they are accessible by untrustworthy users.
 * This way it is possible not to restrict the use of an application but only the
 * access to its files, so keep this in mind when creating any directories or files
 * that are used.
 *
 * Generally, there are three approaches of how to implement
 * a NETCONF server using libnetconf. The order in which they are mentioned is ascending
 * in their implementation complexity. The use of the first single-level architecture
 * for production environment is discouraged.
 *
 * \subsection server-arch-singlelevel Single-level Architecture
 *
 * ![Single-level architecture](../../img/sl_arch.png "Single-level architecture")
 *
 * In this case, all the server functionality is implemented as a single process.
 * It is started by SSH daemon as its Subsystem for each incoming NETCONF
 * session connection. The main issue of this approach is a simultaneous access
 * to shared resources. The device manager has to solve concurrent access to the
 * controlled device from its multiple instances. libnetconf itself has to deal
 * with simultaneous access to a shared configuration datastore.
 *
 * \subsection server-arch-multilevel Multi-level Architecture
 *
 * ![Multi-level architecture](../../img/ml_arch.png "Multi-level architecture")
 *
 * In the second case, there is only one device manager (NETCONF server) running
 * as a system daemon. This solves the problem of concurrent device access from
 * multiple processes. On the other hand, there is a need for inter-process
 * communication between the device manager and the agents launched as SSH
 * Subsystems. These agents hold NETCONF sessions and receive requests from the
 * clients. libnetconf provides functions (nc_rpc_dump() and nc_rpc_build()) to
 * (de-)serialise content of the NETCONF messages. This allows the NETCONF messages
 * to be passed between an agent and a device manager that applies requests to
 * the operated device and a configuration datastore.
 *
 * \subsection server-arch-integrated Integrated Architecture
 *
 * ![Integrated architecture](../../img/in_arch.png "Integrated architecture")
 *
 * In this last case, there is only a single application running having the SSH
 * transport integrated. This way there is no need for any communication to
 * other processes, which can be substituted by threads. At the cost of
 * significantly increasing the complexity of the server and likely requiring
 * additional libraries, it gains full control over client connections including
 * the transport layer and dependencies on any external applications are removed.
 * This is especially convenient for TLS, which in effect makes this architecture
 * the only viable option that fully supports it.
 *
 * \section server-workflow Server Workflow
 *
 * Here is a description of using libnetconf functions in a NETCONF server.
 * According to the used architecture, the workflow can be split between an agent
 * and a server. For this purpose, functions nc_rpc_dump(), nc_rpc_build() and
 * nc_session_dummy() can be very helpful.
 *
 * -# **Set the verbosity** (optional)\n
 * The verbosity of the libnetconf can be set by nc_verbosity(). By default,
 * libnetconf is completely silent.\n
 * There is a default message printing function writing messages on stderr.
 * On the server side, this is not very useful, since server usually runs
 * as a daemon without stderr. In this case, something like syslog should be
 * used. The application's specific message printing function can be set via
 * nc_callback_print() function.
 * -# **Initiate libnetconf**\n
 * As the first step, libnetconf MUST be initiated using nc_init(). At this
 * moment, the libnetconf subsystems, such as NETCONF Notifications or NETCONF
 * Access Control, are initiated according to the specified parameter of the
 * nc_init() function.
 * -# **Set With-defaults basic mode** (optional)\n
 * By default, libnetconf uses _explicit_ basic mode of the with-defaults
 * capability. The basic mode can be changed via ncdflt_set_basic_mode()
 * function. libnetconf supports _explicit_, _trim_, _report-all_ and
 * _report-all-tagged_ basic modes of the with-defaults capability.
 * -# **Initiate datastore**\n
 * Now, a NETCONF datastore(s) can be created. Each libnetconf's datastore
 * is connected with a single configuration data model. This connection is
 * defined by calling the ncds_new() function, which returns a datastore handler
 * for further manipulation with an uninitialized datastore. Using this function,
 * caller also specifies which datastore implementation type will be used.
 * Optionally, some implementation-type-specific parameters can be set (e.g.
 * ncds_file_set_path()). Finally, datastore must be initiated by ncds_init()
 * that returns datastore's ID which is used in the subsequent calls. There is a set
 * of special implicit datastores with ID #NCDS_INTERNAL_ID that refer
 * to the libnetconf's internal datastore(s).\n
 * Optionally, each datastore can be extended by an augment data model that can
 * be specified by ncds_add_model(). The same function can be used to specify
 * models to resolve YANG's `import` statements. Alternatively, using
 * ncds_add_models_path(), caller can specify a directory where such models can
 * be found automatically. libnetconf searches for the needed models
 * based on the modules names. Filename of the model is expected in a form
 * `module_name[@revision].yin`.\n
 * Caller can also switch on or off the YANG `feauters` in the specific module
 * using ncds_feature_enable(), ncds_feature_disable(), ncds_features_enableall()
 * and ncds_features_disableall() functions.\n
 * Finally, ncds_consolidate() must be called to check all the internal structures
 * and to solve all `import`, `uses` and `augment` statements.
 * -# **Initiate the controlled device**\n
 * This step is actually out of the libnetconf scope. From the NETCONF point
 * of view, startup configuration data should be applied to the running datastore
 * at this point. ncds_device_init() can be used to perform this task,
 * but applying running configuration data to the controlled device must be done
 * by a server specific (non-libnetconf) function.
 * -# **Accept incoming NETCONF connection**.\n
 * This is done by a single call of nc_session_accept() or
 * nc_session_Accept_username() alternatively. Optionally, any specific
 * capabilities supported by the server can be set as the function's parameter.
 * -# Server loop\n Repeat these three steps:
 *  -# **Process incoming requests**.\n
 *  Use nc_session_recv_rpc() to get the next request from the client from the
 *  specified NETCONF session. In case of an error return code, the state of the
 *  session should be checked by nc_session_get_status() to learn if the
 *  session can be further used.\n
 *  According to the type of the request (nc_rpc_get_type()), perform an appropriate
 *  action:
 *   - *NC_RPC_DATASTORE_READ* or *NC_RPC_DATASTORE_WRITE*: use ncds_apply_rpc2all()
 *   to perform the requested operation on the datastore. If the request affects
 *   the running datastore (nc_rpc_get_target() returns NC_DATASTORE_RUNNING),
 *   apply configuration changes to the controlled device.
 *   - *NC_RPC_SESSION*: See the [Netopeer](https://code.google.com/p/netopeer)
 *   example server source codes. There will be a common function added in the
 *   future to handle these requests.
 *  -# **Reply to the client's request**.\n
 *  The reply message is automatically generated by the ncds_apply_rpc2all() function.
 *  However, server can generate its own replies using nc_reply_ok(),
 *  nc_reply_data() (nc_reply_data_ns()) or nc_reply_error() functions. The
 *  reply is sent to the client using nc_session_send_reply() call.
 *  -# **Free all unused objects**.\n
 *  Do not forget to free received rpc messages (nc_rpc_free()) and any created
 *  replies (nc_reply_free()).
 * -# **Close the NETCONF session**.\n
 * Use functions nc_session_free() to close and free all the used sources and
 * structures connected with the session.
 * Server should close the session when a nc_session_* function fails and
 * libnetconf set the status of the session as non-working
 * (nc_session_get_status != NC_SESSION_STATUS_WORKING).
 * -# **Close the libnetconf instance**\n
 * Close internal libnetconf structures and subsystems by the nc_close() call.
 *
 */

/**
 * \page transport Transport Protocol
 *
 * [RFC6241]: http://tools.ietf.org/html/rfc6241 "RFC 6241"
 * [RFC5539bis]:http://tools.ietf.org/html/draft-ietf-netconf-rfc5539bis-05 "RFC 5539bis"
 * [netopeer]:https://code.google.com/p/netopeer/ "Netopeer"
 *
 * NETCONF protocol specifies the set of requirements for the transport protocol
 * in [RFC 6241][RFC6241]. There are currently 2 specifications how to use
 * specific transport protocols for NETCONF: [RFC 6242][RFC6242] for Secure
 * SHell (SSH) and [RFC 5539bis][RFC5539bis] for Transport Layer Security (TLS).
 * SSH is mandatory transport for NETCONF implementations.
 *
 * libnetconf supports SSH transport out of the box. From version 0.8, there is
 * also experimental support for TLS transport. By default, this is not
 * available by default. To enable TLS transport, following action must be
 * performed:
 * - run configure with `--enable-tls` option:
 * ~~~~
 * ./configure --enable-tls
 * ~~~~
 *
 * See the [Netopeer project][netopeer] as a reference implementation.
 *
 * \section transport-client Transport Support on the Client Side
 *
 * To switch from the default SSH transport to the TLS transport, application
 * must call nc_session_transport() with #NC_TRANSPORT_TLS parameter. Remember,
 * that this change applies only to the current thread.
 *
 * Next step is to initiate TLS context for the new NETCONF session. Using
 * nc_tls_init() function, client is supposed to set its client certificate and
 * CA for server certificate validation.
 *
 * Now the TLS context is ready and new NETCONF session over TLS can be
 * established using nc_session_connect(). Application can run nc_tls_init()
 * again, but the changed parameters will be applied only to the newly created
 * NETCONF sessions.
 *
 * To properly clean all resources, call nc_tls_destroy(). It will destroy
 * TLS connection context. This function can be called despite the running
 * NETCONF session, but creating a new NETCONF session over TLS is not allowed
 * after calling nc_tls_destroy() - the client has to call nc_tls_init() again.
 *
 * The whole process described here is supposed to be performed in a single
 * thread. SSH transport works out of the box, so no specific step, as mentioned
 * for TLS in this section, is required.
 *
 * \section transport-server Transport Support on the Server Side
 *
 * There is no specific support for neither SSH or TLS on the server side.
 * libnetconf doesn't implement SSH nor TLS server - it is expected, that
 * NETCONF server application uses external application (sshd, stunnel,...)
 * serving as SSH/TLS server and providing NETCONF data to the NETCONF server
 * stdin, where libnetconf can read the data, and getting data from the NETCONF
 * server stdout to encapsulate the data and send to a client.
 *
 * For both cases, SSH as well as TLS, there are two functions: nc_session_accept()
 * and nc_session_accept_username(), that serve to accept incoming connection
 * despite the transport protocol. As mentioned, they read data from stdin and
 * write data to the stdout. Difference between those functions is in recognizing
 * NETCONF username. nc_session_accept() guesses username from the process's UID.
 * For example, in case of using SSH Subsystem mechanism in OpenSSH
 * implementation, SSH daemon automatically changes UID of the launched SSH
 * Subsystem process (NETCONF server/agent) to the UID of the logged user. But
 * in case of other SSH/TLS server, this doesn't have to be done. In such a case,
 * NETCONF server itself is supposed to correctly recognize the NETCONF username
 * and specify it explicitly when establishing NETCONF session using
 * nc_session_accept_username().
 *
 */

/**
 * \page callhome Call Home
 *
 * [callhomessh]: http://tools.ietf.org/html/draft-ietf-netconf-reverse-ssh-05 "NETCONF over SSH Call Home Draft"
 * [callhometls]: hhttp://tools.ietf.org/html/draft-ietf-netconf-rfc5539bis-05 "RFC 5539bis (NETCONF over TLS)"
 * [servercfg]: http://tools.ietf.org/html/draft-kwatsen-netconf-server-01 "NETCONF Server Configuration Data Model"
 * [netopeer]: https://code.google.com/p/netopeer
 *
 * Call Home is a mechanism how a NETCONF server can initiate connection to a
 * NETCONF client. Specification for this technique can be found in [NETCONF
 * over SSH Call Home draft][callhomessh] and in [NETCONF over TLS RFC]
 * [callhometls]. Some other aspects of this are also described in [NETCONF
 * Server Configuration Data Model][servercfg].
 *
 * This Mechanism is extremely useful especially in case the device (NETCONF
 * server) is deployed behind a NAT and management application is not able to
 * connect (NETCONF client) to such a device.
 *
 * See the [Netopeer project][netopeer] as a reference implementation.
 *
 * \section callhome-client Call Home on the Client Side
 *
 * This section describes how to receive Call Home connection.
 *
 * Because the client usually doesn't except incoming connection, it is
 * necessary to explicitly start listening for Call Home. For this, libnetconf
 * provides nc_callhome_listen() function that allows to specify port where
 * it will wait for incoming Call Home connections. The function makes the
 * caller listening on all interfaces supporting both, IPv4 and IPv6 addresses.
 * To stop listening, nc_callhome_listen_stop() can be used.
 *
 * To get the first connection request from the queue of pending Call Home
 * connections, use nc_callhome_accept(). It gets incoming Call Home TCP socket
 * and uses it to establish a new NETCONF session according to given parameters
 * similarly to nc_session_connect() function.
 *
 * From this point, client can work with returned NETCONF session as usual.
 * There is no special termination function for NETCONF session from Call Home.
 *
 * \section callhome-server Call Home on the Server Side
 *
 * For Call Home, the server initiate connection. Therefore, transport protocol
 * must be set before starting Call Home. It is done by nc_session_transport()
 * function.
 *
 * Next, server is supposed to prepare the list of servers, where libnetconf
 * will be trying to establish the Call Home connection. There is a set of
 * nc_callhome_mngmt_server_*() functions, used for this purpose.
 *
 * Finally, there is nc_callhome_connect() to establish new NETCONF session
 * based on Call Home mechanism. Note, that the function doesn't return a
 * NETCONF session. It forks the process to start a standalone transport
 * protocol server (SSH/TLS) according to the given parameters. It returns PID
 * of the started process and TCP socket used for the communication. This socket
 * can be used for monitoring state of the connection. Do not read any data from
 * this socket.
 *
 * \section callhome-workflow Call Home workflow in libnetconf
 *
 * ![callhome workflow](../../img/callhome.png "Call Home workflow in libnetconf")
 *
 */

/**
 * \page transapi Transaction API (transAPI)
 *
 * Libnetconf transAPI is a framework designed to save developers time and let them
 * focus on configuring and managing their device instead of fighting with the NETCONF
 * protocol.
 *
 * It allows a developer to choose parts of a configuration that can be easily configured
 * as a single block. Based on a list of so called 'sensitive paths' the generator creates
 * C code containing a single callback function for every 'sensitive path'. Whenever
 * something changes in the configuration file, the appropriate callback function is called
 * and it is supposed to reflect configuration changes in the actual device behavior.
 *
 * Additionaly, transAPI provides an opportunity to implement behavior of NETCONF
 * RPC operation defined in the data model. In case *lnctool(1)* finds an RPC
 * definition inside the provided data model, it generates callbacks for it too.
 * Whenever a server calls ncds_apply_rpc2all() with RPC message containing
 * such defined RPC operation, libnetconf uses callback function implemented
 * in the module.
 *
 * \section understanding-parameters Understanding callback parameters
 *  Every transapi callback function has fixed set of parameters. Function header looks like this:
 *
 *  ~~~~~~~{.c}
 *  int callback_path_into_configuration_xml(void **data, XMLDIFF_OP op, xmlNodePtr old_node, xmlNodePtr new_node, struct nc_err **error)
 *  ~~~~~~~
 *
 *  \subsection data void **data
 *
 *   This parameter was added to provide a way to share any data between callbacks.
 *   libnetconf never change (or even access) content of this parameter. Initialy content of 'data' is NULL.
 *   transapi module may use 'data' as it like but is also fully responsible for correct memory handling
 *   and freeing of no longer needed memory referenced by 'data'.
 *
 *  \subsection op XMLDIFF_OP op
 *
 *   Parameter op indicates what event(s) was occured on node. All events are bitwise ored. To test if certaint event occured on node use bitwise and (&).
 *
 *   - Node can be added or removed.
 *    - XMLDIFF_ADD = Node was added.
 *    - XMLDIFF_REM = Node was removed.
 *   - Nodes of type leaf can be changed.
 *    - XMLDIFF_MOD = Node content was changed.
 *   - Container nodes are informed about events occured on descendants. It can be distinguished whether the event was processed or not.
 *    - XMLDIFF_MOD = Some of node children was changed and there is not callback specified for it.
 *    - XMLDIFF_CHAIN = Some of node children was changed and associated callback was called.
 *   - Additionaly, user-ordered lists and leaf-lists are notified when change in order occurs.
 *    - XMLDIFF_SIBLING = Change in order. Some of siblings was added, removed or changed place.
 *    - XMLDIFF_REORDER = Undrelying user-ordered list has changed order.
 *
 *   \subsubsection combinations Valid combinations of events
 *
 *    - XMLDIFF_ADD and XMLDIFF_REM can never be specified simutaneously.
 *    - other restrictions depend on node type:
 *     - Leaf: exactly one of XMLDIFF_ADD, XMLDIFF_REM, XMLDIFF_MOD
 *     - Container: at least one of XMLDIFF_ADD, XMLDIFF_REM, XMLDIFF_MOD, XMLDIFF_CHAIN and posibly XMLDIFF_REORDER when node holds user-ordered list
 *     - List (system-ordered): at least one of XMLDIFF_ADD, XMLDIFF_REM, XMLDIFF_MOD, XMLDIFF_CHAIN and posibly XMLDIFF_REORDER when node holds user-ordered list
 *     - List (user-ordered): at least one of XMLDIFF_ADD, XMLDIFF_REM, XMLDIFF_MOD, XMLDIFF_CHAIN, XMLDIFF_SIBLING and posibly XMLDIFF_REORDER when node holds user-ordered list
 *     - Leaf-list (system-ordered): exactly one of XMLDIFF_ADD, XMLDIFF_REM
 *     - Leaf-list (user-ordered): at least one of XMLDIFF_ADD, XMLDIFF_REM, XMLDIFF_SIBLING
 *
 * Ex.: Leaf processing
 * ~~~~~~~{.c}
 * int callback_some_leaf(void **data, XMLDIFF_OP op, xmlNodePtr old_node, xmlNodePtr new_node, struct nc_err **error) {
 *   if (op & XMLDIFF_MOD) {
 *     // change configured value
 *   } else if (op & XMLDIFF_REM) {
 *     // leaf removed (disable service, close port, ...)
 *   } else if (op & XMLDIFF_ADD) {
 *     // leaf added (enable service, open port, ...)
 *   } else {
 *     *error = nc_err_new(NC_ERR_OP_FAILED);
 *     nc_err_set(error, NC_ERR_PARAM_MSG, "Invalid event for leaf node /some/leaf.");
 *     return(EXIT_FAILURE);
 *   }
 *   return(EXIT_SUCCESS);
 * }
 * ~~~~~~~
 *
 *  \subsection node xmlNodePtr old_node & new_node
 *
 *   Pointer to a particular node instance in either the old or new
 *   configuration document, in which the event was detected. When the node was
 *   removed, new node is not set and when the node was deleted, old node is
 *   not set. It is safe to traverse the whole document using these pointers,
 *   but should be used only when necessary, since transAPI itself does this
 *   for you.
 *
 *   \note It is safe to traverse these nodes, but any modification will
 *   normally be lost. If you need to change some nodes, you can do so, but
 *   only in the new_node subtree (not available on XMLDIFF_REM). Then, for
 *   these changes to be written to the datastore, change 'config_modified'
 *   to 1. This variable is generated into every module.
 *
 *  \subsection error strict nc_err **error
 *
 *   libnetconf's error structure. May (and should) be used to specify error when it occurs and callback returns EXIT_FAILURE. Error description is forwarded to client.
 *
 * \section transAPI-history History of the transAPI versions
 *
 * Each transAPI module source code is generated with the ``transapi_version``
 * variable set to the transAPI version supported by the code generator
 * (*lnctool(1)*). libnetconf requires exactly the same transAPI version in the
 * modules as it supports itself. However, some of the transAPI versions are
 * kind of backward compatible, so it is possible to simply change the value
 * of the ``transapi_version`` variable in the module source code. In that case
 * no additional changes to the transAPI module source code are required.
 *
 * Here is the list of transAPI versions with notes to the changed things and
 * to the backward compatibility.
 *
 * - *version 1*
 *   - Initial reversion.
 * - *version 2*
 *   - Allow callbacks to modify configuration data. This action is announced
 *   by the callback via the ``config_modified`` variable.
 *   - Changes prototype of the transAPI callbacks. It allows to return NETCONF
 *   error description structure from the callbacks.
 *   - Backward incompatible.
 * - *version 3*
 *   - Changes prototype of the ``transapi_init()`` function. It allows the
 *   module can announce to libnetconf the initial configuration of the device
 *   when the module is loaded.
 *   - Changes prototype of the transAPI callbacks. The configuration data are
 *   passed to the callbacks only as the libxml2 structures. Callbacks variant
 *   passing configuration data as strings are no longer available.
 *   - Backward incompatible.
 * - *version 4*
 *   - Callback order - the module can change the order of executing callbacks
 *   from the default 'from leafs to root` to 'from root to leafs`. This is done
 *   via the ``callbacks_order`` variable. If the variable is not defined
 *   (such as in a transAPI v3 module), the default callback order is used.
 *   - Backward compatible.
 * - *version 5*
 *   - Adds support for monitoring external files.
 *   - Backward compatible.
 * - *version 6*
 *   - Every data callback now receives the corresponding node from both the old
 *   configuration and the new configuration. This holds for every operation
 *   except XMLDIFF_ADD (the old node is NULL) and XMLDIFF_REM (the new node
 *   is NULL).
 *   - Every RPC callback now receives a list of all the arguments and it is up
 *   to developers to parse them themselves. To help with this, a simple
 *   function get_rpc_node() is included in a transAPI module code.
 *   - Backward incompatible.
 *
 * \section transapiTutorial transAPI Tutorial
 *
 * [netopeer]: https://code.google.com/p/netopeer
 *
 * On this page we will show how to write a simple module
 * for controlling an example Turing machine.
 * The full implementation can be found in the [Netopeer project](https://code.google.com/p/netopeer).
 * \note To install libnetconf follow the instructions on the \ref install page.
 *
 * \subsection transapiTutorial-prepare Preparations
 *
 * In this example we will work with the data model of a Turing machine provided
 * by the Pyang project (<https://code.google.com/p/pyang/source/browse/trunk/doc/tutorial/examples/turing-machine.yang>).
 *
 * First, we need to identify important parts of the configuration data.
 * Since the turing-machine data model describes only one configurable subtree,
 * we have an easy choice.
 * So, we can create the 'paths_file' file containing the specification of our
 * chosen element and mapping prefixes with URIs for any used namespace.
 *
 * Our file may look like this (irrespective of order):
 * ~~~~~~~{.xml}
 * tm=http://example.net/turing-machine
 * /tm:turing-machine/tm:transition-function/tm:delta
 * ~~~~~~~
 *
 * \subsection transapiTutorial-generating Generating code
 *
 * -# Create a new directory for the turing-machine module and move the data model and the path file into it:
 * ~~~~~~~{.sh}
 * $ mkdir turing-machine && cd turing-machine/
 * $ mv ../turing-machine.yang ../paths_file .
 * ~~~~~~~
 * -# Run *lnctool(1)* for transapi:
 * ~~~~~~~{.sh}
 * $ lnctool --model ./turing-machine.yang transapi --paths ./paths_file
 * ~~~~~~~
 *
 * Besides the generated source code of our transAPI module and GNU Build
 * System files (Makefile.in, configure.in,...), *lnctool(1)* also generates YIN
 * format of the data model and validators accepted by the libnetconf's
 * ncds_new_transapi() and ncds_set_validation() functions:
 * - *.yin - YIN format of the data model
 * - *.rng - RelagNG schema for syntax validation
 * - *-schematron.xsl - Schematron XSL stylesheet for semantics validation
 *
 * The data model can define various `feature`s and use them via the `if-feature`
 * clauses. By default, all features are enabled for the validators. If you plan
 * to to implement only a specific set (or none) of features, specify it to using
 * the `--feature`` option (that can be used multiple times). The value has the
 * following syntax:
 *
 * ~~~~~~~{.sh}
 * --feature module_name:feature_to_enable
 * ~~~~~~~
 *
 * If you want to disable all features of the module, use the following syntax:
 *
 * ~~~~~~~{.sh}
 * --feature module_name:
 * ~~~~~~~
 *
 * \subsection transapiTutorial-augmenting Augmenting module
 *
 * When you are adding a model augmenting the original model, you have generally 2 ways of doing so:
 * \n
 * -# Create a new transAPI module implementing the original model with any augments, basically treating it as a single model. This way you receive a standalone transAPI module that will make the original module obsolete. *lnctool(1)* command:
 * ~~~~~~~{.sh}
 * $ lnctool --model <original_model> --augment-model <augment_model> transapi --path <paths_for_original_and_augment_model>
 * ~~~~~~~
 * \n
 * -# Create a new transAPI module implementing only the augmented parts. This way you receive an additional module that will be used together with the original module, which does not need to be modified in any way. *lnctool(1)* command:
 * ~~~~~~~{.sh}
 * $ lnctool --model <augment_model> transapi --path <paths_for_augment_model>
 * ~~~~~~~
 * .
 * \n
 * However, the case when a model is augmenting an RPC in the original model is special and ONLY the first way of augmenting a module can and MUST be used.
 *
 * \subsection transapiTutorial-coding Filling up functionality
 *
 * Here we show a turing-machine simulating module. The full implementation can
 * be found in the [Netopeer project][netopeer] repository (transAPI/turing/turing-machine.c).
 * The example functions and all the code is simplified and NOT thread-safe.
 *
 * -# Open 'turing-machine.c' file with your favorite editor:\n\n
 * ~~~~~~~{.sh}
 * $ vim turing-machine.c
 * ~~~~~~~
 * \n
 * -# Add global variables and auxiliary functions. This is completely up to you,
 * libnetconf does not work with this anyway. For full explanation of this
 * code look into the referenced working example. It should be called to run
 * the Turing machine, once set up, but some parts were omitted.\n\n
 * ~~~~~~~{.c}
 * static tape_symbol *tm_head = NULL;
 * static tape_symbol *tm_tape = NULL;
 * static cell_index tm_tape_len = 0;
 * static state_index tm_state = 0;
 * static struct delta_rule *tm_delta = NULL;
 *
 * static void* tm_run(void *arg) {
 * 	int changed = 1;
 * 	struct delta_rule *rule = NULL;
 *
 * 	while(changed) {
 * 		changed = 0;
 *
 * 		if (tm_head < tm_tape || (tm_head - tm_tape) >= tm_tape_len) {
 * 			break;
 * 		}
 *
 * 		for (rule = tm_delta; rule != NULL; rule = rule->next) {
 * 			if (rule->in_state == tm_state && rule->in_symbol == tm_head[0]) {
 * 				if (rule->out_state != 0xffff) {
 * 					tm_state = rule->out_state;
 * 				}
 * 				tm_head[0] = rule->out_symbol;
 * 				tm_head = tm_head + rule->head_move;
 *
 * 				changed = 1;
 *
 * 				usleep(100);
 * 				break;
 * 			}
 * 		}
 * 	}
 *
 * 	return (NULL);
 * }
 * ~~~~~~~
 * \n
 * -# Complete the 'transapi_init()' function with actions that will be run
 * right after the module loads and before any other function in the module is
 * called.\n\n
 * The 'running' parameter can optionally return the current configuration
 * state of the device as the 'transapi_init()' detects it. The configuration
 * must correspond with the device data model and it is supposed to contain
 * only the configuration data (defined with 'config true`). The returned
 * data are then compared with the startup configuration and only the diverging
 * values are set according to the startup content using the appropriate
 * transAPI callback functions.\n\n
 * We ignore it in our example - the Turing machine does not have any
 * configuration that could be read from (depend on the state of) the system.
 *
 *   \note After returning from 'transapi_init()' it is assumed that the current
 * running configuration reflects the actual state of the controlled device.
 * For instance, if the model includes some default values and the running
 * configuration is empty, libnetconf assumes that the device is in this default
 * state as defined in the model. If not, then you should apply the default
 * configuration on the device in 'transapi_init()'.
 *
 * \n
 * ~~~~~~~{.c}
 * int transapi_init(xmlDocPtr *running) {
 *     return EXIT_SUCCESS;
 * }
 * ~~~~~~~
 * \n
 * -# Locate the 'transapi_close()' function and fill it with actions that will
 * be run just before the module unloads. No other function of the transAPI
 * module is called after the 'transapi_close()'. We free up all the temporary
 * variables used to store the current state of the machine.\n\n
 * ~~~~~~~{.c}
 * void transapi_close(void) {
 * 	struct delta_rule *rule;
 *
 * 	free(tm_tape);
 *
 * 	for (rule = tm_delta; rule != NULL; rule = tm_delta) {
 * 		tm_delta = rule->next;
 * 		free_delta_rule(rule);
 * 	}
 * }
 * ~~~~~~~
 * \n
 * -# Fill 'get_state_data()' function. This function returns (only!) the state
 * data (defined with 'config false').\n\n
 * ~~~~~~~{.c}
 * char * get_state_data(char * model, char * running, struct nc_err **err) {
 *     return strdup("<?xml version="1.0"?><turing-machine xmlns="http://example.net/turing-machine"> ... </turing-machine>");
 * }
 * ~~~~~~~
 * \n
 * -# Complete the configuration callbacks (they have the `callback_` prefix).
 * The 'op' parameter can be used to determine operation which was done with the
 * node. Parameter 'old_node' holds a copy of node before the change and 'new_node'
 * after the change. More detailed information about the callback parameters can be found above in
 * the \ref understanding-parameters section.\n\n
 * In this code, we treat modification as a removal and an immediate addition to
 * limit branching and simplifiy the code.
 * ~~~~~~~{.c}
 * int callback_tm_turing_machine_tm_transition_function_tm_delta(void **data, XMLDIFF_OP op, xmlNodePtr old_node, xmlNodePtr new_node, struct nc_err **error) {
 * 	char *content = NULL;
 * 	xmlNodePtr n1, n2;
 * 	struct delta_rule *rule = NULL;
 *
 * 	if (op & XMLDIFF_MOD) {
 * 		op = op | (XMLDIFF_REM & XMLDIFF_ADD);
 * 	}
 *
 * 	if (op & XMLDIFF_REM) {
 * 		//remove the rule from the internal list
 * 	}
 *
 * 	if (op & XMLDIFF_ADD) {
 * 		//parse the children of new_node and add the new rule to the list
 * 	}
 *
 * 	return EXIT_SUCCESS;
 * }
 * ~~~~~~~
 * \n
 * -# Fill the RPC message callback functions with the code that will be run
 * when an RPC message with the defined operation arrives.\n\n
 * The RPC defines an optional parameter 'tape-content', so we use it
 * if specified, otherwise use the default value.\n\n
 * ~~~~~~~{.c}
 * nc_reply *rpc_initialize(xmlNodePtr input) {
 * 	xmlNodePtr tape_content = get_rpc_node("tape-content", input);
 * 	struct nc_err* e = NULL;
 *
 * 	free(tm_tape);
 *
 * 	tm_state = 0;
 * 	tm_tape = tm_head = (char*)xmlNodeGetContent(tape_content);
 *
 * 	if (tm_tape == NULL) {
 * 		tm_tape = tm_head = strdup("");
 * 	}
 *
 * 	tm_tape_len = strlen(tm_tape) + 1;
 * 	pthread_mutex_unlock(&tm_run_lock);
 *
 * 	return nc_reply_ok();
 * }
 * ~~~~~~~
 * \n
 * To run the machine, we create another thread and let it
 * run in the background. There are no parameters, so we ignore
 * the input.\n\n
 * ~~~~~~~{.c}
 * nc_reply *rpc_run(xmlNodePtr input) {
 * 	pthread_t tm_run_thread;
 * 	struct nc_err *e;
 * 	char *emsg = NULL;
 * 	int r;
 *
 * 	if ((r = pthread_create(&tm_run_thread, NULL, tm_run, NULL)) != 0) {
 * 		e = nc_err_new(NC_ERR_OP_FAILED);
 * 		asprintf(&emsg, "Unable to start turing machine thread (%s)", strerror(r));
 * 		nc_err_set(e, NC_ERR_PARAM_MSG, emsg);
 * 		free(emsg);
 * 		return nc_reply_error(e);
 * 	}
 *
 * 	pthread_detach(tm_run_thread);
 *
 * 	return nc_reply_ok();
 * }
 * ~~~~~~~
 * \n
 * -# Optionally, you can set monitoring for some external configuration file.\n\n
 * Let's say, that our Turing machine has a textual configuration located in the
 * `/etc/turing.conf` file. libnetconf can monitor this file for modification
 * and whenever an external application changes content of the file, the
 * specified callback is executed. It's up to the callback function to open the
 * file for reading and get the current configuration data.\n\n
 * ~~~~~~~{.c}
 * int example_callback(const char *filepath, xmlDocPtr *running, int* execflag) {
 *     // do nothing
 *     *running = NULL;
 *     *execflag = 0;
 *
 *     return EXIT_SUCCESS;
 * }
 *
 * struct transapi_file_callbacks file_clbks = {
 *     .callbacks_count = 1,
 *     .callbacks = {{.path = "/etc/turing.conf", .func = example_callback}}
 * };
 * ~~~~~~~
 * \n
 * Here is the description of the callback function parameters:\n
 *   - **const char *filepath** - input parameter providing the path to the changed file
 *   - **xmlDocPtr *edit_config** - output parameter to return content for the
 *   `edit-config` operation to change the content of the NETCONF running datastore.
 *   - **int *exec** - output parameter to set if the performed changes should
 *   cause execution of the regular transAPI callbacks. If set to `0`, the
 *   changes are only reflected in the running configuration datastore, but no
 *   transAPI callback is executed.\n\n
 * -# Done
 *
 * \subsection transapiTutorial-compiling Compiling module
 *
 * Following sequence of commands will produce the shared library 'turing-machine.so' which may be loaded into libnetconf:
 * ~~~~~~~{.sh}
 * $ autoreconf --force --install
 * $ ./configure
 * $ make
 * ~~~~~~~
 *
 * \subsection transapiTutorial-using Integrating to a server
 *
 * In a server we use libnetconf's function ncds_new_transapi() instead of ncds_new() to create a transAPI-capable data store.
 * Then, you do not need to process any data-writing (edit-config, copy-config, delete-config, lock, unlock), data-reading (get, get-config)
 * or module data-model-defined RPC operations. All these operations are processed inside the ncds_apply_rpc2all() function.
 *
 */

/**
 * \page misc Miscelaneous
 *
 * [RFC6021]: http://tools.ietf.org/html/rfc6021 "RFC 6021"
 * [RFC6241]: http://tools.ietf.org/html/rfc6241 "RFC 6241"
 *
 * \section misc-datetime Date and Time
 *
 * Various YANG data models uses YANG date-and-time type for representation of
 * dates and times. The type is defined in ietf-yang-types module that comes from
 * [RFC 6021][RFC6021]. The date-and-time type is, for example, used in all
 * NETCONF event notifications.
 *
 * The date-and-time format examples:
 * ~~~~~~~
 *  2014-06-05T12:19:58Z
 *  2014-06-05T14:19:58+02:00
 * ~~~~~~~
 *
 * libnetconf provides two functions to handle timestamps:
 * - nc_datetime2time() to convert YANG date-and-time value to ISO C time_t data
 *   type representing the time as the number of seconds since Epoch
 *   (1970-01-01 00:00:00 +0000 UTC). This data type is used by various standard
 *   functions such as localtime() or ctime().
 * - nc_time2datetime() is a reverse function to the previous one. Optionally,
 *   it accepts specification of the timezone in which the resulting
 *   date-and-time value will be returned.
 *
 * \section misc-errors NETCONF Errors Handling
 *
 * NETCONF protocol specifications ([RFC 6241][RFC6241]) defines list of errors
 * that can be returned by the server. libnetconf provides a set of functions
 * that server to create, manipulate and process NETCONF errors.
 *
 * NETCONF error is represented by `struct nc_err`. Specifying one of the \link
 * #NC_ERR basic errors\endlink, a new error structure can be created by calling
 * the nc_err_new() function. If the caller want to change a \link #NC_ERR_PARAM
 * specific attribute\endlink, the nc_err_set() function can be used. To get
 * the value of the \link #NC_ERR_PARAM specific error attribute\endlink, there
 * is nc_err_get() function. If the caller want to create a copy of some existing
 * error structure, nc_err_dup() can be used. And finally to free all allocated
 * resources, the nc_err_free() is available.
 *
 * To create \<rpc-error\> reply, the application is supposed to use
 * nc_reply_error() and nc_reply_error_add() to add another error structure to
 * the reply.
 *
 * On the client side, \<rpc-error\> replies can be handled manually as any
 * other \<rpc-reply\> recognizing its type using nc_reply_get_type() and then
 * by getting the error message via nc_reply_get_errormsg() and parsing its
 * content manually. The other, recomended, way how to handle incoming \<rpc-error\>
 * replies is to specify the callback function via nc_callback_error_reply().
 * The callback is automatically invoked to process (e.g. print) \<rpc-error\>
 * message. In this case, all NETCONF error attributes are parsed and passed to
 * the callback function.
 *
 * \section misc-logging Logging Messages
 *
 * libnetconf provides information about the data processing on several \link
 * #NC_VERB_LEVEL verbose levels\endlink. However, by default there is no
 * printing routine, so no message from libnetconf appears until the printing
 * callback is specified using nc_callback_print() function. Application is able
 * to set (and change) the current \link #NC_VERB_LEVEL verbose levels\endlink
 * via the nc_verbosity() function.
 *
 * Mainly for \ref transapi "transAPI" modules, there is a set of functions
 * to access message printing callback outside the library:
 * - nc_verb_error()
 * - nc_verb_warning()
 * - nc_verb_verbose()
 *
 */

/**
 * \page troubles Troubleshooting
 *
 * Since libnetconf shares some resources (statistics, datastores,...) among
 * all its instances, various locks usage is necessary. During the development,
 * crashes of an application is very usual. And ometimes, such a crash can happen
 * during a critical section in libnetconf. Then, some of the locks used by
 * libnetconf can stay unlocked so a consequent call to a libnetconf function
 * can cause application freeze. To recover from this state, a developer has to
 * manually unlock the locks removing the following files.
 *  - /dev/shm/sem.NCDS_FLOCK_*
 *  - /var/lib/libnetconf/libnetconf_sessions.bin
 *
 * Note that before removing the locks manually, all applications using
 * libnetconf should be stopped.
 */

/**
 * \defgroup genAPI General functions
 * \brief libnetconf's miscellaneous functions.
 */

/**
 * \defgroup session NETCONF Session
 * \brief libnetconf's functions for handling NETCONF sessions.
 */

/**
 * \defgroup callhome Call Home
 * \brief libnetconf's functions implementing NETCONF Call Home (both SSH and
 * TLS) mechanism. More information can be found at \ref callhome page.
 */

/**
 * \defgroup rpc NETCONF rpc
 * \brief libnetconf's functions for handling NETCONF \<rpc\> messages.
 */

/**
 * \defgroup reply NETCONF rpc-reply
 * \brief libnetconf's functions for handling NETCONF \<rpc-reply\> messages.
 */

/**
 * \defgroup store Datastore operations
 * \brief libnetconf's functions for handling NETCONF datastores. More information
 * can be found at \ref datastores page.
 */

/**
 * \defgroup withdefaults With-defaults capability
 * \brief libnetconf's settings for NETCONF :with-defaults capability as
 * defined in RFC 6243.
 */

/**
 * \defgroup url URL capability
 * \brief libnetconf's settings for NETCONF :url capability as
 * defined in RFC 6241.
 */

/**
 * \defgroup notifications NETCONF Event Notifications
 * \brief libnetconf's implementation of NETCONF asynchronous message delivery
 * as defined in RFC 5277.
 */

/**
 * \defgroup transapi Transaction API
 * \brief libnetconf's implementation of transaction-based partial device reconfiguration.
 */

/**
 * \internal
 * \defgroup internalAPI Internal API
 * \brief libnetconf's functions, structures and macros for internal usage.
 */

#include "config.h"

#include "netconf.h"
#include "callbacks.h"
#include "session.h"
#include "messages.h"
#include "error.h"
#include "datastore.h"
#include "datastore/custom/datastore_custom.h"
#include "with_defaults.h"
#include "transport.h"

#ifndef DISABLE_NOTIFICATIONS
#  include "notifications.h"
#endif

#ifndef DISABLE_URL
#  include "url.h"
#endif

#endif /* LIBNETCONF_H_ */
