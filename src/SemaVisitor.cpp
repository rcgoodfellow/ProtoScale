#include "SemaVisitor.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"
#include "ModelParser.hpp"
#include "ModelScanner.hpp"

#include <iostream>

using namespace ps;
using namespace ps::meta;

using std::string;
using std::vector;
using std::unordered_map;

unordered_map<string, Definition*>
ps::Scope::definitions()
{
  unordered_map<string, Definition*> d;
  for(auto *c : contexts)
  {
    auto defs = c->definitions();
    d.insert(defs.begin(), defs.end());
  }
  return d;
}

Definition*
ps::Scope::definition(string name)
{
  Definition *d{nullptr};
  for(auto *c : contexts)
  {
    d = c->definition(name);
    if(d){ return d; }
  }
  return nullptr;
}

//Sema implementation ---------------------------------------------------------

meta::ModuleFragment*
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
  module_fragment_map[mm->name].push_back(mm);
  return mm;
}

model::ModelFragment*
Sema::buildModelAst(const string &src)
{
  mdl = nullptr;
  std::string source = readFile(src); 
  modelyy_scan_string(source.c_str());
  modelyyparse();
  if(mdl == nullptr)
  {
    throw std::runtime_error("compilation failed for " + src);
  }
  model_fragment_map[mm->name].push_back(mdl);
  return mdl;
}

void Sema::check()
{
  for(const string &src_file : source_files)
  {
    curr_file = src_file;

    if(src_file.substr(src_file.length()-4, src_file.length()) == ".pmm")
    {
      meta::ModuleFragment *m;
      try{ m = buildMetaAst(src_file); }
      catch(compilation_error &e)
      {
        for(Diagnostic &d : e.diagnostics) { d.filename = src_file; }
        throw;
      }
      meta_visitor.filename = curr_file;
      m->accept(meta_visitor);
    }
    else if(src_file.substr(src_file.length()-3, src_file.length()) == ".pm")
    {
      model::ModelFragment *m;
      try{ m = buildModelAst(src_file); }
      catch(compilation_error &e)
      {
        for(Diagnostic &d : e.diagnostics) { d.filename = src_file; }
        throw;
      }
      //model_visitor.scope = meta_visitor.scope;
      model_visitor.filename = curr_file;
      model_visitor.model = m;
      m->accept(model_visitor);
    }
  }
}

meta::Element*
Sema::element(std::string name)
{
  meta::Definition *d;
  meta::Element *e;
  for(auto &p : module_fragment_map)
  {
    for(ModuleFragment *m : p.second)
    {
      d = m->definition(name);
      if(d)
      {
        e = dynamic_cast<Element*>(d);
        if(e){ return e; }
      }
    }
  }

  return nullptr;
}

// Meta Visitor Implementation ------------------------------------------------

void MetaSemaVisitor::visit(ModuleFragment *m) 
{ 
  for(auto &p : m->nodes) { p.second->parent = m; }
  for(auto &p : m->links) { p.second->parent = m; }
  ctx = m;
  //scope.contexts.push_back(m);
}
void MetaSemaVisitor::leave(ModuleFragment*) 
{
}


void MetaSemaVisitor::visit(Node *n) 
{ 
  for(auto &p : n->interlates) { p.second->parent = n; }
  saved_ctx = ctx;
  ctx = n;
}
void MetaSemaVisitor::leave(Node*) 
{ 
  ctx = saved_ctx;
}

void MetaSemaVisitor::visit(Link *l) 
{ 
  saved_ctx = ctx;
  ctx = l;
}
void MetaSemaVisitor::leave(Link*) 
{ 
  ctx = saved_ctx;
}


void MetaSemaVisitor::visit(Interlate *i) 
{ 
  saved_ctx = ctx;
  ctx = i;
}
void MetaSemaVisitor::leave(Interlate*) 
{ 
  ctx = saved_ctx;
}

