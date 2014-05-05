#ifndef _PS_CPPGEN_
#define _PS_CPPGEN_

#include "MetaAST.hpp"
#include "ModelAST.hpp"

#include <unordered_set>
#include <fstream>
#include <algorithm>

#endif

namespace ps { namespace gen { namespace cpp {

struct Generator;

class Indentation
{
  size_t _at{0};  
  size_t _width{0};

  public:
    Indentation(size_t width=2) : _width{width} {}
    Indentation & operator ++()    { ++_at; return *this; }
    Indentation & operator ++(int) { _at++; return *this; }
    Indentation & operator --()    { --_at; return *this; }
    Indentation & operator --(int) { _at--; return *this; }

    size_t width() { return _width; }
    size_t at() { return _at; }
    size_t pos() { return _at * _width; }
    std::string get() { return std::string(pos(), ' '); }

};


struct MetaVisitor : public meta::Visitor
{
  Generator *gen;
  MetaVisitor(Generator *gen) : gen{gen} {}
  void visit(meta::ModuleFragment*) override; 
  void leave(meta::ModuleFragment*) override;
  void visit(meta::Element*) override;
  void leave(meta::Node*) override;
  void leave(meta::Link*) override;
  //void visit(meta::Node*) override;
  void visit(meta::Interlate*) override;
};

struct ModelVisitor : public meta::Visitor
{
  Generator *gen;
  ModelVisitor(Generator *gen) : gen{gen} {}
};

struct Generator
{
  std::unordered_set<std::string, std::vector<meta::ModuleFragment*>> modules; 
  std::unordered_set<std::string, std::vector<model::ModelFragment*>> models;

  MetaVisitor meta_visitor{this};
  ModelVisitor model_visitor{this};

  std::ofstream ofs_hpp;
  Indentation indent_hpp;

  std::ofstream ofs_cpp;
  Indentation indent_cpp;

  void emit_hpp(std::string);
  void emit_hpp_newline(size_t);
  void emit_cpp(std::string);
  void emit_cpp_newline(size_t);

  void emitModule(meta::ModuleFragment*);
};

}}}
