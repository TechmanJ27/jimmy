#include "database.h"

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "statements/check_version.h"
#include "statements/conf.h"
#include "statements/database_init.h"
#include "statements/add_case.h"
#include "statements/add_rule.h"
#include "statements/add_tban.h"
#include "statements/get_case.h"
#include "statements/get_conf.h"
#include "statements/get_guild_dat.h"
#include "statements/get_rule.h"
#include "statements/get_tban.h"
#include "statements/guild_dat.h"
#include "statements/member.h"
#include "statements/remove_expired_tbans.h"
#include "statements/set_version.h"
#include "statements/set_member_watch.h"
#include "statements/update_conf.h"

static sqlite3 *g_database = NULL;

static int bind_snowflake(sqlite3_stmt *stmt, int index, u64snowflake value) {
  if (value > (u64snowflake) INT64_MAX) {
    return SQLITE_RANGE;
  }
  return sqlite3_bind_int64(stmt, index, (sqlite3_int64)value);
}

static int bind_optional_snowflake(sqlite3_stmt *stmt, int index,
                                   const u64snowflake *value) {
  return value == NULL ? sqlite3_bind_null(stmt, index)
                        : bind_snowflake(stmt, index, *value);
}

static int bind_optional_int64(sqlite3_stmt *stmt, int index,
                               const int64_t *value) {
  return value == NULL ? sqlite3_bind_null(stmt, index)
                        : sqlite3_bind_int64(stmt, index, *value);
}

static int bind_optional_text(sqlite3_stmt *stmt, int index, const char *value) {
  return value == NULL ? sqlite3_bind_null(stmt, index) : sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
}

static int execute(sqlite3_stmt *stmt) {
  int status = sqlite3_step(stmt);
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return status == SQLITE_DONE ? SQLITE_OK : status;
}

static int prepare_lazy(const char *sql, sqlite3_stmt **stmt, sqlite3 **owner) {
  if (g_database == NULL) {
    return SQLITE_MISUSE;
  }
  if (*owner != g_database) {
    if (*stmt != NULL)
      sqlite3_finalize(*stmt);
    *stmt = NULL;
    *owner = g_database;
  }
  if (*stmt != NULL)
    return SQLITE_OK;
  int status = sqlite3_prepare_v2(g_database, sql, -1, stmt, NULL);
  if (status != SQLITE_OK)
    *owner = NULL;
  return status;
}

int database_init(int argc, char **argv) {
  const char *path = argc > 2 ? argv[2] : "jimmy.db";
  printf("Database path: %s\n", path);

  int status = sqlite3_open_v2(
      path, &g_database,
      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s\n", sqlite3_errmsg(g_database));
    database_fini();
    return status;
  }

  int version = database_get_version();
  if (version < 0) {
    fprintf(stderr, "Unable to read database version\n");
    database_fini();
    return SQLITE_ERROR;
  }
  if (version != 0 && version < DATABASE_CURRENT_VERSION) {
    fprintf(stderr, "Unsupported database version: %d (expected %d)\n",
            version, DATABASE_CURRENT_VERSION);
    database_fini();
    return SQLITE_SCHEMA;
  }

  status = sqlite3_exec(g_database, g_database_init_stmt, NULL, NULL, NULL);
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s\n", sqlite3_errmsg(g_database));
    database_fini();
    return status;
  }

  if (version == 0) {
    static sqlite3_stmt *stmt = NULL;
    static sqlite3 *owner = NULL;
    status = prepare_lazy(g_set_version_stmt, &stmt, &owner);
    if (status == SQLITE_OK)
      status = execute(stmt);
  }
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s\n", sqlite3_errmsg(g_database));
    database_fini();
  }
  return status;
}

void database_fini(void) {
  if (g_database == NULL) {
    return;
  }
  int status = sqlite3_close_v2(g_database);
  if (status != SQLITE_OK) {
    fprintf(stderr, "%s\n", sqlite3_errstr(status));
  } else {
    g_database = NULL;
  }
}

int database_get_version(void) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (prepare_lazy(g_check_version_stmt, &stmt, &owner) != SQLITE_OK)
    return -1;
  int status = sqlite3_step(stmt);
  int version = status == SQLITE_ROW ? sqlite3_column_int(stmt, 0) : -1;
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return version;
}

