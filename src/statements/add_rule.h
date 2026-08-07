#ifndef ADD_RULE_H
#define ADD_RULE_H

#include "helper.h"

static const char *g_add_rule_stmt = MK_STMT(
INSERT INTO rules (title, desc, color, img) VALUES (?, ?, ?, ?);
);

#endif // ADD_RULE_H
