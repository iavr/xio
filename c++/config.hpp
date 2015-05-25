#include <array>
#include <vector>
#include <string>

#ifndef XIO_CONFIG
#define XIO_CONFIG

//-----------------------------------------------------------------------------

namespace xio {

//-----------------------------------------------------------------------------
// stream configuration

constexpr size_t buffer_size() { return 1 << 16; }

template<typename S, typename T>
void setbuf(S& s, typename S::char_type* b, T n) { s.rdbuf()->pubsetbuf(b, n); }

//-----------------------------------------------------------------------------
// contiguous container configuration

template<typename T>
std::false_type is_contiguous(const T&);

template<typename T, size_t N>
std::true_type is_contiguous(const T(&)[N]);

template<typename T, size_t N>
std::true_type is_contiguous(const std::array<T,N>&);

template<typename T>
std::true_type is_contiguous(const std::vector<T>&);

template<>
std::false_type is_contiguous(const std::vector<bool>&);

template<typename C, typename T>
std::true_type is_contiguous(const std::basic_string<C,T>&);

//-----------------------------------------------------------------------------

}  // namespace xio

//-----------------------------------------------------------------------------

#endif // XIO_CONFIG
