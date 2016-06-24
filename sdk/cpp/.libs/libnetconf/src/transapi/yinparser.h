/**
 * \file yinparser.h
 * \author David Kupka <dkupka@cesnet.cz>
 * \author Michal Vasko <mvasko@cesent.cz>
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions implementing YIN format parser
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

#ifndef NC_YINPARSER_H
#define NC_YINPARSER_H

#include <libxml/tree.h>

#include "../transapi.h"

/**
 * @ingroup transapi
 * @brief enum type for yin/yang constructs
 */
typedef enum {
	YIN_TYPE_MODULE, /**< Top of the YANG data model (http://tools.ietf.org/html/rfc6020#section-7.1) */
	YIN_TYPE_CONTAINER, /**< YANG container statement (http://tools.ietf.org/html/rfc6020#section-7.5) */
	YIN_TYPE_LEAF, /**< YANG leaf statement (http://tools.ietf.org/html/rfc6020#section-7.6) */
	YIN_TYPE_LIST, /**< YANG list statement (https://tools.ietf.org/html/rfc6020#section-7.8) */
	YIN_TYPE_LEAFLIST, /**< YANG leaf-list statement (http://tools.ietf.org/html/rfc6020#section-7.7) */
	YIN_TYPE_CHOICE, /**< YANG choice statement (https://tools.ietf.org/html/rfc6020#section-7.9) */
	YIN_TYPE_ANYXML, /**< YANG anyxml statement (https://tools.ietf.org/html/rfc6020#section-7.10) */
	YIN_TYPE_GROUPING, /**< YANG grouping statement (https://tools.ietf.org/html/rfc6020#section-7.11) */
	YIN_TYPE_IMPORT, /**< YANG import statement (https://tools.ietf.org/html/rfc6020#section-7.17.2) */
	YIN_TYPE_AUGMENT /**< YANG augment statement (https://tools.ietf.org/html/rfc6020#section-7.15) */
} YIN_TYPE;

typedef enum {
	YIN_ORDER_SYSTEM,
	YIN_ORDER_USER
} YIN_ORDER;

/**
 * @ingroup transapi
 * @brief structure holding information about used data model in YIN format
 */
struct model_tree {
	YIN_TYPE type;
	YIN_ORDER ordering; /** < list ordering valid only when type=={YIN_TYPE_LIST|YIN_TYPE_LEAFLIST} */
	char* name;
	char** keys;
	char* ns_uri;
	char* ns_prefix;
	struct model_tree* children;
	int keys_count;
	int children_count;
};

/**
 * @ingroup transapi
 * @brief Parse YIN data model
 *
 * @param model_doc	Data model in YIN format.
 * @param ns_mapping Pairing prefixes with URIs.
 *
 * @return yinmodel structure or NULL
 */
struct model_tree* yinmodel_parse(xmlDocPtr model_doc, struct ns_pair ns_mapping[]);

/**
 * @ingroup transapi
 * @brief Destroy yinmodel structure and free allocated memory.
 *
 * @param yin	Structure to be freed.
 */
void yinmodel_free(struct model_tree * yin);

#endif /* NC_YINPARSER_H */
