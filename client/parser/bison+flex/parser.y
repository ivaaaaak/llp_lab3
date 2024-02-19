%code requires {
    #include "../parser/include/query.h"
    extern struct query q;
    int yylex();
	void yyerror(const char *s);
}

%union{
    bool bool_value;
    int int_value;
    double double_value;
    char* string_value;

    struct value struct_value;
    struct filter filter_value;
}

%type <struct_value> value
%type <filter_value> condition
%type <int_value> compare_op
%type <string_value> field

%{
struct query q = {0};
%}

%token LPAREN RPAREN LBRACE RBRACE DOT COLON COMMA REDGE
%token <string_value> SET DELETE CREATE MATCH LEDGE
%token WHERE RETURN
%token <string_value> AND OR
%token EOL

%token <int_value> INT_T
%token <bool_value> BOOL_T
%token <string_value> IDENTIFIER_T STRING_T
%token <double_value> DOUBLE_T

%token <int_value> EQUALS_T NOT_EQUALS_T LESS_THAN_T GREATER_THAN_T CONTAINS_T

%%

end : query EOL  { return 0; }

query
    : create_query
    | set_query 
    | delete_query
    | match_query
    | match_edge_query
    ;

create_query
    : CREATE node { set_query_type(&q, $1); }
    ;

set_query
    : MATCH node SET assignments { set_query_type(&q, $3); }
    | MATCH node where_part SET assignments { set_query_type(&q, $4); }
    ;

delete_query
    : MATCH node DELETE IDENTIFIER_T { set_query_type(&q, $3); set_delete_value(&q, $4); }
    | MATCH node where_part DELETE IDENTIFIER_T { set_query_type(&q, $4); set_delete_value(&q, $5); }
    ;

match_query
    : MATCH node RETURN IDENTIFIER_T { set_query_type(&q, $1);}
    | MATCH node where_part RETURN IDENTIFIER_T { set_query_type(&q, $1);  }
    ;

match_edge_query
    : MATCH node LEDGE IDENTIFIER_T REDGE node where_part RETURN return_part { set_query_type(&q, $3); }
    ;

node 
    : LPAREN IDENTIFIER_T COLON IDENTIFIER_T properties_clause RPAREN { set_var_name_and_label(&q, $2, $4); }
    | LPAREN IDENTIFIER_T COLON IDENTIFIER_T RPAREN { set_var_name_and_label(&q, $2, $4); }
    ;


where_part
    : WHERE conditions
    ;

return_part
    : IDENTIFIER_T
    | return_part COMMA IDENTIFIER_T
    ;

conditions
    : condition
    | conditions AND condition {set_new_logical_operation(&q, $2);}
    | conditions OR condition {set_new_logical_operation(&q, $2);}

condition
    : field compare_op value {set_new_filter(&q, $1, $2, $3);}
    ;

properties_clause
    : LBRACE properties RBRACE
    ;

properties
    : property
    | properties COMMA property
    ;

property
    : IDENTIFIER_T COLON value {set_new_property(&q, $1, $3, "create"); }
    ;

assignments
    : assignment
    | assignments COMMA assignment
    ;

assignment
    : field EQUALS_T value {set_new_property(&q, $1, $3, "set");}
    ;

field
    : IDENTIFIER_T DOT IDENTIFIER_T {$$ = $3;}
    ;

value
    : BOOL_T {$$ = create_boolean($1);}
    | INT_T {$$ = create_int($1);}
    | DOUBLE_T {$$ = create_float($1);}
    | STRING_T {$$ = create_string($1);}
    ;

compare_op: EQUALS_T | NOT_EQUALS_T | LESS_THAN_T | GREATER_THAN_T | CONTAINS_T;

%%


void yyerror(const char *s) {
    fprintf(stderr, "error: %s\n", s);
}