#include "MetaAST.hpp"

using namespace ps;
using namespace ps::meta;

const Variable*
Node::getVar(const std::string &s) const
{
  return
    *std::find_if(vars.begin(), vars.end(), 
        [&s](const Variable *v){ return v->name == s; });
}
