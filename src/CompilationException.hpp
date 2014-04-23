#ifndef _PS_COMPILATION_EXCEPTION_
#define _PS_COMPILATION_EXCEPTION_

#include <stdexcept>
#include <vector>
#include <string>

struct Diagnostic 
{ 
  Diagnostic(std::string f, size_t l, std::string m) 
    : filename{f}, line_no{l}, msg{m} {}

  std::string filename;
  size_t line_no; 
  std::string msg; 
};

struct compilation_error : std::runtime_error
{
  compilation_error(std::vector<Diagnostic> diagnostics)
    : std::runtime_error{""}, diagnostics{diagnostics}
  {
    _what = "";
    for(const Diagnostic &d : diagnostics)
    {
      _what += d.filename + "["+std::to_string(d.line_no)+"] " + d.msg + "\n";
    }
  }

  const char* what() const _NOEXCEPT override
  {
     return _what.c_str();
  }

  std::vector<Diagnostic> diagnostics;
  
  private:
    std::string _what{"compilation exception"};
};

#endif
