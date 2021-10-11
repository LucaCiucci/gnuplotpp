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

# Build and Install the *Gnuplot++* library

In this file you will find a brief description on how to build, install and use the *Gnuplot++* library.

If you are using *Visual Studio*, check [VS_guide](VS_guide).

## Requirements

To build *Gnuplot++* using CMake you need to fulfill some requirements:
 - having an *up-to-date* compiler
 - having an *up-to-date* CMake version
 - having the *LC* library installed [optional]

## Download the *Gnuplot++* library

You can download the library from the [main repository](https://github.com/LucaCiucci/gnuplotpp) and place it in a folder you like. If you want to contribute you may want to fork the repo and use a git to keep track of changes.

## Configuring CMake

If you are using CMake from a terminal, navigate to the library folder and type:
```sh
cmake .
```
This will configure the project.

### Changing CMake params
(You may want to read [this CMake documentation](https://cmake.org/cmake/help/latest/manual/cmake.1.html))
During build time, some CMake parameters are considered. To set a parameter, type:
```sh
cmake -Dparam=value .
```
instead of `cmake .`.

Some parameters:
 - `CMAKE_INSTALL_PREFIX`
 - `GNUPLOTPP_HEADER_ONLY`
 - `GNUPLOTPP_USE_LC_LIBRARY`

Example :
```sh
cmake -DGNUPLOTPP_USE_LC_LIBRARY=ON -DCMAKE_INSTALL_PREFIX="non_conventional_install_dir" .
```

## Build
Now, to build the whole projet, type:
```sh
cmake --build .
```
and after a few seconds the library should be built.

## Install
To install the library so that external projects can find and use it, type:
```sh
# Note: not cmake but make
make install
```

## Link from an external project

Example `CMakeLists.txt` for an extenal project:
```cmake
# project setup
# https://cmake.org/cmake/help/latest/guide/tutorial/index.html
project(
    "mySimpleProject" # your project name
    VERSION 0.0       # your project version
    DESCRIPTION "..." # your project brief description
    LANGUAGES CXX     # languages we will use (C++)
)

# create an executable target named "myProgram"
add_executable("myProgram")

# find the gnuplotpp package you have already installed
find_package(gnuplotpp REQUIRED)

# link the gnuplotpp library to our target
target_link_libraries("myProgram" PRIVATE "gnuplotpp")

# add sources to our target
target_sources("myProgram" PRIVATE "main.cpp")
```