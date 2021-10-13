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

#include <gnuplotpp/classes/Vector.hpp>

namespace lc
{

}

#ifndef _GNUPLOTPP_USE_LC_LIBRARY
std::ostream& operator<<(std::ostream& ostream, const lc::Vector2d& v)
{
	ostream << "(" << v.x() << ", " << v.y() << ")" << std::endl;
	return ostream;
}
#endif // !_GNUPLOTPP_USE_LC_LIBRARY