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
    size_t line = (*duplicate)->line_no();
    string msg = "duplicate variable name " + (*duplicate)->name;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }

}
