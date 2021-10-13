<!-- LC_NOTICE_BEGIN
===============================================================================
|                        Copyright (C) 2021 Luca Ciucci                       |
|-----------------------------------------------------------------------------|
| Important notices:                                                          |
|  - This work is distributed under the MIT license, feel free to use this    |
|   work as you wish.                                                         |
|  - Read the license file for further info.                                  |
| Written by Luca Ciucci <luca.ciucci99@gmail.com>, 2021                      |
===============================================================================
LC_NOTICE_END -->

# Detailed usage of the *Gnuplot++* library

 - [Introduction](#introduction)
 - [Assumptions](#assumptions)
 - [Where does the output is?](#where-does-the-output-is)
 - [Basic plotting](#basic-plotting)
   - [A basic plot](#a-basic-plot)
   - [Title and labels](#title-and-labels)
   - [X and Y plot](#y-and-y-plot)
 - [Plot Styles](#plot-styles)
   - [Grid](#grid)
   - [Errorbar](#errorbar)
   - [Overlapped plots](#overlapped-plots)

## Introduction

At the current state, *Gnuplot++* uses a **pipe** to comunicate with the *Gnuplot* execution.

There is a class named `GnuplotPipe` that launches a *Gnuplot* execution with a pipe using a custom `streambuf` named `PipeStreamBuf`.  
`PipeStreamBuf` is created as follow:
```cpp
// PipeStreamBuf inherits from std::streambuf
PipeStreamBuf pipeStreamBuf("<command to execute>");
```
To start a Gnuplot session, the `GnuplotPipe` class creates `PipeStreamBuf` with the command `gnuplot [--persist]`.

`GnuplotPipe` is a `stream` and it can be created as follow:
```cpp
GnuplotPipe pipe; // type 'pipe(false)' to remove '--presist'
```
This pipe is an `ostream` and it can therefore accept data using the "<<" operator, for example:
```cpp
pipe << "some dommand or data" << std::endl;
```

Note that `endl` sends a new line and flushes the buffer (like pressing <kbd>Enter</kbd> on the keyboard).

We could now plot using the following syntax:
```cpp
pipe << "plot [-pi/2:pi] cos(x),-(sin(x) > sin(x+1) ? sin(x) : sin(x+1))" << std::endl;
```

But this is pretty boring since it is *text-based*. For this reason, ther is another class called `Gnuplotpp` that takes care of some of this mess and exposes some simple functiions:
```cpp
class Gnuplotpp {};
Gnuplotpp gp; // Gnuplotpp is basically a 'GnuplotPipe'
```

## Assumptions

Let's assume that, in our code, we wrote:
```cpp
#include <gnuplotpp/gnuplotpp.hpp>
// ...
Gnuplotpp gp;
```
and:
```cpp
#include <random>
#include <vector>
// ...
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
```

## Where does the output is?

`Gnuplotpp` by default puts commands on the pipe it has opened with gnuplot. You could redirect it to a file using:
```cpp
// writes the output commands in a file named "out.p" instead of sending commands to gnuplot
Gnuplotpp gp(std::ofstream("out.p"));
```

Another way is the following:
```cpp
// Creates a standard live gnuplot session
Gnuplotpp gp;

// Tell to type the commands sent to Gnuplot also on cout.
// You can add other buffers if you want.
gp.addRdbuf(std::cout.rdbuf());

// You can even write to other ostream, for example a file:
gp.addOstream(std::make_unique<std::ofstream>("out.p"));
```

`Gnuplotpp` is a `MultiStream`, this meas every command is sent to every streambuffer, for example, the statement:
```cpp
gp << "<some command>";
```
sends `"<some command>"` to:
  1. the `GnuplotPipe` i.e. to Gnuplot
  2. prints on `cout` (because we've passed its streambuffer)
  3. writes to `"out.p"` file

## Basic plotting

Plotting coes as follows:
  1. create some plots: `plot1`, `plot2`, ...
  2. use the `Gnuplotpp::draw()` function to actually draw the plots by sending the commands

Plots can be created in various ways, for example creating a basic 2d plot:
```cpp
// Create a plot object and then set data and options
Gnuplotpp::Plot2d plot;
plot.yData = {/* values */};
plot.options; // you can set options
```
You could also use a constructor, for example:
```cpp
auto plot = Gnuplotpp::Plot2d({/* values */}, {/* options */});
```
or a `Gnuplotpp` static function aimed to simplify plot creation, for example:
```cpp
auto p = Gnuplotpp::plot({/* values */}, {/* options */});
// also gp.plot() is the same function
```

---

TODO update and reorganize from here to EOF

---

### A basic plot

```cpp
// Creates a basic plot with the given data.
// Data is passed as a 'double' 'vector' altough any
// container of values convertible to 'double' is accepted
// (for example std::list<float>)
auto myPlot = gp.plot({ 0, 1, 2, 3, 4, 3, 2, 1, 0, 5, -1});

// draw the plot. In the curvy braces we write the list of plots to draw
gp.draw({ myPlot });
```
![](img/usage/basic_plot.png)

### Title and labels

```cpp
// ...

// Set plot title
gp.setTitle("Your Title");

// Set plot axes labels
gp.xLabel("X");
gp.yLabel("Y");

// You have to set title and labels before drawing!
gp.draw({ p });
```
![](img/usage/title_labels.png)

To unset something, just call, for example, `gp.yLabel()` with no params.

### X and Y plot
```cpp
// Plot data with X and Y values
auto p = gp.plot(randVec(100), randVec(100));
gp.draw({ p });
```
![](img/usage/xy.png)

## Plot Styles

### Grid

```cpp
// Enable majer and minor tics with the default values
gp.setTicksOptions(Gnuplotpp::TicksOptions{});

// Tells to plot both Major and Minor ticks
gp.setGridOptions(Gnuplotpp::GridOptions{ true, true });

gp.draw({ p });
```
![](img/usage/grid.jpg)

#### Changing Grid Style
```cpp
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
```
![](img/usage/grid-style.jpg)

### Errorbar
```cpp
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
gp.draw({ plot });
```

![](img/usage/errorbar.jpg)

### Overlapped plots
```cpp
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

// !!!
gp.draw({ plot1, plot2 });
```
![](img/usage/overlapped.jpg)

### Multiplot
```cpp
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
```
![](img/usage/multiplot.jpg)

#### Changing sizes
```cpp
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
```
![](img/usage/multiplot-size.jpg)

---
---
---
---
---
---
---
---
---
---
---
---
---
---
---
---
---
---
---
---
---
---