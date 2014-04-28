#ifndef __VARIADIC_VEC__
#define __VARIADIC_VEC__

namespace variadic {

template <typename... Ts> struct list {};

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
          typename... Fourths, typename... Fifths, typename... Sixths>
struct concat<list<Firsts...>, list<Seconds...>, list<Thirds...>,
              list<Fourths...>, list<Fifths...>, list<Sixths...> > {
  using type = list<Firsts..., Seconds..., Thirds..., Fourths..., Fifths..., Sixths...>;
};

template <typename... Firsts, typename... Seconds, typename... Thirds,
          typename... Fourths, typename... Fifths, typename... Sixths, typename... Rest>
struct concat<list<Firsts...>, list<Seconds...>, list<Thirds...>,
              list<Fourths...>, list<Fifths...>, list<Sixths...>, Rest...> {
  using type = typename concat<
      list<Firsts..., Seconds..., Thirds..., Fourths..., Fifths..., Sixths...>,
      Rest...>::type;
};

template <typename list> struct concat_list;

template <typename... elements> struct concat_list<list<elements...> > {
  using type = typename concat<elements...>::type;
};

template <typename From, template <typename... Ts> class To> struct copy_pack;

template <typename... Ts, template <typename...> class To>
struct copy_pack<list<Ts...>, To> {
  using type = To<Ts...>;
};

template <typename list, template <typename Val> class func> struct map;

template <typename... elems, template <typename Val> class func>
struct map<list<elems...>, func> {
  using type = list<typename func<elems>::type...>;
};

template <typename list, template <typename Val> class func> struct lazy_map;

template <typename... elems, template <typename Val> class func>
struct lazy_map<list<elems...>, func> {
  using type = list<func<elems>...>;
};
}

#endif