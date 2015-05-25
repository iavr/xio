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
// shortcut to std::declval()

template<typename A>
typename std::add_rvalue_reference<A>::type gen();

//-----------------------------------------------------------------------------
// generic SFINAE test

template<template<typename...> class F, typename... A, typename = F<A...>>
_true test(int);

template<template<typename...> class F, typename... A>
_false test(...);

template<template<typename...> class F, typename... A>
using sfinae = decltype(test<F, A...>(0));

//-----------------------------------------------------------------------------
// container method tests

template<typename A> using _begin = decltype(std::begin(gen<A>()));
template<typename A> using has_begin = sfinae<_begin, A>;

template<typename A> using _end = decltype(std::end(gen<A>()));
template<typename A> using has_end = sfinae<_end, A>;

template<typename A> using _size = decltype(gen<A>().size());
template<typename A> using has_size = sfinae<_size, A>;

template<typename A> void _ins_rng(A& a) { a.insert(a.begin(), a.end()); }
template<typename A> using _insert_rng = decltype(_ins_rng(gen<A>()));
template<typename A> using has_insert_rng = sfinae<_insert_rng, A>;

//-----------------------------------------------------------------------------
// container classification
// is_fixed is defined in fun.hpp

template<typename A>
using is_range = expr<has_begin<A>{} && has_end<A>{}>;

template<typename A>
using is_contig = decltype(is_contiguous(gen<A>()));

template<typename A>
using is_seq = expr<!has_insert_rng<A>{}>;

//-----------------------------------------------------------------------------
// iterator and value type without members, only via begin()

template<typename A>
using iter = decltype(std::begin(gen<A>()));

template<typename A>
using elem = typename std::decay<decltype(*std::begin(gen<A>()))>::type;

//-----------------------------------------------------------------------------
// element classification

template<typename A>
using is_triv = std::is_trivially_copyable<A>;

template<typename A>
using is_cont_triv = expr<is_contig<A>{} && is_triv<elem<A>>{}>;

//-----------------------------------------------------------------------------

template<typename S>
using chr = typename S::char_type;

//-----------------------------------------------------------------------------

}  // namespace xio_details

//-----------------------------------------------------------------------------

}  // namespace xio

//-----------------------------------------------------------------------------

#endif // XIO_TRAITS
