#ifndef _PS_ASTNODE_
#define _PS_ASTNODE_

#include <string>
#include <vector>

struct Module;
struct Element;
using Elements = std::vector<Element*>;
struct Funcall;
struct AddOp;
struct Expr;
using Exprs = std::vector<Expr*>;

struct Module
{
  std::string name;
  Elements elements; 
  Module(std::string n, Elements e) : name{n}, elements{e} {}
};

struct Element
{
  enum class Kind { Node, Link };
  Element(Kind k) : _kind{k} {}
  virtual ~Element() {}
  Kind kind() { return _kind; }
  private:
    Kind _kind;
};

struct NodeElement 
{ 
  enum class Kind { Variable, Alias, DiffRel, Interlate };
  NodeElement(Kind k) : _kind{k} {}
  virtual ~NodeElement() {}
  Kind kind() { return _kind; }
  private:
    Kind _kind;
};
using NodeElements = std::vector<NodeElement*>;

struct Variable : public NodeElement
{ 
  std::string name; 
  std::string type; 
  Variable(std::string n, std::string t) 
    : NodeElement{Kind::Variable}, name{n}, type{t} {}
};
using Variables = std::vector<Variable*>;

struct Alias : public NodeElement
{
  std::string name;
  Expr *expr;
  int oper;
  Alias(std::string n, Expr *e, int o) 
    : NodeElement{Kind::Alias}, name{n}, expr{e}, oper{o} {}
};
using Aliases = std::vector<Alias*>;

struct DiffRel : public NodeElement
{
  std::string tgt;
  Expr *expr;
  std::string time_unit;
  DiffRel(std::string t, Expr *e, std::string i)
    : NodeElement{Kind::DiffRel}, tgt{t}, expr{e}, time_unit{i} {}
};
using DiffRels = std::vector<DiffRel*>;

struct Eqtn
{
  std::string tgt;
  bool differential;
  std::string time_unit;
  int linkop;
  Expr *expr;
  Eqtn(std::string t, int l, Expr *e, bool d=false, std::string i="") 
    : tgt{t}, linkop{l}, expr{e}, differential{d}, time_unit{i} {}
};
using Eqtns = std::vector<Eqtn*>;

struct Interlate : public NodeElement
{
  std::string name;
  Variables params;
  Eqtns eqtns;
  Interlate(std::string n) 
    : NodeElement{Kind::Interlate}, name{n} {}
};
using Interlates = std::vector<Interlate*>;

struct Node : public Element
{
  std::string name;
  Variables vars;
  Aliases aliases;
  Interlates interlates;
  DiffRels diffrels;
  Node(std::string n) : Element{Kind::Node}, name{n} {}

};

struct Link : public Element
{
  std::string name;
  Variables vars;
  Aliases aliases;
  Link(std::string n) : Element{Kind::Link}, name{n} {}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Expr 
{
  enum class Kind 
  { 
    Expr, 
    AddOp, 
    MulOp, 
    ExpOp, 
    Atom, 
    Real, 
    Symbol,
    Funcall,
    ExprAtom
  };

  Expr(Kind k) : _kind{k} {}
  virtual ~Expr() {}

  Kind kind() const { return _kind; }
  private:
    Kind _kind;
};

struct AddOp : public Expr
{
  struct Expr *l, *r;
  int op;
  AddOp(Expr *l, Expr *r=nullptr, int o=0, Kind k=Kind::AddOp) 
    : Expr{k}, l{l}, r{r}, op{o} {}
};

struct MulOp : public AddOp
{
  MulOp(Expr *l, Expr *r=nullptr, int o=0, Kind k=Kind::MulOp) 
    : AddOp{l, r, o, k} {}
};

struct ExpOp : public MulOp
{
  ExpOp(Expr *l, Expr *r=nullptr, int o=0) 
    : MulOp{l, r, o, Kind::ExpOp} {}
};

struct Atom : public Expr
{
  Atom(Kind k) : Expr{k} {}
  virtual ~Atom() {} 
};
using Atoms = std::vector<Atom*>;

struct Real : public Atom
{
  double value;
  Real(double v) : Atom{Kind::Real}, value{v} {}
};

struct Symbol : public Atom
{
  std::string value;
  Symbol(std::string v) : Atom{Kind::Symbol}, value{v} {}
};

struct ExprAtom : public Atom
{
  const Expr *value;
  ExprAtom(const Expr *v) : Atom{Kind::ExprAtom}, value{v} {}
};

struct FuncallAtom : public Atom
{
  const Funcall *value;
  FuncallAtom(const Funcall *f) 
    : Atom{Kind::Funcall}, value{f} {}
};

struct Funcall
{
  std::string name;
  Exprs args;
  Funcall(std::string n, Exprs a) : name{n}, args{a} {}
};

#endif
