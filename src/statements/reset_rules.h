#ifndef RESET_RULES_H
#define RESET_RULES_H

#include "helper.h"

static const char *g_reset_rules_stmt = MK_STMT(
DELETE FROM rules WHERE gid = ?;
);

static const char *g_reset_guild_dat_rid_stmt = MK_STMT(
UPDATE guild_dat SET curr_rid = 0 WHERE gid = ?;
);

#endif // RESET_RULES_H
