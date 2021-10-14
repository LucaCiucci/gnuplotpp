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

#include <iostream>
#include <stdexcept>

#include "main_functions.hpp"

namespace
{
	int safe_main(int argc, char** argv);
}

int main(int argc, char** argv)
{
	try
	{
		return safe_main(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Uncaught Exception thrown from safe_main(): " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Uncaught Unknown Exception thrown from safe_main()" << std::endl;
	}
	return EXIT_FAILURE;
}

// ================================================================
//                          EXECUTION
// ================================================================

namespace
{
	int safe_main(int argc, char** argv)
	{
		std::cout << "Hello There!" << std::endl;

		auto examples = examples_ns::getExamples();

		std::cout << "Examples list:" << std::endl;
		for (size_t i = 0; i < examples.size(); i++)
			std::cout << " - example #" << (i + 1) << ": \"" << examples[i].name << "\"" << std::endl;

		std::cout << "Example to run? (0 to exit)" << std::endl;

		size_t i = 0;
		std::cin >> i;

		if (i == 0)
			return EXIT_SUCCESS;
		i--;

		// TODO check out of bound
		const auto& ex = examples[i];

		std::cout << "running example \"" << ex.name << "\"..." << std::endl;
		try
		{
			auto r = ex.main();
			std::cout << "\nThe example finished with exit code " << r << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "An exception of type \"" << typeid(e).name() <<  "\" occurred when running the example : " << e.what() << std::endl;
		}

		return EXIT_SUCCESS;
	}
}