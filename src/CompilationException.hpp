#ifndef _PS_COMPILATION_EXCEPTION_
#define _PS_COMPILATION_EXCEPTION_

#include <stdexcept>
#include <vector>
#include <string>

struct Diagnostic 
{
  enum class Kind{ Error, Warning, Info };

  Diagnostic(std::string f, size_t l, std::string m, Kind k=Kind::Error) 
    : _kind{k}, filename{f}, line_no{l}, msg{m} {}

  Kind kind() const { return _kind; }

  std::string filename;
  size_t line_no; 
  std::string msg; 

  private:
    Kind _kind;
};

struct compilation_error : std::runtime_error
{
  compilation_error(std::vector<Diagnostic> diagnostics)
    : std::runtime_error{""}, diagnostics{diagnostics}
  {
    _what = "";
    using K = Diagnostic::Kind;
    for(const Diagnostic &d : diagnostics)
    {
      switch(d.kind())
      {
        case K::Error: _what += "error: "; break;
        case K::Warning: _what += "warning: "; break;
        case K::Info: _what += "  info: "; break;
      }
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
