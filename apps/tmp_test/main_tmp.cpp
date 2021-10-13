#include <iostream>

#include <random>

#include <gnuplotpp/gnuplotpp.hpp>

int main_tmp(int argc, char** argv)
{
    using namespace lc;

    //Gnuplotpp gp(std::ofstream("a.p"));
    Gnuplotpp gp;

    //gp.setTerminal(Gnuplotpp::Terminal::PNG, "a.png", Gnuplotpp::Vector2i{ 400, 300 });
    gp.setTerminal(Gnuplotpp::Terminal::JPEG, "a.jpg", Vector2i{ 400, 300 });

    std::default_random_engine e;
    std::normal_distribution n;

    auto randVec = [&](size_t N, double mean = 0.0, double sigma = 1.0) -> std::vector<double>
    {
        std::vector<double> v; v.reserve(N);
        for (size_t i = 0; i < N; i++)
            v.push_back(n(e) * sigma + mean);
        return v;
    };

    auto p = gp.plot(randVec(100), randVec(100));

    gp.setTitle("Y vs X scatter plot");
    gp.xLabel("X");
    gp.yLabel("Y");

    // Enable majer and minor tics with the default values
    gp.setTicksOptions(Gnuplotpp::TicksOptions{});

    // Tells to plot both Major and Minor ticks
    gp.setGridOptions(Gnuplotpp::GridOptions{ true, true });

    gp.draw({ p });

    // grid
    {
        Gnuplotpp gp;
        gp.setTerminal(Gnuplotpp::Terminal::JPEG, "grid.jpg", Vector2i{ 400, 300 });

        auto plot = gp.plot(randVec(100), randVec(100));

        gp.setTitle("Y vs X scatter plot");
        gp.xLabel("X");
        gp.yLabel("Y");

        // Enable majer and minor tics with the default values
        gp.setTicksOptions(Gnuplotpp::TicksOptions{});

        // Tells to plot both Major and Minor ticks
        gp.setGridOptions(Gnuplotpp::GridOptions{ true, true });

        gp.draw({ plot });
    }

    // grid - style
    {
        Gnuplotpp gp;
        gp.setTerminal(Gnuplotpp::Terminal::JPEG, "grid-style.jpg", Vector2i{ 400, 300 });

        auto plot = gp.plot(randVec(100), randVec(100));

        gp.setTitle("Y vs X scatter plot");
        gp.xLabel("X");
        gp.yLabel("Y");

        // Ticks options
        Gnuplotpp::TicksOptions ticks;
        ticks.minorXdivider = ticks.minorYdivider = 3;
        gp.setTicksOptions(Gnuplotpp::TicksOptions{});

        // Create a line style for major ticks
        Gnuplotpp::LineStyle majorLineStyle, minorLineStyle;
        majorLineStyle.lineColor = "red";
        minorLineStyle.lineColor = Gnuplotpp::Color{ 0, 128, 255, 128 }; // RGBA

        // Tells to plot both Major and Minor ticks with different styles
        Gnuplotpp::GridOptions options;
        gp.setGridOptions(Gnuplotpp::GridOptions
            {
                .major = true,
                .minor = true,
                .majorLineStyle = majorLineStyle,
                .minorLineStyle = minorLineStyle
            }
        );

        gp.draw({ plot });
    }

    // errorbar
    {
        Gnuplotpp gp;
        gp.setTerminal(Gnuplotpp::Terminal::JPEG, "errorbar.jpg", Vector2i{ 400, 300 });
        gp.addOstream(std::make_unique<std::ofstream>("ee.p"));

        size_t N = 20;
        auto plot = gp.errorbar(
            // Data
            {
                .y = randVec(N),
                .x = randVec(N),
                .yErr = randVec(N, 0, 0.2),
                .xErr = randVec(N, 0, 0.2)
            },
            // Plot options
            {
                .title = "data"
            }
        );

        gp.setTitle("Errorbar");
        gp.xLabel("X");
        gp.yLabel("Y");
        gp.setTicksOptions(Gnuplotpp::TicksOptions{});
        gp.setGridOptions(Gnuplotpp::GridOptions{ true, true });
        gp.draw({ plot });
    }

    // overlapped
    {
        Gnuplotpp gp;
        gp.setTerminal(Gnuplotpp::Terminal::JPEG, "overlapped.jpg", Vector2i{ 400, 300 });

        size_t N = 20;
        auto plot1 = gp.errorbar(
            {
                .y = randVec(N),
                .x = randVec(N),
                .yErr = randVec(N, 0, 0.2),
                .xErr = randVec(N, 0, 0.2)
            },
            {
                .title = "data",
            }
        );

        Gnuplotpp::LineStyle lineStyle; lineStyle.lineColor = "blue";
        Gnuplotpp::Marker marker; marker.pointType = Gnuplotpp::PointType::RhombusDot; marker.pointSize = 2;
        auto plot2 = gp.plot(
            randVec(N),
            randVec(N),
            {
                .title = "data2",
                .lineStyle = lineStyle,
                .marker = marker
            }
        );

        gp.setTitle("Errorbar");
        gp.xLabel("X");
        gp.yLabel("Y");
        gp.setTicksOptions(Gnuplotpp::TicksOptions{});
        gp.setGridOptions(Gnuplotpp::GridOptions{ true, true });
        gp.draw({ plot1, plot2 });
    }

    // multiplot
    {
        Gnuplotpp gp;
        gp.setTerminal(Gnuplotpp::Terminal::JPEG, "multiplot.jpg", Vector2i{ 800, 600 });

        size_t N = 20;
        auto plot1 = gp.errorbar(
            {
                .y = randVec(N),
                .x = randVec(N),
                .yErr = randVec(N, 0, 0.2),
                .xErr = randVec(N, 0, 0.2)
            },
            {
                .title = "data",
            }
            );

        Gnuplotpp::LineStyle lineStyle; lineStyle.lineColor = "blue";
        Gnuplotpp::Marker marker; marker.pointType = Gnuplotpp::PointType::RhombusDot; marker.pointSize = 2;
        auto plot2 = gp.plot(
            randVec(N),
            randVec(N),
            {
                .title = "data2",
                .lineStyle = lineStyle,
                .marker = marker
            }
        );
        lineStyle.lineColor = "orange";
        auto plot3 = gp.plot(
            randVec(N),
            randVec(N),
            {
                .title = "data3",
                .lineStyle = lineStyle,
                .marker = marker
            }
        );

        gp.setTitle("Errorbar");
        gp.xLabel("X");
        gp.yLabel("Y");
        gp.setTicksOptions(Gnuplotpp::TicksOptions{});
        gp.setGridOptions(Gnuplotpp::GridOptions{ true, true });

        if (auto multiplot = gp.multiplot(2, 2))
        {
            gp.setTitle("Plot 1");
            gp.draw({ plot1 });
            gp.setTitle("Plot 2");
            gp.draw({ plot2 });
            gp.setTitle("Plot 3");
            gp.draw({ plot3 });
            gp.setTitle("Plot 4");
            gp.draw({ plot3 });
        }
    }

    // multiplot - size
    {
        Gnuplotpp gp;
        gp.setTerminal(Gnuplotpp::Terminal::JPEG, "multiplot-size.jpg", Vector2i{ 800, 600 });

        size_t N = 20;
        auto plot1 = gp.errorbar(
            {
                .y = randVec(N),
                .x = randVec(N),
                .yErr = randVec(N, 0, 0.2),
                .xErr = randVec(N, 0, 0.2)
            },
            {
                .title = "data",
            }
            );

        Gnuplotpp::LineStyle lineStyle; lineStyle.lineColor = "blue";
        Gnuplotpp::Marker marker; marker.pointType = Gnuplotpp::PointType::RhombusDot; marker.pointSize = 2;
        auto plot2 = gp.plot(
            randVec(N),
            randVec(N),
            {
                .title = "data2",
                .lineStyle = lineStyle,
                .marker = marker
            }
        );
        lineStyle.lineColor = "orange";
        auto plot3 = gp.plot(
            randVec(N),
            randVec(N),
            {
                .title = "data3",
                .lineStyle = lineStyle,
                .marker = marker
            }
        );

        gp.setTitle("Errorbar");
        gp.xLabel("X");
        gp.yLabel("Y");
        gp.setTicksOptions(Gnuplotpp::TicksOptions{});
        gp.setGridOptions(Gnuplotpp::GridOptions{ true, true });

        if (auto multiplot = gp.multiplot(2, 1))
        {
            // plot 2 height (relative to 1)
            double h = 1 / 2.5;

            gp.setTitle("Plot 1");
            gp.setOrigin({ 0, h });
            gp.setSize({ 1, 1 - h });
            gp.draw({ plot1 });

            gp.setTitle("Plot 2");
            gp.setSize({ 1, h });
            gp.draw({ plot2, plot3 });
        }
    }


    /*
    Gnuplotpp::LineStyle lineStyle;
    lineStyle.lineColor = "red";

    Gnuplotpp::Marker markerStyle;
    markerStyle.pointType = Gnuplotpp::PointType::RhombusDot;
    markerStyle.pointSize = 1;

    auto myPlot = gp.errorbar({ .y = { 1, 2, 3, 4, 0 }, .yErr = { 0.5 }, .xErr = { 0.1, 0.2, 0.3, 1, 1 } }, { .lineStyle = lineStyle, .marker = markerStyle });
    //auto myPlot = gp.plot({ 1, 2, 3, 4, 0, 0 }, { .lineStyle = lineStyle, .marker = markerStyle });

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

    //gp << "set multiplot layout 2, 2" << std::endl;
    //gp << "set multiplot" << std::endl;
    //gp << "set size 0.5,0.5" << std::endl;
    //gp << "set origin 0.0, 0.5" << std::endl;

    gp.setTerm(Gnuplotpp::Term::PNG, "a.png");
    if (auto multiplot = gp.multiplot(2, 1))
    {
        double p = 1.0/3;

        gp.setPlotOrigin({ 0, p });
        gp.setPlotSize({ 1, 1 - p });
        gp.draw({ myPlot });

        gp.setPlotSize({ 1, p });
        gp.draw({ myPlot });
    }
    */

    return 0;
}