int database_set_guild_dat(const struct DatabaseGuildDat *data) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (data == NULL || data->gid == NULL || data->curr_cid == NULL ||
      data->rids == NULL)
    return SQLITE_MISUSE;
  int status = prepare_lazy(g_set_guild_dat_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  status = bind_snowflake(stmt, 1, *data->gid);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int64(stmt, 2, *data->curr_cid);
  if (status == SQLITE_OK)
    status = bind_optional_text(stmt, 3, data->rids);
  return status == SQLITE_OK ? execute(stmt) : (sqlite3_reset(stmt), sqlite3_clear_bindings(stmt), status);
}

int database_set_conf(const struct DatabaseConf *data) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (data == NULL || data->gid == NULL)
    return SQLITE_MISUSE;
  int status = prepare_lazy(g_set_conf_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  status = bind_snowflake(stmt, 1, *data->gid);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 2, data->message);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 3, data->member);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 4, data->join_leave);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 5, data->watch);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 6, data->mod);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 7, data->appeal);
  return status == SQLITE_OK ? execute(stmt) : (sqlite3_reset(stmt), sqlite3_clear_bindings(stmt), status);
}

int database_update_conf(u64snowflake gid, unsigned fields,
                         const struct DatabaseConf *data) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  const u64snowflake *values[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
  const unsigned MASKS[6] = {
      DATABASE_CONF_MESSAGE, DATABASE_CONF_MEMBER, DATABASE_CONF_JOIN_LEAVE,
      DATABASE_CONF_WATCH, DATABASE_CONF_MOD, DATABASE_CONF_APPEAL};
  if (fields == 0 || (fields & ~((1u << 6) - 1u)) != 0)
    return SQLITE_MISUSE;
  if (data != NULL) {
    values[0] = data->message;
    values[1] = data->member;
    values[2] = data->join_leave;
    values[3] = data->watch;
    values[4] = data->mod;
    values[5] = data->appeal;
  }

  int status = prepare_lazy(g_update_conf_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  for (size_t i = 0; i < 6 && status == SQLITE_OK; i++) {
    status = sqlite3_bind_int(stmt, (int)(i * 2 + 1),
                              (fields & MASKS[i]) != 0);
    if (status == SQLITE_OK)
      status = bind_optional_snowflake(stmt, (int)(i * 2 + 2), values[i]);
  }
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 13, gid);
  return status == SQLITE_OK ? execute(stmt)
                             : (sqlite3_reset(stmt), sqlite3_clear_bindings(stmt), status);
}

int64_t database_add_rule(const struct DatabaseRule *rule) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (rule == NULL || rule->title == NULL || rule->color == NULL)
    return -1;
  if (prepare_lazy(g_add_rule_stmt, &stmt, &owner) != SQLITE_OK)
    return -1;
  int status = sqlite3_bind_text(stmt, 1, rule->title, -1, SQLITE_TRANSIENT);
  if (status == SQLITE_OK)
    status = bind_optional_text(stmt, 2, rule->description);
  if (status == SQLITE_OK)
    status = sqlite3_bind_text(stmt, 3, rule->color, -1, SQLITE_TRANSIENT);
  if (status == SQLITE_OK)
    status = bind_optional_text(stmt, 4, rule->image);
  if (status != SQLITE_OK) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return -1;
  }
  status = sqlite3_step(stmt);
  int64_t rid = status == SQLITE_DONE
                    ? (int64_t)sqlite3_last_insert_rowid(g_database)
                    : -1;
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return rid;
}

int database_add_case(const struct DatabaseCase *case_data) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (case_data == NULL || case_data->id == NULL || case_data->gid == NULL ||
      case_data->uid == NULL || case_data->type == NULL ||
      case_data->rid == NULL || case_data->mod_uid == NULL ||
      case_data->time == NULL)
    return SQLITE_MISUSE;
  int status = prepare_lazy(g_add_case_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  status = sqlite3_bind_int64(stmt, 1, *case_data->id);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 2, *case_data->gid);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 3, *case_data->uid);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int(stmt, 4, *case_data->type);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int64(stmt, 5, *case_data->rid);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 6, case_data->message_id);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 7, *case_data->mod_uid);
  if (status == SQLITE_OK)
    status = bind_optional_text(stmt, 8, case_data->note);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int64(stmt, 9, *case_data->time);
  if (status == SQLITE_OK)
    status = bind_optional_int64(stmt, 10, case_data->expire);
  return status == SQLITE_OK ? execute(stmt) : (sqlite3_reset(stmt), sqlite3_clear_bindings(stmt), status);
}

