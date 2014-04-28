#ifndef _PS_GEN_POWER_MODULE_
#define _PS_GEN_POWER_MODULE_


#include <complex>


using complex = std::complex<double>;
using real = double;


struct Bus; 
struct Generator; 
struct Source; 
struct Load; 
struct Line; 


struct Bus
{
  complex S;
  complex V;
  static complex Y;


  inline real p() const
  {
    return S.real()
  }
  inline void p(real v)
  {
    S.real(v)
  }

  inline real q() const
  {
    return S.imag()
  }
  inline void q(real v)
  {
    S.imag(v)
  }

  inline real v() const
  {
    return std::abs(V)
  }
  inline void v(real v)
  {
    real mag = std::abs(V);
    real angle = std::arg(V);
    V = std::polar(mag+v, angle);
  }

  inline real va() const
  {
    return std::arg(V)
  }
  inline void va(real v)
  {
    real mag = std::abs(V);
    real angle = std::arg(V);
    V = std::polar(mag, angle+v);
  }

  inline real y() const
  {
    return std::abs(Y)
  }
  inline void y(real v)
  {
    real mag = std::abs(Y);
    real angle = std::arg(Y);
    Y = std::polar(mag+v, angle);
  }

  inline real ya() const
  {
    return std::arg(Y)
  }
  inline void ya(real v)
  {
    real mag = std::abs(Y);
    real angle = std::arg(Y);
    Y = std::polar(mag, angle+v);
  }

  inline real g() const
  {
    return Y.real()
  }
  inline void g(real v)
  {
    Y.real(v)
  }

  inline real b() const
  {
    return Y.imag()
  }
  inline void b(real v)
  {
    Y.imag(v)
  }

};

struct Generator
{
  complex S;


  inline real p() const
  {
    return S.real()
  }
  inline void p(real v)
  {
    S.real(v)
  }

  inline real q() const
  {
    return S.imag()
  }
  inline void q(real v)
  {
    S.imag(v)
  }

};

struct Source
{
  real f;
  real p;


};

struct Load
{
  complex S;
  real fp;


  inline real p() const
  {
    return S.real()
  }
  inline void p(real v)
  {
    S.real(v)
  }

  inline real q() const
  {
    return S.imag()
  }
  inline void q(real v)
  {
    S.imag(v)
  }

};

struct Line
{
  static complex Y;


  inline real y() const
  {
    return std::abs(Y)
  }
  inline void y(real v)
  {
    real mag = std::abs(Y);
    real angle = std::arg(Y);
    Y = std::polar(mag+v, angle);
  }

  inline real ya() const
  {
    return std::arg(Y)
  }
  inline void ya(real v)
  {
    real mag = std::abs(Y);
    real angle = std::arg(Y);
    Y = std::polar(mag, angle+v);
  }

};



#endif