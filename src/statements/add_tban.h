#ifndef ADD_TBAN_H
#define ADD_TBAN_H

#include "helper.h"

static const char *g_add_tban_stmt = MK_STMT(
INSERT INTO tbans (gid, uid, cid, expire) VALUES (?, ?, ?, ?)
ON CONFLICT(gid, uid) DO UPDATE SET cid=excluded.cid, expire=excluded.expire;
);

#endif // ADD_TBAN_H
