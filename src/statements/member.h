#ifndef MEMBER_H
#define MEMBER_H

#include "helper.h"

static const char *g_set_member_stmt = MK_STMT(
INSERT INTO members (uid, gid, link_uid, watch) VALUES (?, ?, ?, ?)
ON CONFLICT(gid, uid) DO UPDATE SET link_uid=excluded.link_uid, watch=excluded.watch;
);

#endif // MEMBER_H
