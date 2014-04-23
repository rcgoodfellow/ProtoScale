#include "MetaSema.hpp"

using namespace ps::meta;

using std::string;
using std::vector;

void
Sema::check(const Module* m)
{
  using K = Element::Kind;
  for(Element *e: m->elements)
  {
    switch(e->kind())
    {
      case K::Node: check(dynamic_cast<const Node*>(e)); break;
      //case K::Link: check(dynamic_cast<const Link*>(e)); break;
    }
  }
}

void
Sema::check(const Node* n)
{
  checkFor_DuplicateVarNames(n->vars);
}

void
Sema::checkFor_DuplicateVarNames(const Variables &v)
{
  auto vc = v;
  std::sort(vc.begin(), vc.end(), 
      [](Variable *a, Variable *b) { return a->name < b->name; });

  auto duplicate =
    std::adjacent_find(vc.begin(), vc.end(),
        [](Variable *a, Variable *b) { return a->name == b->name; });

  if(duplicate != vc.end())
  {
    throw std::runtime_error{
      "duplicate variable name " + (*duplicate)->name
    };
  }

}
