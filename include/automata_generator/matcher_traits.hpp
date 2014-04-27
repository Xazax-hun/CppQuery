#ifndef __MATCHER_TRAITS__
#define __MATCHER_TRAITS__

#include <type_traits>
#include <utility>

#include <boost/mpl/vector.hpp>

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/LLVM.h"

// TODO: handle templates: possible solution: replace templated parameters with
// a type such that any type can be converted to that type.

#define MATCHER(x)                                                             \
  std::tuple<typename matcher_trait<decltype(x)>::type,                        \
             typename matcher_trait<decltype(x)>::object_type,                 \
             MetaString<_S(#x)> >

template <typename T, typename U> struct matcher_trait_helper;

template <typename T> struct matcher_trait_helper<T, std::true_type> {
  typedef T type;
  typedef typename boost::function_types::result_type<T>::type object_type;
};

template <typename T, typename U>
struct matcher_trait_helper<
    const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<T, U>,
    std::false_type> {
  typedef typename boost::function_types::function_type<boost::mpl::vector<
      clang::ast_matchers::internal::Matcher<T>,
      clang::ast_matchers::internal::Matcher<U> > >::type type;
  typedef const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<T, U>
  object_type;
};

template <typename T>
struct matcher_trait_helper<
    const clang::ast_matchers::internal::VariadicAllOfMatcher<T>,
    std::false_type> {
  typedef typename boost::function_types::function_type<boost::mpl::vector<
      clang::ast_matchers::internal::Matcher<T>,
      clang::ast_matchers::internal::Matcher<T> > >::type type;
  typedef const clang::ast_matchers::internal::VariadicAllOfMatcher<T>
  object_type;
};

template <template <typename ToArg, typename FromArg> class ArgumentAdapter,
          typename T, typename U>
struct matcher_trait_helper<
    const clang::ast_matchers::internal::ArgumentAdaptingMatcherFunc<
        ArgumentAdapter, T, U>,
    std::false_type> {
  typedef typename boost::function_types::function_type<boost::mpl::vector<
      clang::ast_matchers::internal::Matcher<T>,
      clang::ast_matchers::internal::Matcher<U> > >::type type;
  typedef const clang::ast_matchers::internal::ArgumentAdaptingMatcherFunc<
      ArgumentAdapter, T, U> object_type;
};

template<typename Result, typename Arg, Result (*Func)(llvm::ArrayRef<const Arg *>)> 
struct var_func_container {
  typedef Result result;
  typedef Arg arg;
};

template <typename Result, typename Arg,
          Result (*Func)(llvm::ArrayRef<const Arg *>)>
var_func_container<Result, Arg, Func>
getLLVMVarFunc(llvm::VariadicFunction<Result, Arg, Func> *) {}

template <typename T> struct matcher_trait_helper<const T, std::false_type> {

  typedef typename decltype(getLLVMVarFunc((T*)0))::result result;
  typedef typename decltype(getLLVMVarFunc((T*)0))::arg arg;

  typedef typename boost::function_types::function_type<
      boost::mpl::vector<result, arg> >::type type;

  typedef T object_type;
};

template <typename T>
struct matcher_trait
    : matcher_trait_helper<T, typename std::is_function<T>::type> {};

#endif
