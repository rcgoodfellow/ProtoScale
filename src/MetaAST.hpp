#ifndef _PS_ASTNODE_
#define _PS_ASTNODE_

#include "Diagnostics.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <complex>


namespace ps { namespace meta {

struct Visitor;

struct ASTNode
{
  virtual ~ASTNode(){}
  virtual void accept(Visitor&) = 0;
};

struct Lexeme
{
  Lexeme(size_t line_number) : line_number{line_number} {}
  size_t line_number;
};

struct ModuleFragment;
struct Definition;
struct Element;
struct Node;
struct Link;
struct Variable;
struct Alias;
struct Interlate;
struct Interlation;
struct DiffRel;
struct LazyVariable;
struct DifferentialEqtn;

struct Expression;
struct BinaryOperation;
struct Sum;
struct Product;
struct Power;
struct FunctionCall;
struct Symbol;
struct RealLiteral;
struct ComplexLiteral;

struct Visitor
{
  virtual void visit(ModuleFragment*){}
  virtual void leave(ModuleFragment*){}
  virtual void visit(Definition*){} 
  virtual void leave(Definition*){}
  virtual void visit(Element*){} 
  //virtual void leave(Element*){}
  virtual void visit(Node*){}
  virtual void leave(Node*){}
  virtual void visit(Link*){}
  virtual void leave(Link*){};
  virtual void visit(Variable*){}
  virtual void leave(Variable*){};
  virtual void visit(Alias*){}
  virtual void leave(Alias*){};
  virtual void visit(Interlate*){}
  virtual void leave(Interlate*){};
  virtual void visit(Interlation*){}
  virtual void leave(Interlation*){};
  virtual void visit(LazyVariable*){}
  virtual void leave(LazyVariable*){};
  virtual void visit(DifferentialEqtn*){}
  virtual void leave(DifferentialEqtn*){};

  virtual void visit(Expression*){}
  virtual void leave(Expression*){};
  virtual void visit(BinaryOperation*){}
  virtual void leave(BinaryOperation*){};
  virtual void visit(Sum*){}
  virtual void leave(Sum*){};
  virtual void visit(Product*){}
  virtual void leave(Product*){};
  virtual void visit(Power*){}
  virtual void leave(Power*){};
  virtual void visit(FunctionCall*){}
  virtual void leave(FunctionCall*){};
  virtual void visit(Symbol*){}
  virtual void leave(Symbol*){};
  virtual void visit(RealLiteral*){}
  virtual void leave(RealLiteral*){};
  virtual void visit(ComplexLiteral*){}
  virtual void leave(ComplexLiteral*){};
};

template <class Derived>
struct ASTNodeT : public ASTNode
{
  using ASTNodeType = Derived;
};

struct Definition : public ASTNodeT<Definition>, public Lexeme
{
  std::string name, type{""};
  size_t arity;

  Definition() = delete;
  Definition(std::string name, size_t arity, size_t line_no)
    : Lexeme{line_no}, name{name}, arity{arity} {}

  virtual ~Definition(){}
  virtual void addTo(void *s) = 0;
};

template <class Derived, class Scope>
struct DefinitionT : public Definition
{
  using DefinitionType = Derived;
  Scope *owner;

  DefinitionT() = delete;
  DefinitionT(std::string name, size_t arity, size_t line_no)
    : Definition{name, arity, line_no} {}

  virtual void addTo(void *s) override 
  { 
    owner = reinterpret_cast<Scope*>(s);
    addTo(owner); 
  }
  virtual void addTo(Scope *s) = 0;
};

struct DefinitionContext
{
  DefinitionContext *parent;
  
  DefinitionContext(DefinitionContext *parent) : parent{parent} {}

  virtual std::unordered_map<std::string, Definition*> definitions() = 0;
  virtual Definition* definition(std::string) = 0;
};

struct ModuleFragment : 
  public DefinitionT<ModuleFragment, std::vector<ModuleFragment*>>, 
  public DefinitionContext
{
  std::unordered_map<std::string, Node*> nodes;
  std::unordered_map<std::string, Link*> links;

  ModuleFragment(std::string name, 
                 std::vector<Element*> *elements, 
                 size_t line_no);
  
  virtual std::unordered_map<std::string, Definition*> definitions() override;
  virtual Definition* definition(std::string) override;

  void accept(Visitor &v) override;

  void addTo(std::vector<ModuleFragment*> *modules) override
  {
    modules->push_back(this);
  }
};

struct Element : public DefinitionT<Element, ModuleFragment>, 
                 public DefinitionContext
{
  std::unordered_map<std::string, Variable*> variables;
  std::unordered_map<std::string, Alias*> aliases;
  std::unordered_map<std::string, LazyVariable*> lazyvars;
  std::vector<std::string> params;

  Element(std::string name, std::vector<std::string> params, 
          ModuleFragment *definition_context, size_t line_no);

  virtual std::unordered_map<std::string, Definition*> definitions() override;
  virtual Definition* definition(std::string) override;
  
  void accept(Visitor &v) override;
};

template <class Derived>
struct ElementT : public Element
{
  using ElementType = Derived;