int database_add_tban(u64snowflake gid, u64snowflake uid, int64_t cid,
                      int64_t expire) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  int status = prepare_lazy(g_add_tban_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  status = bind_snowflake(stmt, 1, gid);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 2, uid);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int64(stmt, 3, cid);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int64(stmt, 4, expire);
  return status == SQLITE_OK ? execute(stmt) : (sqlite3_reset(stmt), sqlite3_clear_bindings(stmt), status);
}

int database_remove_expired_tbans(int64_t timestamp,
                                       struct DatabaseTbanKey **removed,
                                       size_t *count) {
  if (removed == NULL || count == NULL)
    return SQLITE_MISUSE;
  *removed = NULL;
  *count = 0;

  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  int status = prepare_lazy(g_remove_expired_tbans_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  status = sqlite3_bind_int64(stmt, 1, timestamp);
  if (status != SQLITE_OK) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return status;
  }

  while ((status = sqlite3_step(stmt)) == SQLITE_ROW) {
    struct DatabaseTbanKey *next = realloc(
        *removed, (*count + 1) * sizeof(**removed));
    if (next == NULL) {
      free(*removed);
      *removed = NULL;
      *count = 0;
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      return SQLITE_NOMEM;
    }
    *removed = next;
    (*removed)[*count].gid = (u64snowflake)sqlite3_column_int64(stmt, 0);
    (*removed)[*count].uid = (u64snowflake)sqlite3_column_int64(stmt, 1);
    (*count)++;
  }

  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  if (status != SQLITE_DONE) {
    free(*removed);
    *removed = NULL;
    *count = 0;
    return status;
  }
  return SQLITE_OK;
}

int database_set_member(const struct DatabaseMember *member) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (member == NULL || member->uid == NULL || member->gid == NULL ||
      member->watch == NULL)
    return SQLITE_MISUSE;
  int status = prepare_lazy(g_set_member_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  status = bind_snowflake(stmt, 1, *member->uid);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 2, *member->gid);
  if (status == SQLITE_OK)
    status = bind_optional_snowflake(stmt, 3, member->link_uid);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int(stmt, 4, *member->watch);
  return status == SQLITE_OK ? execute(stmt) : (sqlite3_reset(stmt), sqlite3_clear_bindings(stmt), status);
}

int database_set_member_watch(u64snowflake uid, u64snowflake gid, int watch) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  int status = prepare_lazy(g_set_member_watch_stmt, &stmt, &owner);
  if (status != SQLITE_OK)
    return status;
  status = bind_snowflake(stmt, 1, uid);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 2, gid);
  if (status == SQLITE_OK)
    status = sqlite3_bind_int(stmt, 3, watch);
  return status == SQLITE_OK ? execute(stmt)
                             : (sqlite3_reset(stmt), sqlite3_clear_bindings(stmt), status);
}

static char *copy_column_text(sqlite3_stmt *stmt, int column) {
  const unsigned char *text = sqlite3_column_text(stmt, column);
  if (text == NULL)
    return NULL;
  size_t size = (size_t)sqlite3_column_bytes(stmt, column) + 1;
  char *copy = malloc(size);
  if (copy != NULL)
    memcpy(copy, text, size);
  return copy;
}

struct DatabaseGuildDat *database_get_guild_dat(u64snowflake gid) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (prepare_lazy(g_get_guild_dat_stmt, &stmt, &owner) != SQLITE_OK)
    return NULL;
  int status = bind_snowflake(stmt, 1, gid);
  if (status == SQLITE_OK)
    status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return NULL;
  }

  struct DatabaseGuildDat *data = calloc(1, sizeof(*data));
  if (data != NULL) {
    data->gid = malloc(sizeof(*data->gid));
    data->curr_cid = malloc(sizeof(*data->curr_cid));
    data->rids = copy_column_text(stmt, 2);
    if (data->gid != NULL)
      *data->gid = (u64snowflake)sqlite3_column_int64(stmt, 0);
    if (data->curr_cid != NULL)
      *data->curr_cid = sqlite3_column_int64(stmt, 1);
    if (data->gid == NULL || data->curr_cid == NULL || data->rids == NULL) {
      database_free_guild_dat(data);
      data = NULL;
    }
  }
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return data;
}

