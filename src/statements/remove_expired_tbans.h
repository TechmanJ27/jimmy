#ifndef REMOVE_EXPIRED_TBANS_H
#define REMOVE_EXPIRED_TBANS_H

#include "helper.h"

static const char *g_remove_expired_tbans_stmt = MK_STMT(
DELETE FROM tbans WHERE expire < ?;
);

#endif // REMOVE_EXPIRED_TBANS_H
