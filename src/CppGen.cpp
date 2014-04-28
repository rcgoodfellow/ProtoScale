#include "CppGen.hpp"

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
  return "return this->_" + varname + "_.real()";
}
string re_setter(string varname, string argname)
{
  return "this->_"+varname+"_.real("+argname+")";
}

string im_getter(string varname)
{
  return "return this->_" + varname + "_.imag()";
}
string im_setter(string varname, string argname)
{
  return "this->_"+varname+"_.imag("+argname+")";
}

string mag_getter(string varname)
{
  return "return std::abs(this->_"+varname+"_)";
}
string mag_setter(string varname, string argname, string indent)
{
  return
    indent + "real angle = std::arg(this->_"+varname+"_);\n" +
    indent + "this->_"+varname+"_ = std::polar("+argname+", angle);";
}

string angle_getter(string varname)
{
  return "return std::arg(this->_"+varname+"_)";
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
    ofs_hpp << "  std::vector<double> " << i->name + "_lob;" << std::endl;
    ofs_hpp << "  std::vector<std::pair<"+lnk->type+"*, "+nod->type+"*>> "
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
