#include <array>
#include <vector>
#include <string>

#ifndef XIO_CONFIG
#define XIO_CONFIG

//-----------------------------------------------------------------------------

namespace xio {

//-----------------------------------------------------------------------------

constexpr size_t buffer_size() { return 1 << 16; }

//-----------------------------------------------------------------------------

template<typename T>
struct is_contiguous : std::false_type {};

template<typename T, size_t N>
struct is_contiguous<T(&)[N]> : std::true_type {};

template<typename T, size_t N>
struct is_contiguous<std::array<T,N>> : std::true_type {};

template<typename T>
struct is_contiguous<std::vector<T>> : std::true_type {};

template<>
struct is_contiguous<std::vector<bool>> : std::false_type {};

template<typename C, typename T>
struct is_contiguous<std::basic_string<C,T>> : std::true_type {};

//-----------------------------------------------------------------------------

}  // namespace xio

//-----------------------------------------------------------------------------

#endif // XIO_CONFIG
