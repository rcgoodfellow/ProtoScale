#include "CommandAST.hpp"
#include "MetaAST.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"
#include "MetaASTPrinter.hpp"

using namespace ps;
using namespace ps::cmd;


extern meta::Module *mm;

FileSet BuildASTCommand::operator()() const
{
  FileSet fs;

  for(const std::string &s : *args)
  {
    std::string source = readFile(s); 
    metayy_scan_string(source.c_str());
    metayyparse();
    if(mm == NULL)
    {
      throw std::runtime_error("compilation failed for "+s);
    }
    meta::ASTPrinter pp;
    std::cout << "AST for " + s << std::endl;
    std::string ast = pp.print(mm);
    std::cout << ast;
  }


  return fs;
}
