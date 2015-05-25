#include <type_traits>

#ifndef XIO_TRAITS
#define XIO_TRAITS

//-----------------------------------------------------------------------------

namespace xio {

//-----------------------------------------------------------------------------

namespace xio_details {

//-----------------------------------------------------------------------------
// more convenient alternative to std::enable_if

template <bool C, typename T = int>
struct only_if_t { using type = T; };

template <typename T>
struct only_if_t<false, T>;

template <bool C, typename T = int>
using only_if = typename only_if_t<C, T>::type;

//-----------------------------------------------------------------------------
// shortcut for boolean expressions

template<bool C>
using expr = std::integral_constant<bool, C>;

using _true = expr<true>;
using _false = expr<false>;

//-----------------------------------------------------------------------------
// generic SFINAE test

template<template<typename...> class F, typename... A, typename = F<A...>>
std::true_type test(int);

template<template<typename...> class F, typename... A>
std::false_type test(...);

template<template<typename...> class F, typename... A>
using sfinae = decltype(test<F, A...>(0));

//-----------------------------------------------------------------------------
// container method tests

template<typename A> using _begin = decltype(std::begin(std::declval<A>()));
template<typename A> using has_begin = sfinae<_begin, A>;

template<typename A> using _end = decltype(std::end(std::declval<A>()));
template<typename A> using has_end = sfinae<_end, A>;

template<typename A> using _clear = decltype(std::declval<A>().clear());
template<typename A> using has_clear = sfinae<_clear, A>;

template<typename A> using _size = decltype(std::declval<A>().size());
template<typename A> using has_size = sfinae<_size, A>;

template<typename A> using _back = decltype(std::declval<A>().back());
template<typename A> using has_back = sfinae<_back, A>;

template<typename A> using _rdbuf = decltype(std::declval<A>().rdbuf());
template<typename A> using has_rdbuf = sfinae<_rdbuf, A>;

//-----------------------------------------------------------------------------
// container classification

template<typename A>
using is_range = expr<has_begin<A>() && has_end<A>()>;

template<typename A>
using is_fixed = expr<!has_clear<A>()>;

template<typename A>
using is_seq = has_back<A>;

//-----------------------------------------------------------------------------
// iterator and value type without members, only via begin()

template<typename A>
using iter = decltype(std::begin(std::declval<A>()));

template<typename A>
using elem = typename std::decay<decltype(*std::begin(std::declval<A>()))>::type;

//-----------------------------------------------------------------------------
// element classification

template<typename A>
using is_triv = std::is_trivially_copyable<A>;

template<typename A>
using is_cont_triv = expr<is_contiguous<A>() && is_triv<elem<A>>()>;

//-----------------------------------------------------------------------------

template<typename S>
using chr = typename S::char_type;

//-----------------------------------------------------------------------------

}  // namespace xio_details

//-----------------------------------------------------------------------------

}  // namespace xio

//-----------------------------------------------------------------------------

#endif // XIO_TRAITS