void MetaSemaVisitor::visit(Symbol *s) 
{ 
  s->definition_context = ctx;
  if(s->subscripted())
  {
    //get the location of the variable
    Definition *location = 
      s->definition_context->definition(s->location());
    if(!location)
    {
      string msg = "unknown location "+s->location();
      diagnostics.push_back({filename, s->line_number, msg});
      throw compilation_error{diagnostics};
    }

    //get the type of the location
    Definition *location_type = 
      s->definition_context->definition(location->type);
    if(!location_type)
    {
      string msg = "unknown location type "+location->type;
      diagnostics.push_back({filename, s->line_number, msg});
      throw compilation_error{diagnostics};
    }
  
    //get the definition context for the location
    DefinitionContext *location_ctx = 
      dynamic_cast<DefinitionContext*>(location_type);
    if(!location_ctx)
    {
      string msg = "] invalid location "+location->type;
      diagnostics.push_back({filename, s->line_number, msg});
      throw compilation_error{diagnostics};
    }

    //get the definition for the reference
    Definition *d = location_ctx->definition(s->localName());
    if(!d)
    {
      string msg = location_type->name+" does not contain a member "+
                   s->localName();
      diagnostics.push_back({filename, s->line_number, msg});
      throw compilation_error{diagnostics};
    }
    s->definition = d;
  }
  else
  {
    Definition *symbol_defn = 
      s->definition_context->definition(s->localName()); 
    if(!symbol_defn)
    {
      string msg = "unkown symbol "+s->localName();
      diagnostics.push_back({filename, s->line_number, msg});
      throw compilation_error{diagnostics};
    }
    s->definition = symbol_defn; 
  }
}

void
MetaSemaVisitor::leave(Symbol *s)
{
  s->type = s->definition->type; 

  Variable *v = dynamic_cast<Variable*>(s->definition);
  if(v){ s->dynamic = v->dynamic; return; }

  Alias *a = dynamic_cast<Alias*>(s->definition);
  if(a){ s->dynamic = a->accessor->dynamic; return; }

}

void
MetaSemaVisitor::leave(FunctionCall *f)
{
  f->type = "real";
  f->dynamic = false;
  for(Expression *e : f->arguments)
  {
    if(e->type == "complex"){ f->type = "complex"; }
    if(e->dynamic){ f->dynamic = true; }
  }
}

void
MetaSemaVisitor::leave(BinaryOperation *b)
{
  if(b->lhs->type == "complex" || b->rhs->type == "complex")
  { 
    b->type = "complex";
  }
  else{ b->type = "real"; }

  if(b->lhs->dynamic || b->rhs->dynamic){ b->dynamic = true; }
  else{ b->dynamic = false; }
} 

void
MetaSemaVisitor::leave(Alias *a)
{
  a->type = "real";
  a->dynamic = a->accessor->arguments[0]->dynamic;
}


//Model Visitor Implementation ------------------------------------------------

void ModelSemaVisitor::visit(Import *i)
{
  vector<ModuleFragment*> mod_frags;
  try { mod_frags = sema->module_fragment_map.at(i->module_name); }
  catch(std::out_of_range &e)
  {
    string msg = "unknown module "+i->module_name;
    diagnostics.push_back({filename, i->line_no(), msg});
    throw compilation_error{diagnostics};
  }
}

