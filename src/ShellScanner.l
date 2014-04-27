%{
#include <string>
#include "ShellAST.hpp"
#include "ShellParser.hpp"
#define SAVE_TOKEN shellyylval.string = new std::string(shellyytext, shellyyleng)
#define TOKEN(t) (shellyylval.token = t)
#define YY_USER_ACTION shellyylloc.first_line = shellyylloc.last_line = shellyylineno;
%}

%option yylineno
%option noyywrap

%%
`.*`                      ; //comment
[ \t\r\n]                 ;
"import"                  return TOKEN(TK_IMPORT);
"create"                  return TOKEN(TK_CREATE);
"connect"                 return TOKEN(TK_CONNECT);
"symmetric"               return TOKEN(TK_SYMMETRIC);

[a-zA-Z_][a-zA-Z0-9_\.]*  SAVE_TOKEN; return TL_IDENT;
[0-9]+(\.[0-9]*)*         SAVE_TOKEN; return TL_REAL;

";"                       return TOKEN(TO_SEMI);
","                       return TOKEN(TO_COMMA);
"{"                       return TOKEN(TS_OBR);
"}"                       return TOKEN(TS_CBR);
.                         printf("Unknown token: %s\n", shellyytext); yyterminate();
%%
