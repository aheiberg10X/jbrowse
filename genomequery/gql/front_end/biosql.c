
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "biosql.y"

#include<cstdlib>
//#include<stdio.h>
#include<vector>
#include<list>
#include<map>
#include<iostream>
#include"biosql_defs.h"
#include<string>

#define MAX_FROM_TABLES 2048

using namespace std;

/*extern "C"
{
	int yyparse(void);
	int yylex(void);
	int yywrap(){
		return 1;
	}
}*/

extern int lineNo;
extern int errorNo;
int nextstat=1; //keep count of the lines of 

string parsed_table_file;

map<string, list<st_node> > st; //the symbol table
map<string, list<st_node> > tmp_st; //the active symbol table from the from_statements
int arg_cnt=1;
vector<string> emit_lst;
vector<string> select_lst;
st_node *cur_owner=NULL;



/* Line 189 of yacc.c  */
#line 112 "biosql.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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
     MAPJOIN = 292,
     IMPORT = 293,
     USE = 294,
     USING = 295,
     INTERVAL_CREATION = 296,
     CREATE_INTERVALS = 297,
     MERGE_INTERVALS = 298,
     INTERVALS = 299,
     BOTH_MATES = 300,
     GENOME = 301,
     PRINT = 302,
     TK_NEGATIVE_SIGN = 303,
     TK_POSITIVE_SIGN = 304,
     TK_RIGHT_SQUARE_BRACKET = 305,
     TK_LEFT_SQUARE_BRACKET = 306,
     TK_RIGHT_BRACKET = 307,
     TK_LEFT_BRACKET = 308
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 40 "biosql.y"

	int int_val;
	std::string* str;
	struct __st_node* entry;



