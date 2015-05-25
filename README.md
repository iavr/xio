# [xio](https://github.com/iavr/xio)

`xio` is a library for generic input/output of arbitrary data in an open binary format among different languages and platforms. It is currently in a very early stage of development, supporting very few basic functionalities in C++ and Matlab.

For each supported language, `xio` is an independent library. What is common is only the identical binary format and the similar but not indentical syntax.

## Licence

`xio` has a 2-clause BSD license. See file [LICENSE](/LICENSE) for the complete license text.

## Directory structure

There is one folder for each supported language. The following are currently available:

	/c++
	/matlab

## Requirements

`xio/c++` requires C++11 features that are available only in recent compilers. In particular, it has been tested on gcc 5.1 and clang 3.6.

`xio/matlab` has been tested on Matlab version 2014A, but should be fine with any relatively recent version.

## Installation

No installation is needed.

`xio/c++` is a template-only library, so requires no building. To use it, just `#include <xio>` and compile with `-I/path/to/xio/c++`.

To use `xio/matlab`, just `addpath('/path/to/xio/matlab')`.

## Usage

There is currently little functionality available, and only a small subset is documented below that is expected to remain stable. The functionality is not necessarily the same in all supported languages.

### `xio` binary format

Built-in types are directly copied from/to memory.

Containers are represented by their size as `uint64` followed by their elements.

N-dimensional arrays are similar, except that the size is replaced by the dimensions; these are expected to be one-dimensional containers, represented as above.

Nested containers are only supported in C++.

Fixed sizes, e.g. of built-in arrays and [`std::array`](http://en.cppreference.com/w/cpp/container/array) in C++, are not stored. This is only supported in C++.

There is currently no provision for switching between big/little endian integer representation.

### Using `xio/c++`

Arbitrary containers and n-dimensional arrays are currently supported. Because `xio/c++` is completely open with respect to data structures, some setup is needed. Let us define a minimal n-dimensional array structure:

	template<typename Data, typename Dims = std::vector<size_t>>
	struct array_nd : Data
	{
		Dims dims;
		using Data::Data;
	};

This structure uses an arbitrary structure `Data` for data storage and another structure `Dims` to represent its dimensions. We have to tell `xio` how to obtain the dimensions of a given `array_nd`:

	template<typename A, typename D>
	D& dims(array_nd<A, D>& array) { return array.dims; }

	template<typename A, typename D>
	const D& dims(const array_nd<A, D>& array) { return array.dims; }

We are now ready to go. Assuming we have e.g. an array

	using int_nd = array_nd<std::vector<int>>;
	int_nd a;

we can save it to file `name` with

	xio::xsave(name, a);

and load it with

	xio::xload(name, a);

Alternatively,

	a = xio::xload<int_nd>(name);

though this use may be less efficient.

Custom one-dimensional containers are easier to set up, since they do not need `dims()`. In general, `xio` considers an object to be a container as long as [`std::begin()`](http://en.cppreference.com/w/cpp/iterator/begin), [`std::end()`](http://en.cppreference.com/w/cpp/iterator/end) are defined on them. Other requirements include `clear()`, `resize()` or `insert()` depending on whether a container is contiguous or fixed. These are not precisely documented yet.

Built-in arrays and all C++ standard sequence and associative containers are supported without any setup, except `std::forward_list` and container adaptors. Arbitrarily nested containers are also supported, though not tested.

The basic element types should be [`std::is_trivially_copyable`](http://en.cppreference.com/w/cpp/types/is_trivially_copyable), but arbitrary types can be easily supported by extending `xio`. This is not tested or documented yet.

### Using `xio/matlab`

Arbitrary n-dimensional arrays of non-fixed size are currently supported. To save array `a` to file `name`, use

	xsave(name, a);

To load it, use

	a = xload(name);

On loading, the data type is automatically determined by the file extension:

	ext   type
	---   ----
	.u1   uint8
	.u2   uint16
	.u4   uint32
	.u8   uint64
	.i1   int8
	.i2   int16
	.i4   int32
	.i8   int64
	.f4   single
	.f8   double

On saving, the actual data type of `a` is ignored and the data are converted and saved according to extension as above.

It is possible to load only part of the data; this is not documented yet.
