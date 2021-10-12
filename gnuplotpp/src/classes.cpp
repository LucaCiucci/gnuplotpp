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

#include <gnuplotpp/classes.hpp>

#include "pipe.hpp"

namespace lc
{
	// ================================================================
	//                       PIPE Streambuf
	// ================================================================

	// https://en.cppreference.com/w/cpp/io/basic_streambuf
	// http://www.cplusplus.com/reference/ostream/ostream/
	// https://codereview.stackexchange.com/questions/185490/custom-ostream-for-a-println-like-function
	// https://siware.dev/007-cpp-custom-streambuf/
	// https://siware.dev/007-cpp-custom-streambuf/
	// https://blog.csdn.net/tangyin025/article/details/50487544

	////////////////////////////////////////////////////////////////
	PipeStreamBuf::PipeStreamBuf(const std::string& command, size_t buffSize)
	{
		// TODO is a buffer necessary? maybe directly print on the pipe
		m_buff.resize(buffSize);

		m_pipe = _LC_GNUPLOT_POPEN(command.c_str(), "w");
		//m_pipe = std::fopen("a.txt", "w");

		if (!m_pipe)
			throw std::runtime_error("could not open the pipe");
	}

	////////////////////////////////////////////////////////////////
	PipeStreamBuf::~PipeStreamBuf()
	{
		// for flush ??? I don't know if it is necessary
		this->sync();

		if (m_pipe)
			_LC_GNUPLOT_PCLOSE(m_pipe);
	}

	////////////////////////////////////////////////////////////////
	int PipeStreamBuf::sync(void)
	{
		// we call overflow that will flush the content of the buffer
		if (this->overflow() == std::char_traits<char>::eof())
			// error
			return -1;

		// success
		return 0;
	}

	////////////////////////////////////////////////////////////////
	PipeStreamBuf::int_type PipeStreamBuf::overflow(int_type ch)
	{
		std::string s;

		// number of char in the buffer
		auto N = this->pptr() - this->pbase();

		s = std::string(this->pbase(), this->pptr());

		if (ch != std::char_traits<char>::eof())
		{
			s += ch;
			N++;
		}

		if (N > 0)
			// print the data on the pipe
			std::fprintf(m_pipe, "%s", s.c_str());

		// flush the pipe
		std::fflush(m_pipe);

		// reset pointers
		this->setp(m_buff.data(), m_buff.data() + m_buff.capacity());

		// success
		// giusto ?!?!? not_eof?
		//return std::char_traits<char>::eof() + 1;
		return std::char_traits<char>::not_eof(ch);
	}

	// ================================================================
	//                         MULTI-STREAM
	// ================================================================

	////////////////////////////////////////////////////////////////
	void MultiStream::addRdbuf(std::streambuf* streambuf)
	{
		m_ostreams.emplace_back(streambuf);
	}
}


#ifndef _GNUPLOTPP_USE_LC_LIBRARY
std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::Vector2d& v)
{
	ostream << "(" << v.x << ", " << v.y << std::endl;
	return ostream;
}
#endif // !_GNUPLOTPP_USE_LC_LIBRARY