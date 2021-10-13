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
#include <vector>
#include <streambuf>

#include "NonCopyable.hpp"

namespace lc
{
	// ================================================================
	//                        PIPE Streambuf
	// ================================================================

	// This class represents a stream buffer for a pipe
	// TODO add to LC library
	class PipeStreamBuf : public std::streambuf, NonCopyable
	{
	public:

		PipeStreamBuf(const std::string& command, size_t buffSize = 1 << 10);

		~PipeStreamBuf() override;

		// check if the pipe is open
		bool isOpen(void) const { return m_pipe; };

		// check if the pipe is open
		operator bool() const { return this->isOpen(); };

	protected:

		// this is the function that synchronizes the pipe (i.e. flushes)
		int sync(void) override;

		// this is the function that takes a sequence of chars and puts it in the
		// buffer optionally calling overflow()
		//std::streamsize xsputn(const char* ptr, std::streamsize count) override;

		// this is the function the flushes the buffer into the pie
		int_type overflow(int_type ch = std::char_traits<char>::eof()) override;


	private:
		std::vector<char> m_buff;
		FILE* m_pipe;
	};
}