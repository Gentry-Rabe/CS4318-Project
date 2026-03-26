/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/parser.y"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<../src/tree.h>
#include<../src/strtab.h>

extern int yylineno;
extern int yylex(void);

int yywarning(char *msg);
int yyerror(char *msg);


typedef struct treenode tree;
extern tree *ast;
//This is really dumb but I don't want to bother making an accessor.
extern struct strEntry strTable[MAXIDS];
extern int yycol;
extern char *scan_error_msg;

/* nodeTypes refer to different types of internal and external nodes
  that can be part of the abstract syntax tree.
  */
enum nodeTypes {PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
                FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
                STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
                CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
                ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
                ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR,
                FUNCTYPENAME};

enum opType {ADD, SUB, MUL, DIV, LT, LTE, EQ, GTE, GT, NEQ};

/* NOTE: mC has two kinds of scopes for variables : local and global.
  Variables declared outside any function are considered globals,
    whereas variables (and parameters) declared inside a function foo
    are local to foo.
  You should update the scope variable whenever you are inside a production
    that matches function definition (funDecl production).
  The rationale is that you are entering that function, so all variables,
    arrays, and other functions should be within this scope.
  You should pass this variable whenever you are
    calling the ST_insert or ST_lookup functions.
  This variable should be updated to scope = ""
    to indicate global scope whenever funDecl finishes.
  Treat these hints as helpful directions only.
  You may implement all of the functions as you like
    and not adhere to my instructions.
  As long as the directory structure is correct and the file names are correct,
    we are okay with it.
  */
static char scopeBuf[256] = "";
static char *scope = scopeBuf;

static void set_scope(const char *name) {
    if (!name) {
        name = "";
    }
    snprintf(scopeBuf, sizeof(scopeBuf), "%s", name);
}

static int resolve_symbol(const char *id) {
    int idx = ST_lookup((char *)id, scope);
    if (idx == -1 && scope[0] != '\0') {
        idx = ST_lookup((char *)id, "");
    }
    return idx;
}

static int resolve_function(const char *id) {
    int idx = ST_lookup((char *)id, "");
    if (idx == -1) {
        return -1;
    }
    if (strTable[idx].symbol_type != FUNCTION) {
        return -1;
    }
    return idx;
}

static void addAllChildren(tree *parent, tree *node) {
    if (!parent || !node) return;
    if (node->nodeKind == parent->nodeKind) {
        for (int i = 0; i < node->numChildren; ++i) {
            addChild(parent, node->children[i]);
        }
    } else {
        addChild(parent, node);
    }
}


