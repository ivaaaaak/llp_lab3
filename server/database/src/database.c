#include "../include/database.h"


int create_db(char* file_name, struct database* db) {
    FILE* file = fopen(file_name, "wb+");
    if (file != NULL) {
        db->file = file;
        db->last_page_num = 0;
        db->last_node_type_id = 0;
        db->first_free_space = NULL;
        return 0;
    }
    return 1;
}

int destroy_db(struct database* db) {
    fclose(db->file);

    struct free_space* fs = db->first_free_space;

    while (fs != NULL) {
        struct free_space* next_fs = fs->next_free_space;
        free(fs);
        fs = next_fs;
    }
        
    return 0;
}

void add_free_space_to_db(struct database* db, uint8_t type_id) {
    struct free_space* fs = malloc(sizeof(struct free_space));

    fs->type_id = type_id;
    fs->page_num = db->last_page_num;
    fs->next_free_space = NULL;

    if (db->first_free_space == NULL) {
        db->first_free_space = fs;
    } else {
        struct free_space* efs = db->first_free_space;
        while (efs->next_free_space != NULL) {
            efs = efs->next_free_space;
        }
        efs->next_free_space = fs;
    }
}

void remove_free_space_from_db(struct database* db, size_t page_num) {
    struct free_space* prev_fs = db->first_free_space;

    if (prev_fs->page_num == page_num) {
        db->first_free_space = NULL;
        return;
    }
    
    struct free_space* fs = prev_fs->next_free_space;

    while (prev_fs != NULL) {

        if (fs == NULL) {
            prev_fs->next_free_space = NULL;
            return;
        }

        if (fs->page_num == page_num) {
            prev_fs->next_free_space = fs->next_free_space;
            free(fs);
            return;
        }

        prev_fs = fs;
        fs = fs->next_free_space;
    }
}
 
size_t find_free_page_num(struct database* db, uint8_t type_id) {
    struct free_space* fs = db->first_free_space;

    while (fs != NULL) {

        if (fs->type_id == type_id) {
            return fs->page_num;
        }

        fs = fs->next_free_space;
    }

    return 0;
}

struct page create_new_node_page(struct database* db, struct node_type type) {
    db->last_page_num += 1;
    db->last_node_type_id += 1;
    add_free_space_to_db(db, type.id);

    struct page new_page = {

            .header = {
                    .is_occupied = true,
                    .page_num = db->last_page_num,
                    .next_same_page_num = 0,
                    .occupied_blocks_num = 0,
                    .occupied_blocks = {0}
            },

            .node_type = type,
            .blocks = {0}
    };

    return new_page;
}

struct page find_first_page(struct database* db, const char* type_name) {
    struct page page;

    for (size_t page_num = 1; page_num <= db->last_page_num; page_num++) {

        read_page(db->file, page_num, &page);

        if (page.header.is_occupied) {

            if (strcmp(page.node_type.name, type_name) == 0) {
                return page;
            }
        }
    }

    page.header.is_occupied = 0;
    return page;
}


struct page read_node_page_with_free_space(struct database* db, struct node_type type) {
    size_t page_num = find_free_page_num(db, type.id);

    if (page_num != 0) {
        struct page page;
        read_page(db->file, page_num, &page);
        return page;
    }

    return create_new_node_page(db, type); 
}


uint64_t add_node_block_to_page(struct database* db, struct node_type type, struct page* page, struct block block) {
    
    bool has_free_blocks = page->header.occupied_blocks_num < BLOCKS_NUM;

    if (!has_free_blocks)  {
        remove_free_space_from_db(db, page->header.page_num);
        if (write_page(db->file, page->header.page_num, *page) != 0) {
            return 0;
        }
        *page = read_node_page_with_free_space(db, type);
    }

    for (size_t i = 0; i < BLOCKS_NUM; i++) {
        if (!page->header.occupied_blocks[i]) {

            page->blocks[i] = block;
            page->header.occupied_blocks[i] = true;
            page->header.occupied_blocks_num += 1;

            return find_block_addr(page->header.page_num, i);
        }
    }
    return 0;
}


