#ifndef WARN_H
#define WARN_H

#include <concord/discord.h>

void warn(struct discord *client, const struct discord_interaction *event);

#endif // WARN_H