#line 165 "obj/y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "y.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_INTCONST = 4,                   /* INTCONST  */
  YYSYMBOL_CHARCONST = 5,                  /* CHARCONST  */
  YYSYMBOL_KWD_IF = 6,                     /* KWD_IF  */
  YYSYMBOL_KWD_ELSE = 7,                   /* KWD_ELSE  */
  YYSYMBOL_KWD_WHILE = 8,                  /* KWD_WHILE  */
  YYSYMBOL_KWD_INT = 9,                    /* KWD_INT  */
  YYSYMBOL_KWD_CHAR = 10,                  /* KWD_CHAR  */
  YYSYMBOL_KWD_RETURN = 11,                /* KWD_RETURN  */
  YYSYMBOL_KWD_VOID = 12,                  /* KWD_VOID  */
  YYSYMBOL_OPER_ADD = 13,                  /* OPER_ADD  */
  YYSYMBOL_OPER_SUB = 14,                  /* OPER_SUB  */
  YYSYMBOL_OPER_MUL = 15,                  /* OPER_MUL  */
  YYSYMBOL_OPER_DIV = 16,                  /* OPER_DIV  */
  YYSYMBOL_OPER_LT = 17,                   /* OPER_LT  */
  YYSYMBOL_OPER_GT = 18,                   /* OPER_GT  */
  YYSYMBOL_OPER_GTE = 19,                  /* OPER_GTE  */
  YYSYMBOL_OPER_LTE = 20,                  /* OPER_LTE  */
  YYSYMBOL_OPER_EQ = 21,                   /* OPER_EQ  */
  YYSYMBOL_OPER_NEQ = 22,                  /* OPER_NEQ  */
  YYSYMBOL_OPER_ASGN = 23,                 /* OPER_ASGN  */
  YYSYMBOL_LSQ_BRKT = 24,                  /* LSQ_BRKT  */
  YYSYMBOL_RSQ_BRKT = 25,                  /* RSQ_BRKT  */
  YYSYMBOL_LCRLY_BRKT = 26,                /* LCRLY_BRKT  */
  YYSYMBOL_RCRLY_BRKT = 27,                /* RCRLY_BRKT  */
  YYSYMBOL_LPAREN = 28,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 29,                    /* RPAREN  */
  YYSYMBOL_COMMA = 30,                     /* COMMA  */
  YYSYMBOL_SEMICLN = 31,                   /* SEMICLN  */
  YYSYMBOL_ERROR = 32,                     /* ERROR  */
  YYSYMBOL_ILLEGAL_TOK = 33,               /* ILLEGAL_TOK  */
  YYSYMBOL_LOWER_THAN_ELSE = 34,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_YYACCEPT = 35,                  /* $accept  */
  YYSYMBOL_program = 36,                   /* program  */
  YYSYMBOL_declList = 37,                  /* declList  */
  YYSYMBOL_decl = 38,                      /* decl  */
  YYSYMBOL_typeSpecifier = 39,             /* typeSpecifier  */
  YYSYMBOL_varDecl = 40,                   /* varDecl  */
  YYSYMBOL_funDecl = 41,                   /* funDecl  */
  YYSYMBOL_42_1 = 42,                      /* $@1  */
  YYSYMBOL_optFormalDeclList = 43,         /* optFormalDeclList  */
  YYSYMBOL_formalDeclList = 44,            /* formalDeclList  */
  YYSYMBOL_formalDecl = 45,                /* formalDecl  */
  YYSYMBOL_funBody = 46,                   /* funBody  */
  YYSYMBOL_statementList = 47,             /* statementList  */
  YYSYMBOL_statement = 48,                 /* statement  */
  YYSYMBOL_compoundStmt = 49,              /* compoundStmt  */
  YYSYMBOL_assignStmt = 50,                /* assignStmt  */
  YYSYMBOL_condStmt = 51,                  /* condStmt  */
  YYSYMBOL_loopStmt = 52,                  /* loopStmt  */
  YYSYMBOL_returnStmt = 53,                /* returnStmt  */
  YYSYMBOL_var = 54,                       /* var  */
  YYSYMBOL_expression = 55,                /* expression  */
  YYSYMBOL_relop = 56,                     /* relop  */
  YYSYMBOL_addExpr = 57,                   /* addExpr  */
  YYSYMBOL_addop = 58,                     /* addop  */
  YYSYMBOL_term = 59,                      /* term  */
  YYSYMBOL_mulop = 60,                     /* mulop  */
  YYSYMBOL_factor = 61,                    /* factor  */
  YYSYMBOL_funCallExpr = 62,               /* funCallExpr  */
  YYSYMBOL_argList = 63                    /* argList  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   138

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  29
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  107

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   159,   159,   168,   173,   181,   186,   194,   195,   196,
     200,   209,   223,   222,   248,   251,   258,   263,   272,   281,
     293,   302,   305,   314,   318,   320,   322,   324,   326,   328,
     333,   341,   347,   354,   360,   370,   379,   383,   391,   401,
     415,   417,   427,   428,   429,   430,   431,   432,   436,   438,
     448,   449,   453,   455,   465,   466,   470,   472,   474,   476,
     478,   483,   494,   507,   512
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "INTCONST",
  "CHARCONST", "KWD_IF", "KWD_ELSE", "KWD_WHILE", "KWD_INT", "KWD_CHAR",
  "KWD_RETURN", "KWD_VOID", "OPER_ADD", "OPER_SUB", "OPER_MUL", "OPER_DIV",
  "OPER_LT", "OPER_GT", "OPER_GTE", "OPER_LTE", "OPER_EQ", "OPER_NEQ",
  "OPER_ASGN", "LSQ_BRKT", "RSQ_BRKT", "LCRLY_BRKT", "RCRLY_BRKT",
  "LPAREN", "RPAREN", "COMMA", "SEMICLN", "ERROR", "ILLEGAL_TOK",
  "LOWER_THAN_ELSE", "$accept", "program", "declList", "decl",
  "typeSpecifier", "varDecl", "funDecl", "$@1", "optFormalDeclList",
  "formalDeclList", "formalDecl", "funBody", "statementList", "statement",
  "compoundStmt", "assignStmt", "condStmt", "loopStmt", "returnStmt",
  "var", "expression", "relop", "addExpr", "addop", "term", "mulop",
  "factor", "funCallExpr", "argList", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-46)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      64,   -46,   -46,   -46,     2,    64,   -46,    25,   -46,   -46,
     -46,   -46,    -4,    37,   -46,    16,    45,    64,    44,    88,
      63,   -46,    66,   -46,    73,    72,    64,    90,     3,   -46,
     -46,   -46,    35,   -46,   -46,    85,    86,    14,     3,    33,
     -46,   113,   -46,   105,     3,   -46,   -46,   -46,   -46,   -46,
      94,    47,    -9,    42,   -46,   -46,    33,    21,    33,    33,
     -46,   -46,    62,   106,    83,    -4,   -46,   -46,    33,   -46,
     -46,   -46,   -46,   -46,   -46,   -46,    33,   -46,   -46,    33,
     -46,   -46,    33,    26,   -46,   108,    65,    89,   102,   -46,
     -46,   -46,    68,    -9,    42,   -46,   -46,   -46,    33,     3,
       3,   -46,   108,   127,   -46,     3,   -46
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     7,     8,     9,     0,     2,     3,     0,     5,     6,
       1,     4,    12,     0,    10,     0,     0,    14,     0,     0,
       0,    15,    16,    11,    18,     0,     0,     0,    21,    13,
      17,    19,    38,    59,    60,     0,     0,     0,    21,     0,
      23,     0,    24,     0,    21,    25,    26,    27,    28,    29,
      57,     0,    40,    48,    52,    58,     0,     0,     0,     0,
      36,    57,     0,     0,     0,     0,    20,    22,     0,    43,
      44,    45,    42,    46,    47,    32,     0,    50,    51,     0,
      54,    55,     0,     0,    62,    63,     0,     0,     0,    37,
      30,    56,     0,    41,    49,    53,    39,    61,     0,     0,
       0,    31,    64,    33,    35,     0,    34
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -46,   -46,   -46,   130,    30,    43,   -46,   -46,   -46,   110,
     -46,   -46,     8,   -45,   -46,   -46,   -46,   -46,   -46,   -28,
     -36,   -46,   -23,   -46,    58,   -46,    56,   -46,   -46
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,    41,    42,     9,    15,    20,    21,
      22,    29,    43,    44,    45,    46,    47,    48,    49,    61,
      51,    76,    52,    79,    53,    82,    54,    55,    86
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      50,    62,    10,    64,    77,    78,    32,    33,    34,    35,
      50,    36,     1,     2,    37,     3,    50,    32,    33,    34,
      13,    85,    87,    88,    32,    33,    34,    14,    12,    38,
       7,    39,    92,    83,    40,     7,    32,    33,    34,    77,
      78,    16,    39,     8,    17,    60,    63,    19,     8,    39,
      84,    96,    67,    93,   103,   104,    19,    80,    81,    56,
     106,    39,   102,    57,    69,    70,    71,    72,    73,    74,
      18,    50,    50,     1,     2,    23,     3,    50,    75,    69,
      70,    71,    72,    73,    74,    69,    70,    71,    72,    73,
      74,    24,    25,    89,    97,    98,    26,    27,    28,   101,
      69,    70,    71,    72,    73,    74,    69,    70,    71,    72,
      73,    74,    91,    58,    59,    31,    65,    68,    99,    69,
      70,    71,    72,    73,    74,    69,    70,    71,    72,    73,
      74,   100,    66,    90,   105,    11,    30,    94,    95
};

static const yytype_int8 yycheck[] =
{
      28,    37,     0,    39,    13,    14,     3,     4,     5,     6,
      38,     8,     9,    10,    11,    12,    44,     3,     4,     5,
      24,    57,    58,    59,     3,     4,     5,    31,     3,    26,
       0,    28,    68,    56,    31,     5,     3,     4,     5,    13,
      14,     4,    28,     0,    28,    31,    38,    17,     5,    28,
      29,    25,    44,    76,    99,   100,    26,    15,    16,    24,
     105,    28,    98,    28,    17,    18,    19,    20,    21,    22,
      25,    99,   100,     9,    10,    31,    12,   105,    31,    17,
      18,    19,    20,    21,    22,    17,    18,    19,    20,    21,
      22,     3,    29,    31,    29,    30,    30,    24,    26,    31,
      17,    18,    19,    20,    21,    22,    17,    18,    19,    20,
      21,    22,    29,    28,    28,    25,     3,    23,    29,    17,
      18,    19,    20,    21,    22,    17,    18,    19,    20,    21,
      22,    29,    27,    27,     7,     5,    26,    79,    82
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     9,    10,    12,    36,    37,    38,    39,    40,    41,
       0,    38,     3,    24,    31,    42,     4,    28,    25,    39,
      43,    44,    45,    31,     3,    29,    30,    24,    26,    46,
      44,    25,     3,     4,     5,     6,     8,    11,    26,    28,
      31,    39,    40,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    57,    59,    61,    62,    24,    28,    28,    28,
      31,    54,    55,    47,    55,     3,    27,    47,    23,    17,
      18,    19,    20,    21,    22,    31,    56,    13,    14,    58,
      15,    16,    60,    57,    29,    55,    63,    55,    55,    31,
      27,    29,    55,    57,    59,    61,    25,    29,    30,    29,
      29,    31,    55,    48,    48,     7,    48
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    35,    36,    37,    37,    38,    38,    39,    39,    39,
      40,    40,    42,    41,    43,    43,    44,    44,    45,    45,
      46,    47,    47,    48,    48,    48,    48,    48,    48,    48,
      49,    50,    50,    51,    51,    52,    53,    53,    54,    54,
      55,    55,    56,    56,    56,    56,    56,    56,    57,    57,
      58,    58,    59,    59,    60,    60,    61,    61,    61,    61,
      61,    62,    62,    63,    63
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       3,     6,     0,     7,     0,     1,     1,     3,     2,     4,
       3,     0,     2,     1,     1,     1,     1,     1,     1,     1,
       3,     4,     2,     5,     7,     5,     2,     3,     1,     4,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     4,     3,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: declList  */
