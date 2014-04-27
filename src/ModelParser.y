%{
#include "ModelAST.hpp"
#include <iostream>
ps::model::Model *mdl;
extern int yylex();
using namespace ps::model;
#define CURRLINE modelyylloc.first_line
%}

%locations
%error-verbose

%union {
  ps::model::Command *command;
  ps::model::Commands *commands;
  ps::model::Import *import;
  ps::model::Create *create;
  ps::model::Connect *connect;
  ps::model::CreateFormat *c_fmt;
  ps::model::CreateArgs *c_args;
  ps::model::CreateTarget *c_tgt;
  ps::model::CreateTargets *c_tgts;
  std::vector<std::string> *strings;
  ps::model::Connection *connection;
  ps::model::Connections *connections;
  std::string *string;
  int token;
  bool boolean;
}

%{
  void yyerror(const char *s) 
  { 
    printf("error[%d]: %s\n", modelyylloc.first_line, s); 
  }
%}

%token <string> TL_IDENT TL_REAL
%token <token> TK_IMPORT TK_CREATE TK_CONNECT TK_SYMMETRIC TK_MODEL
%token <token> TO_SEMI TO_COMMA TO_COLON TS_OBR TS_CBR

%type <model> model
%type <commands> commands
%type <command> command create import connect
%type<c_fmt> cr_format
%type<strings> names
%type<c_tgt> cr_tgt
%type<c_tgts> cr_tgts
%type<c_args> cr_tgt_args reals
%type<connection> connection
%type<connections> connections
%type<boolean> symmetric


%start model

%%

model: TK_MODEL TL_IDENT TO_COLON commands TO_COLON TO_COLON { mdl = new Model(*$2, $4, CURRLINE); }
     ;

commands: command { $$ = new Commands(); $$->push_back($1); } 
        | commands command { $1->push_back($2); }
        ;

command: import TO_SEMI { $$ = $1; }
       | create TO_SEMI { $$ = $1; }
       | connect TO_SEMI { $$ = $1; }
       ;

import: TK_IMPORT TL_IDENT { $$ = new Import(*$2, CURRLINE); }
      ;

create: TK_CREATE TL_IDENT cr_format cr_tgts 
        { $$ = new Create(*$2, $3, $4, CURRLINE); }
      ;

cr_format: {$$ = new CreateFormat(CURRLINE); }
         | TS_OBR names TS_CBR { $$ = new CreateFormat(CURRLINE, $2); }
         ;

names: TL_IDENT { $$ = new std::vector<std::string>(); $$->push_back(*$1); }
     | names TO_COMMA TL_IDENT { $1->push_back(*$3); }
     ;

cr_tgts: cr_tgt { $$ = new CreateTargets(); $$->push_back($1); } 
       | cr_tgts TO_COMMA cr_tgt { $1->push_back($3); }
       ;

cr_tgt: TL_IDENT cr_tgt_args { $$ = new CreateTarget(*$1, $2, CURRLINE); }
      ;

cr_tgt_args: { $$ = new CreateArgs(CURRLINE); }
           | TS_OBR reals TS_CBR { $$ = $2; }
           ;

reals: TL_REAL { $$ = new CreateArgs(CURRLINE); $$->vals.push_back(*$1); }
     | reals TO_COMMA TL_REAL { $1->vals.push_back(*$3); }
     ;

connect: TK_CONNECT symmetric connections { $$ = new Connect($3, $2, CURRLINE); }
       ;

symmetric: { $$ = false; }
         | TK_SYMMETRIC { $$ = true; }
         ;

connections: connection { $$ = new Connections; $$->push_back($1); }
           | connections TO_COMMA connection {$1->push_back($3); }
           ;

connection: TL_IDENT TL_IDENT TL_IDENT TL_IDENT 
            { $$ = new Connection(*$1, *$2, *$3, *$4, CURRLINE); }
          ;

%%

