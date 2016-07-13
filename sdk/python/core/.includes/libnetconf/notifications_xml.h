/**
 * \file notifications_xml.h
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief NETCONF Notifications functions with libxml2 support.
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

#ifndef NC_NOTIFICATIONS_XML_H_
#define NC_NOTIFICATIONS_XML_H_

#include <libxml/tree.h>

#include "netconf.h"
#include "notifications.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup notifications_xml
 * @brief Store a new event into the specified stream. Parameters are specific
 * for different events.
 *
 * ### Event parameters:
 * - #NCNTF_GENERIC
 *  - **const xmlNodePtr content** Content of the notification as defined in RFC 5277.
 *  eventTime is added automatically. The parameter can be a single XML node as
 *  well as a node list.
 * - #NCNTF_BASE_CFG_CHANGE
 *  - #NC_DATASTORE **datastore** Specify which datastore has changed.
 *  - #NCNTF_EVENT_BY **changed_by** Specify the source of the change.
 *   - If the value is set to #NCNTF_EVENT_BY_USER, the following parameter is
 *   required:
 *  - **const struct nc_session* session** Session that required the configuration change.
 * - #NCNTF_BASE_CPBLT_CHANGE
 *  - **const struct nc_cpblts* old** Old list of capabilities.
 *  - **const struct nc_cpblts* new** New list of capabilities.
 *  - #NCNTF_EVENT_BY **changed_by** Specify the source of the change.
 *   - If the value is set to #NCNTF_EVENT_BY_USER, the following parameter is
 *   required:
 *  - **const struct nc_session* session** Session that required the configuration change.
 * - #NCNTF_BASE_SESSION_START
 *  - **const struct nc_session* session** Started session (#NC_SESSION_STATUS_DUMMY session is also allowed).
 * - #NCNTF_BASE_SESSION_END
 *  - **const struct nc_session* session** Finished session (#NC_SESSION_STATUS_DUMMY session is also allowed).
 *  - #NC_SESSION_TERM_REASON **reason** Session termination reason.
 *   - If the value is set to #NC_SESSION_TERM_KILLED, the following parameter is
 *   required.
 *  - **const char* killed-by-sid** The ID of the session that directly caused
 *  the session termination. If the session was terminated by a non-NETCONF
 *  process unknown to the server, use NULL as the value.
 *
 * ### Examples:
 * - ncxmlntf_event_new("mystream", -1, NCNTF_GENERIC, my_xml_data);
 * - ncxmlntf_event_new("netconf", -1, NCNTF_BASE_CFG_CHANGE, NC_DATASTORE_RUNNING, NCNTF_EVENT_BY_USER, my_session);
 * - ncxmlntf_event_new("netconf", -1, NCNTF_BASE_CPBLT_CHANGE, old_cpblts, new_cpblts, NCNTF_EVENT_BY_SERVER);
 * - ncxmlntf_event_new("netconf", -1, NCNTF_BASE_SESSION_START, my_session);
 * - ncxmlntf_event_new("netconf", -1, NCNTF_BASE_SESSION_END, my_session, NC_SESSION_TERM_KILLED, "123456");
 *
 * @param[in] etime Time of the event, if set to -1, the current time is used.
 * @param[in] event Event type to distinguish the following parameters.
 * @param[in] ... Specific parameters for different event types as described
 * above.
 * @return 0 for success, non-zero value else.
 */
int ncxmlntf_event_new(time_t etime, NCNTF_EVENT event, ...);

/**
 * @ingroup notifications_xml
 * @brief Create a new \<notification\> message with the given eventTime and content.
 *
 * @param[in] event_time Time of the event.
 * @param[in] content Description of the event in XML. This parameter is taken
 * as a node list, so the sibling nodes will be also included.
 * @return Created notification message.
 */
nc_ntf* ncxmlntf_notif_create(time_t event_time, const xmlNodePtr content);

/**
 * @ingroup notifications_xml
 * @brief Get the description of the event reported in the notification message.
 * @param[in] notif Notification message.
 * @return Copy of the event description content (single node or node list,
 * eventTime is not included). The caller is supposed to free the returned structure
 * with xmlFreeNodeList().
 */
xmlNodePtr ncxmlntf_notif_get_content(nc_ntf* notif);

#ifdef __cplusplus
}
#endif

#endif /* NC_NOTIFICATIONS_XML_H_ */
