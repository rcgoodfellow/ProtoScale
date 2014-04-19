
#include "FileUtil.hpp"

using std::string;
using std::ifstream;

string readFile(string filename)
{
  std::ifstream in(filename, std::ios::in);
  if(in)
  {
    string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
  }
  throw std::runtime_error("invalid file name '" + filename + "'");
}