/* Line 214 of yacc.c  */
#line 209 "biosql.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 221 "biosql.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  8
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   156

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  67
/* YYNRULES -- Number of states.  */
#define YYNSTATES  127

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   308

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     8,    11,    12,    18,    21,    23,    25,
      29,    31,    34,    37,    40,    43,    46,    50,    53,    54,
      58,    60,    67,    72,    78,    82,    89,    96,    98,   100,
     104,   106,   108,   112,   114,   116,   125,   136,   138,   142,
     146,   150,   153,   157,   159,   163,   167,   170,   175,   177,
     179,   181,   183,   185,   189,   191,   193,   195,   197,   199,
     201,   203,   205,   207,   209,   212,   213,   216
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    56,    63,    64,    80,    -1,    57,    56,
      -1,    -1,    58,    24,    61,    25,    22,    -1,    36,    59,
      -1,    31,    -1,     5,    -1,    61,    26,    62,    -1,    62,
      -1,    27,    59,    -1,    28,    59,    -1,    29,    59,    -1,
      30,    59,    -1,    63,    63,    -1,    46,    59,    22,    -1,
      65,    64,    -1,    -1,    66,     7,    67,    -1,    59,    -1,
      32,    69,    33,    72,    34,    74,    -1,    32,    68,    33,
      72,    -1,    32,    69,    33,    37,    71,    -1,    42,    24,
      25,    -1,    43,    24,    59,     9,    60,    25,    -1,    43,
      24,    59,    10,    60,    25,    -1,     8,    -1,    70,    -1,
      59,    26,    70,    -1,    59,    -1,    77,    -1,    72,    26,
      71,    -1,    72,    -1,    73,    -1,    73,    40,    44,    24,
      76,    26,    76,    25,    -1,    73,    40,    44,    24,    76,
      26,    76,    26,    45,    25,    -1,    59,    -1,    74,    26,
      74,    -1,    74,    19,    74,    -1,    74,    20,    74,    -1,
      21,    74,    -1,    24,    74,    25,    -1,    75,    -1,    76,
      78,    76,    -1,    76,    79,    76,    -1,    79,    76,    -1,
      59,    24,    76,    25,    -1,    59,    -1,    77,    -1,     5,
      -1,     4,    -1,     3,    -1,    59,    23,    59,    -1,     6,
      -1,    13,    -1,    12,    -1,     9,    -1,    10,    -1,    17,
      -1,    18,    -1,    14,    -1,    15,    -1,    16,    -1,    81,
      80,    -1,    -1,    47,    59,    -1,    47,    59,    45,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   119,   119,   121,   122,   125,   131,   142,   147,   152,
     157,   162,   166,   170,   174,   187,   188,   194,   195,   198,
     205,   214,   219,   223,   229,   234,   240,   248,   251,   254,
     257,   260,   266,   267,   270,   274,   281,   292,   299,   304,
     309,   314,   319,   324,   331,   339,   345,   351,   359,   367,
     374,   378,   383,   389,   402,   403,   404,   405,   406,   409,
     410,   411,   412,   413,   417,   418,   421,   428
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CONST_CHAR", "CONST_STRING", "NUMBER",
  "EQUAL", "ASSIGN", "STAR", "GREATER", "LESS", "NOT_EQUAL", "LESS_EQUAL",
  "GREATER_EQUAL", "MUL", "DIV", "MOD", "PLUS", "MINUS", "AND", "OR",
  "NOT", "SEMICOLON", "DOT", "LPAREN", "RPAREN", "COMMA", "INTEGER",
  "FLOAT", "CHAR", "STRING_TYPE", "ID", "SELECT", "FROM", "WHERE", "COUNT",
  "TABLE", "MAPJOIN", "IMPORT", "USE", "USING", "INTERVAL_CREATION",
  "CREATE_INTERVALS", "MERGE_INTERVALS", "INTERVALS", "BOTH_MATES",
  "GENOME", "PRINT", "TK_NEGATIVE_SIGN", "TK_POSITIVE_SIGN",
  "TK_RIGHT_SQUARE_BRACKET", "TK_LEFT_SQUARE_BRACKET", "TK_RIGHT_BRACKET",
  "TK_LEFT_BRACKET", "$accept", "program", "table_prototypes",
  "table_prototype", "table_keyword", "names", "numbers", "table_args",
  "table_arg", "genome_names", "assigned_selects", "assigned_select",
  "lvalue", "select_statement", "select_intrvl_args", "select_args",
  "select_arg_series", "from_args", "compound_from_arg", "from_arg",
  "where_args", "lowest_expr", "arith_expr", "obj_names", "comparison_op",
  "arith_op", "print_statements", "print_statement", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    56,    56,    57,    58,    59,    60,    61,
      61,    62,    62,    62,    62,    63,    63,    64,    64,    65,
      66,    67,    67,    67,    68,    68,    68,    69,    69,    70,
      70,    70,    71,    71,    72,    72,    72,    73,    74,    74,
      74,    74,    74,    74,    75,    76,    76,    76,    76,    76,
      76,    76,    76,    77,    78,    78,    78,    78,    78,    79,
      79,    79,    79,    79,    80,    80,    81,    81
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     2,     0,     5,     2,     1,     1,     3,
       1,     2,     2,     2,     2,     2,     3,     2,     0,     3,
       1,     6,     4,     5,     3,     6,     6,     1,     1,     3,
       1,     1,     3,     1,     1,     8,    10,     1,     3,     3,
       3,     2,     3,     1,     3,     3,     2,     4,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     0,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     0,     4,     0,     7,     6,     1,     0,
      18,     3,     0,     0,    20,    15,    65,    18,     0,     0,
       0,     0,     0,     0,    10,    16,     0,     2,    65,    17,
       0,    11,    12,    13,    14,     0,     0,    66,    64,     0,
      19,     5,     9,    67,    27,     0,     0,    30,     0,     0,
      28,    31,     0,     0,     0,     0,     0,     0,    24,     0,
      53,    29,    37,    22,    34,     0,     0,     0,     0,     0,
      23,    33,     0,     8,     0,     0,     0,     0,    52,    51,
      50,    61,    62,    63,    59,    60,     0,     0,    48,    21,
      43,     0,    49,     0,    25,    26,     0,    32,    41,     0,
       0,     0,     0,     0,    54,    57,    58,    56,    55,     0,
       0,    46,     0,    42,     0,    39,    40,    38,    44,    45,
       0,    47,     0,    35,     0,     0,    36
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,     4,     5,    88,    74,    23,    24,    15,
      16,    17,    18,    40,    48,    49,    50,    70,    71,    64,
      89,    90,    91,    92,   109,    93,    27,    28
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -86
static const yytype_int16 yypact[] =
{
     -21,   -26,    28,     1,   -21,    25,   -86,   -86,   -86,   -26,
     -29,   -86,   100,    41,   -86,     1,    21,   -26,    68,   -26,
     -26,   -26,   -26,   -22,   -86,   -86,   -26,   -86,    21,   -86,
      45,   -86,   -86,   -86,   -86,    89,   100,    71,   -86,    -7,
     -86,   -86,   -86,   -86,   -86,   107,   110,    67,   102,   103,
     -86,   -86,   112,   -26,   -26,   -26,   -26,    49,   -86,    75,
     -86,   -86,   -86,   -86,    98,   -26,   105,   135,   135,    97,
     -86,   116,    27,   -86,   118,   119,   121,   -26,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,    27,    27,   109,    20,
     -86,    82,   -86,    56,   -86,   -86,    56,   -86,   120,    95,
      56,    27,    27,    27,   -86,   -86,   -86,   -86,   -86,    56,
      56,   108,    -4,   -86,    92,   120,    31,    20,   108,   108,
      56,   -86,    87,   -86,   104,   122,   -86
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -86,   -86,   144,   -86,   -86,    -1,    83,   -86,   114,   149,
     136,   -86,   -86,   -86,   -86,   -86,    99,    78,    62,   -86,
     -20,   -86,   -31,   -32,   -86,   -85,   128,   -86
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
       7,    44,     6,    35,    36,     6,   110,    51,    13,    14,
      81,    82,    83,    84,    85,     1,    14,     9,    31,    32,
      33,    34,   120,    51,     6,    37,   110,   110,     8,   110,
      78,    79,    80,   110,   110,    45,    46,   110,    47,   101,
     102,    81,    82,    83,    84,    85,   103,     9,    86,    12,
     101,    87,    59,    60,    47,    62,    62,   103,     6,    78,
      79,    80,   111,    25,    62,   112,    98,    99,    26,   114,
      81,    82,    83,    84,    85,    30,    62,    39,   118,   119,
       6,   115,   116,   117,    67,    68,    65,     6,   104,   122,
      54,   105,   106,    55,   107,   108,    81,    82,    83,    84,
      85,    81,    82,    83,    84,    85,    81,    82,    83,    84,
      85,    41,   123,   124,   101,   102,    43,   121,    63,    66,
     113,   103,    81,    82,    83,    84,    85,    19,    20,    21,
      22,    52,    54,   100,    53,    56,    57,    58,    69,    72,
      73,    76,    77,    94,    95,    96,   103,   126,    11,   125,
      42,    75,    10,    29,    61,    97,    38
};

