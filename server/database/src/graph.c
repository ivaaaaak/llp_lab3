#include "../include/graph.h"


uint64_t calculate_node_data_size(const struct node node) {
    uint64_t data_size = 0;

    for (uint8_t i = 0; i < node.type.attribute_num; i++) {
        switch (node.attributes[i].type) {
            
            case INT32:
                data_size += sizeof(int32_t);
                break;

            case FLOAT:
                data_size += sizeof(float);
                break;

            case BOOL:
                data_size += sizeof(bool);
                break;

            case STRING:
                data_size += strlen(node.attributes[i].value.as_string) + 1;
                break;    
        }
    }
    return data_size;
}


struct node create_node_struct(struct node_type node_type) {
    struct node node = {
        .type = node_type,
        .attributes = malloc(sizeof(struct attribute) * node_type.attribute_num)
    };
    return node;
}


void destroy_node_struct(struct node* node) {
    
    for (size_t i = 0; i < node->type.attribute_num; i++) {

        if (node->attributes[i].type == STRING) {
            free(node->attributes[i].value.as_string);
        }
    }
    
    free(node->attributes);
}


bool compare_attributes(struct attribute attr1, struct attribute attr2, enum operation op) {

    switch (attr1.type) {
        case INT32:
            switch (op) {
                case EQ:
                    return attr1.value.as_int32 == attr2.value.as_int32;
                case NE:
                    return attr1.value.as_int32 != attr2.value.as_int32;
                case LT:
                    return attr1.value.as_int32 < attr2.value.as_int32;
                case GT:
                    return attr1.value.as_int32 > attr2.value.as_int32;
            }
        case FLOAT:
            switch (op) {
                case EQ:
                    return attr1.value.as_float == attr2.value.as_int32;
                case NE:
                    return attr1.value.as_float != attr2.value.as_int32;
                case LT:
                    return attr1.value.as_float < attr2.value.as_int32;
                case GT:
                    return attr1.value.as_float > attr2.value.as_int32;
            }
        case BOOL:
            switch (op) {
                case EQ:
                    return attr1.value.as_bool == attr2.value.as_bool;
                case NE:
                    return attr1.value.as_bool != attr2.value.as_bool;
            }           
        case STRING:
            switch (op) {
                case EQ:
                    return strcmp(attr1.value.as_string, attr2.value.as_string) == 0 ? true : false;
                case NE:
                     return strcmp(attr1.value.as_string, attr2.value.as_string) == 0 ? false : true;
            }   
        default:
            return false;
    }
}
