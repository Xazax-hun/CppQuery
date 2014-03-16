#ifndef __CONVERTVECTOR_HPP__
#define __CONVERTVECTOR_HPP__

#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/begin_end.hpp>

// Converts a boost::mpl::vector (specified with size and Vector) to a variadic
// pack of parameters (represented by Pack) and passes it to Host.
template <template <typename... Ts> class Host, typename Begin, typename End,
          typename... Pack>
struct ConvertVectorImpl {
  typedef typename ConvertVectorImpl<
      Host, typename boost::mpl::next<Begin>::type, End,
      typename boost::mpl::deref<Begin>::type, Pack...>::result result;
};

template <template <typename... Ts> class Host, typename End, typename... Pack>
struct ConvertVectorImpl<Host, End, End, Pack...> {
  typedef Host<Pack...> result;
};

template <template <typename... Ts> class Host, typename Vector>
struct ConvertVector
    : ConvertVectorImpl<Host, typename boost::mpl::begin<Vector>::type,
                        typename boost::mpl::end<Vector>::type> {};

#endif // __CONVERTVECTOR_HPP__
