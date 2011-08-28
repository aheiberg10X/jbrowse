
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CONST_CHAR = 258,
     CONST_STRING = 259,
     NUMBER = 260,
     EQUAL = 261,
     ASSIGN = 262,
     STAR = 263,
     GREATER = 264,
     LESS = 265,
     NOT_EQUAL = 266,
     LESS_EQUAL = 267,
     GREATER_EQUAL = 268,
     MUL = 269,
     DIV = 270,
     MOD = 271,
     PLUS = 272,
     MINUS = 273,
     AND = 274,
     OR = 275,
     NOT = 276,
     SEMICOLON = 277,
     DOT = 278,
     LPAREN = 279,
     RPAREN = 280,
     COMMA = 281,
     INTEGER = 282,
     FLOAT = 283,
     CHAR = 284,
     STRING_TYPE = 285,
     ID = 286,
     SELECT = 287,
     FROM = 288,
     WHERE = 289,
     COUNT = 290,
     TABLE = 291,
     TK_NEGATIVE_SIGN = 292,
     TK_POSITIVE_SIGN = 293,
     TK_RIGHT_SQUARE_BRACKET = 294,
     TK_LEFT_SQUARE_BRACKET = 295,
     TK_RIGHT_BRACKET = 296,
     TK_LEFT_BRACKET = 297
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 23 "biosql.y"

	int int_val;
	char *string;
	struct st_node *entry;



/* Line 1676 of yacc.c  */
#line 102 "biosql.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


