
/* definitions */

%{
#include <stdio.h>
#include <stdlib.h>
#include "tokendef.h"

 /* integers to track line and column numbers */
int scancol = 1;
int yycol = 1;
int scanlineno = 1; // what is the point of this variable? it's included in the assigned file but we needed not use it

 /* char pointer to pass error messages to driver.c */
char *yyerror;

void updateCol();
void countLines();

int processString();

%}

newline         \n
whitespace      [ \t]+
integer         [0-9]
 // inserting definition for integers with leading '0's here since they're referenced in a macro later
integerlead0    0[0-9]+
character       \'([^\\\n]|\\.)\'

 // string can contain any characters between the double quotes
 // other than a newline or unescaped doublequotes
string       \"([^\\\n]|\\.)*\"


 // if the end quote is not found in the same line the string is unterminated
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
"if" { // why do these all require this style of bracket instead of the extra spaced brackets I like? infuriating.
    updateCol();
    return KWD_IF; 
}
"else" { 
    updateCol();
    return KWD_ELSE; 
}
"while" { 
    updateCol(); 
    return KWD_WHILE; 
    }
"int" { 
    updateCol(); 
    return KWD_INT; 
    }
"string" { 
    updateCol(); 
    return KWD_STRING; 
    }
"char" { 
    updateCol(); 
    return KWD_CHAR; 
    }
"return" { 
    updateCol(); 
    return KWD_RETURN; 
    }
"void" { 
    updateCol(); 
    return KWD_VOID; 
    }

 /* Operators */
"+"   { 
    updateCol(); 
    return OPER_ADD; 
    }
"-"   { 
    updateCol(); 
    return OPER_SUB; 
    }
"*"   { 
    updateCol(); 
    return OPER_MUL; 
    }
"/"   { 
    updateCol(); 
    return OPER_DIV; 
    }
"<"   { 
    updateCol(); 
    return OPER_LT; 
    }
">"   { 
    updateCol(); 
    return OPER_GT; 
    }
">="  { 
    updateCol(); 
    return OPER_GTE; 
    }
"<="  { 
    updateCol(); 
    return OPER_LTE; 
    }
"=="  { 
    updateCol(); 
    return OPER_EQ; 
    }
"!="  { 
    updateCol(); 
    return OPER_NEQ; 
    }
"="   { 
    updateCol(); 
    return OPER_ASGN; 
    }
"@"   { 
    updateCol(); 
    return OPER_AT; 
    }
"++"  { 
    updateCol(); 
    return OPER_INC; 
    }
"--"  { 
    updateCol(); 
    return OPER_DEC; 
    }
"&&"  { 
    updateCol(); 
    return OPER_AND; 
    }
"||"  { 
    updateCol(); 
    return OPER_OR; 
    }
"!"   { 
    updateCol(); 
    return OPER_NOT; 
    }
"%"   { 
    updateCol(); 
    return OPER_MOD; 
    } // maybe we should condense these down to single lines

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
{integerlead0} { // needs to be before integer since order matters
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
    // ignore single line comment
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
    yylineno++;
    scancol = 1;
}
{whitespace} {
    updateCol();
}
.               {return ILLEGAL_TOK;}

%%

/* user routines */

void updateCol() {
    yycol = scancol;     // save starting column
    scancol += yyleng;     // advance column
}

void countLines() {
    yylineno++;
    scancol = 1;
}

int processString()
{
    int i = 1;              // skip opening quote
    int j = 0;              // write position

    while (i < yyleng - 1) // stop before closing quote
    {   
        if (yytext[i] == '\\') 
        {

            i++;

            switch (yytext[i]) 
            {
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
        else 
        {
            yytext[j++] = yytext[i];
        }

        i++;
    }

    yytext[j] = '\0';   // terminate processed string

    return STRCONST;
}
