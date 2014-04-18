%{
  #include "CommandAST.hpp"
  Commands *cmds;
  extern int yylex();
%}

%locations
%error-verbose

%union {
  CommandBase *cmd;
  Arg *arg;
  Args *args;
  int token;
}

%{
  void yyerror(const char *s) 
  { 
    printf("ERROR[%d]: %s\n", yylloc.first_line, s); 
  }
%}

%token <arg> F_PMM F_PM ARG
%token <token> C_BUILD CB_PKG CB_CODE CB_AST CB_FROM;

%type <cmd> command build
%type <args> sources

%start command

%%

command : build { $$ = $1; }
        ;

build : C_BUILD CB_AST CB_FROM sources { $$ = new BuildASTCommand{$4}; }
/*    | C_BUILD CB_PKG CB_FROM srouces
      | C_BUILD CB_CODE CB_FROM srouces   */
      ;

sources : F_PMM { $$ = new Args; $$->push_back(*$1); }
        | F_PM  { $$ = new Args; $$->push_back(*$1); }
        | sources F_PMM { $1->push_back(*$2); }
        | sources F_PM  { $1->push_back(*$2); }
        ;

%%