void fill_node_blocks(const struct node node, uint64_t data_size, size_t blocks_num, struct block* blocks) {

    size_t attr_num = 0;
    uint8_t *attr_ptr;
    size_t cur_attr_size = 0;
    size_t written_attr_size = 0;

    for (size_t i = 0; i < blocks_num; i++) {

        size_t block_data_size = data_size > BLOCK_DATA_SIZE? BLOCK_DATA_SIZE : data_size;
        data_size -= BLOCK_DATA_SIZE;

        struct block node_block = {0};
        if (i == 0) {
            node_block.info.is_additional = false;
            node_block.info.data_size = data_size + BLOCK_DATA_SIZE;
        } else {
            node_block.info.is_additional = true;
            node_block.info.data_size = block_data_size;
        }

        size_t filled_bytes_num = 0;

        while (attr_num < node.type.attribute_num && filled_bytes_num < block_data_size) {

            if (written_attr_size == 0) {

                switch (node.attributes[attr_num].type) {
                    
                    case STRING:
                        attr_ptr = (uint8_t *) node.attributes[attr_num].value.as_string;
                        cur_attr_size = strlen(node.attributes[attr_num].value.as_string) + 1;
                        break;
                    case BOOL:
                        attr_ptr = (uint8_t *) &node.attributes[attr_num].value;
                        cur_attr_size = sizeof(bool);
                        break;
                    default:
                        attr_ptr = (uint8_t *) &node.attributes[attr_num].value;
                        cur_attr_size = sizeof(int32_t);
                        break;
                }
            }

            while (written_attr_size < cur_attr_size && filled_bytes_num < block_data_size) {
                node_block.data[filled_bytes_num] = *attr_ptr;
                attr_ptr++;
                filled_bytes_num++;
                written_attr_size++;
            }

            if (written_attr_size == cur_attr_size) {
                attr_num++;
                written_attr_size = 0;
            }
        }
        blocks[i] = node_block;
    }
}


int write_node_blocks(struct database* db, struct node_type type, size_t blocks_num, struct block* blocks) {
    struct page page = read_node_page_with_free_space(db, type);
    
    uint64_t prev_block_addr = 0;

    for (size_t i = 0; i < blocks_num - 1; i++) {
        size_t j = blocks_num - 1 - i;
        blocks[j].info.additional_block_addr = prev_block_addr;

        prev_block_addr = add_node_block_to_page(db, type, &page, blocks[j]);
        if (prev_block_addr == 0) {
            return 1;
        }
    }

    blocks[0].info.additional_block_addr = prev_block_addr;
    if (add_node_block_to_page(db, type, &page, blocks[0]) == 0) {
        return 1;
    }

    return write_page(db->file, page.header.page_num, page);
}


void fill_node_attributes_from_buffer(const uint8_t* buffer, struct node* node) {
    size_t string_len;

    for (size_t i = 0; i < node->type.attribute_num; i++) {

        switch (node->type.attribute_types[i]) {

            case INT32:
                node->attributes[i].type = INT32;
                node->attributes[i].value.as_int32 = (int32_t) *buffer;
                buffer += sizeof(int32_t);
                break;

            case FLOAT:
                node->attributes[i].type = FLOAT;
                node->attributes[i].value.as_float = (float) *buffer;
                buffer += sizeof(float);
                break;

            case BOOL:
                node->attributes[i].type = BOOL;
                node->attributes[i].value.as_bool = (bool) *buffer;
                buffer += sizeof(bool);
                break;

            case STRING:
                node->attributes[i].type = STRING;
                string_len = strlen((char *) buffer) + 1;
                node->attributes[i].value.as_string = malloc(string_len);

                if (node->attributes[i].value.as_string) {
                    for (size_t j = 0; j < string_len; j++) {
                        node->attributes[i].value.as_string[j] = ((char *) buffer)[j];
                    }
                }
                buffer += string_len;
                break;
        }
    }
}


