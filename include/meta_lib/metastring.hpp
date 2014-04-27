#ifndef __METASTRING_HPP__
#define __METASTRING_HPP__

#include <string>

#include "metastringrepr.hpp"

template<unsigned N>
constexpr unsigned countChars(const char(&str)[N], unsigned i) {
  return (str[i] == 0) ? i : countChars(str, i+1);
}

template <char... cs> struct MetaString {

  static std::string GetRuntimeString() {
    constexpr char contents[] = { cs... };
    constexpr unsigned count = countChars(contents, 0);
    return std::string(contents, count);
  }

  // Returns true for strings, that have the same characters
  // in the same order that in the template parameter of the parser.
  static bool Equals(const std::string &s) { return GetRuntimeString() == s; }
};

#endif // __METASTRING_HPP__
