#ifndef _PS_METASEMA_
#define _PS_METASEMA_

#include "MetaAST.hpp"
#include "ModelAST.hpp"
#include "FileUtil.hpp"
#include "CompilationException.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>

extern ps::meta::Module *mm;
extern ps::model::Model *mdl;

namespace ps {

struct ModuleFragment 
{ 
  ModuleFragment(meta::Module *m, std::string fn) : m{m}, filename{fn} {}
  meta::Module *m; std::string filename; 
};

struct ShellFragment
{
  ShellFragment(model::Commands *c, std::string fn) : c{c}, filename{fn} {}
  model::Commands *c;
  std::string filename;
};

class Sema
{
  public:
    Sema(std::vector<std::string> source_files) : source_files{source_files} {}
    void check();


  private:
    std::vector<std::string> source_files;
    std::string curr_file;
    std::vector<Diagnostic> diagnostics;

    std::unordered_map<std::string, 
                       std::vector<ModuleFragment>
                      > module_source_map;

    std::unordered_map<std::string, 
                       std::vector<ShellFragment>
                      > cmd_source_map;

    meta::Module* buildMetaAst(const std::string&);
    model::Model* buildShellAst(const std::string&);
    
    void check(const meta::Module*);

    //Node checks
    void check(const meta::Element*, const meta::Module*);
    void check(const meta::Link*);

    void checkFor_DuplicateNames(const meta::Element*);
    
    //Invalid Reference checking
    void checkFor_InvalidReferences(const meta::Element*, const meta::Module*);
    void checkFor_InvalidReferences(const meta::LazyVar*, const meta::Element*);
    void checkFor_InvalidReferences(const meta::DiffRel*, const meta::Element*);
    void checkFor_InvalidReferences(const meta::Accessor*, const meta::Element*);

    void checkFor_InvalidReferences(const meta::FuncallAtom*, 
                                    const meta::Element*);

    void checkFor_InvalidReferences(const meta::Funcall*, const meta::Element*);
    void checkFor_InvalidReferences(const meta::Symbol*, const meta::Element*);


    void checkFor_InvalidReferences(const meta::Interlate*, 
                                    const meta::Node*, const meta::Module*);

    void checkFor_InvalidReferences(const meta::Eqtn*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Expr*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::AddOp*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::MulOp*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::ExpOp*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Real*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Symbol*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::FuncallAtom*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Funcall*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::ExprAtom*, const meta::Element*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    //Model command checks
    void check(model::Model*);

    std::vector<ModuleFragment> check_Import(const model::Import*);
    void check_Create(model::Create*, 
                      const std::vector<ModuleFragment>&);

    void check_CreateType(model::Create*,
                          const std::vector<ModuleFragment>&);

    void check_CreateParamsLegit(model::Create*);
    void check_CreateRequiredParams(model::Create*);
    void check_CreateArgsParamList(model::Create*);
    void check_ConnectReferences(model::Connect*, model::Model*);
    void check_ConnectionReferences(model::Connection*, model::Model*);
    void check_ConnectInterlate(model::Connect*);
    void check_ConnectInterlate(model::Connection*);
    

    void undefined_Var(const std::string&, const meta::Lexeme*);
};

} //::ps

#endif
