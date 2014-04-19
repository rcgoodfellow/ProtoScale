#ifndef _PS_METAASTPRINTER_
#define _PS_METAASTPRINTER_

#include "MetaAST.hpp"
#include <sstream>
#include <string>

class Indentation
{
  size_t _at{0};  
  size_t _width{0};

  public:
    Indentation(size_t width=2) : _width{width} {}
    Indentation & operator ++()    { _at += _width; return *this; }
    Indentation & operator ++(int) { _at += _width; return *this; }
    Indentation & operator --()    { _at -= _width; return *this; }
    Indentation & operator --(int) { _at -= _width; return *this; }

    size_t width() { return _width; }
    size_t at() { return _at; }
    size_t pos() { return _at * _width; }

};

class MetaASTPrinter
{
  public:
    std::string print(const ASTNode *n); 

  private:
    Indentation indent{};
    std::stringstream ss{};
    void print(std::string);
    void print(const Ident*);
    void print(const Module*);
    void print(const Decl*);
    void print(const NodeDecl*);
    void print(const LinkDecl*);

};

template <class To, class From>
To* as(From *p)
{
  return dynamic_cast<To*>(p);
}

#endif
