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

<!-- https://github.com/badges/shields/blob/master/README.md -->
<!-- https://shields.io/ -->

# *Gnuplot++* ![GitHub](https://img.shields.io/github/license/LucaCiucci/gnuplotpp) ![GitHub issues](https://img.shields.io/github/issues/LucaCiucci/gnuplotpp) ![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/LucaCiucci/gnuplotpp) ![GitHub top language](https://img.shields.io/github/languages/top/LucaCiucci/gnuplotpp) [![](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.com/donate?hosted_button_id=6WDUWJUNWKKP4)
A simple C++ Gnuplot interface.


This is a very simple library that allows you to call the [Gnuplot](http://www.gnuplot.info/) program from C++ code using a pipe.

Make sure to [install Gnuplot](http://www.gnuplot.info/download.html) before using this library (on Windows you have to add Gnuplot to PATH by selecting the option during installation or by adding the bin folder to PATH by hand). To test your gnuplot installation you can open a terminal and type `gnuplot --version`.

Also take a look at the [Gnuplot manual](http://www.gnuplot.info/docs_5.0/gnuplot.pdf) and the [gnuplot++ wiki](https://github.com/LucaCiucci/gnuplotpp/wiki).

If you don't know how to use *C++*, take a look at [IDontKnowCpp](./docs/IDontKnowCpp.md) guide

## Features

 - extremely simple and easy to use
 - no need of knowing gnuplot syntax
 - simple `std::cout` like interaction for sending commands using the `<<` operator
 - modern `C++` style interation using high level of abstraction functions
 - no messing with Gnuplot indices
 - no temporary data files needed
 - easy plot customization
 - MIT license

## Usage
Using the `Gnuplotpp` class is extremely esasy.

Take a look at the [Including](./docs/Including.md) guide to discover how you can use the _Gnuplot++_ library in your project, then read the [Quick Start guide](./docs/QuickStart.md) to rapidly start using the library

You can send commands like you would do in the gnuplot shell:
```cpp
#include <gnuplotpp/gnuplotpp.hpp>
// ...
Gnuplotpp gp;
gp << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;
```
or use the interface functions to easily create complex and customizable plots:
```cpp
Gnuplotpp gp;
auto myPlot = gnuplot.plot({ 1, 2, 3, 2 });
gp.render({ myPlot });
```

## Why *Gnuplot*

TODO

## Why not *Gnuplot*

TODO

## Contributing

 - read [`CODE_OF_CONDUCT.md`](./CODE_OF_CONDUCT.md) and [`CONTRIBUTING.md`](CONTRIBUTING.md)

You can **fork** the project and create your own version.

If you want to tell a problem, suggest a feature or request assistance for a problem that can be shared, create an **issue**.

If you want to directly contribute to the main project, create a **fork** and then create a pull request that I will review. Please, read the rules in the [`CONTRIBUTING.md`](CONTRIBUTING.md) file before creating a pull request.

If you appreciate my work, please consider making a donation using the [Donate](https://www.paypal.com/donate?hosted_button_id=6WDUWJUNWKKP4) button: **STUDENTS NEED COFFEE :coffee:**

---
---
---
---
---
---
---
---
---
---
---
---
---
---

# OLD

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