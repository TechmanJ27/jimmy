#ifndef ADD_RULE_H
#define ADD_RULE_H

#include "helper.h"

static const char *g_add_rule_stmt = MK_STMT(
INSERT INTO rules (gid, rid, title, desc, color, img) VALUES (?, ?, ?, ?, ?, ?);
);

static const char *g_inc_rule_stmt = MK_STMT(
INSERT INTO guild_dat (gid, curr_cid, curr_rid) VALUES (?, 0, 1)
ON CONFLICT(gid) DO UPDATE SET curr_rid = guild_dat.curr_rid + 1
RETURNING curr_rid;
);

#endif // ADD_RULE_H
