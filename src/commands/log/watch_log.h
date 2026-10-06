#ifndef WATCH_LOG_H
#define WATCH_LOG_H

#include <concord/discord.h>

void watch_log(struct discord *client, const struct discord_interaction *event);

#endif // WATCH_LOG_H
