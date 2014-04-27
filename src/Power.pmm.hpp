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
  complex Y;


  real p() const { }
  void p(real v) { }

  real q() const { }
  void q(real v) { }

  real v() const { }
  void v(real v) { }

  real va() const { }
  void va(real v) { }

  real y() const { }
  void y(real v) { }

  real ya() const { }
  void ya(real v) { }

  real g() const { }
  void g(real v) { }

  real b() const { }
  void b(real v) { }

};

struct Generator
{
  complex S;


  real p() const { }
  void p(real v) { }

  real q() const { }
  void q(real v) { }

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


  real p() const { }
  void p(real v) { }

  real q() const { }
  void q(real v) { }

};

struct Line
{
  complex Y;


  real y() const { }
  void y(real v) { }

  real ya() const { }
  void ya(real v) { }

};



#endif