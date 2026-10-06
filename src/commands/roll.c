#include "roll.h"

#include <stdlib.h>
#include "../random.h"

#define LOGMOD_HEADER
#include <concord/logmod.h>
#include <string.h>

#define MAX_ROLL_MESSAGE_LEN 1024

static void free_response_buffer(struct discord *client, void *data) {
  free(data);
}

// Big and complex thing for no reason
void roll(struct discord *client, const struct discord_interaction *event) {
  int amount = 1;
  int sides = 6;

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

  char *content = 0;
  char *allocated_content = calloc(MAX_ROLL_MESSAGE_LEN, 1);
  if (allocated_content == NULL) {
    logmod_log(ERROR, NULL, "Unable to allocate roll response buffer");
    return;
  }
  int rolls[MAX_ROLL_AMOUNT];
  size_t allocated_content_current_len = 0;

  if (amount > MAX_ROLL_AMOUNT) {
    content = "Hey hey hey!! That's too many dice! I don't have that many dice.";
    goto send;
  }
  if (amount < 1) {
    content = "What are you trying to roll??";
    goto send;
  }
  if (sides > MAX_ROLL_SIDES || sides <= 1) {
    content = "I would have rolled that if I had had those dice...";
    goto send;
  }

  if (sides == 2) {
    content = allocated_content;

    char *coin_prefix;
    if (amount > 1) {
      coin_prefix = "> **";
    } else {
      coin_prefix = ":coin: You got **";
    }

    char *heads_str;
    char *tails_str;

    if (amount == 1) {
      heads_str = "heads";
      tails_str = "tails";
      snprintf(allocated_content, MAX_ROLL_MESSAGE_LEN,
        ":coin: You got **%s**!", random_int(1, 2) == 1 ? heads_str : tails_str);
      content = allocated_content;
      goto send;
    } else {
      heads_str = "H";
      tails_str = "T";
    }

    int heads_count = 0;
    int tails_count = 0;

    for (int i = 0; i < amount; i++) {
      rolls[i] = random_int(1, 2);

      if (rolls[i] == 1) {
        heads_count++;
      }
      if (rolls[i] == 2) {
        tails_count++;
      }

      int written = snprintf(
        allocated_content + allocated_content_current_len,
        MAX_ROLL_MESSAGE_LEN - allocated_content_current_len,
        "%s%s",
        i == 0 ? coin_prefix : ", ",
        rolls[i] == 1 ? heads_str : tails_str
      );
      if (written < 0) {
        content = "I lost my coins... Oops! Try again?";
        goto send;
      }

      size_t remaining = MAX_ROLL_MESSAGE_LEN - allocated_content_current_len;
      if ((size_t)written >= remaining) {
        allocated_content_current_len = MAX_ROLL_MESSAGE_LEN;
        break;
      }
      allocated_content_current_len += (size_t)written;
    }

    snprintf(allocated_content + allocated_content_current_len,
      MAX_ROLL_MESSAGE_LEN - allocated_content_current_len,
      "**\n:coin: total: **%d** | heads: **%d** | tails: **%d**", amount, heads_count, tails_count);

    goto send;
  }

  const char *prefix = "";
  if (amount > 1) {
    prefix = "> **";
  } else {
    const int RESULT = random_int(1, sides);
    snprintf(allocated_content, MAX_ROLL_MESSAGE_LEN,
      ":game_die: You got **%d**!", RESULT);
    content = allocated_content;
    goto send;
  }

  content = allocated_content;
  for (int i = 0; i < amount; i++) {
    rolls[i] = random_int(1, sides);
    int written = snprintf(
      allocated_content + allocated_content_current_len,
      MAX_ROLL_MESSAGE_LEN - allocated_content_current_len,
      "%s%d",
      i == 0 ? prefix : ", ",
      rolls[i]
    );
    if (written < 0) {
      content = "The die just disappeared when I was counting them! Maybe we could try again?";
      goto send;
    }

    size_t remaining = MAX_ROLL_MESSAGE_LEN - allocated_content_current_len;
    if ((size_t)written >= remaining) {
      allocated_content_current_len = MAX_ROLL_MESSAGE_LEN;
      break;
    }
    allocated_content_current_len += (size_t)written;
  }

  int total = 0;
  int min = MAX_ROLL_SIDES;
  int max = 0;
  int avg = 0;
  for (int i = amount - 1; i >= 0; i--) {
    total += rolls[i];
    if (rolls[i] < min) {
      min = rolls[i];
    }
    if (rolls[i] > max) {
      max = rolls[i];
    }
  }
  avg = total / amount;

  snprintf(allocated_content + allocated_content_current_len,
    MAX_ROLL_MESSAGE_LEN - allocated_content_current_len,
    "**\n:game_die: total: **%d** | min: **%d** | max: **%d** | avg: **%d** | **%dd%d**", total, min, max, avg, amount, sides);

  send:;

  struct discord_interaction_response params = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
    .data = &(struct discord_interaction_callback_data){
      .content = content
    }
  };

  struct discord_ret_interaction_response response = {
    .data = allocated_content,
    .cleanup = free_response_buffer,
  };

  discord_create_interaction_response(client, event->id, event->token, &params, &response);
}
