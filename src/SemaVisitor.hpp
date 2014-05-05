#ifndef _PS_SEMAVISITOR_
#define _PS_SEMAVISITOR_

#include "MetaAST.hpp"
#include "ModelAST.hpp"
#include "FileUtil.hpp"
#include "Diagnostics.hpp"

#include <unordered_set>

extern ps::meta::ModuleFragment *mm;
extern ps::model::ModelFragment *mdl;

namespace ps {

  using namespace meta;
  using namespace model;
  
  struct Scope : meta::DefinitionContext
  {
    std::vector<meta::DefinitionContext*> contexts;

    virtual std::unordered_map<std::string, meta::Definition*> 
      definitions() override;

    virtual meta::Definition* definition(std::string) override;
  };
  
  struct Sema;

  struct MetaSemaVisitor :  public meta::Visitor
  {
    std::vector<Diagnostic> diagnostics;
    std::string filename;
    DefinitionContext *ctx, *saved_ctx;


    void visit(ModuleFragment*) override; 
    void leave(ModuleFragment*) override;

    void visit(Node*) override;
    void leave(Node*) override;

    void visit(Link*) override; 
    void leave(Link*) override;

    void visit(Interlate*) override; 
    void leave(Interlate*) override;

    void visit(Symbol*) override; 

    //expression typing bottom up traversal
    void leave(Symbol*) override;
    void leave(FunctionCall*) override;
    void leave(BinaryOperation*) override;
    void leave(Alias*) override;
  };

  struct ModelSemaVisitor : public model::Visitor
  {
    std::vector<Diagnostic> diagnostics;
    std::string filename;
    Sema *sema; 
    ModelFragment *model;
    std::unordered_map<std::string, Create*> names;


    virtual void visit(Import*) override; 
    virtual void visit(Create*) override; 
    virtual void visit(Connect*) override; 
  };


  
  struct Sema
  {
    Sema(std::vector<std::string> source_files) : source_files{source_files} 
    {
      model_visitor.sema = this;
    }

    void check();

    std::vector<std::string> source_files;
    std::string curr_file;
    MetaSemaVisitor meta_visitor; 
    ModelSemaVisitor model_visitor;

    meta::ModuleFragment* buildMetaAst(const std::string&);
    model::ModelFragment* buildModelAst(const std::string&);

    meta::Element* element(std::string name);
  
    std::unordered_map<std::string, 
                        std::vector<meta::ModuleFragment*>
                      > module_fragment_map;

    std::unordered_map<std::string,
                        std::vector<model::ModelFragment*>
                      > model_fragment_map;
  };

}

#endif
