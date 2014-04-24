#include "MetaAST.hpp"

using namespace ps;
using namespace ps::meta;

using std::string;
using std::vector;

Node::Node(string n, size_t line_no)
  : Lexeme{line_no}, Element{Kind::Node}, name{n}
{
  //All elements have an implicit time variable
  vars.push_back(new Variable("t", "time", 0));
  aliases.push_back(new Alias{"t", new Accessor{"time", "t", 0}, 0});
  lazy_vars.push_back(new LazyVar{"t", new Symbol{"t", 0}, 0});
}

Variable*
Node::getVar(const std::string &s) const
{
  auto i = 
    std::find_if(vars.begin(), vars.end(), 
        [&s](const Variable *v){ return v->name == s; });
  
  return i == vars.end() ? nullptr : *i;
}

Alias*
Node::getAlias(const std::string &s) const
{
  auto i = 
    std::find_if(aliases.begin(), aliases.end(),
        [&s](const Alias *a){ return a->name == s; });

  return i == aliases.end() ? nullptr : *i;
}

LazyVar*
Node::getLazyVar(const std::string &s) const
{
  auto i = 
    std::find_if(lazy_vars.begin(), lazy_vars.end(),
        [&s](const LazyVar *a){ return a->name == s; });

  return i == lazy_vars.end() ? nullptr : *i;
}

bool
Node::hasSymbol(const std::string &s) const
{
  Variable *v = getVar(s);
  Alias *a = getAlias(s);
  LazyVar *l = getLazyVar(s);
  return v || a || l;
}

Variable*
Link::getVar(const std::string &s) const
{
  auto i = 
    std::find_if(vars.begin(), vars.end(), 
        [&s](const Variable *v){ return v->name == s; });
  
  return i == vars.end() ? nullptr : *i;
}

Alias*
Link::getAlias(const std::string &s) const
{
  auto i = 
    std::find_if(aliases.begin(), aliases.end(),
        [&s](const Alias *a){ return a->name == s; });

  return i == aliases.end() ? nullptr : *i;
}

bool
Link::hasSymbol(const std::string &s) const
{
  Variable *v = getVar(s);
  Alias *a = getAlias(s);
  return v || a;
}

Node*
Module::getNode(const std::string &s) const
{
  auto i = 
    std::find_if(nodes.begin(), nodes.end(),
        [&s](Node *n){ return n->name == s; });

  return i == nodes.end() ? nullptr : *i;
}

Link*
Module::getLink(const std::string &s) const
{
  auto i = 
    std::find_if(links.begin(), links.end(),
        [&s](Link *l){ return l->name == s; });
  
  return i == links.end() ? nullptr : *i;

}
