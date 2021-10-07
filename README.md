<!-- LC_NOTICE_BEGIN
===============================================================================
|                        Copyright (C) 2021 Luca Ciucci                       |
|-----------------------------------------------------------------------------|
| Important notices:                                                          |
|  - This work is distributed under the MIT license, feel free to use this    |
|   work as you wish.                                                         |
|  - Read the license file for further info.                                  |
| Written by Luca Ciucci <luca.ciucci99@gmail.com>, 2021                      |
===============================================================================
LC_NOTICE_END -->

# gnuplot++
A simple C++ Gnuplot interface.

This is a very simple library that allows you to call the [Gnuplot](http://www.gnuplot.info/) program from C++ code using a pipe.

Make sure to [install Gnuplot](http://www.gnuplot.info/download.html) before using this library (on Windows you have to add Gnuplot to path by selecting the option during installation or by adding the bin folder to PATH by hand). To test your gnuplot installation you can open a terminal and type `gnuplot --version`.

Also take a look at the [Gnuplot manual](http://www.gnuplot.info/docs_5.0/gnuplot.pdf).

## Usage

You have several options to use this library (see below):
- using as a single header library (simplest)
- building and installing (recommended)
- adding both `.hpp` and `.cpp` files into your project

Once you have followed one of the methods, you can simply use the `GnuplotInterface` class like this:
```cpp
// TODO
```

See the [gnuplot++ wiki](https://github.com/LucaCiucci/gnuplotpp/wiki) for additional usege tips.

### Using as a single header library

This is the simplest option: you have to place the `gnuplotpp.hpp` and the `gnuplotpp.cpp` file in your favourite location, for example near your source file, then write:
```cpp
#define _LC_GNUPLOTPP_HEADER_ONLY
#include "gnuplotpp.hpp"
```

Optionally, if you are using CMake as build system:
```cmake
target_compile_definitions(
  your_target_name
  PUBLIC
    _LC_GNUPLOTPP_HEADER_ONLY
)
```
and then, in your C++ code simply:
```C++
#include "gnuplotpp.hpp"
```

If you are using a different build system, you can add the `_LC_GNUPLOTPP_HEADER_ONLY` definition in different ways.

### Building and installing

This is the recommended option if you are using CMake as build system.

On linux or any other system where you can call cmake from console you can navigate to the project folder and type:
```
cmake .
```
this will configure the project, it sould take a few seconds.

Then you have to build the library, type:
```
cmake --build .
```
or
```
make
```

And after another few seconds the library will be built. To install, type:
```
make install
```

At this point you can use the library adding the header path and linking the lib. With CMake this is much simpler:
```cmake
find_package(gnuplotpp REQUIRED)
target_link_libraries(your_target_name PUBLIC gnuplotpp) # or PRIVATE
```

Please, read the [wiki Building and Installing page](https://github.com/LucaCiucci/gnuplotpp/wiki/Building-and-Installing) for more detailed info and common problems solutions.

### Adding `.hpp` and `.cpp` fiels to your project

This is another simple option: you add both `gnuplotpp.hpp` and `gnuplotpp.cpp` to your project so that `gnuplotpp.cpp` gets compiled.

## Contributing

Read `CONTRIBUTING.md`.