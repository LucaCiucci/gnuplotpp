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

#include <chrono>
#include <thread>

// !!!
#include <iomanip>      // std::setw
#include <random>

#include <gnuplotpp/gnuplotpp.hpp>

int safe_main(int argc, char** argv);

int main_(int argc, char** argv)
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
	using lc::Gnuplotpp;

	//lc::Gnuplotpp gnuplot(std::ofstream("a.p"));
	lc::Gnuplotpp gnuplot;

	const auto cmd = R"qwerty(
plot '-' pt 6 ps 5, '-' pt 3 ps 5
1
3
2
1
EOF
2
5
6
2
3
EOF
)qwerty";

	gnuplot << cmd << std::endl;

	int a;
	std::vector<std::variant<std::reference_wrapper<int>, char>> va = { a, a, a, a, 'a' };
	va.push_back(a);
	for (int i = 0; i < va.size(); i++)
		std::cout << va[i].index() << std::endl;

	Gnuplotpp::SinglePlotOptions opt;
	opt.lineStyle = Gnuplotpp::LineStyle();
	opt.marker = Gnuplotpp::Marker();
	opt.marker.value().pointSize = 3;
	opt.lineStyle.value().lineColor = "red";
	opt.marker.value().pointType = Gnuplotpp::PointType::Cross;
	auto plot1 = gnuplot.plot({ 1, 2, 3, 2, 2, 2, 0, 1, 1, 1, 2 }, opt);
	opt.lineStyle.value().lineColor = "green";
	opt.lineStyle.value().dashType = "-";
	opt.marker.value().pointType = Gnuplotpp::PointType::Star;
	opt.axes = Gnuplotpp::PlotAxes::x1y2;
	auto plot2 = gnuplot.plot({ 1, 2, 3, 2, 2, 2, 10, 1, 1, 2, 2 }, opt);
	gnuplot.draw({ plot1, plot2 });

	return 0;

	//gnuplot << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;
	gnuplot << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;
	//gnuplot.plot_tmp({1, 2, 3, 2, 1});

	lc::Gnuplotpp::DataBuffer buffer;
	std::vector<double> v{ 1, 2, 2, 3, 2, 1, 1, 2, 1, 42, 40, 30, 20, 10, 0 };
	for (const auto& y : v)
		buffer << (int)y << lc::endRow;
	//gnuplot.plot(buffer);
	gnuplot.plot(std::vector<float>{ 42, 40, 30, 20, 10, 0 }/*, {.spacing = 0.1, .options = {.title = "Hello There!"}}*/);

	if (0)
	{//gnuplot << "set mxtics 2" << std::endl;
		gnuplot << "set style line 81 lt 0 lc rgb \"#808080\" lw 0.5" << std::endl;
		gnuplot << "unset grid" << std::endl;
		gnuplot << "set grid xtics" << std::endl;
		gnuplot << "set grid mxtics" << std::endl;
		gnuplot.setGridOptions({ true, true });
	}

	gnuplot.errorbar({ .y = {0, 1, 1, 2, 3, 10, 2, 1}, .yErr = {1, 1, 1, 1, 2, 1, 2, 5}}/*, {.options = {.title = "a"}}*/);
	gnuplot.errorbar({ .y = {0, 1, 1, 2, 3, 10, 2, 1}, .yErr = {1, 1, 1, 1, 2, 1, 2, 5}}/*, {.options = {.title = "a"}}*/);

	/*
	for (int i = 0; i <= 10*0 - 1; i++)
	{
		lc::Gnuplotpp::LineStyle ls;
		//ls.copyFrom = 3;
		ls.dashType = i;
		ls.index = 81;
		ls.lineColor = "red";
		gnuplot.lineStyle(ls);
		gnuplot.setGridOptions({true});

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
		gnuplot.errorbar({ .y = {0, 1, 1, 2, 3, 10, 2, 1}, .yErr = {1, 1, 1, 1, 2, 1, 2, 5} }, { .options = {.title = "a"} });
	}*/
	{
		lc::Gnuplotpp::LineStyle ls;
		//ls.index = 50;
		//ls.copyFrom = 3;
		ls.dashType = ".... - ... - .. - . -";
		ls.dashType = ".-_";
		ls.dashType = ".._";
		//ls.lineWidth = 4;
		//ls.index = 81;
		ls.lineColor = "red";
		//gnuplot.lineStyle(ls);
		auto ls2 = ls;
		//ls2.index = 51;
		ls2.lineColor = "blue";
		gnuplot.setGridOptions({ true, true });

		std::cout << std::hex << 42 << 42 << std::endl;
		std::cout << std::setw(2) << std::setfill('0') << std::hex << 2 << std::endl;
		std::cout << std::hex << 2 << std::endl;
		std::cout << std::hex << 2 << std::endl;

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
		gnuplot.errorbar({ .y = {0, 1, 1, 2, 3, 10, 2, 1}, .yErr = {1, 1, 1, 1, 2, 1, 2, 5} }/*, {.options = {.title = "a"}}*/);

		std::default_random_engine engine;
		std::normal_distribution n;
		std::vector<double> data;
		for (size_t i = 0; i < 10; i++)
			data.push_back(n(engine));

		/*
		lc::Gnuplotpp::SinglePlotOptions options;
		options.options.lineStyle = lc::Gnuplotpp::LineStyle{};
		//options.options.lineStyle.value().marker.value().pointType = Gnuplotpp::PointType::Cross;
		//options.options.lineStyle.value().dashType;
		options.options.marker.value().pointType = Gnuplotpp::PointType::Circle;
		options.options.marker.value().pointSize = 2;
		*/

		lc::Gnuplotpp gnuplot2(std::ofstream("gnuplot-commands.txt"));
		gnuplot.plot(data/*, options*/);
		//gnuplot2.plot(data, options);

		//gnuplot << "set multiplot" << std::endl;
		gnuplot.plot(
			// DATA
			{ 1, 3, 2, 2, 1, 0 },
			{
				.lineStyle = Gnuplotpp::LineStyle{ .lineColor = "red" },
				.marker = Gnuplotpp::Marker {
					.pointType = Gnuplotpp::PointType::Star,
					//.pointSize = 2.0
				},
			}
		);
		gnuplot.plot(
			// DATA
			{ 1, 3, 2, 4, 1, 0 },
			{
				.lineStyle = Gnuplotpp::LineStyle{.lineColor = "blue" },
				.marker = Gnuplotpp::Marker {
					.pointType = Gnuplotpp::PointType::Star,
					//.pointSize = 2.0
				},
				//.replot=true
			}
			);
		//gnuplot << "unset multiplot" << std::endl;
	}
	if (0)
	{
		std::default_random_engine engine;
		std::normal_distribution n;
		std::vector<double> data; data.reserve(100 * 1000);
		for (size_t i = 0; i < 100; i++)
		{
			for (int i = 0; i < 1000; i++)
				data.push_back(n(engine));
			gnuplot.plot(data);

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(0.01s);
		}
	}

	return 0;
}