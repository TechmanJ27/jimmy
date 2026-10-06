#ifndef SET_MEMBER_WATCH_H
#define SET_MEMBER_WATCH_H

#include "helper.h"

static const char *g_set_member_watch_stmt = MK_STMT(
INSERT INTO members (uid, gid, watch) VALUES (?, ?, ?)
ON CONFLICT(gid, uid) DO UPDATE SET watch=excluded.watch;
);

#endif // SET_MEMBER_WATCH_H
