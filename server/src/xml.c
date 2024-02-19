#include "../include/xml.h"


void set_query_type(xmlNodePtr query_node, struct query* query) {
    if (strcmp(query_node->name, "MATCH_QUERY") == 0) {
        query->type = MATCH;

    } else if (strcmp(query_node->name, "CREATE_QUERY") == 0) {
        query->type = CREATE;

    } else if (strcmp(query_node->name, "DELETE_QUERY") == 0) {
        query->type = DELETE;

    } else if (strcmp(query_node->name, "SET_QUERY") == 0) {
        query->type = SET;
    }
}

struct attribute get_attribute_from_value_node(xmlNodePtr value_node) {
    struct attribute attr = {0};

    if (strcmp(value_node->name, "INTEGER") == 0) {
        attr.type = INT32;
        attr.value.as_int32 = atoi((char *) value_node->children->content);
        
    } else if (strcmp(value_node->name, "STRING") == 0) {
        attr.type = STRING;
        attr.value.as_string = malloc(strlen(value_node->children->content) + 1);
        strcpy(attr.value.as_string, value_node->children->content);
        
    } else if (strcmp(value_node->name, "FLOAT") == 0) {
        attr.type = FLOAT;
        attr.value.as_float = atof((char *) value_node->children->content);
        
    } else if (strcmp(value_node->name, "BOOLEAN") == 0) {
        attr.type = BOOL;
        attr.value.as_bool = strcmp((char *) value_node->children->content, "true") == 0;
    }

    return attr;
}

void process_cond_node(xmlNodePtr cond_node, struct query* query) {

    struct filter* f;
    struct logic_operation* lo;

    if (strcmp(cond_node->name, "EQ") == 0) {
        struct filter* new_f = malloc(sizeof(struct filter));
        
        new_f->operation = EQ;
        strcpy(new_f->prop.attr_name, cond_node->children->children->content);
        new_f->prop.attr = get_attribute_from_value_node(cond_node->children->next);
        new_f->next_filter = NULL;

        if (query->as_match.cond.filter == NULL) {
            query->as_match.cond.filter = new_f;
        } else {
            f = query->as_match.cond.filter;
            while (f->next_filter != NULL) {
                f = f->next_filter;
            }
            f->next_filter = new_f;
        }

    } else if (strcmp(cond_node->name, "NE") == 0) {
        struct filter* new_f = malloc(sizeof(struct filter));

        new_f->operation = NE;
        strcpy(new_f->prop.attr_name, cond_node->children->children->content);
        new_f->prop.attr = get_attribute_from_value_node(cond_node->children->next);
        new_f->next_filter = NULL;

        if (query->as_match.cond.filter == NULL) {
            query->as_match.cond.filter = new_f;
        } else {
            f = query->as_match.cond.filter;
            while (f->next_filter != NULL) {
                f = f->next_filter;
            }
            f->next_filter = new_f;
        }
        
    } else if (strcmp(cond_node->name, "GT") == 0) {
        struct filter* new_f = malloc(sizeof(struct filter));

        new_f->operation = GT;
        strcpy(new_f->prop.attr_name, cond_node->children->children->content);
        new_f->prop.attr = get_attribute_from_value_node(cond_node->children->next);
        new_f->next_filter = NULL;

        if (query->as_match.cond.filter == NULL) {
            query->as_match.cond.filter = new_f;
        } else {
            f = query->as_match.cond.filter;
            while (f->next_filter != NULL) {
                f = f->next_filter;
            }
            f->next_filter = new_f;
        }
        
    } else if (strcmp(cond_node->name, "LT") == 0) {
        struct filter* new_f = malloc(sizeof(struct filter));

        new_f->operation = LT;
        strcpy(new_f->prop.attr_name, cond_node->children->children->content);
        new_f->prop.attr = get_attribute_from_value_node(cond_node->children->next);
        new_f->next_filter = NULL;

        if (query->as_match.cond.filter == NULL) {
            query->as_match.cond.filter = new_f;
        } else {
            f = query->as_match.cond.filter;
            while (f->next_filter != NULL) {
                f = f->next_filter;
            }
            f->next_filter = new_f;
        }

    } else if (strcmp(cond_node->name, "AND") == 0) {

        struct logic_operation* new_lo = malloc(sizeof(struct logic_operation));
        new_lo->operation = AND;
        new_lo->next_lo = NULL;

        if (query->as_match.cond.log_op == NULL) {
            query->as_match.cond.log_op = new_lo;
        } else {
            lo = query->as_match.cond.log_op;
            while (lo->next_lo != NULL) {
                 lo = lo->next_lo;
            }
            lo->next_lo = new_lo;
        }

        cond_node = cond_node->children;
        process_cond_node(cond_node, query);
        process_cond_node(cond_node->next, query);

    } else if (strcmp(cond_node->name, "OR") == 0) {

        struct logic_operation* new_lo = malloc(sizeof(struct logic_operation));
        new_lo->operation = OR;
        new_lo->next_lo = NULL;

        if (query->as_match.cond.log_op == NULL) {
            query->as_match.cond.log_op = new_lo;
        } else {
            lo = query->as_match.cond.log_op;
            while (lo->next_lo != NULL) {
                 lo = lo->next_lo;
            }
            lo->next_lo = new_lo;
        }
        
        cond_node = cond_node->children;
        process_cond_node(cond_node, query);
        process_cond_node(cond_node->next, query);
    }
}

