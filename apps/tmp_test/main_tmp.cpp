
#include <iostream>
#include <random>
#include <vector>

#include <gnuplotpp/gnuplotpp.hpp>
using namespace lc;

int main(int argc, char** argv)
{
	// Create a gnuplot insance
	Gnuplotpp gp(std::ofstream("a.p"));

	// generate some random numbers
	std::default_random_engine engine;
	std::normal_distribution generator;
	std::vector<double> data;
	std::vector<double> data2;
	for (size_t i = 0; i < 5; i++)
	{
		data.push_back(generator(engine));
		data2.push_back(data.back() * data.back());
	}

	// Marker type and size
	Gnuplotpp::Marker marker;
	marker.pointType = Gnuplotpp::PointType::Circle;
	marker.pointSize = 2;

	// Setting a line with red color
	Gnuplotpp::LineStyle lineStyle;
	lineStyle.lineColor = "red";

	// create a plot
	auto myPlot = gp.plot(
		// plot data
		data, data2,
		// plot options
		{
			.lineStyle = lineStyle,
			.marker = marker
		}
	);

	// render the plot
	gp.render({ myPlot });

	return 0;
}