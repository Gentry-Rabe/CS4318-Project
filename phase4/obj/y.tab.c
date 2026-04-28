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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "strtab.h"

extern int yylineno;
extern int yylex(void);

typedef struct treenode tree;
extern tree *ast;

int yywarning(char *msg);
int yyerror(char *msg);

/* nodeTypes refer to different types of internal and external nodes
   that can be part of the abstract syntax tree.
 */
enum nodeTypes {
    PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
    FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
    STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
    CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
    ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
    ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR,
    FUNCTYPENAME
};

enum opType {ADD, SUB, MUL, DIV, LT, LTE, EQ, GTE, GT, NEQ};

static int current_function_index = -1;
static int current_function_return_type = VOID_TYPE;
static int current_param_count = 0;
static int suppress_arg_semantic_errors = 0;

#define INVALID_SYM_TYPE (-1)

static void sem_error(const char *msg) {
    if (!suppress_arg_semantic_errors) {
        yyerror((char *)msg);
    }
}

static tree *make_identifier_node(int idx) {
    return maketreeWithVal(IDENTIFIER, idx);
}

static void set_node_attrs(tree *node, int type, int sym_type) {
    if (node != NULL) {
        node->type = type;
        node->sym_type = sym_type;
    }
}

static int type_of_node(tree *node) {
    return node ? node->type : VOID_TYPE;
}

static int symtype_of_node(tree *node) {
    return node ? node->sym_type : SCALAR;
}

static int is_unindexed_array_var(tree *node) {
    return node != NULL && node->nodeKind == VAR && symtype_of_node(node) == ARRAY;
}

static int is_constant_int_expr(tree *node) {
    if (node == NULL) {
        return 0;
    }
    switch (node->nodeKind) {
        case INTEGER:
            return 1;
        case ADDEXPR:
            if (node->numChildren == 3 && node->children[1] != NULL &&
                node->children[1]->nodeKind == ADDOP) {
                return is_constant_int_expr(node->children[0]) &&
                       is_constant_int_expr(node->children[2]);
            }
            return 0;
        case TERM:
            if (node->numChildren == 3 && node->children[1] != NULL &&
                node->children[1]->nodeKind == MULOP) {
                return is_constant_int_expr(node->children[0]) &&
                       is_constant_int_expr(node->children[2]);
            }
            return 0;
        default:
            return 0;
    }
}

static int eval_constant_int_expr(tree *node, int *value) {
    int left = 0;
    int right = 0;

    if (node == NULL || value == NULL) {
        return 0;
    }

    switch (node->nodeKind) {
        case INTEGER:
            *value = node->val;
            return 1;
        case ADDEXPR:
            if (node->numChildren != 3 || node->children[1] == NULL ||
                node->children[1]->nodeKind != ADDOP) {
                return 0;
            }
            if (!eval_constant_int_expr(node->children[0], &left) ||
                !eval_constant_int_expr(node->children[2], &right)) {
                return 0;
            }
            if (node->children[1]->val == ADD) {
                *value = left + right;
                return 1;
            }
            if (node->children[1]->val == SUB) {
                *value = left - right;
                return 1;
            }
            return 0;
        case TERM:
            if (node->numChildren != 3 || node->children[1] == NULL ||
                node->children[1]->nodeKind != MULOP) {
                return 0;
            }
            if (!eval_constant_int_expr(node->children[0], &left) ||
                !eval_constant_int_expr(node->children[2], &right)) {
                return 0;
            }
            if (node->children[1]->val == MUL) {
                *value = left * right;
                return 1;
            }
            if (node->children[1]->val == DIV) {
                if (right == 0) {
                    return 0;
                }
                *value = left / right;
                return 1;
            }
            return 0;
        default:
            return 0;
    }
}

static int is_simple_rhs(tree *node) {
    if (node == NULL) {
        return 0;
    }
    switch (node->nodeKind) {
        case VAR:
        case ARRAYDECL:
        case FUNCCALLEXPR:
        case INTEGER:
        case CHAR:
            return 1;
        default:
            return 0;
    }
}

static int arg_list_count(tree *args) {
    return args ? args->numChildren : 0;
}

static tree *arg_list_child(tree *args, int i) {
    if (args == NULL || i < 0 || i >= args->numChildren) {
        return NULL;
    }
    return args->children[i];
}

static int arg_has_invalid_semantics(tree *arg) {
    return arg != NULL && symtype_of_node(arg) == INVALID_SYM_TYPE;
}

static int param_matches_arg(const param *p, tree *arg) {
    if (p == NULL || arg == NULL) {
        return 0;
    }
    if (arg_has_invalid_semantics(arg)) {
        return 1;
    }
    if (p->data_type != type_of_node(arg)) {
        return 0;
    }
    if (p->symbol_type == ARRAY) {
        return symtype_of_node(arg) == ARRAY;
    }
    return symtype_of_node(arg) != ARRAY;
}

