#include "MetaASTPrinter.hpp"

using std::string;
using std::endl;

string
MetaASTPrinter::print(const Module *m)
{
  using K = Element::Kind;

  print("[Module] " + m->name);
  indent++;

  for(Element *e : m->elements)
  {
    switch(e->kind())
    {
      case K::Node: print(as<const Node>(e)); break;
    }
  }

  indent--;

  return ss.str();
}

void
MetaASTPrinter::print(const Node *n)
{
  print("[Node] " + n->name);
  indent++;
  
  for(Variable *v : n->vars) { print(v); }
  for(Alias *a : n->aliases) { print(a); }

  indent--;
}

void
MetaASTPrinter::print(const Alias *a)
{
  print("[Alias] name=" + a->name);
  indent++;

  print(a->expr);

  indent--;
}

void
MetaASTPrinter::print(const Expr *e)
{
  print("[Expr] ");
  indent++;

  print(e->l);
  if(e->r){ print(e->r); }

  indent--;
}

void
MetaASTPrinter::print(const Term *t)
{
  print("[Term] ");
  indent++;

  print(t->l);
  if(t->r){ print(t->r); }

  indent--;
}

void
MetaASTPrinter::print(const Factor *f)
{
  print("[Factor] ");
  indent++;

  print(f->atom);
  if(f->exp){ print(f->exp); }

  indent--;
}

void
MetaASTPrinter::print(const Atom *a)
{
  print("[Atom] ");
  indent++;

  using K = Atom::Kind;
  switch(a->kind())
  {
    case K::Real: print(as<const Real>(a)); break;
    case K::Symbol: print(as<const Symbol>(a)); break;
    case K::Expr: print(as<const ExprAtom>(a)); break;
    case K::Funcall: print(as<const FuncallAtom>(a)); break;
  }

  indent--;
}

void
MetaASTPrinter::print(const Real *r)
{
  print("[Real] " + std::to_string(r->value));
}

void
MetaASTPrinter::print(const Symbol *s)
{
  print("[Symbol] " + s->value);
}

void
MetaASTPrinter::print(const ExprAtom *s)
{
  print(s->value);
}

void
MetaASTPrinter::print(const FuncallAtom *f)
{
  print("[Funcall] ");
}

void 
MetaASTPrinter::print(const Variable *v)
{
  print("[Variable] name=" + v->name + " type=" + v->type);
}

void 
MetaASTPrinter::print(string s)
{
  ss << string(indent.pos(), ' ') << s << endl;
}

/*
string
MetaASTPrinter::print(const ASTNode *n)
{
  using K = ASTNode::Kind;

  switch(n->kind())
  {
    case K::Module: print(as<const Module>(n)); break;
  }

  return ss.str();
}

void
MetaASTPrinter::print(const Ident *id)
{
  print("[Ident] " + id->name);
}

void 
MetaASTPrinter::print(const Module *m)
{
  print("[Module]");
  indent++;
  
  print(m->id);
  for(Decl *d : *(m->decls))
  {
    print(d);
  }

  indent--;
}

void
MetaASTPrinter::print(const Decl *d)
{
  using K = ASTNode::Kind;
  switch(d->kind())
  {
    case K::NodeDecl: print(as<const NodeDecl>(d)); break;
    case K::LinkDecl: print(as<const LinkDecl>(d)); break;
  }
}

void
MetaASTPrinter::print(const NodeDecl *n)
{
  print("[NodeDecl]");
  indent++;

  print(n->id);
  print(n->info); 

  indent--;
}

void
MetaASTPrinter::print(const Assignment *a)
{
  print("[Assignment]");
  indent++;

  print(a->l);
  print(a->r);

  indent--;
}

void
MetaASTPrinter::print(const Stmt *s)
{
  using K = ASTNode::Kind;
  switch(s->kind())
  {
    case K::Ident: print(as<const Ident>(s)); break;
    case K::Array: print(as<const Array>(s)); break;
  }
}

void
MetaASTPrinter::print(const Array *a)
{
  print("[Array]");
  indent++;

  for(Stmt *s : *(a->elems)) { print(s); }

  indent--;
}

void
MetaASTPrinter::print(const LinkDecl *n)
{
  print("[LinkDecl]");
  indent++;

  print(n->id);

  indent--;
}


*/

