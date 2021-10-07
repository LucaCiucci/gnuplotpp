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

#include <string>
//#include <ostream>
#include <fstream>

#if defined(_WIN32) || defined (_MSVC_VER)
#include <Windows.h>
#endif

namespace lc
{
	// TDOO read http://videocortex.io/2017/custom-stream-buffers/

	namespace _gnuplot_impl_
	{
		// https://github.com/LucaCiucci/LC/blob/d709ae3e40c469be148c4d71c8b77eb8993151dc/include/LC/utils/types/NonCopyable/NonCopyable#L7
		class NonCopyable
		{
			NonCopyable(const NonCopyable&) = delete;
			NonCopyable& operator=(const NonCopyable&) = delete;
		public:
			NonCopyable() = default;
			NonCopyable(NonCopyable&&) = default;
		};

		class GnuplotPipe : NonCopyable, public std::ofstream
		{
		protected:

			bool init(bool persist);

			bool isOpen(void) const { return m_pipe; };

			operator bool() const { return this->isOpen(); };

		public:

			~GnuplotPipe();

		private:
			FILE* m_pipe = {};
		};
	}

	// ================================================================
	//                       GNUPLOT++ INTERFACE
	// ================================================================

	class Gnuplotpp : public _gnuplot_impl_::GnuplotPipe
	{
	public:

		// ================================
		//          CONSTRUCTORS
		// ================================

		// Default constructor
		Gnuplotpp(bool persist = true);

		// default move costructor
		Gnuplotpp(Gnuplotpp&&) = default;

		// ================================
		//           DESTRUCTOR
		// ================================

		~Gnuplotpp();

		// ================================
		//          COMUNICATION
		// ================================

		void sendLine(const std::string& line);

	private:
	};
}

// ================================================================================================================================
// ================================================================================================================================
//                                                               INL
// ================================================================================================================================
// ================================================================================================================================

namespace lc
{
	// ================================================================
	//                       GNUPLOT++ INTERFACE
	// ================================================================

	// ================================
	//          CONSTRUCTORS
	// ================================

	// ================================
	//           DESTRUCTOR
	// ================================

	// ================================
	//          COMUNICATION
	// ================================
}