  ElementT(std::string name, std::vector<std::string> params, 
           ModuleFragment *definition_context, size_t line_no)
    : Element{name, params, definition_context, line_no} {}

};

struct Node : public ElementT<Node>
{
  std::unordered_map<std::string, Interlate*> interlates;
  std::unordered_map<std::string, DiffRel*> diffrels;

  Node(std::string name, std::vector<std::string> params, 
       ModuleFragment *definition_context, size_t line_no)
    : ElementT{name, params, definition_context, line_no} {}

  void addTo(ModuleFragment *m) override 
  { 
    try{ m->nodes.at(name); }
    catch(std::out_of_range&)
    { 
      m->nodes[name] = this; 
      return;
    }
    //TODO: line_number is at the end of the node and not the beginning
    throw std::runtime_error{"Duplicate Node "+name+" at "
                              +std::to_string(line_number)};
  }
  
  void accept(Visitor &v) override;

  virtual std::unordered_map<std::string, Definition*> definitions() override;
  virtual Definition* definition(std::string) override;
};

struct Link : public ElementT<Link>
{
  Link(std::string name, std::vector<std::string> params, 
       ModuleFragment *definition_context, size_t line_no)
    : ElementT{name, params, definition_context, line_no} {}

  void addTo(ModuleFragment *m) override 
  { 
    try{ m->links.at(name); }
    catch(std::out_of_range&)
    {
      m->links[name] = this; 
      return;
    }
    throw std::runtime_error{"Duplicate Link "+name+" at "
                              +std::to_string(line_number)};
  }
  
  void accept(Visitor &v) override;
  virtual std::unordered_map<std::string, Definition*> definitions() override;
  virtual Definition* definition(std::string) override;
};

struct Variable : public DefinitionT<Variable, Element>
{
  bool dynamic{true};

  Variable(std::string name, std::string type, bool dynamic, size_t line_no)
    : DefinitionT{name, 0, line_no}, dynamic{dynamic} 
  { this->type = type; }

  void addTo(Element *e) override 
  { 
    try{ e->variables.at(name); }
    catch(std::out_of_range&)
    {
      e->variables[name] = this; 
      return;
    }
  
    std::string msg = "Duplicate variable "+name;
    throw compilation_error{Diagnostic{"", line_number, msg}};
  }
  
  void accept(Visitor &v) override;
};

struct Alias : public DefinitionT<Alias, Element>
{
  FunctionCall *accessor;
  bool dynamic{true};

  Alias(std::string name, FunctionCall *accessor, size_t line_no)
    : DefinitionT{name, 0, line_no}, accessor{accessor} {}

  void addTo(Element *e) override 
  { 
    try{ e->aliases.at(name); }
    catch(std::out_of_range&)
    {
      e->aliases[name] = (this); 
      return;
    }
    throw std::runtime_error{"Duplicate Alias "+name+" at "
                              +std::to_string(line_number)};
  }
  
  void accept(Visitor &v) override;
};

struct LazyVariable : public DefinitionT<Alias, Element>
{
  Expression *expression;

  LazyVariable(std::string name, Expression *expression, size_t line_no)
    : DefinitionT{name, 0, line_no}, expression{expression} {}

  void addTo(Element *e) override 
  { 
    try{ e->lazyvars.at(name); }
    catch(std::out_of_range&)
    {
      e->lazyvars[name] = (this); 
      return;
    }
    throw std::runtime_error{"Duplicate Lazy Variable "+name+" at "
                               +std::to_string(line_number)};
  }
  
  void accept(Visitor &v) override;
};

struct Function : public DefinitionT<Function, ModuleFragment>
{
  std::vector<Variable*> parameters;
  std::vector<Expression*> body;

  Function(std::string name, size_t arity, 
           std::vector<Variable*> parameters,
           std::vector<Expression*> body,
           size_t line_no)
    : DefinitionT{name, arity, line_no}, parameters{parameters}, body{body} {}

  void addTo(ModuleFragment*) override 
  { throw std::runtime_error{"not implemented"}; }
  
  void accept(Visitor &v) override;
};

struct Interlation : public ASTNodeT<Interlation>
{
  Symbol *target;
  Expression *expression;

  Interlation(Symbol *target, Expression *expression)
    : target{target}, expression{expression} {}
  
  void accept(Visitor &v) override;
  bool dynamic();
  std::string type();
};

struct Interlate : public DefinitionT<Interlate, Node>, 
                   public DefinitionContext
{
  Variable *link_param, *node_param;
  std::vector<Interlation*> body;

  Interlate(std::string name, Node *definition_context, size_t line_no)
    : DefinitionT{name, 2, line_no}, DefinitionContext{definition_context} {}

  void addTo(Node *n) override 
  { 
    try{ n->interlates.at(name); }
    catch(std::out_of_range&)
    {
      n->interlates[name] = this; 
      return;
    }
    throw std::runtime_error{"Duplicate Interlate "+name+" at "
                               + std::to_string(line_number)};
  }
  
  virtual std::unordered_map<std::string, Definition*> definitions() override;
  virtual Definition* definition(std::string) override;
  
  void accept(Visitor &v) override;
};

struct DiffRel : public DefinitionT<DiffRel, Node>
{
  Symbol *time_unit;
  Variable *target;
  Expression *expression;

