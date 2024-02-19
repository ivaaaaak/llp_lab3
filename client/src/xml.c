#include "../include/xml.h"

xmlNodePtr set_query_node(xmlNodePtr root_node, struct query q) {
    switch (q.type) {
        case MATCH_T:
            return xmlNewChild(root_node, NULL, BAD_CAST "MATCH_QUERY", NULL);
        case CREATE_T:
            return xmlNewChild(root_node, NULL, BAD_CAST "CREATE_QUERY", NULL);
        case SET_T:
            return xmlNewChild(root_node, NULL, BAD_CAST "SET_QUERY", NULL);
        case DELETE_T:
            return xmlNewChild(root_node, NULL, BAD_CAST "DELETE_QUERY", NULL);
    }
}

void set_property(xmlNodePtr parent, struct property property) {

    xmlNewChild(parent, NULL, BAD_CAST "IDENTIFIER", property.name);

    char attr_buffer[50];

    switch (property.value.type) {

        case INT32:
            snprintf(attr_buffer, sizeof(attr_buffer), "%d", property.value.as_int);
            xmlNewChild(parent, NULL, BAD_CAST "INTEGER", BAD_CAST (attr_buffer));
            break;
        case FLOAT:
            snprintf(attr_buffer, sizeof(attr_buffer), "%f", property.value.as_float);
            xmlNewChild(parent, NULL, BAD_CAST "FLOAT", BAD_CAST (attr_buffer));
            break;
        case STRING:
            xmlNewChild(parent, NULL, BAD_CAST "STRING", BAD_CAST (property.value.as_string));
            break;
        case BOOL:
            xmlNewChild(parent, NULL, BAD_CAST "BOOLEAN", BAD_CAST (property.value.as_bool ? "true": "false"));
            break;
    }
}
 

void set_filter(xmlNodePtr parent, struct filter* filter) {

    switch (filter->prop_op) {

        case EQUALS:
            parent = xmlNewChild(parent, NULL, BAD_CAST "EQ", NULL);
            break;
        case NOT_EQUALS:
            parent = xmlNewChild(parent, NULL, BAD_CAST "NE", NULL);
            break;
        case LESS_THAN:
            parent = xmlNewChild(parent, NULL, BAD_CAST "LT", NULL);
            break;
        case GREATER_THAN:
            parent = xmlNewChild(parent, NULL, BAD_CAST "GT", NULL);
            break;
    }

    set_property(parent, filter->property);
}

void set_logic_operation(xmlNodePtr parent, struct logic_operation* lo, struct filter* f) {

    switch (lo->operation) {

        case AND_T:
            parent = xmlNewChild(parent, NULL, BAD_CAST "AND", NULL);
            break;
    
        case OR_T:
            parent = xmlNewChild(parent, NULL, BAD_CAST "OR", NULL);
            break;
    }

    set_filter(parent, f);
            
    if (lo->next_logic_operation != NULL) {
        set_logic_operation(parent, lo->next_logic_operation, f->next_filter);

    } else {
        set_filter(parent, f->next_filter);
    }
}
    

xmlNodePtr set_where_node(xmlNodePtr query_node, struct query q) {
    xmlNodePtr where_node = xmlNewChild(query_node, NULL, BAD_CAST "WHERE", NULL);

    if (q.as_match.cond.log_op != NULL) {
        set_logic_operation(where_node, q.as_match.cond.log_op, q.as_match.cond.filter);

    } else if (q.as_match.cond.filter != NULL) {
        set_filter(where_node, q.as_match.cond.filter);
    }
}

xmlNodePtr set_field_set_node(xmlNodePtr query_node, struct query q) {
    xmlNodePtr field_set_node = xmlNewChild(query_node, NULL, BAD_CAST "FIELD_SET", NULL);
    struct property* prop;

    switch (q.type) {
        case SET_T:
            prop = q.as_set.prop;
            break;
        case CREATE_T:
            prop = q.as_create.prop;
            break;
    }

    while (prop != NULL) {
        set_property(field_set_node, *prop);
        prop = prop->next_prop;
    }
}

char* build_query(struct query q) {
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "query");
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlDocSetRootElement(doc, root_node);

    xmlNodePtr query_node = set_query_node(root_node, q);
    xmlNodePtr node_node = xmlNewChild(query_node, NULL, BAD_CAST "NODE", NULL);
    xmlSetProp(node_node, BAD_CAST "label", BAD_CAST q.label);

     switch (q.type) {
        case MATCH_T:
            if (q.as_match.cond.filter != NULL) {
                set_where_node(query_node, q);
            }
            break;
        case CREATE_T:
            set_field_set_node(query_node, q);
            break;
        case SET_T:
            if (q.as_set.cond.filter != NULL) {
                set_where_node(query_node, q);
            }
            set_field_set_node(query_node, q);
            break;
        case DELETE_T:
            if (q.as_delete.cond.filter != NULL) {
                set_where_node(query_node, q);
            }
            break;
    }

    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpMemory(doc, &xmlbuff, &buffersize);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return (char *) xmlbuff;
}

void print_response(const char* response) {

    xmlDocPtr doc = xmlReadMemory(response, strlen(response), NULL, NULL, 0);
    xmlNodePtr root_node = xmlDocGetRootElement(doc);

    xmlNodePtr message_node = root_node->children;

    if (message_node != NULL) {
        printf("\nMessage: %s\n", (char *) message_node->children->content);
    }

    xmlNodePtr node = message_node->next;
    size_t cnt = 1;

    while (node != NULL) {
        printf("\nNODE %zu\n", cnt);
        cnt++;
        xmlNodePtr attr = node->children;
        while (attr != NULL) {
            printf("\t%s: %s\n", attr->children->content, attr->next->children->content);
            attr = attr->next->next;
        }
        node = node->next;
    }
    printf("\n");

    xmlFreeDoc(doc);
    xmlCleanupParser();
}


bool validate_response(const char *buffer) {
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