%{
#include <string>
#include "ASTNode.hpp"
#include "Parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() {}
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
%}

%option yylineno

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
.                       printf("Unknown token: %s\n", yytext); yyterminate();

%%
