#ifndef DATABASE_H
#define DATABASE_H

#include "file.h"

struct database {
    FILE* file;
    size_t last_page_num;
    size_t last_node_type_id;
    struct free_space* first_free_space;
};

struct free_space {
    uint8_t type_id;
    size_t page_num;
    struct free_space* next_free_space;
};


struct found_node {
    bool has_found;
    struct node node;
    struct found_node* next_node;
};

struct property {
    char attr_name[MAX_NAME_SIZE];
    struct attribute attr;

    struct property* next_prop;
};

struct filter {
    struct property prop;
    enum operation operation;

    struct filter* next_filter;
};

enum logic_operation_type {
    AND,
    OR
};

struct logic_operation {
    enum logic_operation_type operation;

    struct logic_operation* next_lo;
};

struct conditions {
    struct filter* filter;
    struct logic_operation* log_op;
};


int create_db(char* file_name, struct database* db);
int destroy_db(struct database* db);

struct page create_new_node_page(struct database* db, struct node_type type);
struct page find_first_page(struct database* db, const char* type_name);

void fill_node_blocks(struct node node, uint64_t data_size, size_t blocks_num, struct block* blocks);
int write_node_blocks(struct database* db, struct node_type type, size_t blocks_num, struct block* blocks);

size_t find_block_addr_with_attr(struct database* db, struct page page, struct conditions cond);
void find_all_matches_on_page(struct database* db, struct page page, struct conditions cond, struct found_node* found_node);

int delete_block_from_page(struct database* db, uint64_t block_addr);


#endif

