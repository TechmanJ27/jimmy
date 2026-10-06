#ifndef GET_GUILD_DAT_H
#define GET_GUILD_DAT_H

#include "helper.h"

static const char *g_get_guild_dat_stmt = MK_STMT(
SELECT gid, curr_cid, curr_rid FROM guild_dat WHERE gid = ?;
);

#endif // GET_GUILD_DAT_H
