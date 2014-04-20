#ifndef _PS_ASTNODE_
#define _PS_ASTNODE_

#include <string>
#include <vector>

struct Module;
struct Element;
using Elements = std::vector<Element*>;

struct Module
{
  std::string name;
  Elements elements; 
  Module(std::string n, Elements e) : name{n}, elements{e} {}
};

struct Element
{
  enum class Kind { Node, Link };
  Kind kind() { return _kind; }
  private:
    Kind _kind;
};

struct Variable 
{ 
  std::string name; 
  std::string type; 
  Variable(std::string n, std::string t) : name{n}, type{t} {}
};
using Variables = std::vector<Variable*>;

struct Node : public Element
{
  std::string name;
  Variables vars;
  Node(std::string n) : name{n} {}

};

struct Link : public Element
{
  std::string name;
  Link(std::string n) : name{n} {}
};

#endif
