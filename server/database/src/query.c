#include "../include/query.h"


int create_node_type(struct database* db, const struct query query) {

    struct node_type type = {
        .id = db->last_node_type_id,
        .attribute_num = 0,
    };
    strcpy(type.name, query.node_type_name);

    struct property* prop = query.as_create.prop;

    while (prop != NULL) {
        strcpy(type.attribute_names[type.attribute_num], prop->attr_name);
        type.attribute_types[type.attribute_num] = prop->attr.type;
        type.attribute_num += 1;
        prop = prop->next_prop;
    }

    struct page new_page = create_new_node_page(db, type);
    return write_page(db->file, new_page.header.page_num, new_page);
}


struct query_result create_node(struct database* db, const struct query query) {

    struct page page = find_first_page(db, query.node_type_name);
    
    if (page.header.is_occupied == 0) {
        create_node_type(db, query);
        page = find_first_page(db, query.node_type_name);
    }

    struct node node = create_node_struct(page.node_type);
    struct property* prop = query.as_create.prop;

    while (prop != NULL) {
        for (size_t i = 0; i < node.type.attribute_num; i++) {
            if (strcmp(node.type.attribute_names[i], prop->attr_name) == 0) {
                node.attributes[i] = prop->attr;
            }
        }
        prop = prop->next_prop;
    }

    struct query_result res = {0};

    uint64_t data_size = calculate_node_data_size(node);

    size_t blocks_num = (data_size - 1) / BLOCK_DATA_SIZE + 1;
    struct block* blocks = malloc(sizeof(struct block) * blocks_num);

    if (!blocks) {
        strcpy(res.message, "Failed to allocate memory for blocks");
        return res;
    } 

    fill_node_blocks(node, data_size, blocks_num, blocks);

    if (write_node_blocks(db, node.type, blocks_num, blocks) != 0) {
        strcpy(res.message, "Failed to write blocks to database");
        return res;
    }

    free(blocks);

    strcpy(res.message, "Successfully created");
    return res;
}


struct query_result match_node(struct database* db, struct query query) {
    struct query_result res = {
        .found_node = malloc(sizeof(struct found_node))
    };
    res.found_node->has_found = false;
    res.found_node->next_node = NULL;

    struct page page = find_first_page(db, query.node_type_name);

    if (!page.header.is_occupied) {
        strcpy(res.message, "Node type does not exist");
        return res;
    }

    find_all_matches_on_page(db, page, query.as_match.cond, res.found_node);

    while (page.header.next_same_page_num != 0) {

        if (read_page(db->file, page.header.next_same_page_num, &page) != 0) {
            strcpy(res.message, "Failed to read next page");
            return res;
        }

        struct found_node* last_fn =  res.found_node;
        while (last_fn->next_node != NULL) {
            last_fn = last_fn->next_node;
        }
        
        find_all_matches_on_page(db, page, query.as_match.cond, last_fn);
    }

    if (res.found_node->has_found) {
        strcpy(res.message, "Successfully matched");
    } else {
        strcpy(res.message, "No matches");
    }

    return res;
}


struct query_result delete_node(struct database* db, struct query query) {
    struct query_result res = {0};

    struct page page = find_first_page(db, query.node_type_name);
    uint64_t block_addr = find_block_addr_with_attr(db, page, query.as_delete.cond);

    if (block_addr == 0) {
        strcpy(res.message, "No matches");
        return res;
    }

    struct block block;

    while (block_addr != 0) {

        if (read_block(db->file, block_addr, &block) != 0) {
            strcpy(res.message, "Failed to read next page");
            return res;
        }

        if (delete_block_from_page(db, block_addr) != 0) {
            strcpy(res.message, "Failed to delete block");
            return res;
        }
       
        block_addr = block.info.additional_block_addr;
    }

    strcpy(res.message, "Successfully deleted");
    return res;
}


struct query_result update_node(struct database* db, struct query query) {
    struct query_result up_res = {0};
    struct page page = find_first_page(db, query.node_type_name);

    struct query_result res = match_node(db, query);

    if (strcmp(res.message, "Successfully matched") != 0) {
        strcpy(up_res.message, "No matches");
        return up_res;
    }

    struct found_node* n = res.found_node;
    struct found_node* next_n = res.found_node;

    while (n != NULL) {

        res = delete_node(db, query);

        if (strcmp(res.message, "Successfully deleted") != 0) {
            strcpy(up_res.message, "Failed to delete old node");
            return up_res;
        }

        n = n->next_node;
    }

    while (next_n != NULL) {

        struct property* prop = query.as_set.prop;
        
        for (size_t j = 0; j < next_n->node.type.attribute_num; j++) {

            if (strcmp(next_n->node.type.attribute_names[j], prop->attr_name) != 0) {
                struct property* new_prop = malloc(sizeof(struct property));
                
                new_prop->attr = next_n->node.attributes[j];
                strcpy(new_prop->attr_name, next_n->node.type.attribute_names[j]);
                new_prop->next_prop = prop->next_prop;
    
                prop->next_prop = new_prop;
            }
        }

        struct query new_q = {
            .type = CREATE,
            .as_create.prop = prop
        };

        strcpy(new_q.node_type_name, query.node_type_name);

        res = create_node(db, new_q);
        
        if (strcmp(res.message, "Successfully created") != 0) {
            strcpy(up_res.message, "Failed to create new node");
            return up_res;
        }

        next_n = next_n->next_node;
    }

    strcpy(up_res.message, "Successfully updated");
    return up_res;
}
