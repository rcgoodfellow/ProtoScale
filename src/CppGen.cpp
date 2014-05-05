#include "CppGen.hpp"

using namespace ps::gen::cpp;
using namespace ps::meta;

using std::string;

void
Generator::emit_hpp(string s)
{
  ofs_hpp << indent_hpp.get() << s << std::endl;
}
void
Generator::emit_hpp_newline(size_t n)
{
  for(size_t i=0; i<n; ++i){ ofs_hpp << std::endl; }
}

void
Generator::emit_cpp(string s)
{
  ofs_cpp << indent_cpp.get() << s << std::endl;
}

void
Generator::emit_cpp_newline(size_t n)
{
  for(size_t i=0; i<n; ++i){ ofs_cpp << std::endl; }
}
void
Generator::emitModule(ModuleFragment *m)
{
  m->accept(meta_visitor);
}

void
MetaVisitor::visit(ModuleFragment *m)
{
  /*.hpp file generation ++++++++++++++++++++++*/
  gen->ofs_hpp = std::ofstream{m->name + ".pmm.hpp", std::ofstream::out};  


  string upper_mname; 
  std::transform(m->name.begin(), m->name.end(), 
                 std::back_inserter(upper_mname), ::toupper);
 
  //Include Guards
  gen->emit_hpp("#ifndef _PS_GEN_"+upper_mname+"_MODULE_");
  gen->emit_hpp("#define _PS_GEN_"+upper_mname+"_MODULE_");
  gen->emit_hpp_newline(2);

  //Includes
  gen->emit_hpp("#include \"PSUtil.hpp\"");
  gen->emit_hpp("#include <complex>");
  gen->emit_hpp("#include <vector>");
  gen->emit_hpp("#include <utility>");
  gen->emit_hpp_newline(2);

  //Usings
  gen->emit_hpp("using complex = std::complex<double>;");
  gen->emit_hpp("using real = double;");
  gen->emit_hpp_newline(2);

  //Forward Declarations
  for(const auto &p : m->nodes){ gen->emit_hpp("struct "+p.first+";"); }
  gen->emit_hpp_newline(2);

  
  /*.cpp file generation ++++++++++++++++++++++*/
  gen->ofs_cpp = std::ofstream{m->name + ".pmm.cpp", std::ofstream::out};
  gen->emit_cpp("#include \""+m->name+".pmm.hpp\"");
  gen->emit_cpp_newline(2);
}

void
MetaVisitor::leave(ModuleFragment*)
{
  
  gen->emit_hpp("#endif");
  gen->ofs_hpp.close();
  gen->ofs_cpp.close();
}

void
MetaVisitor::visit(Element *e)
{
  /*.hpp file generation ++++++++++++++++++++++*/
  gen->emit_hpp("struct "+e->name);
  gen->emit_hpp("{");
  gen->indent_hpp++;

  for(const auto &p : e->variables)
  {
    Variable *v = p.second;
    gen->emit_hpp(v->type+" "+v->name+";");
  }
  gen->emit_hpp_newline(1);

  for(const auto &p : e->aliases)
  {
    Alias *a = p.second;
    gen->emit_hpp("real "+a->name+"();");
    gen->emit_hpp("void "+a->name+"(real v);");
    gen->emit_hpp_newline(1);
  }


  /*.cpp file generation ++++++++++++++++++++++*/
  for(const auto &p : e->aliases)
  {
    Alias *a = p.second;
    Symbol *s = dynamic_cast<Symbol*>(a->accessor->arguments[0]);
    
    //getters
    gen->emit_cpp("real "+e->name+"::"+a->name+"()");
    gen->emit_cpp("{");
    gen->indent_cpp++;
    if(a->accessor->name == "re")
    {
      gen->emit_cpp("return this->"+s->name+".real();");
    }
    if(a->accessor->name == "im")
    {
      gen->emit_cpp("return this->"+s->name+".imag();");
    }
    if(a->accessor->name == "mag")
    {
      gen->emit_cpp("return std::abs(this->"+s->name+");");
    }
    if(a->accessor->name == "angle")
    {
      gen->emit_cpp("return std::arg(this->"+s->name+");");
    }
    gen->indent_cpp--;
    gen->emit_cpp("}");
    gen->emit_cpp_newline(1);

    //setters
    gen->emit_cpp("void "+e->name+"::"+a->name+"(real v)");
    gen->emit_cpp("{");
    gen->indent_cpp++;
    if(a->accessor->name == "re")
    {
      gen->emit_cpp("this->"+s->name+".real(v);");
    }
    if(a->accessor->name == "im")
    {
      gen->emit_cpp("this->"+s->name+".imag(v);");
    }
    if(a->accessor->name == "mag")
    {
      gen->emit_cpp("real angle = std::arg(this->"+s->name+");");
      gen->emit_cpp("this->"+s->name+" = std::polar(v, angle);");
    }
    if(a->accessor->name == "angle")
    {
      gen->emit_cpp("real mag = std::abs(this->"+s->name+");");
      gen->emit_cpp("this->"+s->name+" = std::polar(mag, v);");
    }
    gen->indent_cpp--;
    gen->emit_cpp("}");
    gen->emit_cpp_newline(1);
  }
}

void
MetaVisitor::leave(Node*)
{
  gen->indent_hpp--;
  gen->emit_hpp("};");
  gen->emit_hpp_newline(1);
}
void
MetaVisitor::leave(Link*)
{
  gen->indent_hpp--;
  gen->emit_hpp("};");
  gen->emit_hpp_newline(1);
}

void
MetaVisitor::visit(Interlate *i)
{

  /*.hpp file generation ++++++++++++++++++++++*/
  
  //generate lob vectors
  gen->emit_hpp("std::vector<double> "+i->name+"_lob;");

  //neighbor struct
  string link_node_t = i->link_param->type+"_"+i->node_param->type+"_t";
  gen->emit_hpp("struct "+link_node_t);
  gen->emit_hpp("{");
  gen->indent_hpp++;
  gen->emit_hpp(i->link_param->type+" *"+i->link_param->name+";");
  gen->emit_hpp(i->node_param->type+" *"+i->node_param->name+";");
  gen->indent_hpp--;
  gen->emit_hpp("};");
  gen->emit_hpp("std::vector<"+link_node_t+"> "+i->name+"_nbrs;");
  gen->emit_hpp_newline(1);

  //interlate signatures
  for(Interlation *ix : i->body)
  {
    gen->emit_hpp(ix->type()+" "+i->name+"_"+ix->target->name+"();");
  }
  

  /*.cpp file generation ++++++++++++++++++++++*/

}
