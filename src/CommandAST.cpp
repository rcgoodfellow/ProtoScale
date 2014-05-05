#include "CommandAST.hpp"
#include "MetaAST.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"
#include "SemaVisitor.hpp"
#include "CppGen.hpp"

using namespace ps;
using namespace ps::cmd;


extern meta::ModuleFragment *mm;

void doBuildAST(const std::string &src)
{
  mm = nullptr;
  std::string source = readFile(src); 
  metayy_scan_string(source.c_str());
  metayyparse();
  if(mm == nullptr)
  {
    throw std::runtime_error("compilation failed for "+src);
  }
}

FileSet BuildASTCommand::operator()() const
{
#ifdef DEBUG
  std::cout << "Building AST" << std::endl;
#endif

  FileSet fs;

  for(const std::string &s : *args)
  {
    doBuildAST(s);
  }

  return fs;
}

FileSet BuildPKGCommand::operator()() const
{
#ifdef DEBUG
  std::cout << "Building Package" << std::endl;
#endif

  FileSet fs;

  Sema semanticChecker(*args);
  semanticChecker.check();

  gen::cpp::Generator generator;
  generator.emitModule(mm);
  //gen::Cpp generator;
  //generator.emit_Module(mm);

  return fs;
}