#line 160 "src/parser.y"
      {
        tree *progNode = maketree(PROGRAM);
        addChild(progNode, (yyvsp[0].node));
        ast = progNode;
      }
#line 1296 "obj/y.tab.c"
    break;

  case 3: /* declList: decl  */
#line 169 "src/parser.y"
      {
          (yyval.node) = maketree(DECLLIST);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1305 "obj/y.tab.c"
    break;

  case 4: /* declList: declList decl  */
#line 174 "src/parser.y"
      {
          (yyval.node) = (yyvsp[-1].node);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1314 "obj/y.tab.c"
    break;

  case 5: /* decl: varDecl  */
#line 182 "src/parser.y"
      {
          (yyval.node) = maketree(DECL);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1323 "obj/y.tab.c"
    break;

  case 6: /* decl: funDecl  */
#line 187 "src/parser.y"
      {
          (yyval.node) = maketree(DECL);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1332 "obj/y.tab.c"
    break;

  case 7: /* typeSpecifier: KWD_INT  */
#line 194 "src/parser.y"
                { (yyval.node) = maketreeWithVal(TYPESPEC, INT_TYPE); }
#line 1338 "obj/y.tab.c"
    break;

  case 8: /* typeSpecifier: KWD_CHAR  */
#line 195 "src/parser.y"
                { (yyval.node) = maketreeWithVal(TYPESPEC, CHAR_TYPE); }
#line 1344 "obj/y.tab.c"
    break;

  case 9: /* typeSpecifier: KWD_VOID  */
#line 196 "src/parser.y"
                { (yyval.node) = maketreeWithVal(TYPESPEC, VOID_TYPE); }
#line 1350 "obj/y.tab.c"
    break;

  case 10: /* varDecl: typeSpecifier ID SEMICLN  */
#line 201 "src/parser.y"
      {
          ST_insert((yyvsp[-1].strval), scope, (yyvsp[-2].node)->val, SCALAR);
          (yyval.node) = maketree(VARDECL);
          addChild((yyval.node), (yyvsp[-2].node));
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[-1].strval));
          addChild((yyval.node), id);
      }
#line 1363 "obj/y.tab.c"
    break;

  case 11: /* varDecl: typeSpecifier ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN  */
#line 210 "src/parser.y"
      {
          ST_insert((yyvsp[-4].strval), scope, (yyvsp[-5].node)->val, ARRAY);
          (yyval.node) = maketree(VARDECL);
          addChild((yyval.node), (yyvsp[-5].node));
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[-4].strval));
          addChild((yyval.node), id);
          addChild((yyval.node), maketreeWithVal(INTEGER, (yyvsp[-2].value)));
      }
