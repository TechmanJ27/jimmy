#ifndef UPDATE_CONF_H
#define UPDATE_CONF_H

#include "helper.h"

static const char *g_update_conf_stmt = MK_STMT(
UPDATE conf SET
message = CASE WHEN ? THEN ? ELSE message END,
member = CASE WHEN ? THEN ? ELSE member END,
join_leave = CASE WHEN ? THEN ? ELSE join_leave END,
watch = CASE WHEN ? THEN ? ELSE watch END,
mod = CASE WHEN ? THEN ? ELSE mod END,
appeal = CASE WHEN ? THEN ? ELSE appeal END
WHERE gid = ?;
);

#endif // UPDATE_CONF_H
