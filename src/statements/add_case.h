#ifndef ADD_CASE_H
#define ADD_CASE_H

#include "helper.h"

static const char *g_add_case_stmt = MK_STMT(
INSERT INTO cases (id, gid, uid, type, rule_title, rule_desc, mess_id, mod_uid, note, time, expire) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
ON CONFLICT(id, gid) DO UPDATE SET uid=excluded.uid, type=excluded.type, rule_title=excluded.rule_title, rule_desc=excluded.rule_desc, mess_id=excluded.mess_id,
mod_uid=excluded.mod_uid, note=excluded.note, time=excluded.time, expire=excluded.expire;
);

#endif // ADD_CASE_H
