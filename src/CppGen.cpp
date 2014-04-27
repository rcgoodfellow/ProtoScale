#include "CppGen.hpp"

using namespace ps;
using namespace ps::gen;

using std::string;

void
Cpp::emit_Module(const meta::Module *m)
{
  ofs = std::ofstream{m->name + ".pmm.hpp", std::ofstream::out};  
  module = m;
  emit_OpenGuards();
  emit_Includes();
  emit_Usings();
  
  for(const meta::Node *n : module->nodes) { emit_NodeForward(n); }
  for(const meta::Link *n : module->links) { emit_LinkForward(n); }
  ofs << std::endl << std::endl;

  for(const meta::Node *n : module->nodes) { emit_NodeStruct(n); }
  for(const meta::Link *n : module->links) { emit_LinkStruct(n); }

  emit_CloseGuards();
  ofs.close();
}

void 
Cpp::emit_OpenGuards()
{
  string upper_mname; 
  std::transform(module->name.begin(), module->name.end(), 
                 std::back_inserter(upper_mname), ::toupper);

  ofs << "#ifndef _PS_GEN_" << upper_mname << "_MODULE_" << std::endl
      << "#define _PS_GEN_" << upper_mname << "_MODULE_" << std::endl
      << std::endl << std::endl;
}

void
Cpp::emit_Includes()
{
  ofs << "#include <complex>" << std::endl
      << std::endl << std::endl;
}

void
Cpp::emit_Usings()
{
  ofs << "using complex = std::complex<double>;" << std::endl
      << "using real = double;" << std::endl
      << std::endl << std::endl;
}
  
void 
Cpp::emit_CloseGuards()
{
  ofs << std::endl << std::endl
      << "#endif";
}
void
Cpp::emit_NodeForward(const meta::Node *n)
{
  ofs << "struct " << n->name << "; "
      << std::endl;
}

void
Cpp::emit_LinkForward(const meta::Link *l)
{
  ofs << "struct " << l->name << "; "
      << std::endl;
}

void
Cpp::emit_NodeStruct(const meta::Node *n)
{
  ofs << "struct " << n->name
      << std::endl
      << "{" << std::endl;

  emit_ElementVars(n);
  emit_ElementAliases(n);

  ofs << "};" << std::endl << std::endl;
}

void
Cpp::emit_LinkStruct(const meta::Link *l)
{
  ofs << "struct " << l->name 
      << std::endl
      << "{" << std::endl;
  
  emit_ElementVars(l);
  emit_ElementAliases(l);

  ofs << "};" << std::endl << std::endl;
}

void
Cpp::emit_ElementVars(const meta::Element *e)
{
  for(const meta::Variable *v : e->vars)
  {
    ofs << "  " << v->type << " " << v->name << ";" << std::endl;
  }
  ofs << std::endl << std::endl;
}

void
Cpp::emit_ElementAliases(const meta::Element *e)
{
  for(const meta::Alias *a : e->aliases)
  {
    ofs << "  " << "real " << a->name << "() const { }" << std::endl;
    ofs << "  " << "void " << a->name << "(real v) { }" << std::endl 
        << std::endl;
  }
}
