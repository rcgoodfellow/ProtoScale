%{
  #include "CommandAST.hpp"
  ps::cmd::CommandBase *cm;
  using namespace ps::cmd;
  extern int yylex();
%}

%locations
%error-verbose

%union {
  ps::cmd::CommandBase *cmd;
  ps::cmd::Arg *arg;
  ps::cmd::Args *args;
  int token;
}

%{
  void yyerror(const char *s) 
  { 
    printf("error: %s\n", s); 
  }
%}

%token <arg> ARG
%token <token> C_BUILD CB_PKG CB_CODE CB_AST CB_FROM;

%type <cmd> command build
%type <args> sources

%start command

%%

command : build { cm = $1; }
        ;

build : C_BUILD CB_AST CB_FROM sources { $$ = new BuildASTCommand{$4}; }
      | C_BUILD CB_PKG CB_FROM sources { $$ = new BuildPKGCommand{$4}; }
      ;

sources : ARG { $$ = new Args; $$->push_back(*$1); }
        | sources ARG { $1->push_back(*$2); }
        ;

%%
