#ifndef _PS_METAMODEL_
#define _PS_METAMODEL_

#include <stddef.h>

// elemental data types -------------------------------------------------------
// ----------------------------------------------------------------------------
namespace ps {

struct Node;
struct Link;

typedef void (*interlate(const Link&, const Node&, Node&));

template<class T>
struct Glob 
{ 
  T *data{nullptr};
  size_t sz{0};
};

struct Var { unsigned id; double value; };
struct Stmt { };
struct Alias { unsigned id; Stmt s; };
struct Expr { };

}//::ps

// core data types ------------------------------------------------------------
// ----------------------------------------------------------------------------
namespace ps {

struct Link
{
  Glob<Var> info;
};

struct Node
{
  Glob<Var> info;
  Glob<Alias> aliases;
  Glob<interlate> interlates;
  Glob<Node*> neighbors;

  void step();
};

}//::ps

#endif
