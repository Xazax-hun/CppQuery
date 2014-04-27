#ifndef __RULE_GENERATOR_HPP__
#define __RULE_GENERATOR_HPP__

#include <string>
#include <tuple>

#include <type_traits>

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>

#include "boost/mpl/at.hpp"

template <typename T, T *ptr> struct FunctionPointer;

template <typename Ret, typename... Args, Ret (*f)(Args...)>
struct FunctionPointer<Ret(Args...), f> {
  Ret operator()(Args... args) { return f(args...); }
};

#define FUNCTION(x)                                                            \
  std::tuple<decltype(x), FunctionPointer<decltype(x), &x>, MetaString<_S(#x)> >
#define FUNCTOR(x) std::tuple<decltype(&x::operator()), x, MetaString<_S(#x)> >

enum ComposeResult {
  RuleNotApplicable,
  Composable,
  NotComposable,
  OutOfArityBounds
};

template <typename F, typename G, int Par> struct ComposabilityHelper {
  typedef typename boost::function_types::result_type<G>::type RetType;
  typedef typename boost::mpl::at_c<boost::function_types::parameter_types<F>,
                                    Par>::type ArgType;

  static const bool value = std::is_convertible<RetType, ArgType>::value;
};

template <typename T, typename U> struct ComposabilityRule;

// Determines if two functions are composable, where
//  both are represented by their decltypes (F, G)
//  and their names (FName, GName).
template <typename F, typename FOType, typename FName, typename G,
          typename GOType, typename GName>
struct ComposabilityRule<std::tuple<F, FOType, FName>,
                         std::tuple<G, GOType, GName> > {
  typedef ComposabilityRule<std::tuple<F, FOType, FName>,
                            std::tuple<G, GOType, GName> > type;
  typedef F Function;
  typedef FName FunctionName;
  typedef FOType ObjectType;
  static const int arity = boost::function_types::function_arity<F>::value;

  // Checks if this rule is applicable to determining the
  //  composability of the functions f and g
  //  (i.e. whether f refers to FName and g to GName),
  //  and if so, decides whether they are composable or not.
  static ComposeResult TryCompose(const std::string &f, const std::string &g,
                                  int parameter = 0) {
    // TODO: support arbitary number of arity
    static constexpr bool composable[5] = {
      ComposabilityHelper<F, G, 0>::value, ComposabilityHelper<F, G, 1>::value,
      ComposabilityHelper<F, G, 2>::value, ComposabilityHelper<F, G, 3>::value,
      ComposabilityHelper<F, G, 4>::value
    };

    if (parameter > arity)
      return OutOfArityBounds;

    if (!FName::Equals(f) || !GName::Equals(g)) {
      return RuleNotApplicable;
    } else {
      return composable[parameter] ? Composable : NotComposable;
    }
  }

  // Gets the runtime name of the inner function G if
  //  F and G are composable (and f is the name of F).
  static std::string GetInnerFunction(const std::string &f, int parameter = 0) {
    static constexpr bool composable[5] = {
      ComposabilityHelper<F, G, 0>::value, ComposabilityHelper<F, G, 1>::value,
      ComposabilityHelper<F, G, 2>::value, ComposabilityHelper<F, G, 3>::value,
      ComposabilityHelper<F, G, 4>::value
    };

    if (!FName::Equals(f)) {
      return "";
    } else {
      return composable[parameter] ? GName::GetRuntimeString() : "";
    }
  }
};

#endif
