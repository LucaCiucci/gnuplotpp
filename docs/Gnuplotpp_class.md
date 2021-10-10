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

# The `Gnuplotpp` class

## Methods:

Constructors:
 - [`Gnuplotpp()`](#gnuplotppgnuplotpp)
 - [`Gnuplotpp(bool pesrist)`](#ciao)

Plot creation
 - [`Plot2d Gnuplotpp::plot()`]()

## Detailed descriptions

### Constructors

#### `Gnuplotpp::Gnuplotpp()`
<details>
<summary>Default constructor</summary>

This is the default constructor, it is the equivalent of telling `persist = true`

</details>

#### `Gnuplotpp::Gnuplotpp(bool pesrist)`

<details>
<summary>Default constructor</summary>

If `persist` is `true`, Gnuplot is started with `--persist` option

</details>