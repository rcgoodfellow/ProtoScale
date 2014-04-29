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
    
    void check(meta::Module*);

    //Node checks
    void check(meta::Element*, meta::Module*);
    void check(meta::Link*);

    void checkFor_DuplicateNames(meta::Element*);
    
    //Invalid Reference checking
    void checkFor_InvalidReferences(meta::Element*, meta::Module*);
    void checkFor_InvalidReferences(meta::LazyVar*, meta::Element*);
    void checkFor_InvalidReferences(meta::DiffRel*, meta::Element*);
    void checkFor_InvalidReferences(meta::Accessor*, meta::Element*);

    void checkFor_InvalidReferences(meta::FuncallAtom*, meta::Element*);

    void checkFor_InvalidReferences(meta::Funcall*, meta::Element*);
    void checkFor_InvalidReferences(meta::Symbol*, meta::Element*);


    void checkFor_InvalidReferences(meta::Interlate*, meta::Node*, meta::Module*);

    void checkFor_InvalidReferences(meta::Eqtn*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::Expr*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::AddOp*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::MulOp*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::ExpOp*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::Real*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::Symbol*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::FuncallAtom*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::Funcall*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

    void checkFor_InvalidReferences(meta::ExprAtom*, meta::Element*, 
                                    std::string, meta::Link*, 
                                    std::string, meta::Node*);

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