#line 1377 "obj/y.tab.c"
    break;

  case 12: /* $@1: %empty  */
#line 223 "src/parser.y"
      {
          set_scope((yyvsp[0].strval));
          ST_insert((yyvsp[0].strval), "", (yyvsp[-1].node)->val, FUNCTION);
      }
#line 1386 "obj/y.tab.c"
    break;

  case 13: /* funDecl: typeSpecifier ID $@1 LPAREN optFormalDeclList RPAREN funBody  */
#line 228 "src/parser.y"
      {
          (yyval.node) = maketree(FUNDECL);

          tree *ft = maketree(FUNCTYPENAME);
          addChild(ft, (yyvsp[-6].node));

          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[-5].strval));
          addChild(ft, id);

          addChild((yyval.node), ft);
          if ((yyvsp[-2].node)) addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[0].node));

          set_scope("");
      }
#line 1407 "obj/y.tab.c"
    break;

  case 14: /* optFormalDeclList: %empty  */
#line 248 "src/parser.y"
      {
          (yyval.node) = NULL;
      }
#line 1415 "obj/y.tab.c"
    break;

  case 15: /* optFormalDeclList: formalDeclList  */
#line 252 "src/parser.y"
      {
          (yyval.node) = (yyvsp[0].node);
      }
#line 1423 "obj/y.tab.c"
    break;

  case 16: /* formalDeclList: formalDecl  */
