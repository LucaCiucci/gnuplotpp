#[[ LC_NOTICE_BEGIN
===============================================================================
|                        Copyright (C) 2021 Luca Ciucci                       |
|-----------------------------------------------------------------------------|
| Important notices:                                                          |
|  - This work is distributed under the MIT license, feel free to use this    |
|   work as you wish.                                                         |
|  - Read the license file for further info.                                  |
| Written by Luca Ciucci <luca.ciucci99@gmail.com>, 2021                      |
===============================================================================
LC_NOTICE_END ]]

################################################################
#                           INFO
################################################################
#[[

Top level CMake project, see README.md for info

]]

################################################################
#                    basic configurations
################################################################

cmake_minimum_required (VERSION 3.20)

################################################################
#                          OPTIONS
################################################################

option(GNUPLOTPP_HEADER_ONLY "build and install as header only library" OFF)
option(GNUPLOTPP_USE_LC_LIBRARY "use the lc library" ON)

################################################################
#                          PROJECT
################################################################
# main project definition

project("gnuplotpp" VERSION 0.0
    DESCRIPTION "..."
    LANGUAGES CXX
)

# add_subdirectory("cmake")

################################
#         SUBPROJECTS
################################

add_subdirectory("gnuplotpp")
add_subdirectory("apps")