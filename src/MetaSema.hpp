#ifndef _PS_METASEMA_
#define _PS_METASEMA_

#include "MetaAST.hpp"

#include <vector>
#include <string>

namespace ps { namespace meta {

class Sema
{
  public:
    //Returns info, warning and error strings from the semantic analysis
    void check(const Module*);

  private:
    std::vector<std::string> diagnostics;
    
    void check(const Node*);
    void checkFor_DuplicateVarNames(const Variables &v);

    void check(const Link*);
};

}} //ps::meta

#endif
