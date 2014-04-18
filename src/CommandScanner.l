%{
#include <string>
#include "CommandAST.hpp"
#include "CommandParser.hpp"
#define SAVE_TOKEN cmdyylval.arg = new std::string(cmdyytext, cmdyyleng)
#define TOKEN(t) (cmdyylval.token = t)
extern "C" int cmdyywrap() {}
#define CMDYY_USER_ACTION cmdyylloc.first_line = cmdyylloc.last_line = cmdyylineno;
%}

%option yylineno

%%

[ \t\r]         ;
"build"         return TOKEN(C_BUILD);
"from"          return TOKEN(CB_FROM);
"pkg"           return TOKEN(CB_PKG);
"code"          return TOKEN(CB_CODE);
"ast"           return TOKEN(CB_AST);

.*\.pmm         SAVE_TOKEN; return F_PMM;
.*\.pm          SAVE_TOKEN; return F_PM;
.*              SAVE_TOKEN; return ARG;

%%

