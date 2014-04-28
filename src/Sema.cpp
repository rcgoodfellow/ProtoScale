#include "Sema.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"

#include "ModelParser.hpp"
#include "ModelScanner.hpp"

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

model::Model*
Sema::buildShellAst(const string &src)
{
  mdl = nullptr;
  std::string source = readFile(src);
  modelyy_scan_string(source.c_str());
  modelyyparse();
  if(!mdl)
  {
    throw std::runtime_error("compilation failed for " + src);
  }
  return mdl;
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
      model::Model *m = buildShellAst(src_file);
      check(m);
    }
  }
}

void
Sema::check(const Module* m)
{
  for(Node *e: m->nodes) { check(e, m); }
  for(Link *l: m->links) { check(l, m); }
}

void
Sema::check(const Element* e, const Module* m)
{
  checkFor_DuplicateNames(e);
  checkFor_InvalidReferences(e, m);
}

void
Sema::checkFor_DuplicateNames(const Element *elem)
{
  unordered_map <string, vector<const NamedLexeme*>> names;
  for(const Variable *v : elem->vars){ names[v->name].push_back(v); }
  for(const Alias *a : elem->aliases){ names[a->name].push_back(a); }
  for(const LazyVar *v : elem->lazy_vars){ names[v->name].push_back(v); }
  if(elem->kind() == Element::Kind::Node)
  {
    const Node *n = dynamic_cast<const Node*>(elem);
    for(const Interlate *i : n->interlates){ names[i->name].push_back(i); }
  }

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
Sema::checkFor_InvalidReferences(const Element *e, const Module *m)
{
  for(const Alias *a : e->aliases)
  {
    checkFor_InvalidReferences(a->accessor, e);
  }
  for(const LazyVar *v : e->lazy_vars)
  {
    checkFor_InvalidReferences(v, e);
  }
  for(const DiffRel *d : e->diffrels)
  {
    checkFor_InvalidReferences(d, e);
  }
  if(e->kind() == Element::Kind::Node)
  {
    const Node *n = dynamic_cast<const Node*>(e);
    for(const Interlate *i : n->interlates)
    {
      checkFor_InvalidReferences(i, n, m);
    }
  }
}

void
Sema::checkFor_InvalidReferences(const Interlate *i, const Node *n, 
                                 const Module *m)
{
  if(i->params.size() != 2)
  {
    string msg = "interlates only have two parameters, a link and a node";
    diagnostics.push_back(Diagnostic{curr_file, i->line_no(), msg});
    throw compilation_error{ diagnostics };
  }
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
Sema::checkFor_InvalidReferences(const Eqtn *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  Variable *vtgt = elem->getVar(e->tgt);
  Alias *atgt = elem->getAlias(e->tgt);
  if(!vtgt && !atgt)
  {
    size_t line = e->line_no();
    string msg = "undefined interlate target " + e->tgt;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }
  checkFor_InvalidReferences(e->expr, elem, ln, l, rn, r);
}

void
Sema::checkFor_InvalidReferences(const Expr *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  using K = Expr::Kind;
  switch(e->kind())
  {
    case K::AddOp: checkFor_InvalidReferences(dynamic_cast<const AddOp*>(e),
                                              elem, ln, l, rn, r); break;
    case K::MulOp: checkFor_InvalidReferences(dynamic_cast<const MulOp*>(e),
                                              elem, ln, l, rn, r); break;
    case K::ExpOp: checkFor_InvalidReferences(dynamic_cast<const ExpOp*>(e),
                                              elem, ln, l, rn, r); break;
    case K::Real: checkFor_InvalidReferences(dynamic_cast<const Real*>(e),
                                              elem, ln, l, rn, r); break;
    case K::Symbol: checkFor_InvalidReferences(dynamic_cast<const Symbol*>(e),
                                              elem, ln, l, rn, r); break;
    case K::Funcall: checkFor_InvalidReferences(dynamic_cast<const FuncallAtom*>(e),
                                              elem, ln, l, rn, r); break;
    case K::ExprAtom: checkFor_InvalidReferences(dynamic_cast<const ExprAtom*>(e),
                                              elem, ln, l, rn, r); break;
    default: ;
  }
}

void
Sema::checkFor_InvalidReferences(const AddOp *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->l, elem, ln, l, rn, r); 
  if(e->r) checkFor_InvalidReferences(e->r, elem, ln, l, rn, r); 
}

void
Sema::checkFor_InvalidReferences(const MulOp *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->l, elem, ln, l, rn, r); 
  if(e->r) checkFor_InvalidReferences(e->r, elem, ln, l, rn, r); 
}

void
Sema::checkFor_InvalidReferences(const ExpOp *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->l, elem, ln, l, rn, r); 
  if(e->r) checkFor_InvalidReferences(e->r, elem, ln, l, rn, r); 
}

void
Sema::checkFor_InvalidReferences(const Real *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  return;
}

