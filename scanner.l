
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

newline         \n
whitespace      [ \t]+
integer         [0-9]
/*Inserting definition for integers with leading '0's here since they're referenced in a macro later*/
integerlead0    0[0-9]+
character       \'([^\\\n]|\\.)\'

 /* String can contain any characters between the double quotes */
 /* other than a newline or unescaped doublequotes */
*/
string       \"([^\\\n]|\\.)*\"


 /* If the end quote is not found in the same line the string is unterminated */
untermstring \"([^\\\n]|\\.)*

comment         \/\/.*
multlncomment   \/\*([^*]|\*+[^*/])*\*+\/
untermcomment   \/\*([^*]|\*+[^*/])*

identifier      [a-zA-Z]([a-zA-Z]|[0-9]|_)*
illidentifier   [0-9]+([a-zA-Z]|_)+([a-zA-Z]|[0-9]|_)*

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
{identifier} {
    updateCol();
    return ID;
}
{illidentifier} {
    updateCol();
    yyerror = "Illegal identifier";
    return ERROR;
}

 /* Constants */;
{integerlead0} { //Needs to be before integer since order matters
    updateCol();
    yyerror = "Integers may not have leading zeros";
    return ERROR;
}
{integer} {
    updateCol();
    return INTCONST;
}
{character}     //TODO
{string}        {updateCol(); return processString();}
{untermstring}  {updateCol(); yyerror = "Unterminated string"; return ERROR;}

 /* Comments */
{comment} {
    updateCol();
    /* ignore single-line comment */
}
{untermcomment} {
    updateCol();
    yyerror = "Unterminated comment";
    return ERROR;
}
{multlncomment} {
    countLines();
    updateCol();
    /* ignore */
}

 /* Other */
{newline}       //TODO
{whitespace}    //TODO
.               {return ILLEGAL_TOK;}

%%

/* user routines */

void updateCol() {
    yycol += yyleng;
}

void countLines() {
    scanlineno++;
    yycol = 1;
}


/* TODO:
Implement this function to
check for illegal escape sequences in string literals
and
convert valid escape sequences into escaped characters
(string[0] == '\\' and string[1] == 'n', then string[0] should be '\n')
*/
int processString()
{

}
