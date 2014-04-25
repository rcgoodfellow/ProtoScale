#include "MetaASTPrinter.hpp"

using std::string;
using std::endl;

using namespace ps::meta;

string
ASTPrinter::print(const Module *m)
{
  using K = Element::Kind;

  print("[Module] " + m->name);
  indent++;

  for(Node *e : m->nodes) { print(as<const Node>(e)); }
  for(Link *e : m->links) { print(as<const Link>(e)); }

  indent--;

  return ss.str();
}

void
ASTPrinter::print(const Node *n)
{
  print("[Node] " + n->name);
  indent++;
  
  for(string *p : *n->params) { print("[Param] " + *p); }
  for(Variable *v : n->vars) { print(v); }
  for(Alias *a : n->aliases) { print(a); }
  for(LazyVar *v : n->lazy_vars) { print(v); }
  for(Interlate *i : n->interlates) { print(i); }
  for(DiffRel *d : n->diffrels) { print(d); }

  indent--;
}

void
ASTPrinter::print(const Link *l)
{
  print("[Link] " + l->name);
  indent++;

  for(string *p : *l->params) { print("[Param] " + *p); }
  for(Variable *v : l->vars) { print(v); }
  for(Alias *a : l->aliases) { print(a); }
  for(LazyVar *v : l->lazy_vars) { print(v); }

  indent--;
}

void
ASTPrinter::print(const Interlate *i)
{
  print("[Interlate] " + i->name);
  indent++;

  for(Variable *v : i->params) { print(v); }
  for(Eqtn *e : i->eqtns) { print(e); }

  indent--;
}

void
ASTPrinter::print(const Eqtn *e)
{
  std::string linkopstr{" ="};
  if(e->linkop == TO_PLEQ) { linkopstr = " +="; }
  if(e->linkop == TO_MUEQ) { linkopstr = " *="; }
  std::string diffstr{""};
  if(e->differential)
  {
    diffstr += "'(" + e->time_unit + ")";
  }
  print("[Eqtn] " + e->tgt + diffstr + linkopstr);
  indent++;

  print(e->expr);

  indent--;
}

void
ASTPrinter::print(const Alias *a)
{
  print("[Alias] " + a->name);
  indent++;

  print(a->accessor);

  indent--;
}

void
ASTPrinter::print(const LazyVar *l)
{
  print("[LazyVar] " + l->name);
  indent++;

  print(l->expr);

  indent--;
}

void
ASTPrinter::print(const Accessor *a)
{
  print("[Accessor] " + a->name + " tgt = " + a->target);
}

void
ASTPrinter::print(const DiffRel *d)
{
  print("[DiffRel] " + d->tgt + " (" + d->time_unit + ")");
  indent++;

  print(d->expr);

  indent--;
}

void
ASTPrinter::print(const Expr *e)
{
  using K = Expr::Kind;
  switch(e->kind())
  {
    case K::AddOp: print(as<const AddOp>(e)); break;
    case K::MulOp: print(as<const MulOp>(e)); break;
    case K::ExpOp: print(as<const ExpOp>(e)); break;
    case K::Real: print(as<const Real>(e)); break;
    case K::Symbol: print(as<const Symbol>(e)); break;
    case K::Funcall: print(as<const FuncallAtom>(e)); break;
    case K::ExprAtom: print(as<const ExprAtom>(e)); break;
    case K::Expr: break; //abstract node - nothing reasonable to print
    case K::Atom: break; // ""
  }
}

void
ASTPrinter::print(const AddOp *a)
{
  std::string opstr = "";
  if(a->op == TO_PLUS){ opstr = "+"; }
  if(a->op == TO_MINUS){ opstr = "-"; }
  print("["+opstr+"]");
  indent++;

  print(as<const Expr>(a->l));
  print(as<const Expr>(a->r));

  indent--;

};

void
ASTPrinter::print(const MulOp *m)
{
  std::string opstr = "";
  if(m->op == TO_MUL){ opstr = "*"; }
  if(m->op == TO_DIV){ opstr = "/"; }
  print("["+opstr+"]");
  indent++;

  print(as<const Expr>(m->l));
  print(as<const Expr>(m->r));

  indent--;
}

void
ASTPrinter::print(const ExpOp *e)
{
  print("[^]");
  indent++;

  print(as<const Expr>(e->l));
  print(as<const Expr>(e->r));

  indent--;
}


void
ASTPrinter::print(const Atom *a)
{
  // Thus far it is not really usefull to print this in the AST printout
  // as it does not add any additional information
  //print("[Atom] ");
  //indent++;

  using K = Atom::Kind;
  switch(a->kind())
  {
    case K::Real: print(as<const Real>(a)); break;
    case K::Symbol: print(as<const Symbol>(a)); break;
    case K::Funcall: print(as<const FuncallAtom>(a)); break;
    case K::ExprAtom: print(as<const ExprAtom>(a)); break;
    //Becase *a is an atom, we only need to worry about atom subtypes
    default: break;
  }

  //indent--;
}

void
ASTPrinter::print(const Real *r)
{
  print("[Real] " + std::to_string(r->value));
}

void
ASTPrinter::print(const Symbol *s)
{
  print("[Symbol] " + s->value);
}

void
ASTPrinter::print(const ExprAtom *s)
{
  print(s->value);
}

void
ASTPrinter::print(const FuncallAtom *f)
{
  print(f->value);
}

void
ASTPrinter::print(const Funcall *f)
{
  print("[Funcall] " + f->name);
  indent++;
  
  for(Expr *e : f->args)
  {
    print(e);
  }

  indent--;
}

void 
ASTPrinter::print(const Variable *v)
{
  print("[Variable] " + v->name + " " + v->type);
}

void 
ASTPrinter::print(string s)
{
  ss << string(indent.pos(), ' ') << s << endl;
}