void set_conditions(xmlNodePtr where_node, struct query* query) {
    xmlNodePtr cond_node = where_node->children;
    process_cond_node(cond_node, query);
}

void set_properties(xmlNodePtr field_set_node, struct query* query) {
    xmlNodePtr attr_name = field_set_node->children;

    while (attr_name != NULL) {

        struct property* prop;
        struct property* new_prop = malloc(sizeof(struct property));

        strcpy(new_prop->attr_name, attr_name->children->content);
        new_prop->attr = get_attribute_from_value_node(attr_name->next);
        new_prop->next_prop = NULL;

        switch(query->type) {
            case CREATE:
                if (query->as_create.prop == NULL) {
                    query->as_create.prop = new_prop;

                } else {
                    prop = query->as_create.prop;
                    while (prop->next_prop != NULL) {
                        prop = prop->next_prop;
                    }
                    prop->next_prop = new_prop;
                }
                break;

            case SET:
                if (query->as_set.prop == NULL) {
                    query->as_set.prop = new_prop;
                    
                } else {
                    prop = query->as_set.prop;
                    while (prop->next_prop != NULL) {
                        prop = prop->next_prop;
                    }
                    prop->next_prop = new_prop;
                }
                break;
        }

        attr_name = attr_name->next->next;
    }
}

void set_query_arguments(xmlNodePtr query_node, struct query* query) {
    if (query_node) {
        switch(query->type) {
            case MATCH:
                if (strcmp(query_node->name, "WHERE") == 0) {
                    set_conditions(query_node, query);
                }
                break;

            case CREATE:
                set_properties(query_node, query);
                break;

            case DELETE:
                if (strcmp(query_node->name, "WHERE") == 0) {
                    set_conditions(query_node, query);
                }
                break;

            case SET:
                if (strcmp(query_node->name, "WHERE") == 0) {
                    set_conditions(query_node, query);
                    query_node = query_node->next;
                }
                set_properties(query_node, query);
                
                break;
        }
    }
}

int parse_query_from_buffer(char* buffer, struct query* query) {
    xmlDocPtr doc = xmlReadMemory(buffer, strlen(buffer), NULL, NULL, 0);

    if (doc == NULL) {
        fprintf(stderr, "Failed to parse XML from memory.\n");
        return 1;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(doc);
    xmlNodePtr query_node = root_node->children;

    set_query_type(query_node, query);

    query_node = query_node->children;
    xmlAttrPtr attribute = query_node->properties;
    strcpy(query->node_type_name, xmlNodeGetContent(attribute->children));

    set_query_arguments(query_node->next, query);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}

xmlDocPtr build_response(struct query_result result) {

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "result");
    xmlDocSetRootElement(doc, root_node);
    xmlNewChild(root_node, NULL, BAD_CAST "message", BAD_CAST (result.message));

    struct found_node* fnode = result.found_node;
    char attr_buffer[50];

    while (fnode != NULL) {

        if (fnode->has_found) {

            xmlNodePtr fode = xmlNewChild(root_node, NULL, BAD_CAST "node", NULL);

            for (size_t i = 0; i < fnode->node.type.attribute_num; i++) {
    
                xmlNewChild(fode, NULL, BAD_CAST "attr_name", BAD_CAST (fnode->node.type.attribute_names[i]));

                switch (fnode->node.attributes[i].type) {
                    case INT32:
                        snprintf(attr_buffer, sizeof(attr_buffer), "%d", fnode->node.attributes[i].value.as_int32);
                        xmlNewChild(fode, NULL, BAD_CAST "attr_value", BAD_CAST (attr_buffer));
                        break;
                    case FLOAT:
                        snprintf(attr_buffer, sizeof(attr_buffer), "%f", fnode->node.attributes[i].value.as_float);
                        xmlNewChild(fode, NULL, BAD_CAST "attr_value", BAD_CAST (attr_buffer));
                        break;
                    case STRING:
                        xmlNewChild(fode, NULL, BAD_CAST "attr_value", BAD_CAST (fnode->node.attributes[i].value.as_string));
                        break;
                    case BOOL:
                        xmlNewChild(fode, NULL, BAD_CAST "attr_value", BAD_CAST (fnode->node.attributes[i].value.as_bool ? "true": "false"));
                        break;
                }
            }
        }

        fnode = fnode->next_node;
    }

    return doc;
}

bool validate_request(const char *buffer) {
    xmlDocPtr doc = xmlReadMemory(buffer, strlen(buffer), NULL, NULL, 0);
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewParserCtxt(SCHEMA_FILE_PATH);
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    int result = xmlSchemaValidateDoc(valid_ctxt, doc);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return result == 0;
}