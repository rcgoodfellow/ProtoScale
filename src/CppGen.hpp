#ifndef _PS_CPPGEN_
#define _PS_CPPGEN_

#include "MetaAST.hpp"
#include "ModelAST.hpp"

#include <fstream>
#include <algorithm>

namespace ps { namespace gen {

struct Cpp
{
  std::ofstream ofs_hpp;
  std::ofstream ofs_cpp;
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

  void emit_TimeFunc();

  void emit_ElementVars(const meta::Element*);
  void emit_ElementAliases(const meta::Element*);
  void emit_NodeInterlates(const meta::Node*);
  void emit_InterlateEqtn(const meta::Eqtn*, 
                          const meta::Interlate*,
                          const meta::Node*);

  void emit_InterlateExpr(const meta::Expr*, 
                          const meta::Interlate*,
                          const meta::Node*,
                          bool top=false);

  void emit_InterlateAddOp(const meta::AddOp*, 
                           const meta::Interlate*,
                           const meta::Node*, 
                           bool top);

  void emit_InterlateMulOp(const meta::MulOp*, 
                           const meta::Interlate*,
                           const meta::Node*,
                           bool top=false);

  void emit_InterlateExpOp(const meta::ExpOp*, 
                           const meta::Interlate*,
                           const meta::Node*);

  void emit_InterlateReal(const meta::Real*);

  void emit_InterlateSymbol(const meta::Symbol*, 
                            const meta::Interlate*,
                            const meta::Node*,
                            bool top=false);

  void emit_InterlateFuncallAtom(const meta::FuncallAtom*, 
                                 const meta::Interlate*,
                                 const meta::Node*);

  void emit_InterlateFuncall(const meta::Funcall*, 
                             const meta::Interlate*,
                             const meta::Node*);

  bool hasRemoteRefs(const meta::Expr*);
  bool isComplex(const meta::Expr*);

  void emit_Model(const model::Model*);
};

}} // ps::gen

#endif
