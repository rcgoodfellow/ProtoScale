#ifndef _PS_ASTNODE_
#define _PS_ASTNODE_

#include <string>
#include <vector>

struct Module;
struct Element;
using Elements = std::vector<Element*>;
struct Expr;
struct Funcall;

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
  enum class Kind { Variable, Alias, Interlate };
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

struct Atom
{
  enum class Kind { Real, Symbol, Expr, Funcall};
  Atom(Kind k) : _kind{k} {}
  virtual ~Atom() {} 
  Kind kind() const { return _kind; }
  private:
    Kind _kind;
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
  ExprAtom(const Expr *v) : Atom{Kind::Symbol}, value{v} {}
};

struct FuncallAtom : public Atom
{
  const Funcall *value;
  FuncallAtom(const Funcall *f) 
    : Atom{Kind::Funcall}, value{f} {}
};

struct Factor
{
  Atom *atom, *exp;
  Factor(Atom *a, Atom *e=nullptr) : atom{a}, exp{e} {}
};
using Factors = std::vector<Factor*>;

struct Term 
{ 
  Factor *l, *r;
  int op;
  Term(Factor *l, Factor *r=nullptr) : l{l}, r{r} {}
};
using Terms = std::vector<Term*>;

struct Expr 
{ 
  Term *l, *r;
  int op;
  Expr(Term *l, Term *r=nullptr) : l{l}, r{r} {}
};
using Exprs = std::vector<Expr*>;

struct Funcall
{
  std::string name;
  Exprs args;
  Funcall(std::string n, Exprs a) : name{n}, args{a} {}
};

struct Alias : public NodeElement
{
  std::string name;
  Expr *expr;
  Alias(std::string n, Expr *e) 
    : NodeElement{Kind::Alias}, name{n}, expr{e} {}
};
using Aliases = std::vector<Alias*>;

struct Eqtn
{
  std::string tgt;
  int linkop;
  Expr *expr;
  Eqtn(std::string t, int l, Expr *e) : tgt{t}, linkop{l}, expr{e} {}
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
  Node(std::string n) : Element{Kind::Node}, name{n} {}

};

struct Link : public Element
{
  std::string name;
  Link(std::string n) : Element{Kind::Link}, name{n} {}
};

#endif
