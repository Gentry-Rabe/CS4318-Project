%{
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
%}

/* the union describes the fields available in the yylval variable */
%union
{
    int value;
    struct treenode *node;
    char *strval;
}

/*Add token declarations below.
  The type <value> indicates that the associated token will be
    of a value type such as integer, float etc.,
    and <strval> indicates that the associated token will be of string type.
  */
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



/* TODO: Declare non-terminal symbols as of type node.
  Provided below is one example.
  node is defined as 'struct treenode *node' in the above union data structure.
  This declaration indicates to parser that these non-terminal variables
    will be implemented using a 'treenode *' type data structure.
  Hence, the circles you draw when drawing a parse tree,
    the following lines are telling yacc that these
    will eventually become circles in an AST.
  This is one of the connections between the AST you draw by hand
    and how yacc implements code to concretize that.
  We provide with two examples:
    program and declList from the grammar. Make sure to add the rest.
*/

%type <node> program declList decl
%type <node> varDecl typeSpecifier funDecl
%type <node> optFormalDeclList
%type <node> formalDeclList formalDecl
%type <node> funBody statementList statement
%type <node> compoundStmt assignStmt condStmt loopStmt returnStmt
%type <node> var expression relop addExpr addop term mulop factor
%type <node> funCallExpr argList
/*%type <node> typeNode idNode intNode charNode*/

%start program

%%
/* TODO: Your grammar and semantic actions go here.
  We provide with two example productions
    and their associated code for adding non-terminals to the AST.
  */

program         
      : declList
      {
        tree *progNode = maketree(PROGRAM);
        addChild(progNode, $1);
        ast = progNode;
      }
      ;

declList        
      : decl
      {
        tree* declListNode = maketree(DECLLIST);
        addChild(declListNode, $1);
        $$ = declListNode;
      }
    | declList decl
      {
        tree* declListNode = maketree(DECLLIST);
        addChild(declListNode, $1);
        addChild(declListNode, $2);
        $$ = declListNode;
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
    : KWD_INT   { $$ = maketreeWithVal(TYPESPEC, INT_TYPE); }
    | KWD_CHAR  { $$ = maketreeWithVal(TYPESPEC, CHAR_TYPE); }
    | KWD_VOID  { $$ = maketreeWithVal(TYPESPEC, VOID_TYPE); }
    ;

varDecl
    : typeSpecifier ID SEMICLN
      {
          ST_insert($2, scope, $1->val, SCALAR);
          $$ = maketree(VARDECL);
          addChild($$, $1);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($2);
          addChild($$, id);
      }
    | typeSpecifier ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN
      {
          ST_insert($2, scope, $1->val, ARRAY);
          $$ = maketree(VARDECL);
          addChild($$, $1);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($2);
          addChild($$, id);
          addChild($$, maketreeWithVal(INTEGER, $4));
      }
    ;

funDecl
    : typeSpecifier ID
      {
          set_scope($2);
          ST_insert($2, "", $1->val, FUNCTION);
      }
      LPAREN optFormalDeclList RPAREN funBody
      {
          $$ = maketree(FUNDECL);

          tree *ft = maketree(FUNCTYPENAME);
          addChild(ft, $1);

          tree *id = maketree(IDENTIFIER);
          id->text = strdup($2);
          addChild(ft, id);

          addChild($$, ft);
          if ($5) addChild($$, $5);
          addChild($$, $7);

          set_scope("");
      }
    ;

optFormalDeclList // Left- factoring to fix reduce/reduce conflict
    : /* empty */
      {
          $$ = NULL;
      }
    | formalDeclList
      {
          $$ = $1;
      }
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
          addChild($$, $3);
      }
    ;

formalDecl
    : typeSpecifier ID
      {
          ST_insert($2, scope, $1->val, SCALAR);
          $$ = maketree(FORMALDECL);
          addChild($$, $1);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($2);
          addChild($$, id);
      }
    | typeSpecifier ID LSQ_BRKT RSQ_BRKT
      {
          ST_insert($2, scope, $1->val, ARRAY);
          $$ = maketree(FORMALDECL);
          addChild($$, $1);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($2);
          addChild($$, id);
      }
    ;

funBody
    : LCRLY_BRKT statementList RCRLY_BRKT
      {
          $$ = maketree(FUNBODY);
          if ($2) addChild($$, $2);
      }
    ;

statementList
    : /* empty */
      {
          $$ = NULL;
      }
    | statement statementList
      {
          $$ = maketree(STATEMENTLIST);
          addChild($$, $1);
          if ($2) addChild($$, $2);
      }
    ;

