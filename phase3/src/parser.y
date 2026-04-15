%{
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

static void sem_error(const char *msg) {
    yyerror((char *)msg);
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

static int param_matches_arg(const param *p, tree *arg) {
    if (p == NULL || arg == NULL) {
        return 0;
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
    symEntry *entry = ST_lookup((char *)id);
    if (entry == NULL || entry->symbol_type != FUNCTION) {
        sem_error("undeclared symbol");
        set_node_attrs(node, VOID_TYPE, SCALAR);
        return;
    }

    int actual_count = arg_list_count(args);
    int expected_count = entry->size;
    if (actual_count != expected_count) {
        sem_error("function call mismatch");
    } else {
        param *p = entry->params;
        for (int i = 0; i < actual_count; ++i) {
            if (!param_matches_arg(p, arg_list_child(args, i))) {
                sem_error("function call mismatch");
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
    if (entry == NULL) {
        sem_error("undeclared symbol");
        set_node_attrs(node, VOID_TYPE, SCALAR);
        return;
    }
    if (entry->symbol_type != ARRAY) {
        sem_error("array indexing error");
    }
    if (type_of_node(index_expr) != INT_TYPE) {
        sem_error("array indexing error");
    }
    if (index_expr != NULL && index_expr->nodeKind == INTEGER && entry->size > 0) {
        if (index_expr->val < 0 || index_expr->val >= entry->size) {
            sem_error("array indexing error");
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
%}

%union {
    int value;
    struct treenode *node;
    char *strval;
}

%token <strval> ID
%token <value> INTCONST
%token <value> CHARCONST

%token <strval> KWD_IF KWD_ELSE KWD_WHILE KWD_INT KWD_CHAR KWD_RETURN KWD_VOID
%token <strval> OPER_ADD OPER_SUB OPER_MUL OPER_DIV OPER_LT OPER_GT OPER_GTE OPER_LTE OPER_EQ OPER_NEQ OPER_ASGN
%token <strval> LSQ_BRKT RSQ_BRKT LCRLY_BRKT RCRLY_BRKT LPAREN RPAREN COMMA SEMICLN
%token <strval> ERROR ILLEGAL_TOK

%nonassoc OPER_EQ OPER_NEQ
%nonassoc OPER_LT OPER_LTE OPER_GT OPER_GTE
%left OPER_ADD OPER_SUB
%left OPER_MUL OPER_DIV
%nonassoc LOWER_THAN_ELSE
%nonassoc KWD_ELSE

%type <node> program declList decl
%type <node> varDecl typeSpecifier funDecl
%type <node> optFormalDeclList
%type <node> formalDeclList formalDecl
%type <node> funBody statementList statement
%type <node> compoundStmt assignStmt condStmt loopStmt returnStmt
%type <node> var expression relop addExpr addop term mulop factor
%type <node> funCallExpr argList

%start program

%%

program
    : declList
      {
          tree *progNode = maketree(PROGRAM);
          if ($1 != NULL) {
              addChild(progNode, $1);
          }
          ast = progNode;
      }
    ;

declList
    : decl
      {
          $$ = maketree(DECLLIST);
          addChild($$, $1);
      }
    | declList decl
      {
          $$ = $1;
          addChild($$, $2);
      }
    ;

decl
    : varDecl
      {
          $$ = maketree(DECL);
          addChild($$, $1);
      }
    | funDecl
      {
          $$ = maketree(DECL);
          addChild($$, $1);
      }
    ;

typeSpecifier
    : KWD_INT
      { $$ = maketreeWithVal(TYPESPEC, INT_TYPE); set_node_attrs($$, INT_TYPE, SCALAR); }
    | KWD_CHAR
      { $$ = maketreeWithVal(TYPESPEC, CHAR_TYPE); set_node_attrs($$, CHAR_TYPE, SCALAR); }
    | KWD_VOID
      { $$ = maketreeWithVal(TYPESPEC, VOID_TYPE); set_node_attrs($$, VOID_TYPE, SCALAR); }
    ;

varDecl
    : typeSpecifier ID SEMICLN
      {
          int existing = ST_lookup_current_index($2);
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert($2, $1->val, SCALAR, NULL);
          }
          $$ = maketree(VARDECL);
          addChild($$, $1);
          addChild($$, make_identifier_node(idx));
          if (existing == -1) {
              symEntry *entry = ST_lookup($2);
              if (entry != NULL) {
                  entry->size = 0;
              }
          }
      }
    | typeSpecifier ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN
      {
          int existing = ST_lookup_current_index($2);
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert($2, $1->val, ARRAY, NULL);
          }
          $$ = maketree(VARDECL);
          addChild($$, $1);
          addChild($$, make_identifier_node(idx));
          addChild($$, maketreeWithVal(INTEGER, $4));
          set_node_attrs($$, $1->val, ARRAY);
          if (existing == -1) {
              symEntry *entry = ST_lookup($2);
              if (entry != NULL) {
                  entry->size = $4;
              }
          }
      }
    ;

funDecl
    : typeSpecifier ID
      {
          int existing = ST_lookup_current_index($2);
          if (existing != -1) {
              sem_error("multiply declared symbol");
              current_function_index = existing;
          } else {
              current_function_index = ST_insert($2, $1->val, FUNCTION, NULL);
          }
          begin_function($2, $1->val);
          new_scope();
      }
      LPAREN optFormalDeclList RPAREN
      {
          connect_params(current_function_index, current_param_count);
      }
      funBody
      {
          $$ = maketree(FUNDECL);
          tree *ft = maketree(FUNCTYPENAME);
          addChild(ft, $1);
          addChild(ft, make_identifier_node(current_function_index));
          set_node_attrs(ft, $1->val, FUNCTION);
          addChild($$, ft);
          if ($5 != NULL) {
              addChild($$, $5);
          }
          addChild($$, $8);
          end_function();
      }
    ;

optFormalDeclList
    : /* empty */
      { $$ = NULL; }
    | formalDeclList
      { $$ = $1; }
    ;

formalDeclList
    : formalDecl
      {
          $$ = maketree(FORMALDECLLIST);
          addChild($$, $1);
      }
    | formalDecl COMMA formalDeclList
      {
          $$ = maketree(FORMALDECLLIST);
          addChild($$, $1);
          addAllChildren($$, $3);
      }
    ;

formalDecl
    : typeSpecifier ID
      {
          int existing = ST_lookup_current_index($2);
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert($2, $1->val, SCALAR, NULL);
              add_param($1->val, SCALAR);
              current_param_count++;
          }
          $$ = maketree(FORMALDECL);
          addChild($$, $1);
          addChild($$, make_identifier_node(idx));
          if (existing == -1) {
              symEntry *entry = ST_lookup($2);
              if (entry != NULL) {
                  entry->size = 0;
              }
          }
      }
    | typeSpecifier ID LSQ_BRKT RSQ_BRKT
      {
          int existing = ST_lookup_current_index($2);
          int idx = existing;
          if (existing != -1) {
              sem_error("multiply declared symbol");
          } else {
              idx = ST_insert($2, $1->val, ARRAY, NULL);
              add_param($1->val, ARRAY);
              current_param_count++;
          }
          $$ = maketree(FORMALDECL);
          addChild($$, $1);
          addChild($$, make_identifier_node(idx));
          if (existing == -1) {
              symEntry *entry = ST_lookup($2);
              if (entry != NULL) {
                  entry->size = 0;
              }
          }
      }
    ;

funBody
    : LCRLY_BRKT statementList RCRLY_BRKT
      {
          $$ = maketree(FUNBODY);
          addChild($$, $2);
      }
    ;

statementList
    : /* empty */
      {
          $$ = maketree(STATEMENTLIST);
      }
    | statement statementList
      {
          $$ = maketree(STATEMENTLIST);
          if ($1 != NULL) addChild($$, $1);
          addAllChildren($$, $2);
      }
    ;

statement
    : SEMICLN
      {
          $$ = maketree(STATEMENT);
      }
    | varDecl
      { $$ = $1; }
    | compoundStmt
      { $$ = $1; }
    | assignStmt
      { $$ = $1; }
    | condStmt
      { $$ = $1; }
    | loopStmt
      { $$ = $1; }
    | returnStmt
      { $$ = $1; }
    ;

compoundStmt
    : LCRLY_BRKT statementList RCRLY_BRKT
      {
          $$ = maketree(COMPOUNDSTMT);
          if ($2 != NULL) addChild($$, $2);
      }
    ;

assignStmt
    : var OPER_ASGN expression SEMICLN
      {
          check_assignment($1, $3);
          $$ = maketree(ASSIGNSTMT);
          addChild($$, $1);
          addChild($$, $3);
      }
    | expression SEMICLN
      {
          $$ = $1;
      }
    ;

condStmt
    : KWD_IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
      {
          $$ = maketree(CONDSTMT);
          addChild($$, $3);
          addChild($$, $5);
      }
    | KWD_IF LPAREN expression RPAREN statement KWD_ELSE statement
      {
          $$ = maketree(CONDSTMT);
          addChild($$, $3);
          addChild($$, $5);
          addChild($$, $7);
      }
    ;

loopStmt
    : KWD_WHILE LPAREN expression RPAREN statement
      {
          $$ = maketree(LOOPSTMT);
          addChild($$, $3);
          addChild($$, $5);
      }
    ;

returnStmt
    : KWD_RETURN SEMICLN
      {
          check_return(NULL);
          $$ = maketree(RETURNSTMT);
      }
    | KWD_RETURN expression SEMICLN
      {
          check_return($2);
          $$ = maketree(RETURNSTMT);
          addChild($$, $2);
      }
    ;

var
    : ID
      {
          symEntry *entry = ST_lookup($1);
          int idx = (entry == NULL || entry->symbol_type == FUNCTION) ? -1 : ST_lookup_index($1);
          $$ = maketree(VAR);
          addChild($$, make_identifier_node(idx));
          if (entry == NULL || entry->symbol_type == FUNCTION) {
              sem_error("undeclared symbol");
              set_node_attrs($$, VOID_TYPE, SCALAR);
          } else {
              set_node_attrs($$, entry->data_type, entry->symbol_type);
          }
      }
    | ID LSQ_BRKT addExpr RSQ_BRKT
      {
          symEntry *entry = ST_lookup($1);
          int idx = (entry != NULL && entry->symbol_type == ARRAY) ? ST_lookup_index($1) : -1;
          $$ = maketree(ARRAYDECL);
          addChild($$, make_identifier_node(idx));
          addChild($$, $3);
          if (entry == NULL) {
              sem_error("undeclared symbol");
              set_node_attrs($$, VOID_TYPE, SCALAR);
          } else {
              check_array_index($1, $3, $$);
              addAllChildren($$, NULL);
          }
      }
    ;

expression
    : addExpr
      { $$ = $1; }
    | expression relop addExpr
      {
          if (is_unindexed_array_var($1) || is_unindexed_array_var($3)) {
              sem_error("array indexing error");
          } else if (type_of_node($1) != type_of_node($3)) {
              sem_error("type mismatch");
          }
          $$ = maketree(EXPRESSION);
          addChild($$, $1);
          addChild($$, $2);
          addChild($$, $3);
          set_node_attrs($$, INT_TYPE, SCALAR);
      }
    ;

relop
    : OPER_LTE { $$ = maketreeWithVal(RELOP, LTE); set_node_attrs($$, INT_TYPE, SCALAR); }
    | OPER_LT  { $$ = maketreeWithVal(RELOP, LT);  set_node_attrs($$, INT_TYPE, SCALAR); }
    | OPER_GT  { $$ = maketreeWithVal(RELOP, GT);  set_node_attrs($$, INT_TYPE, SCALAR); }
    | OPER_GTE { $$ = maketreeWithVal(RELOP, GTE); set_node_attrs($$, INT_TYPE, SCALAR); }
    | OPER_EQ  { $$ = maketreeWithVal(RELOP, EQ);  set_node_attrs($$, INT_TYPE, SCALAR); }
    | OPER_NEQ { $$ = maketreeWithVal(RELOP, NEQ); set_node_attrs($$, INT_TYPE, SCALAR); }
    ;

addExpr
    : term
      { $$ = $1; }
    | addExpr addop term
      {
          if (is_unindexed_array_var($1) || is_unindexed_array_var($3)) {
              sem_error("array indexing error");
          } else if (type_of_node($1) != type_of_node($3)) {
              sem_error("type mismatch");
          }
          $$ = maketree(ADDEXPR);
          addChild($$, $1);
          addChild($$, $2);
          addChild($$, $3);
          set_node_attrs($$, type_of_node($1), SCALAR);
      }
    ;

addop
    : OPER_ADD { $$ = maketreeWithVal(ADDOP, ADD); set_node_attrs($$, INT_TYPE, SCALAR); }
    | OPER_SUB { $$ = maketreeWithVal(ADDOP, SUB); set_node_attrs($$, INT_TYPE, SCALAR); }
    ;

term
    : factor
      { $$ = $1; }
    | term mulop factor
      {
          if (is_unindexed_array_var($1) || is_unindexed_array_var($3)) {
              sem_error("array indexing error");
          } else if (type_of_node($1) != type_of_node($3)) {
              sem_error("type mismatch");
          }
          $$ = maketree(TERM);
          addChild($$, $1);
          addChild($$, $2);
          addChild($$, $3);
          set_node_attrs($$, type_of_node($1), SCALAR);
      }
    ;

mulop
    : OPER_MUL { $$ = maketreeWithVal(MULOP, MUL); set_node_attrs($$, INT_TYPE, SCALAR); }
    | OPER_DIV { $$ = maketreeWithVal(MULOP, DIV); set_node_attrs($$, INT_TYPE, SCALAR); }
    ;

factor
    : LPAREN expression RPAREN
      { $$ = $2; }
    | var
      { $$ = $1; }
    | funCallExpr
      { $$ = $1; }
    | INTCONST
      {
          $$ = maketreeWithVal(INTEGER, $1);
          set_node_attrs($$, INT_TYPE, SCALAR);
      }
    | CHARCONST
      {
          $$ = maketreeWithVal(CHAR, $1);
          set_node_attrs($$, CHAR_TYPE, SCALAR);
      }
    ;

funCallExpr
    : ID LPAREN argList RPAREN
      {
          symEntry *entry = ST_lookup($1);
          int idx = (entry != NULL && entry->symbol_type == FUNCTION) ? ST_lookup_index($1) : -1;
          $$ = maketree(FUNCCALLEXPR);
          addChild($$, make_identifier_node(idx));
          if ($3 != NULL) addChild($$, $3);
          check_function_call($1, $3, $$);
      }
    | ID LPAREN RPAREN
      {
          symEntry *entry = ST_lookup($1);
          int idx = (entry != NULL && entry->symbol_type == FUNCTION) ? ST_lookup_index($1) : -1;
          $$ = maketree(FUNCCALLEXPR);
          addChild($$, make_identifier_node(idx));
          check_function_call($1, NULL, $$);
      }
    ;

argList
    : expression
      {
          $$ = maketree(ARGLIST);
          addChild($$, $1);
      }
    | argList COMMA expression
      {
          $$ = $1;
          addChild($$, $3);
      }
    ;
%%

int yywarning(char *msg) {
    printf("warning: line %d: %s\n", yylineno, msg);
    return 0;
}

int yyerror(char *msg) {
    printf("error: line %d: %s\n", yylineno, msg);
    return 0;
}
