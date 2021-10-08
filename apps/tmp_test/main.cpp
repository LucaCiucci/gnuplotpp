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

#include <iostream>

#include <gnuplotpp/gnuplotpp.hpp>

int safe_main(int argc, char** argv);

int main(int argc, char** argv)
{
	try
	{
		return safe_main(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cerr << "uncaught exception thrown from safe_main: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "uncaught unknown exception thrown from safe_main" << std::endl;
	}

	return EXIT_FAILURE;
}

int safe_main(int argc, char** argv)
{
	lc::Gnuplotpp gnuplot;

	//gnuplot << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;
	gnuplot << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;;
	gnuplot.plot_tmp({1, 2, 3, 2, 1});

	return 0;
}