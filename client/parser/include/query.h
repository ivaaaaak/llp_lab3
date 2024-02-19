#ifndef QUERY_H
#define QUERY_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STRING_SIZE 100

enum query_type {
    CREATE_T,
    MATCH_T,
    MATCH_EDGE_T,
    SET_T,
    DELETE_T
};


enum value_type {
    BOOL,
    INT32,
    FLOAT,
    STRING
};

enum property_operation {
    EQUALS,
    NOT_EQUALS,
    LESS_THAN,
    GREATER_THAN,
    CONTAINS
};

struct value {
    enum value_type type;
    union {
        bool as_bool;
        int32_t as_int;
        float as_float;
        char as_string[MAX_STRING_SIZE];
    };
};


struct property {
    char name[MAX_STRING_SIZE];
    struct value value;
    struct property* next_prop;
};

enum logic_operation_type {
    NO_T,
    AND_T,
    OR_T
};

struct logic_operation {
    enum logic_operation_type operation;
    struct logic_operation* next_logic_operation;
};


struct filter {
    struct property property;
    enum property_operation prop_op;
    struct filter* next_filter;
};

struct conditions {
    struct filter* filter;
    struct logic_operation* log_op;
};

struct match_query {
    struct conditions cond;
    char return_value[MAX_STRING_SIZE];
};

struct delete_query {
    struct conditions cond;
    char delete_value[MAX_STRING_SIZE];
};

struct set_query {
    struct conditions cond;
    struct property* prop;
};


struct create_query {
    struct property* prop;
};


struct query {
    char var_name[MAX_STRING_SIZE];
    char label[MAX_STRING_SIZE];

    enum query_type type;
    union {
        struct create_query as_create;
        struct match_query as_match;
        struct set_query as_set;
        struct delete_query as_delete;
    };
};

struct value create_boolean(bool value);
struct value create_int(int32_t value);
struct value create_float(float value);
struct value create_string(char* value);

void set_query_type(struct query* q, char *type);
void set_var_name_and_label(struct query* q, char* var_name, char* label);
void set_new_property(struct query* q, char* name, struct value value, char* type);
void set_new_logical_operation(struct query* q, char* oper);
void set_new_filter(struct query* q, char* name, int operation, struct value value);
void set_delete_value(struct query* q, char* name);

#endif