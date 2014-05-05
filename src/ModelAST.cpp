#include "ModelAST.hpp"

using namespace ps::model;

Create*
ModelFragment::find(const std::string &name)
{
  for(Command *c : *commands)
  {
    Create *cr = dynamic_cast<Create*>(c);
    if(cr)
    { 
      for(CreateTarget *t : *(cr->tgts))
      {
        if(t->name == name) { return cr; }
      }
    }
  }
  return nullptr;
}

void
ModelFragment::accept(Visitor &v)
{
  v.visit(this);
  for(Command *c : *commands){ c->accept(v); }
  v.leave(this);
}

void
Import::accept(Visitor &v)
{
  v.visit(this);
  v.leave(this);
}

void
Create::accept(Visitor &v)
{
  v.visit(this);
  v.leave(this);
}

void
Connect::accept(Visitor &v)
{
  v.visit(this);
  v.leave(this);
}
