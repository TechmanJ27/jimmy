#include "roll.h"

#include <stdlib.h>
#include "../random.h"

// TODO: Here
void roll(struct discord *client, const struct discord_interaction *event) {
  char *content;
  asprintf(&content, ":game_die: You rolled a %d!", random_int(1, 20));
  struct discord_interaction_response params = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
    .data = &(struct discord_interaction_callback_data){
      .content = content
    }
  };
  discord_create_interaction_response(client, event->id, event->token, &params, NULL);
  free(content);
}