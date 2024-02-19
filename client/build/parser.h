/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_HOME_IVAAAAAK_LLP_LAB3_CLIENT_BUILD_PARSER_H_INCLUDED
# define YY_YY_HOME_IVAAAAAK_LLP_LAB3_CLIENT_BUILD_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "/home/ivaaaaak/llp_lab3/client/parser/bison+flex/parser.y"

    #include "../parser/include/query.h"
    extern struct query q;
    int yylex();
	void yyerror(const char *s);

#line 55 "/home/ivaaaaak/llp_lab3/client/build/parser.h"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LPAREN = 258,
    RPAREN = 259,
    LBRACE = 260,
    RBRACE = 261,
    DOT = 262,
    COLON = 263,
    COMMA = 264,
    REDGE = 265,
    SET = 266,
    DELETE = 267,
    CREATE = 268,
    MATCH = 269,
    LEDGE = 270,
    WHERE = 271,
    RETURN = 272,
    AND = 273,
    OR = 274,
    EOL = 275,
    INT_T = 276,
    BOOL_T = 277,
    IDENTIFIER_T = 278,
    STRING_T = 279,
    DOUBLE_T = 280,
    EQUALS_T = 281,
    NOT_EQUALS_T = 282,
    LESS_THAN_T = 283,
    GREATER_THAN_T = 284,
    CONTAINS_T = 285
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 8 "/home/ivaaaaak/llp_lab3/client/parser/bison+flex/parser.y"

    bool bool_value;
    int int_value;
    double double_value;
    char* string_value;

    struct value struct_value;
    struct filter filter_value;

#line 107 "/home/ivaaaaak/llp_lab3/client/build/parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HOME_IVAAAAAK_LLP_LAB3_CLIENT_BUILD_PARSER_H_INCLUDED  */
