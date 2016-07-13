/**
 * \file libnetconf_xml.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief libnetconf's main header for libxml2 variants of some functions.
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

#ifndef LIBNETCONF_XML_H_
#define LIBNETCONF_XML_H_

/**
 * \defgroup rpc_xml NETCONF rpc (libxml2)
 * \brief libnetconf's functions for handling the NETCONF \<rpc\> messages. These
 * functions accepts parameters as libxml2 structures.
 */

/**
 * \defgroup reply_xml NETCONF rpc-reply (libxml2)
 * \brief libnetconf's functions for handling the NETCONF \<rpc-reply\> messages.
 * These functions accepts parameters as libxml2 structures.
 */

/**
 * \defgroup session NETCONF Session
 * \brief libnetconf's functions for handling NETCONF sessions.
 */

/**
 * \defgroup store Datastore operations
 * \brief libnetconf's functions for handling NETCONF datastores.
 */

/**
 * \defgroup genAPI General functions
 * \brief libnetconf's miscellaneous functions.
 */

/**
 * \defgroup withdefaults With-defaults capability
 * \brief libnetconf's implementation of NETCONF with-defaults capability as
 * defined in RFC 6243.
 */

/**
 * \defgroup notifications_xml NETCONF Event Notifications (libxml2)
 * \brief libnetconf's implementation of NETCONF asynchronous message delivery
 * as defined in RFC 5277. These functions accepts selected parameters as
 * libxml2 structures.
 */

/**
 * \internal
 * \defgroup internalAPI Internal API
 * \brief libnetconf's functions, structures and macros for internal usage.
 */

#include "notifications_xml.h"
#include "messages_xml.h"
#include "datastore_xml.h"
#include "transapi.h"

#include "libnetconf.h"

#endif /* LIBNETCONF_XML_H_ */
