#include "d20.h"

#include <stdlib.h>
#include "../random.h"

#include <string.h>

void d20(struct discord *client, const struct discord_interaction *event) {
  char *content;
  char allocated_content[28];
  int result = random_int(1, 20);
  switch (result) {
  case 20:
    content = ":star: Natural 20!";
    break;
  case 1:
    content = ":x: Critical 1!";
    break;
  default:
    snprintf(allocated_content, sizeof(allocated_content), ":game_die: You rolled a %d!", result);
    content = allocated_content;
    break;
  }
  struct discord_interaction_response params = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
    .data = &(struct discord_interaction_callback_data){
      .content = content
    }
  };
  discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}