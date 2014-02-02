#ifndef __METASTRINGREPR_HPP__
#define __METASTRINGREPR_HPP__

#include <boost/preprocessor/repetition/repeat.hpp>

#ifndef STRING_MAX_LENGTH
  #define STRING_MAX_LENGTH 20
#endif
 
#define DO(z, n, s) at(s, n),

#define _S(s)                                  \
  BOOST_PP_REPEAT(STRING_MAX_LENGTH, DO, s)    \
  '\0'

template <int N>
constexpr char at(char const(&s)[N], int i)
{
  return i >= N ? '\0' : s[i];
}

#endif // __METASTRINGREPR_HPP__