#ifndef SET_VERSION_H
#define SET_VERSION_H

#include "helper.h"

#define DATABASE_CURRENT_VERSION 1
#define DATABASE_STRINGIFY_VALUE(value) #value
#define DATABASE_STRINGIFY(value) DATABASE_STRINGIFY_VALUE(value)

static const char *g_set_version_stmt =
    "PRAGMA user_version = " DATABASE_STRINGIFY(DATABASE_CURRENT_VERSION) ";";

#endif // SET_VERSION_H
