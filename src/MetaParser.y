%{
  #include "MetaAST.hpp"
  Module *mm;
  extern int yylex();
%}

%locations
%error-verbose

%union {
  Module *module;
  Decls *decls;
  ASTNode *node;
  Decl *decl;
  Expr *expr;
  Stmt *stmt;
  Stmts *stmts;
  Ident *ident;
  Idents *idents;
  IdentQual *identqual;
  IdentQuals *identquals;
  Array *array;
  EqOp *eqop;
  EqStmt *eqstmt;
  EqStmts *eqstmts;
  EqReal *eqreal;
  EqSym *eqsym;
  FuncSig *funcsig;
  Assignment *assignment;
  Assignments *assignments;
  NodeDecl *nodedecl;
  LinkDecl *linkdecl;
  NodeDecls *nodedecls;
  std::string *string;
  int token;
}

%{
  void yyerror(const char *s) 
  { 
    printf("ERROR[%d]: %s\n", yylloc.first_line, s); 
  }
%}

%token <string> TIDENT TREAL TINT
%token <token> TI NR TKMODULE TKNODE TKLINK TKINFO TASSIGN TSEMI TQUAL TPLEQ
%token <token> TEQ TPLUS TMINUS TDIV TMUL TPOW TARROPEN TARRCLOSE TCOMMA TPOPEN 
%token <token> TPCLOSE TALIAS TINTERLATE TDOT

%type <module> metamodel module
%type <decls> decls
%type <stmts> syms eqtns eq_stmts
%type <ident> ident
%type <identqual> ident_qual
%type <identquals> ident_quals
%type <array> sym_array
%type <eqstmt> eqtn eq_stmt eq_term eq_fac eq_pgroup eq_atom eq_real eq_sym eq_funcall
%type <funcsig> func_sig
%type <assignment> info alias interlate
%type <assignments> aliases interlates maybe_aliases
%type <nodedecl> node_decl
%type <linkdecl> link_decl

%left TPLUS TMINUS
%left TMUL TDIV

%start metamodel

%%

metamodel : module decls { mm = $1, $1->decls = $2; }
          ;

module : TKMODULE ident { $$ = new Module($2); }
       ;

decls : TKNODE node_decl { $$ = new Decls(); $$->push_back($2); }
      | TKLINK link_decl { $$ = new Decls(); $$->push_back($2); }
      | decls TKNODE node_decl { $1->push_back($3); }
      | decls TKLINK link_decl { $1->push_back($3); }
      ;

node_decl : ident TSEMI
            TKINFO TSEMI info
            maybe_aliases
            TINTERLATE TSEMI interlates
            { $$ = new NodeDecl($1, $5, $6, $9); }
          ;

maybe_aliases : { $$ = nullptr; }
              | TALIAS TSEMI aliases { $$ = $3; }
              ;

ident : TIDENT { $$ = new Ident(*$<string>1); }
      ;

info : sym_array { $$ = new Assignment(new Ident("info"), $1); }
     ;

sym_array : syms { $$ = new Array($1); }
          ;

syms : ident { $$ = new Stmts(); $$->push_back($1); }
     | syms TCOMMA ident { $$ = new Stmts(); $$->push_back($3); }
     ;

aliases : alias { $$ = new Assignments(); $$->push_back($1); }
        | aliases TCOMMA alias { $1->push_back($3); }
        ;

alias : ident TEQ eq_stmt { $$ = new Assignment($1, $3); }

interlates : interlate { $$ = new Assignments(); $$->push_back($1); }
           | interlates interlate { $1->push_back($2); }
           ;

interlate : func_sig eqtns 
            { $$ =  new Assignment($1, new Array($2)); }
          ;

eqtns : eqtn { $$ = new Stmts(); $$->push_back($1); }
      | eqtns TCOMMA eqtn { $1->push_back($3); }

eqtn : eq_stmt TPLEQ eq_stmt { $1->op = new EqOp("+="); $1->r = $3; }

func_sig : ident TPOPEN ident_quals TPCLOSE TSEMI { $$ = new FuncSig($1, $3); }
         ;

ident_quals : ident_qual { $$ = new IdentQuals(); $$->push_back($1); }
            | ident_quals TCOMMA ident_qual { $1->push_back($3); }
            ;

ident_qual : ident TQUAL ident { $$ = new IdentQual($1, $3); }
           ;

eq_stmt : eq_term { $$ = $1; }
        | eq_stmt TPLUS eq_term  { $1->op = new EqOp("+"); $1->r = $3; }
        | eq_stmt TMINUS eq_term { $1->op = new EqOp("+"); $1->r = $3; }
        ;

eq_stmts : eq_stmt { $$ = new Stmts(); $$->push_back($1); }
         | eq_stmts TCOMMA eq_stmt { $1->push_back($3); }

eq_term : eq_fac { $$ = $1; }
        | eq_term TMUL eq_fac { $1->op = new EqOp("*"); $1->r = $3; }
        | eq_term TDIV eq_fac  { $1->op = new EqOp("/"); $1->r = $3; }
        ;

eq_pgroup : TPOPEN eq_stmt TPCLOSE { $$ = $2; }
          ;

eq_fac : eq_atom { $$ = $1; }
       | eq_atom TPOW eq_atom { $$->op = new EqOp("^"); $$->r = $3; }
       ;

eq_atom : eq_real { $$ = $1; }
        | eq_sym { $$ = $1; }
        | eq_pgroup { $$ = $1; }
        | eq_funcall { $$ = $1; }
        ;

eq_funcall : ident TPOPEN eq_stmts TPCLOSE { $$ = new EqFuncall($1, $3); }

eq_real : TREAL { $$ = new EqReal(stod(*$1)); }
        ;

eq_sym : TIDENT { $$ = new EqSym(*$1); }
       ;

link_decl : ident TSEMI
            TKINFO TSEMI info 
            { $$ = new LinkDecl($1, $5); }
          ;


%%
