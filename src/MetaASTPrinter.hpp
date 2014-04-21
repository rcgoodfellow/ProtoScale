#ifndef _PS_METAASTPRINTER_
#define _PS_METAASTPRINTER_

#include "MetaAST.hpp"
#include "MetaParser.hpp"
#include <sstream>
#include <string>

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

};

class MetaASTPrinter
{
  public:
    //std::string print(const ASTNode *n); 
    std::string print(const Module*);

  private:
    Indentation indent{};
    std::stringstream ss{};
    void print(std::string);
    void print(const Node*);
    void print(const Link*);
    void print(const Variable*);
    void print(const Alias*);
    void print(const Expr*);
    
    void print(const AddOp*);
    void print(const MulOp*);
    void print(const ExpOp*);


    void print(const Atom*);
    void print(const Real*);
    void print(const Symbol*);
    void print(const ExprAtom*);
    void print(const FuncallAtom*);
    void print(const Funcall*);
    void print(const Interlate*);
    void print(const Eqtn*);

};

template <class To, class From>
To* as(From *p)
{
  return dynamic_cast<To*>(p);
}

#endif
