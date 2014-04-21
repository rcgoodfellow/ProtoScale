%{
  #include "MetaAST.hpp"
  #include <iostream>
  Module *mm;
  extern int yylex();
%}

%locations
%error-verbose

%union {
  Module *module;
  Element *element;
  Elements *elements;
  Expr *expr;
  Exprs *exprs;
  Variable *variable;
  Variables *variables;
  std::vector<std::string*> *strings;
  std::string *string;
  Node *node;
  Link *link;
  Term *term;
  Terms *terms;
  NodeElement *node_element;
  NodeElements *node_elements;
  Factor *factor;
  Factors *factors;
  Atom *atom;
  Atoms *atoms;
  Funcall *funcall;
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
%type <strings> var_names
%type <string> typename
%type <node_element> interlate
%type <node_elements> node_element node_elements var_decl_group var_decl_groups var_decl_groups_cs alias
%type <expr> expr
%type <exprs> stmts
%type <term> term
%type <token> addop mulop
%type <factor> factor
%type <atom> atom
%type <funcall> funcall

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

node: TK_NODE TL_IDENT TO_SEMI node_elements 
                    { 
                      auto *n = new Node(*$2); 
                      for(NodeElement *v : *$4) 
                      { 
                        if(v->kind() == NodeElement::Kind::Variable)
                        {
                          n->vars.push_back(dynamic_cast<Variable*>(v)); 
                        }
                        if(v->kind() == NodeElement::Kind::Alias)
                        {
                          n->aliases.push_back(dynamic_cast<Alias*>(v));
                        }
                      }
                      $$ = n;
                    }
    ;

node_elements: node_element { $$ = new NodeElements(); 
                              $$->insert($$->end(), $1->begin(), $1->end()); }
             | node_elements node_element 
                            { $1->insert($1->end(), $2->begin(), $2->end()); }
             ;

node_element: var_decl_group { $$ = $1; }
            | alias { $$ = $1; }
            | interlate  { $$ = new NodeElements(); $$->push_back($1); }
            ;

var_decl_groups: var_decl_group { $$ = new NodeElements();
                                  $$->insert($$->end(), $1->begin(), $1->end()); }
               | var_decl_groups var_decl_group
                                { $1->insert($1->end(), $2->begin(), $2->end()); }
               ;

var_decl_groups_cs: var_decl_group { $$ = new NodeElements();
                                     $$->insert($$->end(), $1->begin(), $1->end()); }
                  | var_decl_groups_cs TO_COMMA var_decl_group
                                   { $1->insert($1->end(), $3->begin(), $3->end()); }
                  ;

var_decl_group: var_names typename 
              { $$ = new NodeElements();
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
       { 
         $$ = new NodeElements();  
         for(size_t i=0; i<$1->size(); ++i)
         {
            std::string s = *((*$1)[i]);
            Expr *e = (*$3)[i];
            $$->push_back(new Alias(s, e));
         }
       }
     ;

interlate: TL_IDENT TS_POPEN var_decl_groups_cs TS_PCLOSE TO_SEMI
            eqtns { $$ = new Interlate(); }
         ;

eqtn: TL_IDENT linkop expr
    ;

eqtns: eqtn
     | eqtns eqtn
     ;

linkop: TO_PLEQ
      | TO_MUEQ
      ;

stmts: expr { $$ = new Exprs(); $$->push_back($1); }
     | stmts TO_COMMA expr { $1->push_back($3); }
     ;

expr: term { $$ = new Expr($1); }
    | expr addop term { $1->op = $2; $1->r = $3; }
    ;

addop: TO_PLUS
     | TO_MINUS
     ;

mulop: TO_MUL
     | TO_DIV
     ;

term: factor { $$ = new Term($1); }
    | term mulop factor { $1->op = $2; $1->r = $3; }
    ;

factor: atom { $$ = new Factor($1); }
      | atom TO_POW atom { $$ = new Factor($1, $3); }
      ;

atom: TL_REAL { $$ = new Real(stod(*$1)); }
    | TL_IDENT { $$ = new Symbol(*$1); }
    | TS_POPEN expr TS_PCLOSE { $$ = new ExprAtom($2); }
    | funcall { $$ = new FuncallAtom($1); }
    ;

funcall: TL_IDENT TS_POPEN stmts TS_PCLOSE { $$ = new Funcall(*$1, *$3); }

link: TK_LINK TL_IDENT TO_SEMI 
        var_decl_groups 
        aliases { $$ = new Link(*$2); }
    ;

%%
