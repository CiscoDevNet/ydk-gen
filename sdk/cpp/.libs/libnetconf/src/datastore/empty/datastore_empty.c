/**
 * \file datastore_empty.c
 * \author David Kupka <dkupka@cesnet.cz>
 * \brief NETCONF datastore handling function prototypes and structures for state only devices
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

#include <stdlib.h>
#include <string.h>

#include "../../netconf_internal.h"
#include "datastore_empty.h"

static const char rcsid[] __attribute__((used)) ="$Id: "__FILE__": "RCSID" $";

int ncds_empty_init (struct ncds_ds * UNUSED(ds))
{
	/* nothing to do (for now) */
	return EXIT_SUCCESS;
}

void ncds_empty_free (struct ncds_ds* UNUSED(ds))
{
	return;
}

struct ncds_lockinfo lockinfo = {NC_DATASTORE_ERROR, NULL, NULL};

int ncds_empty_changed(struct ncds_ds* UNUSED(ds))
{
	/* datastore never changes */
	return(0);
}

int ncds_empty_rollback(struct ncds_ds* UNUSED(ds))
{
	return EXIT_SUCCESS;
}

const struct ncds_lockinfo *ncds_empty_lockinfo(struct ncds_ds* UNUSED(ds), NC_DATASTORE UNUSED(target))
{
	return (&lockinfo);
}

int ncds_empty_lock(struct ncds_ds* UNUSED(ds), const struct nc_session* UNUSED(session), NC_DATASTORE UNUSED(target), struct nc_err** UNUSED(error))
{
	return EXIT_SUCCESS;
}

int ncds_empty_unlock(struct ncds_ds* UNUSED(ds), const struct nc_session* UNUSED(session), NC_DATASTORE UNUSED(target), struct nc_err** UNUSED(error))
{
	return EXIT_SUCCESS;
}

char* ncds_empty_getconfig(struct ncds_ds* UNUSED(ds), const struct nc_session* UNUSED(session), NC_DATASTORE UNUSED(target), struct nc_err** UNUSED(error))
{
	return strdup ("");
}

int ncds_empty_copyconfig(struct ncds_ds* UNUSED(ds), const struct nc_session* UNUSED(session), const nc_rpc* UNUSED(rpc), NC_DATASTORE UNUSED(target), NC_DATASTORE UNUSED(source), char*  UNUSED(config), struct nc_err** UNUSED(error))
{
	return EXIT_SUCCESS;
}

int ncds_empty_deleteconfig(struct ncds_ds* UNUSED(ds), const struct nc_session* UNUSED(session), NC_DATASTORE UNUSED(target), struct nc_err** UNUSED(error))
{
	return EXIT_SUCCESS;
}

int ncds_empty_editconfig(struct ncds_ds* UNUSED(ds), const struct nc_session* UNUSED(session), const nc_rpc* UNUSED(rpc), NC_DATASTORE UNUSED(target), const char *  UNUSED(config), NC_EDIT_DEFOP_TYPE  UNUSED(defop), NC_EDIT_ERROPT_TYPE  UNUSED(errop), struct nc_err **UNUSED(error))
{
	return EXIT_SUCCESS;
}
