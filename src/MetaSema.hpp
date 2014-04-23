#ifndef _PS_METASEMA_
#define _PS_METASEMA_

#include "MetaAST.hpp"
#include "FileUtil.hpp"

#include <vector>
#include <string>
#include "CompilationException.hpp"

extern ps::meta::Module *mm;

namespace ps { namespace meta {

class Sema
{
  public:
    Sema(std::vector<std::string> source_files) : source_files{source_files} {}
    void check();


  private:
    std::vector<std::string> source_files;
    std::string curr_file;
    std::vector<Diagnostic> diagnostics;

    void buildAst(const std::string&);
    
    void check(const Module*);

    //Node checks
    void check(const Node*);
    void checkFor_DuplicateVarNames(const Variables&);
    void checkFor_InvalidReferences(const Node*);
    void checkFor_InvalidReferences(const FuncallAtom*, const Node*);
    void checkFor_InvalidReferences(const Funcall*, const Node*);
    void checkFor_InvalidReferences(const Expr*, const Node*);
    void checkFor_InvalidReferences(const Symbol*, const Node*);

    void check(const Link*);
};

}} //ps::meta

#endif