#line 259 "src/parser.y"
      {
          (yyval.node) = maketree(FORMALDECLLIST);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1432 "obj/y.tab.c"
    break;

  case 17: /* formalDeclList: formalDecl COMMA formalDeclList  */
#line 264 "src/parser.y"
      {
          (yyval.node) = maketree(FORMALDECLLIST);
          addChild((yyval.node), (yyvsp[-2].node));
          addAllChildren((yyval.node), (yyvsp[0].node));
      }
#line 1442 "obj/y.tab.c"
    break;

  case 18: /* formalDecl: typeSpecifier ID  */
#line 273 "src/parser.y"
      {
          ST_insert((yyvsp[0].strval), scope, (yyvsp[-1].node)->val, SCALAR);
          (yyval.node) = maketree(FORMALDECL);
          addChild((yyval.node), (yyvsp[-1].node));
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[0].strval));
          addChild((yyval.node), id);
      }
#line 1455 "obj/y.tab.c"
    break;

  case 19: /* formalDecl: typeSpecifier ID LSQ_BRKT RSQ_BRKT  */
#line 282 "src/parser.y"
      {
          ST_insert((yyvsp[-2].strval), scope, (yyvsp[-3].node)->val, ARRAY);
          (yyval.node) = maketree(FORMALDECL);
          addChild((yyval.node), (yyvsp[-3].node));
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[-2].strval));
          addChild((yyval.node), id);
      }
#line 1468 "obj/y.tab.c"
    break;

  case 20: /* funBody: LCRLY_BRKT statementList RCRLY_BRKT  */
