#ifndef __RULE_COMPOSER_HPP__
#define __RULE_COMPOSER_HPP__

#include <vector>
#include <string>
#include <utility>

#include <cassert>

#include "meta_lib/variadic_list.hpp"

#include "rule_generator.hpp"

struct RuntimeRule {
  std::string Fname;
  std::string Gname;
  ComposeResult composability;
};

template <typename T, unsigned Param> struct RuntimeRuleFactory;

template <typename F, typename FOType, typename FName, typename G,
          typename GOType, typename GName, unsigned Param>
struct RuntimeRuleFactory<ComposabilityRule<variadic::list<F, FOType, FName>,
                                            variadic::list<G, GOType, GName> >,
                          Param> {
  static RuntimeRule GetRuntimeRule() {
    return { FName::GetRuntimeString(), GName::GetRuntimeString(),
             (ComposabilityHelper<F, G, Param>::value
                  ? ComposeResult::Composable
                  : ComposeResult::NotComposable) };
  }
};

// Given a list of compile time composability rules, provides an interface
//  to access the composability information during runtime about
//  functions represented by their names.
template <class GetInstancePolicy, typename... IsComposable>
struct ComposedRules {
  ComposedRules() {
    rules0 = std::vector<RuntimeRule>(
        { RuntimeRuleFactory<IsComposable, 0>::GetRuntimeRule()... });
    rules1 = std::vector<RuntimeRule>(
        { RuntimeRuleFactory<IsComposable, 1>::GetRuntimeRule()... });
  }
  // Checks whether the functions represented by their names are composable.
  bool CanCompose(const std::string &f, const std::string &g,
                  int parameter = 0) {
    for (RuntimeRule r : GetRuleForParam(parameter)) {
      if (r.Fname == f && r.Gname == g)
        return r.composability == ComposeResult::Composable;
    }
    assert("Unreachable code");
    return false;
  }

  // Returns a vector of function names representing all functions with
  //  whom the function represented by its 'f' name is composable.
  std::vector<std::string> GetComposables(const std::string &f,
                                          int parameter = 0) {
    std::vector<std::string> v;
    for (RuntimeRule r : GetRuleForParam(parameter)) {
      if (r.Fname == f && r.composability == ComposeResult::Composable)
        v.push_back(r.Gname);
    }
    return v;
  }

private:
  // TODO: transform into matrix for prformance
  std::vector<RuntimeRule> rules0;
  std::vector<RuntimeRule> rules1;

  std::vector<RuntimeRule> &GetRuleForParam(int parameter) {
    switch (parameter) {
    case 0:
      return rules0;
    case 1:
      return rules1;
    }
    return rules0;
  }
};

#endif
