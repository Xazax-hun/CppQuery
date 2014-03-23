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

// Check if Dest is composable with any of the functions that are
// after Dest.
template <typename Dest, typename Target, typename... Targets>
struct TryComposeWithAll {
  typedef typename variadic::push_back<
      typename TryComposeWithAll<Dest, Targets...>::result,
      ComposabilityRule<Dest, Target>,
      ComposabilityRule<Target, Dest> >::type result;
};

// Specialized for Dest == Target
template <typename Dest, typename... Targets>
struct TryComposeWithAll<Dest, Dest, Targets...> {
  typedef typename variadic::push_back<
      typename TryComposeWithAll<Dest, Targets...>::result,
      ComposabilityRule<Dest, Dest> >::type result;
};

template <typename Dest, typename Target>
struct TryComposeWithAll<Dest, Target> {
  // check if we can compose Dest and Target in any way, i.e. Dest(Target) or
  // Target(Dest)

  typedef typename variadic::list<ComposabilityRule<Dest, Target>,
                                  ComposabilityRule<Target, Dest> >::type
  result;
};

// Specialized for Dest == Target
template <typename Dest> struct TryComposeWithAll<Dest, Dest> {
  typedef typename variadic::list<ComposabilityRule<Dest, Dest> >::type result;
};

// Try to compose all of the possible pairing of the functions.
template <typename Pair, typename... Pairs> struct AutomataImpl {
  // this is basically an iteration over 'Pairs', like for(Pair : Pairs)

  // gather all results for 'Pair', i.e. all functions that are composable with
  // 'Pair'
  typedef typename TryComposeWithAll<Pair, Pair, Pairs...>::result partial;

  // merge the results for 'Pair' with the results of the elements of 'Pairs'
  // (recurse)
  typedef typename variadic::append<typename AutomataImpl<Pairs...>::result,
                                    partial>::type result;
};

template <typename Pair> struct AutomataImpl<Pair> {
  typedef typename variadic::list<ComposabilityRule<Pair, Pair> >::type result;
};

// 'Pairs' is a pack of (decltype, name) pairs: one such pair represents a
// single function whose composability we analyze.
template <typename... Pairs> struct Automata {
  template <typename... IsComposables>
  using CompRules = ComposedRules<DefaultGetInstancePolicy, IsComposables...>;

  typedef typename AutomataImpl<Pairs...>::result vec;
  typedef typename variadic::copy_pack<vec, CompRules>::type result;
};

template <typename GetInstancePolicy, typename... Pairs>
struct AutomataWithGetInst {
  template <typename... IsComposables>
  using CompRules = ComposedRules<GetInstancePolicy, IsComposables...>;

  typedef typename AutomataImpl<Pairs...>::result vec;
  typedef typename variadic::copy_pack<vec, CompRules>::type result;
};
#endif
