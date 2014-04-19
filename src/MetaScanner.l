%{
#include <string>
#include "MetaAST.hpp"
#include "MetaParser.hpp"
#define SAVE_TOKEN metayylval.string = new std::string(metayytext, metayyleng)
#define TOKEN(t) (metayylval.token = t)
#define YY_USER_ACTION metayylloc.first_line = metayylloc.last_line = metayylineno;
%}

%option yylineno
%option noyywrap

%%
`.*`                    ; //comment
[ \t\r\n]               ;
"module"                return TOKEN(TKMODULE);
"node"                  return TOKEN(TKNODE);
"link"                  return TOKEN(TKLINK);
"info"                  return TOKEN(TKINFO);
"alias"                 return TOKEN(TALIAS);
"interlate"             return TOKEN(TINTERLATE);

[a-zA-Z_][a-zA-Z0-9_\.]*  SAVE_TOKEN; return TIDENT;
[0-9]+(\.[0-9]*)*       SAVE_TOKEN; return TREAL;


":="                    return TOKEN(TASSIGN);
":"                     return TOKEN(TSEMI);
"::"                    return TOKEN(TQUAL);
"+="                    return TOKEN(TPLEQ);
"="                     return TOKEN(TEQ);
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
"^"                     return TOKEN(TPOW);
"["                     return TOKEN(TARROPEN);
"]"                     return TOKEN(TARRCLOSE);
"("                     return TOKEN(TPOPEN);
")"                     return TOKEN(TPCLOSE);
","                     return TOKEN(TCOMMA);
"."                     return TOKEN(TDOT);
.                       printf("Unknown token: %s\n", metayytext); yyterminate();

%%
