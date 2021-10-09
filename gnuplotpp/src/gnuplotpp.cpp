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

// !!!
#include <chrono>
#include <thread>
#include <iomanip>      // std::setw

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

	////////////////////////////////////////////////////////////////
	std::string Gnuplotpp::LineStyle::str(void) const
	{
		std::stringstream ss;

		ss << " " << this->index;

		if (this->copyFrom)
			ss << " lt " << this->copyFrom.value();// "lt" = "linetype"

		if (this->lineWidth)
			ss << " lw " << this->lineWidth.value();

		if (this->lineColor)
		{
			// "lc" = "linecolor"

			if (const auto* pString = std::get_if<std::string>(&this->lineColor.value()))
				ss << " lc \"" << *pString << "\"";
			if (const auto* pColor = std::get_if<Color>(&this->lineColor.value()))
				ss << " lc rgb \"#" << *pColor << "\"";
		}

		if (this->dashType)
		{
			// "dt" = "dashtype"

			if (const auto* pInt = std::get_if<int>(&this->dashType.value()))
				ss << " dt " << *pInt;
			if (const auto* pString = std::get_if<std::string>(&this->dashType.value()))
				ss << " dt \"" << *pString << "\"";
		}

		if (this->pointType)
			ss << " lc " << this->pointType.value();

		if (this->pointSize)
			ss << " lc " << this->pointSize.value();

		std::cout << "ls: " << ss.str() << std::endl;
		return ss.str();
	}

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

		this->setTicksOptions({});
		this->setGridOptions({});
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
	void Gnuplotpp::resetSession(void)
	{
		auto& gp = *this;

		//https://stackoverflow.com/questions/44813827/setting-line-opacity-in-gnuplot-without-using-a-hex-code
		gp << "reset session" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::plot(const std::vector<double>& data, SinglePlotOptions singlePlotOptions)
	{
		// temporary buffer
		DataBuffer buffer;

		if (singlePlotOptions.spacing)
		{
			buffer = DataBuffer(2);

			size_t counter = 0;
			for (const auto& y : data)
				buffer << ((counter++) * singlePlotOptions.spacing.value()) << y << endRow;
			singlePlotOptions.options.cols = { 0, 1 };
		}
		else
		{
			for (const auto& y : data)
				buffer << y << endRow;
			singlePlotOptions.options.cols = { 0 };
		}

		this->plot(buffer, singlePlotOptions.options);
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::plot(DataBuffer& buffer, PlotOptions options)
	{
		// TODO quando esco usa uno scope exit per annullare gnuplot, oppure
		// usa uno stringstream e poi passa tutto... ma potrebbe occupare troppa memoria

		// alias "gnuplot"
		auto& gp = *this;
		//std::stringstream gp;

		// this macro is used to put a space
#define _TMP_GNUPLOTPP_SPACE gp << " "
		
		// start the plot commandline, we insert '-' to tell gnuplot to use
		// a datablock that we will write later
		gp << "plot '-'";

		_TMP_GNUPLOTPP_SPACE;

		// tell which data columns to plot
		{
			// we need at least a column to plot
			if (options.cols.size() <= 0)
				throw std::runtime_error("cannot plot no columns: options.cols.size() must be > 0");

			// to tell wich columns to use, we need to type something like:
			// > plot ... using 1:2
			// where 1:2 means: use the first and second column as coordinates, another option is
			// to simply write something like:
			// > plot ... using 1
			// to plot only using ordinate values
			gp << "using ";

			// now we type the column numbers:
			for (auto it = options.cols.begin(); it != options.cols.end(); it++)
			{
				if (*it >= buffer.cols())
					throw std::runtime_error("cannot plot columns index higher than buffer columns");

				// note that gnuplot indices start from 1, therfore we have to add 1
				gp << (*it + 1);

				// insert a ":" between every number
				if (std::next(it) != options.cols.end())
					gp << ":";
			}

			_TMP_GNUPLOTPP_SPACE;
		}

		if (options.errorBars)
		{
			gp << "with ";

			switch (options.errorBars.value())
			{
			case lc::Gnuplotpp::ErrorBar::X:
				gp << "xerr";
				break;
			case lc::Gnuplotpp::ErrorBar::Y:
				gp << "yerr";
				break;
			case lc::Gnuplotpp::ErrorBar::XY:
				gp << "xyerrorbars";
				break;
			default:
				break;
			}

			_TMP_GNUPLOTPP_SPACE;
		}

		// setting plot title only if set
		if (options.title)
		{
			// TODO check no special characters in title

			// tell gnuplot the plot title
			gp << "title '" << options.title.value() << "'";

			_TMP_GNUPLOTPP_SPACE;
		}

		// passing data to gnuplot
		// see https://stackoverflow.com/questions/3318228/how-to-plot-data-without-a-separate-file-by-specifying-all-points-inside-the-gnu
		{
			// data on new line
			gp << std::endl;
			
			// write the data
			gp << buffer;

			// tell End Of Data
			gp << Datablock_EOD << std::endl;
			//gp << Datablock_E << std::endl;// <- this would do the same
		}

		//std::cout << gp.str() << std::endl;

#undef _TMP_GNUPLOTPP_SPACE
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::errorbar(ErrorbarData data, SinglePlotOptions singlePlotOptions)
	{
		auto& options = singlePlotOptions.options;

		struct ColumnIndices
		{
			std::optional<size_t> x, y, dx, dy;
		};
		ColumnIndices indices;

		{
			double spacing = singlePlotOptions.spacing ? singlePlotOptions.spacing.value() : 1;
			if (data.x.size() <= 0)
				for (size_t i = 0; i < data.y.size(); i++)
					data.x.push_back(i * spacing);
		}

		size_t indicesCounter = 0;
		{
			if (data.x.size() > 0)
				indices.x = indicesCounter++;
			indices.y = indicesCounter++;
			if (data.xErr.size() > 0)
				indices.dx = indicesCounter++;
			if (data.yErr.size() > 0)
				indices.dy = indicesCounter++;
		}

		if ((!indices.dx) && (!indices.dy))
			throw std::runtime_error("Gnuplotpp::errorbar() requires errors on at least one axis");
		if (indices.x)
			if (data.x.size() != data.y.size())
				throw std::runtime_error(
					(std::string)"in Gnuplotpp::errorbar(), data.x.size() must be equal to data.y.size() but sizes were: "
					+ std::to_string(data.x.size()) + ", " + std::to_string(data.y.size()));
		if (indices.dx)
			if (data.xErr.size() > 1)
				if (data.xErr.size() != data.y.size())
					throw std::runtime_error(
						(std::string)"in Gnuplotpp::errorbar(), data.xErr.size() must be equal to data.y.size() or a single value but sizes were: "
						+ std::to_string(data.xErr.size()) + ", " + std::to_string(data.y.size()));
		if (indices.dy)
			if (data.yErr.size() > 1)
				if (data.yErr.size() != data.y.size())
					throw std::runtime_error(
						(std::string)"in Gnuplotpp::errorbar(), data.yErr.size() must be equal to data.y.size() or a single value but sizes were: "
						+ std::to_string(data.yErr.size()) + ", " + std::to_string(data.y.size()));

		{
			if (indices.dx)
				options.errorBars = ErrorBar::X;
			if (indices.dy)
				options.errorBars = ErrorBar::Y;
			if (indices.dx && indices.dy)
				options.errorBars = ErrorBar::XY;
		}

		auto buildErrs = [&](std::vector<double>& v) -> void
		{
			if (v.size() == 1)
				std::fill(v.begin(), v.end(), v.front());
		};
		buildErrs(data.xErr);
		buildErrs(data.yErr);

		DataBuffer buffer(indicesCounter);

		{
			auto& cols = options.cols;
			cols.clear();
			if (indices.x) cols.push_back(indices.x.value());
			if (indices.y) cols.push_back(indices.y.value());
			if (indices.dx) cols.push_back(indices.dx.value());
			if (indices.dy) cols.push_back(indices.dy.value());
		}
		for (size_t i = 0; i < data.y.size(); i++)
		{
			if (indices.x)
				buffer << data.x[i];

			if (indices.y)
				buffer << data.y[i];

			if (indices.dx)
				buffer << data.xErr[i];

			if (indices.dy)
				buffer << data.yErr[i];

			buffer << endRow;
		}

		// TODO singlePlotOptions.spacing

		this->plot(buffer, options);
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setTicksOptions(TicksOptions options)
	{
		auto& gp = *this;

		gp << "set mxtics " << options.minorXdivider << std::endl;
		gp << "set mytics " << options.minorYdivider << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setGridOptions(GridOptions options)
	{
		auto& gp = *this;
		
		// unset the previous grid options
		gp << "unset grid" << std::endl;

		// grid ticks
		//gp << "show style line" << std::endl;
		//gp << "show grid" << std::endl;
		if (options.major.x) gp << "set grid xtics" << std::endl;
		if (options.major.y) gp << "set grid ytics" << std::endl;
		if (options.minor.x) gp << "set grid mxtics" << std::endl;
		if (options.minor.y) gp << "set grid mytics" << std::endl;

		// grid level
		switch (options.gridLevel)
		{
		case GridOptions::GridLevel::LayerDefault:
			gp << "set grid layerdefault" << std::endl;
			break;
		case GridOptions::GridLevel::Front:
			gp << "set grid front" << std::endl;
			break;
		case GridOptions::GridLevel::Back:
			gp << "set grid back" << std::endl;
			break;
		default:
			break;
		}

		this->lineStyle(options.majorLineStyle);
		this->lineStyle(options.minorLineStyle);
		gp << "set grid ls " << options.majorLineStyle.index << ", ls " << options.minorLineStyle.index << std::endl;

		if constexpr (0)
		{
			//gp << "set style line 81 default" << std::endl;
			//gp << "set style line 81 lt 2 lw 2 pt 3 ps 0.5" << std::endl;
			LineStyle ls;
			//ls.copyFrom = 3;
			ls.dashType = 1;
			ls.index = 81;
			ls.lineColor = "red";
			//gp.lineStyle(ls);
			gp << "set grid ls 81" << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::lineStyle(LineStyle style)
	{
		auto& gp = *this;

		gp << "unset style line " << style.index << std::endl;
		gp << "set style line " << style.str() << std::endl;
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
			this->plot(buff, {.title="serena"});
		}
	}

	// ================================================================
	//                      GNUPLOT++ DATA BUFFER
	// ================================================================

	////////////////////////////////////////////////////////////////
	Gnuplotpp::DataBuffer& endRow(Gnuplotpp::DataBuffer& buff)
	{
		buff.push_row(buff.m_tmpData);
		buff.m_tmpData.clear();
		return buff;
	}

	// ================================
	//          CONSTRUCTORS
	// ================================

	////////////////////////////////////////////////////////////////
	Gnuplotpp::DataBuffer::DataBuffer(size_t cols) :
		m_cols(cols)
	{
		assert(("cols must be > 0", cols > 0));
	}

	// ================================
	//             DATA
	// ================================

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::DataBuffer::push_row(std::list<double> row)
	{
		if (row.size() != this->cols())
			throw std::runtime_error("row size must be equal to the number of cols");

		for (const auto& x : row)
			m_data.push_back(x);
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::DataBuffer& Gnuplotpp::DataBuffer::operator<<(const double value)
	{
		m_tmpData.push_back(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::DataBuffer& Gnuplotpp::DataBuffer::operator<<(std::function<DataBuffer&(DataBuffer&)> f)
	{
		return f(*this);
	}
}

std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::DataBuffer& buffer)
{
	// we write data in the form:
	// 0 1 2 3
	// 4 5 6 10

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
std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::Color& color)
{
	// TODO con std::format
	ostream << std::hex
		<< std::setw(2) << std::setfill('0') << (int)color.transparency
		<< std::setw(2) << std::setfill('0') << (int)color.r
		<< std::setw(2) << std::setfill('0') << (int)color.g
		<< std::setw(2) << std::setfill('0') << (int)color.b
		<< std::dec;
	return ostream;
}