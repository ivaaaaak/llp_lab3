%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
%}

%%

"(" { return LPAREN; }
")" { return RPAREN; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"." { return DOT; }
"," { return COMMA; }
":" { return COLON; }
"-[:" { yylval.string_value = strdup(yytext); return LEDGE; }
"]->" { return REDGE; }


"="  {yylval.int_value = 0; return EQUALS_T;}
"!=" {yylval.int_value = 1; return NOT_EQUALS_T;}
"<"  {yylval.int_value = 2; return LESS_THAN_T;}
">"  {yylval.int_value = 3; return GREATER_THAN_T;}
"contains"   {yylval.int_value = 4; return CONTAINS_T;}


"return"       {yylval.string_value = strdup(yytext); return RETURN;}

"where"       {yylval.string_value = strdup(yytext); return WHERE;}
"and"         {yylval.string_value = strdup(yytext); return AND;}
"or"          {yylval.string_value = strdup(yytext); return OR;}

"create"       {yylval.string_value = strdup(yytext); return CREATE;}
"match"        {yylval.string_value = strdup(yytext); return MATCH;}
"set"          {yylval.string_value = strdup(yytext); return SET;}
"delete"       {yylval.string_value = strdup(yytext); return DELETE;}

"true"|"false"          {yylval.bool_value = strdup(yytext); return BOOL_T;}
[a-zA-Z_]+              {yylval.string_value = strdup(yytext); return IDENTIFIER_T; }
([+-]?[0-9])+           {yylval.int_value = atoi(yytext); return INT_T;}
[+-]?([0-9]*[.])?[0-9]+ {yylval.double_value = atof(yytext); return DOUBLE_T;}
\"[^\"]*\" 				{yylval.string_value = strdup(yytext); return STRING_T;}


[ \t\r]+

";" {return EOL; }

. { printf("Unrecognized character: %s\n", yytext); exit(1); }

%%

int yywrap(void){
	return 1;
}