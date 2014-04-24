#include "MetaSema.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"

using namespace ps::meta;

using std::string;
using std::vector;

void 
Sema::buildAst(const string &src)
{
  mm = nullptr;
  std::string source = readFile(src); 
  metayy_scan_string(source.c_str());
  metayyparse();
  if(mm == nullptr)
  {
    throw std::runtime_error("compilation failed for "+src);
  }
}

void
Sema::check()
{
  for(const string &src_file : source_files)
  {
    curr_file = src_file;
    buildAst(src_file);
    check(mm);
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
  checkFor_DuplicateVarNames(n->vars);
  checkFor_DuplicateAliasNames(n->aliases);
  checkFor_DuplicateLazyVarNames(n->lazy_vars);
  checkFor_InvalidReferences(n, m);
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
    size_t line = (*duplicate)->line_no();
    string msg = "duplicate variable name " + (*duplicate)->name;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }
}

void
Sema::checkFor_DuplicateAliasNames(const Aliases &a)
{
  auto ac = a;
  std::sort(ac.begin(), ac.end(),
      [](Alias *a, Alias *b) { return a->name < b->name; });

  auto duplicate = 
    std::adjacent_find(ac.begin(), ac.end(),
        [](Alias *a, Alias *b) { return a->name == b->name; });

  if(duplicate != ac.end())
  {
    size_t line = (*duplicate)->line_no();
    string msg = "duplicate alias name " + (*duplicate)->name;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }
}

void
Sema::checkFor_DuplicateLazyVarNames(const LazyVars &l)
{
  auto lc = l;
  std::sort(lc.begin(), lc.end(),
      [](LazyVar *a, LazyVar *b) { return a->name < b->name; });

  auto duplicate = 
    std::adjacent_find(lc.begin(), lc.end(),
        [](LazyVar *a, LazyVar *b) { return a->name == b->name; });

  if(duplicate != lc.end())
  {
    size_t line = (*duplicate)->line_no();
    string msg = "duplicate lazy variable name " + (*duplicate)->name;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }
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
