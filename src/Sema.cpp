#include "Sema.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"

#include "ShellParser.hpp"
#include "ShellScanner.hpp"

using namespace ps;
using namespace meta;

using std::string;
using std::vector;
using std::unordered_map;

meta::Module*
Sema::buildMetaAst(const string &src)
{
  mm = nullptr;
  std::string source = readFile(src); 
  metayy_scan_string(source.c_str());
  metayyparse();
  if(mm == nullptr)
  {
    throw std::runtime_error("compilation failed for " + src);
  }
  module_source_map[mm->name].push_back({mm, src});
  return mm;
}

shell::Commands*
Sema::buildShellAst(const string &src)
{
  sh_cmds = nullptr;
  std::string source = readFile(src);
  shellyy_scan_string(source.c_str());
  shellyyparse();
  if(!sh_cmds)
  {
    throw std::runtime_error("compilation failed for " + src);
  }
  return sh_cmds;
}

void
Sema::check()
{
  for(const string &src_file : source_files)
  {
    curr_file = src_file;

    if(src_file.substr(src_file.length()-4, src_file.length()) == ".pmm")
    {
      meta::Module *m = buildMetaAst(src_file);
      check(m);
    }
    else if(src_file.substr(src_file.length()-3, src_file.length()) == ".pm")
    {
      shell::Commands *c = buildShellAst(src_file);
      check(c);
    }
  }
}

void
Sema::check(const Module* m)
{
  using K = Element::Kind;
  for(Node *e: m->nodes) { check(dynamic_cast<const Node*>(e), m); }
}

void
Sema::check(const Node* n, const Module* m)
{
  checkFor_DuplicateNames(n);
  checkFor_InvalidReferences(n, m);
}

void
Sema::checkFor_DuplicateNames(const Node *n)
{
  unordered_map <string, vector<const NamedLexeme*>> names;
  for(const Variable *v : n->vars){ names[v->name].push_back(v); }
  for(const Alias *a : n->aliases){ names[a->name].push_back(a); }
  for(const LazyVar *v : n->lazy_vars){ names[v->name].push_back(v); }

  bool name_conflict{false};
  for(auto &e : names)
  {
    if(e.second.size() > 1)
    {
      name_conflict = true;
      diagnostics.push_back(
          Diagnostic{curr_file, 
                     e.second[0]->line_no(),
                     "duplicate name " + e.second[0]->name}
      );
      for(size_t i=1; i< e.second.size(); ++i)
      {
        diagnostics.push_back(
            Diagnostic{curr_file,
                       e.second[i]->line_no(),
                       "also declared here",
                       Diagnostic::Kind::Info}
            );
      }
    }
  }
  if(name_conflict) { throw compilation_error{ diagnostics }; }

}

void
Sema::checkFor_InvalidReferences(const Node *n, const Module *m)
{
  using K = Expr::Kind;
  for(const Alias *a : n->aliases)
  {
    checkFor_InvalidReferences(a->accessor, n);
  }
  for(const Interlate *i : n->interlates)
  {
    checkFor_InvalidReferences(i, n, m);
  }
  for(const LazyVar *v : n->lazy_vars)
  {
    checkFor_InvalidReferences(v, n);
  }
  for(const DiffRel *d : n->diffrels)
  {
    checkFor_InvalidReferences(d, n);
  }
}

void
Sema::checkFor_InvalidReferences(const Interlate *i, const Node *n, 
                                 const Module *m)
{
  Variable *link = i->params[0],
           *remote = i->params[1];
 
  //Find the link type from within the module
  Link *link_type = m->getLink(link->type);
  if(!link_type)
  {
    size_t line = link->line_no();
    string msg = "undefined link type " + link->type;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }

  //Find the node neighbor type from within the module
  Node *remote_type = m->getNode(remote->type);
  if(!remote_type)
  {
    size_t line = remote->line_no();
    string msg = "undefined node type " + remote->type;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }

  for(const Eqtn *e : i->eqtns)
  {
    checkFor_InvalidReferences(e, n, 
                               link->name, link_type, 
                               remote->name, remote_type);
  }
}

void
Sema::checkFor_InvalidReferences(const Eqtn *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  Variable *vtgt = n->getVar(e->tgt);
  Alias *atgt = n->getAlias(e->tgt);
  if(!vtgt && !atgt)
  {
    size_t line = e->line_no();
    string msg = "undefined interlate target " + e->tgt;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }
  checkFor_InvalidReferences(e->expr, n, ln, l, rn, r);
}

