#ifndef _PS_ASTNODE_
#define _PS_ASTNODE_

#include <string>
#include <vector>

class ASTNode
{
  public:
    enum class Kind
    {
      Decl,
      Expr,
      Stmt,
      Ident,
      Module,
      IdentQual,
      EqOp,
      EqStmt,
      EqReal,
      EqSym,
      EqFuncall,
      Eqtn,
      Array,
      FuncSig,
      Assignment,
      NodeDecl,
      LinkDecl
    };

    ASTNode(Kind k) : k{k} {}
    virtual ~ASTNode() {}

    Kind kind() { return k; }
  private:
    Kind k;
};

struct Decl : public ASTNode 
{ 
  Decl(Kind k) : ASTNode{k} {}
};
using Decls = std::vector<Decl*>;

struct Expr : public ASTNode 
{ 
  Expr(Kind k) : ASTNode{k} {}
};
using Exprs = std::vector<Expr*>;

struct Stmt : public ASTNode 
{ 
  Stmt(Kind k) : ASTNode{k} {}
};
using Stmts = std::vector<Stmt*>;

struct Ident : public Stmt
{
  std::string name;
  Ident(const std::string name) : Stmt{Kind::Ident}, name{name} {}
  Ident(const Ident *id, Kind k) : Stmt{k}, name{id->name} {}
};
using Idents = std::vector<Ident*>;

struct Module : public Stmt
{
  const Ident *id;
  Decls *decls;
  Module(const Ident *id) : Stmt{Kind::Module}, id{id} {}
};

struct IdentQual : public Ident
{
  const Ident *type;
  IdentQual(const Ident *id, const Ident *type) 
    : Ident{id, Kind::IdentQual}, type{type} {}
};
using IdentQuals = std::vector<IdentQual*>;

struct Array : public Stmt
{
  const std::vector<Stmt *> *elems;
  Array(const std::vector<Stmt *> *v) : Stmt{Kind::Array}, elems{v} {}
};

struct EqOp : public ASTNode
{
  std::string value;
  EqOp(std::string v) : ASTNode{Kind::EqOp}, value{v} {}
};

struct EqStmt : public Stmt
{
  const EqStmt *l, *r;
  const EqOp *op;
  EqStmt(const EqStmt *l, const EqStmt *r, const EqOp *op, Kind k=Kind::EqStmt) 
    : Stmt{k}, l{l}, r{r}, op(op) 
  {}
};

struct EqReal : public EqStmt
{
  double v;
  EqReal(double v) : EqStmt{nullptr, nullptr, nullptr, Kind::EqReal}, v{v} {}
};

struct EqSym : public EqStmt
{
  std::string v;
  EqSym(std::string v) : EqStmt{nullptr, nullptr, nullptr, Kind::EqSym}, v{v} {}
};

struct EqFuncall : public EqStmt
{
  const Ident *id;
  const Stmts *args;
  EqFuncall(const Ident *id, const Stmts *args)
    : EqStmt{nullptr, nullptr, nullptr, Kind::EqFuncall}, id{id}, args{args} {}
};

//struct EqTerm : public EqStmt { };
//struct EqFac : public EqTerm { };
//struct EqExp : public EqFac { };
//struct EqReal : public EqExp { };

using EqStmts = std::vector<EqStmt*>;

struct Eqtn : public Stmt
{
  const EqStmt *l, *r; //top level operator always `=`
  Eqtn(const EqStmt *l, const EqStmt *r) : Stmt{Kind::Eqtn}, l{l}, r{r} {}
};

struct FuncSig : public Stmt
{
  const Ident *id;
  const std::vector<IdentQual*> *params;
  FuncSig(const Ident *id, const std::vector<IdentQual*> *params) 
    : Stmt{Kind::FuncSig}, id{id}, params{params} {}
};

struct Assignment : public Expr
{
  const Stmt *l;
  const Stmt *r;
  Assignment(const Stmt *l, const Stmt *r) : Expr{Kind::Assignment}, l{l}, r{r} {}
};

using Assignments = std::vector<Assignment*>;

struct NodeDecl : public Decl
{
  Ident *id;
  const Assignment *info;
  const Assignments *aliases;
  const Assignments *interlates;

  NodeDecl(Ident *id, Assignment *info, Assignments *aliases, 
          Assignments *interlates) 
    : Decl{Kind::NodeDecl}, id{id}, info{info}, aliases{aliases}, 
      interlates{interlates} 
  {}
};
using NodeDecls = std::vector<NodeDecl*>;

struct LinkDecl : public Decl
{
  Ident *id;
  const Assignment *info;
  LinkDecl(Ident *id, const Assignment *info) 
    : Decl{Kind::LinkDecl}, id{id}, info{info} {}
};
using LinkDecls = std::vector<LinkDecl*>;


#endif
