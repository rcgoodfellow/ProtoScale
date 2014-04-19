#ifndef _PS_METAASTPRINTER_
#define _PS_METAASTPRINTER_

#include "MetaAST.hpp"
#include <string>

class MetaASTPrinter
{
  public:
    std::string print(ASTNode *n); //TODO: you are here, implement these in a .cpp

  private:
    std::string print(Ident *id);

};

#endif