static void check_function_call(const char *id, tree *args, tree *node) {
    symEntry *entry = ST_lookup_function((char *)id);
    if (entry == NULL) {
        sem_error("Undefined function");
        set_node_attrs(node, VOID_TYPE, SCALAR);
        return;
    }

    int actual_count = arg_list_count(args);
    int expected_count = entry->size;
    if (actual_count < expected_count) {
        sem_error("Too few arguments provided in function call.");
    } else if (actual_count > expected_count) {
        sem_error("Too many arguments provided in function call.");
    } else {
        param *p = entry->params;
        for (int i = 0; i < actual_count; ++i) {
            if (!param_matches_arg(p, arg_list_child(args, i))) {
                sem_error("Argument type mismatch in function call.");
                break;
            }
            if (p != NULL) {
                p = p->next;
            }
        }
    }

    set_node_attrs(node, entry->data_type, SCALAR);
}

static void check_assignment(tree *lhs, tree *rhs) {
    if (lhs == NULL || rhs == NULL) {
        return;
    }
    if (is_unindexed_array_var(lhs) || (is_simple_rhs(rhs) && is_unindexed_array_var(rhs))) {
        sem_error("array indexing error");
        return;
    }
    if (is_simple_rhs(rhs) && type_of_node(lhs) != type_of_node(rhs)) {
        sem_error("type mismatch");
    }
}

static void check_array_index(const char *id, tree *index_expr, tree *node) {
    symEntry *entry = ST_lookup((char *)id);
    int const_index = 0;

    if (entry == NULL) {
        sem_error("Undeclared variable");
        set_node_attrs(node, VOID_TYPE, SCALAR);
        return;
    }
    if (entry->symbol_type != ARRAY) {
        sem_error("Non-array identifier used as an array.");
    }
    if (type_of_node(index_expr) != INT_TYPE) {
        sem_error("Array indexed using non-integer expression.");
    }
    if (entry->symbol_type == ARRAY && entry->size > 0 &&
        is_constant_int_expr(index_expr) &&
        eval_constant_int_expr(index_expr, &const_index)) {
        if (const_index < 0 || const_index >= entry->size) {
            sem_error("Statically sized array indexed with constant, out-of-bounds expression.");
        }
    }
    set_node_attrs(node, entry->data_type, SCALAR);
}

static void check_return(tree *expr) {
    if (current_function_index < 0) {
        return;
    }
    if (is_unindexed_array_var(expr)) {
        sem_error("array indexing error");
        return;
    }
    if (current_function_return_type == VOID_TYPE) {
        if (expr != NULL) {
            sem_error("type mismatch");
        }
        return;
    }
    if (expr == NULL) {
        sem_error("type mismatch");
        return;
    }
    if (type_of_node(expr) != current_function_return_type) {
        sem_error("type mismatch");
    }
}

static void begin_function(const char *name, int return_type) {
    set_current_function_name(name);
    current_function_return_type = return_type;
    current_param_count = 0;
}

static void end_function(void) {
    up_scope();
    current_function_index = -1;
    current_function_return_type = VOID_TYPE;
    current_param_count = 0;
}

static tree *make_binary_node(int kind, tree *left, tree *mid, tree *right, int result_type) {
    tree *node = maketree(kind);
    if (left != NULL) addChild(node, left);
    if (mid != NULL) addChild(node, mid);
    if (right != NULL) addChild(node, right);
    set_node_attrs(node, result_type, SCALAR);
    return node;
}

