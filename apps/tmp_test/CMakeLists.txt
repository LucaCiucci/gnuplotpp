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

TODO

]]

################################################################
#                    basic configurations
################################################################

################################################################
#                        DEPENDENCIES
################################################################

################################################################
#                         EXECUTABLE
################################################################

add_executable("tmp_test")

add_dependencies("tmp_test" "gnuplotpp")
target_link_libraries("tmp_test" PRIVATE "gnuplotpp")

target_sources("tmp_test" PRIVATE "main.cpp")