statement
    : SEMICLN
      {
          $$ = maketree(STATEMENT);
      }
    | varDecl
      {
          $$ = maketree(STATEMENT);
          addChild($$, $1);
      }
    | compoundStmt
      {
          $$ = maketree(STATEMENT);
          addChild($$, $1);
      }
    | assignStmt
      {
          $$ = maketree(STATEMENT);
          addChild($$, $1);
      }
    | condStmt
      {
          $$ = maketree(STATEMENT);
          addChild($$, $1);
      }
    | loopStmt
      {
          $$ = maketree(STATEMENT);
          addChild($$, $1);
      }
    | returnStmt
      {
          $$ = maketree(STATEMENT);
          addChild($$, $1);
      }
    ;

compoundStmt
    : LCRLY_BRKT statementList RCRLY_BRKT
      {
          $$ = maketree(COMPOUNDSTMT);
          if ($2) addChild($$, $2);
      }
    ;

assignStmt
    : var OPER_ASGN expression SEMICLN
      {
          $$ = maketree(ASSIGNSTMT);
          addChild($$, $1);
          addChild($$, $3);
      }
    | expression SEMICLN
      {
          $$ = maketree(ASSIGNSTMT);
          addChild($$, $1);
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
          $$ = maketree(RETURNSTMT);
      }
    | KWD_RETURN expression SEMICLN
      {
          $$ = maketree(RETURNSTMT);
          addChild($$, $2);
      }
    ;

var
    : ID
      {
          if (resolve_symbol($1) == -1) {
              yywarning("undeclared symbol");
          }
          $$ = maketree(VAR);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($1);
          addChild($$, id);
      }
    | ID LSQ_BRKT addExpr RSQ_BRKT
      {
          if (resolve_symbol($1) == -1) {
              yywarning("undeclared symbol");
          }
          $$ = maketree(ARRAYDECL);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($1);
          addChild($$, id);
          addChild($$, $3);
      }
    ;

expression
    : addExpr
      {
          $$ = maketree(EXPRESSION);
          addChild($$, $1);
      }
    | expression relop addExpr
      {
          $$ = maketree(EXPRESSION);
          addChild($$, $1);
          addChild($$, $2);
          addChild($$, $3);
      }
    ;

relop
    : OPER_LTE { $$ = maketreeWithVal(RELOP, 0); }
    | OPER_LT  { $$ = maketreeWithVal(RELOP, 1); }
    | OPER_GT  { $$ = maketreeWithVal(RELOP, 2); }
    | OPER_GTE { $$ = maketreeWithVal(RELOP, 3); }
    | OPER_EQ  { $$ = maketreeWithVal(RELOP, 4); }
    | OPER_NEQ { $$ = maketreeWithVal(RELOP, 5); }
    ;

addExpr
    : term
      {
          $$ = maketree(ADDEXPR);
          addChild($$, $1);
      }
    | addExpr addop term
      {
          $$ = maketree(ADDEXPR);
          addChild($$, $1);
          addChild($$, $2);
          addChild($$, $3);
      }
    ;

addop
    : OPER_ADD { $$ = maketreeWithVal(ADDOP, 0); }
    | OPER_SUB { $$ = maketreeWithVal(ADDOP, 1); }
    ;

term
    : factor
      {
          $$ = maketree(TERM);
          addChild($$, $1);
      }
    | term mulop factor
      {
          $$ = maketree(TERM);
          addChild($$, $1);
          addChild($$, $2);
          addChild($$, $3);
      }
    ;

mulop
    : OPER_MUL { $$ = maketreeWithVal(MULOP, 0); }
    | OPER_DIV { $$ = maketreeWithVal(MULOP, 1); }
    ;

factor
    : LPAREN expression RPAREN
      {
          $$ = maketree(FACTOR);
          addChild($$, $2);
      }
    | var
      {
          $$ = maketree(FACTOR);
          addChild($$, $1);
      }
    | funCallExpr
      {
          $$ = maketree(FACTOR);
          addChild($$, $1);
      }
    | INTCONST
      {
          $$ = maketreeWithVal(INTEGER, $1);
      }
    | CHARCONST
      {
        $$ = maketreeWithVal(CHAR, $1);
      }
    ;

funCallExpr
    : ID LPAREN argList RPAREN
      {
          if (resolve_function($1) == -1) {
              yywarning("undeclared symbol");
          }
          $$ = maketree(FUNCCALLEXPR);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($1);
          addChild($$, id);
          if ($3) addChild($$, $3);
      }
    | ID LPAREN RPAREN
      {
          if (resolve_function($1) == -1) {
              yywarning("undeclared symbol");
          }
          $$ = maketree(FUNCCALLEXPR);
          tree *id = maketree(IDENTIFIER);
          id->text = strdup($1);
          addChild($$, id);
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
          $$ = maketree(ARGLIST);
          addChild($$, $1);
          addChild($$, $3);
      }
    ;
%%

int yywarning(char *msg){
  printf("warning: line %d: %s\n", yylineno, msg);
  return 0;
}

int yyerror(char * msg){
  printf("error: line %d: %s\n", yylineno, msg);
  return 0;
}
