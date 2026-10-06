#ifndef JOIN_LEAVE_LOG_H
#define JOIN_LEAVE_LOG_H

#include <concord/discord.h>

void join_leave_log(struct discord *client, const struct discord_interaction *event);

#endif // JOIN_LEAVE_LOG_H
