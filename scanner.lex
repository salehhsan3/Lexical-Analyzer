%{
    #include <stdio.h>
    #include <cstddef>
    #include "tokens.hpp"
%}

%option yylineno

%x START_COMMENT

WHITESPACE ([\t\r\n ])
INT ([i][n][t])
BYTE ([b][y][t][e])
B ([b])
BOOL ([b][o][o][l])
AND ([a][n][d])
OR ([o][r])
NOT ([n][o][t])
TRUE ([t][r][u][e])
FALSE ([f][a][l][s][e])
RETURN ([r][e][t][u][r][n])
IF ([i][f])
ELSE ([e][l][s][e])
WHILE ([w][h][i][l][e])
BREAK ([b][r][e][a][k])
CONTINUE ([c][o][n][t][i][n][u][e])
SC ([;])
LPAREN ([\(])
RPAREN ([\)])
LBRACE ([\{])
RBRACE ([\}])
ASSIGN ([=])
RELOP ([=][=]|[!][=]|<|>|[<][=]|[>][=])
COMMENT_SIGN [\/][\/]
BINOP ([+\-*\/])
ID ([a-zA-Z]+[a-zA-Z0-9]*)
NUM ([0]|[1-9]+[0-9]*)
PRINTABLE_ASCII ([ -!#-~])
/* ESCAPE_SEQUENCE ([\\]|[\n]|[\t]|[\r][\"][\0]) */
ESCAPE_SEQUENCE ([\"\\0ntr])
HEX_FORMAT (x[0-7][0-9a-fA-F])
/* STRING  (\"({PRINTABLE_ASCII}|(\\){ESCAPE_SEQUENCE}|(\\){HEX_FORMAT})*\") */
/* STRING (\"(\\.|[^\"\\\n\r])*\") */
/* UNCLOSED_STRING (\"(\\.|[^\"\\\n\r])*) */
STRING (\"(\\.|[^\"\\\n\r])*(\")?)
/* STRING (\"({PRINTABLE_ASCII}|\\{HEX_FORMAT}|\\{ESCAPE_SEQUENCE})*\") */
/* ERROR ( (((\")(\0)*{PRINTABLE_ASCII}*(\0)*(\\{HEX_FORMAT})*(\0)*(\\{ESCAPE_SEQUENCE})*(\0)*(\")?)) | (.) ) */
ERROR (.)

%%
{WHITESPACE} {continue;}
{INT} {return INT;}
{BYTE} {return BYTE;}
{B} {return B;}
{BOOL} {return BOOL;}
{AND} {return AND;}
{OR} {return OR;}
{NOT} {return NOT;}
{TRUE} {return TRUE;}
{FALSE} {return FALSE;}
{RETURN} {return RETURN;}
{IF} {return IF;}
{ELSE} {return ELSE;}
{WHILE} {return WHILE;}
{BREAK} {return BREAK;}
{CONTINUE} {return CONTINUE;}
{SC} {return SC;}
{LPAREN} {return LPAREN;}
{RPAREN} {return RPAREN;}
{LBRACE} {return LBRACE;}
{RBRACE} {return RBRACE;}
{ASSIGN} {return ASSIGN;}
{RELOP} {return RELOP;}
{BINOP} {return BINOP;}
{ID} {return ID;}
{NUM} {return NUM;}
{STRING} {return STRING;}
{ERROR} {return 0;} 

[\/][\/] { BEGIN(START_COMMENT); return COMMENT; }

<START_COMMENT>[^\n\r\n\r]* { /* skip */ }

<START_COMMENT>\n|\r|\r\n { BEGIN(INITIAL); }

%%

int yywrap()
{
    return 1;
}
