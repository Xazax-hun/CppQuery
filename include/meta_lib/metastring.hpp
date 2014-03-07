#ifndef __METASTRING_HPP__
#define __METASTRING_HPP__

#include <string>

#include "metastringrepr.hpp"

template <char... cs> struct Accumulator {
  static std::string GetRuntimeString() {
    return { cs... };
  }
};

template <typename T, char...> struct MetaStringImpl;

template <char... accumulated, char head, char... leftovers>
struct MetaStringImpl<Accumulator<accumulated...>, head, leftovers...> {
  typedef typename MetaStringImpl<Accumulator<accumulated..., head>,
                                  leftovers...>::result result;
};

// Truncate trailing nulls
template <char... accumulated, char... leftovers>
struct MetaStringImpl<Accumulator<accumulated...>, '\0', leftovers...> {
  typedef Accumulator<accumulated...> result;
};

// No trailing null left
template <char... accumulated>
struct MetaStringImpl<Accumulator<accumulated...> > {
  typedef Accumulator<accumulated...> result;
};

template <char... cs> struct MetaString {
  typedef typename MetaStringImpl<Accumulator<>, cs...>::result trimmed;

  static std::string GetRuntimeString() { return trimmed::GetRuntimeString(); }

  // Returns true for strings, that have the same characters
  // in the same order that in the template parameter of the parser.
  static bool Equals(const std::string &s) {
    return trimmed::GetRuntimeString() == s;
  }
};

#endif // __METASTRING_HPP__
