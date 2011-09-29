
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

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"biosql_defs.h"
#define MAX_FROM_TABLES 2048

extern int lineNo;
extern int errorNo;
st_node *cur_owner=NULL;
st_node *lst=NULL; //the symbol table
st_node **from_lst=NULL;//(st_node**)malloc(sizeof(st_node*)*(MAX_FROM_TABLES+1));
int len_from_lst=0;
st_node *select_lst=NULL;
st_node *where_lst=NULL;

st_node *emit_lst=NULL;
int nextstat=1; //keep count of the lines of 

char *parsed_table_file;



/* Line 189 of yacc.c  */
#line 97 "biosql.c"

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
     TK_NEGATIVE_SIGN = 295,
     TK_POSITIVE_SIGN = 296,
     TK_RIGHT_SQUARE_BRACKET = 297,
     TK_LEFT_SQUARE_BRACKET = 298,
     TK_RIGHT_BRACKET = 299,
     TK_LEFT_BRACKET = 300
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 24 "biosql.y"

	int int_val;
	char *string;
	struct st_node *entry;



/* Line 214 of yacc.c  */
#line 186 "biosql.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 198 "biosql.c"

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
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   104

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  55
/* YYNRULES -- Number of states.  */
#define YYNSTATES  93

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   300

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
      45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     8,    10,    13,    14,    20,    23,    25,
      29,    31,    34,    37,    40,    43,    46,    49,    53,    56,
      57,    61,    63,    70,    76,    78,    80,    82,    86,    88,
      92,    94,    96,   100,   104,   108,   111,   115,   117,   121,
     125,   128,   130,   132,   134,   136,   138,   140,   142,   144,
     146,   148,   150,   152,   154,   156
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      47,     0,    -1,    54,    55,    56,    59,    -1,    48,    -1,
      49,    48,    -1,    -1,    50,    24,    52,    25,    22,    -1,
      36,    51,    -1,    31,    -1,    52,    26,    53,    -1,    53,
      -1,    27,    51,    -1,    28,    51,    -1,    29,    51,    -1,
      30,    51,    -1,    39,    51,    -1,    55,    55,    -1,    38,
      51,    22,    -1,    57,    56,    -1,    -1,    58,     7,    59,
      -1,    51,    -1,    32,    60,    33,    63,    34,    64,    -1,
      32,    60,    33,    37,    62,    -1,     8,    -1,    35,    -1,
      61,    -1,    51,    26,    61,    -1,    51,    -1,    63,    26,
      62,    -1,    63,    -1,    51,    -1,    64,    26,    64,    -1,
      64,    19,    64,    -1,    64,    20,    64,    -1,    21,    64,
      -1,    24,    64,    25,    -1,    65,    -1,    66,    67,    69,
      -1,    66,    68,    66,    -1,    68,    66,    -1,    51,    -1,
       5,    -1,     6,    -1,    13,    -1,    12,    -1,     9,    -1,
      10,    -1,    17,    -1,    18,    -1,    14,    -1,    15,    -1,
      16,    -1,     3,    -1,     4,    -1,     5,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    93,    93,    95,    98,    99,   102,   109,   123,   129,
     143,   155,   160,   165,   170,   179,   183,   184,   189,   190,
     193,   196,   204,   227,   254,   258,   261,   263,   267,   274,
     275,   278,   288,   292,   296,   300,   304,   308,   314,   353,
     360,   366,   379,   386,   387,   388,   389,   390,   393,   394,
     395,   396,   397,   399,   402,   405
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
  "TABLE", "MAPJOIN", "IMPORT", "USE", "TK_NEGATIVE_SIGN",
  "TK_POSITIVE_SIGN", "TK_RIGHT_SQUARE_BRACKET", "TK_LEFT_SQUARE_BRACKET",
  "TK_RIGHT_BRACKET", "TK_LEFT_BRACKET", "$accept", "program",
  "table_prototypes", "table_prototype", "table_keyword", "names",
  "table_args", "table_arg", "table_source", "import_tables",
  "assigned_selects", "assigned_select", "lvalue", "select_statement",
  "select_args", "select_arg_series", "from_args", "from_arg",
  "where_args", "lowest_expr", "arith_expr", "comparison_op", "arith_op",
  "rvalue", 0
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
     295,   296,   297,   298,   299,   300
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    46,    47,    47,    48,    48,    49,    50,    51,    52,
      52,    53,    53,    53,    53,    54,    55,    55,    56,    56,
      57,    58,    59,    59,    60,    60,    60,    61,    61,    62,
      62,    63,    64,    64,    64,    64,    64,    64,    65,    66,
      66,    66,    66,    67,    67,    67,    67,    67,    68,    68,
      68,    68,    68,    69,    69,    69
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     1,     2,     0,     5,     2,     1,     3,
       1,     2,     2,     2,     2,     2,     2,     3,     2,     0,
       3,     1,     6,     5,     1,     1,     1,     3,     1,     3,
       1,     1,     3,     3,     3,     2,     3,     1,     3,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     0,     0,     3,     5,     0,     0,     8,     7,
      15,     1,     4,     0,     0,    19,     0,     0,     0,     0,
       0,    10,     0,    21,    16,     0,    19,     0,    11,    12,
      13,    14,     0,     0,    17,     0,     2,    18,     0,     6,
       9,    24,    25,    28,     0,    26,    20,     0,     0,    27,
       0,    31,     0,    23,    30,     0,     0,    42,    50,    51,
      52,    48,    49,     0,     0,    41,    22,    37,     0,     0,
      29,    35,     0,     0,     0,     0,    43,    46,    47,    45,
      44,     0,     0,    40,    36,    33,    34,    32,    53,    54,
      55,    38,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,     5,     6,    65,    20,    21,     7,    24,
      25,    26,    27,    36,    44,    45,    53,    54,    66,    67,
      68,    81,    69,    91
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -60
static const yytype_int8 yypact[] =
{
     -32,     6,     6,    39,   -60,    14,    28,    16,   -60,   -60,
     -60,   -60,   -60,    52,     6,     0,     6,     6,     6,     6,
      61,   -60,    66,   -60,    16,    42,     6,    82,   -60,   -60,
     -60,   -60,    68,    52,   -60,    -5,   -60,   -60,    42,   -60,
     -60,   -60,   -60,    65,    59,   -60,   -60,     6,   -29,   -60,
       6,   -60,    60,   -60,    67,    27,     6,   -60,   -60,   -60,
     -60,   -60,   -60,    27,    27,   -60,     9,   -60,    47,     5,
     -60,    69,   -14,    27,    27,    27,   -60,   -60,   -60,   -60,
     -60,    80,     5,    55,   -60,    69,    49,     9,   -60,   -60,
     -60,   -60,    55
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -60,   -60,    91,   -60,   -60,    -1,   -60,    64,   -60,    92,
      72,   -60,   -60,    62,   -60,    54,    46,    56,     3,   -60,
     -42,   -60,   -59,   -60
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
       9,    10,     8,    41,     1,    73,    74,     2,    50,    82,
      57,    84,    75,    22,    23,    28,    29,    30,    31,    58,
      59,    60,    61,    62,    82,    23,     8,    83,    73,    74,
      42,     8,    57,    82,    43,    75,     8,     8,    14,    11,
      92,    58,    59,    60,    61,    62,    43,    51,    63,    51,
       1,    64,    13,    76,    14,    51,    77,    78,     8,    79,
      80,    58,    59,    60,    61,    62,    71,    72,    73,    58,
      59,    60,    61,    62,    35,    75,    85,    86,    87,    16,
      17,    18,    19,    88,    89,    90,    32,    33,    34,    38,
      39,    47,    48,    56,    55,    75,    12,    40,    37,    15,
      46,    49,    70,     0,    52
};

