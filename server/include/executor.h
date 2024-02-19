#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../database/include/query.h"

struct query_result execute_query(struct database* db, struct query q);

#endif