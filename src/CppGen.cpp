#include "CppGen.hpp"

#include "MetaParser.hpp"

using namespace ps;
using namespace ps::gen;

using std::string;

void
Cpp::emit_Module(const meta::Module *m)
{
  ofs_hpp = std::ofstream{m->name + ".pmm.hpp", std::ofstream::out};  
  ofs_cpp = std::ofstream{m->name + ".pmm.cpp", std::ofstream::out};
  module = m;
  emit_OpenGuards();
  emit_Includes();
  emit_Usings();
  
  for(const meta::Node *n : module->nodes) { emit_NodeForward(n); }
  for(const meta::Link *n : module->links) { emit_LinkForward(n); }
  ofs_hpp << std::endl << std::endl;

  for(const meta::Node *n : module->nodes) { emit_NodeStruct(n); }
  for(const meta::Link *n : module->links) { emit_LinkStruct(n); }

  emit_CloseGuards();
  ofs_hpp.close();
  ofs_cpp.close();
}

void 
Cpp::emit_OpenGuards()
{
  string upper_mname; 
  std::transform(module->name.begin(), module->name.end(), 
                 std::back_inserter(upper_mname), ::toupper);

  ofs_hpp << "#ifndef _PS_GEN_" << upper_mname << "_MODULE_" << std::endl
      << "#define _PS_GEN_" << upper_mname << "_MODULE_" << std::endl
      << std::endl << std::endl;
}

void
Cpp::emit_Includes()
{
  ofs_hpp << "#include <complex>" << std::endl
      << "#include <vector>" << std::endl
      << "#include <utility>" << std::endl
      << "#include \"PSUtil.hpp\"" << std::endl
      << std::endl << std::endl;

  ofs_cpp << "#include \"" << module->name << ".pmm.hpp\"" 
          << std::endl << std::endl;
}

void
Cpp::emit_Usings()
{
  ofs_hpp << "using complex = std::complex<double>;" << std::endl
      << "using real = double;" << std::endl
      << std::endl << std::endl;
}
  
void 
Cpp::emit_CloseGuards()
{
  ofs_hpp << std::endl << std::endl
      << "#endif";
}
void
Cpp::emit_NodeForward(const meta::Node *n)
{
  ofs_hpp << "struct " << n->name << "; "
      << std::endl;
}

void
Cpp::emit_LinkForward(const meta::Link *l)
{
  ofs_hpp << "struct " << l->name << "; "
      << std::endl;
}

void
Cpp::emit_NodeStruct(const meta::Node *n)
{
  ofs_hpp << "struct " << n->name
      << std::endl
      << "{" << std::endl;

  emit_ElementVars(n);
  emit_TimeFunc();
  emit_ElementAliases(n);
  emit_NodeInterlates(n);

  ofs_hpp << "};" << std::endl << std::endl;
}

void
Cpp::emit_LinkStruct(const meta::Link *l)
{
  ofs_hpp << "struct " << l->name 
      << std::endl
      << "{" << std::endl;
  
  emit_ElementVars(l);
  emit_ElementAliases(l);

  ofs_hpp << "};" << std::endl << std::endl;
}

//TODO: Implement this for real
void
Cpp::emit_TimeFunc()
{
  ofs_hpp << "  double t() { return 0; }" 
          << std::endl << std::endl << std::endl;
}

void
Cpp::emit_ElementVars(const meta::Element *e)
{
  for(const meta::Variable *v : e->vars)
  {
    ofs_hpp << "  " << v->type << " _" << v->name << "_;" << std::endl;

    ofs_hpp << "  inline " << v->type << " " << v->name << "()" << std::endl 
        << "  {" << std::endl
        << "    return " << "this->_" << v->name << "_;" << std::endl
        << "  }" << std::endl;
    
    ofs_hpp << "  inline void " << v->name 
        << "("<< v->type << " " << "v" << ")" << std::endl 
        << "  {" << std::endl
        << "    " << "this->_" << v->name << "_ = v;" << std::endl
        << "  }" << std::endl << std::endl;
  }
  ofs_hpp << std::endl << std::endl;
}

string re_getter(string varname)
{
  return "return this->_" + varname + "_.real();";
}
string re_setter(string varname, string argname)
{
  return "this->_"+varname+"_.real("+argname+");";
}

