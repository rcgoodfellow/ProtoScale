%{
  #include "MetaAST.hpp"
  #include <iostream>
  ps::meta::ModuleFragment *mm;
  ps::meta::DefinitionContext *ctx;
  extern int yylex();
  using namespace ps::meta;
  using std::vector;
  using std::string;
  #define CURRLINE metayylloc.first_line
%}

%locations
%error-verbose

%union {
  ps::meta::ASTNode            *astnode;
  ps::meta::ModuleFragment     *module;
  ps::meta::Definition         *definition;
  std::vector<ps::meta::Definition*> *definitions;
  ps::meta::Element            *element;
  std::vector<ps::meta::Element*> *elements;
  ps::meta::Node               *node;
  ps::meta::Link               *link; 
  ps::meta::Variable           *variable;
  ps::meta::Alias              *alias;
  ps::meta::Interlation        *interlation;
  std::vector<ps::meta::Interlation*> *interlations;
  ps::meta::Interlate          *interlate;
  ps::meta::LazyVariable       *lazyvariable;
  ps::meta::DifferentialEqtn   *differentialeqtn;

  
  ps::meta::Expression         *expression;
  std::vector<ps::meta::Expression*> *expressions;
  ps::meta::BinaryOperation    *binaryoperation;
  ps::meta::Sum                *sum;
  ps::meta::Product            *product;
  ps::meta::Power              *power;
  ps::meta::FunctionCall       *functioncall;
  std::vector<ps::meta::FunctionCall*> *functioncalls;
  ps::meta::Symbol             *symbol;
  ps::meta::RealLiteral        *realliteral;
  ps::meta::ComplexLiteral     *complexliteral;
  int                               token;
  std::string                       *string;
  std::vector<std::string>          *strings;
  bool                              boolean;
  ps::meta::Sum::Operator      sumop_t;
  ps::meta::Product::Operator  productop_t;
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
%token <token> TT_REAL TT_COMPLEX TT_STATIC
/* Operators */
%token <token> TO_COLON TO_ASSIGN TO_PLUS TO_MINUS TO_MUL TO_DIV TO_EQ TO_PLEQ 
%token <token> TO_SEMI TO_PRIME TO_GETS
%token <token> TO_MUEQ TO_POW TO_DOT TO_COMMA
/* Sepcial Symbols */
%token <token> TS_POPEN TS_PCLOSE TS_BOPEN TS_BCLOSE

%type <module> module
%type <element> node link
%type <elements> elements
%type <strings> var_names params
%type <string> typename
%type <definition> interlate diffrel
%type <definitions> node_definition node_definitions var_decl_group alias link_elements link_element lazy_var
%type <expression> expr sum product exponential atom
%type <expressions> stmts
%type <token> linkop
%type <sumop_t> addop
%type <productop_t> mulop
%type <functioncall> funcall
%type <functioncall> accessor
%type <functioncalls> accessors
%type <boolean> static
%type <interlation> interlation
%type <interlations> interlations

%left TO_PLUS TO_MINUS
%left TO_MUL TO_DIV

%start module

%%

module: TK_MODULE TL_IDENT TO_COLON elements TO_COLON TO_COLON 
        { mm = new ModuleFragment(*$2, $4, CURRLINE); }
      ;

elements: TK_NODE node {$$ = new vector<Element*>; $$->push_back($2); }
        | TK_LINK link {$$ = new vector<Element*>; $$->push_back($2); }
        | elements TK_NODE node { $1->push_back($3); }
        | elements TK_LINK link { $1->push_back($3); }
        ;

params: { $$ = new vector<string>; }
      | TS_BOPEN var_names TS_BCLOSE { $$ = $2; }
      ;

node: TL_IDENT params TO_COLON node_definitions TO_COLON TO_COLON 
      { auto *n = new Node(*$1, *$2, mm, CURRLINE); 
        for(Definition *d : *$4) { d->addTo(n); }
        ctx = n;
        $$ = n; }
    ;

node_definitions: node_definition 
                  { $$ = new vector<Definition*>; 
                    $$->insert($$->end(), $1->begin(), $1->end()); }
                | node_definitions node_definition
                  { $1->insert($1->end(), $2->begin(), $2->end()); }
                ;

node_definition: var_decl_group TO_SEMI { $$ = $1; }
               | alias TO_SEMI { $$ = $1; }
               | lazy_var TO_SEMI { $$ = $1; }
               | diffrel TO_SEMI 
                 { $$ = new vector<Definition*>; $$->push_back($1); }
               | interlate TO_COLON TO_COLON
                 { $$ = new vector<Definition*>; $$->push_back($1); }
               ;

diffrel: TL_IDENT TO_PRIME TO_DIV TL_IDENT TO_EQ expr 
          { $$ = new DiffRel(*$1, $6, new Symbol(*$4, ctx, CURRLINE), CURRLINE); }
       ;

var_decl_group: var_names static typename
                { $$ = new vector<Definition*>;
                  for(string v : *$1)
                  {
                    $$->push_back(new Variable(v, *$3, $2, CURRLINE));
                  } }
         ;

var_names: TL_IDENT { $$ = new vector<string>(); $$->push_back(*$1); }
         | var_names TO_COMMA TL_IDENT { $1->push_back(*$3); }
         ;

typename: TT_COMPLEX { $$ = new string("complex"); }
        | static TT_REAL { $$ = new string("real"); }
        | TL_IDENT { $$ = $1; }
        ;

static: { $$ = false; }
      | TT_STATIC { $$ = true; }
      ;

alias: var_names TO_ASSIGN accessors
       { $$ = new vector<Definition*>;  
         for(size_t i=0; i<$1->size(); ++i)
         {
            string s = (*$1)[i];
            FunctionCall *a = (*$3)[i];
            $$->push_back(new Alias(s, a, CURRLINE));
         } }
     ;

lazy_var: var_names TO_GETS stmts
          { $$ = new vector<Definition*>;
            for(size_t i=0; i<$1->size(); ++i)
            {
              string s = (*$1)[i];
              Expression *e = (*$3)[i];
              $$->push_back(new LazyVariable(s, e, CURRLINE));
            } }
        ;

accessor: TL_IDENT TS_POPEN TL_IDENT TS_PCLOSE 
          { vector<Expression*> args;
            args.push_back(new Symbol(*$3, ctx, CURRLINE));
            $$ = new FunctionCall(*$1, args, ctx, CURRLINE); }
        ;

accessors: accessor { $$ = new vector<FunctionCall*>; $$->push_back($1); }
         | accessors TO_COMMA accessor {$1->push_back($3); }
         ;

interlate: TL_IDENT TS_POPEN TL_IDENT TL_IDENT TO_COMMA TL_IDENT TL_IDENT 
           TS_PCLOSE TO_COLON interlations
           { Interlate *i = new Interlate(*$1, 
                                          dynamic_cast<Node*>(ctx), 
                                          CURRLINE); 
             i->link_param = new Variable(*$3, *$4, true, CURRLINE);
             i->node_param = new Variable(*$6, *$7, true, CURRLINE);
             i->body = *$10;
             ctx = i;
             $$ = i; }
         ;

interlations: interlation { $$ = new vector<Interlation*>; $$->push_back($1); }
            | interlations interlation { $1->push_back($2); }
            ;

interlation: TL_IDENT linkop expr TO_SEMI 
             { $$ = new Interlation(new Symbol(*$1, ctx, CURRLINE), $3); }
           ;

linkop: TO_PLEQ { $$ = $1; }
      | TO_MUEQ { $$ = $1; }
      | TO_EQ { $$ = $1; }
      ;

stmts: expr { $$ = new vector<Expression*>; $$->push_back($1); }
     | stmts TO_COMMA expr { $1->push_back($3); }
     ;

expr: sum
    ;

sum: product { $$ = $1; }
   | sum addop sum { $$ = new Sum($1, $3, $2, CURRLINE); }
   ;

product: exponential { $$ = $1; }
       | product mulop product 
         { $$ = new Product($1, $3, $2, CURRLINE); }
       ;

exponential: atom { $$ = $1; }
           | exponential TO_POW exponential 
              { $$ = new Power($1, $3, CURRLINE); }
           ;

addop: TO_PLUS { $$ = Sum::Operator::add; }
     | TO_MINUS { $$ = Sum::Operator::subtract; }
     ;

mulop: TO_MUL { $$ = Product::Operator::multiply; }
     | TO_DIV { $$ = Product::Operator::divide; }
     ;

atom: TL_REAL { $$ = new RealLiteral(stod(*$1), CURRLINE); }
    | TL_IDENT { $$ = new Symbol(*$1, ctx, CURRLINE); }
    | TS_POPEN expr TS_PCLOSE { $$ = $2; }
    | funcall { $$ = $1; }
    ;

funcall: TL_IDENT TS_POPEN stmts TS_PCLOSE 
         { $$ = new FunctionCall(*$1, *$3, ctx, CURRLINE); }

link: TL_IDENT params TO_COLON link_elements TO_COLON TO_COLON
        { auto *l = new Link(*$1, *$2, mm, CURRLINE); 
          for(Definition *d : *$4) { d->addTo(l); }
          ctx = l;
          $$ = l; }
    ;

link_elements: link_element { $$ = new vector<Definition*>; 
                              $$->insert($$->end(), $1->begin(), $1->end()); }
             | link_elements link_element 
                            { $1->insert($1->end(), $2->begin(), $2->end()); }
             ;


link_element: var_decl_group TO_SEMI { $$ = $1; }
            | alias TO_SEMI { $$ = $1; }
            | lazy_var TO_SEMI { $$ = $1; }
            ;

%%
