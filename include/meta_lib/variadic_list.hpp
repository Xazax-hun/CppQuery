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

template <typename... lists> struct concat;

template <> struct concat<> {
  using type = list<>;
};

template <typename... Firsts> struct concat<list<Firsts...> > {
  using type = list<Firsts...>;
};

template <typename... Firsts, typename... Seconds>
struct concat<list<Firsts...>, list<Seconds...> > {
  using type = list<Firsts..., Seconds...>;
};

template <typename... Firsts, typename... Seconds, typename... Thirds>
struct concat<list<Firsts...>, list<Seconds...>, list<Thirds...> > {
  using type = list<Firsts..., Seconds..., Thirds...>;
};

template <typename... Firsts, typename... Seconds, typename... Thirds,
          typename... Fourths>
struct concat<list<Firsts...>, list<Seconds...>, list<Thirds...>,
              list<Fourths...> > {
  using type = list<Firsts..., Seconds..., Thirds..., Fourths...>;
};

template <typename... Firsts, typename... Seconds, typename... Thirds,
          typename... Fourths, typename... Fifths>
struct concat<list<Firsts...>, list<Seconds...>, list<Thirds...>,
              list<Fourths...>, list<Fifths...> > {
  using type = list<Firsts..., Seconds..., Thirds..., Fourths..., Fifths...>;
};

template <typename... Firsts, typename... Seconds, typename... Thirds,
          typename... Fourths, typename... Fifths, typename... Rest>
struct concat<list<Firsts...>, list<Seconds...>, list<Thirds...>,
              list<Fourths...>, list<Fifths...>, Rest...> {
  using type = concat<
      list<Firsts..., Seconds..., Thirds..., Fourths..., Fifths...>, Rest...>;
};

template <typename From, template <typename... Ts> class To> struct copy_pack;

template <typename... Ts, template <typename...> class To>
struct copy_pack<list<Ts...>, To> {
  using type = To<Ts...>;
};
}

#endif