void ModelSemaVisitor::visit(Create *c)
{
  Definition *d{nullptr};
  Element *e{nullptr};

  //Find the type definition for the elements being created
  for(auto &p : sema->module_fragment_map)
  {
    for(ModuleFragment *f : p.second)
    {
      d = f->definition(c->type_tgt);
      if(d){ break; }
    }
    if(d){ break; }
  }
  //If no type is found throw
  if(!d)
  {
    string msg = "unknown element "+c->type_tgt;
    diagnostics.push_back({filename, c->line_no(), msg});
    throw compilation_error{diagnostics};
  }
  //If the type found is not an element throw
  e = dynamic_cast<Element*>(d);
  if(!e)
  {
    string msg = "unknown element "+c->type_tgt;
    diagnostics.push_back({filename, c->line_no(), msg});
    throw compilation_error{diagnostics};
  }
  //Check initialization parameters
  std::unordered_set<string> 
    p_required{e->params.begin(), e->params.end()}, 
    p_supplied{c->fmt->var_names->begin(), c->fmt->var_names->end()}, 
    required_but_not_supplied,
    supplied_but_not_required;

  std::set_difference(p_required.begin(), p_required.end(),
                      p_supplied.begin(), p_supplied.end(),
                      std::inserter(required_but_not_supplied, 
                                    required_but_not_supplied.end()
                      )
  );
  if(!required_but_not_supplied.empty())
  {
    for(const string &p : required_but_not_supplied)
    {
      string msg = c->type_tgt+" constructor must initialize the parameter "+p;
      diagnostics.push_back({filename, c->line_no(), msg});
    }
      
    throw compilation_error{diagnostics};

  }

  std::set_difference(p_supplied.begin(), p_supplied.end(),
                      p_required.begin(), p_required.end(),
                      std::inserter(supplied_but_not_required,
                                    supplied_but_not_required.end()
                        )
  );
  if(!supplied_but_not_required.empty())
  {
    for(const string &p : supplied_but_not_required)
    {
      string msg = c->type_tgt+" constructor cannot initialize the parameter "+p;
      diagnostics.push_back({filename, c->line_no(), msg});
    }
      
    throw compilation_error{diagnostics};

  }

  bool err{false};
  for(CreateTarget *t : *(c->tgts))
  {
    if(names.find(t->name) != names.end())
    {
      string msg = "Duplicate name "+t->name;
      diagnostics.push_back({filename, t->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    if(t->args->vals.size() != c->fmt->var_names->size())
    {
      err = true;
      string msg = c->type_tgt+" has invalid format, should initialize "+
                   std::to_string(c->fmt->var_names->size())+
                   " values but actually initializes "+
                   std::to_string(t->args->vals.size());
      diagnostics.push_back({filename, t->line_no(), msg});
    }
    if(!err){ names[t->name] = c; }
  }
  if(err) { throw compilation_error{diagnostics}; }
}

void ModelSemaVisitor::visit(Connect *c)
{
  for(Connection *cnx : *(c->connections))
  {
    Create *a = model->find(cnx->a),
           *b = model->find(cnx->b),
           *v = model->find(cnx->via);

    if(!a)
    {
      string msg = "unknown node "+cnx->a;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    if(!b)
    {
      string msg = "unknown node "+cnx->b;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    a->type = dynamic_cast<meta::Node*>(sema->element(a->type_tgt));
    if(!a->type)
    {
      string msg = "unknown node type "+a->type_tgt;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    meta::Definition *d = a->type->definition(cnx->interlate);
    if(!d || !dynamic_cast<meta::Interlate*>(d))
    {
      string msg = "The node type "+a->type_tgt+
                   " does not contain an interlate named "+cnx->interlate;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    
    b->type = dynamic_cast<meta::Node*>(sema->element(b->type_tgt));
    if(!b->type)
    {
      string msg = "unknown node type "+b->type_tgt;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    
    if(c->symmetric)
    {
      if(a->type->name != b->type->name)
      {
        string msg = 
          "Symmetric connections must be between nodes of the same type";
        diagnostics.push_back({filename, cnx->line_no(), msg});
        throw compilation_error{diagnostics};
      }
    }

    
    if(!v)
    {
      string msg = "unknown link "+cnx->via;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    v->type = dynamic_cast<meta::Link*>(sema->element(v->type_tgt));
    if(!v->type)
    {
      string msg = "unknown link type "+v->type_tgt;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    
    Interlate *i = dynamic_cast<Interlate*>(a->type->definition(cnx->interlate)); 
    if(v->type->name != i->link_param->type)
    {
      string msg = "Expected link parameter of type "+i->link_param->type+
                   " but got a "+v->type->name;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
    if(b->type->name != i->node_param->type)
    {
      string msg = "Expected node parameter of type "+i->node_param->type+
                   " but got a "+b->type->name;
      diagnostics.push_back({filename, cnx->line_no(), msg});
      throw compilation_error{diagnostics};
    }
  }
} 
