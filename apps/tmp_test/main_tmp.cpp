#include <iostream>

#include <random>

#include <gnuplotpp/gnuplotpp.hpp>

int main(int argc, char** argv)
{
    using namespace lc;

    // In this way the output will be redirected to a file instead of gnuplot
    //Gnuplotpp gp(std::ofstream("a.p"));
    Gnuplotpp gp;

    gp.setTerm(Gnuplotpp::Term::PNG);

    Gnuplotpp::LineStyle lineStyle;
    lineStyle.lineColor = "red";

    Gnuplotpp::Marker markerStyle;
    markerStyle.pointType = Gnuplotpp::PointType::RhombusDot;
    markerStyle.pointSize = 1;

    auto myPlot = gp.errorbar({ .y = { 1, 2, 3, 4, 0 }, .yErr = { 0.5 }, .xErr = { 0.1, 0.2, 0.3, 1, 1 } }, { .lineStyle = lineStyle, .marker = markerStyle });
    //auto myPlot = gp.plot({ 1, 2, 3, 4, 0, 0 }, { .lineStyle = lineStyle, .marker = markerStyle });

    std::default_random_engine e;
    std::normal_distribution n;
    int N = 100;
    std::vector<double> x, y, dx, dy;
    for (int i = 0; i < N; i++)
    {
        x.push_back(n(e));
        y.push_back(n(e));
        dx.push_back(n(e) * 0.2);
        dy.push_back(n(e) * 0.2);
    }

    myPlot = gp.errorbar({ .y = y, .x = x, .yErr = dy, .xErr = dx }, { .marker = markerStyle });

    gp.draw({ myPlot });

    return 0;
}