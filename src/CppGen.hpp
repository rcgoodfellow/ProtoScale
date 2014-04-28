#ifndef _PS_CPPGEN_
#define _PS_CPPGEN_

#include "MetaAST.hpp"
#include "ModelAST.hpp"

#include <fstream>
#include <algorithm>

namespace ps { namespace gen {

struct Cpp
{
  std::ofstream ofs;
  const meta::Module *module;

  void emit_Module(const meta::Module*);
  void emit_OpenGuards();
  void emit_CloseGuards();
  void emit_Includes();
  void emit_Usings();
  
  void emit_NodeForward(const meta::Node*);
  void emit_LinkForward(const meta::Link*);

  void emit_NodeStruct(const meta::Node*);
  void emit_LinkStruct(const meta::Link*);

  void emit_ElementVars(const meta::Element*);
  void emit_ElementAliases(const meta::Element*);
  void emit_NodeInterlates(const meta::Node*);



  void emit_Model(const model::Model*);
};

}} // ps::gen

#endif
