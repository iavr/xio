#include <sstream>
#include <numeric>
#include <cstdint>

#ifndef XIO_FUN
#define XIO_FUN

//-----------------------------------------------------------------------------

namespace xio {

//-----------------------------------------------------------------------------

namespace xio_details {

//-----------------------------------------------------------------------------
// inline stringstream

struct ss
{
	std::stringstream s;
	template<typename A>
	ss& operator<<(const A& a) { s << a; return *this; }
	operator std::string() { return s.str(); }
};

//-----------------------------------------------------------------------------
// file open exception

struct e_open : std::exception
{
	std::string msg;
	e_open(const std::string& f) :
		msg(ss() << "cannot open file " << f << "\n") {}
	const char* what() const noexcept override { return msg.c_str(); }
};

//-----------------------------------------------------------------------------
// array base pointer, only via begin(); this is exactly where abstraction
// is sacrificed for efficient serialization of contiguous arrays

template<typename A>
auto base(A& a) -> decltype(&*std::begin(a)) { return &*std::begin(a); }

//-----------------------------------------------------------------------------
// size() for arrays that support it, otherwise via begin()/end()

template<typename A, only_if<!has_size<A>{}> = 0>
size_t size(const A& a) { return std::distance(std::begin(a), std::end(a)); }

template<typename A, only_if<has_size<A>{}> = 0>
size_t size(const A& a) { return a.size(); }

//-----------------------------------------------------------------------------
// default dimensions for arbitrary 1-dimensional range, represented as
// 64-bit unsigned integer

template<typename A, only_if<is_range<A>{}> = 0>
uint64_t dims(const A& a) { return size(a); }

//-----------------------------------------------------------------------------
// product of elements of a range (1 if empty)

template<typename A>
elem<A> prod(const A& a)
{
	using T = elem<A>;
	return std::accumulate(a.begin(), a.end(), T(1), std::multiplies<T>());
}

//-----------------------------------------------------------------------------
// total length of range with dimensions given in `a`; if a is scalar, the
// length is just `a`; if it is a container, the length is zero if `a` is
// empty and the product of its elements otherwise

template<typename A, only_if<is_range<A>{}> = 0>
size_t total(const A& a) { return size(a) ? prod(a) : 0; }

template<typename A, only_if<!is_range<A>{}> = 0>
size_t total(const A& a) { return a; }

//-----------------------------------------------------------------------------
// resize() if contiguous range of trivial elements (read by memory copy),
// clear() otherwise (read by insert())

template<typename A, only_if<is_cont_triv<A>{}> = 0>
void resize(A& a, size_t n) { a.resize(n); }

template<typename A, only_if<!is_cont_triv<A>{}> = 0>
void resize(A& a, size_t) { a.clear(); }

//-----------------------------------------------------------------------------
// fixed if cannot be resized, in one way or another

template<typename A> using _resize = decltype(resize(gen<A&>(), 0));
template<typename A> using is_fixed = expr<!sfinae<_resize, A>{}>;

//-----------------------------------------------------------------------------
// two different syntaxes for insert: sequence and associate containers

template<typename A, typename I, only_if<is_seq<A>{}> = 0>
void insert(A& a, I b, I e) { a.insert(a.end(), b, e); }

template<typename A, typename I, only_if<!is_seq<A>{}> = 0>
void insert(A& a, I b, I e) { a.insert(b, e); }

//-----------------------------------------------------------------------------
// only trivial and range types currently unsupported

template<typename A>
void support()
{
	static_assert(is_triv<A>(), "Object type unsupported for serialization. Consider extending xread()/xwrite().");
}

//-----------------------------------------------------------------------------
// set stream buffer only if supported

//-----------------------------------------------------------------------------

}  // namespace xio_details

//-----------------------------------------------------------------------------

}  // namespace xio

//-----------------------------------------------------------------------------

#endif // XIO_FUN