string im_getter(string varname)
{
  return "return this->_" + varname + "_.imag();";
}
string im_setter(string varname, string argname)
{
  return "this->_"+varname+"_.imag("+argname+");";
}

string mag_getter(string varname)
{
  return "return std::abs(this->_"+varname+"_);";
}
string mag_setter(string varname, string argname, string indent)
{
  return
    indent + "real angle = std::arg(this->_"+varname+"_);\n" +
    indent + "this->_"+varname+"_ = std::polar("+argname+", angle);";
}

string angle_getter(string varname)
{
  return "return std::arg(this->_"+varname+"_);";
}

string angle_setter(string varname, string argname, string indent)
{
  return
    indent + "real mag = std::abs(this->_"+varname+"_);\n" +
    indent + "this->_"+varname+"_ = std::polar(mag, "+argname+");";
}

void
Cpp::emit_ElementAliases(const meta::Element *e)
{
  for(const meta::Alias *a : e->aliases)
  {
    ofs_hpp << "  inline " << "real " << a->name << "() const" << std::endl
        << "  {" << std::endl;

    if(a->accessor->name == "re")
    {
      ofs_hpp << "    " << re_getter(a->accessor->target) << std::endl;
    }
    if(a->accessor->name == "im")
    {
      ofs_hpp << "    " << im_getter(a->accessor->target) << std::endl;
    }
    if(a->accessor->name == "mag")
    {
      ofs_hpp << "    " << mag_getter(a->accessor->target) << std::endl;
    }
    if(a->accessor->name == "angle")
    {
      ofs_hpp << "    " << angle_getter(a->accessor->target) << std::endl;
    }

    ofs_hpp << "  }" << std::endl;
    ofs_hpp << "  inline " << "void " << a->name << "(real v)" << std::endl
        << "  {" << std::endl;
    
    if(a->accessor->name == "re")
    {
      ofs_hpp << "    " << re_setter(a->accessor->target, "v") << std::endl;
    }
    if(a->accessor->name == "im")
    {
      ofs_hpp << "    " << im_setter(a->accessor->target, "v") << std::endl;
    }
    if(a->accessor->name == "mag")
    {
      ofs_hpp << mag_setter(a->accessor->target, "v", "    ") << std::endl;
    }
    if(a->accessor->name == "angle")
    {
      ofs_hpp << angle_setter(a->accessor->target, "v", "    ") << std::endl;
    }

    ofs_hpp << "  }" << std::endl
        << std::endl;
  }
}

void
Cpp::emit_NodeInterlates(const meta::Node *n)
{
  for(const meta::Interlate *i : n->interlates)
  {
    const meta::Variable *lnk = i->params[0],
                         *nod = i->params[1];

    //generate search line-of-bearing and neighbor vectors
    string line_bus_t = lnk->type+"_"+nod->type+"_t";
    ofs_hpp << "  std::vector<double> " << i->name + "_lob;" << std::endl;
    ofs_hpp << "  struct " << line_bus_t
            << "{ "
            << lnk->type << " *" << lnk->name << "; "
            << nod->type << " *" << nod->name << ";"
            << " };" << std::endl;

    ofs_hpp << "  std::vector<"+line_bus_t+"> "
        << i->name << "_nbrs;" << std::endl;

    for(const meta::Eqtn *e : i->eqtns)
    {
      //generate interlate signature
      //expression implementation
      ofs_hpp << "  void " << i->name << "_" << e->tgt << "("
              <<  ");" << std::endl;
      //expression derivative implementation
      ofs_hpp << "  void " << i->name << "_d" << e->tgt << "("
              <<  ");" << std::endl << std::endl;
    
      //generate interlate implementation
      ofs_cpp << "void " << n->name << "::" << i->name << "_" << e->tgt << "("
              << ")" << std::endl
              << "{" << std::endl;
      
      emit_InterlateEqtn(e, i, n);
      
      ofs_cpp << "}" << std::endl << std::endl;

      ofs_cpp << "void " << n->name << "::" << i->name << "_d" << e->tgt << "("
              << ")" << std::endl
              << "{" << std::endl;

      ofs_cpp << "}" << std::endl << std::endl << std::endl;
    }
    ofs_hpp << std::endl;
    ofs_cpp << std::endl;
  }
}

