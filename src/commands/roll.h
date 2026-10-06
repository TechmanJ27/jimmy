#ifndef ROLL_H
#define ROLL_H

#define MAX_ROLL_AMOUNT 100
#define MAX_ROLL_SIDES 100

#include <concord/discord.h>

void roll(struct discord *client, const struct discord_interaction *event);

#endif // ROLL_H
