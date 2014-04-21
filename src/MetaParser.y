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
  NodeElement *node_element;
  NodeElements *node_elements;
  Atom *atom;
  Atoms *atoms;
  Funcall *funcall;
  Eqtn *eqtn;
  Eqtns *eqtns;
  AddOp *addop;
  MulOp *mulop;
  ExpOp *expop;
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
%token <token> TO_COLON TO_ASSIGN TO_PLUS TO_MINUS TO_MUL TO_DIV TO_EQ TO_PLEQ TO_SEMI TO_PRIME
%token <token> TO_MUEQ TO_POW TO_DOT TO_COMMA
/* Sepcial Symbols */
%token <token> TS_POPEN TS_PCLOSE

%type <module> module
%type <element> node link
%type <elements> elements
%type <strings> var_names
%type <string> typename
%type <node_element> interlate diffrel
%type <node_elements> node_element node_elements var_decl_group var_decl_groups_cs alias link_elements link_element
%type <expr> expr sum product exponential atom
%type <exprs> stmts
%type <token> addop mulop linkop
%type <funcall> funcall
%type <eqtns> eqtns
%type <eqtn> eqtn

%left TO_PLUS TO_MINUS
%left TO_MUL TO_DIV

%start module

%%

module: TK_MODULE TL_IDENT TO_COLON elements { mm = new Module(*$2, *$4); }
      ;

elements: TK_NODE node {$$ = new Elements(); $$->push_back($2); }
        | TK_LINK link {$$ = new Elements(); $$->push_back($2); }
        | elements TK_NODE node { $1->push_back($3); }
        | elements TK_LINK link { $1->push_back($3); }
        ;

node: TL_IDENT TO_COLON node_elements TO_COLON TO_COLON 
                    { 
                      auto *n = new Node(*$1); 
                      for(NodeElement *v : *$3) 
                      { 
                        if(v->kind() == NodeElement::Kind::Variable)
                        {
                          n->vars.push_back(dynamic_cast<Variable*>(v)); 
                        }
                        if(v->kind() == NodeElement::Kind::Alias)
                        {
                          n->aliases.push_back(dynamic_cast<Alias*>(v));
                        }
                        if(v->kind() == NodeElement::Kind::DiffRel)
                        {
                          n->diffrels.push_back(dynamic_cast<DiffRel*>(v));
                        }
                        if(v->kind() == NodeElement::Kind::Interlate)
                        {
                          n->interlates.push_back(dynamic_cast<Interlate*>(v));
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

node_element: var_decl_group TO_SEMI{ $$ = $1; }
            | alias TO_SEMI { $$ = $1; }
            | diffrel TO_SEMI 
                { $$ = new NodeElements(); $$->push_back($1); }
            | interlate TO_COLON TO_COLON
                { $$ = new NodeElements(); $$->push_back($1); }
            ;

diffrel: TL_IDENT TO_PRIME TO_EQ expr { $$ = new DiffRel(*$1, $4); }

var_decl_groups_cs: var_decl_group 
                    { 
                      $$ = new NodeElements();
                      $$->insert($$->end(), $1->begin(), $1->end()); 
                    }
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

interlate: TL_IDENT TS_POPEN var_decl_groups_cs TS_PCLOSE TO_COLON
           eqtns
           { Interlate *i = new Interlate(*$1); 
             for(NodeElement *e : *$3)
             {
               i->params.push_back(dynamic_cast<Variable*>(e));
             }
             i->eqtns = *$6;
             $$ = i; 
           }
         ;

eqtn: TL_IDENT linkop expr TO_SEMI { $$ = new Eqtn(*$1, $2, $3); }
    ;

eqtns: eqtn { $$ = new Eqtns(); $$->push_back($1); }
     | eqtns eqtn { $1->push_back($2); }
     ;

linkop: TO_PLEQ { $$ = $1; }
      | TO_MUEQ { $$ = $1; }
      | TO_EQ { $$ = $1; }
      ;

stmts: expr { $$ = new Exprs(); $$->push_back($1); }
     | stmts TO_COMMA expr { $1->push_back($3); }
     ;

expr: sum
    ;

sum: product { $$ = $1; }
   | sum addop sum { $$ = new AddOp($<addop>1, $<addop>3, $2); }
   ;

product: exponential { $$ = $1; }
       | product mulop product { $$ = new MulOp($<mulop>1, $<mulop>3, $2); }
       ;

exponential: atom { $$ = $1; }
           | exponential TO_POW exponential 
              { $$ = new ExpOp($<expop>1, $<expop>3, TO_POW); }
           ;

addop: TO_PLUS { $$ = $1; }
     | TO_MINUS { $$ = $1; }
     ;

mulop: TO_MUL { $$ = $1; }
     | TO_DIV { $$ = $1; }
     ;

atom: TL_REAL { $$ = new Real(stod(*$1)); }
    | TL_IDENT { $$ = new Symbol(*$1); }
    | TS_POPEN expr TS_PCLOSE { $$ = new ExprAtom($2); }
    | funcall { $$ = new FuncallAtom($1); }
    ;

funcall: TL_IDENT TS_POPEN stmts TS_PCLOSE { $$ = new Funcall(*$1, *$3); }

link: TL_IDENT TO_COLON link_elements TO_COLON TO_COLON
        { 
          auto *l = new Link(*$1); 
          for(NodeElement *v : *$3)
          {
            if(v->kind() == NodeElement::Kind::Variable)
            {
              l->vars.push_back(dynamic_cast<Variable*>(v)); 
            }
            if(v->kind() == NodeElement::Kind::Alias)
            {
              l->aliases.push_back(dynamic_cast<Alias*>(v));
            }
          }
          $$ = l;
        }
    ;

link_elements: link_element { $$ = new NodeElements(); 
                              $$->insert($$->end(), $1->begin(), $1->end()); }
             | link_elements link_element 
                            { $1->insert($1->end(), $2->begin(), $2->end()); }
             ;


link_element: var_decl_group TO_SEMI { $$ = $1; }
            | alias TO_SEMI { $$ = $1; }
            ;



%%
