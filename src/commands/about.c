#include "about.h"

#include "../colors.h"
#include "../images.h"

#include <concord/discord.h>

static const struct discord_embed_field G_FIELDS[] = {
  {
    .name = "What is Jimmy?",
    .value = "Jimmy is a multipurpose open source Discord bot written in C by TheMonHub"
             "using [Concord](https://github.com/Cogmasters/concord)."
  },
  {
    .name = "Version",
    .value = PROJECT_VERSION,
  },
  {
    .name = "Source code",
    .value = "https://github.com/TheMonHub/jimmy"
  },
};

static const struct discord_embed G_EMBEDS[] = {
  {
    .title = "About",
    .image =
        &(struct discord_embed_image){
          .url = JIMMY_IMAGE_URL,
      },
    .description = "A Discord bot that will conquer the world one day...",
    .color = COLOR_BLUE,
    .footer =
        &(struct discord_embed_footer) {
          .text = "Powered by Concord",
          .icon_url = CONCORD_ICON_URL
      },
.fields =
  &(struct discord_embed_fields) {
    .size = sizeof(G_FIELDS) / sizeof *G_FIELDS,
    .array = (struct discord_embed_field*) G_FIELDS,
},
},
};

static struct discord_interaction_response g_params = {
  .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
  .data = &(struct discord_interaction_callback_data){
    .embeds = &(struct discord_embeds){
      .size = sizeof(G_EMBEDS) / sizeof *G_EMBEDS,
      .array = (struct discord_embed*) G_EMBEDS,
  }
  }
};

void about(struct discord *client, const struct discord_interaction *event) {
  discord_create_interaction_response(client, event->id, event->token, &g_params, NULL);
}