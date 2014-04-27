#ifndef __AUTOMATA_GENERATOR__
#define __AUTOMATA_GENERATOR__

#include "meta_lib/convert_vector.hpp"
#include "meta_lib/metastring.hpp"
#include "meta_lib/variadic_list.hpp"

#include "rule_generator.hpp"
#include "rule_composer.hpp"

// TODO: general getinstance policy
struct DefaultGetInstancePolicy {
  typedef void ResultType;
  template <typename T> T Create() {
    return T{};
  }
};

template <template <typename First, typename Second> class Func,
          typename Binding>
struct BindSecond {
  template <typename FirstArg> using type = Func<FirstArg, Binding>;
};

template <typename Type, typename TypeList> struct ComposeOneTypeWithAll;

template <typename Type, typename... Others>
struct ComposeOneTypeWithAll<Type, variadic::list<Others...> > {
  using type = typename variadic::map<
      variadic::list<Others...>,
      BindSecond<ComposabilityRule, Type>::template type>::type;
};

template <typename... Elements> struct ComposeAllWithAll {
  using helper = typename variadic::map<
      variadic::list<Elements...>,
      BindSecond<ComposeOneTypeWithAll,
                 variadic::list<Elements...> >::template type>::type;

  using type = typename variadic::concat_list<helper>::type;
};

// 'Pairs' is a pack of (decltype, name) pairs: one such pair represents a
// single function whose composability we analyze.
template <typename... Pairs> struct Automata {
  template <typename... IsComposables>
  using CompRules = ComposedRules<DefaultGetInstancePolicy, IsComposables...>;

  typedef typename ComposeAllWithAll<Pairs...>::type list;
  typedef typename variadic::copy_pack<list, CompRules>::type result;
};

template <typename GetInstancePolicy, typename... Pairs>
struct AutomataWithGetInst {
  template <typename... IsComposables>
  using CompRules = ComposedRules<GetInstancePolicy, IsComposables...>;

  typedef typename ComposeAllWithAll<Pairs...>::type list;
  typedef typename variadic::copy_pack<list, CompRules>::type result;
};
#endif