struct DatabaseRule *database_get_rule(int64_t rid) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (prepare_lazy(g_get_rule_stmt, &stmt, &owner) != SQLITE_OK)
    return NULL;
  int status = sqlite3_bind_int64(stmt, 1, rid);
  if (status == SQLITE_OK)
    status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return NULL;
  }

  struct DatabaseRule *rule = calloc(1, sizeof(*rule));
  if (rule != NULL) {
    rule->title = copy_column_text(stmt, 0);
    rule->description = copy_column_text(stmt, 1);
    rule->color = copy_column_text(stmt, 2);
    rule->image = copy_column_text(stmt, 3);
    if (rule->title == NULL || rule->description == NULL ||
        rule->color == NULL ||
        (sqlite3_column_type(stmt, 3) != SQLITE_NULL && rule->image == NULL)) {
      database_free_rule(rule);
      rule = NULL;
    }
  }
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return rule;
}

struct DatabaseCase *database_get_case(int64_t id, u64snowflake gid) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (prepare_lazy(g_get_case_stmt, &stmt, &owner) != SQLITE_OK)
    return NULL;
  int status = sqlite3_bind_int64(stmt, 1, id);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 2, gid);
  if (status == SQLITE_OK)
    status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return NULL;
  }

  struct DatabaseCase *case_data = calloc(1, sizeof(*case_data));
  if (case_data != NULL) {
    case_data->id = malloc(sizeof(*case_data->id));
    case_data->gid = malloc(sizeof(*case_data->gid));
    case_data->uid = malloc(sizeof(*case_data->uid));
    case_data->type = malloc(sizeof(*case_data->type));
    case_data->rid = malloc(sizeof(*case_data->rid));
    case_data->message_id = sqlite3_column_type(stmt, 5) == SQLITE_NULL
                                ? NULL
                                : malloc(sizeof(*case_data->message_id));
    case_data->mod_uid = malloc(sizeof(*case_data->mod_uid));
    case_data->note = copy_column_text(stmt, 7);
    case_data->time = malloc(sizeof(*case_data->time));
    case_data->expire = sqlite3_column_type(stmt, 9) == SQLITE_NULL
                            ? NULL
                            : malloc(sizeof(*case_data->expire));
    if (case_data->id != NULL)
      *case_data->id = sqlite3_column_int64(stmt, 0);
    if (case_data->gid != NULL)
      *case_data->gid = (u64snowflake)sqlite3_column_int64(stmt, 1);
    if (case_data->uid != NULL)
      *case_data->uid = (u64snowflake)sqlite3_column_int64(stmt, 2);
    if (case_data->type != NULL)
      *case_data->type = sqlite3_column_int(stmt, 3);
    if (case_data->rid != NULL)
      *case_data->rid = sqlite3_column_int64(stmt, 4);
    if (case_data->message_id != NULL)
      *case_data->message_id = (u64snowflake)sqlite3_column_int64(stmt, 5);
    if (case_data->mod_uid != NULL)
      *case_data->mod_uid = (u64snowflake)sqlite3_column_int64(stmt, 6);
    if (case_data->time != NULL)
      *case_data->time = sqlite3_column_int64(stmt, 8);
    if (case_data->expire != NULL)
      *case_data->expire = sqlite3_column_int64(stmt, 9);
    if (case_data->id == NULL || case_data->gid == NULL ||
        case_data->uid == NULL || case_data->type == NULL ||
        case_data->rid == NULL || case_data->mod_uid == NULL ||
        case_data->time == NULL ||
        (sqlite3_column_type(stmt, 5) != SQLITE_NULL &&
         case_data->message_id == NULL) ||
        (sqlite3_column_type(stmt, 7) != SQLITE_NULL &&
         case_data->note == NULL) ||
        (sqlite3_column_type(stmt, 9) != SQLITE_NULL &&
         case_data->expire == NULL)) {
      database_free_case(case_data);
      case_data = NULL;
    }
  }
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return case_data;
}

