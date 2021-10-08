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

	const char* Gnuplotpp::Datablock_E = "E";
	const char* Gnuplotpp::Datablock_EOF = "EOF";
	const char* Gnuplotpp::Datablock_EOD = "EOD";

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

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::plot(DataBuffer& buffer, PlotOptions options)
	{
		// TODO quando esco usa uno scope exit per annullare gnuplot, oppure
		// usa uno stringstream e poi passa tutto... ma potrebbe occupare troppa memoria

		auto& gp = *this;
#define _TMP_GNUPLOTPP_SPACE gp << " "
		
		gp << "plot '-'";

		_TMP_GNUPLOTPP_SPACE;

		// telling which data columns to plot
		{
			if (options.cols.size() <= 0)
				throw std::runtime_error("cannot plot no columns: options.cols.size() must be > 0");
			gp << "using ";
			for (auto it = options.cols.begin(); it != options.cols.end(); it++)
			{
				if (*it >= buffer.cols())
					throw std::runtime_error("cannot plot columns index higher than buffer columns");
				gp << (*it + 1);// note that gnuplot indices start from 1, therfore we have to add 1
				if (std::next(it) != options.cols.end())
					gp << ":";
			}
		}

		_TMP_GNUPLOTPP_SPACE;

		// setting plot title
		if (options.title)
		{
			gp << "title '" << options.title.value() << "'";
			_TMP_GNUPLOTPP_SPACE;
		}

		// passing data to gnuplot
		{
			// data on new line
			gp << std::endl;
			
			// write the data
			gp << buffer;

			// tell end of data
			gp << Datablock_EOD << std::endl;
			//gp << Datablock_E << std::endl;// <- this would do the same
		}

#undef _TMP_GNUPLOTPP_SPACE
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::m_plot(const std::vector<double>& data)
	{
		if constexpr (0)
		{
			auto name = "__tmp_aasdf__.txt";
			std::ofstream file(name);
			for (const auto& y : data)
				file << y << std::endl;
			file.close();
			//*this << "plot \"tmp.txt\" using 1:2 title 'step'" << std::endl;
			*this << "plot \"" << name << "\" using 1 title 'step'" << std::endl;
		}

		if constexpr (0)
		{
			DataBuffer buff;
			for (const auto& y : data)
				buff.push_row({ y });
			*this << "plot '-' using 1" << "\n";
			*this << buff;
			*this << Datablock_EOD << std::endl;
		}

		if constexpr (1)
		{
			DataBuffer buff;
			for (const auto& y : data)
				buff.push_row({ y });
			this->plot(buff);
		}
	}

	Gnuplotpp::DataBuffer::DataBuffer(size_t cols) :
		m_cols(cols)
	{
		assert(("cols must be > 0", cols > 0));
	}

	void Gnuplotpp::DataBuffer::push_row(std::list<double> row)
	{
		assert(("row size must be equal to the number of cols", row.size() == this->cols()));
		if (row.size() != this->cols())
			throw std::runtime_error("row size must be equal to the number of cols");

		for (const auto& x : row)
			m_data.push_back(x);
	}
}

std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::DataBuffer& buffer)
{
	for (size_t i = 0; i < buffer.rows(); i++)
	{
		for (size_t j = 0; j < buffer.cols(); j++)
		{
			ostream << buffer.at(i, j);
			if (j < buffer.cols() - 1)
				ostream << "\t";
		}
		ostream << std::endl;
	}

	return ostream;
}