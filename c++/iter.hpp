#include <iterator>
#include <cstddef>

#ifndef XIO_ITER
#define XIO_ITER

//-----------------------------------------------------------------------------

namespace xio {

//-----------------------------------------------------------------------------

namespace xio_details {

//-----------------------------------------------------------------------------

inline void check(bool) {}  // TODO: exception

//-----------------------------------------------------------------------------
// forward declarations

struct xreader;
struct xwriter;

//-----------------------------------------------------------------------------
// input stream iterator

template<typename S, typename T, typename F = xreader>
class istream_iter :
	public std::iterator<std::input_iterator_tag, T, ptrdiff_t, const T*, const T&>
{
	using I = istream_iter;
	S* stream;
	T elem;
	bool ok;

	void read()
	{
		ok = stream && *stream;
		if(ok) { F()(*stream, elem); ok = *stream; }
	}

public:
	constexpr istream_iter() : stream(nullptr), elem(), ok(false) {}
	istream_iter(S& s)       : stream(&s), elem() { read(); }
	istream_iter(const I& i) : stream(i.stream), elem(i.elem), ok(i.ok) {}

	const T& operator*() const { check(ok); return elem; }
	const T* operator->() const { return &(operator*()); }

	I& operator++() { check(ok); read(); return *this; }
	I operator++(int) { check(ok); I i = *this; read(); return i; }

	bool equal(const I& i) const
	{
		return (ok == i.ok) && (!ok || stream == i.stream);
	}
};

//-----------------------------------------------------------------------------
// input stream iterator equality

template<typename S, typename T, typename F>
inline bool
operator==(const istream_iter<S,T,F>& x, const istream_iter<S,T,F>& y)
	{ return x.equal(y); }

template<typename S, typename T, typename F>
inline bool
operator!=(const istream_iter<S,T,F>& x, const istream_iter<S,T,F>& y)
	{ return !x.equal(y); }

//-----------------------------------------------------------------------------
// output stream iterator

template<typename S, typename T, typename F = xwriter>
class ostream_iter :
	public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	using I = ostream_iter;
	S* stream;

	public:
	ostream_iter(S& s) : stream(&s) {}
	ostream_iter(const I& i) : stream(i.stream) {}

	I& operator=(const T& e) { check(stream); F()(*stream, e); return *this; }

	I& operator*() { return *this; }
	I& operator++() { return *this; }
	I& operator++(int) { return *this; }
};

//-----------------------------------------------------------------------------
// begin()/end() constructors

template<typename T, typename F = xwriter>
istream_iter<std::istream, T, F>
begin(std::istream& s, T& e) { return istream_iter<std::istream, T, F>(s, e); }

template<typename T, typename F = xwriter>
istream_iter<std::istream, T, F>
end(std::istream&) { return istream_iter<std::istream, T, F>(); }

template<typename T, typename F = xwriter>
ostream_iter<std::ostream, T, F>
begin(std::ostream& s) { return ostream_iter<std::ostream, T, F>(s); }

template<typename T, typename F = xwriter>
ostream_iter<std::ostream, T, F>
end(std::ostream&) { return ostream_iter<std::ostream, T, F>(); }

//-----------------------------------------------------------------------------

}  // namespace xio_details

//-----------------------------------------------------------------------------

}  // namespace xio

//-----------------------------------------------------------------------------

#endif // XIO_ITER
