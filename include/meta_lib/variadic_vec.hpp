#ifndef __VARIADIC_VEC__
#define __VARIADIC_VEC__

namespace variadic {

template <typename... Ts> struct vector {
  using result = vector;
};

template <typename vec, typename... elements> struct push_back;

template <typename... Ts, typename... elements>
struct push_back<vector<Ts...>, elements...> {
  using result = vector<Ts..., elements...>;
};

template <typename first, typename second> struct append;

template <typename... Firsts, typename... Seconds>
struct append<vector<Firsts...>, vector<Seconds...> > {
  using result = vector<Firsts..., Seconds...>;
};

template <typename... Vs> struct concat;

template <typename Head, typename Second, typename... Tail>
struct concat<Head, Second, Tail...> {
  using result =
      typename concat<typename append<Head, Second>::result, Tail...>::result;
};

template <typename Head, typename Second> struct concat<Head, Second> {
  using result = typename append<Head, Second>::result;
};

template <typename From, template <typename... Ts> class To>
struct copy_pack;

template <typename... Ts, template <typename... Ts> class To>
struct copy_pack<vector<Ts...>, To> {
  using result = To<Ts...>;
};

}

#endif