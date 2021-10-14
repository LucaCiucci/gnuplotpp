/* LC_NOTICE_BEGIN
===============================================================================
|                        Copyright (C) 2021 Luca Ciucci                       |
|-----------------------------------------------------------------------------|
| Important notices:                                                          |
|  - This work is distributed under the MIT license, feel free to use this    |
|   work as you wish.                                                         |
|  - Read the license file for further info.                                  |
| Written by Luca Ciucci <luca.ciucci99@gmail.com>, 2021                      |
===============================================================================
LC_NOTICE_END */

// for std::cout and std::cin
#include <iostream>

// Include the Gnuplot++ header
#include <gnuplotpp/gnuplotpp.hpp>

const char* main_001_name = "Gnuplot++ initialization";
const char* main_001_description = R"qwerty(
TODO
)qwerty";

int main_001(void)
{
	// To simplify the code
	using namespace lc; // Gnuplotpp classes are in the 'lc' namspace

	// Basic usage
	// Create a Gnuplot instance (executes gnuplot with "--persist" option)
	Gnuplotpp gp;

	std::cout << "sending and example command, you should now see some plots" << std::endl;

	// sending an example command
	gp << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;

	return EXIT_SUCCESS;
}