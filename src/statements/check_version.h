#ifndef CHECK_VERSION_H
#define CHECK_VERSION_H
#include "helper.h"

static const char *g_check_version_stmt = MK_STMT(
PRAGMA user_version;
);

#endif // CHECK_VERSION_H
