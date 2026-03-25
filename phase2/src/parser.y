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
char* scope = "";
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

/* TODO: Add the rest of the tokens below.*/

%token <strval> CHARCONST  
%token <strval> STRCONST   

/* keywords */
%token <strval> KWD_IF     
%token <strval> KWD_ELSE   
%token <strval> KWD_WHILE  
%token <strval> KWD_INT    
%token <strval> KWD_STRING 
%token <strval> KWD_CHAR   
%token <strval> KWD_RETURN 
%token <strval> KWD_VOID   

/* operators */
%token <strval> OPER_ADD   
%token <strval> OPER_SUB   
%token <strval> OPER_MUL   
%token <strval> OPER_DIV   
%token <strval> OPER_LT    
%token <strval> OPER_GT    
%token <strval> OPER_GTE   
%token <strval> OPER_LTE   
%token <strval> OPER_EQ    
%token <strval> OPER_NEQ   
%token <strval> OPER_ASGN  
%token <strval> OPER_AT    
%token <strval> OPER_INC   
%token <strval> OPER_DEC   
%token <strval> OPER_AND   
%token <strval> OPER_OR    
%token <strval> OPER_NOT   
%token <strval> OPER_MOD   

/* brackets & parens */
%token <strval> LSQ_BRKT   
%token <strval> RSQ_BRKT   
%token <strval> LCRLY_BRKT 
%token <strval> RCRLY_BRKT 
%token <strval> LPAREN     
%token <strval> RPAREN     

/* punctuation */
%token <strval> COMMA      
%token <strval> SEMICLN    

/* other */
%token <strval> ERROR      
%token <strval> ILLEGAL_TOK

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




%start program

%%
/* TODO: Your grammar and semantic actions go here.
  We provide with two example productions
    and their associated code for adding non-terminals to the AST.
  */

program         : declList
                 {
                    tree *progNode = maketree(PROGRAM);
                    addChild(progNode, $1);
                    ast = progNode;
                 }
                ;

declList        : decl
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

/* TODO: This isn't the correct grammar for decl */
decl            : INTCONST
                 {
                    // $1 is an INTCONST
                    // INTCONST defined above as a token with yylval.value
                    printf("INCONST=%d\n",$1);
                    // the tree functions aren't implemented
                    $$ = maketree(DECL);
                    // so $$ is now NULL, we can't dereference it
                    //$$->val = $1;
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
