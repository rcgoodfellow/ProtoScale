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
struct Symbol;

struct Lexeme
{
  Lexeme(size_t l) : _line_no{l} {}
  size_t line_no() const { return _line_no; }
  
  private:
    size_t _line_no;
};

struct NamedLexeme : public Lexeme
{
  std::string name;
  NamedLexeme(std::string n, size_t l) : Lexeme{l}, name{n} {}
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

//TODO: Perhaps get rid of this and just use TypedSymbol ?
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

struct TypedSymbol : public NodeElement, public NamedLexeme
{
  std::string type{};
  bool is_static{false};

  TypedSymbol(NodeElement::Kind k, std::string n, std::string t, bool is, 
              size_t line_no)
    : NodeElement{k}, NamedLexeme{n, line_no}, type{t}, is_static{is} {}

  TypedSymbol(NodeElement::Kind k, std::string n, std::string t, size_t l)
    : NodeElement{k}, NamedLexeme{n, l}, type{t} {}

  TypedSymbol(NodeElement::Kind k, std::string n, size_t line_no)
    : NodeElement{k}, NamedLexeme{n, line_no} {}

  virtual ~TypedSymbol() {}
};

struct Variable : TypedSymbol
{ 
  Variable(std::string n, std::string t, bool is, size_t line_no) 
    : TypedSymbol{Kind::Variable, n, t, is, line_no}
  {
    if(type.substr(0,6) == "static")
    {
      is_static = true;
      type = type.substr(7, t.length());
    }
  }
};
using Variables = std::vector<Variable*>;

struct Accessor : public Lexeme
{
  std::string name, target;
  Accessor(std::string n, std::string t, size_t line_no)
    : Lexeme{line_no}, name{n}, target{t} {}
};
using Accessors = std::vector<Accessor*>;

struct Alias : public TypedSymbol
{
  Accessor *accessor;
  Alias(std::string n, Accessor *a, size_t line_no)
    : TypedSymbol{Kind::Alias, n, "real", line_no}, accessor{a} {}
};
using Aliases = std::vector<Alias*>;

struct LazyVar : public TypedSymbol
{
  Expr *expr;
  LazyVar(std::string n, Expr *e, size_t line_no)
    : TypedSymbol{Kind::LazyVar, n, line_no}, expr{e} {}
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
  //TODO: tgt should be a symbol
  std::string tgt;
  TypedSymbol *ts{nullptr};
  bool differential;
  std::string time_unit;
  int linkop;
  Expr *expr;
  Eqtn(std::string t, int l, Expr *e, size_t line_no, bool d=false, 
       std::string i="") 
    : Lexeme{line_no}, tgt{t}, linkop{l}, expr{e}, differential{d}, time_unit{i} {}

  std::vector<meta::Symbol*> findDynamics();
};
using Eqtns = std::vector<Eqtn*>;

struct Interlate : public NodeElement, public NamedLexeme
{
  Variables params;
  Eqtns eqtns;
  Interlate(std::string n, size_t line_no) 
    : NamedLexeme{n, line_no}, NodeElement{Kind::Interlate} {}
};
using Interlates = std::vector<Interlate*>;

struct Element
{
  enum class Kind { Node, Link };
  Element(Kind k, std::string name, std::vector<std::string*> *p); 
  virtual ~Element() {}
  Kind kind() const { return _kind; }

  std::vector<std::string*> *params;
  std::string name;
  Variables vars;
  Aliases aliases;
  LazyVars lazy_vars;
  DiffRels diffrels;
  
  Variable* getVar(const std::string &s) const;
  Alias* getAlias(const std::string &s) const;
  LazyVar* getLazyVar(const std::string &s) const;
  TypedSymbol* getSymbol(const std::string &s);
  bool hasSymbol(const std::string &s) const;
  std::string symbolType(const std::string &s) const;
  std::vector<TypedSymbol*> dynamics() const;

  private:
    Kind _kind;
};

struct Node : public Element, public Lexeme
{
  Interlates interlates;
  Node(std::string n, std::vector<std::string*> *p, size_t line_no);
  Interlate* getInterlate(const std::string&);
};

struct Link : public Element, public Lexeme
{
  Link(std::string n, std::vector<std::string*> *p, size_t line_no);
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
  TypedSymbol *typed{nullptr};
  Symbol(std::string v, size_t line_no) 
    : Lexeme{line_no}, Atom{Kind::Symbol}, value{v} {}
};

struct ExprAtom : public Atom, public Lexeme
{
  Expr *value;
  ExprAtom(Expr *v, size_t line_no) 
    : Lexeme{line_no}, Atom{Kind::ExprAtom}, value{v} {}
};

struct FuncallAtom : public Atom, public Lexeme
{
  Funcall *value;
  FuncallAtom(Funcall *f, size_t line_no) 
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
