#ifndef __CALLABLE_TRAITS_H__
#define __CALLABLE_TRAITS_H__

#include <type_traits>
#include "variadic_list.hpp"

template <class Functor> struct is_functor {
  template <class T> static std::true_type test(decltype(&T::operator()));

  template <class T> static std::false_type test(...);

  typedef decltype(test<Functor>(nullptr)) type;

  static const bool value = type::value;
};

template <typename... Args> struct arg_traits;

template <typename Arg0> struct arg_traits<Arg0> {
  using arg0 = Arg0;
};

template <typename Arg0, typename Arg1> struct arg_traits<Arg0, Arg1> {
  using arg0 = Arg0;
  using arg1 = Arg1;
};

template <typename Arg0, typename Arg1, typename Arg2>
struct arg_traits<Arg0, Arg1, Arg2> {
  using arg0 = Arg0;
  using arg1 = Arg1;
  using arg2 = Arg2;
};

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
struct arg_traits<Arg0, Arg1, Arg2, Arg3> {
  using arg0 = Arg0;
  using arg1 = Arg1;
  using arg2 = Arg2;
  using arg3 = Arg3;
};

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3,
          typename Arg4>
struct arg_traits<Arg0, Arg1, Arg2, Arg3, Arg4> {
  using arg0 = Arg0;
  using arg1 = Arg1;
  using arg2 = Arg2;
  using arg3 = Arg3;
  using arg4 = Arg4;
};

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3,
          typename Arg4, typename... Rest>
struct arg_traits<Arg0, Arg1, Arg2, Arg3, Arg4, Rest...> {
  using arg0 = Arg0;
  using arg1 = Arg1;
  using arg2 = Arg2;
  using arg3 = Arg3;
  using arg4 = Arg4;
  using rest = variadic::list<Rest...>;
};

template <typename T, typename IsFunctor> struct callable_traits_helper;

template <typename Ret, typename... Args>
struct callable_traits_helper<Ret(Args...),
                              std::false_type> : arg_traits<Args...> {
  using result_type = Ret;
  using arguments = variadic::list<Args...>;
};

template <typename Ret, typename... Args>
struct callable_traits_helper<
    Ret (*)(Args...),
    std::false_type> : callable_traits_helper<Ret(Args...), std::false_type> {};

template <typename T, typename Ret, typename... Args>
struct callable_traits_helper<
    Ret (T::*)(Args...),
    std::false_type> : callable_traits_helper<Ret(Args...), std::false_type> {};

template <typename T>
struct callable_traits_helper<T, std::true_type> : callable_traits_helper<
                                                       decltype(&T::operator()),
                                                       std::false_type> {};

template <typename T>
struct callable_traits
    : callable_traits_helper<T, typename is_functor<T>::type> {};

#endif