%{
#include <string>
#include "CommandAST.hpp"
#include "CommandParser.hpp"
#define SAVE_TOKEN cmdyylval.arg = new std::string(cmdyytext, cmdyyleng)
#define TOKEN(t) (cmdyylval.token = t)
#define CMDYY_USER_ACTION cmdyylloc.first_line = cmdyylloc.last_line = cmdyylineno;
%}

%option yylineno
%option noyywrap

%%

[ \t]*                ;
"build"               return TOKEN(C_BUILD);
"from"                return TOKEN(CB_FROM);
"pkg"                 return TOKEN(CB_PKG);
"code"                return TOKEN(CB_CODE);
"ast"                 return TOKEN(CB_AST);

[0-9a-zA-Z_\./"']*    SAVE_TOKEN; return ARG;
.                     printf("Unknown token: %s\n", cmdyytext); yyterminate();

%%

