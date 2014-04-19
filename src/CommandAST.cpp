#include "CommandAST.hpp"
#include "MetaASTPrinter.hpp"

FileSet BuildASTCommand::operator()() const
{
  FileSet fs;

  std::cout << "I am AST Builder hear me roar!" << std::endl;

  return fs;
}
