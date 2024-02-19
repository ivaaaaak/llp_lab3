#ifndef QUERY_H
#define QUERY_H

#include "database.h"

enum query_type {
    MATCH,
    CREATE,
    DELETE,
    SET
};


struct match_query {
    struct conditions cond;
};

struct create_query {
    struct property* prop;
};

struct delete_query {
    struct conditions cond;
};

struct set_query {
    struct conditions cond;
    struct property* prop;
};

struct query {
    enum query_type type;
    char node_type_name[MAX_NAME_SIZE];
    union {
        struct match_query as_match;
        struct create_query as_create;
        struct delete_query as_delete;
        struct set_query as_set;
    };
};

struct query_result {
    char message[50];
    struct found_node* found_node;
};

struct query_result create_node(struct database* db, struct query query);
struct query_result match_node(struct database* db, struct query query);
struct query_result update_node(struct database* db, struct query query);
struct query_result delete_node(struct database* db, struct query query);

#endif
