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


  inline real p() const { }
  inline void p(real v) { }

  inline real q() const { }
  inline void q(real v) { }

  inline real v() const { }
  inline void v(real v) { }

  inline real va() const { }
  inline void va(real v) { }

  inline real y() const { }
  inline void y(real v) { }

  inline real ya() const { }
  inline void ya(real v) { }

  inline real g() const { }
  inline void g(real v) { }

  inline real b() const { }
  inline void b(real v) { }

};

struct Generator
{
  complex S;


  inline real p() const { }
  inline void p(real v) { }

  inline real q() const { }
  inline void q(real v) { }

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


  inline real p() const { }
  inline void p(real v) { }

  inline real q() const { }
  inline void q(real v) { }

};

struct Line
{
  complex Y;


  inline real y() const { }
  inline void y(real v) { }

  inline real ya() const { }
  inline void ya(real v) { }

};



#endif