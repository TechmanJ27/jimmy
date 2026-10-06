#ifndef GET_RULE_H
#define GET_RULE_H

#include "helper.h"

static const char *g_get_rule_stmt = MK_STMT(
SELECT gid, rid, title, desc, color, img FROM rules WHERE gid = ? AND rid = ?;
);

#endif // GET_RULE_H
