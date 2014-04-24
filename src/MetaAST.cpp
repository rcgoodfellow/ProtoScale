#include "MetaAST.hpp"

using namespace ps;
using namespace ps::meta;

const Variable*
Node::getVar(const std::string &s) const
{
  auto i = 
    std::find_if(vars.begin(), vars.end(), 
        [&s](const Variable *v){ return v->name == s; });
  
  return i == vars.end() ? nullptr : *i;
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
