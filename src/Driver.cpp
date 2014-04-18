#include <iostream>
#include "ASTNode.hpp"

extern Decls *mm;
extern int yyparse();

int main()
{
  yyparse();
  std::cout << mm << std::endl;
  return 0;
}
