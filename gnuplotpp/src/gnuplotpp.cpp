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

// this macro is used to put a space
#define _TMP_GNUPLOTPP_SPACE(s) s << " "

namespace lc
{
	namespace _gnuplot_impl_
	{
		// ================================================================
		//                       PIPE Streambuf
		// ================================================================

		////////////////////////////////////////////////////////////////
		PipeStreamBuf::PipeStreamBuf(const std::string& command, size_t buffSize)
		{
			// TODO: is a buffer necessary? maybe directly print on the pipe
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
				// print the data
				std::fprintf(m_pipe, s.c_str());

			std::fflush(m_pipe);

			// reset pointers
			this->setp(m_buff.data(), m_buff.data() + m_buff.capacity());

			// success
			// giusto ?!?!? not_eof?
			//return std::char_traits<char>::eof() + 1;
			return std::char_traits<char>::not_eof(ch);
		}

		// ================================================================
		//                         GNUPLOT PIPE
		// ================================================================

		////////////////////////////////////////////////////////////////
		GnuplotPipe::GnuplotPipe(bool persist) :
			std::ostream{ &m_pipeStreamBuf },
			m_pipeStreamBuf{ persist ? "gnuplot --persist" : "gnuplot" }
		{
			// necessary ???
			this->rdbuf(&m_pipeStreamBuf);
		}

		////////////////////////////////////////////////////////////////
		void MultiStream::addRdbuf(std::streambuf* streambuf)
		{
			m_ostreams.emplace_back(streambuf);
		}
	}

	// ================================================================
	//                       GNUPLOT++ INTERFACE
	// ================================================================

