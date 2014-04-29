#include "MetaAST.hpp"

using namespace ps;
using namespace ps::meta;

using std::string;
using std::vector;

Element::Element(Kind k, string n, vector<string*> *p)
  : _kind{k}, name{n}, params{p}
{
  //All elements have an implicit time variable
  lazy_vars.push_back(new LazyVar{"t", new Symbol{"t", 0}, 0});
}

Node::Node(string n, vector<string*> *p, size_t line_no)
  : Lexeme{line_no}, Element{Kind::Node, n, p} { }

Link::Link(string n, vector<string*> *p, size_t line_no)
  : Lexeme{line_no}, Element{Kind::Link, n, p} { }

Interlate*
Node::getInterlate(const string &name)
{
  auto i =
    std::find_if(interlates.begin(), interlates.end(),
        [&name](const Interlate *a){ return a->name == name; }
    );
  return i == interlates.end() ? nullptr : *i;
}

Variable*
Element::getVar(const std::string &s) const
{
  auto i = 
    std::find_if(vars.begin(), vars.end(), 
        [&s](const Variable *v){ return v->name == s; });
  
  return i == vars.end() ? nullptr : *i;
}

Alias*
Element::getAlias(const std::string &s) const
{
  auto i = 
    std::find_if(aliases.begin(), aliases.end(),
        [&s](const Alias *a){ return a->name == s; });

  return i == aliases.end() ? nullptr : *i;
}

LazyVar*
Element::getLazyVar(const std::string &s) const
{
  auto i = 
    std::find_if(lazy_vars.begin(), lazy_vars.end(),
        [&s](const LazyVar *a){ return a->name == s; });

  return i == lazy_vars.end() ? nullptr : *i;
}

bool
Element::hasSymbol(const std::string &s) const
{
  Variable *v = getVar(s);
  Alias *a = getAlias(s);
  LazyVar *l = getLazyVar(s);
  return v || a || l;
}

string
Element::symbolType(const std::string &s) const
{
  Variable *v = getVar(s);
  if(v){ return v->type; }
  Alias *a = getAlias(s);
  if(a){ return "real"; }
  //TODO a lazy var could be complex
  LazyVar *l = getLazyVar(s);
  if(l){ return "real"; }
  return "";
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