void
Sema::checkFor_InvalidReferences(const Expr *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  using K = Expr::Kind;
  switch(e->kind())
  {
    case K::AddOp: checkFor_InvalidReferences(dynamic_cast<const AddOp*>(e),
                                              n, ln, l, rn, r); break;
    case K::MulOp: checkFor_InvalidReferences(dynamic_cast<const MulOp*>(e),
                                              n, ln, l, rn, r); break;
    case K::ExpOp: checkFor_InvalidReferences(dynamic_cast<const ExpOp*>(e),
                                              n, ln, l, rn, r); break;
    case K::Real: checkFor_InvalidReferences(dynamic_cast<const Real*>(e),
                                              n, ln, l, rn, r); break;
    case K::Symbol: checkFor_InvalidReferences(dynamic_cast<const Symbol*>(e),
                                              n, ln, l, rn, r); break;
    case K::Funcall: checkFor_InvalidReferences(dynamic_cast<const FuncallAtom*>(e),
                                              n, ln, l, rn, r); break;
    case K::ExprAtom: checkFor_InvalidReferences(dynamic_cast<const ExprAtom*>(e),
                                              n, ln, l, rn, r); break;
  }
}

void
Sema::checkFor_InvalidReferences(const AddOp *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->l, n, ln, l, rn, r); 
  if(e->r) checkFor_InvalidReferences(e->r, n, ln, l, rn, r); 
}

void
Sema::checkFor_InvalidReferences(const MulOp *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->l, n, ln, l, rn, r); 
  if(e->r) checkFor_InvalidReferences(e->r, n, ln, l, rn, r); 
}

void
Sema::checkFor_InvalidReferences(const ExpOp *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->l, n, ln, l, rn, r); 
  if(e->r) checkFor_InvalidReferences(e->r, n, ln, l, rn, r); 
}

void
Sema::checkFor_InvalidReferences(const Real *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  return;
}

void
Sema::checkFor_InvalidReferences(const Symbol *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  size_t dotsep = e->value.find(".");
  if(dotsep != std::string::npos)
  {
    std::string varname = e->value.substr(0, dotsep); 
    std::string varloc = e->value.substr(dotsep+1, e->value.length());
    if(varloc == ln)
    {
      if(!l->hasSymbol(varname))
      {
        size_t line = e->line_no();
        string msg = "undefined link variable " + varname;
        diagnostics.push_back(Diagnostic{curr_file, line, msg});
        throw compilation_error{ diagnostics };
      }
    }
    else if(varloc == rn)
    {
      if(!r->hasSymbol(varname))
      {
        size_t line = e->line_no();
        string msg = "undefined remote variable " + varname;
        diagnostics.push_back(Diagnostic{curr_file, line, msg});
        throw compilation_error{ diagnostics };
      }
    }
    else
    {
      size_t line = e->line_no();
      string msg = "undefined variable location " + varloc;
      diagnostics.push_back(Diagnostic{curr_file, line, msg});
      throw compilation_error{ diagnostics };
    }
  }
  else
  {
    if(!n->hasSymbol(e->value))
    {
      size_t line = e->line_no();
      string msg = "undefined symbol " + e->value;
      diagnostics.push_back(Diagnostic{curr_file, line, msg});
      throw compilation_error{ diagnostics };
    }
  }
}

void
Sema::checkFor_InvalidReferences(const FuncallAtom *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->value, n, ln, l, rn, r);
}

void
Sema::checkFor_InvalidReferences(const Funcall *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  for(Expr *ex : e->args) { checkFor_InvalidReferences(ex, n, ln, l, rn, r); }
}

void
Sema::checkFor_InvalidReferences(const ExprAtom *e, const Node *n,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->value, n, ln, l, rn, r);
}

void
Sema::checkFor_InvalidReferences(const Funcall *f, const Node *n)
{
  for(const Expr *e : f->args) { checkFor_InvalidReferences(e, n); }
}

void
Sema::checkFor_InvalidReferences(const Expr *e, const Node *n)
{
  using K = Expr::Kind;
  switch(e->kind())
  {
    case K::Symbol: checkFor_InvalidReferences(
                        dynamic_cast<const Symbol*>(e), n);
                    break;
  }
}

void
Sema::undefined_Var(const std::string &s, const Lexeme *l)
{
  size_t line = l->line_no();
  string msg = "undefined variable name " + s;
  diagnostics.push_back(Diagnostic{curr_file, line, msg});
  throw compilation_error{ diagnostics };

}

void
Sema::checkFor_InvalidReferences(const Symbol *s, const Node *n)
{
  const Variable *v = n->getVar(s->value);
  if(!v) { undefined_Var(s->value, s); }
}

void
Sema::checkFor_InvalidReferences(const Accessor *a, const Node *n)
{
  const Variable *v = n->getVar(a->target);
  if(!v) { undefined_Var(a->target, a); }
}

void
Sema::checkFor_InvalidReferences(const FuncallAtom *f, const Node *n)
{
  checkFor_InvalidReferences(f->value, n);
}

void
Sema::checkFor_InvalidReferences(const LazyVar *v, const Node *n)
{
  checkFor_InvalidReferences(v->expr, n, "", nullptr, "", nullptr);   
}

void
Sema::checkFor_InvalidReferences(const DiffRel *d, const Node *n)
{  
  if(!n->hasSymbol(d->tgt))
  {
    size_t line = d->line_no();
    string msg = "undefined differential relation target " + d->tgt;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }
  checkFor_InvalidReferences(d->expr, n, "", nullptr, "", nullptr);
}

void
Sema::check(const shell::Commands *cs)
{

}
