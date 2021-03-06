#include <iostream>
#include <fstream>

#ifndef XIO_IO
#define XIO_IO

//-----------------------------------------------------------------------------

namespace xio {

//-----------------------------------------------------------------------------

namespace xio_details {

//-----------------------------------------------------------------------------
// forward declarations

template<typename S, typename A> void read(S& s, A& a);
template<typename S, typename A> void write(S& s, A& a);
template<typename S, typename A> void xread(S& s, A& a);
template<typename S, typename A> void xwrite(S& s, A& a);

//-----------------------------------------------------------------------------
// low-level serialization as direct memory copy, for trivially-copyable
// types only

template<typename T, typename C>
std::streamsize io_size(size_t size) { return size * sizeof(T) / sizeof(C); }

template<typename S, typename T, typename C = chr<S>>
void r_mem(S& s, T* base, size_t size = 1)
{
	s.read(reinterpret_cast<C*>(base), io_size<T, C>(size));
}

template<typename S, typename T, typename C = chr<S>>
void w_mem(S& s, const T* base, size_t size = 1)
{
	s.write(reinterpret_cast<const C*>(base), io_size<T, C>(size));
}

//-----------------------------------------------------------------------------
// serialization of contiguous range trivial elements by direct memory copy;
// this is the fastest method

template<typename S, typename A, only_if<is_contig<A>{}> = 0>
void r_elem_triv(S& s, A& a, size_t n) { r_mem(s, base(a), size(a)); }

template<typename S, typename A, only_if<is_contig<A>{}> = 0>
void w_elem_triv(S& s, const A& a) { w_mem(s, base(a), size(a)); }

//-----------------------------------------------------------------------------
// serialization of non-contiguous range trivial elements using a contiguous
// buffer for acceleration followed by custom element insertion

template<typename S, typename A, only_if<!is_contig<A>{}> = 0>
void r_elem_triv(S& s, A& a, size_t n)
{
	std::vector<elem<A>> b(n);
	read(s, b); insert(a, b.begin(), b.end());
}

template<typename S, typename A, only_if<!is_contig<A>{}> = 0>
void w_elem_triv(S& s, const A& a)
{
	std::vector<elem<A>> b(size(a));
	std::copy(a.begin(), a.end(), b.begin()); write(s, b);
}

//-----------------------------------------------------------------------------
// serialization of range non-trivial elements using custom instertion on
// stream iterators; this is the most generic and slowest method

template<typename S, typename A, only_if<is_triv<elem<A>>{}> = 0>
void r_elem(S& s, A& a, size_t n) { r_elem_triv(s, a, n); }

template<typename S, typename A, only_if<is_triv<elem<A>>{}> = 0>
void w_elem(S& s, A& a) { w_elem_triv(s, a); }

template<typename S, typename A, only_if<!is_triv<elem<A>>{}> = 0>
void r_elem(S& s, A& a, size_t)
{
	using T = elem<A>;
	insert(a, begin<T>(s), end<T>(s));
}

template<typename S, typename A, only_if<!is_triv<elem<A>>{}> = 0>
void w_elem(S& s, A& a)
{
	std::copy(a.begin(), a.end(), begin<elem<A>>(s));
}

//-----------------------------------------------------------------------------
// serialization of dimensions; assumes function call `dims(a)` yields
// another range `d` holding the dimensions of the given range `a`;
// serialization is ignored if `d` is fixed.

template<bool X, typename S, typename A, only_if<!X || is_fixed<A>{}> = 0>
size_t r_dims(expr<X>, S& s, A& a) { return size(a); }

template<bool X, typename S, typename A, only_if<!X || is_fixed<A>{}> = 0>
void w_dims(expr<X>, S& s, const A& a) {}

template<bool X, typename S, typename A, only_if<X && !is_fixed<A>{}> = 0>
size_t r_dims(expr<X>, S& s, A& a)
{
	auto&& d = dims(a); xread(s, d);
	size_t n = total(d); resize(a, n); return n;
}

template<bool X, typename S, typename A, only_if<X && !is_fixed<A>{}> = 0>
void w_dims(expr<X>, S& s, const A& a) { xwrite(s, dims(a)); }

//-----------------------------------------------------------------------------
// range serialization: serialization of dimensions followed by elements

template<typename X, typename S, typename A, only_if<is_range<A>{}> = 0>
void r_rng(X x, S& s, A& a) { size_t n = r_dims(x, s, a); r_elem(s, a, n); }

template<typename X, typename S, typename A, only_if<is_range<A>{}> = 0>
void w_rng(X x, S& s, const A& a) { w_dims(x, s, a); w_elem(s, a); }

template<typename X, typename S, typename A, only_if<!is_range<A>{}> = 0>
void r_rng(X, S& s, A& a) { support<A>(); }

template<typename X, typename S, typename A, only_if<!is_range<A>{}> = 0>
void w_rng(X, S& s, const A& a) { support<A>(); }

//-----------------------------------------------------------------------------
// classification into trivial/non-trival type

template<typename X, typename S, typename A, only_if<!is_triv<A>{}> = 0>
void r_main(X x, S& s, A& a) { r_rng(x, s, a); }

template<typename X, typename S, typename A, only_if<!is_triv<A>{}> = 0>
void w_main(X x, S& s, const A& a) { w_rng(x, s, a); }

template<typename X, typename S, typename A, only_if<is_triv<A>{}> = 0>
void r_main(X, S& s, A& a) { r_mem(s, &a); }

template<typename X, typename S, typename A, only_if<is_triv<A>{}> = 0>
void w_main(X, S& s, const A& a) { w_mem(s, &a); }

//-----------------------------------------------------------------------------
// classification into extended/non-extended serialization

template<typename S, typename A>
void read(S& s, A& a) { r_main(_false(), s, a); }

template<typename S, typename A>
void write(S& s, A& a) { w_main(_false(), s, a); }

template<typename S, typename A>
void xread(S& s, A& a) { r_main(_true(), s, a); }

template<typename S, typename A>
void xwrite(S& s, A& a) { w_main(_true(), s, a); }

//-----------------------------------------------------------------------------
// multi-argument generalizations for any data type

template<typename S, typename A, typename B, typename... C>
void read(S& s, A& a, B& b, C&... c) { read(s, a); read(s, b, c...); }

template<typename S, typename A, typename B, typename... C>
void write(S& s, const A& a, const B& b, const C&... c)
	{ write(s, a); write(s, b, c...); }

template<typename S, typename A, typename B, typename... C>
void xread(S& s, A& a, B& b, C&... c) { xread(s, a); xread(s, b, c...); }

template<typename S, typename A, typename B, typename... C>
void xwrite(S& s, const A& a, const B& b, const C&... c)
	{ xwrite(s, a); xwrite(s, b, c...); }

//-----------------------------------------------------------------------------
// function objects

struct xreader
{
	template<typename S, typename A, typename... B>
	void operator()(S& s, A& a, B&... b) { xread(s, a, b...); }
};

struct xwriter
{
	template<typename S, typename A, typename... B>
	void operator()(S& s, const A& a, const B&... b) { xwrite(s, a, b...); }
};

//-----------------------------------------------------------------------------
// open stream, optionally setting buffer, error handling; buffer can be
// a pointer + size, a built-in array or any contiguous range of the
// character type of the stream

template<typename S, typename F, typename T>
void xopen(S& s, const F& f, chr<S>* u, T n)
{
	if(u) setbuf(s, u, n);
	s.open(f, std::ios_base::binary);
	if(!s) throw e_open(f);
}

template<typename S, typename F, typename B>
void xopen(S& s, const F& f, B& b) { xopen(s, f, base(b), size(b)); }

template<typename S, typename F>
void xopen(S& s, const F& f) { xopen(s, f, nullptr, buffer_size()); }

//-----------------------------------------------------------------------------
// file operations, creating file streams from file names

template<typename F, typename A, typename... B>
void xload(const F& f, A& a, B&... b)
{
	std::ifstream s;
	std::vector<char> u(buffer_size());
	xopen(s, f, u); xread(s, a, b...);
}

template<typename F, typename A, typename... B>
void xsave(const F& f, const A& a, const B&... b)
{
	std::ofstream s;
	std::vector<char> u(buffer_size());
	xopen(s, f, u); xwrite(s, a, b...);
}

//-----------------------------------------------------------------------------
// convenience one-argument read/load operations; type A is required on call
// xread may be inefficient on large types, requiring additional copy

template<typename A, typename S>
A read(S& s) { A a; read(s, a); return a; }

template<typename A, typename S>
A xread(S& s) { A a; xread(s, a); return a; }

template<typename A, typename F>
A xload(F& f) { A a; xload(f, a); return a; }

//-----------------------------------------------------------------------------

}  // namespace xio_details

//-----------------------------------------------------------------------------

using xio_details::read;
using xio_details::write;
using xio_details::xread;
using xio_details::xwrite;
using xio_details::xopen;
using xio_details::xload;
using xio_details::xsave;

//-----------------------------------------------------------------------------

}  // namespace xio

//-----------------------------------------------------------------------------

#endif // XIO_IO