static const yytype_int8 yycheck[] =
{
       1,     2,    31,     8,    36,    19,    20,    39,    37,    68,
       5,    25,    26,    14,    15,    16,    17,    18,    19,    14,
      15,    16,    17,    18,    83,    26,    31,    69,    19,    20,
      35,    31,     5,    92,    35,    26,    31,    31,    38,     0,
      82,    14,    15,    16,    17,    18,    47,    48,    21,    50,
      36,    24,    24,     6,    38,    56,     9,    10,    31,    12,
      13,    14,    15,    16,    17,    18,    63,    64,    19,    14,
      15,    16,    17,    18,    32,    26,    73,    74,    75,    27,
      28,    29,    30,     3,     4,     5,    25,    26,    22,     7,
      22,    26,    33,    26,    34,    26,     5,    33,    26,     7,
      38,    47,    56,    -1,    48
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    36,    39,    47,    48,    49,    50,    54,    31,    51,
      51,     0,    48,    24,    38,    55,    27,    28,    29,    30,
      52,    53,    51,    51,    55,    56,    57,    58,    51,    51,
      51,    51,    25,    26,    22,    32,    59,    56,     7,    22,
      53,     8,    35,    51,    60,    61,    59,    26,    33,    61,
      37,    51,    63,    62,    63,    34,    26,     5,    14,    15,
      16,    17,    18,    21,    24,    51,    64,    65,    66,    68,
      62,    64,    64,    19,    20,    26,     6,     9,    10,    12,
      13,    67,    68,    66,    25,    64,    64,    64,     3,     4,
       5,    69,    66
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
        case 6:

/* Line 1455 of yacc.c  */
#line 102 "biosql.y"
    {cur_owner=NULL;
	//if(strcmp($1->name, "READS")!=0){
		emit(&emit_lst, "end_load_table", NULL, NULL, NULL,NULL);
	//}
;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 109 "biosql.y"
    {
	if(lookup(lst, (yyvsp[(2) - (2)].string), NULL, NULL, NULL)==NULL){
		(yyval.entry)=create_node((yyvsp[(2) - (2)].string), "table", "table", NULL);
		cur_owner=(yyval.entry);
		add_node(&lst, (yyval.entry));
		//if(strcmp($2, "READS")!=0){
			emit(&emit_lst, "load_table", (yyval.entry)->name, NULL, NULL,NULL);
		//}
	}
	else symerror("Conflict with name tables");
;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 123 "biosql.y"
    {
	(yyval.string)=(char*)malloc(strlen(yylval.string)+3);
	strcpy((yyval.string), yylval.string);
;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 129 "biosql.y"
    {
//arguments need to appear in output in the order that the user enders them.

	//if(strcmp(cur_owner->name, "READS")!=0){
		if(strcmp((yyvsp[(3) - (3)].entry)->type,"integer")==0)
			emit(&emit_lst, "loaded_param", "int", (yyvsp[(3) - (3)].entry)->name, NULL,NULL);
		else if(strcmp((yyvsp[(3) - (3)].entry)->type,"string")==0)
			emit(&emit_lst, "loaded_param", "char*", (yyvsp[(3) - (3)].entry)->name, NULL,NULL);
		else
			emit(&emit_lst, "loaded_param", (yyvsp[(3) - (3)].entry)->type, (yyvsp[(3) - (3)].entry)->name, NULL,NULL);
	//}
	//free(tmp_str);
;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 143 "biosql.y"
    {
	//if(strcmp(cur_owner->name, "READS")!=0){
		if(strcmp((yyvsp[(1) - (1)].entry)->type,"integer")==0)
			emit(&emit_lst, "loaded_param", "int", (yyvsp[(1) - (1)].entry)->name, NULL,NULL);
		else if(strcmp((yyvsp[(1) - (1)].entry)->type,"string")==0)
			emit(&emit_lst, "loaded_param", "char*", (yyvsp[(1) - (1)].entry)->name, NULL,NULL);
		else
			emit(&emit_lst, "loaded_param", (yyvsp[(1) - (1)].entry)->type, (yyvsp[(1) - (1)].entry)->name, NULL,NULL);
	//}
;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 155 "biosql.y"
    {
	(yyval.entry)=check_and_create(lst, (yyvsp[(2) - (2)].string), "integer", "attribute", cur_owner);
	if((yyval.entry)!=NULL) add_node(&lst, (yyval.entry));
	else symerror("Conflicting attribute names");
	;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 160 "biosql.y"
    {
	(yyval.entry)=check_and_create(lst, (yyvsp[(2) - (2)].string), "float", "attribute", cur_owner);
	if((yyval.entry)!=NULL) add_node(&lst, (yyval.entry));
	else symerror("Conflicting attribute names");
	;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 165 "biosql.y"
    {
	(yyval.entry)=check_and_create(lst, (yyvsp[(2) - (2)].string), "char", "attribute", cur_owner);
	if((yyval.entry)!=NULL) add_node(&lst, (yyval.entry));
	else symerror("Conflicting attribute names");
	;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 170 "biosql.y"
    {
	(yyval.entry)=check_and_create(lst, (yyvsp[(2) - (2)].string), "string", "attribute", cur_owner);
	if((yyval.entry)!=NULL) add_node(&lst, (yyval.entry));
	else symerror("Conflicting attribute names");
	;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 179 "biosql.y"
    {
	parsed_table_file=strdup((yyvsp[(2) - (2)].string));
;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 184 "biosql.y"
    {
	load_table(&lst, &emit_lst, parsed_table_file, (yyvsp[(2) - (3)].string));
;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 193 "biosql.y"
    {emit(&emit_lst, "result_table", ":", (yyvsp[(1) - (3)].entry)->name, NULL,NULL); cur_owner=NULL;;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 196 "biosql.y"
    {
	(yyval.entry)=check_and_create(lst, (yyvsp[(1) - (1)].string), "table", "table", NULL);
	if((yyval.entry)==NULL) symerror("assigned table already exists");
	add_node(&lst, (yyval.entry));
	cur_owner=(yyval.entry);
;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 204 "biosql.y"
    {
	if(1/*cur_owner != NULL*/){ //indication of assigned select...need to update the symbol table.
		if(select_lst->name[0]=='*'){
			replicate_args(from_lst, len_from_lst, lst, cur_owner); //All properties of the tables in from_args are replicated under a new owner.
			emit(&emit_lst, "return_arg",":", "*",NULL, NULL);
		}
		else if(strcmp(select_lst->name, "countvec")==0){
			add_node(&lst, create_node("countvec", "integer", "attribute", cur_owner));
			emit(&emit_lst, "return_arg",":","strength_vector",NULL, NULL);
		}
		else{
			check_and_copy_from_mult(&lst, select_lst, from_lst, len_from_lst, cur_owner);
			st_node *tmp;
			for(tmp=select_lst;tmp!=NULL;tmp=tmp->next) emit(&emit_lst, "return_arg",":",tmp->name,NULL, NULL);
		}
		
	}
	len_from_lst=0;
	where_lst=NULL;
	destroy_list(select_lst);
	select_lst=NULL;

;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 227 "biosql.y"
    {
	
	squeeze_node(emit_lst, "Input", "begin_intersect"); //add a flag to help with code generation

	emit(&emit_lst, "end_intersect",NULL, NULL,NULL, NULL);
	if(select_lst->name[0]=='*'){
		replicate_args(from_lst, len_from_lst, lst, cur_owner); //All properties of the tables in from_args are replicated under a new owner.
		///////////////emit(&emit_lst, "return_arg",":", "*",NULL, NULL);
	}
	else if(strcmp(select_lst->name, "countvec")==0){
		add_node(&lst, create_node("countvec", "integer", "attribute", cur_owner));
		emit(&emit_lst, "return_arg",":","strength_vector",NULL, NULL);
	}
	else{
		check_and_copy_from_mult(&lst, select_lst, from_lst, len_from_lst, cur_owner);
		st_node *tmp;
		for(tmp=select_lst;tmp!=NULL;tmp=tmp->next) emit(&emit_lst, "return_arg",":",tmp->name,NULL, NULL);
	}

	len_from_lst=0;
	where_lst=NULL;
	destroy_list(select_lst);
	select_lst=NULL;

;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 254 "biosql.y"
    {
	add_node(&select_lst, create_node("*",NULL, NULL, NULL));

;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 258 "biosql.y"
    {
	add_node(&select_lst, create_node("countvec", NULL, NULL, NULL));
;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 263 "biosql.y"
    {
	//printf("@@adding %s\n",$1);
	add_node(&select_lst, create_node((yyvsp[(1) - (3)].string), NULL, NULL, NULL)); //remembers which names have been encountered by select
;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 267 "biosql.y"
    {
	//printf("@@adding %s\n",$1);
	add_node(&select_lst, create_node((yyvsp[(1) - (1)].string), NULL, NULL, NULL)); //remembers which names have been encountered by select
;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 278 "biosql.y"
    {
	(yyval.entry)=lookup(lst, (yyvsp[(1) - (1)].string), "table", "table", NULL);
	//st_node *tmp;
	//for (tmp=lst;tmp!=NULL;tmp=tmp->next) printf("name: %s type: %s kind: %s owner: %x\n",tmp->name, tmp->type, tmp->kind, tmp->owner);
	if((yyval.entry)==NULL) symerror("Unknown table in the FROM statement");
	add_node_to_array(from_lst, &len_from_lst, (yyval.entry));
	emit(&emit_lst, "Input","=",(yyval.entry)->name,NULL, NULL);
;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 288 "biosql.y"
    { //the type of where_args is str and denotes the respective temporary variable
	(yyval.string)=get_newtemp();
	emit(&emit_lst, (yyval.string),"=",(yyvsp[(1) - (3)].string), "&&", (yyvsp[(3) - (3)].string));
;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 292 "biosql.y"
    {
(yyval.string)=get_newtemp();
	emit(&emit_lst, (yyval.string),"=",(yyvsp[(1) - (3)].string), "&&", (yyvsp[(3) - (3)].string));
;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 296 "biosql.y"
    {
	(yyval.string)=get_newtemp();
	emit(&emit_lst, (yyval.string),"=",(yyvsp[(1) - (3)].string), "||", (yyvsp[(3) - (3)].string));
;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 300 "biosql.y"
    {
	(yyval.string)=get_newtemp();
	emit(&emit_lst, (yyval.string),"=","!",(yyvsp[(2) - (2)].string), NULL);
;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 304 "biosql.y"
    {
	(yyval.string)=strdup((yyvsp[(2) - (3)].string));
	//emit(&emit_lst, $$,"=",$1.place, NULL, NULL);
;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 308 "biosql.y"
    {
	(yyval.string)=strdup((yyvsp[(1) - (1)].string));
;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 314 "biosql.y"
    {

	if (strcmp((yyvsp[(1) - (3)].entry)->type, (yyvsp[(3) - (3)].entry)->type)!=0) symerror("Incompatible types in a where expression");

	char *tmp_str1=(char*)malloc(1024);
	char *tmp_str2=(char*)malloc(1024);
	
	if (strcmp((yyvsp[(1) - (3)].entry)->type, "string")!=0){
		sprintf(tmp_str1,"%s %s %s",(yyvsp[(1) - (3)].entry)->place,(yyvsp[(2) - (3)].string),(yyvsp[(3) - (3)].entry)->name);
		sprintf(tmp_str2,"%d",nextstat+3);
	}
	else{
		if(strcmp((yyvsp[(2) - (3)].string),"==")==0)
			sprintf(tmp_str1, "(strcmp( %s , %s )==0)", (yyvsp[(1) - (3)].entry)->place, (yyvsp[(3) - (3)].entry)->name);
		else if(strcmp((yyvsp[(2) - (3)].string),"!=")==0)
			sprintf(tmp_str1, "(strcmp( %s , %s )!=0)", (yyvsp[(1) - (3)].entry)->place, (yyvsp[(3) - (3)].entry)->name);
		else if((yyvsp[(2) - (3)].string)[0]=='>')
			sprintf(tmp_str1, "(strcmp( %s , %s )>0)", (yyvsp[(1) - (3)].entry)->place, (yyvsp[(3) - (3)].entry)->name);
		else 
			sprintf(tmp_str1, "(strcmp( %s , %s )<0)", (yyvsp[(1) - (3)].entry)->place, (yyvsp[(3) - (3)].entry)->name);
	}


	

	(yyval.string)=get_newtemp();
	/*emit(&emit_lst, "if", tmp_str1, "goto",tmp_str2,NULL);//original impl
	emit(&emit_lst, $$,"=","0",NULL, NULL);*/
	emit(&emit_lst, (yyval.string), "=", tmp_str1,NULL, NULL);
	/*sprintf(tmp_str2,"%d",nextstat+2);
	emit(&emit_lst, "goto", tmp_str2,NULL, NULL, NULL);
	emit(&emit_lst, $$,"=","1",NULL,NULL);*/

	free(tmp_str1);
	free(tmp_str2);

;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 353 "biosql.y"
    {
	if(strcmp((yyvsp[(1) - (3)].entry)->type,"integer")!=0 || strcmp((yyvsp[(3) - (3)].entry)->type,"integer")!=0) symerror("Only integers are allowed in arithmetic operations");
	(yyval.entry)=create_node((yyvsp[(1) - (3)].entry)->name, (yyvsp[(1) - (3)].entry)->type, (yyvsp[(1) - (3)].entry)->kind, (yyvsp[(1) - (3)].entry)->owner);
	(yyval.entry)->place=get_newtemp();
	emit(&emit_lst, (yyval.entry)->place, "=", (yyvsp[(1) - (3)].entry)->place, (yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].entry)->place);
;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 360 "biosql.y"
    {
	if(strcmp((yyvsp[(2) - (2)].entry)->type,"integer")!=0) symerror("Only integers are allowed in arithmetic operations");
	(yyval.entry)=create_node((yyvsp[(2) - (2)].entry)->name, (yyvsp[(2) - (2)].entry)->type, (yyvsp[(2) - (2)].entry)->kind, (yyvsp[(2) - (2)].entry)->owner);
	(yyval.entry)->place=get_newtemp();
	emit(&emit_lst, (yyval.entry)->place, "=", (yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].entry)->place, NULL);
;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 366 "biosql.y"
    {
	st_node *tmp=lookup_mult_tables(from_lst, len_from_lst, lst, (yyvsp[(1) - (1)].string));
	if (tmp==NULL){
		printf("name: %s\n",(yyvsp[(1) - (1)].string));
		symerror("Unknown attribute in where expressions\n");
	}
	if(tmp->kind==NULL || strcmp(tmp->kind, "attribute")!=0) symerror("Illegal non attribute in an arithmetic op");
	(yyval.entry)=create_node(tmp->name, tmp->type, tmp->kind, tmp->owner);
	(yyval.entry)->place=strdup(tmp->name);
	//printf("tmp: %s type %s $3 %s type %s\n",tmp->name, tmp->type, $3->name, $3->type);

;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 379 "biosql.y"
    {
	(yyval.entry)=create_node(yylval.string, "integer", NULL, NULL);
	(yyval.entry)->place=strdup((yyval.entry)->name);

;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 386 "biosql.y"
    {(yyval.string)=strdup("==");;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 387 "biosql.y"
    {(yyval.string)=strdup(">=");;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 388 "biosql.y"
    {(yyval.string)=strdup("<=");;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 389 "biosql.y"
    {(yyval.string)=strdup(">");;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 390 "biosql.y"
    {(yyval.string)=strdup("<");;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 393 "biosql.y"
    {(yyval.string)=strdup("+");;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 394 "biosql.y"
    {(yyval.string)=strdup("-");;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 395 "biosql.y"
    {(yyval.string)=strdup("x");;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 396 "biosql.y"
    {(yyval.string)=strdup("/");;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 397 "biosql.y"
    {(yyval.string)=strdup("%");;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 399 "biosql.y"
    {
	(yyval.entry)=create_node(yylval.string, "char", NULL, NULL);
;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 402 "biosql.y"
    {
	(yyval.entry)=create_node(yylval.string, "string", NULL, NULL);
;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 405 "biosql.y"
    {
	(yyval.entry)=create_node(yylval.string, "integer", NULL, NULL);
;}
    break;



/* Line 1455 of yacc.c  */
#line 2001 "biosql.c"
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
#line 410 "biosql.y"

extern FILE *yyin;

int main(){
	//yydebug=1;
	yylval.entry = (st_node*) malloc(sizeof(st_node));
	yylval.entry->next=NULL;

	from_lst=(st_node**)malloc(sizeof(st_node*)*(MAX_FROM_TABLES+1));

	yyparse();
	if(errorNo==0){
		printf("Compilation completed without errors\n");
		fprintf(stderr,"Compilation completed without errors\n");
		output_emit_list(emit_lst);
		//writeToFile(target_file);//the intermediate code will be printed at argv[1].interm
	}
	else{
		printf("There were %d errors\n",errorNo);
		fprintf(stderr,"There were %d errors\n",errorNo);
	}

	/*st_node *tmp=lst;
	for(tmp=lst;tmp!=NULL;tmp=tmp->next){
		printf("%s %s %s",tmp->name, tmp->type, tmp->kind);
		if (tmp->owner!=NULL) printf(" %s\n",tmp->owner->name);
		else printf("\n");
	}*/
	

	return 0;
}

int yyerror()
{ 

	printf("\nline %d: syntax Error",lineNo);
	///exit(2);
	errorNo++;
	if(errorNo>5){
		printf("\nToo many errors to continue");
		exit(1);
	}
	
	//return 1;
	
}

int symerror(char *msg){
	printf("\nline %d: Symbol Error: %s",lineNo, msg);
	exit(2);
	errorNo++;
	if(errorNo>5){
		printf("\nToo many errors to continue");
		exit(1);
	}
	
	//return 1;
	
}

void ioerror(char *msg){
	fprintf(stderr, "FATAL ERROR!!! %s",msg);
	exit(2);
}

/*lst is the head of a list of st_node. The function
returns the node whose name, type, kind and owner match the input. If any of
type, kind, owner is NULL it is considered as wildcard.
The function returns NULL on failure*/
st_node *lookup(st_node *lst, char *name, char *type, char *kind, st_node *owner){
	st_node *ret;
	//printf("name: %s, lst: %x\n",name, lst);
	if(lst==NULL) return NULL;

	for (ret=lst;ret!=NULL;ret=ret->next){
		if(strcmp(name, ret->name)==0){
			if(type==NULL || strcmp(type, ret->type)==0){
				if(kind==NULL || strcmp(kind, ret->kind)==0){
					if(owner==NULL || owner==ret->owner){
						return ret;
					}
				}
			}
		}
	}
	return NULL;
}

//it creates a node whose properties are initialized from the input args
st_node *create_node(char *name, char *type, char *kind, st_node *owner){
	st_node *tmp=(st_node*)malloc(sizeof(st_node));
	if(name!=NULL) tmp->name=strdup(name); else tmp->name=NULL;
	if(type!=NULL) tmp->type=strdup(type); else tmp->type=NULL;
	if(kind!=NULL) tmp->kind=strdup(kind); else tmp->kind=NULL;
	tmp->owner=owner;
	tmp->next=NULL;
	tmp->place=NULL;
	return tmp;
}



/*lst is the head of a list of st_node. The function creates a node
whose data are taken from the input and adds it at the beginning of the list*/
//void add_node(st_node *lst, char *name, char *type, char *kind, st_node *owner){
/*lst is the head of a list of st_node in which newnode is going to be added
at the beginning of the list*/
void add_node(st_node **lst, st_node *newnode){
	if ((*lst)==NULL) *lst=newnode;
	else{
		newnode->next=*lst;
		*lst=newnode;
	}
}

//It creates a node with name node_info and adds it to the list at the place that precedes
//where_str. For example if the list consists of A->B->C and where_str=B, the new node
//is going to be placed between B and C. Now if there are multiple B's such as A->B->B-C
//the new node is going to be inserted between B and C
void squeeze_node(st_node *lst, char *where_str, char *node_info){
	st_node *tmp=lst;
	st_node *new_tmp_nxt;
	for(tmp=lst;tmp!=NULL;tmp=tmp->next){
		if(tmp->next==NULL) break;
		if(strstr(tmp->name, where_str)!=NULL && strstr(tmp->next->name, where_str)==NULL)
			break;
	}
	new_tmp_nxt=tmp->next;
	add_node(&new_tmp_nxt, create_node(node_info, NULL, NULL, NULL));
	tmp->next=new_tmp_nxt;
}

//It returns that node of lst that has been entered prior to list_node. In this implementation
//the answer is lst_node->next, but it can change if the list implementation changes.
st_node *get_previous_node(st_node *lst_node){
	if(lst_node==NULL) return NULL;
	else return lst_node->next;
}

//It adds newnode to the array list lst of length len_lst. The basic operation is
//lst[n++]=newnode
void add_node_to_array(st_node **lst, int *len_lst, st_node *newnode){
	int n=*len_lst;
	if(n>=MAX_FROM_TABLES) symerror("Cannot handle that many tables in the from statement\n");
	lst[n++]=newnode;
	*len_lst=n;
}


void destroy_list(st_node *lst){
	st_node *tmp, *vic;
	tmp=lst;
	while(tmp!=NULL){
		vic=tmp;
		tmp=tmp->next;
		free(vic->name);
		vic->name=NULL;
		free(vic->type);
		vic->type=NULL;
		free(vic->kind);
		vic->kind=NULL;
		free(vic);
		vic=NULL;
	}
	return;
}

//If lst does not contain already an attribute with the same name and the same
//owner, it creates one. Otherwise it returns NULL
st_node *check_and_create(st_node *lst, char *name, char *type, char *kind, st_node *cur_owner){
	st_node *ret=NULL;
	if (lookup(lst, name, type, "attribute", cur_owner)==NULL)
		ret=create_node(name, type, kind, cur_owner);
	return ret;
}

//It returns that node of lst where the name matches and the owner is one
//of the tables of from_lst. Both lists contain pointers to the same 
//objects
st_node *lookup_mult_tables(st_node **from_lst, int len_from_lst, st_node *lst, char *name){
	st_node *tmp;
	st_node *hit=NULL;
	int i=0;
	//printf("!!!!!!!looking up name: %s tblist: %x\n",name, tbl_lst);
	for(i=0;i<MAX_FROM_TABLES;i++){
		tmp=from_lst[i];
		//printf("tbl name: %s tmp: %x\n",tmp->name,tmp);
		//for(hit=lst;hit!=NULL;hit=hit->next) if(strcmp("arg21", name)==0)printf(">>>name: %s kind: %s owner: %x\n",hit->name, hit->kind, hit->owner);
		hit=lookup(lst, name, NULL, "attribute", tmp);
		if(hit!=NULL){
			//printf("actual hit: %x\n",hit);
			return hit;
		}
	}
	//printf("returning NULL\n");
	return NULL;
}

//it checks whether each of the nodes of arg_lst is a property of any of the 
//tables in the "from_args" and adds them in lst with owner as specified
//by the input
void check_and_copy_from_mult(st_node **lst, st_node *arg_lst, st_node **from_lst, int len_from_lst, st_node *owner){
	st_node *tmp;
	st_node *lkup;
	//printf("tmp: %s\n",arg_lst->name);
	for (tmp=arg_lst;tmp!=NULL;tmp=tmp->next){
		lkup=lookup_mult_tables(from_lst, len_from_lst, *lst, tmp->name);
		if(lkup==NULL) symerror("An attribute in select is not contained in the from tables\n");
		//printf(">>>adding %s %s\n",lkup->name, lkup->type);
		add_node(lst, create_node(lkup->name, lkup->type, lkup->kind, owner));
	}
}



//It replicates all properties of lst whose owner is a table in the from_args
//and cur_owner is the owner of the new attributes.
void replicate_args(st_node **from_lst, int len_from_lst, st_node *lst, st_node *cur_owner){
	st_node *tmp;
	int i=0;
	for (tmp=lst;tmp!=NULL;tmp=tmp->next){
		if(strcmp(tmp->kind, "attribute")==0 && (tmp->owner!=NULL)){
			for(i=0;i<len_from_lst;i++){
				if(strcmp(from_lst[i]->name, tmp->owner->name)==0 && strcmp(from_lst[i]->kind, "table")==0){
					/////if(lookup(from_lst, tmp->owner->name, "table", "table", NULL)!=NULL){ //check if the owner table of tmp is in from_lst
					add_node(&lst, create_node(tmp->name, tmp->type, tmp->kind, cur_owner));
					break;
				}
			}
		}
	}
	return;
}

//it adds to em_lst an st_node whose name is the concatenation of str1...4 
//and also increases nextstat.
void emit(st_node **em_lst, char *str1, char *str2, char *str3, char *str4, char *str5){
	char *str=(char*)malloc(2048);
	int cnt=0;
	*str='\0';
	if(str1!=NULL){
		strcat(str, str1);
		strcat(str," ");
		cnt+=strlen(str1);
	}
	if(str2!=NULL){
		strcat(str, str2);
		strcat(str," ");
		cnt+=strlen(str2);
	}
	if(str3!=NULL){
		strcat(str, str3);
		strcat(str," ");
		cnt+=strlen(str3);
	}
	if(str4!=NULL){
		strcat(str, str4);
		strcat(str," ");
		cnt+=strlen(str4);
	}
	if(str5!=NULL){
		strcat(str, str5);
		strcat(str," ");
		cnt+=strlen(str5);
	}
	if(cnt>=2048){ioerror("Too many characters appended to emit");}
	add_node(em_lst, create_node(str, NULL, NULL, NULL));
	nextstat++;
}

//it traverses lst backwards and outputs the name of each node.
//Remember: nodes have been added to the top of the list each time.
//So the list needs to be tranversed in a reverse direction.
void output_emit_list(st_node *lst){
	static int i=1;
	if (lst==NULL) return;
	output_emit_list(lst->next);
	printf("%d. %s\n", i++, lst->name);
}


//it generates a new temporary variable
char *get_newtemp(){
	static int cnt=0;
	char *ret=(char *)malloc(sizeof(char)*(50));
	sprintf(ret, "t%d",cnt++);
	return ret;
}

//It opens the file that contains the parsed tables and it loads the table and the respective
//parameters to symbl_lst. If name!=READS, it also populates emit_lst with the proper commands
void load_table(st_node **symbl_lst, st_node **emit_lst, char *parsed_table_file, char *name){
	FILE *fp=fopen(parsed_table_file, "r");
	if(fp==NULL) ioerror("Cannot open the file of parsed_tables");
	char *buf=malloc(2048);
	char *tok;
	int table_found=0;
	char *type;
	char *pname;
	st_node *nd;
	if (fgets(buf, 2048, fp)==NULL) ioerror("Can't read the first line");
	while(fgets(buf, 2048, fp)!=NULL){
		if (strstr(buf, " load_table")!=NULL && strstr(buf, name)!=NULL){//the table was located
			table_found=1;
			if(lookup(*symbl_lst, name, NULL, NULL, NULL)==NULL){
				nd=create_node(name, "table", "table", NULL);
				cur_owner=nd;
				add_node(symbl_lst, nd);
				if(strcmp(name,"READS")!=0)
					emit(emit_lst, "load_table", name, NULL, NULL,NULL);
			}
			else symerror("Conflict with table names");
		}
		else if(strstr(buf,"loaded_param")!=NULL && table_found){
			tok=strtok(buf, " "); //the line No
			tok=strtok(NULL, " "); //the "loaded_param" keyword
			tok=strtok(NULL, " "); //the type
			type=tok;
			tok=strtok(NULL, " "); //the parameter name
			pname=tok;
			//pname[strlen(pname)-1]='\0'; //get rid of '\n'
			if (strcmp(type, "int")==0)
				nd=check_and_create(*symbl_lst, pname, "integer", "attribute", cur_owner);
			else if(strcmp(type, "float")==0)
				nd=check_and_create(*symbl_lst, pname, "float", "attribute", cur_owner);
			else if(strcmp(type, "char")==0)
				nd=check_and_create(*symbl_lst, pname, "char", "attribute", cur_owner);
			else if(strcmp(type, "char*")==0)
				nd=check_and_create(*symbl_lst, pname, "string", "attribute", cur_owner);
			else symerror("Unknown type");
			if(nd!=NULL) add_node(symbl_lst, nd);
			else symerror("Conflicting attribute names");
			if(strcmp(name, "READS")!=0)
				emit(emit_lst, "loaded_param", type, pname, NULL,NULL);
		}
		else if(strstr(buf, "end_load_table")!=NULL && table_found){
			table_found=0;
			if(strcmp(name, "READS")!=0)
				emit(emit_lst, "end_load_table", NULL, NULL, NULL,NULL);
			free(buf);
			fclose(fp);
			return;
		}
	}
	symerror("Cannot import a table with the given name");
}