#line 294 "src/parser.y"
      {
          (yyval.node) = maketree(FUNBODY);
          addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1477 "obj/y.tab.c"
    break;

  case 21: /* statementList: %empty  */
#line 302 "src/parser.y"
      {
          (yyval.node) = maketree(STATEMENTLIST);
      }
#line 1485 "obj/y.tab.c"
    break;

  case 22: /* statementList: statement statementList  */
#line 306 "src/parser.y"
      {
          (yyval.node) = maketree(STATEMENTLIST);
          if ((yyvsp[-1].node)) addChild((yyval.node), (yyvsp[-1].node));
          addAllChildren((yyval.node), (yyvsp[0].node));
      }
#line 1495 "obj/y.tab.c"
    break;

  case 23: /* statement: SEMICLN  */
#line 315 "src/parser.y"
      {
          (yyval.node) = maketree(STATEMENT);
      }
#line 1503 "obj/y.tab.c"
    break;

  case 24: /* statement: varDecl  */
#line 319 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1509 "obj/y.tab.c"
    break;

  case 25: /* statement: compoundStmt  */
#line 321 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1515 "obj/y.tab.c"
    break;

  case 26: /* statement: assignStmt  */
#line 323 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1521 "obj/y.tab.c"
    break;

  case 27: /* statement: condStmt  */
#line 325 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1527 "obj/y.tab.c"
    break;

  case 28: /* statement: loopStmt  */
#line 327 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1533 "obj/y.tab.c"
    break;

  case 29: /* statement: returnStmt  */
#line 329 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1539 "obj/y.tab.c"
    break;

  case 30: /* compoundStmt: LCRLY_BRKT statementList RCRLY_BRKT  */
#line 334 "src/parser.y"
      {
          (yyval.node) = maketree(COMPOUNDSTMT);
          if ((yyvsp[-1].node)) addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1548 "obj/y.tab.c"
    break;

  case 31: /* assignStmt: var OPER_ASGN expression SEMICLN  */
#line 342 "src/parser.y"
      {
          (yyval.node) = maketree(ASSIGNSTMT);
          addChild((yyval.node), (yyvsp[-3].node));
          addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1558 "obj/y.tab.c"
    break;

  case 32: /* assignStmt: expression SEMICLN  */
#line 348 "src/parser.y"
      {
          (yyval.node) = (yyvsp[-1].node);   // don't wrap plain expression statements!!!
      }
#line 1566 "obj/y.tab.c"
    break;

  case 33: /* condStmt: KWD_IF LPAREN expression RPAREN statement  */
#line 355 "src/parser.y"
      {
          (yyval.node) = maketree(CONDSTMT);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1576 "obj/y.tab.c"
    break;

  case 34: /* condStmt: KWD_IF LPAREN expression RPAREN statement KWD_ELSE statement  */
#line 361 "src/parser.y"
      {
          (yyval.node) = maketree(CONDSTMT);
          addChild((yyval.node), (yyvsp[-4].node));
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1587 "obj/y.tab.c"
    break;

  case 35: /* loopStmt: KWD_WHILE LPAREN expression RPAREN statement  */
#line 371 "src/parser.y"
      {
          (yyval.node) = maketree(LOOPSTMT);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1597 "obj/y.tab.c"
    break;

  case 36: /* returnStmt: KWD_RETURN SEMICLN  */
#line 380 "src/parser.y"
      {
          (yyval.node) = maketree(RETURNSTMT);
      }
#line 1605 "obj/y.tab.c"
    break;

  case 37: /* returnStmt: KWD_RETURN expression SEMICLN  */
#line 384 "src/parser.y"
      {
          (yyval.node) = maketree(RETURNSTMT);
          addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1614 "obj/y.tab.c"
    break;

  case 38: /* var: ID  */
#line 392 "src/parser.y"
      {
          if (resolve_symbol((yyvsp[0].strval)) == -1) {
              yywarning("undeclared symbol");
          }
          (yyval.node) = maketree(VAR);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[0].strval));
          addChild((yyval.node), id);
      }
#line 1628 "obj/y.tab.c"
    break;

  case 39: /* var: ID LSQ_BRKT addExpr RSQ_BRKT  */
#line 402 "src/parser.y"
      {
          if (resolve_symbol((yyvsp[-3].strval)) == -1) {
              yywarning("undeclared symbol");
          }
          (yyval.node) = maketree(ARRAYDECL);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[-3].strval));
          addChild((yyval.node), id);
          addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1643 "obj/y.tab.c"
    break;

  case 40: /* expression: addExpr  */
#line 416 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1649 "obj/y.tab.c"
    break;

  case 41: /* expression: expression relop addExpr  */
#line 418 "src/parser.y"
      {
          (yyval.node) = maketree(EXPRESSION);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[-1].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1660 "obj/y.tab.c"
    break;

  case 42: /* relop: OPER_LTE  */
#line 427 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, 0); }
#line 1666 "obj/y.tab.c"
    break;

  case 43: /* relop: OPER_LT  */
#line 428 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, 1); }
#line 1672 "obj/y.tab.c"
    break;

  case 44: /* relop: OPER_GT  */
#line 429 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, 2); }
#line 1678 "obj/y.tab.c"
    break;

  case 45: /* relop: OPER_GTE  */
#line 430 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, 3); }
#line 1684 "obj/y.tab.c"
    break;

  case 46: /* relop: OPER_EQ  */
#line 431 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, 4); }
#line 1690 "obj/y.tab.c"
    break;

  case 47: /* relop: OPER_NEQ  */
#line 432 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, 5); }
#line 1696 "obj/y.tab.c"
    break;

  case 48: /* addExpr: term  */
#line 437 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1702 "obj/y.tab.c"
    break;

  case 49: /* addExpr: addExpr addop term  */
#line 439 "src/parser.y"
      {
          (yyval.node) = maketree(ADDEXPR);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[-1].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1713 "obj/y.tab.c"
    break;

  case 50: /* addop: OPER_ADD  */
#line 448 "src/parser.y"
               { (yyval.node) = maketreeWithVal(ADDOP, 0); }
#line 1719 "obj/y.tab.c"
    break;

  case 51: /* addop: OPER_SUB  */
#line 449 "src/parser.y"
               { (yyval.node) = maketreeWithVal(ADDOP, 1); }
#line 1725 "obj/y.tab.c"
    break;

  case 52: /* term: factor  */
#line 454 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1731 "obj/y.tab.c"
    break;

  case 53: /* term: term mulop factor  */
#line 456 "src/parser.y"
      {
          (yyval.node) = maketree(TERM);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[-1].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1742 "obj/y.tab.c"
    break;

  case 54: /* mulop: OPER_MUL  */
#line 465 "src/parser.y"
               { (yyval.node) = maketreeWithVal(MULOP, 0); }
#line 1748 "obj/y.tab.c"
    break;

  case 55: /* mulop: OPER_DIV  */
#line 466 "src/parser.y"
               { (yyval.node) = maketreeWithVal(MULOP, 1); }
#line 1754 "obj/y.tab.c"
    break;

  case 56: /* factor: LPAREN expression RPAREN  */
#line 471 "src/parser.y"
      { (yyval.node) = (yyvsp[-1].node); }
#line 1760 "obj/y.tab.c"
    break;

  case 57: /* factor: var  */
#line 473 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1766 "obj/y.tab.c"
    break;

  case 58: /* factor: funCallExpr  */
#line 475 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1772 "obj/y.tab.c"
    break;

  case 59: /* factor: INTCONST  */
#line 477 "src/parser.y"
      { (yyval.node) = maketreeWithVal(INTEGER, (yyvsp[0].value)); }
#line 1778 "obj/y.tab.c"
    break;

  case 60: /* factor: CHARCONST  */
#line 479 "src/parser.y"
      { (yyval.node) = maketreeWithVal(CHAR, (yyvsp[0].value)); }
#line 1784 "obj/y.tab.c"
    break;

  case 61: /* funCallExpr: ID LPAREN argList RPAREN  */
#line 484 "src/parser.y"
      {
          if (resolve_function((yyvsp[-3].strval)) == -1) {
              yywarning("undeclared symbol");
          }
          (yyval.node) = maketree(FUNCCALLEXPR);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[-3].strval));
          addChild((yyval.node), id);
          if ((yyvsp[-1].node)) addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1799 "obj/y.tab.c"
    break;

  case 62: /* funCallExpr: ID LPAREN RPAREN  */
#line 495 "src/parser.y"
      {
          if (resolve_function((yyvsp[-2].strval)) == -1) {
              yywarning("undeclared symbol");
          }
          (yyval.node) = maketree(FUNCCALLEXPR);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup((yyvsp[-2].strval));
          addChild((yyval.node), id);
      }
#line 1813 "obj/y.tab.c"
    break;

  case 63: /* argList: expression  */
#line 508 "src/parser.y"
      {
          (yyval.node) = maketree(ARGLIST);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1822 "obj/y.tab.c"
    break;

  case 64: /* argList: argList COMMA expression  */
#line 513 "src/parser.y"
      {
          (yyval.node) = (yyvsp[-2].node);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1831 "obj/y.tab.c"
    break;


#line 1835 "obj/y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 518 "src/parser.y"


int yywarning(char *msg){
  printf("warning: line %d: %s\n", yylineno, msg);
  return 0;
}

int yyerror(char * msg){
  printf("error: line %d: %s\n", yylineno, msg);
  return 0;
}
