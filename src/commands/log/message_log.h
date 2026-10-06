#ifndef MESSAGE_LOG_H
#define MESSAGE_LOG_H

#include <concord/discord.h>

void message_log(struct discord *client, const struct discord_interaction *event);

#endif // MESSAGE_LOG_H
