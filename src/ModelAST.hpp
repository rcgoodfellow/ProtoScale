#ifndef _PS_SHELLAST_
#define _PS_SHELLAST_

#include "MetaAST.hpp"
#include <vector>
#include <string>

namespace ps { namespace model {

struct Visitor;

struct ASTNode
{
  virtual ~ASTNode() {}
  virtual void accept(Visitor&) = 0;
};

struct Command;
struct ModelFragment;
struct Import;
struct Create;
struct Connect;

struct Visitor
{
  virtual void visit(ModelFragment*){}
  virtual void leave(ModelFragment*){}
  virtual void visit(Command*){}
  virtual void leave(Command*){}
  virtual void visit(Import*){}
  virtual void leave(Import*){}
  virtual void visit(Create*){}
  virtual void leave(Create*){}
  virtual void visit(Connect*){}
  virtual void leave(Connect*){}
};

struct Lexeme
{
  Lexeme(size_t l) : _line_no{l} {}
  size_t line_no() const { return _line_no; }
  
  private:
    size_t _line_no;
};

struct Command : public ASTNode
{
  enum class Kind { Import, Create, Connect };

  Command(Kind k) : _kind{k} {}
  virtual ~Command() {}

  Kind kind() const { return _kind; } 

  private:
    Kind _kind;
};
using Commands = std::vector<Command*>;

struct ModelFragment : public Lexeme, ASTNode
{
  std::string name;
  Commands *commands;

  ModelFragment(std::string n, Commands *c, size_t l) 
    : Lexeme{l}, name{n}, commands{c} {}
  
  void accept(Visitor&) override;
  Create* find(const std::string &name);
};

struct Import : public Command, public Lexeme
{
  std::string module_name;

  Import(std::string mn, size_t line_no) 
    : Command{Kind::Import}, Lexeme{line_no}, module_name{mn} {}

  void accept(Visitor&) override;
};
using Imports = std::vector<Import*>;

struct CreateFormat : public Lexeme
{
  std::vector<std::string> *var_names;

  CreateFormat(size_t line_no,
      std::vector<std::string> *vn = new std::vector<std::string>()) 
    : Lexeme{line_no}, var_names{vn} {}
};

struct CreateArgs : public Lexeme
{
  std::vector<std::string> vals;

  CreateArgs(size_t line_no) : Lexeme{line_no} {}
};

struct CreateTarget : public Lexeme
{
  std::string name;
  CreateArgs *args;

  CreateTarget(std::string n, CreateArgs *a, size_t line_no)
    : Lexeme{line_no}, name{n}, args{a} {}
};
using CreateTargets = std::vector<CreateTarget*>;

struct Create : public Command, public Lexeme
{
  std::string type_tgt;
  meta::Element *type{nullptr};
  CreateFormat *fmt;
  CreateTargets *tgts;

  Create(std::string tt, CreateFormat *cf, CreateTargets *ts, size_t line_no) 
    : Command{Kind::Create}, Lexeme{line_no}, type_tgt{tt}, fmt{cf}, tgts{ts} {}

  void accept(Visitor&) override;
};
using Creates = std::vector<Create*>;

struct Connection : public Lexeme
{
  std::string a, interlate, b, via;
  Create *ap{nullptr}, *bp{nullptr}, *viap{nullptr};
  bool symmetric{false};

  Connection(std::string a, 
             std::string i, 
             std::string b, 
             std::string via, 
             size_t line_no)
    : Lexeme{line_no}, a{a}, interlate{i}, b{b}, via{via} {}

};
using Connections = std::vector<Connection*>;

struct Connect : public Command, public Lexeme
{
  Connections *connections;
  bool symmetric;

  Connect(Connections *c, bool sym, size_t line_no) 
    : Command{Kind::Connect}, Lexeme{line_no}, connections{c}, symmetric{sym} 
  {
    if(symmetric)
    {
      for(Connection *cn : *connections) { cn->symmetric = true; }
    }
  }
  
  void accept(Visitor&) override;
};
using Connects = std::vector<Connect*>;


}} // ps::shell

#endif
