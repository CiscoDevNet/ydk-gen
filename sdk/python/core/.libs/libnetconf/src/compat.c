/**
 * \file compat.c
 * \author Radek Krejci <rkrejci@cesnet.cz>
 * \brief Compatibility functions for various platforms.
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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"

#ifndef HAVE_EACCESS

static int group_member(gid_t gid)
{
	int n = 0;
	gid_t *groups = NULL;

	n = getgroups(0, groups);
	groups = malloc(n * sizeof(gid_t*));
	getgroups(n, groups);

	while (n-- > 0) {
		if (groups[n] == gid) {
			return (1);
		}
	}
	return (0);
}

int eaccess(const char *pathname, int mode)
{
	uid_t uid, euid;
	gid_t gid, egid;
	struct stat st;
	int granted;

	uid = getuid();
	euid = geteuid();
	gid = getgid();
	egid = getegid();

	if (uid == euid && gid == egid) {
		/* If we are not set-uid or set-gid, access does the same.  */
		return (access(pathname, mode));
	}

	if (stat(pathname, &st) != 0) {
		return (-1);
	}

	/* root can read/write any file, and execute any file that anyone can execute. */
	if (euid == 0 && ((mode & X_OK) == 0 || (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))) {
		return (0);
	}

	mode &= (X_OK | W_OK | R_OK);	/* Clear any bogus bits. */

	/* check access rights according to the effective id */
	if (euid == st.st_uid) {
		granted = (unsigned int) (st.st_mode & (mode << 6)) >> 6;
	} else if (egid == st.st_gid || group_member(st.st_gid)) {
		granted = (unsigned int) (st.st_mode & (mode << 3)) >> 3;
	} else {
		granted = (st.st_mode & mode);
	}

	if (granted == mode) {
		return (0);
	} else {
		return (-1);
	}
}

#endif /* not HAVE_EACCESS */

