#ifndef __CONVERTVECTOR_HPP__
#define __CONVERTVECTOR_HPP__

#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/begin_end.hpp>

#include <boost/mpl/push_back.hpp>
#include <boost/mpl/pop_front.hpp>

// Converts a boost::mpl::forward sequence to a variadic
// pack of parameters (represented by Pack) and passes it to Host.
template <template <typename... Ts> class Host, typename Begin, typename End,
          typename... Pack>
struct ConvertSequenceImpl {
  typedef typename ConvertSequenceImpl<
      Host, typename boost::mpl::next<Begin>::type, End,
      typename boost::mpl::deref<Begin>::type, Pack...>::result result;
};

template <template <typename... Ts> class Host, typename End, typename... Pack>
struct ConvertSequenceImpl<Host, End, End, Pack...> {
  typedef Host<Pack...> result;
};

template <template <typename... Ts> class Host, typename Vector>
struct ConvertSequence
    : ConvertSequenceImpl<Host, typename boost::mpl::begin<Vector>::type,
                        typename boost::mpl::end<Vector>::type> {};

// Converts a boost::mpl::vector (specified with size and Vector) to a variadic
// pack of parameters (represented by Pack) and passes it to Host.
template <template <typename... Ts> class Host, int size, typename Vector,
          typename... Pack>
struct ConvertVector {
  typedef typename ConvertVector<
      Host, size - 1, typename boost::mpl::pop_front<Vector>::type,
      typename boost::mpl::front<Vector>::type, Pack...>::result result;
};

template <template <typename... Ts> class Host, typename T, typename... Pack>
struct ConvertVector<Host, 0, T, Pack...> {
  typedef Host<Pack...> result;
};

#endif // __CONVERTVECTOR_HPP__
