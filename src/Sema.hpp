#ifndef _PS_METASEMA_
#define _PS_METASEMA_

#include "MetaAST.hpp"
#include "ShellAST.hpp"
#include "FileUtil.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include "CompilationException.hpp"

extern ps::meta::Module *mm;
extern ps::shell::Commands *sh_cmds;

namespace ps {

struct ModuleFragment 
{ 
  ModuleFragment(meta::Module *m, std::string fn) : m{m}, filename{fn} {}
  meta::Module *m; std::string filename; 
};

struct ShellFragment
{
  ShellFragment(shell::Commands *c, std::string fn) : c{c}, filename{fn} {}
  shell::Commands *c;
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
    shell::Commands* buildShellAst(const std::string&);
    
    void check(const meta::Module*);

    //Node checks
    void check(const meta::Node*, const meta::Module*);
    void checkFor_DuplicateNames(const meta::Node*);
    void checkFor_InvalidReferences(const meta::Node*, const meta::Module*);

    //Invalid Reference checking
    void checkFor_InvalidReferences(const meta::FuncallAtom*, 
                                    const meta::Node*);

    void checkFor_InvalidReferences(const meta::Funcall*, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Expr*, const meta::Node*);
    
    void checkFor_InvalidReferences(const meta::Symbol*, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Accessor*, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Interlate*, 
                                    const meta::Node*, const meta::Module*);

    void checkFor_InvalidReferences(const meta::Eqtn*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Expr*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::AddOp*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::MulOp*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::ExpOp*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Real*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Symbol*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::FuncallAtom*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::Funcall*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::ExprAtom*, const meta::Node*, 
                                    const std::string&, const meta::Link*, 
                                    const std::string&, const meta::Node*);

    void checkFor_InvalidReferences(const meta::LazyVar*, const meta::Node*);
    void checkFor_InvalidReferences(const meta::DiffRel*, const meta::Node*);

    void check(const meta::Link*);

    //Shell command checks
    void check(const shell::Commands*);

    void undefined_Var(const std::string&, const meta::Lexeme*);
};

} //::ps

#endif
