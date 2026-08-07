#ifndef CONF_H
#define CONF_H

#include "helper.h"

static const char *g_set_conf_stmt = MK_STMT(
INSERT INTO conf (gid, message, member, join_leave, watch, mod, appeal) VALUES (?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(gid) DO UPDATE SET message=excluded.message, member=excluded.member, join_leave=excluded.join_leave,
watch=excluded.watch, mod=excluded.mod, appeal=excluded.appeal;
);

#endif // CONF_H
