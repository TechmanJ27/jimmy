#include "ban.h"

#include "../database.h"

#include <concord/discord.h>
#include <stdlib.h>
#include <string.h>

static void send_error(struct discord *client, const struct discord_interaction *event) {
  struct discord_interaction_response params = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
    .data = &(struct discord_interaction_callback_data){
      .content = "For some unknown reasons, I couldn't ban the user."
    }
  };
  discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}

void ban(struct discord *client, const struct discord_interaction *event) {
  u64snowflake user_to_ban = 0;
  char *rules_reason = NULL;
  char *duration_str = NULL;
  bool delete_messages = false;

  const struct discord_application_command_interaction_data_options *options =
  event->data->options;
  for (size_t i = 0; options != NULL && i < options->size; i++) {
    const struct discord_application_command_interaction_data_option *option =
      &options->array[i];

    if (strcmp(option->name, "member") == 0) {
      user_to_ban = strtol(option->value, NULL, 10);
    } else if (strcmp(option->name, "rules") == 0) {
      rules_reason = option->value;
    } else if (strcmp(option->name, "duration") == 0) {
      duration_str = option->value;
    } else if (strcmp(option->name, "delete_messages") == 0) {
      delete_messages = strcmp(option->value, "True") == 0;
    }
  }

  if (user_to_ban == 0) {
    send_error(client, event);
  }

  struct discord_create_guild_ban ban_params;

  

  if (delete_messages) {
    ban_params.delete_message_days = 1;
  }

  discord_create_guild_ban(client, event->guild_id, user_to_ban, &ban_params, NULL);
}