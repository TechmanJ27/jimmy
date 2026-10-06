#ifndef DATABASE_H
#define DATABASE_H

#include <concord/discord.h>

struct DatabaseGuildDat {
  u64snowflake *gid;
  int64_t *curr_cid;
  int64_t *curr_rid;
};

struct DatabaseConf {
  u64snowflake *gid;
  u64snowflake *message;
  u64snowflake *member;
  u64snowflake *join_leave;
  u64snowflake *watch;
  u64snowflake *mod;
  const char *appeal;
};

enum DatabaseConfField {
  DATABASE_CONF_MESSAGE = 1u << 0,
  DATABASE_CONF_MEMBER = 1u << 1,
  DATABASE_CONF_JOIN_LEAVE = 1u << 2,
  DATABASE_CONF_WATCH = 1u << 3,
  DATABASE_CONF_MOD = 1u << 4,
  DATABASE_CONF_APPEAL = 1u << 5
};

struct DatabaseRule {
  u64snowflake *gid;
  int64_t *rid;
  const char *title;
  const char *description;
  const char *color;
  const char *image;
};

struct DatabaseCase {
  int64_t *id;
  u64snowflake *gid;
  u64snowflake *uid;
  int *type;
  const char *rule_title;
  const char *rule_description;
  u64snowflake *message_id;
  u64snowflake *mod_uid;
  const char *note;
  int64_t *time;
  int64_t *expire;
};

struct DatabaseMember {
  u64snowflake *uid;
  u64snowflake *gid;
  int *watch;
};

struct DatabaseTban {
  u64snowflake *gid;
  u64snowflake *uid;
  int64_t *cid;
  int64_t *expire;
};

struct DatabaseTbanKey {
  u64snowflake gid;
  u64snowflake uid;
};

int database_init(int argc, char **argv);
void database_fini();
int database_get_version(void);

int database_set_guild_dat(const struct DatabaseGuildDat *data);
int database_set_conf(const struct DatabaseConf *data);
int database_update_conf(u64snowflake gid, unsigned fields,
                         const struct DatabaseConf *data);
int64_t database_add_rule(const struct DatabaseRule *rule);
int database_reset_rules(u64snowflake gid);
int database_add_case(const struct DatabaseCase *case_data);
int database_add_tban(u64snowflake gid, u64snowflake uid, int64_t cid, int64_t expire);
int database_remove_expired_tbans(int64_t timestamp,
                                       struct DatabaseTbanKey **removed,
                                       size_t *count);
int database_set_member(const struct DatabaseMember *member);
int database_set_member_watch(u64snowflake uid, u64snowflake gid, int watch);

int database_add_member_link(u64snowflake uid, u64snowflake gid,
                             u64snowflake link_uid);
int database_remove_member_link(u64snowflake uid, u64snowflake gid,
                                u64snowflake link_uid);
int database_list_member_links(u64snowflake uid, u64snowflake gid,
                               u64snowflake **links, size_t *count);

struct DatabaseGuildDat *database_get_guild_dat(u64snowflake gid);
struct DatabaseRule *database_get_rule(u64snowflake gid, int64_t rid);
struct DatabaseCase *database_get_case(int64_t id, u64snowflake gid);
struct DatabaseConf *database_get_conf(u64snowflake gid);
struct DatabaseTban *database_get_tban(u64snowflake gid, u64snowflake uid);

void database_free_guild_dat(struct DatabaseGuildDat *data);
void database_free_rule(struct DatabaseRule *rule);
void database_free_case(struct DatabaseCase *case_data);
void database_free_conf(struct DatabaseConf *data);
void database_free_tban(struct DatabaseTban *tban);

#endif // DATABASE_H
