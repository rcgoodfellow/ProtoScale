%{
  #include "MetaAST.hpp"
  #include <iostream>
  ps::meta::Module *mm;
  extern int yylex();
  using namespace ps::meta;
  #define CURRLINE metayylloc.first_line
%}

%locations
%error-verbose

%union {
  ps::meta::Module *module;
  ps::meta::Element *element;
  ps::meta::Elements *elements;
  ps::meta::Expr *expr;
  ps::meta::Exprs *exprs;
  ps::meta::Variable *variable;
  ps::meta::Variables *variables;
  std::vector<std::string*> *strings;
  std::string *string;
  ps::meta::Node *node;
  ps::meta::Link *link;
  ps::meta::NodeElement *node_element;
  ps::meta::NodeElements *node_elements;
  ps::meta::Atom *atom;
  ps::meta::Atoms *atoms;
  ps::meta::Funcall *funcall;
  ps::meta::Eqtn *eqtn;
  ps::meta::Eqtns *eqtns;
  ps::meta::AddOp *addop;
  ps::meta::MulOp *mulop;
  ps::meta::ExpOp *expop;
  ps::meta::Accessor *accessor;
  ps::meta::Accessors *accessors;
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
%token <token> TO_COLON TO_ASSIGN TO_PLUS TO_MINUS TO_MUL TO_DIV TO_EQ TO_PLEQ TO_SEMI TO_PRIME TO_GETS
%token <token> TO_MUEQ TO_POW TO_DOT TO_COMMA
/* Sepcial Symbols */
%token <token> TS_POPEN TS_PCLOSE

%type <module> module
%type <element> node link
%type <elements> elements
%type <strings> var_names
%type <string> typename
%type <node_element> interlate diffrel
%type <node_elements> node_element node_elements var_decl_group var_decl_groups_cs alias link_elements link_element lazy_var
%type <expr> expr sum product exponential atom
%type <exprs> stmts
%type <token> addop mulop linkop
%type <funcall> funcall
%type <eqtns> eqtns
%type <eqtn> eqtn
%type <accessor> accessor
%type <accessors> accessors

%left TO_PLUS TO_MINUS
%left TO_MUL TO_DIV

%start module

%%

module: TK_MODULE TL_IDENT TO_COLON elements TO_COLON TO_COLON 
        { 
          mm = new Module(*$2, CURRLINE); 
          for(Element *e : *$4)
          {
            if(e->kind() == Element::Kind::Node)
            {
              mm->nodes.push_back(dynamic_cast<Node*>(e));
            }
            if(e->kind() == Element::Kind::Link)
            {
              mm->links.push_back(dynamic_cast<Link*>(e));
            }
          }
        }
      ;

elements: TK_NODE node {$$ = new Elements(); $$->push_back($2); }
        | TK_LINK link {$$ = new Elements(); $$->push_back($2); }
        | elements TK_NODE node { $1->push_back($3); }
        | elements TK_LINK link { $1->push_back($3); }
        ;

node: TL_IDENT TO_COLON node_elements TO_COLON TO_COLON 
                    { 
                      auto *n = new Node(*$1, CURRLINE); 
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
                        if(v->kind() == NodeElement::Kind::LazyVar)
                        {
                          n->lazy_vars.push_back(dynamic_cast<LazyVar*>(v));
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
            | lazy_var TO_SEMI { $$ = $1; }
            | diffrel TO_SEMI 
                { $$ = new NodeElements(); $$->push_back($1); }
            | interlate TO_COLON TO_COLON
                { $$ = new NodeElements(); $$->push_back($1); }
            ;

diffrel: TL_IDENT TO_PRIME TO_DIV TL_IDENT TO_EQ expr 
          { $$ = new DiffRel(*$1, $6, *$4, CURRLINE); }
       ;

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
                  $$->push_back(new Variable(*v, *$2, CURRLINE));
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

alias: var_names TO_ASSIGN accessors
       { 
         $$ = new NodeElements();  
         for(size_t i=0; i<$1->size(); ++i)
         {
            std::string s = *((*$1)[i]);
            Accessor *a = (*$3)[i];
            $$->push_back(new Alias(s, a, CURRLINE));
         }
       }
     ;

lazy_var: var_names TO_GETS stmts
          {
            $$ = new NodeElements();
            for(size_t i=0; i<$1->size(); ++i)
            {
              std::string s = *((*$1)[i]);
              Expr *e = (*$3)[i];
              $$->push_back(new LazyVar(s, e, CURRLINE));
            }
          }
        ;

accessor: TL_IDENT TS_POPEN TL_IDENT TS_PCLOSE 
          { $$ = new Accessor(*$1, *$3, CURRLINE); }
        ;

accessors: accessor { $$ = new Accessors(); $$->push_back($1); }
         | accessors TO_COMMA accessor {$1->push_back($3); }
         ;

interlate: TL_IDENT TS_POPEN var_decl_groups_cs TS_PCLOSE TO_COLON
           eqtns
           { Interlate *i = new Interlate(*$1, CURRLINE); 
             for(NodeElement *e : *$3)
             {
               i->params.push_back(dynamic_cast<Variable*>(e));
             }
             i->eqtns = *$6;
             $$ = i; 
           }
         ;

eqtn: TL_IDENT linkop expr TO_SEMI { $$ = new Eqtn(*$1, $2, $3, CURRLINE); }
    | TL_IDENT TO_PRIME TO_DIV TL_IDENT linkop expr TO_SEMI 
              { $$ = new Eqtn(*$1, $5, $6, CURRLINE, true, *$4); }
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
   | sum addop sum { $$ = new AddOp($<addop>1, CURRLINE, $<addop>3, $2); }
   ;

product: exponential { $$ = $1; }
       | product mulop product 
         { $$ = new MulOp($<mulop>1, CURRLINE, $<mulop>3, $2); }
       ;

exponential: atom { $$ = $1; }
           | exponential TO_POW exponential 
              { $$ = new ExpOp($<expop>1, CURRLINE, $<expop>3, TO_POW); }
           ;

addop: TO_PLUS { $$ = $1; }
     | TO_MINUS { $$ = $1; }
     ;

mulop: TO_MUL { $$ = $1; }
     | TO_DIV { $$ = $1; }
     ;

atom: TL_REAL { $$ = new Real(stod(*$1), CURRLINE); }
    | TL_IDENT { $$ = new Symbol(*$1, CURRLINE); }
    | TS_POPEN expr TS_PCLOSE { $$ = new ExprAtom($2, CURRLINE); }
    | funcall { $$ = new FuncallAtom($1, CURRLINE); }
    ;

funcall: TL_IDENT TS_POPEN stmts TS_PCLOSE { $$ = new Funcall(*$1, *$3, CURRLINE); }

link: TL_IDENT TO_COLON link_elements TO_COLON TO_COLON
        { 
          auto *l = new Link(*$1, CURRLINE); 
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
