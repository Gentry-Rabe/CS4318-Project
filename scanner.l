
/* definitions */

%{
#include <stdio.h>
#include <stdlib.h>
#include "tokendef.h"

 /* integers to track line and column numbers */
int scancol = 1;
int yycol = 1;
int scanlineno = 1;

 /* char pointer to pass error messages to driver.c */
char *yyerror;

 /* TODO: functions to update line and column numbers */
void updateCol();
void countLines();

 /* TODO: function to process a string literal */
int processString();

%}

newline         //TODO
whitespace      //TODO
integer         //TODO
character       //TODO

 /* String can contain any characters between the double quotes */
 /* other than a newline or unescaped doublequotes */
*/
string       //TODO


 /* If the end quote is not found in the same line the string is unterminated */
untermstring //TODO

comment         //TODO
multlncomment   //TODO
untermcomment   //TODO

identifier      //TODO
illidentifier   //TODO: regular expression for an illegal identifier

yylineno = 1;

%%

 /* rules */

 /* Keywords */
"if"        { return KWD_IF; }
"else"      { return KWD_ELSE; }
"while"     { return KWD_WHILE; }
"int"       { return KWD_INT; }
"string"    { return KWD_STRING; }
"char"      { return KWD_CHAR; }
"return"    { return KWD_RETURN; }
"void"      { return KWD_VOID; }

 /* Operators */
"+"          { return KWD_OPER_ADD; }
"-"          { return KWD_OPER_SUB; }
"*"          { return KWD_OPER_MUL; }
"/"          { return KWD_OPER_DIV; }
"<"          { return KWD_PER_LT; }
">"          { return KWD_OPER_GT; }
">="          { return KWD_OPER_GTE; }
"<="          { return KWD_OPER_LTE; }
"=="          { return KWD_OPER_EQ; }
"!="          { return KWD_OPER_NEQ; }
"="          { return KWD_OPER_ASGN; }
"@"          { return KWD_OPER_AT; }
"++"          { return KWD_OPER_INC; }
"--"          { return KWD_OPER_DEC; }
"&&"          { return KWD_OPER_AND; }
"||"          { return KWD_OPER_OR; }
"!"          { return KWD_OPER_NOT; }
"%"          { return KWD_OPER_MOD; }

 /* Identifiers */;
{identifier}    //TODO
{illidentifier} //TODO

 /* Constants */;
{integer}       //TODO
{integerlead0}  //TODO: "Integers may not have leading zeros"
{character}     //TODO
{string}        {updateCol(); return processString();}
{untermstring}  {updateCol(); yyerror = "Unterminated string"; return ERROR;}

 /* Comments */
{comment}       //TODO
{untermcomment} //TODO
{multlncomment} //TODO

 /* Other */
{newline}       //TODO
{whitespace}    //TODO
.               {return ILLEGAL_TOK;}

%%

/* user routines */

/* TODO:
Implement this function to keep track of column numbers
*/
void updateCol(){
}

/* TODO:
Implement this function to keep track of line numbers
*/
void countLines(){
}


/* TODO:
Implement this function to
check for illegal escape sequences in string literals
and
convert valid escape sequences into escaped characters
(string[0] == '\\' and string[1] == 'n', then string[0] should be '\n')
*/
int processString(){
}
