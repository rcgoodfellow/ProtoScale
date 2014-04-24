#include "MetaAST.hpp"

using namespace ps;
using namespace ps::meta;

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

bool
Node::hasSymbol(const std::string &s) const
{
  Variable *v = getVar(s);
  Alias *a = getAlias(s);
  return v || a;
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