void
Sema::checkFor_InvalidReferences(const Symbol *e, const Element *elem,
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
    if(!elem->hasSymbol(e->value))
    {
      size_t line = e->line_no();
      string msg = "undefined symbol " + e->value;
      diagnostics.push_back(Diagnostic{curr_file, line, msg});
      throw compilation_error{ diagnostics };
    }
  }
}

void
Sema::checkFor_InvalidReferences(const FuncallAtom *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->value, elem, ln, l, rn, r);
}

void
Sema::checkFor_InvalidReferences(const Funcall *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  for(Expr *ex : e->args) { checkFor_InvalidReferences(ex, elem, ln, l, rn, r); }
}

void
Sema::checkFor_InvalidReferences(const ExprAtom *e, const Element *elem,
                                 const std::string &ln, const Link *l, 
                                 const std::string &rn, const Node *r)
{
  checkFor_InvalidReferences(e->value, elem, ln, l, rn, r);
}

void
Sema::checkFor_InvalidReferences(const Funcall *f, const Element *elem)
{
  for(const Expr *e : f->args) 
  { 
    checkFor_InvalidReferences(e, elem, "", nullptr, "", nullptr); 
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
Sema::checkFor_InvalidReferences(const Symbol *s, const Element *elem)
{
  const Variable *v = elem->getVar(s->value);
  if(!v) { undefined_Var(s->value, s); }
}

void
Sema::checkFor_InvalidReferences(const Accessor *a, const Element *e)
{
  const Variable *v = e->getVar(a->target);
  if(!v) { undefined_Var(a->target, a); }

  if(!(a->name == "re"  || a->name == "im" || 
       a->name == "mag" || a->name == "angle"))
  {
    string msg = "undefined accessor " + a->name;
    diagnostics.push_back(Diagnostic{curr_file, a->line_no(), msg});
    throw compilation_error{ diagnostics };
  }
}

void
Sema::checkFor_InvalidReferences(const FuncallAtom *f, const Element *elem)
{
  checkFor_InvalidReferences(f->value, elem);
}

void
Sema::checkFor_InvalidReferences(const LazyVar *v, const Element *e)
{
  checkFor_InvalidReferences(v->expr, e, "", nullptr, "", nullptr);   
}

void
Sema::checkFor_InvalidReferences(const DiffRel *d, const Element *elem)
{  
  if(!elem->hasSymbol(d->tgt))
  {
    size_t line = d->line_no();
    string msg = "undefined differential relation target " + d->tgt;
    diagnostics.push_back(Diagnostic{curr_file, line, msg});
    throw compilation_error{ diagnostics };
  }
  checkFor_InvalidReferences(d->expr, elem, "", nullptr, "", nullptr);
}

void
Sema::check(model::Model *m)
{
  using K = model::Command::Kind;
  std::vector<ModuleFragment> mfrags;;
  for(model::Command* c : *m->commands)
  {
    switch(c->kind())
    {
      case K::Import: 
      {
        std::vector<ModuleFragment> mfs = 
          check_Import(dynamic_cast<const model::Import*>(c)); 
        mfrags.insert(mfrags.end(), mfs.begin(), mfs.end());
        break;    
      }
      case K::Create: 
      {
        check_Create(dynamic_cast<model::Create*>(c), mfrags);
        break;
      }
      case K::Connect:
      {
        model::Connect *cnx = dynamic_cast<model::Connect*>(c);
        check_ConnectReferences(cnx, m);
        check_ConnectInterlate(cnx);
        break;
      }
    }
  }
}

vector<ModuleFragment>
Sema::check_Import(const model::Import *i)
{
  try
  {
    return module_source_map.at(i->module_name);
  }
  catch(std::out_of_range &e)
  {
    string msg = "undefined module " + i->module_name;
    diagnostics.push_back( Diagnostic{curr_file, i->line_no(), msg} );
    throw compilation_error { diagnostics };
  }
}

void
Sema::check_Create(model::Create *c,
                   const std::vector<ModuleFragment> &mfs)
{
  check_CreateType(c, mfs);
  check_CreateParamsLegit(c);
  check_CreateRequiredParams(c);
  check_CreateArgsParamList(c);
}

void 
Sema::check_CreateType(model::Create *c, 
                       const std::vector<ModuleFragment> &mfs)
{
  Element *type;
  for(const ModuleFragment &f : mfs)
  {
    type = f.m->getNode(c->type_tgt);
    if(type)
    {
      c->type = type;
      break;
    }
    type = f.m->getLink(c->type_tgt);
    {
      c->type = type;
      break;
    }
  }

  if(!type)
  {
    string msg = "undefined element type " + c->type_tgt;
    diagnostics.push_back( Diagnostic{curr_file, c->line_no(), msg} );
    throw compilation_error { diagnostics };
  }
}

void
Sema::check_CreateRequiredParams(model::Create *c)
{
  bool err{false};

  for(const string *a : *(c->type->params))
  {
    auto i = std::find_if(
        c->fmt->var_names->begin(),
        c->fmt->var_names->end(),
        [a](const string &s){ return s == *a; }
        );
    
    if(i == c->fmt->var_names->end())
    {
      err = true;
      string msg = "create parameters for " + c->type_tgt
                 + " must include " + *a;
      diagnostics.push_back( Diagnostic{curr_file, c->line_no(), msg} );
    }
  }

  if(err)
  {
    throw compilation_error { diagnostics };
  }
}

void
Sema::check_CreateParamsLegit(model::Create *c)
{
  bool err{false};

  for(const string &a : *(c->fmt->var_names))
  {
    auto i = std::find_if(
        c->type->params->begin(),
        c->type->params->end(),
        [&a](const string *s){ return *s == a; }
        );

    if(i == c->type->params->end())
    {
      err = true;
      string msg = a + " is an invalid create parameter for " + c->type_tgt;
      diagnostics.push_back( Diagnostic{curr_file, c->line_no(), msg} );
    }
  }

  if(err)
  {
    throw compilation_error { diagnostics };
  }
}

void 
Sema::check_CreateArgsParamList(model::Create *c)
{
  bool err{false};

  for(const model::CreateTarget *t : *(c->tgts))
  {
    if(t->args->vals.size() != c->fmt->var_names->size())
    {
      err = true;
      string msg = "the element type " + c->type_tgt
                 + " requires " + std::to_string(c->fmt->var_names->size())
                 + " parameters for creation";
      diagnostics.push_back( Diagnostic{curr_file, c->line_no(), msg} );
    }
  }
  
  if(err)
  {
    throw compilation_error { diagnostics };
  }
}
    
void 
Sema::check_ConnectReferences(model::Connect *cn, model::Model *m)
{
  for(model::Connection *c : *(cn->connections))
  {
    check_ConnectionReferences(c, m);
  }
}

void
Sema::check_ConnectionReferences(model::Connection *cnx, model::Model *m)
{
  for(model::Command *c : *m->commands)
  {
    if(c->kind() == model::Command::Kind::Create)
    {
      model::Create *cr = dynamic_cast<model::Create*>(c);
      if(cr->type->kind() == meta::Element::Kind::Node)
      {
        for(model::CreateTarget *tgt : *(cr->tgts))
        {
          if(tgt->name == cnx->a)
          {
            cnx->ap = cr;
          }
          if(tgt->name == cnx->b)
          {
            cnx->bp = cr;
          }
          if(cnx->ap && cnx->bp) { break; }
        }
      }
      if(cr->type->kind() == meta::Element::Kind::Link)
      {
        for(model::CreateTarget *tgt : *(cr->tgts))
        {
          if(tgt->name == cnx->via)
          {
            cnx->viap = cr;
            break;
          }
        }
      }
      if(cnx->ap && cnx->bp && cnx->viap) { break; }
    }
  }
  if(!(cnx->ap && cnx->bp && cnx->viap)) 
  { 
    if(!cnx->ap)
    {
      string msg = "undefined reference to " + cnx->a;
      diagnostics.push_back(Diagnostic{curr_file, cnx->line_no(), msg});
    }
    if(!cnx->bp)
    {
      string msg = "undefined reference to " + cnx->b;
      diagnostics.push_back(Diagnostic{curr_file, cnx->line_no(), msg});
    }
    if(!cnx->viap)
    {
      string msg = "undefined reference to " + cnx->via;
      diagnostics.push_back(Diagnostic{curr_file, cnx->line_no(), msg});
    }
    
    throw compilation_error { diagnostics };
  }
  
}

void
Sema::check_ConnectInterlate(model::Connect *cnx)
{
  for(model::Connection *c : *(cnx->connections))
  {
    check_ConnectInterlate(c);
  }

}

void
Sema::check_ConnectInterlate(model::Connection *cnx)
{
  if(cnx->symmetric && (cnx->ap->type != cnx->bp->type))
  {
    string msg = "A connection between two different types of nodes "
               + string("cannot be symmetric");
    diagnostics.push_back(Diagnostic{curr_file, cnx->line_no(), msg});
    throw compilation_error{ diagnostics };
  }

  //check that target interlate exists
  meta::Node *n = dynamic_cast<meta::Node*>(cnx->ap->type);
  meta::Interlate *i = n->getInterlate(cnx->interlate); 
  if(!i)
  {
    string msg = "The node " + n->name + " does not have an interlate named "
               + cnx->interlate;
    diagnostics.push_back(Diagnostic{curr_file, cnx->line_no(), msg});
    throw compilation_error { diagnostics };
  }

  //check the argument types against the interlate
  //node type 
  if(i->params[1]->type != cnx->bp->type->name)
  {
    string msg = "The interlate " + cnx->interlate
               + " does not accept a node parameter of type "
               + cnx->bp->type->name;
    diagnostics.push_back(Diagnostic{curr_file, cnx->line_no(), msg});
    throw compilation_error { diagnostics };
  }
  
  //link type
  if(i->params[0]->type != cnx->viap->type->name)
  {
    string msg = "The interlate " + cnx->interlate
               + " does not accept a link parameter of type "
               + cnx->viap->type->name;
    diagnostics.push_back(Diagnostic{curr_file, cnx->line_no(), msg});
    throw compilation_error { diagnostics };
  }


}
