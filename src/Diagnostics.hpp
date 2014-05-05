#ifndef _PS_COMPILATION_EXCEPTION_
#define _PS_COMPILATION_EXCEPTION_

#include <stdexcept>
#include <vector>
#include <string>

struct Diagnostic 
{
  enum class Kind{ Error, Warning, Info };

  Diagnostic(std::string f, size_t l, std::string m, Kind k=Kind::Error) 
    : filename{f}, line_no{l}, msg{m}, _kind{k} {}

  Kind kind() const { return _kind; }

  std::string filename;
  size_t line_no; 
  std::string msg; 

  private:
    Kind _kind;
};

struct compilation_error : std::runtime_error
{
  compilation_error(Diagnostic d)
    : compilation_error{std::vector<Diagnostic>{d}} {}

  compilation_error(std::vector<Diagnostic> diagnostics)
    : std::runtime_error{""}, diagnostics{diagnostics} { }

  const char* what() const _NOEXCEPT override
  {
    std::string _what = "";
    using K = Diagnostic::Kind;
    for(const Diagnostic &d : diagnostics)
    {
      switch(d.kind())
      {
        case K::Error: _what += "error: "; break;
        case K::Warning: _what += "warning: "; break;
        case K::Info: _what += "  info: "; break;
      }
      std::string short_fn = d.filename.substr(d.filename.find_last_of("/")+1, 
                                               d.filename.length());
      _what += short_fn + "["+std::to_string(d.line_no)+"] " + d.msg + "\n";
    }
    return _what.c_str();
  }

  std::vector<Diagnostic> diagnostics;
  
  private:
};

#endif
