/* LC_NOTICE_BEGIN
LC_NOTICE_END */

#include <gnuplotpp/gnuplotpp.hpp>

#include <iostream>

#include <assert.h>

#if defined(_WIN32) || defined (_MSVC_VER)
#define _LC_GNUPLOT_POPEN _popen
#define _LC_GNUPLOT_PCLOSE _pclose
#else
#define _LC_GNUPLOT_POPEN popen
#define _LC_GNUPLOT_PCLOSE pclose
#endif

namespace lc
{
	namespace _gnuplot_impl_
	{
		////////////////////////////////////////////////////////////////
		bool GnuplotPipe::init(bool persist)
		{
			std::clog << "Opening gnuplot... ";

			// open the pipe
			m_pipe = _LC_GNUPLOT_POPEN(persist ? "gnuplot -persist" : "gnuplot", "w");

			if (!m_pipe)
			{
				std::clog << "failed!" << std::endl;
				return false;
			}
			std::clog << "succeded." << std::endl;
			(std::ofstream&)(*this) = std::move(std::ofstream(m_pipe));

			return (bool)m_pipe;
		}

		////////////////////////////////////////////////////////////////
		GnuplotPipe::~GnuplotPipe()
		{
			//if (m_pipe)
			//	_LC_GNUPLOT_PCLOSE(m_pipe);
		}
	}

	// ================================================================
	//                       GNUPLOT++ INTERFACE
	// ================================================================

	// ================================
	//          CONSTRUCTORS
	// ================================

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Gnuplotpp(bool persist)
	{
		// try to init the gnuplot pipe, in case of failure we will throw an error
		if (!GnuplotPipe::init(persist))
		{
#ifndef NDEBUG
			std::cerr << "could not init the gnuplot pipe, maybe gnuplot was not installed?" << std::endl;
#endif // !NDEBUG
			throw std::runtime_error("could not init the gnuplot pipe");
		}
	}

	// ================================
	//           DESTRUCTOR
	// ================================

	Gnuplotpp::~Gnuplotpp()
	{
		// TODO ...
	}

	// ================================
	//          COMUNICATION
	// ================================

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::sendLine(const std::string& line)
	{
		assert(("GnuplotPipe::sendLine requires an open pipe", this->isOpen()));
		if (!this->isOpen())
			throw std::runtime_error("GnuplotPipe::sendLine() requires an open pipe");

		*this << line << std::endl;
	}
}