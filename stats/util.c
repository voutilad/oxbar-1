/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/ioctl.h>

#include <net/if.h>
#include <net/if_dl.h>

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char*
get_egress()
{
	struct ifgroupreq  ifgr;
	struct ifg_req    *ifg;
	unsigned int       len;
	int                ioctlfd;
	char              *name;

	memset(&ifgr, 0, sizeof(ifgr));
	strlcpy(ifgr.ifgr_name, "egress", sizeof(ifgr.ifgr_name));

	if ((ioctlfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err(1, "egress lookup: socket(AF_INET, SOCK_DGRAM)");

	if (ioctl(ioctlfd, SIOCGIFGMEMB, (caddr_t)&ifgr) == -1)
		err(1, "egress lookup: ioctl(SIOCGIFGMEMB)");

	len = ifgr.ifgr_len;
	if (NULL == (ifgr.ifgr_groups = calloc(1, len)))
		err(1, "egress lookup: calloc w/ len = %d", len);

	if (ioctl(ioctlfd, SIOCGIFGMEMB, (caddr_t)&ifgr) == -1)
		err(1, "egress lookup: ioctl(SIOCGIFGMEMB)");

	name = NULL;
	for (ifg = ifgr.ifgr_groups; ifg && len >= sizeof(*ifg); ifg++) {
		len -= sizeof(*ifg);
		if (NULL != name)
			continue;   /* just use first egress interface */
		if (NULL == (name = strdup(ifg->ifgrq_member)))
			errx(1, "egress: strdup '%s'", ifg->ifgrq_member);
	}

	free(ifgr.ifgr_groups);
	return name;
}
