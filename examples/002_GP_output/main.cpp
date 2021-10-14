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
#include <thread>
#include <chrono>
#include <string>

#include <gnuplotpp/gnuplotpp.hpp>

const char* main_002_name = "Gnuplot++ output commands redirecting";
const char* main_002_description = R"qwerty(
TODO
)qwerty";

int main_002(void)
{
	using namespace lc;
	using namespace std::chrono_literals;

	auto t = 2s;

	// On 'std::cout'
	{
		Gnuplotpp gp;

		// tell gnuplot to write the output also on cout 
		gp.addRdbuf(std::cout.rdbuf());

		// tell gnuplot to write the output also on a file
		gp.addOstream(std::make_unique<std::ofstream>("out.p"));

		std::cout << "sending and example command, you should now see some plots, the command on cout and the out also in a file named \"out.p\"" << std::endl;

		gp << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;
	}

	std::this_thread::sleep_for(t);

	{
		// Output on file only
		Gnuplotpp gp(std::ofstream("out2.p"));

		std::cout << "Now writing on a file only" << std::endl;

		gp << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;

		std::cout << "output is in \"out2.p\"" << std::endl;

		std::cout << "Now we will try to run the file directly calling gnuplot" << std::endl;
		std::string command = "gnuplot --persist \"out2.p\"";
		std::this_thread::sleep_for(t * 2);
		std::system(command.c_str());
	}

	return EXIT_SUCCESS;
}