#include "../include/printer.h"

const char* query_type_string[] = {
	[CREATE_T] = "CREATE",
	[MATCH_T] = "MATCH",
	[SET_T] =  "SET",
	[DELETE_T] = "DELETE"
};

const char* property_operation_string[] = {
	[EQUALS] =  "EQUALS",
	[NOT_EQUALS] = "NOT_EQUALS",
	[LESS_THAN] =  "LESS_THAN",
	[GREATER_THAN] = "GREATER_THAN",
    [CONTAINS] = "CONTAINS"
};

void print_tab(int indent_level) {
    for (int i = 0; i < indent_level; i++) {
        printf("\t");
    }
}

void print_value(struct property* prop) {
    switch (prop->value.type) {
        case BOOL:
            printf("value: %s\n\n", prop->value.as_bool ? "true" : "false");
            break;
        case INT:
            printf("value: %d\n\n",  prop->value.as_int);
            break;
        case DOUBLE:
            printf("value: %f\n\n",  prop->value.as_double);
            break;
        case STRING:
            printf("value: %s\n\n",  prop->value.as_string);
            break;
    }
}

void print_filter(int tab_num, struct filter* filter) {
    print_tab(tab_num);
    printf("field name: %s\n", filter->property.name);

    print_tab(tab_num);
    printf("operation: %s\n", property_operation_string[filter->prop_op]);

    print_tab(tab_num);
    print_value(&(filter->property));
}


void print_conditions(struct query q) {
    struct filter* filter = q.as_match.cond.filter;
    struct logic_operation* prev_log_op = NULL;
    struct logic_operation* next_log_op = q.as_match.cond.log_op;

    if (filter) {
        printf("where conditions:\n\n");
    }

    while (filter != NULL) {

        if (prev_log_op == NULL && next_log_op == NULL) {
            print_filter(1, filter);
            break;

        } else if (prev_log_op == NULL) {

            if (next_log_op->operation == AND_T) {
                print_filter(3, filter);

                print_tab(2);
                printf("AND\n");

            } else if (next_log_op->operation == OR_T) {
                print_filter(2, filter);

                print_tab(1);
                printf("OR\n");
            }

        } else if (next_log_op == NULL) {

            if (prev_log_op->operation == AND_T) {
                print_filter(3, filter);
            } else if (prev_log_op->operation == OR_T) {
                print_filter(2, filter);
            }
            break;

        } else {

            if (prev_log_op->operation == AND_T && next_log_op->operation == AND_T) {
                print_filter(3, filter);
                print_tab(2);
                printf("AND\n");

            } else if (prev_log_op->operation == OR_T && next_log_op->operation == OR_T) {
                print_filter(2, filter);
                print_tab(1);
                printf("OR\n");

            } else if (prev_log_op->operation == AND_T && next_log_op->operation == OR_T) {
                print_filter(3, filter);
                print_tab(1);
                printf("OR\n");

            } else if (prev_log_op->operation == OR_T && next_log_op->operation == AND_T) {
                print_filter(3, filter);
                print_tab(2);
                printf("AND\n");
            }
        }
        
        filter = filter->next_filter;
        prev_log_op = next_log_op;
        next_log_op = next_log_op->next_logic_operation;
    }
}


void print_query(struct query q) {
    printf("PARSED QUERY: \n\n");

    printf("query type: %s\n\n", query_type_string[q.type]);
    print_tab(1);
    printf("var name: %s\n", q.var_name);
    print_tab(1);
    printf("node label: %s\n\n", q.label);
    
    struct property* prop;
    
    switch (q.type) {
        case CREATE_T:
            prop = q.as_create.prop;

            if (prop != NULL) {
                print_tab(1);
                printf("properties: \n");
            }

            while(prop != NULL) {
                print_tab(2);
                printf("name: %s\n", prop->name);

                print_tab(2);
                print_value(prop);

                prop = prop->next_prop;
            }
            break;

        case MATCH_T:
            print_conditions(q);
            printf("return value: %s\n", q.as_match.return_value);
            break;

        case SET_T:
            print_conditions(q);
            printf("changed properties:\n\n");
            prop = q.as_set.prop;

            while(prop != NULL) {
                print_tab(2);
                printf("name: %s\n", prop->name);

                print_tab(2);
                print_value(prop);

                prop = prop->next_prop;
            }
            break;

        case DELETE_T:
            print_conditions(q);
            printf("deleted: %s\n", q.as_delete.delete_value);
            break;
    }
}