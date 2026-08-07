#ifndef DATABASE_INIT_H
#define DATABASE_INIT_H

#include "helper.h"

static const char *g_database_init_stmt = MK_STMT(
PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;
PRAGMA cache_size = -200000;
PRAGMA temp_store = MEMORY;

CREATE TABLE IF NOT EXISTS guild_dat (
  gid INTEGER PRIMARY KEY,
  curr_cid INTEGER NOT NULL DEFAULT 0,
  rids TEXT NOT NULL DEFAULT ""
) STRICT;

CREATE TABLE IF NOT EXISTS rules (
  rid INTEGER PRIMARY KEY AUTOINCREMENT,
  title TEXT NOT NULL,
  desc TEXT,
  color TEXT NOT NULL,
  img TEXT
) STRICT;

CREATE TABLE IF NOT EXISTS conf (
  gid INTEGER PRIMARY KEY,
  message INTEGER,
  member INTEGER,
  join_leave INTEGER,
  watch INTEGER,
  mod INTEGER,
  appeal INTEGER
) STRICT;

CREATE TABLE IF NOT EXISTS cases (
  id INTEGER NOT NULL,
  gid INTEGER NOT NULL,
  uid INTEGER NOT NULL,
  type INTEGER NOT NULL,
  rid INTEGER NOT NULL,
  mess_id INTEGER,
  mod_uid INTEGER NOT NULL,
  note TEXT,
  time INTEGER NOT NULL,
  expire INTEGER,
  PRIMARY KEY (id, gid)
) STRICT;

CREATE TABLE IF NOT EXISTS tbans (
  gid INTEGER NOT NULL,
  uid INTEGER NOT NULL,
  cid INTEGER NOT NULL,
  expire INTEGER NOT NULL,
  PRIMARY KEY (gid, uid)
) STRICT;

CREATE TABLE IF NOT EXISTS members (
  uid INTEGER NOT NULL,
  gid INTEGER NOT NULL,
  note TEXT,
  link_uid INTEGER,
  watch INTEGER NOT NULL DEFAULT 0,
  PRIMARY KEY (uid, gid)
) STRICT;

CREATE UNIQUE INDEX IF NOT EXISTS members_gid_uid ON members (gid, uid);
);

#endif // DATABASE_INIT_H
