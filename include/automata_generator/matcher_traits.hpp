#ifndef __MATCHER_TRAITS__
#define __MATCHER_TRAITS__

#include <type_traits>
#include <utility>

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/LLVM.h"

#include "meta_lib/variadic_list.hpp"

// TODO: handle templates: possible solution: replace templated parameters with
// a type such that any type can be converted to that type.

#define GET_MATCHER_OVERLOAD(name, Id)                                         \
  typename std::remove_reference<                                              \
      decltype(static_cast< ::clang::ast_matchers::name##_Type##Id>(           \
          ::clang::ast_matchers::name))>::type

#define MATCHER(x)                                                             \
  variadic::list<typename matcher_trait<decltype(x)>::type,                    \
                 typename matcher_trait<decltype(x)>::object_type,             \
                 MetaString<_S(#x)> >

#define MATCHER2(x)                                                            \
  variadic::list<                                                              \
      typename matcher_trait<GET_MATCHER_OVERLOAD(x, 0)>::type,                \
      typename matcher_trait<GET_MATCHER_OVERLOAD(x, 0)>::object_type,         \
      MetaString<_S(#x)> >

template <typename To, typename From>
struct ImplicitlyConvertableMatcherParameter {
  template <typename T,
            typename std::enable_if<
                clang::ast_matchers::internal::TypeListContainsSuperOf<
                    From, T>::value>::type * = nullptr>
  ImplicitlyConvertableMatcherParameter(
      const clang::ast_matchers::internal::Matcher<T> &) {}

  template <typename T,
            typename std::enable_if<
                clang::ast_matchers::internal::TypeListContainsSuperOf<
                    To, T>::value>::type * = nullptr>
  operator clang::ast_matchers::internal::Matcher<T>() const {}
};

template <typename T> struct MatcherReturnTypeAdapter : T {};

template <template <typename> class MatcherT, typename ReturnTypesF>
struct MatcherReturnTypeAdapter<
    clang::ast_matchers::internal::PolymorphicMatcherWithParam0<
        MatcherT, ReturnTypesF> > {
  typedef typename clang::ast_matchers::internal::PolymorphicMatcherWithParam0<
      MatcherT, ReturnTypesF>::ReturnTypes Types;

  template <typename T,
            typename std::enable_if<
                clang::ast_matchers::internal::TypeListContainsSuperOf<
                    Types, T>::value>::type * = nullptr>
  operator clang::ast_matchers::internal::Matcher<T>() const {}
};

template <template <typename, typename> class MatcherT, typename P1,
          typename ReturnTypesF>
struct MatcherReturnTypeAdapter<
    clang::ast_matchers::internal::PolymorphicMatcherWithParam1<
        MatcherT, P1, ReturnTypesF> > {
  typedef typename clang::ast_matchers::internal::PolymorphicMatcherWithParam1<
      MatcherT, P1, ReturnTypesF>::ReturnTypes Types;

  explicit MatcherReturnTypeAdapter(const P1 &Param1) {}

  template <typename T,
            typename std::enable_if<
                clang::ast_matchers::internal::TypeListContainsSuperOf<
                    Types, T>::value>::type * = nullptr>
  operator clang::ast_matchers::internal::Matcher<T>() const {}
};

template <template <typename, typename, typename> class MatcherT, typename P1,
          typename P2, typename ReturnTypesF>
struct MatcherReturnTypeAdapter<
    clang::ast_matchers::internal::PolymorphicMatcherWithParam2<
        MatcherT, P1, P2, ReturnTypesF> > {
  typedef typename clang::ast_matchers::internal::PolymorphicMatcherWithParam2<
      MatcherT, P1, P2, ReturnTypesF>::ReturnTypes Types;

  explicit MatcherReturnTypeAdapter(const P1 &Param1, const P2 &Param2) {}

  template <typename T,
            typename std::enable_if<
                clang::ast_matchers::internal::TypeListContainsSuperOf<
                    Types, T>::value>::type * = nullptr>
  operator clang::ast_matchers::internal::Matcher<T>() const {}
};

template <typename T> struct ReturnTypeTransformer;

template <typename Ret, typename... Args>
struct ReturnTypeTransformer<Ret(Args...)> {
  using type = MatcherReturnTypeAdapter<Ret>(Args...);
  using return_type = Ret;
};

template <typename T, typename U> struct matcher_trait_helper;

template <typename T> struct matcher_trait_helper<T, std::true_type> {
  typedef typename ReturnTypeTransformer<T>::type type;

  typedef typename ReturnTypeTransformer<T>::return_type object_type;
};

template <typename T, typename U>
struct matcher_trait_helper<
    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<T, U>,
    std::false_type> {
  typedef clang::ast_matchers::internal::Matcher<T>
  type(clang::ast_matchers::internal::Matcher<U>);

  typedef const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<T, U>
  object_type;
};

template <typename T>
struct matcher_trait_helper<
    const clang::ast_matchers::internal::VariadicAllOfMatcher<T>,
    std::false_type> {
  typedef clang::ast_matchers::internal::Matcher<T>
  type(clang::ast_matchers::internal::Matcher<T>);

  typedef const clang::ast_matchers::internal::VariadicAllOfMatcher<T>
  object_type;
};

template <template <typename ToArg, typename FromArg> class ArgumentAdapter,
          typename T, typename U>
struct matcher_trait_helper<
    const clang::ast_matchers::internal::ArgumentAdaptingMatcherFunc<
        ArgumentAdapter, T, U>,
    std::false_type> {
  typedef ImplicitlyConvertableMatcherParameter<T, U> Param;

  typedef Param type(Param);

  typedef const clang::ast_matchers::internal::ArgumentAdaptingMatcherFunc<
      ArgumentAdapter, T, U> object_type;
};

template <typename Result, typename Arg,
          Result (*Func)(llvm::ArrayRef<const Arg *>)>
struct var_func_container {
  typedef Result result;
  typedef Arg arg;
};

template <typename Result, typename Arg,
          Result (*Func)(llvm::ArrayRef<const Arg *>)>
var_func_container<Result, Arg, Func>
getLLVMVarFunc(llvm::VariadicFunction<Result, Arg, Func> *) {}

template <typename T> struct matcher_trait_helper<const T, std::false_type> {

  typedef typename decltype(getLLVMVarFunc((T *)0))::result result;
  typedef typename decltype(getLLVMVarFunc((T *)0))::arg arg;

  typedef result type(arg);

  typedef T object_type;
};

template <unsigned Min, unsigned Max>
struct matcher_trait_helper<
    const clang::ast_matchers::internal::VariadicOperatorMatcherFunc<Min, Max>,
    std::false_type> {
  typedef clang::ast_matchers::internal::PolymorphicMatcherWithParam0<
      clang::ast_matchers::internal::TrueMatcher> Anything;
  typedef clang::ast_matchers::internal::DynTypedMatcher Argument;
  typedef Anything type(Argument, Argument);

  typedef const clang::ast_matchers::internal::VariadicOperatorMatcherFunc<Min, Max>
  object_type;
};

template <typename T>
struct matcher_trait
    : matcher_trait_helper<T, typename std::is_function<T>::type> {};

#endif
