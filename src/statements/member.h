#ifndef MEMBER_H
#define MEMBER_H

#include "helper.h"

static const char *g_set_member_stmt = MK_STMT(
INSERT INTO members (uid, gid, watch) VALUES (?, ?, ?)
ON CONFLICT(gid, uid) DO UPDATE SET watch=excluded.watch;
);

static const char *g_add_member_link_stmt = MK_STMT(
INSERT OR IGNORE INTO member_links (uid, gid, link_uid) VALUES (?, ?, ?);
);

static const char *g_remove_member_link_stmt = MK_STMT(
DELETE FROM member_links
WHERE uid = ? AND gid = ? AND link_uid = ?;
);

static const char *g_list_member_links_stmt = MK_STMT(
SELECT link_uid
FROM member_links
WHERE uid = ? AND gid = ?;
);

#endif // MEMBER_H
