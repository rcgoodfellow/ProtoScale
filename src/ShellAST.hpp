#ifndef _PS_SHELLAST_
#define _PS_SHELLAST_

#include "MetaAST.hpp"
#include <vector>
#include <string>

namespace ps { namespace shell {

struct Lexeme
{
  Lexeme(size_t l) : _line_no{l} {}
  size_t line_no() const { return _line_no; }
  
  private:
    size_t _line_no;
};

struct Command
{
  enum class Kind { Import, Create, Connect };

  Command(Kind k) : _kind{k} {}
  virtual ~Command() {}

  Kind kind() const { return _kind; } 

  private:
    Kind _kind;
};
using Commands = std::vector<Command*>;

struct Import : public Command, public Lexeme
{
  std::string module_name;

  Import(std::string mn, size_t line_no) 
    : Lexeme{line_no}, Command{Kind::Import}, module_name{mn} {}
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
    : Lexeme{line_no}, Command{Kind::Create}, type_tgt{tt}, fmt{cf}, tgts{ts} {}
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
    : Lexeme{line_no}, Command{Kind::Connect}, connections{c}, symmetric{sym} 
  {
    if(symmetric)
    {
      for(Connection *cn : *connections)
      {
        cn->symmetric = true;
      }
    }
  }
};
using Connects = std::vector<Connect*>;


}} // ps::shell

#endif