  DiffRel(std::string name, Expression *expression, Symbol *time_unit, 
          size_t line_no)
    : DefinitionT{name+"'", 0, line_no}, time_unit{time_unit},
      expression{expression} {}

  void addTo(Node *n) override 
  { 
    try{ n->diffrels.at(name); }
    catch(std::out_of_range&)
    {
      n->diffrels[name] = this;
      return;
    }
    throw std::runtime_error{"Duplicate Differential Relation Names"
                              +name+" at "+ std::to_string(line_number)};
  }
  
  void accept(Visitor &v) override;
};

//Expression Subtree ----------------------------------------------------------
struct Expression : public ASTNodeT<Expression>
{
  std::string type;
  bool dynamic{true};
};

template <class Derived>
struct ExpressionT : public Expression
{
  using ExpressionType = Derived;
};

struct BinaryOperation : public ExpressionT<BinaryOperation>
{
  Expression *lhs, *rhs;

  BinaryOperation(Expression *lhs, Expression *rhs)
    : lhs{lhs}, rhs{rhs} {}
  
  void accept(Visitor &v) override;
};

template <class Derived>
struct BinaryOperationT : public BinaryOperation
{
  using BinaryOperationType = Derived;

  using BinaryOperation::BinaryOperation;
};

struct Sum : public BinaryOperationT<Sum>, public Lexeme
{
  enum class Operator { add, subtract };
  Operator op;

  Sum() = delete;
  Sum(Expression *lhs, Expression *rhs, Operator op, size_t line_no)
    : BinaryOperationT{lhs, rhs}, Lexeme{line_no}, op{op} {}
  
  void accept(Visitor &v) override;
};

struct Product : public BinaryOperationT<Product>, public Lexeme
{
  enum class Operator { multiply, divide };
  Operator op;

  Product() = delete;
  Product(Expression *lhs, Expression *rhs, Operator op, size_t line_no)
    : BinaryOperationT{lhs, rhs}, Lexeme{line_no}, op{op} {}
  
  void accept(Visitor &v) override;

};

struct Power : public BinaryOperationT<Power>, public Lexeme
{
  Power() = delete;
  Power(Expression *lhs, Expression *rhs, size_t line_no)
    : BinaryOperationT{lhs, rhs}, Lexeme{line_no} {}
  
  void accept(Visitor &v) override;
};

struct Reference : public ExpressionT<Reference>
{
  DefinitionContext *definition_context;

  Reference(DefinitionContext *definition_context)
    : definition_context{definition_context} {}

  virtual ~Reference() {}
};

template <class Derived, class RefT>
struct ReferenceT : public Reference
{
  using ReferenceType = Derived;
  using TargetType = RefT;

  TargetType *definition;

  ReferenceT(DefinitionContext *definition_context)
    : Reference{definition_context} {}

};

struct FunctionCall : public ReferenceT<FunctionCall, Function>, public Lexeme
{
  std::string name;
  std::vector<Expression*> arguments;

  FunctionCall(std::string name, std::vector<Expression*> arguments,
               DefinitionContext *definition_context, size_t line_no)
    : ReferenceT{definition_context}, Lexeme{line_no}, name{name}, 
      arguments{arguments} {}
  
  void accept(Visitor &v) override;
};

struct Symbol : public ReferenceT<Symbol, Variable>, public Lexeme
{
  std::string name;
  Definition *definition;

  Symbol(std::string name, DefinitionContext *definition_context,
         size_t line_no)
    : ReferenceT{definition_context}, Lexeme{line_no}, name{name} {}
  
  void accept(Visitor &v) override;

  bool subscripted()
  {
    return name.find('.') != std::string::npos;
  }

  std::string location()
  {
    auto dot = name.find('.');
    return name.substr(dot+1, name.length());
  }

  std::string localName()
  {
    auto dot = name.find('.');
    return name.substr(0, dot);
  }
};

struct RealLiteral : public ExpressionT<Symbol>, public Lexeme
{
  double value;

  RealLiteral(double value, size_t line_no)
    : Lexeme{line_no}, value{value}
  {
    type = "real";
  }
  
  void accept(Visitor &v) override;
};

struct ComplexLiteral : public ExpressionT<Symbol>, public Lexeme
{
  std::complex<double> value;

  ComplexLiteral(std::complex<double> value, size_t line_no)
    : Lexeme{line_no}, value{value}
  {
    type = "complex";
  }
  
  void accept(Visitor &v) override;
};

}} //ps::meta

#endif
