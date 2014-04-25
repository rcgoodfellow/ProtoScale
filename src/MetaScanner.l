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
`.*`                      ; //comment
[ \t\r\n]                 ;
"module"                  return TOKEN(TK_MODULE);
"node"                    return TOKEN(TK_NODE);
"link"                    return TOKEN(TK_LINK);

"complex"                 return TOKEN(TT_COMPLEX);
"real"                    return TOKEN(TT_REAL);

[a-zA-Z_][a-zA-Z0-9_\.]*  SAVE_TOKEN; return TL_IDENT;
[0-9]+(\.[0-9]*)*         SAVE_TOKEN; return TL_REAL;


":="                      return TOKEN(TO_ASSIGN);
"<-"                      return TOKEN(TO_GETS);
":"                       return TOKEN(TO_COLON);
";"                       return TOKEN(TO_SEMI);
"+="                      return TOKEN(TO_PLEQ);
"*="                      return TOKEN(TO_MUEQ);
"="                       return TOKEN(TO_EQ);
"+"                       return TOKEN(TO_PLUS);
"-"                       return TOKEN(TO_MINUS);
"*"                       return TOKEN(TO_MUL);
"/"                       return TOKEN(TO_DIV);
"^"                       return TOKEN(TO_POW);
"."                       return TOKEN(TO_DOT);
","                       return TOKEN(TO_COMMA);
"("                       return TOKEN(TS_POPEN);
")"                       return TOKEN(TS_PCLOSE);
"{"                       return TOKEN(TS_BOPEN);
"}"                       return TOKEN(TS_BCLOSE);
"'"                       return TOKEN(TO_PRIME);
.                         printf("Unknown token: %s\n", metayytext); yyterminate();

%%
