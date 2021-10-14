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

#pragma once

#include <vector>
#include <functional>
#include <string>

// !!!
#include <stdexcept>

extern const char* main_001_name;
extern const char* main_001_description;
extern int main_001(void);

extern const char* main_002_name;
extern const char* main_002_description;
extern int main_002(void);

extern const char* main_003_name;
extern const char* main_003_description;
extern int main_003(void);

namespace examples_ns
{
	struct Example
	{
		std::string name;
		std::string description;
		std::function<int(void)> main;
	};

	std::vector<Example> getExamples(void)
	{
		std::vector<Example> examples;


		{
			Example ex;
			ex.name = main_001_name;
			ex.description = main_001_description;
			ex.main = main_001;
			examples.push_back(ex);
		}

		{
			Example ex;
			ex.name = main_002_name;
			ex.description = main_002_description;
			ex.main = main_002;
			examples.push_back(ex);
		}

		{
			Example ex;
			ex.name = main_003_name;
			ex.description = main_003_description;
			ex.main = main_003;
			examples.push_back(ex);
		}

		return examples;
	}
}