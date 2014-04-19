#include "CommandAST.hpp"

#include "MetaAST.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"
#include "MetaASTPrinter.hpp"

extern Module *mm;

FileSet BuildASTCommand::operator()() const
{
  FileSet fs;

  std::cout << "I am AST Builder hear me roar!" << std::endl;

  std::vector<std::string> input_files;
  for(const std::string &s : *args)
  {
    input_files.push_back(readFile(s)); 
  }

  MetaASTPrinter pp;
  for(const std::string &s : input_files)
  {
    metayy_scan_string(s.c_str());
    metayyparse();
    if(mm == NULL)
    {
      throw std::runtime_error("compilation failed");
    }
  }
  //pp.print(

  return fs;
}
