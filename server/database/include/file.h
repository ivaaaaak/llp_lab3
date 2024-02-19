#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "graph.h"

#define PAGE_SIZE 4096

#define CONTENT_SIZE (PAGE_SIZE - sizeof(struct page_header) - sizeof(struct node_type))
#define BLOCKS_NUM 12
#define BLOCK_SIZE (CONTENT_SIZE / BLOCKS_NUM)
#define BLOCK_DATA_SIZE (BLOCK_SIZE - sizeof(struct block_info))


#pragma pack(push, 1)
struct page_header {
    uint8_t is_occupied;

    uint64_t page_num;
    uint64_t next_same_page_num;

    uint64_t occupied_blocks_num;
    uint8_t occupied_blocks[BLOCKS_NUM];
};

struct block_info {
    uint8_t is_additional;
    uint64_t additional_block_addr;
    uint64_t data_size;
};

struct block {
    struct block_info info;
    uint8_t data[BLOCK_DATA_SIZE];
};

struct page {
    struct page_header header;
    struct node_type node_type;
    struct block blocks[BLOCKS_NUM];
};
#pragma pack(pop)


int read_page(FILE* file, size_t page_num, struct page* page);
int write_page(FILE* file, size_t page_num, const struct page page);

int read_block(FILE* file, size_t block_addr, struct block* block);
int write_block(FILE* file, size_t block_addr, struct block block);

uint64_t find_block_addr(size_t page_num, size_t block_num);
size_t find_page_num_from_addr(uint64_t addr);
size_t find_block_num_from_addr(uint64_t block_addr);

#endif