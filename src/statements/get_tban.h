#ifndef GET_TBAN_H
#define GET_TBAN_H

#include "helper.h"

static const char *g_get_tban_stmt = MK_STMT(
SELECT gid, uid, cid, expire FROM tbans WHERE gid = ? AND uid = ?;
);

#endif // GET_TBAN_H
