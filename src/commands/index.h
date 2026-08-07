#ifndef INDEX_H
#define INDEX_H

#include <concord/discord.h>

#include <stddef.h>

typedef void (*cmd_fn)(struct discord *client, const struct discord_interaction *event);

typedef struct {
  const char *name;
  const char *description;
  struct discord_application_command_options *options;
  cmd_fn run;
} Command;

extern const Command G_GLOBAL_COMMANDS_LIST[];
extern const size_t G_GLOBAL_COMMANDS_COUNT;

extern const Command G_GENERAL_COMMANDS_LIST[];
extern const size_t G_GENERAL_COMMANDS_COUNT;

extern struct discord_create_guild_application_command g_commands[];
extern const size_t G_APPLICATION_COMMANDS_COUNT;

#endif // INDEX_H
