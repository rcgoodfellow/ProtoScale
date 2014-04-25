#ifndef _PS_METASEMA_
#define _PS_METASEMA_

#include "MetaAST.hpp"
#include "ShellAST.hpp"
#include "FileUtil.hpp"

#include <vector>
#include <string>
#include "CompilationException.hpp"

extern ps::meta::Module *mm;
extern ps::shell::Commands *sh_cmds;

namespace ps {

class Sema
{
  public:
    Sema(std::vector<std::string> source_files) : source_files{source_files} {}
    void check();


  private:
    std::vector<std::string> source_files;
    std::string curr_file;
    std::vector<Diagnostic> diagnostics;

    void buildMetaAst(const std::string&);
    void buildShellAst(const std::string&);
    
    void check(const meta::Module*);

    //Node checks
    void check(const meta::Node*, const meta::Module*);
    void checkFor_DuplicateNames(const meta::Node*);

    void checkFor_InvalidReferences(const meta::Node*, 
                                    const meta::Module*);

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

    void undefined_Var(const std::string&, const meta::Lexeme*);
};

} //::ps

#endif
