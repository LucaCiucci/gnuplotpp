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

// this macro is used to put a space
#define _TMP_GNUPLOTPP_SPACE(s) s << " "

namespace lc
{
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

	namespace _gnuplot_impl_
	{

		
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
			case ErrorBarDir::X:
				_TMP_GNUPLOTPP_SPACE(os);
				os << "with ";
				os << ((m_opt.lineStyle) ? "xerrorlines" : "xerr");
				break;
			case ErrorBarDir::Y:
				_TMP_GNUPLOTPP_SPACE(os);
				os << "with ";
				os << ((m_opt.lineStyle) ? "yerrorlines" : "yerr");
				break;
			case ErrorBarDir::XY:
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

	////////////////////////////////////////////////////////////////
	Gnuplotpp::PlotOptions Gnuplotpp::Plot2d::getOptions(void) const
	{
		auto options = BASE::getOptions();
		if (this->xData.size() > 0 || this->options.spacing)
			options.cols = { 0, 1 };
		else
			options.cols = { 0 };
		return options;
	}

	// TODO private namespace and change name
	double m_ValueOrConstant(const std::vector<double>& v, size_t i)
	{
		assert(v.size() > 0);
		return (i < v.size()) ? v[i] : v.back();
	}

	// TODO private namespace and change name
	double m_ValueOrSpacing(const std::vector<double>& v, size_t i, double spacing)
	{
		return (i < v.size()) ? v[i] : (spacing * i);
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::DataBuffer Gnuplotpp::Plot2d::getData(void) const
	{
		DataBuffer buffer;

		if (this->xData.size() > 0)
			if (this->xData.size() != this->yData.size())
				throw std::runtime_error("wrong sizes");

		if (this->xData.size() > 0 || this->options.spacing)
			buffer = DataBuffer(2);

		double spacing = 0;
		if (this->options.spacing)
			spacing = this->options.spacing.value();
		// TODO reserve

		for (size_t i = 0; i < this->yData.size(); i++)
		{
			// X
			if (this->xData.size() > 0 || this->options.spacing)
				buffer << m_ValueOrSpacing(this->xData, i, spacing);

			// Y
			buffer << this->yData[i];

			// ;
			buffer << endRow;
		}

		return buffer;
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::PlotOptions Gnuplotpp::Errorbar::getOptions(void) const
	{
		auto options = BASE::getOptions();

		if (this->xErr.size() > 0)
			options.errorBars = ErrorBarDir::X, options.cols = { 0, 1, 2 };
		if (this->yErr.size() > 0)
			options.errorBars = ErrorBarDir::Y, options.cols = { 0, 1, 2 };
		if (this->xErr.size() > 0 && this->yErr.size() > 0)
			options.errorBars = ErrorBarDir::XY, options.cols = { 0, 1, 2, 3 };

		return options;
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::DataBuffer Gnuplotpp::Errorbar::getData(void) const
	{
		struct ColumnIndices
		{
			std::optional<size_t> x, y, dx, dy;
		};

		size_t indicesCounter = 0;
		ColumnIndices indices;

		indices.x = indicesCounter++;
		indices.y = indicesCounter++;

		size_t N = std::max(this->x.size(), this->y.size());

		if (this->x.size() <= 0 && this->y.size() <= 0)
			throw std::runtime_error("errorbar: no x-y data");

		if (this->x.size() != this->y.size() || ((this->x.size() == 0 || this->y.size() == 0) && !this->options.spacing))
			if (this->x.size() > 0 && this->y.size() > 0)
				throw std::runtime_error("invalid data sizes");

		double spacing = 1;
		if (this->options.spacing)
			spacing = this->options.spacing.value();

		if (this->xErr.size() <= 0 && this->yErr.size())
			throw std::runtime_error("errorbar: no x-y error data");

		if (
			this->xErr.size() != this->x.size() && this->xErr.size() != 1 &&
			this->yErr.size() != this->y.size() && this->yErr.size() != 1
			)
			throw std::runtime_error("invalid error-data sizes");

		if (this->xErr.size() > 0) indices.dx = indicesCounter++;
		if (this->yErr.size() > 0) indices.dy = indicesCounter++;

		DataBuffer buffer(indicesCounter);
		for (size_t i = 0; i < N; i++)
		{
			if (indices.x)
				buffer << m_ValueOrSpacing(this->x, i, spacing);

			if (indices.y)
				buffer << m_ValueOrSpacing(this->y, i, spacing);

			if (indices.dx)
				buffer << m_ValueOrConstant(this->xErr, i);

			if (indices.dy)
				buffer << m_ValueOrConstant(this->yErr, i);

			buffer << endRow;
		}

		return buffer;
	}

	// ================================
	//          CONSTRUCTORS
	// ================================

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Gnuplotpp(bool persist, size_t buffSize)
	{
		this->addOstream(std::make_unique<GnuplotPipe>(persist));
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
				gp << " size " << size.value().x() << ", " << size.value().y();
			gp << std::endl;
			break;
		case Terminal::PNG:
			gp << "set term png";
			if (size)
				gp << " size " << size.value().x() << ", " << size.value().y();
			gp << std::endl;
			if (!outFile)
				throw std::runtime_error("Gnuplotpp::setTerm with term=PNG requires an output file");
			break;
		case Terminal::JPEG:
			gp << "set term jpeg";
			if (size)
				gp << " size " << size.value().x() << ", " << size.value().y();
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
		return Gnuplotpp::plot({}, data, singlePlotOptions);
	}

	////////////////////////////////////////////////////////////////
	Gnuplotpp::Plot2d Gnuplotpp::plot(const std::vector<double>& xData, const std::vector<double>& yData, SinglePlotOptions singlePlotOptions)
	{
		Plot2d plot;

		plot.xData = xData;
		plot.yData = yData;
		plot.options = singlePlotOptions;

		return plot;
	}

	Gnuplotpp::Errorbar Gnuplotpp::errorbar(ErrorbarData data, SinglePlotOptions singlePlotOptions)
	{
		Errorbar plot;
		(ErrorbarData&)plot = data;
		plot.options = singlePlotOptions;
		return plot;
	}

	// TODO move
	void m_forEachPlot(std::list<Gnuplotpp::Plot2dRef> plots, std::function<void(const Gnuplotpp::Plot2dBase&, bool first)> func)
	{
		using Plot2dBase = Gnuplotpp::Plot2dBase;

		bool first = true;

		for (auto it = plots.begin(); it != plots.end(); it++)
		{
			const Plot2dBase* m_pPlot = nullptr;
			{
				if (const auto* pRef = std::get_if<std::reference_wrapper<const Plot2dBase>>(&*it))
					m_pPlot = &pRef->get();
				if (const auto* pPtr = std::get_if<std::shared_ptr<const Plot2dBase>>(&*it))
					m_pPlot = pPtr->get();
				if (!m_pPlot)
					continue;
			}
			const Plot2dBase& plot = *m_pPlot;

			func(plot, first);
			first = false;
		}
	}

	void Gnuplotpp::draw(const std::list<Plot2dRef>& plots)
	{
		// TODO chack not empty

		// alias "gnuplot"
		auto& gp = *this;

		std::list<PlotOptionsSerializer> serializers;
		m_forEachPlot(plots, [&](const Plot2dBase& plot, bool first)
			{
				serializers.emplace_back(plot.getOptions());
				serializers.back().prepare(gp);
			}
		);

		gp << "plot";

		std::list<PlotOptionsSerializer>::iterator it = serializers.begin();
		m_forEachPlot(plots, [&](const Plot2dBase& plot, bool first)
			{
				ScopeGuard i_guard([&]() { it++; });

				if (!first)
					gp << ",";

				_TMP_GNUPLOTPP_SPACE(gp);
				gp << "'-'";

				_TMP_GNUPLOTPP_SPACE(gp);
				it->print(gp);
			}
		);

		gp << std::endl;

		// passing data to gnuplot
		// see https://stackoverflow.com/questions/3318228/how-to-plot-data-without-a-separate-file-by-specifying-all-points-inside-the-gnu
		m_forEachPlot(plots, [&](const Plot2dBase& plot, bool first)
			{
				// data on new line
				gp << std::endl;

				// write the data
				auto buffer = plot.getData();
				gp << buffer;

				// tell End Of Data
				gp << Datablock_EOD << std::endl;
				//gp << Datablock_E << std::endl;// <- this would do the same
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

	Vector2d Gnuplotpp::getMouseClick(void)
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
		*this << "set origin " << pos.x() << ", " << pos.y() << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setOrigin(void)
	{
		*this << "unset origin" << std::endl;
	}

	////////////////////////////////////////////////////////////////
	void Gnuplotpp::setSize(const Vector2d& pos)
	{
		*this << "set size " << pos.x() << ", " << pos.y() << std::endl;
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
	//void Gnuplotpp::addOstream(std::unique_ptr<std::ostream>&& pOstream)
	//{
	//	// TODO checks on pOstream
	//
	//	this->addRdbuf(pOstream->rdbuf());
	//	m_ostreams.emplace_back(std::move(pOstream));
	//}

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
#if defined(_GNUPLOTPP_USE_CONCEPTS)
		buff.push_row(buff.m_tmpData);
#else
		buff.push_row(std::list<double>(buff.m_tmpData.begin(), buff.m_tmpData.end()));
#endif
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


#undef _TMP_GNUPLOTPP_SPACE