void
Cpp::emit_InterlateEqtn(const meta::Eqtn *e, 
                        const meta::Interlate *i, 
                        const meta::Node *n)
{
  ofs_cpp << "  " << e->tgt << "(" << std::endl;
  
  ofs_cpp << "    ";
  emit_InterlateExpr(e->expr, i, n, true);
  ofs_cpp << std::endl;

  ofs_cpp << "  );" << std::endl;
}

void
Cpp::emit_InterlateExpr(const meta::Expr *e, 
                        const meta::Interlate *i, 
                        const meta::Node *n,
                        bool top)
{
  using K = meta::Expr::Kind;
  switch(e->kind())
  {
    case K::AddOp : 
      emit_InterlateAddOp(dynamic_cast<const meta::AddOp*>(e), i, n, top); 
      break;

    case K::MulOp :
      emit_InterlateMulOp(dynamic_cast<const meta::MulOp*>(e), i, n, top);
      break;

    case K::ExpOp :
      emit_InterlateExpOp(dynamic_cast<const meta::ExpOp*>(e), i, n);
      break;

    case K::Real :
      emit_InterlateReal(dynamic_cast<const meta::Real*>(e));
      break;

    case K::Symbol :
      emit_InterlateSymbol(dynamic_cast<const meta::Symbol*>(e), i, n, top);
      break;

    case K::Funcall :
      emit_InterlateFuncallAtom(dynamic_cast<const meta::FuncallAtom*>(e), i, n);
  }
}

void
Cpp::emit_InterlateAddOp(const meta::AddOp *e,
                         const meta::Interlate *i,
                         const meta::Node *n,
                         bool top)
{
  string nbr_t = i->params[0]->type +"_"+i->params[1]->type+"_t";
  string cname = i->name+"_nbrs";
  bool remotes = hasRemoteRefs(e->l) && top;
  if(remotes) 
  { 
    string et = isComplex(e->l) ? "complex" : "real";
    ofs_cpp << std::endl 
            << "    ps::sum<"+et+">("+cname+", [this](const "+nbr_t+" &_)" 
            << std::endl 
            << "    {return "; 
  }
  emit_InterlateExpr(e->l, i, n);
  if(remotes) { ofs_cpp << ";})" << std::endl; }

  if(e->op == TO_PLUS){ ofs_cpp << " + "; }
  if(e->op == TO_MINUS) { ofs_cpp << " - "; }

  remotes = hasRemoteRefs(e->r) && top;
  if(remotes) 
  { 
    string et = isComplex(e->l) ? "complex" : "real";
    ofs_cpp << std::endl 
            << "    ps::sum<"+et+">("+cname+", [this](const "+nbr_t+" &_)" 
            << std::endl 
            << "    {return "; 
  }
  emit_InterlateExpr(e->r, i, n);
  if(remotes) { ofs_cpp << ";})" << std::endl; }
}

void
Cpp::emit_InterlateMulOp(const meta::MulOp *e, 
                         const meta::Interlate *i,
                         const meta::Node *n,
                         bool top)
{
  string nbr_t = i->params[0]->type +"_"+i->params[1]->type+"_t";
  string cname = i->name+"_nbrs";
  bool remotes = hasRemoteRefs(e->l) && top;
  if(remotes) 
  { 
    ofs_cpp << std::endl 
            << "    ps::sum<real>("+cname+", [this](const "+nbr_t+" &_)" 
            << std::endl 
            << "    {return "; 
  }

  emit_InterlateExpr(e->l, i, n);
  if(remotes) { ofs_cpp << ";})" << std::endl << "    "; }

  if(e->op == TO_MUL){ ofs_cpp << " * "; }
  if(e->op == TO_DIV) { ofs_cpp << " / "; }


  remotes = hasRemoteRefs(e->r) && top;
  if(remotes) 
  { 
    ofs_cpp << std::endl 
            << "    ps::sum<real>("+cname+", [this](const "+nbr_t+" &_)" 
            << std::endl 
            << "    {return "; 
  }
  emit_InterlateExpr(e->r, i, n);
  if(remotes) { ofs_cpp << ";})" << std::endl; }
}

void
Cpp::emit_InterlateExpOp(const meta::ExpOp *e, 
                         const meta::Interlate *i, 
                         const meta::Node *n)
{
  ofs_cpp << "pow(";
  emit_InterlateExpr(e->l, i, n);
  ofs_cpp << ", ";
  emit_InterlateExpr(e->r, i, n);
  ofs_cpp << ")";
}

