#ifndef GET_CONF_H
#define GET_CONF_H

#include "helper.h"

static const char *g_get_conf_stmt = MK_STMT(
SELECT gid, message, member, join_leave, watch, mod, appeal
FROM conf WHERE gid = ?;
);

#endif // GET_CONF_H
