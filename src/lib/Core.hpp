#ifndef _PS_CORE_
#define _PS_CORE_

#include <vector>
#include <complex>
#include <functional>

namespace ps {

//Forward Decl
struct Element;
struct Node;
struct Link;

using complex = std::complex<double>;

struct Alias
{
  using Setter = std::function<complex(double)>;
  using Getter = std::function<double(complex)>;

  complex *tgt;
  Setter set;
  Getter get;

  Alias(complex *t, Setter s, Getter g) : tgt{t}, set{s}, get{g} {}
};

template <typename T>
using LazyVar = std::function<T(Element*)>;

using LazyReal = LazyVar<double>;
using LazyComplex = LazyVar<complex>;

using Interlate = std::function<void(Node*, Link*, Node*)>;

struct Element
{
  std::vector<complex> complex_vars; 
  std::vector<double> real_vars;
  std::vector<Alias> aliases;
  std::vector<LazyReal> lazy_reals;
  std::vector<LazyComplex> lazy_complexes;
};

struct Node : public Element
{
  std::vector<Interlate> interlates;
};

struct Link : public Element
{

};


} //::ps

#endif

