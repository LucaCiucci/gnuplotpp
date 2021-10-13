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
#include <LC/utils/types/ScopeGuard.hpp>
#endif

namespace lc
{
	// ================================================================
	//                          SCOPE GUARD
	// ================================================================
#ifndef _GNUPLOTPP_USE_LC_LIBRARY

	// This is a class That executes some it is destroyed (for example if it goes out of scope)
	// https://github.com/LucaCiucci/LC/blob/master/include/LC/utils/types/ScopeGuard/ScopeGuard
	class ScopeGuard : NonCopyable
	{
	public:

		ScopeGuard(ScopeGuard&&) = default;
		ScopeGuard(std::function<void(void)> cleanup_function = []() {}) : m_cleanup_fcn(cleanup_function) {}
		~ScopeGuard() { if (m_cleanup_fcn) m_cleanup_fcn(); }

		void setFunction(std::function<void(void)> cleanup_function = []() {}) { m_cleanup_fcn = cleanup_function; }
		void release(void) { m_cleanup_fcn = {}; };

	private:
		std::function<void(void)> m_cleanup_fcn;
	};
#else // !_GNUPLOTPP_USE_LC_LIBRARY

	// Using LC's definition for ScopeGuard
	using ::lc::ScopeGuard;

#endif // !_GNUPLOTPP_USE_LC_LIBRARY
}