	const char* Gnuplotpp::Datablock_E = "E";
	const char* Gnuplotpp::Datablock_EOF = "EOF";
	const char* Gnuplotpp::Datablock_EOD = "EOD";

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::MarkerSerializer::prepare(Gnuplotpp& os) const
	{
		// nothing to do
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::MarkerSerializer::print(Gnuplotpp& os) const
	{
		// pt = pointtype
		// ps = pointsize

		if (m_m.pointType)
		{
			_TMP_GNUPLOTPP_SPACE(os);
			if (const auto* pInt = std::get_if<int>(&m_m.pointType.value()))
				os << "pt " << *pInt;
			if (const auto* pType = std::get_if<PointType>(&m_m.pointType.value()))
				os << "pt " << static_cast<std::underlying_type_t<const PointType>>(*pType);
		}

		if (m_m.pointSize)
		{
			_TMP_GNUPLOTPP_SPACE(os);
			os << "ps " << m_m.pointSize.value();
		}
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::LineStyleSerializer::prepare(Gnuplotpp& os) const
	{
		auto id = m_id->id;
		m_id->onExit = [&os, id]() { os << "unset style line " << id; };

		if (m_ls.marker)
		{
			m_pms = std::make_unique<MarkerSerializer>(m_ls.marker.value());
			m_pms->prepare(os);
		}
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::LineStyleSerializer::print(Gnuplotpp& os) const
	{
		// "lt" = "linetype"
		// "lc" = "linecolor"

		os << "unset style line " << m_id->id << std::endl;

		os << "set style line " << m_id->id;

		if (m_ls.copyFrom)
		{
			_TMP_GNUPLOTPP_SPACE(os);
			os << "lt " << m_ls.copyFrom.value();
		}

		if (m_ls.lineWidth)
		{
			_TMP_GNUPLOTPP_SPACE(os);
			os << "lw " << m_ls.lineWidth.value();
		}

		if (m_ls.lineColor)
		{
			_TMP_GNUPLOTPP_SPACE(os);
			if (const auto* pString = std::get_if<std::string>(&m_ls.lineColor.value()))
				os << "lc \"" << *pString << "\"";
			if (const auto* pColor = std::get_if<Color>(&m_ls.lineColor.value()))
				os << "lc rgb \"#" << *pColor << "\"";
		}

		if (m_ls.dashType)
		{
			// "dt" = "dashtype"
			_TMP_GNUPLOTPP_SPACE(os);
			if (const auto* pInt = std::get_if<int>(&m_ls.dashType.value()))
				os << "dt " << *pInt;
			if (const auto* pString = std::get_if<std::string>(&m_ls.dashType.value()))
				os << "dt \"" << *pString << "\"";
		}

		if (m_pms)
			m_pms->print(os);

		os << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::PlotOptionsSerializer::prepare(Gnuplotpp& os) const
	{
		if (m_opt.lineStyle)
		{
			if (m_opt.marker)
			{
				m_opt.lineStyle.value().marker = m_opt.marker;
				m_opt.marker = {};
			}

			m_lsSserializer = std::make_unique<LineStyleSerializer>(os.createNewID(), m_opt.lineStyle.value());
			m_lsSserializer->prepare(os);
			m_lsSserializer->print(os);
		}

		if (m_opt.marker)
		{
			m_pms = std::make_unique<MarkerSerializer>(m_opt.marker.value());
			m_pms->prepare(os);
		}
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::PlotOptionsSerializer::print(Gnuplotpp& os) const
	{
		// cols
		{
			_TMP_GNUPLOTPP_SPACE(os);

			// to tell wich columns to use, we need to type something like:
			// > plot ... using 1:2
			// where 1:2 means: use the first and second column as coordinates, another option is
			// to simply write something like:
			// > plot ... using 1
			// to plot only using ordinate values
			os << "using ";

			// now we type the column numbers:
			for (auto it = m_opt.cols.begin(); it != m_opt.cols.end(); it++)
			{
				// TODO this cekck in another function
				if constexpr (0)
				{
					//if (*it >= buffer.cols())
					//	throw std::runtime_error("cannot plot columns index higher than buffer columns");
				}

				// note that gnuplot indices start from 1, therfore we have to add 1
				os << (*it + 1);

				// insert a ":" between every number
				if (std::next(it) != m_opt.cols.end())
					os << ":";
			}
		}
	
		if (m_opt.errorBars)
		{
			switch (m_opt.errorBars.value())
			{
			case ErrorBar::X:
				_TMP_GNUPLOTPP_SPACE(os);
				os << "with ";
				os << ((m_opt.lineStyle) ? "xerrorlines" : "xerr");
				break;
			case ErrorBar::Y:
				_TMP_GNUPLOTPP_SPACE(os);
				os << "with ";
				os << ((m_opt.lineStyle) ? "yerrorlines" : "yerr");
				break;
			case ErrorBar::XY:
				_TMP_GNUPLOTPP_SPACE(os);
				os << "with ";
				os << ((m_opt.lineStyle) ? "xyerrorlines" : "xyerrorbars");
				break;
			default:
				break;
			}
		}

		// setting plot line style
		if (m_lsSserializer)
		{
			_TMP_GNUPLOTPP_SPACE(os);

			if (!m_opt.errorBars)
				// TODO change ?!?!?
				os << "with linespoint ";

			os << "ls " << m_lsSserializer->m_id->id;
		}

		// setting plot title only if set
		if (m_opt.title)
		{
			_TMP_GNUPLOTPP_SPACE(os);

			// TODO check no special characters in title

			// tell gnuplot the plot title
			os << "title '" << m_opt.title.value() << "'";
		}

		if (m_pms)
			m_pms->print(os);

		if (m_opt.axes)
		{
			_TMP_GNUPLOTPP_SPACE(os);

			os << "axes ";
			auto v = m_opt.axes.value();
			switch (v)
			{
			case PlotAxes::x1y1:
				os << "x1y1";
				break;
			case PlotAxes::x1y2:
				os << "x1y2";
				break;
			case PlotAxes::x2y1:
				os << "x2y1";
				break;
			case PlotAxes::x2y2:
				os << "x2y2";
				break;
			default:
				break;
			}
		}
	}


	// ================================
	//          CONSTRUCTORS
	// ================================

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Gnuplotpp(bool persist, size_t buffSize)
	{
		this->addOstream(std::make_unique<_gnuplot_impl_::GnuplotPipe>(persist));
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Gnuplotpp(std::ofstream&& oFileStream)
	{
		this->addOstream(std::make_unique<std::ofstream>(std::move(oFileStream)));
	}

	////////////////////////////////////////////////////////////////
	//Gnuplotpp::Gnuplotpp(std::unique_ptr<std::ostream>&& pOstream)
	//{
	//	this->addOstream(std::move(pOstream));
	//}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Gnuplotpp(std::shared_ptr<std::ostream> ostream)
	{
		this->addOstream(ostream);
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
		// TODO move inside GnuplotPipe
		//assert(("GnuplotPipe::sendLine requires an open pipe", this->isOpen()));
		//if (!this->isOpen())
		//	throw std::runtime_error("GnuplotPipe::sendLine() requires an open pipe");

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
	void Gnuplotpp::setTerminal(Terminal term, std::optional<std::string> outFile, std::optional<Vector2i> size)
	{
		auto& gp = *this;

		gp << "unset term" << std::endl;
		gp << "unset output" << std::endl;

		switch (term)
		{
		case Terminal::None:
			break;
		case Terminal::Qt:
			gp << "set term qt";
			if (size)
				gp << " size " << size.value().x << ", " << size.value().y;
			gp << std::endl;
			break;
		case Terminal::PNG:
			gp << "set term png";
			if (size)
				gp << " size " << size.value().x << ", " << size.value().y;
			gp << std::endl;
			if (!outFile)
				throw std::runtime_error("Gnuplotpp::setTerm with term=PNG requires an output file");
			break;
		case Terminal::JPEG:
			gp << "set term jpeg";
			if (size)
				gp << " size " << size.value().x << ", " << size.value().y;
			gp << std::endl;
			if (!outFile)
				throw std::runtime_error("Gnuplotpp::setTerm with term=PNG requires an output file");
			break;
		default:
			break;
		}

		if (outFile)
			gp << "set output \"" << outFile.value() << "\"" << std::endl;

		// http://www.math.utk.edu/~vasili/refs/How-to/gnuplot.print.html
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setTitle(std::optional<std::string> title)
	{
		if (title)
			*this << "set title \"" << title.value() << "\"" << std::endl;
		else
			*this << "unset title" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::xLabel(std::optional<std::string> label)
	{
		if (label)
			*this << "set xlabel \"" << label.value() << "\"" << std::endl;
		else
			*this << "unset xlabel" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::yLabel(std::optional<std::string> label)
	{
		if (label)
			*this << "set ylabel \"" << label.value() << "\"" << std::endl;
		else
			*this << "unset ylabel" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Plot2d Gnuplotpp::plot(const std::vector<double>& data, SinglePlotOptions singlePlotOptions)
	{
		Plot2d plot;

		// buffer with 1 column
		plot.pBuffer = std::make_unique<DataBuffer>(1);
		auto& buffer = *plot.pBuffer.get();

		plot.pOptions = std::make_unique<PlotOptions>((PlotOptions)singlePlotOptions);
		auto& options = *plot.pOptions.get();

		if (singlePlotOptions.spacing)
		{
			buffer = DataBuffer(2);

			// TODO RESERVE
			size_t counter = 0;
			for (const auto& y : data)
				buffer << ((counter++) * singlePlotOptions.spacing.value()) << y << endRow;
			options.cols = { 0, 1 };
		}
		else
		{
			// TODO RESERVE
			for (const auto& y : data)
				buffer << y << endRow;
			options.cols = { 0 };
		}

		return plot;
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Plot2d Gnuplotpp::plot(const std::vector<double>& xData, const std::vector<double>& yData, SinglePlotOptions singlePlotOptions)
	{
		Plot2d plot;

		// buffer with 2 columns
		plot.pBuffer = std::make_unique<DataBuffer>(2);
		auto& buffer = *plot.pBuffer.get();

		plot.pOptions = std::make_unique<PlotOptions>((PlotOptions)singlePlotOptions);
		auto& options = *plot.pOptions.get();

		if (xData.size() != yData.size())
			throw std::runtime_error(
				(std::string)"in Gnuplotpp::plot, xData.size() must be equal to yData.size() but they were "
				+ std::to_string(xData.size()) + ", " + std::to_string(yData.size())
			);

		// TODO RESERVE
		for (size_t i = 0; i < xData.size(); i++)
			buffer << xData[i] << yData[i] << endRow;
		options.cols = { 0, 1 };

		return plot;
	}

	Gnuplotpp::Plot2d Gnuplotpp::errorbar(ErrorbarData data, SinglePlotOptions singlePlotOptions)
	{
		Plot2d plot;

		plot.pOptions = std::make_unique<PlotOptions>((PlotOptions)singlePlotOptions);
		auto& options = *plot.pOptions.get();

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
			{
				v.resize(data.y.size());
				std::fill(v.begin(), v.end(), v.front());
			}
		};
		buildErrs(data.xErr);
		buildErrs(data.yErr);

		plot.pBuffer = std::make_unique<DataBuffer>(indicesCounter);
		auto& buffer = *plot.pBuffer.get();

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

		return plot;
	}

	// TODO move
	void m_forEachPlot(std::list<Gnuplotpp::Plot2dRef> plots, std::function<void(Gnuplotpp::Plot2d&, bool first)> func)
	{
		using Plot2d = Gnuplotpp::Plot2d;

		for (auto it = plots.begin(); it != plots.end(); it++)
		{
			Plot2d* m_pPlot = nullptr;
			{
				if (auto* pRef = std::get_if<std::reference_wrapper<Plot2d>>(&*it))
					m_pPlot = &pRef->get();
				if (auto* pPtr = std::get_if<std::shared_ptr<Plot2d>>(&*it))
				{
					if (!*pPtr)
						continue;
					m_pPlot = pPtr->get();
				}
			}
			Plot2d& plot = *m_pPlot;

			func(plot, it == plots.begin());
		}
	}

	void Gnuplotpp::draw(const std::list<Plot2dRef>& plots)
	{
		// TODO chack not empty

		// alias "gnuplot"
		auto& gp = *this;

		m_forEachPlot(plots, [&](Plot2d& plot, bool first)
			{
				if (plot.pOptions)
				{
					plot.pSerializer = std::make_unique<PlotOptionsSerializer>(*plot.pOptions.get());
					plot.pSerializer->prepare(gp);
				}
			}
		);

		gp << "plot";

		m_forEachPlot(plots, [&](Plot2d& plot, bool first)
			{
				if (!first)
					gp << ",";

				_TMP_GNUPLOTPP_SPACE(gp);
				if (plot.pBuffer)
					gp << "'-'";

				if (plot.pSerializer)
				{
					_TMP_GNUPLOTPP_SPACE(gp);
					plot.pSerializer->print(gp);
				}
			}
		);

		gp << std::endl;

		// passing data to gnuplot
		// see https://stackoverflow.com/questions/3318228/how-to-plot-data-without-a-separate-file-by-specifying-all-points-inside-the-gnu
		m_forEachPlot(plots, [&](Plot2d& plot, bool first)
			{
				if (plot.pBuffer)
				{
					// data on new line
					gp << std::endl;

					// write the data
					gp << *plot.pBuffer.get();

					// tell End Of Data
					gp << Datablock_EOD << std::endl;
					//gp << Datablock_E << std::endl;// <- this would do the same
				}
			}
		);
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setTicksOptions(std::optional<TicksOptions> options)
	{
		auto& gp = *this;

		if (options)
		{
			gp << "set mxtics " << options.value().minorXdivider << std::endl;
			gp << "set mytics " << options.value().minorYdivider << std::endl;
		}
		else
		{
			gp << "unset mxtics" << std::endl;
			gp << "unset mytics" << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setGridOptions(std::optional<GridOptions> options)
	{
		auto& gp = *this;
		
		// unset the previous grid options
		gp << "unset grid" << std::endl;

		if (!options)
			return;

		// grid ticks
		//gp << "show style line" << std::endl;
		//gp << "show grid" << std::endl;
		if (options.value().major.x) gp << "set grid xtics" << std::endl;
		if (options.value().major.y) gp << "set grid ytics" << std::endl;
		if (options.value().minor.x) gp << "set grid mxtics" << std::endl;
		if (options.value().minor.y) gp << "set grid mytics" << std::endl;

		// grid level
		switch (options.value().gridLevel)
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

		LineStyleSerializer majorLineStyleSerialiser(this->createNewID(), options.value().majorLineStyle);
		LineStyleSerializer minorLineStyleSerializer(this->createNewID(), options.value().minorLineStyle);
		majorLineStyleSerialiser.prepare(*this); minorLineStyleSerializer.prepare(*this);
		majorLineStyleSerialiser.print(*this); minorLineStyleSerializer.print(*this);
		gp << "set grid ls " << majorLineStyleSerialiser.m_id->id << ", ls " << minorLineStyleSerializer.m_id->id << std::endl;

		if constexpr (0)
		{
			//gp << "set style line 81 default" << std::endl;
			//gp << "set style line 81 lt 2 lw 2 pt 3 ps 0.5" << std::endl;
			LineStyle ls;
			//ls.copyFrom = 3;
			ls.dashType = 1;
			//ls.index = 81;
			ls.lineColor = "red";
			//gp.lineStyle(ls);
			gp << "set grid ls 81" << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////
	/*void Gnuplotpp::lineStyle(LineStyle style)
	{
		auto& gp = *this;

		gp << "unset style line " << style.index << std::endl;
		gp << "set style line " << style.str() << std::endl;
	}*/

	Gnuplotpp::Vector2d Gnuplotpp::getMouseClick(void)
	{
		// TODO read https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session
		// https://devblogs.microsoft.com/commandline/windows-command-line-introducing-the-windows-pseudo-console-conpty/

		auto& gp = *this;

		Vector2d v;

		/*
		gp << "set mouse" << std::endl;
		gp << "set mouse" << std::endl;

		//gp << "pause mouse \"Mouse Click!\"" << std::endl;
		gp << "pause mouse" << std::endl;
		std::string s;

		while (1)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(0.2s);

			gp >> s;
			std::cout << "s = " << s << std::endl;
		}

		//*this << "if (exists(\"MOUSE_X\")) print MOUSE_X, MOUSE_Y, MOUSE_BUTTON; else print 0, 0, -1;" << std::endl;
		gp << "if (exists(\"MOUSE_X\")) print MOUSE_X; else print 0;" << std::endl;
		//gp >> v.x;
		gp << "if (exists(\"MOUSE_Y\")) print MOUSE_Y; else print 0;" << std::endl;
		//gp >> v.y;
		*/

		return v;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setOrigin(const Vector2d& pos)
	{
		*this << "set origin " << pos.x << ", " << pos.y << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setOrigin(void)
	{
		*this << "unset origin" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setSize(const Vector2d& pos)
	{
		*this << "set size " << pos.x << ", " << pos.y << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setSize(void)
	{
		*this << "unset size" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::beginMultiplot(size_t rows, size_t cols)
	{
		*this << "set multiplot layout " << rows << ", " << cols << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::endMultiplot(void)
	{
		*this << "unset multiplot" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::MultiplotGuard Gnuplotpp::multiplot(size_t rows, size_t cols)
	{
		this->beginMultiplot(rows, cols);
		return MultiplotGuard(this);
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::CommandLineID Gnuplotpp::createNewID(void)
	{
		auto ID = std::make_shared<CommandLineIDImpl>();

		ID->id = 50;
		while (m_idMap.find(ID->id) != m_idMap.end())
			ID->id++;

		m_idMap[ID->id] = ID;
		this->cleanIDs();

		return CommandLineID(ID);
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::cleanIDs(void)
	{
		std::erase_if(m_idMap, [](const auto& pair) -> bool { return !pair.second.lock(); });
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::addOstream(std::unique_ptr<std::ostream>&& pOstream)
	{
		// TODO checks on pOstream

		this->addRdbuf(pOstream->rdbuf());
		m_ostreams.emplace_back(std::move(pOstream));
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::addOstream(const std::shared_ptr<std::ostream>& pOstream)
	{
		// TODO checks on pOstream

		this->addRdbuf(pOstream->rdbuf());
		m_ostreams.emplace_back(std::move(pOstream));
	}

	////////////////////////////////////////////////////////////////
	/*void Gnuplotpp::addRdbuf(std::streambuf* streambuf)
	{
		//this->addOstream(std::make_unique<std::ostream>(streambuf));

		MultiStream::addRdbuf(streambuf);
	}*/

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::writeCommandsOnFile(const std::filesystem::path& path)
	{
		this->writeCommandsOnFile(std::ofstream(path));
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::writeCommandsOnFile(std::ofstream&& file)
	{
		this->addOstream(std::make_unique<std::ofstream>(std::move(file)));
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
std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::Vector2d& v)
{
	ostream << "(" << v.x << ", " << v.y << std::endl;
	return ostream;
}

#undef _TMP_GNUPLOTPP_SPACE