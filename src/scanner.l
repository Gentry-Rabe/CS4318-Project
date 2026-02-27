
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
"+"   { return OPER_ADD; }
"-"   { return OPER_SUB; }
"*"   { return OPER_MUL; }
"/"   { return OPER_DIV; }
"<"   { return OPER_LT; }
">"   { return OPER_GT; }
">="  { return OPER_GTE; }
"<="  { return OPER_LTE; }
"=="  { return OPER_EQ; }
"!="  { return OPER_NEQ; }
"="   { return OPER_ASGN; }
"@"   { return OPER_AT; }
"++"  { return OPER_INC; }
"--"  { return OPER_DEC; }
"&&"  { return OPER_AND; }
"||"  { return OPER_OR; }
"!"   { return OPER_NOT; }
"%"   { return OPER_MOD; }

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
{character} {
    updateCol();

    if (yytext[1] == '\\') {
        switch (yytext[2]) {
            case 'n':  break;
            case 't':  break;
            case 'r':  break;
            case '\\': break;
            case '\'': break;
            default:
                yyerror = "Illegal escape in character constant";
                return ERROR;
        }
    }

    return CHARCONST;
}
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
{newline} {
    countLines();
}
{whitespace} {
    updateCol();
}
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

int processString()
{
    int i = 1;              /* skip opening quote */
    int j = 0;              /* write position */

    while (i < yyleng - 1) {   /* stop before closing quote */

        if (yytext[i] == '\\') {

            i++;

            switch (yytext[i]) {
                case 'n':  yytext[j++] = '\n'; break;
                case 't':  yytext[j++] = '\t'; break;
                case 'r':  yytext[j++] = '\r'; break;
                case '\\': yytext[j++] = '\\'; break;
                case '"':  yytext[j++] = '"';  break;

                default:
                    yyerror = "Illegal escape sequence in string";
                    return ERROR;
            }
        }
        else {
            yytext[j++] = yytext[i];
        }

        i++;
    }

    yytext[j] = '\0';   /* terminate processed string */

    return STRCONST;
}
