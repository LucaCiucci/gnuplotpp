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

#if defined(_WIN32) || defined (_MSVC_VER)
#include <Windows.h>
#define _LC_GNUPLOT_POPEN _popen
#define _LC_GNUPLOT_PCLOSE _pclose
#else
#define _LC_GNUPLOT_POPEN popen
#define _LC_GNUPLOT_PCLOSE pclose
#endif