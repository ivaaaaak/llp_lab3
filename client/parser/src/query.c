#include "../include/query.h"

struct value create_boolean(bool value) {
    struct value v = {
        .type = BOOL,
        .as_bool = value
    };
    return v;
}

struct value create_int(int32_t value) {
    struct value v = {
        .type = INT32,
        .as_int = value
    };
    return v;
}

struct value create_float(float value) {
    struct value v = {
        .type = FLOAT,
        .as_float = value
    };
    return v;
}

struct value create_string(char* value) {
    struct value v = {
        .type = STRING,
    };
    strcpy(v.as_string, value);
    return v;
}

void set_query_type(struct query* q, char* type) {
    if (strcmp(type, "create") == 0) {
        q->type = CREATE_T;
    } else if (strcmp(type, "set") == 0) {
        q->type = SET_T;
    } else if (strcmp(type, "delete") == 0) {
        q->type = DELETE_T;
    } else if (strcmp(type, "match") == 0) {
        q->type = MATCH_T;
    } else if (strcmp(type, "-[:") == 0) {
        q->type = MATCH_EDGE_T;
    }
}

void set_var_name_and_label(struct query* q, char* var_name, char* label) {
    strcpy(q->var_name, var_name);
    strcpy(q->label, label);
}

void set_new_property(struct query* q, char* name, struct value value, char* type) {
    struct property* new_prop = malloc(sizeof(struct property));
    strcpy(new_prop->name, name); 
    new_prop->value = value;
    new_prop->next_prop = NULL;

    set_query_type(q, type);
    
    struct property* prop;
    switch (q->type) {
        case CREATE_T:
            prop = q->as_create.prop;
            if (prop != NULL) {
                while (prop->next_prop != NULL) {
                    prop = prop->next_prop;
                }
                prop->next_prop = new_prop;
            } else {
                q->as_create.prop = new_prop;
            }
            break;

        case SET_T:
            prop = q->as_set.prop;
            if (prop != NULL) {
                while (prop->next_prop != NULL) {
                    prop = prop->next_prop;
                }
                prop->next_prop = new_prop;
            } else {
                q->as_set.prop = new_prop;
            }
            break;
    }
}

void set_new_logical_operation(struct query* q, char* oper) {
    struct logic_operation* lo = malloc(sizeof(struct logic_operation));
    lo->next_logic_operation = NULL;

    if (strcmp(oper, "and") == 0) {
        lo->operation = AND_T;
    } else if (strcmp(oper, "or") == 0) {
        lo->operation = OR_T;
    }

    struct logic_operation* l = q->as_match.cond.log_op;
    if (l != NULL) {
        while (l->next_logic_operation != NULL) {
            l = l->next_logic_operation;
        }
        l->next_logic_operation = lo;
    } else {
        q->as_match.cond.log_op = lo;
    }
}


void set_new_filter(struct query* q, char* name, int operation, struct value value) {
    struct filter* new_f = malloc(sizeof(struct filter));

     switch (operation) {
        case 0:
            new_f->prop_op = EQUALS;
            break;
        case 1:
            new_f->prop_op = NOT_EQUALS;
            break;
        case 2:
            new_f->prop_op = LESS_THAN;
            break;
        case 3:
            new_f->prop_op = GREATER_THAN;
            break;
        case 4:
            new_f->prop_op = CONTAINS;
            break;
    }

    strcpy(new_f->property.name, name);
    new_f->property.value = value;
    new_f->next_filter = NULL;
    
    struct filter* f = q->as_match.cond.filter;
    if (f != NULL) {
        while (f->next_filter != NULL) {
            f = f->next_filter;
        }
        f->next_filter = new_f;
    } else {
        q->as_match.cond.filter = new_f;
    }
}

void set_delete_value(struct query* q, char* name) {
    set_query_type(q, "delete"); 
    strcpy(q->as_delete.delete_value, name);
}



