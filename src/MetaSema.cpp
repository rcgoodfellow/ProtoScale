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
Sema::checkFor_InvalidReferences(const Node *n, const Module *m)
{
  using K = Expr::Kind;
  for(const Alias *a : n->aliases)
  {
    checkFor_InvalidReferences(a->accessor, n);
  }
}

void
Sema::checkFor_InvalidReferences(const Interlate *i, const Node *n, 
                                 const Module *m)
{
  Variable *line = i->params[0],
           *node = i->params[1];

  
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
