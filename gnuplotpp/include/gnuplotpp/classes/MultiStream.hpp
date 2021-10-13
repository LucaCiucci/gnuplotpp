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

#include <ostream>
#include <list>

#include "NonCopyable.hpp"

namespace lc
{
	// ================================================================
	//                         MULTI-STREAM
	// ================================================================

	// This class allows us to use the << and >> operators on multiple streambuffers
	// TODO add to LC library
	class MultiStream : NonCopyable
	{
	public:

		// TODO concept
		template <class T>
		MultiStream& operator<<(const T& obj)
		{
			for (auto& ostream : m_ostreams)
				ostream << obj;
			return *this;
		}

		MultiStream& operator<<(std::ostream& (*endl)(std::ostream& os))
		{
			for (auto& ostream : m_ostreams)
				ostream << endl;
			return *this;
		}

		// Add a streambuffer to the multistream
		void addRdbuf(std::streambuf* streambuf);

	private:
		std::list<std::ostream> m_ostreams;
	};
}