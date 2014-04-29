#ifndef _PS_UTIL_
#define _PS_UTIL_

namespace ps
{

template <class R, class C, class F>
R sum(C c, F f)
{
  R s{0};
  for(const auto &e : c) { s += f(e); }
  return s;
}

}

#endif
