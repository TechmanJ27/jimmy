#include "ban.h"

#include <concord/discord.h>
#include <string.h>

void ban(struct discord *client, const struct discord_interaction *event) {
  struct discord_guild_member user_data = *event->member;

  const struct discord_application_command_interaction_data_options *options =
  event->data->options;
  for (size_t i = 0; options != NULL && i < options->size; i++) {
    const struct discord_application_command_interaction_data_option *option =
      &options->array[i];

    if (strcmp(option->name, "amount") == 0) {
      amount = strtol(option->value, NULL, 10);
    } else if (strcmp(option->name, "sides") == 0) {
      sides = strtol(option->value, NULL, 10);
    }
  }
}