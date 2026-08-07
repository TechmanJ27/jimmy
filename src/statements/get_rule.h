#ifndef GET_RULE_H
#define GET_RULE_H

#include "helper.h"

static const char *g_get_rule_stmt = MK_STMT(
SELECT title, desc, color, img FROM rules WHERE rid = ?;
);

#endif // GET_RULE_H