void read_attributes_to_buffer(struct database* db, struct block block, uint64_t buffer_size, uint8_t buffer[]) {
    size_t read_bytes_num = 0;

    while (read_bytes_num < buffer_size) {

        bool is_additional = block.info.is_additional;
        uint64_t block_data_size = is_additional? block.info.data_size : (buffer_size > BLOCK_DATA_SIZE? BLOCK_DATA_SIZE : buffer_size);

        for (size_t i = 0; i < block_data_size; i++) {
            buffer[read_bytes_num] = block.data[i];
            read_bytes_num++;
        }

        uint64_t next_block_addr = block.info.additional_block_addr;

        if (next_block_addr != 0) {
            read_block(db->file, next_block_addr, &block);
        }
    }
}


void fill_node_from_block(struct database* db, struct block block, struct node* node) {
    uint64_t all_data_size = block.info.data_size;
    uint8_t buffer[all_data_size];

    read_attributes_to_buffer(db, block, all_data_size, buffer);
    fill_node_attributes_from_buffer(buffer, node);
}


bool compare_node(struct conditions cond, struct node* node) {

    struct filter* f = cond.filter;
    struct logic_operation* lo = cond.log_op;

    while (f != NULL) {

        for (size_t j = 0; j < node->type.attribute_num; j++){

            if (strcmp(node->type.attribute_names[j], f->prop.attr_name) == 0) {

                bool is_equal = compare_attributes(node->attributes[j], f->prop.attr, f->operation);

                if (lo == NULL) {
                    return is_equal;

                } else if (lo->operation == AND && !is_equal) {
                    return false;

                } else if (lo->operation == OR && is_equal) {
                    return true;
                }

                break;
            }
            
        }
        f = f->next_filter;
        lo = lo->next_lo;
    }            
    return false;
}


void find_all_matches_on_page(struct database* db, struct page page, struct conditions cond, struct found_node* last_found_node) {
   
    for (size_t i = 0; i < BLOCKS_NUM; i++) {

        if (page.header.occupied_blocks[i] && !page.blocks[i].info.is_additional) {

            struct node node = create_node_struct(page.node_type);
            fill_node_from_block(db, page.blocks[i], &node);
            
            if (compare_node(cond, &node) || cond.filter == NULL) {

                if (last_found_node->has_found) {
                    struct found_node* new_f = malloc(sizeof(struct found_node));
                    new_f->has_found = true;
                    new_f->node = node;
                    new_f->next_node = NULL;

                    last_found_node->next_node = new_f;
                    last_found_node = new_f;

                } else {
                    last_found_node->has_found = true;
                    last_found_node->node = node;
                    last_found_node->next_node = NULL;
                }
                
            } else {
                destroy_node_struct(&node);
            }
        }
    }
}


size_t find_block_addr_with_attr(struct database* db, struct page page, struct conditions cond) {
    struct node node = create_node_struct(page.node_type);

    for (size_t i = 0; i < BLOCKS_NUM; i++) {

        if (page.header.occupied_blocks[i] && !page.blocks[i].info.is_additional) {
            fill_node_from_block(db, page.blocks[i], &node);
            
            if (compare_node(cond, &node)) {
                return find_block_addr(page.header.page_num, i);
            }
        }
    }
    return 0;
}


int delete_block_from_page(struct database* db, uint64_t block_addr) {
    struct page page;
   
    if (read_page(db->file, find_page_num_from_addr(block_addr), &page) != 0) {
        return 1;
    }

    page.header.occupied_blocks[find_block_num_from_addr(block_addr)] = false;
    page.header.occupied_blocks_num--;

    if (write_page(db->file, page.header.page_num, page) != 0) {
        return 1;
    }
    
    add_free_space_to_db(db, page.node_type.id);
    return 0;
}
