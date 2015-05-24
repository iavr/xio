# [xio](https://github.com/iavr/xio)

`xio` is a library for generic input/output of arbitrary data in an open binary format among different languages and platforms. It is currently in a very early stage of development, supporting very few basic functionalities in C++ and Matlab.

Licence
-------

`xio` has a 2-clause BSD license. See file [LICENSE](/LICENSE) for the complete license text.

Directory structure
-------------------

There is one folder for each suported language. The following are currently available:

	/c++
	/matlab

Requirements
------------

`xio/c++` requires C++11 features that are available only in recent compilers. In particular, it has been tested on `gcc` 5.1 and `clang` 3.6.

`xio/matlab` has been tested on Matlab version 2014A, but should be fine with any relatively recent version.

Installation
------------

No particular setup is needed.

`xio/c++` is a template-only library, so requires no building. To use it, just `#include <xio>`.

To use `xio/matlab`, just add its folder [matlab/](matlab/) to the Matlab path.

## Using `xio/c++`
<!-- --------------- -->

Arbitrary n-dimensional arrays are currently supported. Because `xio/c++` is completely open with respect to data structures, some setup is needed. Let's define a minimal n-dimensional array structure:

	template<typename Data, typename Dims = std::vector<size_t>>
	struct array_nd : Data
	{
		Dims dims;
		using Data::Data;
	};

This structure uses an arbitrary structure `Data` for data storage and another structure `Dims` to represent its dimensions. We have to tell `xio` whether `array_nd` uses contiguous storage, so that i/o is efficient:

	namespace xio {
		template<typename A, typename D>
		struct is_contiguous<array_nd<A,D>> : is_contiguous<A> {};
	}

Finally, we have to tell it how to obtain the dimensions of a given array:

	template<typename A, typename D>
	D& dims(array_nd<A, D>& array) { return array.dims; }

	template<typename A, typename D>
	const D& dims(const array_nd<A, D>& array) { return array.dims; }

We are now ready to go. Assuming we have e.g. an array

	array_nd<std::vector<int>> a;

we can save it to file `name` with

	xsave(name, a);

To load it, use

	xload(name, a);

Built-in arrays and all C++ standard sequence and associative containers are also supported, except `forward_list` and container adaptors, as well as arbitrarily nested containers, though not tested. Arbitrary n-dimensional arrays can be supported as shown above.

The basic element types should be [`std::is_trivially_copyable`](http://en.cppreference.com/w/cpp/types/is_trivially_copyable), but arbitrary types can be supported by extending `xio`. This is not tested or documented yet.

Using `xio/matlab`
------------------

Arbitrary n-dimensional arrays are currently supported. To save array `a` to file `name`, use

	xsave(name, a);

To load it, use

	a = xload(name);

On loading, the data type is automatically determined by the file extension:

	ext   type
	u1    uint8
	u2    uint16
	u4    uint32
	u8    uint64
	i1    int8
	i2    int16
	i4    int32
	i8    int64
	f4    single
	f8    double

On saving, the actual data type of `a` is ignored and the data are converted and saved according to extension as above.

It is possible to load only part of the data; this is not documented yet.