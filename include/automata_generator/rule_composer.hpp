#ifndef __RULE_COMPOSER_HPP__
#define __RULE_COMPOSER_HPP__

#include <vector>
#include <string>

#include <cassert>

#include "rule_generator.hpp"

// Given a list of compile time composability rules, provides an interface
//  to access the composability information during runtime about
//  functions represented by their names.
template <class GetInstancePolicy, typename IsComposable,
          typename... IsComposables>
struct ComposedRules {
  // Checks whether the functions represented by their names are composable.
  static bool CanCompose(const std::string &f, const std::string &g,
                         int parameter = 0) {
    switch (IsComposable::TryCompose(f, g, parameter)) {
    case RuleNotApplicable:
      return ComposedRules<GetInstancePolicy, IsComposables...>::CanCompose(
          f, g, parameter);

    case Composable:
      return true;

    case NotComposable:
      return false;
    }

    assert("Impossible result of TryCompose");
    return false;
  }

  // Returns a vector of function names representing all functions with
  //  whom the function represented by its 'f' name is composable.
  static std::vector<std::string> GetComposables(const std::string &f,
                                                 int parameter = 0) {
    auto v = ComposedRules<GetInstancePolicy, IsComposables...>::GetComposables(
        f, parameter);
    auto s = IsComposable::GetInnerFunction(f, parameter);

    if (s.length()) {
      v.push_back(s);
    }

    return v;
  }

  // TODO: move GetInstance to Automata and make it O(n) instead of O(n^2)
  static typename GetInstancePolicy::ResultType
  GetInstance(const std::string &f) {
    if (IsComposable::FunctionName::GetRuntimeString() != f)
      return ComposedRules<GetInstancePolicy, IsComposables...>::GetInstance(f);

    return GetInstancePolicy::template Create<
        typename IsComposable::ObjectType>();
  }
};

template <typename GetInstancePolicy, typename IsComposable>
struct ComposedRules<GetInstancePolicy, IsComposable> {
  static bool CanCompose(const std::string &f, const std::string &g,
                         int parameter = 0) {
    switch (IsComposable::TryCompose(f, g, parameter)) {
    case RuleNotApplicable:
      return false;

    case Composable:
      return true;

    case NotComposable:
      return false;
    }

    assert("Impossible result of TryCompose");
    return false;
  }

  static std::vector<std::string> GetComposables(const std::string &f,
                                                 int parameter = 0) {
    std::vector<std::string> v;
    auto s = IsComposable::GetInnerFunction(f, parameter);

    if (s.length()) {
      v.push_back(s);
    }

    return v;
  }

  static typename GetInstancePolicy::ResultType
  GetInstance(const std::string &f) {
    if (IsComposable::FunctionName::GetRuntimeString() != f)
      assert(false && IsComposable::FunctionName::GetRuntimeString().c_str() &&
             " can not be instantiated");

    return GetInstancePolicy::template Create<
        typename IsComposable::ObjectType>();
  }
};

#endif
