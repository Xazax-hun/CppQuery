#ifndef __VARIADIC_VEC__
#define __VARIADIC_VEC__

namespace variadic {

template <typename... Ts> struct list {
  using type = list;
};

template <typename vec, typename... elements> struct push_back;

template <typename... Ts, typename... elements>
struct push_back<list<Ts...>, elements...> {
  using type = list<Ts..., elements...>;
};

template <typename first, typename second> struct append;

template <typename... Firsts, typename... Seconds>
struct append<list<Firsts...>, list<Seconds...> > {
  using type = list<Firsts..., Seconds...>;
};

template <typename... Vs> struct concat;

template <typename Head, typename Second, typename... Tail>
struct concat<Head, Second, Tail...> {
  using type =
      typename concat<typename append<Head, Second>::type, Tail...>::type;
};

template <typename Head, typename Second> struct concat<Head, Second> {
  using type = typename append<Head, Second>::type;
};

template <typename From, template <typename... Ts> class To> struct copy_pack;

template <typename... Ts, template <typename... Ts> class To>
struct copy_pack<list<Ts...>, To> {
  using type = To<Ts...>;
};
}

#endif