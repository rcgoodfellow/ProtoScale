%{
  #include "CommandAST.hpp"
  CommandBase *cm;
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

%token <arg> ARG
%token <token> C_BUILD CB_PKG CB_CODE CB_AST CB_FROM;

%type <cmd> command build
%type <args> sources

%start command

%%

command : build { cm = $1; }
        ;

build : C_BUILD CB_AST CB_FROM sources { $$ = new BuildASTCommand{$4}; }
/*    | C_BUILD CB_PKG CB_FROM srouces
      | C_BUILD CB_CODE CB_FROM srouces   */
      ;

sources : ARG { $$ = new Args; $$->push_back(*$1); }
        | sources ARG { $1->push_back(*$2); }
        ;

%%
