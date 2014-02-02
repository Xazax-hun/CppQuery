#ifndef __MATCHER_TRAITS__
#define __MATCHER_TRAITS__

#include <type_traits>

#include <boost/mpl/vector.hpp>

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/result_type.hpp>

#include "clang/ASTMatchers/ASTMatchers.h"

#define MATCHER(x) std::tuple<typename matcher_trait<decltype(x)>::type, typename matcher_trait<decltype(x)>::object_type, MetaString<_S( #x )>>

template<typename T, typename U>
struct matcher_trait_helper;

template<typename T>
struct matcher_trait_helper<T, std::true_type>
{
  typedef T type;
  typedef typename boost::function_types::result_type<T>::type object_type;
};

template<typename T, typename U>
struct matcher_trait_helper<const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<T, U>, std::false_type>
{
  typedef typename boost::function_types::function_type<boost::mpl::vector<clang::ast_matchers::internal::Matcher<T>,clang::ast_matchers::internal::Matcher<U>>>::type type;
  typedef const clang::ast_matchers::internal::VariadicDynCastAllOfMatcher<T, U> object_type;
};

template<typename T>
struct matcher_trait_helper<const clang::ast_matchers::internal::VariadicAllOfMatcher<T>, std::false_type>
{
  typedef typename boost::function_types::function_type<boost::mpl::vector<clang::ast_matchers::internal::Matcher<T>,clang::ast_matchers::internal::Matcher<T>>>::type type;
  typedef const clang::ast_matchers::internal::VariadicAllOfMatcher<T> object_type;
};

template <typename T>
struct matcher_trait :
  matcher_trait_helper<T, typename std::is_function<T>::type>
{};


#endif
