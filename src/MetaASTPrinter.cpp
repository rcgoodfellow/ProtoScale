#include "MetaASTPrinter.hpp"

using std::string;
using std::endl;

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
  print("[Ident] name=" + id->name);
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

void 
MetaASTPrinter::print(string s)
{
  ss << string(indent.pos(), ' ') << s << endl;
}

