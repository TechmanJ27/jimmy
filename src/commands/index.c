#include "index.h"

#include "../macro.h"
#include "about.h"
#include "avatar.h"
#include "d20.h"
#include "ping.h"
#include "roll.h"

#include <concord/discord.h>

#define GLOBAL_COMMANDS(X) \
  X("avatar", "What's my profile picture again?", avatar, &g_avatar_options) \
  X("d20", "Seduce the dragon!", d20, NULL) \
  X("roll", "Where is my dice and coin?", roll, &g_roll_options)

#define GENERAL_COMMANDS(X) \
  X("ping", "Hello?? Are you alive?", ping, NULL) \
  X("about", "The great Jimmy shall introduce.", about, NULL)

#define COMMAND_ENTRY(command_name, command_description, command_handler, command_options) \
  { \
    .name = command_name, \
    .description = command_description, \
    .options = command_options, \
    .run = command_handler \
  },

#define COMMAND_OPTION(command_name, command_description, command_handler, command_options) \
  { \
    .type = DISCORD_APPLICATION_OPTION_SUB_COMMAND, \
    .name = command_name, \
    .description = command_description, \
    .options = command_options \
  },

#define DISCORD_COMMAND_ENTRY(command_name, command_description, command_handler, command_options) \
  { \
    .name = command_name, \
    .description = command_description, \
    .options = command_options \
  },

static struct discord_application_command_option g_avatar_arguments[] = {
  {
    .type = DISCORD_APPLICATION_OPTION_USER,
    .name = "user",
    .description = "User to get the avatar of. (default: yourself)"
  }
};

static struct discord_application_command_options g_avatar_options = {
  .size = 1,
  .array = g_avatar_arguments
};

static struct discord_application_command_option g_roll_arguments[] = {
  {
    .type = DISCORD_APPLICATION_OPTION_INTEGER,
    .name = "amount",
    .description = "Number of dice/coin. (1-" TO_STR(MAX_ROLL_SIDES) " default: 1)"
  },
  {
    .type = DISCORD_APPLICATION_OPTION_INTEGER,
    .name = "sides",
    .description = "Number of sides. Pick 2 sides for coin flip. (2-" TO_STR(MAX_ROLL_AMOUNT) " default: 6)"
  }
};

static struct discord_application_command_options g_roll_options = {
  .size = 2,
  .array = g_roll_arguments
};

const Command G_GLOBAL_COMMANDS_LIST[] = {
  GLOBAL_COMMANDS(COMMAND_ENTRY)
};

const size_t G_GLOBAL_COMMANDS_COUNT = sizeof(G_GLOBAL_COMMANDS_LIST) / sizeof(*G_GLOBAL_COMMANDS_LIST);

const Command G_GENERAL_COMMANDS_LIST[] = {
  GENERAL_COMMANDS(COMMAND_ENTRY)
};

const size_t G_GENERAL_COMMANDS_COUNT = sizeof(G_GENERAL_COMMANDS_LIST) / sizeof(*G_GENERAL_COMMANDS_LIST);

static struct discord_application_command_option g_general_command_options[] = {
  GENERAL_COMMANDS(COMMAND_OPTION)
};

static struct discord_application_command_options g_command_options_list = {
  .size = sizeof(g_general_command_options) / sizeof(*g_general_command_options),
  .array = g_general_command_options
};

struct discord_create_guild_application_command g_commands[] = {
  GLOBAL_COMMANDS(DISCORD_COMMAND_ENTRY)
    GENERAL_COMMANDS(DISCORD_COMMAND_ENTRY)
};

const size_t G_APPLICATION_COMMANDS_COUNT = sizeof(g_commands) / sizeof(*g_commands);
