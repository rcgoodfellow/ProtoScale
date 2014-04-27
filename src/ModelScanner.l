%{
#include <string>
#include "ModelAST.hpp"
#include "ModelParser.hpp"
#define SAVE_TOKEN modelyylval.string = new std::string(modelyytext, modelyyleng)
#define TOKEN(t) (modelyylval.token = t)
#define YY_USER_ACTION modelyylloc.first_line = modelyylloc.last_line = modelyylineno;
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
"model"                   return TOKEN(TK_MODEL);

[a-zA-Z_][a-zA-Z0-9_\.]*  SAVE_TOKEN; return TL_IDENT;
[0-9]+(\.[0-9]*)*         SAVE_TOKEN; return TL_REAL;

";"                       return TOKEN(TO_SEMI);
":"                       return TOKEN(TO_COLON);
","                       return TOKEN(TO_COMMA);
"{"                       return TOKEN(TS_OBR);
"}"                       return TOKEN(TS_CBR);
.                         printf("Unknown token: %s\n", modelyytext); yyterminate();
%%