void
Cpp::emit_InterlateReal(const meta::Real *e)
{
  ofs_cpp << e->value;
}

void
Cpp::emit_InterlateSymbol(const meta::Symbol *e, 
                          const meta::Interlate *i,
                          const meta::Node *n,
                          bool top)
{
  size_t dot = e->value.find('.');
  if(dot == string::npos)
  {
    ofs_cpp << e->value+"()";
    return;
  }
  
  string nbr_t = i->params[0]->type +"_"+i->params[1]->type+"_t";
  string cname = i->name+"_nbrs";

  if(top)
  {
    ofs_cpp << std::endl 
            << "    ps::sum<"+e->type+">("+cname+", [](const "+nbr_t+" &_)" 
            << std::endl 
            << "    {return "; 
  }

  string varname = e->value.substr(0, dot),
         varloc  = e->value.substr(dot+1, e->value.length());
  ofs_cpp << "_."+varloc+"->"+varname+"()";

  if(top) { ofs_cpp << ";})" << std::endl; }
}
  
void 
Cpp::emit_InterlateFuncallAtom(const meta::FuncallAtom *e, 
                               const meta::Interlate *i,
                               const meta::Node *n)
{
  emit_InterlateFuncall(e->value, i, n);
}

void 
Cpp::emit_InterlateFuncall(const meta::Funcall *e, 
                           const meta::Interlate *il, 
                           const meta::Node *n)
{
  ofs_cpp << e->name << "(";
  for(size_t i = 0; i<e->args.size()-1; ++i)
  {
    emit_InterlateExpr(e->args[i], il, n);
    ofs_cpp << ", ";
  }
  emit_InterlateExpr(e->args[e->args.size()-1], il, n);
  ofs_cpp << ")";

}

bool
Cpp::hasRemoteRefs(const meta::Expr *e)
{
  using K = meta::Expr::Kind;
  switch(e->kind())
  {
    case K::AddOp : 
      return hasRemoteRefs(dynamic_cast<const meta::AddOp*>(e)->l)
          || hasRemoteRefs(dynamic_cast<const meta::AddOp*>(e)->r);
      break;

    case K::MulOp :
      return hasRemoteRefs(dynamic_cast<const meta::MulOp*>(e)->l)
          || hasRemoteRefs(dynamic_cast<const meta::MulOp*>(e)->r);
      break;

    case K::ExpOp :
      return hasRemoteRefs(dynamic_cast<const meta::ExpOp*>(e)->l)
          || hasRemoteRefs(dynamic_cast<const meta::ExpOp*>(e)->r);
      break;

    case K::Real :
      return false;
      break;

    case K::Symbol :
      return (dynamic_cast<const meta::Symbol*>(e)->value.find('.') 
                != string::npos);
      break;

    case K::Funcall :
      {
        const meta::Funcall *f = 
          dynamic_cast<const meta::FuncallAtom*>(e)->value;
        bool result{false};
        for(const meta::Expr *e : f->args) 
        { 
          result = result || hasRemoteRefs(e); 
        }
        return result;
      }
      break;

    default : return false;
  }
}

bool
Cpp::isComplex(const meta::Expr *e)
{
  using K = meta::Expr::Kind;
  switch(e->kind())
  {
    case K::AddOp : 
      return isComplex(dynamic_cast<const meta::AddOp*>(e)->l)
          || isComplex(dynamic_cast<const meta::AddOp*>(e)->r);
      break;

    case K::MulOp :
      return isComplex(dynamic_cast<const meta::MulOp*>(e)->l)
          || isComplex(dynamic_cast<const meta::MulOp*>(e)->r);
      break;

    case K::ExpOp :
      return isComplex(dynamic_cast<const meta::ExpOp*>(e)->l)
          || isComplex(dynamic_cast<const meta::ExpOp*>(e)->r);
      break;

    case K::Real :
      return false;
      break;

    case K::Symbol :
      return dynamic_cast<const meta::Symbol*>(e)->type == "complex"; 
      break;

    case K::Funcall :
      {
        const meta::Funcall *f = 
          dynamic_cast<const meta::FuncallAtom*>(e)->value;
        bool result{false};
        for(const meta::Expr *e : f->args) 
        { 
          result = result || hasRemoteRefs(e); 
        }
        return result;
      }
      break;

    default : return false;
  }
}
