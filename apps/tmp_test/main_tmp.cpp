#include <iostream>

#include <gnuplotpp/gnuplotpp.hpp>

int main(int argc, char** argv)
{
    using namespace lc;

    // In this way the output will be redirected to a file instead of gnuplot
    Gnuplotpp gp(std::ofstream("a.p"));

    Gnuplotpp::LineStyle lineStyle;
    lineStyle.lineColor = "red";

    Gnuplotpp::Marker markerStyle;
    markerStyle.pointType = Gnuplotpp::PointType::RhombusDot;
    markerStyle.pointSize = 3;

    auto myPlot = gp.plot({ 1, 2, 3, 4, 0 }, { .lineStyle = lineStyle, .marker = markerStyle });

    gp.draw({ myPlot });

    return 0;
}