#ifndef _PS_ASTNODE_
#define _PS_ASTNODE_

#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <algorithm>

namespace ps { namespace meta {

struct Module;
struct Element;
using Elements = std::vector<Element*>;
struct Funcall;
struct AddOp;
struct Expr;
using Exprs = std::vector<Expr*>;
struct Node;
struct Link;
using Nodes = std::vector<Node*>;
using Links = std::vector<Link*>;

struct Lexeme
{
  Lexeme(size_t l) : _line_no{l} {}
  size_t line_no() const { return _line_no; }
  
  private:
    size_t _line_no;
};


struct Module : public Lexeme
{
  std::string name;
  Nodes nodes;
  Links links;
  Module(std::string n, size_t line) : Lexeme{line}, name{n} {}

  Node* getNode(const std::string&) const;
  Link* getLink(const std::string&) const;
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
  enum class Kind { Variable, LazyVar, Alias, DiffRel, Interlate };
  NodeElement(Kind k) : _kind{k} {}
  virtual ~NodeElement() {}
  Kind kind() { return _kind; }
  private:
    Kind _kind;
};
using NodeElements = std::vector<NodeElement*>;


struct Variable : public NodeElement, public Lexeme
{ 
  std::string name; 
  std::string type; 
  Variable(std::string n, std::string t, size_t line_no) 
    : NodeElement{Kind::Variable}, Lexeme{line_no}, name{n}, type{t} {}
};
using Variables = std::vector<Variable*>;

struct Accessor : public Lexeme
{
  std::string name, target;
  Accessor(std::string n, std::string t, size_t line_no)
    : Lexeme{line_no}, name{n}, target{t} {}
};
using Accessors = std::vector<Accessor*>;

struct Alias : public NodeElement, public Lexeme
{
  std::string name;
  const Accessor *accessor;
  Alias(std::string n, const Accessor *a, size_t line_no)
    : Lexeme{line_no}, NodeElement{Kind::Alias}, name{n}, accessor{a} {}
};
using Aliases = std::vector<Alias*>;

struct LazyVar : public NodeElement, public Lexeme
{
  std::string name;
  const Expr *expr;
  LazyVar(std::string n, const Expr *e, size_t line_no)
    : Lexeme{line_no}, NodeElement{Kind::LazyVar}, name{n}, expr{e} {}
};
using LazyVars = std::vector<LazyVar*>;

struct DiffRel : public NodeElement, public Lexeme
{
  std::string tgt;
  Expr *expr;
  std::string time_unit;
  DiffRel(std::string t, Expr *e, std::string i, size_t line_no)
    : Lexeme{line_no}, NodeElement{Kind::DiffRel}, tgt{t}, expr{e}, time_unit{i} {}
};
using DiffRels = std::vector<DiffRel*>;

struct Eqtn : public Lexeme
{
  std::string tgt;
  bool differential;
  std::string time_unit;
  int linkop;
  Expr *expr;
  Eqtn(std::string t, int l, Expr *e, size_t line_no, bool d=false, 
       std::string i="") 
    : Lexeme{line_no}, tgt{t}, linkop{l}, expr{e}, differential{d}, time_unit{i} {}
};
using Eqtns = std::vector<Eqtn*>;

struct Interlate : public NodeElement, public Lexeme
{
  std::string name;
  Variables params;
  Eqtns eqtns;
  Interlate(std::string n, size_t line_no) 
    : Lexeme{line_no}, NodeElement{Kind::Interlate}, name{n} {}
};
using Interlates = std::vector<Interlate*>;

struct Node : public Element, public Lexeme
{
  std::string name;
  Variables vars;
  Aliases aliases;
  LazyVars lazy_vars;
  Interlates interlates;
  DiffRels diffrels;
  Node(std::string n, size_t line_no) 
    : Lexeme{line_no}, Element{Kind::Node}, name{n} {}

  Variable* getVar(const std::string &s) const;
  Alias* getAlias(const std::string &s) const;

};

struct Link : public Element, public Lexeme
{
  std::string name;
  Variables vars;
  Aliases aliases;
  Link(std::string n, size_t line_no) 
    : Lexeme{line_no}, Element{Kind::Link}, name{n} {}
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

struct AddOp : public Expr, public Lexeme
{
  struct Expr *l, *r;
  int op;
  AddOp(Expr *l, size_t line_no, Expr *r=nullptr, int o=0, Kind k=Kind::AddOp) 
    : Lexeme{line_no}, Expr{k}, l{l}, r{r}, op{o} {}
};

struct MulOp : public AddOp
{
  MulOp(Expr *l, size_t line_no, Expr *r=nullptr, int o=0, Kind k=Kind::MulOp) 
    : AddOp{l, line_no, r, o, k} {}
};

struct ExpOp : public MulOp
{
  ExpOp(Expr *l, size_t line_no, Expr *r=nullptr, int o=0) 
    : MulOp{l, line_no, r, o, Kind::ExpOp} {}
};

struct Atom : public Expr
{
  Atom(Kind k) : Expr{k} {}
  virtual ~Atom() {} 
};
using Atoms = std::vector<Atom*>;

struct Real : public Atom, public Lexeme
{
  double value;
  Real(double v, size_t line_no) 
    : Lexeme{line_no}, Atom{Kind::Real}, value{v} {}
};

struct Symbol : public Atom, public Lexeme
{
  std::string value;
  Symbol(std::string v, size_t line_no) 
    : Lexeme{line_no}, Atom{Kind::Symbol}, value{v} {}
};

struct ExprAtom : public Atom, public Lexeme
{
  const Expr *value;
  ExprAtom(const Expr *v, size_t line_no) 
    : Lexeme{line_no}, Atom{Kind::ExprAtom}, value{v} {}
};

struct FuncallAtom : public Atom, public Lexeme
{
  const Funcall *value;
  FuncallAtom(const Funcall *f, size_t line_no) 
    : Lexeme{line_no}, Atom{Kind::Funcall}, value{f} {}
};

struct Funcall : public Lexeme
{
  std::string name;
  Exprs args;
  Funcall(std::string n, Exprs a, size_t line_no) 
    : Lexeme{line_no}, name{n}, args{a} {}
};

}} //ps::meta

#endif