static tree *make_id_expr(char *name, int idx, int kind) {
    tree *node = maketree(kind);
    tree *id = make_identifier_node(idx);
    addChild(node, id);
    (void)name;
    return node;
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

#line 400 "obj/y.tab.c"

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
  YYSYMBOL_43_2 = 43,                      /* $@2  */
  YYSYMBOL_optFormalDeclList = 44,         /* optFormalDeclList  */
  YYSYMBOL_formalDeclList = 45,            /* formalDeclList  */
  YYSYMBOL_formalDecl = 46,                /* formalDecl  */
  YYSYMBOL_funBody = 47,                   /* funBody  */
  YYSYMBOL_statementList = 48,             /* statementList  */
  YYSYMBOL_statement = 49,                 /* statement  */
  YYSYMBOL_compoundStmt = 50,              /* compoundStmt  */
  YYSYMBOL_assignStmt = 51,                /* assignStmt  */
  YYSYMBOL_condStmt = 52,                  /* condStmt  */
  YYSYMBOL_loopStmt = 53,                  /* loopStmt  */
  YYSYMBOL_returnStmt = 54,                /* returnStmt  */
  YYSYMBOL_var = 55,                       /* var  */
  YYSYMBOL_expression = 56,                /* expression  */
  YYSYMBOL_relop = 57,                     /* relop  */
  YYSYMBOL_addExpr = 58,                   /* addExpr  */
  YYSYMBOL_addop = 59,                     /* addop  */
  YYSYMBOL_term = 60,                      /* term  */
  YYSYMBOL_mulop = 61,                     /* mulop  */
  YYSYMBOL_factor = 62,                    /* factor  */
  YYSYMBOL_funCallExpr = 63,               /* funCallExpr  */
  YYSYMBOL_64_3 = 64,                      /* $@3  */
  YYSYMBOL_argList = 65                    /* argList  */
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
#define YYLAST   134

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  31
/* YYNRULES -- Number of rules.  */
#define YYNRULES  66
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  109

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
       0,   366,   366,   377,   382,   390,   395,   403,   405,   407,
     412,   431,   459,   471,   458,   492,   493,   498,   503,   512,
     533,   557,   566,   569,   578,   582,   584,   586,   588,   590,
     592,   597,   605,   612,   619,   625,   635,   644,   649,   658,
     671,   689,   691,   707,   708,   709,   710,   711,   712,   716,
     718,   734,   735,   739,   741,   757,   758,   762,   764,   766,
     768,   773,   782,   781,   795,   806,   811
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
  "typeSpecifier", "varDecl", "funDecl", "$@1", "$@2", "optFormalDeclList",
  "formalDeclList", "formalDecl", "funBody", "statementList", "statement",
  "compoundStmt", "assignStmt", "condStmt", "loopStmt", "returnStmt",
  "var", "expression", "relop", "addExpr", "addop", "term", "mulop",
  "factor", "funCallExpr", "$@3", "argList", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-54)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      53,   -54,   -54,   -54,    20,    53,   -54,    26,   -54,   -54,
     -54,   -54,    21,    63,   -54,     3,    48,    53,    43,    72,
      61,   -54,    59,   -54,    68,   -54,    53,    69,    67,   -54,
     -54,     7,   -54,    27,   -54,   -54,    83,    84,    22,     7,
      36,   -54,    92,   -54,    86,     7,   -54,   -54,   -54,   -54,
     -54,    73,    39,    -8,    56,   -54,   -54,    36,    85,    36,
      36,   -54,   -54,    60,    88,    81,    21,   -54,   -54,    36,
     -54,   -54,   -54,   -54,   -54,   -54,   -54,    36,   -54,   -54,
      36,   -54,   -54,    36,    23,   -54,    36,    87,   100,   -54,
     -54,   -54,    66,    -8,    56,   -54,   -54,   106,    13,     7,
       7,   -54,   -54,    36,   123,   -54,   106,     7,   -54
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     7,     8,     9,     0,     2,     3,     0,     5,     6,
       1,     4,    12,     0,    10,     0,     0,    15,     0,     0,
       0,    16,    17,    11,    19,    13,     0,     0,     0,    18,
      20,    22,    14,    39,    60,    61,     0,     0,     0,    22,
       0,    24,     0,    25,     0,    22,    26,    27,    28,    29,
      30,    58,     0,    41,    49,    53,    59,     0,    62,     0,
       0,    37,    58,     0,     0,     0,     0,    21,    23,     0,
      44,    45,    46,    43,    47,    48,    33,     0,    51,    52,
       0,    55,    56,     0,     0,    64,     0,     0,     0,    38,
      31,    57,     0,    42,    50,    54,    40,    65,     0,     0,
       0,    32,    63,     0,    34,    36,    66,     0,    35
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -54,   -54,   -54,   126,     4,     2,   -54,   -54,   -54,   -54,
     107,   -54,   -54,   -11,   -53,   -54,   -54,   -54,   -54,   -54,
     -31,   -37,   -54,   -33,   -54,    52,   -54,    51,   -54,   -54,
     -54
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,    42,    43,     9,    15,    28,    20,
      21,    22,    32,    44,    45,    46,    47,    48,    49,    50,
      62,    52,    77,    53,    80,    54,    83,    55,    56,    86,
      98
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      51,    63,     8,    65,     7,    78,    79,     8,    51,     7,
      33,    34,    35,    36,    51,    37,     1,     2,    38,     3,
      10,    19,    87,    88,    84,    33,    34,    35,    64,    12,
      19,    17,    92,    39,    68,    40,    78,    79,    41,    33,
      34,    35,   102,   103,    93,    13,   104,   105,    96,    97,
      40,    57,    14,    61,   108,    58,    70,    71,    72,    73,
      74,    75,     1,     2,    40,     3,   106,    16,    51,    51,
      76,    81,    82,    18,    23,    24,    51,    70,    71,    72,
      73,    74,    75,    70,    71,    72,    73,    74,    75,    26,
      25,    89,    27,    31,    30,    66,    69,   101,    70,    71,
      72,    73,    74,    75,    70,    71,    72,    73,    74,    75,
      91,    59,    60,    67,    85,    90,    99,    70,    71,    72,
      73,    74,    75,    70,    71,    72,    73,    74,    75,   100,
     107,    11,    94,    29,    95
};

