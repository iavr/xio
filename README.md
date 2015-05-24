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

`xio/c++` is a template-only library, so requires no building. To use it, just include file [c++/xio](c++/xio)

To use `xio/matlab`, just add its folder [matlab/](matlab/) to the Matlab path.

Using `xio/matlab`
----------------