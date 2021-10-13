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

#if defined(_GNUPLOTPP_USE_LC_LIBRARY)
#include <LC/utils/types/NonCopyable.hpp>
#endif

namespace lc
{
	// ================================================================
	//                       NON - COPYABLE
	// ================================================================
#ifndef _GNUPLOTPP_USE_LC_LIBRARY

	// Base class for non copyable objects. Intuitively for a non-copyable class id does
	// not make sense to be copyied or it could generare erroneus or undefined behaviour.
	// Copyied from the LC library
	// https://github.com/LucaCiucci/LC/blob/d709ae3e40c469be148c4d71c8b77eb8993151dc/include/LC/utils/types/NonCopyable/NonCopyable#L7
	class NonCopyable
	{
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;
	public:
		NonCopyable() = default;
		NonCopyable(NonCopyable&&) = default;
	};
#else // !_GNUPLOTPP_USE_LC_LIBRARY

	// Using LC's definition for NonCopyable
	using ::lc::NonCopyable;

#endif // !_GNUPLOTPP_USE_LC_LIBRARY
}