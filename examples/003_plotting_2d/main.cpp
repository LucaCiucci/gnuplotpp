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
#include <string>
#include <random>
#include <vector>

#include <gnuplotpp/gnuplotpp.hpp>

const char* main_003_name = "Plotting 2D";
const char* main_003_description = R"qwerty(
TODO
)qwerty";

int main_003(void)
{
	using namespace lc;
	using namespace std::chrono_literals;

	std::default_random_engine e;
	std::normal_distribution n;

	// generates a random vector
	auto randVec = [&](size_t N, double mean = 0.0, double sigma = 1.0) -> std::vector<double>
	{
		std::vector<double> v; v.reserve(N);
		for (size_t i = 0; i < N; i++)
			v.push_back(n(e) * sigma + mean);
		return v;
	};

	size_t N = 20;
	std::string fileName = "out.pdf";

	// some styling
	{
		Gnuplotpp gp;
		
		// we will draw on a PDF file
		gp.setTerminal(Gnuplotpp::Terminal::PDF, fileName);

		// Pasic plot creation
		auto plot = Gnuplotpp::Plot2d(randVec(20));

		// draw the plot
		gp.setTitle("simple Y plot");
		gp.draw({ plot });

		plot.options.title = "data";
		gp.setTitle("changing plot name");
		gp.draw({ plot });

		gp.xLabel("X");
		gp.yLabel("Y");
		gp.setTitle("adding labels");
		gp.draw({ plot });

		// changing the marker
		plot.options.marker = Gnuplotpp::Marker();
		auto& marker = plot.options.marker.value();
		marker.pointType = Gnuplotpp::PointType::SolidCircle;
		gp.setTitle("changing the marker");
		gp.draw({ plot });

		// Adding line to the plot
		plot.options.lineStyle = Gnuplotpp::LineStyle();
		plot.options.lineStyle.value().lineColor = "blue";
		gp.setTitle("adding a line");
		gp.draw({ plot });

		// adding grid
		gp.setTicksOptions(Gnuplotpp::TicksOptions());
		gp.setGridOptions(Gnuplotpp::GridOptions());
		gp.setTitle("adding grid");
		gp.draw({ plot });
		
		// drawing also minor tics on the grid
		gp.setGridOptions(Gnuplotpp::GridOptions(true, true));
		gp.setTitle("changing the grid");
		gp.draw({ plot });

		gp.setTitle("changing data");
		plot.yData = randVec(N);
		gp.draw({ plot });

		gp.setTitle("adding x data");
		plot.xData = randVec(N);
		gp.draw({ plot });

		gp.setTitle("multiple plots");
		auto plot2 = plot; plot2.xData = {};
		auto plot3 = plot; plot3.xData = {};
		plot2.yData = randVec(N);
		plot3.yData = randVec(N);
		plot2.options.lineStyle.value().lineColor = Gnuplotpp::Color(255, 0, 128);
		plot3.options.lineStyle.value().lineColor = Gnuplotpp::Color(0, 128, 255);
		plot2.options.title = "data2";
		plot3.options.title = "data3";
		gp.draw({ plot2, plot3 });

		gp.setTitle("errorbar");
		Gnuplotpp::Errorbar err;
		err.x = randVec(N * 2);
		err.y = randVec(N * 2);
		err.xErr = randVec(N * 2, 0, 0.1);
		err.yErr = randVec(N * 2, 0, 0.1);
		err.options.title = "data";
		gp.draw({ err });

		gp.setGridOptions(Gnuplotpp::GridOptions(false, false));
		gp.setTitle({});
		gp.xLabel({});
		gp.yLabel({});
		if (auto multiplot = gp.multiplot(2, 2))
		{
			gp.draw({ err });
			gp.draw({ plot });
			gp.draw({ plot2, plot3 });
			gp.draw({ plot, err });
		}

		gp.setGridOptions(Gnuplotpp::GridOptions(true, true));
		if (auto multiplot = gp.multiplot(2, 1))
		{
			double h = 1.0 / 2.5;
			
			gp.setSize({1, 1 - h});
			gp.setOrigin({0, h});
			gp.draw({ err });

			gp.setGridOptions(Gnuplotpp::GridOptions(true, false));
			gp.setSize({ 1.0, h });
			gp.draw({ plot2 });
		}

		gp.setGridOptions(Gnuplotpp::GridOptions(true, true));
		gp.draw({ plot, plot2, plot3, err });

		std::cout << "plots are in \"" << fileName << "\"" << std::endl;
	}

	return EXIT_SUCCESS;
}