struct DatabaseConf *database_get_conf(u64snowflake gid) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (prepare_lazy(g_get_conf_stmt, &stmt, &owner) != SQLITE_OK)
    return NULL;
  int status = bind_snowflake(stmt, 1, gid);
  if (status == SQLITE_OK)
    status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return NULL;
  }

  struct DatabaseConf *data = calloc(1, sizeof(*data));
  if (data != NULL) {
    data->gid = malloc(sizeof(*data->gid));
    if (data->gid != NULL)
      *data->gid = (u64snowflake)sqlite3_column_int64(stmt, 0);
    u64snowflake **values[] = {&data->message, &data->member, &data->join_leave,
                               &data->watch,   &data->mod,    &data->appeal};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
      if (sqlite3_column_type(stmt, (int)i + 1) != SQLITE_NULL) {
        *values[i] = malloc(sizeof(**values[i]));
        if (*values[i] != NULL)
          **values[i] = (u64snowflake)sqlite3_column_int64(stmt, (int)i + 1);
      }
    }
    if (data->gid == NULL) {
      database_free_conf(data);
      data = NULL;
    } else {
      for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (sqlite3_column_type(stmt, (int)i + 1) != SQLITE_NULL &&
            *values[i] == NULL) {
          database_free_conf(data);
          data = NULL;
          break;
        }
      }
    }
  }
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return data;
}

struct DatabaseTban *database_get_tban(u64snowflake gid, u64snowflake uid) {
  static sqlite3_stmt *stmt = NULL;
  static sqlite3 *owner = NULL;
  if (prepare_lazy(g_get_tban_stmt, &stmt, &owner) != SQLITE_OK)
    return NULL;
  int status = bind_snowflake(stmt, 1, gid);
  if (status == SQLITE_OK)
    status = bind_snowflake(stmt, 2, uid);
  if (status == SQLITE_OK)
    status = sqlite3_step(stmt);
  if (status != SQLITE_ROW) {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return NULL;
  }

  struct DatabaseTban *tban = calloc(1, sizeof(*tban));
  if (tban != NULL) {
    tban->gid = malloc(sizeof(*tban->gid));
    tban->uid = malloc(sizeof(*tban->uid));
    tban->cid = malloc(sizeof(*tban->cid));
    tban->expire = malloc(sizeof(*tban->expire));
    if (tban->gid != NULL)
      *tban->gid = (u64snowflake)sqlite3_column_int64(stmt, 0);
    if (tban->uid != NULL)
      *tban->uid = (u64snowflake)sqlite3_column_int64(stmt, 1);
    if (tban->cid != NULL)
      *tban->cid = sqlite3_column_int64(stmt, 2);
    if (tban->expire != NULL)
      *tban->expire = sqlite3_column_int64(stmt, 3);
    if (tban->gid == NULL || tban->uid == NULL || tban->cid == NULL ||
        tban->expire == NULL) {
      database_free_tban(tban);
      tban = NULL;
    }
  }
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  return tban;
}

void database_free_guild_dat(struct DatabaseGuildDat *data) {
  if (data == NULL)
    return;
  free(data->gid);
  free(data->curr_cid);
  free((char *)data->rids);
  free(data);
}

void database_free_rule(struct DatabaseRule *rule) {
  if (rule == NULL)
    return;
  free((char *)rule->title);
  free((char *)rule->description);
  free((char *)rule->color);
  free((char *)rule->image);
  free(rule);
}

void database_free_case(struct DatabaseCase *case_data) {
  if (case_data == NULL)
    return;
  free(case_data->id);
  free(case_data->gid);
  free(case_data->uid);
  free(case_data->type);
  free(case_data->rid);
  free(case_data->message_id);
  free(case_data->mod_uid);
  free((char *)case_data->note);
  free(case_data->time);
  free(case_data->expire);
  free(case_data);
}

void database_free_conf(struct DatabaseConf *data) {
  if (data == NULL)
    return;
  free(data->gid);
  free(data->message);
  free(data->member);
  free(data->join_leave);
  free(data->watch);
  free(data->mod);
  free(data->appeal);
  free(data);
}

void database_free_tban(struct DatabaseTban *tban) {
  if (tban == NULL)
    return;
  free(tban->gid);
  free(tban->uid);
  free(tban->cid);
  free(tban->expire);
  free(tban);
}