static const yytype_uint8 yycheck[] =
{
       1,     8,    31,    25,    26,    31,    91,    39,     9,    10,
      14,    15,    16,    17,    18,    36,    17,    46,    19,    20,
      21,    22,    26,    55,    31,    26,   111,   112,     0,   114,
       3,     4,     5,   118,   119,    42,    43,   122,    39,    19,
      20,    14,    15,    16,    17,    18,    26,    46,    21,    24,
      19,    24,    53,    54,    55,    56,    57,    26,    31,     3,
       4,     5,    93,    22,    65,    96,    86,    87,    47,   100,
      14,    15,    16,    17,    18,     7,    77,    32,   109,   110,
      31,   101,   102,   103,     9,    10,    37,    31,     6,   120,
      23,     9,    10,    26,    12,    13,    14,    15,    16,    17,
      18,    14,    15,    16,    17,    18,    14,    15,    16,    17,
      18,    22,    25,    26,    19,    20,    45,    25,    56,    57,
      25,    26,    14,    15,    16,    17,    18,    27,    28,    29,
      30,    24,    23,    24,    24,    33,    33,    25,    40,    34,
       5,    44,    26,    25,    25,    24,    26,    25,     4,    45,
      36,    68,     3,    17,    55,    77,    28
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    36,    55,    56,    57,    58,    31,    59,     0,    46,
      63,    56,    24,    59,    59,    63,    64,    65,    66,    27,
      28,    29,    30,    61,    62,    22,    47,    80,    81,    64,
       7,    59,    59,    59,    59,    25,    26,    59,    80,    32,
      67,    22,    62,    45,     8,    42,    43,    59,    68,    69,
      70,    77,    24,    24,    23,    26,    33,    33,    25,    59,
      59,    70,    59,    72,    73,    37,    72,     9,    10,    40,
      71,    72,    34,     5,    60,    60,    44,    26,     3,     4,
       5,    14,    15,    16,    17,    18,    21,    24,    59,    74,
      75,    76,    77,    79,    25,    25,    24,    71,    74,    74,
      24,    19,    20,    26,     6,     9,    10,    12,    13,    78,
      79,    76,    76,    25,    76,    74,    74,    74,    76,    76,
      26,    25,    76,    25,    26,    45,    25
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:

/* Line 1455 of yacc.c  */
#line 125 "biosql.y"
    {
	cur_owner=NULL;
;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 131 "biosql.y"
    {
	if(*(yyvsp[(2) - (2)].str)=="READS") arg_cnt=-1;
	else arg_cnt=1;
	if(lookup(st, *(yyvsp[(2) - (2)].str), "table", NULL)!=NULL)
		symerror("Multiple definitions for table "+*(yyvsp[(2) - (2)].str));
	check_and_add(st, *(yyvsp[(2) - (2)].str), "table", "table", "table", NULL);
	cur_owner=lookup(st, *(yyvsp[(2) - (2)].str), "table", NULL);
;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 142 "biosql.y"
    {
	(yyval.str)=new string(*(yylval.str));
;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 147 "biosql.y"
    {
	(yyval.str)=new string(*(yylval.str));
;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 152 "biosql.y"
    {
//arguments need to appear in output in the order that the user enders them.

;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 157 "biosql.y"
    {

;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 162 "biosql.y"
    {
	string place=update_attribute_place(arg_cnt, cur_owner->name, *(yyvsp[(2) - (2)].str));
	check_and_add(st, *(yyvsp[(2) - (2)].str), "attribute", "int", place, cur_owner);
;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 166 "biosql.y"
    {
	string place=update_attribute_place(arg_cnt, cur_owner->name, *(yyvsp[(2) - (2)].str));
	check_and_add(st, *(yyvsp[(2) - (2)].str), "attribute", "float", place, cur_owner);
;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 170 "biosql.y"
    {
	string place=update_attribute_place(arg_cnt, cur_owner->name, *(yyvsp[(2) - (2)].str));
	check_and_add(st, *(yyvsp[(2) - (2)].str), "attribute", "char", place, cur_owner);
;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 174 "biosql.y"
    {
	string place=update_attribute_place(arg_cnt, cur_owner->name, *(yyvsp[(2) - (2)].str));
	check_and_add(st, *(yyvsp[(2) - (2)].str), "attribute", "string", place, cur_owner);
	if(*(yyvsp[(2) - (2)].str)=="chromosome" || *(yyvsp[(2) - (2)].str)=="chr" || *(yyvsp[(2) - (2)].str)=="chromo"){
		stringstream ss;
		ss << arg_cnt-1;
		emit(emit_lst, "table: "+cur_owner->name+" chr_loc: "+ss.str());
	}
;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 188 "biosql.y"
    {
	//emit(&emit_lst, "genome", $2, NULL, NULL, NULL);
	emit(emit_lst, "genome "+(*(yyvsp[(2) - (3)].str)));
;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 198 "biosql.y"
    {//emit(&emit_lst, "result_table", ":", $1->name, NULL,NULL); cur_owner=NULL;

	cur_owner=NULL;
	emit(emit_lst, "End_of_sentence");
;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 205 "biosql.y"
    { //$1 should be a new table name which is added to the symbol table
	check_and_add(st, *(yyvsp[(1) - (1)].str), "table", "table", "table", NULL);
	cur_owner=lookup(st, *(yyvsp[(1) - (1)].str), "table", NULL);
	emit(emit_lst, "result_table: "+*(yyvsp[(1) - (1)].str));
	select_lst.clear();
	tmp_st.clear();
;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 214 "biosql.y"
    {
	copy_to_symbol_table(select_lst, st, tmp_st, cur_owner);
	emit(emit_lst, "WHERE");
	emit(emit_lst, (yyvsp[(6) - (6)].entry)->place);
;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 219 "biosql.y"
    {
	copy_to_symbol_table(select_lst, st, tmp_st, cur_owner);
	emit(emit_lst, "INTERVAL_HANDLING");
;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 223 "biosql.y"
    {
	copy_to_symbol_table(select_lst, st, tmp_st, cur_owner);
	emit(emit_lst, "MAPJOIN");
;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 229 "biosql.y"
    {
	emit(emit_lst, "CREATE_INTERVALS");
	select_lst.push_back("st");
	select_lst.push_back("nd");
;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 234 "biosql.y"
    {
	emit(emit_lst, "MERGE_INTERVALS");
	select_lst.push_back("*");
	//select_lst.push_back("nd");
	emit(emit_lst, "merge > "+*(yyvsp[(5) - (6)].str));
;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 240 "biosql.y"
    {
	emit(emit_lst, "MERGE_INTERVALS");
	select_lst.push_back("*");
	//select_lst.push_back("nd");
	emit(emit_lst, " < "+*(yyvsp[(5) - (6)].str));
;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 248 "biosql.y"
    {
	select_lst.push_back("*");
;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 254 "biosql.y"
    {
	select_lst.push_back(*(yyvsp[(1) - (3)].str));
;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 257 "biosql.y"
    {
	select_lst.push_back(*(yyvsp[(1) - (1)].str));
;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 260 "biosql.y"
    {
	//???????? don't do anything for now... return later.
	//add_node(&select_lst, create_node($1->name, NULL, NULL, $1->owner));
;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 270 "biosql.y"
    {
	emit(emit_lst, "INPUT: "+*(yyvsp[(1) - (1)].str));
	
;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 274 "biosql.y"
    {
	emit(emit_lst, "INPUT: "+*(yyvsp[(1) - (8)].str));
	emit(emit_lst, "Interval calling on "+*(yyvsp[(1) - (8)].str));
	emit(emit_lst, "st: "+(yyvsp[(5) - (8)].entry)->place);
	emit(emit_lst, "nd: "+(yyvsp[(7) - (8)].entry)->place);

;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 281 "biosql.y"
    {
	emit(emit_lst, "INPUT: "+*(yyvsp[(1) - (10)].str));
	emit(emit_lst, "Interval calling on "+*(yyvsp[(1) - (10)].str)+ " both_mates");
	//emit(emit_lst, "both_mates");
	emit(emit_lst, "st: "+(yyvsp[(5) - (10)].entry)->place);
	emit(emit_lst, "nd: "+(yyvsp[(7) - (10)].entry)->place);

;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 292 "biosql.y"
    {
	update_active_scope(st, tmp_st, *(yyvsp[(1) - (1)].str));
	(yyval.str)=new string(*(yyvsp[(1) - (1)].str));
	disambiguate(tmp_st);
;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 299 "biosql.y"
    { //the type of where_args is str and denotes the respective temporary variable
	(yyval.entry)=new st_node();
	*(yyval.entry)=*(yyvsp[(1) - (3)].entry);
	(yyval.entry)->place="( "+(yyvsp[(1) - (3)].entry)->place+" ) && ( "+(yyvsp[(3) - (3)].entry)->place+" )";
;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 304 "biosql.y"
    {
	(yyval.entry)=new st_node();
	*(yyval.entry)=*(yyvsp[(1) - (3)].entry);
	(yyval.entry)->place="( "+(yyvsp[(1) - (3)].entry)->place+" ) && ( "+(yyvsp[(3) - (3)].entry)->place+" )";
;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 309 "biosql.y"
    {
	(yyval.entry)=new st_node();
	*(yyval.entry)=*(yyvsp[(1) - (3)].entry);
	(yyval.entry)->place="( "+(yyvsp[(1) - (3)].entry)->place+" ) || ( "+(yyvsp[(3) - (3)].entry)->place+" )";
;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 314 "biosql.y"
    {
	(yyval.entry)=new st_node();
	*(yyval.entry)=*(yyvsp[(2) - (2)].entry);
	(yyval.entry)->place="!"+(yyvsp[(2) - (2)].entry)->place;
;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 319 "biosql.y"
    {
	(yyval.entry)=new st_node();
	*(yyval.entry)=*(yyvsp[(2) - (3)].entry);
	(yyval.entry)->place="( "+(yyvsp[(2) - (3)].entry)->place+" )";
;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 324 "biosql.y"
    {
	(yyval.entry)=new st_node();
	*(yyval.entry)=*(yyvsp[(1) - (1)].entry);
;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 331 "biosql.y"
    {
	(yyval.entry)=new st_node();
	(yyval.entry)->place=(yyvsp[(1) - (3)].entry)->place+" "+*(yyvsp[(2) - (3)].str)+" "+(yyvsp[(3) - (3)].entry)->place;
	(yyval.entry)->type=cmp_type_compatibility((yyvsp[(1) - (3)].entry)->type, (yyvsp[(3) - (3)].entry)->type);

;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 339 "biosql.y"
    {
	(yyval.entry)=new st_node();
	(yyval.entry)->place=(yyvsp[(1) - (3)].entry)->place+" "+*(yyvsp[(2) - (3)].str)+" "+(yyvsp[(3) - (3)].entry)->place;
	(yyval.entry)->type=arith_type_compatibility((yyvsp[(1) - (3)].entry)->type, (yyvsp[(3) - (3)].entry)->type);
;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 345 "biosql.y"
    {
	(yyval.entry)=new st_node();
	(yyval.entry)->type=(yyvsp[(2) - (2)].entry)->type;
	(yyval.entry)->place=*(yyvsp[(1) - (2)].str)+" "+(yyvsp[(2) - (2)].entry)->place;
;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 351 "biosql.y"
    { //abs(expr)
	if(*(yyvsp[(1) - (4)].str) != "abs" && *(yyvsp[(1) - (4)].str) != "ABS")
		symerror("Undefined function call: "+*(yyvsp[(1) - (4)].str));
	(yyval.entry)=new st_node();
	(yyval.entry)->type=(yyvsp[(3) - (4)].entry)->type;
	(yyval.entry)->place=*(yyvsp[(1) - (4)].str)+(yyvsp[(3) - (4)].entry)->place;
;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 359 "biosql.y"
    {
	st_node *tmp=lookup(tmp_st, *(yyvsp[(1) - (1)].str), "attribute", NULL);
	if(tmp==NULL) symerror("Unknown table attribute: "+*(yyvsp[(1) - (1)].str));
	(yyval.entry)=new st_node();
	*(yyval.entry)=*tmp;
	(yyval.entry)->place=(yyval.entry)->type+"var"+" "+(yyval.entry)->place;
;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 367 "biosql.y"
    {
	(yyval.entry)=new st_node();
	*(yyval.entry)=*(yyvsp[(1) - (1)].entry);
	(yyval.entry)->place=(yyval.entry)->type+"var"+" "+(yyval.entry)->place;

;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 374 "biosql.y"
    {
	(yyval.entry)=create_node(*(yylval.str), "int", "", NULL);
	(yyval.entry)->place=(yyval.entry)->name;
;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 378 "biosql.y"
    {
	(yyval.entry)=create_node(*(yylval.str), "string", "", NULL);
	(yyval.entry)->place=(yyval.entry)->name;
;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 383 "biosql.y"
    {
	(yyval.entry)=create_node(*(yylval.str), "char", "", NULL);
	(yyval.entry)->place=(yyval.entry)->name;
;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 389 "biosql.y"
    {
	if (lookup(tmp_st, *(yyvsp[(1) - (3)].str), "table", NULL)==NULL)
		symerror("Parent table "+*(yyvsp[(1) - (3)].str)+" does not appear in FROM table series");
	st_node *tmp_parent=lookup(st, *(yyvsp[(1) - (3)].str), "table", NULL);
	st_node *tmp=lookup(st, *(yyvsp[(3) - (3)].str), "attribute", tmp_parent);
	if(tmp==NULL) symerror("Unknown table attribute: "+*(yyvsp[(3) - (3)].str));
	(yyval.entry)=new st_node();
	//copy_nodes($$, tmp);
	*(yyval.entry)=*tmp;
	(yyval.entry)->place=*(yyvsp[(1) - (3)].str)+"."+tmp->name;
;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 402 "biosql.y"
    {(yyval.str)=new string("==");;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 403 "biosql.y"
    {(yyval.str)=new string(">=");;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 404 "biosql.y"
    {(yyval.str)=new string("<=");;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 405 "biosql.y"
    {(yyval.str)=new string(">");;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 406 "biosql.y"
    {(yyval.str)=new string("<");;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 409 "biosql.y"
    {(yyval.str)=new string("+");;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 410 "biosql.y"
    {(yyval.str)=new string("-");;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 411 "biosql.y"
    {(yyval.str)=new string("x");;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 412 "biosql.y"
    {(yyval.str)=new string("/");;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 413 "biosql.y"
    {(yyval.str)=new string("%");;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 421 "biosql.y"
    {
	if(lookup(st, *(yyvsp[(2) - (2)].str), "table", NULL)==NULL)
		symerror("Unknown table "+*(yyvsp[(2) - (2)].str));
	if(*(yyvsp[(2) - (2)].str)=="READS" || *(yyvsp[(2) - (2)].str)=="Reads")
		symerror("Cannot print the entire reads table. Choose a subset of it");
	else emit(emit_lst, "print: "+*(yyvsp[(2) - (2)].str));
;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 428 "biosql.y"
    {
	st_node* nd=lookup(st, *(yyvsp[(2) - (3)].str), "table", NULL);
	if(nd==NULL)
		symerror("Unknown table "+*(yyvsp[(2) - (3)].str));
	if(*(yyvsp[(2) - (3)].str)=="READS" || *(yyvsp[(2) - (3)].str)=="Reads")
		symerror("Cannot print the entire reads table. Choose a subset of it");
	else emit(emit_lst, "print: "+*(yyvsp[(2) - (3)].str)+" both_mates");
;}
    break;



/* Line 1455 of yacc.c  */
#line 2108 "biosql.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 438 "biosql.y"

extern FILE *yyin;

int main(){
	//yydebug=1;

	yyparse();
	if(errorNo==0){
		cout << "Compilation completed without errors" << endl;
		cerr << "Compilation completed without errors" << endl;
		output_emit_list(emit_lst);
		//writeToFile(target_file);//the intermediate code will be printed at argv[1].interm
	}
	else{
		cout << "There were " << errorNo << "errors" << endl;
		cerr << "There were " << errorNo << "errors" << endl;
	}



	return 0;
}

int yyerror(char* msg)
{ 

	cout << "line "<< lineNo << "syntax Error... contents: " << *yylval.str << endl;

	///exit(2);
	errorNo++;
	if(errorNo>5){
		printf("Too many errors to continue\n");
		exit(1);
	}
	
	//return 1;
	
}


int symerror(string msg){
	cout << "line " << lineNo <<": "<< msg << endl;
	exit(2);
	errorNo++;
	if(errorNo>5){
		cout << "Too many errors to continue" << endl;
		exit(1);
	}
	
	//return 1;
	
}

void ioerror(string msg){
	cout << "FATAL ERROR!!! " << msg << endl;
	exit(2);
}


/*if there is no conflict with the contents of st, it adds to it a node with
fields the input arguments name, kind etc*/
void check_and_add(map<string, list<st_node> >& st, string name, string kind, string type, string place, st_node* owner){
	map<string, list<st_node> >::iterator it=st.find(name);
	if (it!=st.end()){
		list<st_node>::iterator vit = it->second.begin();
		for(;vit!=it->second.end();vit++)
			if(vit->name==name){
				if(vit->kind=="attribute" && vit->owner==owner){
					symerror("Attribute "+name+" already exists for table "+vit->owner->name);
					return;
				}
				else if(vit->name==name && vit->kind=="table" && vit->owner==owner){
					symerror("Table "+name+" already exists. ");
					return;
				}
			}
	}

	st_node nnode;
	nnode.name=name;
	nnode.type=type;
	nnode.kind=kind;
	nnode.place=place;
	nnode.owner=owner;
	st[name].push_back(nnode);
}

/*It returns the entry that matches with all non empty name/kind/owner fields,
NULL otherwise*/
st_node* lookup(map<string, list<st_node> >& st, string name, string kind, st_node* owner){
	map<string, list<st_node> >::iterator it=st.find(name);
	if (it!=st.end()){
		list<st_node>::iterator vit = it->second.begin();
		int i=0;
		for(;vit!=it->second.end();vit++){
			if(name.empty() || vit->name==name)
				if(kind.empty() || vit->kind==kind)
					if(owner==NULL || vit->owner==owner){
						return &(*vit);
					}
			i++;
		}
			
	}
	return NULL;
}


/*in case there are no coflicts between the given table_name and st, it copies
all entries whose owner is the table with the given table_name to tmp_st. */
void update_active_scope(map<string, list<st_node> >& st, map<string, list<st_node> >& tmp_st, string& table_name){
	st_node *parent=lookup(st, table_name, "table", NULL);
	check_and_add(tmp_st, table_name, "table", "table", "table", NULL);
	if(parent==NULL) symerror("Unknown table: "+table_name);
	map<string, list<st_node> >::iterator it=st.begin();
	for(;it!=st.end();it++){
		list<st_node>::iterator vit = it->second.begin();
		for(;vit!=it->second.end();vit++)
			if(vit->owner==parent){
				tmp_st[vit->name].push_back(*vit);
			}
	}
}

/*It disambiguates conflicting attributes of the same owner by appending
subscripts 1 and 2 (e.g. arg1 and arg2 for arg). The assumption is that all 
lists of st with more than one elements contain ambiguous elements*/
void disambiguate(map<string, list<st_node> >&st/*, st_node* owner*/){
	map<string, list<st_node> >::iterator it=tmp_st.begin();
	for(;it!=tmp_st.end();it++){
		list<st_node>::iterator vit = it->second.begin();
		if(it->second.size()==1) 
			continue;
		int cnt=1;
		for(;vit!=it->second.end();vit++){
			//if(vit->owner!=owner) symerror("Bad formated st map");
			st_node nnode=*vit;
			stringstream ss;
			ss << vit->name << cnt++;
			ss >> nnode.name;
			st[nnode.name].push_back(nnode);
		}
	}
}



/*It checks whether the contents of select_lst appear in tmp_st and copies them
to st. If select_lst contains * it copies everything from tmp_st*/
void copy_to_symbol_table(vector<string>& select_lst, map<string, list<st_node> >& st, map<string, list<st_node> >& tmp_st, st_node *cur_owner){
	if(select_lst.size()==2 && select_lst[0]=="st" && select_lst[1]=="nd"){
		st_node *nnode=create_node("st", "int", "attribute", cur_owner);
		nnode->place="st";
		if(cur_owner != NULL ) nnode->place=cur_owner->name+":"+nnode->place;
		st["st"].push_back(*nnode);
		nnode=create_node("nd", "int", "attribute", cur_owner);
		nnode->place="nd";
		if(cur_owner != NULL ) nnode->place=cur_owner->name+":"+nnode->place;
		st["nd"].push_back(*nnode);
	}
	map<string, list<st_node> >::iterator it=tmp_st.begin();
	for(;it!=tmp_st.end();it++){
		list<st_node>::iterator vit = it->second.begin();
		for(;vit!=it->second.end();vit++){
			if(select_lst[0]=="*" || search_vector<string>(select_lst, vit->name)>0){
				st_node nnode=*vit;
				nnode.owner=cur_owner;
				if(cur_owner != NULL ) nnode.place=cur_owner->name+":"+nnode.place;
				st[vit->name].push_back(nnode);
			}
		}
	}
}

/*it creates a node with the given fields*/
st_node* create_node(string name, string type, string kind, st_node* owner){
	st_node *nnode=new st_node();
	nnode->name=name;
	nnode->type=type;
	nnode->kind=kind;
	nnode->owner=owner;
	return nnode;
}

/*It checks whether type1, type2 are compatible and if so it returns the type
of their resulted arith operation. e.g. int + float gives float*/
string arith_type_compatibility(string type1, string type2){
	if(type1=="int" && type2=="int") return "int";
	if(type1=="float" || type2=="float") return "float";
	if(type1=="string" && type2=="string") return "string";
	if(type1=="char" && type2=="char") return "char";
	
	symerror("Illegal arithmetic operation between "+type1+" and "+type2);
}

/*It checks whether type1, type2 are compatible and if so it returns the type
of their resulted arith operation. e.g. int < float gives float*/
string cmp_type_compatibility(string type1, string type2){
	if(type1=="int" && type2=="int") return "int";
	if(type1=="float" || type2=="float") return "int";
	if(type1=="string" && type2=="string") return "int";
	if(type1=="char" && type2=="char") return "int";
	
	symerror("Illegal arithmetic operation between "+type1+" and "+type2);
}

/*It returns the position of the element, -1 if failure*/
template<class D>
int search_vector(vector<D>& lst, D element){
	int i=0;
	for(i=0;i<lst.size();i++)
		if(lst[i]==element) return i;
	return -1;
}


