%{
  #include "MetaAST.hpp"
  Module *mm;
  extern int yylex();
%}

%locations
%error-verbose

%union {
  Module *module;
  Element *element;
  Elements *elements;
  Variable *variable;
  Variables *variables;
  std::vector<std::string*> *strings;
  std::string *string;
  int token;
};

%{
  void yyerror(const char *s) 
  { 
    printf("error[%d]: %s\n", metayylloc.first_line, s); 
  }
%}

/* Literals */
%token <string> TL_IDENT TL_REAL TL_INT
/* Keywords */
%token <token> TK_MODULE TK_NODE TK_LINK
/* Built-In Types */
%token <token> TT_REAL TT_COMPLEX
/* Operators */
%token <token> TO_SEMI TO_ASSIGN TO_PLUS TO_MINUS TO_MUL TO_DIV TO_EQ TO_PLEQ
%token <token> TO_MUEQ TO_POW TO_DOT TO_COMMA
/* Sepcial Symbols */
%token <token> TS_POPEN TS_PCLOSE

%type <module> module
%type <element> link node
%type <elements> elements
%type <variables> var_decl_groups var_decl_group var_decl_groups_cs
%type <strings> var_names
%type <string> typename

%left TO_PLUS TO_MINUS
%left TO_MUL TO_DIV

%start module

%%

module: TK_MODULE TL_IDENT TO_SEMI elements { mm = new Module(*$2, *$4); }
      ;

elements: node {$$ = new Elements(); $$->push_back($1); }
        | link {$$ = new Elements(); $$->push_back($1); }
        | elements node { $1->push_back($2); }
        | elements link { $1->push_back($2); }
        ;

node: TK_NODE TL_IDENT TO_SEMI node_elements { $$ = new Node(*$2); }
    ;

node_elements: node_element
             | node_elements node_element
             ;

node_element: var_decl_group
            | alias
            | interlate
            ;

var_decl_groups: var_decl_group
               | var_decl_groups var_decl_group
               ;

var_decl_groups_cs: var_decl_group
                  | var_decl_groups TO_COMMA var_decl_group
                  ;

var_decl_group: var_names typename { $$ = new Variables();
                                for(std::string *v : *$1)
                                {
                                  $$->push_back(new Variable(*v, *$2));  
                                }
                              }
         ;

var_names: TL_IDENT { $$ = new std::vector<std::string*>(); $$->push_back($1); }
         | var_names TO_COMMA TL_IDENT { $1->push_back($3); }
         ;

typename: TT_COMPLEX { $$ = new std::string("complex"); }
        | TT_REAL { $$ = new std::string("real"); }
        | TL_IDENT { $$ = $1; }
        ;

aliases: alias
       | aliases alias

alias: var_names TO_ASSIGN stmts 
       ;

interlate: TL_IDENT TS_POPEN var_decl_groups_cs TS_PCLOSE TO_SEMI
         | eqtns
         ;

eqtn: TL_IDENT linkop expr
    ;

eqtns: eqtn
     | eqtns eqtn
     ;

linkop: TO_PLEQ
      | TO_MUEQ
      ;

stmts: expr
     | stmts TO_COMMA expr
     ;

expr: term
    | expr addop term
    ;

addop: TO_PLUS
     | TO_MINUS
     ;

mulop: TO_MUL
     | TO_DIV
     ;

term: factor
    | term mulop factor
    ;

factor: atom
      | atom TO_POW atom
      ;

atom: TL_REAL
    | TL_IDENT
    | TS_POPEN expr TS_PCLOSE
    | funcall
    ;

funcall: TL_IDENT TS_POPEN stmts TS_PCLOSE

link: TK_LINK TL_IDENT TO_SEMI 
        var_decl_groups 
        aliases { $$ = new Link(*$2); }
    ;

%%
