#include "avatar.h"

#include <concord/discord.h>
#include "../colors.h"

#include <stdlib.h>

static void send_error(struct discord *client, const struct discord_interaction *event) {
  struct discord_interaction_response params = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
    .data = &(struct discord_interaction_callback_data){
      .content = "I think I messed up and couldn't find that user."
    }
  };
  discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}

void avatar(struct discord *client, const struct discord_interaction *event) {
  struct discord_guild_member fetched_user = {0};
  struct discord_ret_guild_member user = {
    .sync = &fetched_user,
  };
  struct discord_guild_member user_data = *event->member;
  if (event->data->options->size != 0) {
    CCORDcode ret = discord_get_guild_member(client, event->guild_id, strtol(event->data->options->array[0].value, NULL, 10), &user);
    if (ret != CCORD_OK) {
      send_error(client, event);
      return;
    }
    user_data = fetched_user;
  }

  char *avatar_url;
  int asprintf_ret = asprintf(&avatar_url,
    "https://cdn.discordapp.com/avatars/%lu/%s.png?size=1024",
    user_data.user->id, user_data.user->avatar);

  if (asprintf_ret == -1) {
    perror("Error calling asprintf");
    send_error(client, event);
    return;
  }

  const struct discord_embed EMBEDS[1] = {
    {
      .title = user_data.user->username,
      .color = COLOR_BLUE,
      .image =
          &(struct discord_embed_image){
            .url = avatar_url,
        },
    },
  };

  struct discord_interaction_response params = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
    .data = &(struct discord_interaction_callback_data){
      .embeds = &(struct discord_embeds){
        .size = 1,
        .array = (struct discord_embed*) EMBEDS,
    }
    }
  };

  discord_create_interaction_response(client, event->id, event->token, &params, NULL);
  free(avatar_url);
}