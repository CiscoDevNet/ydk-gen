/**
 * \file zmldiff.h
 * \author David Kupka <dkupka@cesnet.cz>
 * \author Michal Vasko <mvasko@cesent.cz>
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Functions implementing XML diff functionality.
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

#ifndef NC_XMLDIFF_H
#define NC_XMLDIFF_H

#include <stdbool.h>
#include <libxml/tree.h>
#include "yinparser.h"
#include "transapi_internal.h"
#include "../transapi.h"

/**
 * @ingroup transapi
 * @brief buffer for storing callback priorities
 */
struct xmldiff_prio {
	int* values;
	size_t used;
	size_t alloc;
};

/**
 * @ingroup transapi
 * @brief tree structure holding all differencies found in compared files
 */
struct xmldiff_tree {
	char* path;
	xmlNodePtr old_node;
	xmlNodePtr new_node;
	XMLDIFF_OP op;

	/*
	 * priority is limited for nodes on the same level, priorities between
	 * different element levels (distance from the root) is irrelevant.
	 */
	int priority;
	/* pointer to the callback connected with this node */
	int (*callback)(void**, XMLDIFF_OP, xmlNodePtr, xmlNodePtr, struct nc_err**);
	CLBCKS_APPLIED applied;

	struct xmldiff_tree* next;
	struct xmldiff_tree* parent;
	struct xmldiff_tree* children;
};

/**
 * @ingroup transapi
 * @brief Destroy and free whole xmldiff structure
 *
 * @param diff	pointer to xmldiff structure
 */
void xmldiff_free (struct xmldiff_tree* diff);

/**
 * @ingroup transapi
 * @brief Module top level function
 *
 * @param old		old version of XML document
 * @param new		new version of XML document
 * @param model	data model in YANG format
 *
 * @return xmldiff structure holding all differences between XML documents or NULL
 */
XMLDIFF_OP xmldiff_diff (struct xmldiff_tree** diff, xmlDocPtr old, xmlDocPtr new, struct model_tree * model);

/**
 * @ingroup transapi
 * @brief this function assigns the callback priority for every change in the tree.
 *		If a change does not have callback, its priority becomes the lowest of
 *		the children priorities.
 * @param tree	difference tree
 * @param callbacks list of transapi callbacks connected with this datastore
 * @param clbk_count Number of callbacks in the callbacks list.
 *
 * @return EXIT_SUCCES on success, EXIT_FAILURE if no callback can
 *		be called for the configuration change
 */
int xmldiff_set_priorities(struct xmldiff_tree* tree, struct clbk *callbacks, int clbk_count);

#endif /* NC_XMLDIFF */
