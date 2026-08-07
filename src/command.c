#include "command.h"

#include "commands/index.h"

#include <concord/discord.h>

#include <string.h>

void deploy_commands(struct discord *client, u64snowflake application_id, u64snowflake guild_id) {
  for (size_t i = 0; i < G_APPLICATION_COMMANDS_COUNT; i++) {
    discord_create_guild_application_command(client, application_id, guild_id, &g_commands[i], NULL);
  }
}

static const Command *find_command_in_list(const char *name, const Command *commands,
                                           size_t count) {
  for (size_t i = 0; i < count; i++) {
    if (strcmp(name, commands[i].name) == 0) {
      return &commands[i];
    }
  }

  return NULL;
}

static const Command *find_command_by_name(const char *name) {
  if (name == NULL) {
    return NULL;
  }

  const Command *command =
      find_command_in_list(name, G_GLOBAL_COMMANDS_LIST, G_GLOBAL_COMMANDS_COUNT);
  if (command == NULL) {
    command = find_command_in_list(name, G_GENERAL_COMMANDS_LIST, G_GENERAL_COMMANDS_COUNT);
  }
  return command;
}

static const Command *find_command(
    const struct discord_application_command_interaction_data_options *options) {
  if (options == NULL || options->size <= 0 || options->array == NULL) {
    return NULL;
  }

  const struct discord_application_command_interaction_data_option *option =
      &options->array[0];

  if (option->type == DISCORD_APPLICATION_OPTION_SUB_COMMAND_GROUP) {
    return find_command(option->options);
  }

  if (option->type == DISCORD_APPLICATION_OPTION_SUB_COMMAND) {
    return find_command_by_name(option->name);
  }

  return NULL;
}

void on_interaction(struct discord *client, const struct discord_interaction *event) {
  if (event == NULL || event->type != DISCORD_INTERACTION_APPLICATION_COMMAND ||
      event->data == NULL) {
    return;
  }

  const Command *command = find_command(event->data->options);
  if (command == NULL) {
    command = find_command_by_name(event->data->name);
  }
  if (command != NULL && command->run != NULL) {
    command->run(client, event);
  }
}
