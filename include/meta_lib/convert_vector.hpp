#ifndef __CONVERTVECTOR_HPP__
#define __CONVERTVECTOR_HPP__

#include <boost/mpl/push_back.hpp>
#include <boost/mpl/pop_front.hpp>

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
