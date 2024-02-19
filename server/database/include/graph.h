#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_SIZE 30
#define MAX_ATTR_NUM 20

#define INT32 0
#define FLOAT 1
#define BOOL 2
#define STRING 3

#define NODE_TYPE 0
#define EDGE_TYPE 1


struct node_type {
    uint8_t id;
    char name[MAX_NAME_SIZE];

    uint8_t attribute_num;
    uint8_t attribute_types[MAX_ATTR_NUM];
    char attribute_names[MAX_ATTR_NUM][MAX_NAME_SIZE];
};


struct node {
    struct node_type type;
    struct attribute* attributes;
};


struct attribute {
    uint8_t type;
    union {
        int32_t as_int32;
        float as_float;
        bool as_bool;
        char* as_string;
    } value;
};

enum operation {
    EQ,
    NE,
    LT,
    GT
};


uint64_t calculate_node_data_size(struct node node);
struct node create_node_struct(struct node_type node_type);
void destroy_node_struct(struct node* node);
bool compare_attributes(struct attribute attr1, struct attribute attr2, enum operation op);

#endif
