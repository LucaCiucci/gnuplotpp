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
#                         EXECUTABLE
################################################################

add_executable("examples")

################################################################
#                        DEPENDENCIES
################################################################

add_dependencies("examples" "gnuplotpp")

################################################################
#                     EXTERNAL LIBRARIES
################################################################

################################################################
#                         LIBRARIES
################################################################

target_link_libraries("examples" PRIVATE "gnuplotpp")

################################################################
#                          SOURCES
################################################################

target_sources(
    "examples"
    PRIVATE
        "main.cpp" "main_functions.hpp"

        # examples
        "001_GP_initialization/main.cpp"
        "002_GP_output/main.cpp"
        "003_plotting_2d/main.cpp"
)

################################
#   PREPROCESSOR DEFINITIONS
################################