static const yytype_int8 yycheck[] =
{
      31,    38,     0,    40,     0,    13,    14,     5,    39,     5,
       3,     4,     5,     6,    45,     8,     9,    10,    11,    12,
       0,    17,    59,    60,    57,     3,     4,     5,    39,     3,
      26,    28,    69,    26,    45,    28,    13,    14,    31,     3,
       4,     5,    29,    30,    77,    24,    99,   100,    25,    86,
      28,    24,    31,    31,   107,    28,    17,    18,    19,    20,
      21,    22,     9,    10,    28,    12,   103,     4,    99,   100,
      31,    15,    16,    25,    31,     3,   107,    17,    18,    19,
      20,    21,    22,    17,    18,    19,    20,    21,    22,    30,
      29,    31,    24,    26,    25,     3,    23,    31,    17,    18,
      19,    20,    21,    22,    17,    18,    19,    20,    21,    22,
      29,    28,    28,    27,    29,    27,    29,    17,    18,    19,
      20,    21,    22,    17,    18,    19,    20,    21,    22,    29,
       7,     5,    80,    26,    83
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     9,    10,    12,    36,    37,    38,    39,    40,    41,
       0,    38,     3,    24,    31,    42,     4,    28,    25,    39,
      44,    45,    46,    31,     3,    29,    30,    24,    43,    45,
      25,    26,    47,     3,     4,     5,     6,     8,    11,    26,
      28,    31,    39,    40,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    58,    60,    62,    63,    24,    28,    28,
      28,    31,    55,    56,    48,    56,     3,    27,    48,    23,
      17,    18,    19,    20,    21,    22,    31,    57,    13,    14,
      59,    15,    16,    61,    58,    29,    64,    56,    56,    31,
      27,    29,    56,    58,    60,    62,    25,    56,    65,    29,
      29,    31,    29,    30,    49,    49,    56,     7,    49
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    35,    36,    37,    37,    38,    38,    39,    39,    39,
      40,    40,    42,    43,    41,    44,    44,    45,    45,    46,
      46,    47,    48,    48,    49,    49,    49,    49,    49,    49,
      49,    50,    51,    51,    52,    52,    53,    54,    54,    55,
      55,    56,    56,    57,    57,    57,    57,    57,    57,    58,
      58,    59,    59,    60,    60,    61,    61,    62,    62,    62,
      62,    62,    64,    63,    63,    65,    65
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       3,     6,     0,     0,     8,     0,     1,     1,     3,     2,
       4,     3,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     3,     4,     2,     5,     7,     5,     2,     3,     1,
       4,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     1,     3,     1,     1,
       1,     1,     0,     5,     3,     1,     3
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
#line 367 "src/parser.y"
      {
          tree *progNode = maketree(PROGRAM);
          if ((yyvsp[0].node) != NULL) {
              addChild(progNode, (yyvsp[0].node));
          }
          ast = progNode;
      }
#line 1537 "obj/y.tab.c"
    break;

  case 3: /* declList: decl  */
#line 378 "src/parser.y"
      {
          (yyval.node) = maketree(DECLLIST);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1546 "obj/y.tab.c"
    break;

  case 4: /* declList: declList decl  */
#line 383 "src/parser.y"
      {
          (yyval.node) = (yyvsp[-1].node);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1555 "obj/y.tab.c"
    break;

  case 5: /* decl: varDecl  */
#line 391 "src/parser.y"
      {
          (yyval.node) = maketree(DECL);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1564 "obj/y.tab.c"
    break;

  case 6: /* decl: funDecl  */
#line 396 "src/parser.y"
      {
          (yyval.node) = maketree(DECL);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1573 "obj/y.tab.c"
    break;

  case 7: /* typeSpecifier: KWD_INT  */
#line 404 "src/parser.y"
      { (yyval.node) = maketreeWithVal(TYPESPEC, INT_TYPE); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 1579 "obj/y.tab.c"
    break;

  case 8: /* typeSpecifier: KWD_CHAR  */
#line 406 "src/parser.y"
      { (yyval.node) = maketreeWithVal(TYPESPEC, CHAR_TYPE); set_node_attrs((yyval.node), CHAR_TYPE, SCALAR); }
#line 1585 "obj/y.tab.c"
    break;

  case 9: /* typeSpecifier: KWD_VOID  */
#line 408 "src/parser.y"
      { (yyval.node) = maketreeWithVal(TYPESPEC, VOID_TYPE); set_node_attrs((yyval.node), VOID_TYPE, SCALAR); }
#line 1591 "obj/y.tab.c"
    break;

  case 10: /* varDecl: typeSpecifier ID SEMICLN  */
#line 413 "src/parser.y"
      {
          int existing = ST_lookup_current_index((yyvsp[-1].strval));
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert((yyvsp[-1].strval), (yyvsp[-2].node)->val, SCALAR, NULL);
          }
          (yyval.node) = maketree(VARDECL);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), make_identifier_node(idx));
          if (existing == -1) {
              symEntry *entry = ST_lookup((yyvsp[-1].strval));
              if (entry != NULL) {
                  entry->size = 0;
              }
          }
      }
#line 1614 "obj/y.tab.c"
    break;

  case 11: /* varDecl: typeSpecifier ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN  */
#line 432 "src/parser.y"
      {
          int existing = ST_lookup_current_index((yyvsp[-4].strval));
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert((yyvsp[-4].strval), (yyvsp[-5].node)->val, ARRAY, NULL);
          }
          (yyval.node) = maketree(VARDECL);
          addChild((yyval.node), (yyvsp[-5].node));
          addChild((yyval.node), make_identifier_node(idx));
          addChild((yyval.node), maketreeWithVal(INTEGER, (yyvsp[-2].value)));
          set_node_attrs((yyval.node), (yyvsp[-5].node)->val, ARRAY);
          if ((yyvsp[-2].value) == 0) {
              sem_error("Array variable declared with size of zero.");
          }
          if (existing == -1) {
              symEntry *entry = ST_lookup((yyvsp[-4].strval));
              if (entry != NULL) {
                  entry->size = (yyvsp[-2].value);
              }
          }
      }
#line 1642 "obj/y.tab.c"
    break;

  case 12: /* $@1: %empty  */
#line 459 "src/parser.y"
      {
          int existing = ST_lookup_current_index((yyvsp[0].strval));
          if (existing != -1) {
              sem_error("multiply declared symbol");
              current_function_index = existing;
          } else {
              current_function_index = ST_insert((yyvsp[0].strval), (yyvsp[-1].node)->val, FUNCTION, NULL);
          }
          begin_function((yyvsp[0].strval), (yyvsp[-1].node)->val);
          new_scope();
      }
#line 1658 "obj/y.tab.c"
    break;

  case 13: /* $@2: %empty  */
#line 471 "src/parser.y"
      {
          connect_params(current_function_index, current_param_count);
      }
#line 1666 "obj/y.tab.c"
    break;

  case 14: /* funDecl: typeSpecifier ID $@1 LPAREN optFormalDeclList RPAREN $@2 funBody  */
#line 475 "src/parser.y"
      {
          (yyval.node) = maketree(FUNDECL);
          tree *ft = maketree(FUNCTYPENAME);
          addChild(ft, (yyvsp[-7].node));
          addChild(ft, make_identifier_node(current_function_index));
          set_node_attrs(ft, (yyvsp[-7].node)->val, FUNCTION);
          addChild((yyval.node), ft);
          if ((yyvsp[-3].node) != NULL) {
              addChild((yyval.node), (yyvsp[-3].node));
          }
          addChild((yyval.node), (yyvsp[0].node));
          end_function();
      }
#line 1684 "obj/y.tab.c"
    break;

  case 15: /* optFormalDeclList: %empty  */
#line 492 "src/parser.y"
      { (yyval.node) = NULL; }
#line 1690 "obj/y.tab.c"
    break;

  case 16: /* optFormalDeclList: formalDeclList  */
#line 494 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1696 "obj/y.tab.c"
    break;

  case 17: /* formalDeclList: formalDecl  */
#line 499 "src/parser.y"
      {
          (yyval.node) = maketree(FORMALDECLLIST);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1705 "obj/y.tab.c"
    break;

  case 18: /* formalDeclList: formalDecl COMMA formalDeclList  */
#line 504 "src/parser.y"
      {
          (yyval.node) = maketree(FORMALDECLLIST);
          addChild((yyval.node), (yyvsp[-2].node));
          addAllChildren((yyval.node), (yyvsp[0].node));
      }
#line 1715 "obj/y.tab.c"
    break;

  case 19: /* formalDecl: typeSpecifier ID  */
#line 513 "src/parser.y"
      {
          int existing = ST_lookup_current_index((yyvsp[0].strval));
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert((yyvsp[0].strval), (yyvsp[-1].node)->val, SCALAR, NULL);
              add_param((yyvsp[-1].node)->val, SCALAR);
              current_param_count++;
          }
          (yyval.node) = maketree(FORMALDECL);
          addChild((yyval.node), (yyvsp[-1].node));
          addChild((yyval.node), make_identifier_node(idx));
          if (existing == -1) {
              symEntry *entry = ST_lookup((yyvsp[0].strval));
              if (entry != NULL) {
                  entry->size = 0;
              }
          }
      }
#line 1740 "obj/y.tab.c"
    break;

  case 20: /* formalDecl: typeSpecifier ID LSQ_BRKT RSQ_BRKT  */
#line 534 "src/parser.y"
      {
          int existing = ST_lookup_current_index((yyvsp[-2].strval));
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert((yyvsp[-2].strval), (yyvsp[-3].node)->val, ARRAY, NULL);
              add_param((yyvsp[-3].node)->val, ARRAY);
              current_param_count++;
          }
          (yyval.node) = maketree(FORMALDECL);
          addChild((yyval.node), (yyvsp[-3].node));
          addChild((yyval.node), make_identifier_node(idx));
          if (existing == -1) {
              symEntry *entry = ST_lookup((yyvsp[-2].strval));
              if (entry != NULL) {
                  entry->size = 0;
              }
          }
      }
#line 1765 "obj/y.tab.c"
    break;

  case 21: /* funBody: LCRLY_BRKT statementList RCRLY_BRKT  */
#line 558 "src/parser.y"
      {
          (yyval.node) = maketree(FUNBODY);
          addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1774 "obj/y.tab.c"
    break;

  case 22: /* statementList: %empty  */
#line 566 "src/parser.y"
      {
          (yyval.node) = maketree(STATEMENTLIST);
      }
#line 1782 "obj/y.tab.c"
    break;

  case 23: /* statementList: statement statementList  */
#line 570 "src/parser.y"
      {
          (yyval.node) = maketree(STATEMENTLIST);
          if ((yyvsp[-1].node) != NULL) addChild((yyval.node), (yyvsp[-1].node));
          addAllChildren((yyval.node), (yyvsp[0].node));
      }
#line 1792 "obj/y.tab.c"
    break;

  case 24: /* statement: SEMICLN  */
#line 579 "src/parser.y"
      {
          (yyval.node) = maketree(STATEMENT);
      }
#line 1800 "obj/y.tab.c"
    break;

  case 25: /* statement: varDecl  */
#line 583 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1806 "obj/y.tab.c"
    break;

  case 26: /* statement: compoundStmt  */
#line 585 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1812 "obj/y.tab.c"
    break;

  case 27: /* statement: assignStmt  */
#line 587 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1818 "obj/y.tab.c"
    break;

  case 28: /* statement: condStmt  */
#line 589 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1824 "obj/y.tab.c"
    break;

  case 29: /* statement: loopStmt  */
#line 591 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1830 "obj/y.tab.c"
    break;

  case 30: /* statement: returnStmt  */
#line 593 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1836 "obj/y.tab.c"
    break;

  case 31: /* compoundStmt: LCRLY_BRKT statementList RCRLY_BRKT  */
#line 598 "src/parser.y"
      {
          (yyval.node) = maketree(COMPOUNDSTMT);
          if ((yyvsp[-1].node) != NULL) addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1845 "obj/y.tab.c"
    break;

  case 32: /* assignStmt: var OPER_ASGN expression SEMICLN  */
#line 606 "src/parser.y"
      {
          check_assignment((yyvsp[-3].node), (yyvsp[-1].node));
          (yyval.node) = maketree(ASSIGNSTMT);
          addChild((yyval.node), (yyvsp[-3].node));
          addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1856 "obj/y.tab.c"
    break;

  case 33: /* assignStmt: expression SEMICLN  */
#line 613 "src/parser.y"
      {
          (yyval.node) = (yyvsp[-1].node);
      }
#line 1864 "obj/y.tab.c"
    break;

  case 34: /* condStmt: KWD_IF LPAREN expression RPAREN statement  */
#line 620 "src/parser.y"
      {
          (yyval.node) = maketree(CONDSTMT);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1874 "obj/y.tab.c"
    break;

  case 35: /* condStmt: KWD_IF LPAREN expression RPAREN statement KWD_ELSE statement  */
#line 626 "src/parser.y"
      {
          (yyval.node) = maketree(CONDSTMT);
          addChild((yyval.node), (yyvsp[-4].node));
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1885 "obj/y.tab.c"
    break;

  case 36: /* loopStmt: KWD_WHILE LPAREN expression RPAREN statement  */
#line 636 "src/parser.y"
      {
          (yyval.node) = maketree(LOOPSTMT);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 1895 "obj/y.tab.c"
    break;

  case 37: /* returnStmt: KWD_RETURN SEMICLN  */
#line 645 "src/parser.y"
      {
          check_return(NULL);
          (yyval.node) = maketree(RETURNSTMT);
      }
#line 1904 "obj/y.tab.c"
    break;

  case 38: /* returnStmt: KWD_RETURN expression SEMICLN  */
#line 650 "src/parser.y"
      {
          check_return((yyvsp[-1].node));
          (yyval.node) = maketree(RETURNSTMT);
          addChild((yyval.node), (yyvsp[-1].node));
      }
#line 1914 "obj/y.tab.c"
    break;

  case 39: /* var: ID  */
#line 659 "src/parser.y"
      {
          symEntry *entry = ST_lookup((yyvsp[0].strval));
          int idx = (entry == NULL || entry->symbol_type == FUNCTION) ? -1 : ST_lookup_index((yyvsp[0].strval));
          (yyval.node) = maketree(VAR);
          addChild((yyval.node), make_identifier_node(idx));
          if (entry == NULL || entry->symbol_type == FUNCTION) {
              sem_error("Undeclared variable");
              set_node_attrs((yyval.node), VOID_TYPE, INVALID_SYM_TYPE);
          } else {
              set_node_attrs((yyval.node), entry->data_type, entry->symbol_type);
          }
      }
#line 1931 "obj/y.tab.c"
    break;

  case 40: /* var: ID LSQ_BRKT addExpr RSQ_BRKT  */
#line 672 "src/parser.y"
      {
          symEntry *entry = ST_lookup((yyvsp[-3].strval));
          int idx = (entry != NULL && entry->symbol_type == ARRAY) ? ST_lookup_index((yyvsp[-3].strval)) : -1;
          (yyval.node) = maketree(ARRAYDECL);
          addChild((yyval.node), make_identifier_node(idx));
          addChild((yyval.node), (yyvsp[-1].node));
          if (entry == NULL) {
              sem_error("Undeclared variable");
              set_node_attrs((yyval.node), VOID_TYPE, INVALID_SYM_TYPE);
          } else {
              check_array_index((yyvsp[-3].strval), (yyvsp[-1].node), (yyval.node));
              addAllChildren((yyval.node), NULL);
          }
      }
#line 1950 "obj/y.tab.c"
    break;

  case 41: /* expression: addExpr  */
#line 690 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 1956 "obj/y.tab.c"
    break;

  case 42: /* expression: expression relop addExpr  */
#line 692 "src/parser.y"
      {
          if (is_unindexed_array_var((yyvsp[-2].node)) || is_unindexed_array_var((yyvsp[0].node))) {
              sem_error("array indexing error");
          } else if (type_of_node((yyvsp[-2].node)) != type_of_node((yyvsp[0].node))) {
              sem_error("type mismatch");
          }
          (yyval.node) = maketree(EXPRESSION);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[-1].node));
          addChild((yyval.node), (yyvsp[0].node));
          set_node_attrs((yyval.node), INT_TYPE, SCALAR);
      }
#line 1973 "obj/y.tab.c"
    break;

  case 43: /* relop: OPER_LTE  */
#line 707 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, LTE); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 1979 "obj/y.tab.c"
    break;

  case 44: /* relop: OPER_LT  */
#line 708 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, LT);  set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 1985 "obj/y.tab.c"
    break;

  case 45: /* relop: OPER_GT  */
#line 709 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, GT);  set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 1991 "obj/y.tab.c"
    break;

  case 46: /* relop: OPER_GTE  */
#line 710 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, GTE); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 1997 "obj/y.tab.c"
    break;

  case 47: /* relop: OPER_EQ  */
#line 711 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, EQ);  set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 2003 "obj/y.tab.c"
    break;

  case 48: /* relop: OPER_NEQ  */
#line 712 "src/parser.y"
               { (yyval.node) = maketreeWithVal(RELOP, NEQ); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 2009 "obj/y.tab.c"
    break;

  case 49: /* addExpr: term  */
#line 717 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 2015 "obj/y.tab.c"
    break;

  case 50: /* addExpr: addExpr addop term  */
#line 719 "src/parser.y"
      {
          if (is_unindexed_array_var((yyvsp[-2].node)) || is_unindexed_array_var((yyvsp[0].node))) {
              sem_error("array indexing error");
          } else if (type_of_node((yyvsp[-2].node)) != type_of_node((yyvsp[0].node))) {
              sem_error("type mismatch");
          }
          (yyval.node) = maketree(ADDEXPR);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[-1].node));
          addChild((yyval.node), (yyvsp[0].node));
          set_node_attrs((yyval.node), type_of_node((yyvsp[-2].node)), SCALAR);
      }
#line 2032 "obj/y.tab.c"
    break;

  case 51: /* addop: OPER_ADD  */
#line 734 "src/parser.y"
               { (yyval.node) = maketreeWithVal(ADDOP, ADD); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 2038 "obj/y.tab.c"
    break;

  case 52: /* addop: OPER_SUB  */
#line 735 "src/parser.y"
               { (yyval.node) = maketreeWithVal(ADDOP, SUB); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 2044 "obj/y.tab.c"
    break;

  case 53: /* term: factor  */
#line 740 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 2050 "obj/y.tab.c"
    break;

  case 54: /* term: term mulop factor  */
#line 742 "src/parser.y"
      {
          if (is_unindexed_array_var((yyvsp[-2].node)) || is_unindexed_array_var((yyvsp[0].node))) {
              sem_error("array indexing error");
          } else if (type_of_node((yyvsp[-2].node)) != type_of_node((yyvsp[0].node))) {
              sem_error("type mismatch");
          }
          (yyval.node) = maketree(TERM);
          addChild((yyval.node), (yyvsp[-2].node));
          addChild((yyval.node), (yyvsp[-1].node));
          addChild((yyval.node), (yyvsp[0].node));
          set_node_attrs((yyval.node), type_of_node((yyvsp[-2].node)), SCALAR);
      }
#line 2067 "obj/y.tab.c"
    break;

  case 55: /* mulop: OPER_MUL  */
#line 757 "src/parser.y"
               { (yyval.node) = maketreeWithVal(MULOP, MUL); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 2073 "obj/y.tab.c"
    break;

  case 56: /* mulop: OPER_DIV  */
#line 758 "src/parser.y"
               { (yyval.node) = maketreeWithVal(MULOP, DIV); set_node_attrs((yyval.node), INT_TYPE, SCALAR); }
#line 2079 "obj/y.tab.c"
    break;

  case 57: /* factor: LPAREN expression RPAREN  */
#line 763 "src/parser.y"
      { (yyval.node) = (yyvsp[-1].node); }
#line 2085 "obj/y.tab.c"
    break;

  case 58: /* factor: var  */
#line 765 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 2091 "obj/y.tab.c"
    break;

  case 59: /* factor: funCallExpr  */
#line 767 "src/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 2097 "obj/y.tab.c"
    break;

  case 60: /* factor: INTCONST  */
#line 769 "src/parser.y"
      {
          (yyval.node) = maketreeWithVal(INTEGER, (yyvsp[0].value));
          set_node_attrs((yyval.node), INT_TYPE, SCALAR);
      }
#line 2106 "obj/y.tab.c"
    break;

  case 61: /* factor: CHARCONST  */
#line 774 "src/parser.y"
      {
          (yyval.node) = maketreeWithVal(CHAR, (yyvsp[0].value));
          set_node_attrs((yyval.node), CHAR_TYPE, SCALAR);
      }
#line 2115 "obj/y.tab.c"
    break;

  case 62: /* $@3: %empty  */
#line 782 "src/parser.y"
      {
          suppress_arg_semantic_errors++;
      }
#line 2123 "obj/y.tab.c"
    break;

  case 63: /* funCallExpr: ID LPAREN $@3 argList RPAREN  */
#line 786 "src/parser.y"
      {
          suppress_arg_semantic_errors--;
          symEntry *entry = ST_lookup_function((yyvsp[-4].strval));
          int idx = (entry != NULL) ? ST_lookup_index((yyvsp[-4].strval)) : -1;
          (yyval.node) = maketree(FUNCCALLEXPR);
          addChild((yyval.node), make_identifier_node(idx));
          if ((yyvsp[-1].node) != NULL) addChild((yyval.node), (yyvsp[-1].node));
          check_function_call((yyvsp[-4].strval), (yyvsp[-1].node), (yyval.node));
      }
#line 2137 "obj/y.tab.c"
    break;

  case 64: /* funCallExpr: ID LPAREN RPAREN  */
#line 796 "src/parser.y"
      {
          symEntry *entry = ST_lookup_function((yyvsp[-2].strval));
          int idx = (entry != NULL) ? ST_lookup_index((yyvsp[-2].strval)) : -1;
          (yyval.node) = maketree(FUNCCALLEXPR);
          addChild((yyval.node), make_identifier_node(idx));
          check_function_call((yyvsp[-2].strval), NULL, (yyval.node));
      }
#line 2149 "obj/y.tab.c"
    break;

  case 65: /* argList: expression  */
#line 807 "src/parser.y"
      {
          (yyval.node) = maketree(ARGLIST);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 2158 "obj/y.tab.c"
    break;

  case 66: /* argList: argList COMMA expression  */
#line 812 "src/parser.y"
      {
          (yyval.node) = (yyvsp[-2].node);
          addChild((yyval.node), (yyvsp[0].node));
      }
#line 2167 "obj/y.tab.c"
    break;


#line 2171 "obj/y.tab.c"

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

#line 817 "src/parser.y"


int yywarning(char *msg) {
    printf("warning: line %d: %s\n", yylineno, msg);
    return 0;
}

int yyerror(char *msg) {
    printf("error: line %d: %s\n", yylineno, msg);
    return 0;
}
