#include "MetaAST.hpp"

using namespace ps::meta;

using std::string;
using std::vector;
using std::pair;
using std::unordered_map;

ModuleFragment::ModuleFragment(
    std::string name, 
    std::vector<Element*> *elements, 
    size_t line_no)
  : DefinitionT{name, 0, line_no}, DefinitionContext{this} 
{
  for(Element *e : *elements) { e->addTo(this); }
}

unordered_map<string,Definition*>
ModuleFragment::definitions()
{
  unordered_map<string, Definition*> d;
  d[name] = this;
  d.insert(nodes.begin(), nodes.end());
  d.insert(links.begin(), links.end());
  return d;
}

Definition*
ModuleFragment::definition(string name)
{
  if(name == this->name){ return this; }

  auto ni = nodes.find(name);
  if(ni != nodes.end()){ return ni->second; }

  auto li = links.find(name);
  if(li != links.end()){ return li->second; }

  return nullptr;
}

Element::Element(string name, vector<string> params, 
                      ModuleFragment *definition_context, size_t line_no)
    : DefinitionT(name, params.size(), line_no),
      DefinitionContext{definition_context}, params{params}

{
  lazyvars["t"] = 
    new LazyVariable("t", new FunctionCall("time", {}, this, 0), 0);
}

unordered_map<string, Definition*>
Element::definitions()
{
  unordered_map<string, Definition*> d;
  d[name] = this;
  d.insert(variables.begin(), variables.end());
  d.insert(aliases.begin(), aliases.end());
  d.insert(lazyvars.begin(), lazyvars.end());
  d.insert(parent->definitions().begin(), parent->definitions().end());
  return d;
}

Definition*
Element::definition(string name)
{
  if(name == this->name){ return this; }

  auto iv = variables.find(name);
  if(iv != variables.end()){ return iv->second; }

  auto ia = aliases.find(name);
  if(ia != aliases.end()){ return ia->second; }

  auto il = lazyvars.find(name);
  if(il != lazyvars.end()){ return il->second; }

  return parent->definition(name);
}

Definition*
Node::definition(string name)
{
  Definition *d = Element::definition(name); 
  if(d){ return d; }

  auto id = diffrels.find(name);
  if(id != diffrels.end()){ return id->second; }
  
  auto il = interlates.find(name);
  if(il != interlates.end()){ return il->second; }

  return parent->definition(name);
}

Definition*
Link::definition(string name)
{
  return Element::definition(name);
}

unordered_map<string, Definition*>
Node::definitions()
{
  unordered_map<string, Definition*> d = Element::definitions();
  d.insert(diffrels.begin(), diffrels.end());
  return d;
}

unordered_map<string, Definition*>
Link::definitions()
{
  return Element::definitions();
}

unordered_map<string, Definition*>
Interlate::definitions()
{
  unordered_map<string, Definition*> d;
  d[link_param->name] = link_param;
  d[node_param->name] = node_param;
  d.insert(parent->definitions().begin(), parent->definitions().end());
  return d;
}

Definition*
Interlate::definition(string name)
{
  if(link_param->name == name){ return link_param; }
  if(node_param->name == name){ return node_param; }
  return parent->definition(name);
}

bool 
Interlation::dynamic(){ return target->dynamic || expression->dynamic; }

string 
Interlation::type(){ return target->type; }

void
ModuleFragment::accept(Visitor &v)
{
  v.visit(this);
  for(pair<string, Node*> p : nodes) { p.second->accept(v); }
  for(pair<string, Link*> p : links) { p.second->accept(v); }
  v.leave(this);
}

void 
Element::accept(Visitor &v)
{
  v.visit(this);
  for(pair<string, Variable*> p : variables) { p.second->accept(v); }
  for(pair<string, Alias*> p : aliases) { p.second->accept(v); }
  for(pair<string, LazyVariable*> p : lazyvars) { p.second->accept(v); }
  //v.leave(this);
}

void 
Node::accept(Visitor &v)
{
  v.visit(this);
  Element::accept(v);
  for(pair<string, Interlate*> p : interlates) { p.second->accept(v); }
  for(pair<string, DiffRel*> p : diffrels) { p.second->accept(v); }
  v.leave(this);
}

void 
Link::accept(Visitor &v)
{
  v.visit(this);
  Element::accept(v);
  v.leave(this);
}

void 
Variable::accept(Visitor &v)
{
  v.visit(this);
  
  v.leave(this);
}

void 
Alias::accept(Visitor &v)
{
  v.visit(this);
  accessor->accept(v);
  v.leave(this);
}

void 
LazyVariable::accept(Visitor &v)
{
  v.visit(this);
  expression->accept(v);
  v.leave(this);
}

void 
Function::accept(Visitor &v)
{
  v.visit(this);
  for(Variable *p : parameters){ p->accept(v); }
  for(Expression *e : body){ e->accept(v); }
  v.leave(this);
}

void 
Interlation::accept(Visitor &v)
{
  v.visit(this);
  target->accept(v);
  expression->accept(v);
  v.leave(this);
}

void 
Interlate::accept(Visitor &v)
{
  v.visit(this);
  link_param->accept(v);
  node_param->accept(v);
  for(Interlation *i : body) { i->accept(v); }
  v.leave(this);
}

void 
DiffRel::accept(Visitor &v)
{
  v.visit(this);
  time_unit->accept(v);
  expression->accept(v);
  v.leave(this);
}

void 
BinaryOperation::accept(Visitor &v)
{
  v.visit(this);
  lhs->accept(v);
  rhs->accept(v);
  v.leave(this);
}

void 
Sum::accept(Visitor &v)
{
  v.visit(this);
  BinaryOperation::accept(v);
  v.leave(this);
}

void 
Product::accept(Visitor &v)
{
  v.visit(this);
  BinaryOperation::accept(v);
  v.leave(this);
}

void 
Power::accept(Visitor &v)
{
  v.visit(this);
  BinaryOperation::accept(v);
  v.leave(this);
}

void 
FunctionCall::accept(Visitor &v)
{
  v.visit(this);
  for(Expression *e : arguments){ e->accept(v); }
  v.leave(this);
}

void 
Symbol::accept(Visitor &v)
{
  v.visit(this);

  v.leave(this);
}

void 
RealLiteral::accept(Visitor &v)
{
  v.visit(this);

  v.leave(this);
}

void 
ComplexLiteral::accept(Visitor &v)
{
  v.visit(this);

  v.leave(this);
}
