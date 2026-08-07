#ifndef GUILD_DAT_H
#define GUILD_DAT_H

#include "helper.h"

static const char *g_set_guild_dat_stmt = MK_STMT(
INSERT INTO guild_dat (gid, curr_cid, rids) VALUES (?, ?, ?)
ON CONFLICT(gid) DO UPDATE SET curr_cid=excluded.curr_cid, rids=excluded.rids;
);

#endif // GUILD_DAT_H
