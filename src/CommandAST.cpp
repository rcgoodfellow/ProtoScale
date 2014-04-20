#include "CommandAST.hpp"

#include "MetaAST.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"
#include "MetaASTPrinter.hpp"

extern Module *mm;

FileSet BuildASTCommand::operator()() const
{
  FileSet fs;

#ifdef DEBUG
  std::cout << "I am AST Builder hear me roar!" << std::endl;
#endif

  for(const std::string &s : *args)
  {
    std::string source = readFile(s); 
    metayy_scan_string(source.c_str());
    metayyparse();
    if(mm == NULL)
    {
      throw std::runtime_error("compilation failed for "+s);
    }
    MetaASTPrinter pp;
    std::cout << "AST for " + s << std::endl;
    std::string ast = pp.print(mm);
    std::cout << ast;
  }


  return fs;
}
