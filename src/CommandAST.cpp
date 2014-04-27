#include "CommandAST.hpp"
#include "MetaAST.hpp"
#include "MetaParser.hpp"
#include "MetaScanner.hpp"
#include "MetaASTPrinter.hpp"
#include "Sema.hpp"
#include "CppGen.hpp"

using namespace ps;
using namespace ps::cmd;


extern meta::Module *mm;

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

    meta::ASTPrinter pp;
    std::string out_fn = s + ".psast";
    std::ofstream out(out_fn);
    std::cout << "AST for " << s << " saved to " << out_fn << std::endl;

    std::string ast = pp.print(mm);
    fs.push_back(out_fn);
    out << ast;
    out.close();
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

  gen::Cpp generator;
  generator.emit_Module(mm);

  return fs;
}
