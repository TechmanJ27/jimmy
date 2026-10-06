#ifndef GET_CASE_H
#define GET_CASE_H

#include "helper.h"

static const char *g_get_case_stmt = MK_STMT(
SELECT id, gid, uid, type, rule_title, rule_desc, mess_id, mod_uid, note, time, expire
FROM cases WHERE id = ? AND gid = ?;
);

#endif // GET_CASE_H
