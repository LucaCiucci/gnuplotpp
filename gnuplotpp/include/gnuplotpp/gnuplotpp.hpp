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
#include <optional>
#include <variant>
#include <sstream>

#if defined(_WIN32) || defined (_MSVC_VER)
#include <Windows.h>
#endif

// TODO https://github.com/LucaCiucci/LC/issues/22
#ifdef _GNUPLOTPP_USE_LC_LIBRARY
#include <LC/math/geometry/Vector.hpp>
#include <LC/math/geometry/Mat.hpp>
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

	// TODO on a custom stream instead of pipe
	class Gnuplotpp : public _gnuplot_impl_::GnuplotPipe
	{
	public:

		class DataBuffer;

		// "e"
		static const char* Datablock_E;

		// "EOF"
		static const char* Datablock_EOF;

		// "EOD"
		static const char* Datablock_EOD;

		enum class ErrorBar
		{
			X,
			Y,
			XY,
		};

		struct PlotStyle
		{

		};

		struct PlotOptions
		{
			std::list<size_t> cols = { 0 };
			std::optional<std::string> title = {};
			PlotStyle style = {};
			std::optional<ErrorBar> errorBars = {};
		};

		struct Color
		{
			uint8_t r = 0;
			uint8_t g = 0;
			uint8_t b = 0;
			uint8_t transparency = 0;
		};

		struct LineStyle
		{
			size_t index = 50;
			std::optional<size_t> copyFrom = {};
			std::optional<double> lineWidth = {};
			std::optional<std::variant<std::string, Color>> lineColor = {};
			std::optional<std::variant<int, std::string>> dashType = {};
			std::optional<size_t> pointType = {};
			std::optional<double> pointSize = {};

			static std::string defaultString(void) { return "default"; };
			std::string str(void) const;
		};

		struct TicksOptions
		{
			size_t minorXdivider = 5;
			size_t minorYdivider = 5;
		};

		struct GridOptions
		{
			struct Enabled // TODO name?
			{
				bool x = {};
				bool y = {};

				Enabled(bool x, bool y) : x{ x }, y{ y } {};
				Enabled(Enabled&) = default;
				Enabled(Enabled&&) = default;
				Enabled(bool value) : x{ value }, y{ value } {};

				Enabled& operator=(const Enabled&) = default;
				Enabled& operator=(Enabled&&) = default;
				Enabled& operator=(bool value) { return *this = Enabled{ value }; };
			};

			enum class GridLevel
			{
				LayerDefault,
				Front,
				Back
			};

			Enabled major = false;
			Enabled minor = false;
			GridLevel gridLevel = GridLevel::LayerDefault;

			LineStyle majorLineStyle = { .index = 51, .lineColor = Color{ 0, 0, 0, 192 } };
			LineStyle minorLineStyle = { .index = 52, .lineColor = Color{ 0, 0, 0, 224 } };
		};

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

		void resetSession(void);

		struct SinglePlotOptions
		{
			std::optional<double> spacing = {};
			PlotOptions options = {};
		};

		// Plot a vector of double values
		// Note that options.cols is ignored
		// examples:
		//  - gnuplot.plot(your_data)
		//  - gnuplot.plot(your_data, { .spacing=0.1 })
		//  - gnuplot.plot(your_data, { .spacing=0.1, .options={ .title="Hello There!" } })
		//  - gnuplot.plot({ 42, 40, 30, 20, 10, 0 }, { .spacing=0.1, .options={ .title="Hello There!" } })
		void plot(const std::vector<double>& data, SinglePlotOptions singlePlotOptions = {});

		// Plot a vector of real values
		// Note that options.cols is ignored
		// examples:
		//  - gnuplot.plot(your_data)
		//  - gnuplot.plot(your_data, { .spacing=0.1 })
		//  - gnuplot.plot(your_data, { .spacing=0.1, .options={ .title="Hello There!" } })
		//  - gnuplot.plot({ 42, 40, 30, 20, 10, 0 }, { .spacing=0.1, .options={ .title="Hello There!" } })
		template <std::convertible_to<double> Ty>
		void plot(const std::vector<Ty>& data, SinglePlotOptions singlePlotOptions = {});

		// plot a buffer according to the options
		void plot(DataBuffer& buffer, PlotOptions options = {});

		struct ErrorbarData
		{
			const std::vector<double> y = {};
			std::vector<double> x = {};
			std::vector<double> yErr = {};
			std::vector<double> xErr = {};
		};
		void errorbar(ErrorbarData data, SinglePlotOptions singlePlotOptions = {});

		void setTicksOptions(TicksOptions options);

		void setGridOptions(GridOptions options);

#ifdef _GNUPLOTPP_USE_LC_LIBRARY

		// ...

#endif // _GNUPLOTPP_USE_LC_LIBRARY

		void lineStyle(LineStyle style);

	private:

		void m_plot(const std::vector<double>& data);

		std::ostream& getCurrentStream(void);

	private:
	};

	// ================================================================
	//                      GNUPLOT++ DATA BUFFER
	// ================================================================

	// funciton used to end a row while pushing data into a buffer value by value, for example:
	// buff << x << y << lc::endRow;
	Gnuplotpp::DataBuffer& endRow(Gnuplotpp::DataBuffer& buff);

	class Gnuplotpp::DataBuffer
	{
	public:

		// ================================
		//          CONSTRUCTORS
		// ================================

		// Construct a buffer with a specific number of columns
		// params:
		//  - cols : number of columns, default value is 1
		DataBuffer(size_t cols = 1);

		// Copy constructor
		DataBuffer(const DataBuffer&) = default;

		// Move constructor
		DataBuffer(DataBuffer&&) = default;

		DataBuffer& operator=(const DataBuffer&) = default;
		DataBuffer& operator=(DataBuffer&&) = default;

		// ================================
		//             DATA
		// ================================

		// ============ size ==============

		// Get the number of comuns
		size_t cols(void) const { return m_cols; }

		// Get the number of rows
		size_t rows(void) const { return m_data.size() / this->cols(); };

		// =========== access =============

		// Acces a specific element
		// params:
		//  - i : row index
		//  - j : column index
		double& at(size_t i, size_t j) { return m_data.at(i * this->cols() + j); };

		// Acces a specific element
		// params:
		//  - i : row index
		//  - j : column index
		const double& at(size_t i, size_t j) const { return m_data.at(i * this->cols() + j); };

		// ========= insert row ===========

		// Insert a double row in the form of a list.
		// The row size shall be equal to the column count
		// params:
		//  - [list<double>] row : the row to insert
		void push_row(std::list<double> row);

		// Insert a row in the form of a container of values convertible to double.
		// The row size shall be equal to the column count
		// params:
		//  - [Constainer of convertible to double] row : the row to insert
		template <std::ranges::range Container>
		requires (std::convertible_to<std::ranges::range_value_t<Container>, double>)
		void push_row(Container row);

		// Insert a row in the form of a container of values convertible to double.
		// The row size shall be equal to the column count
		// params:
		//  - [Constainer of convertible to double] row : the row to insert
		template <std::ranges::range Container>
		requires (std::convertible_to<std::ranges::range_value_t<Container>, double>)
		DataBuffer& operator<<(Container row);

		// Insest data gradually, value by value, for example:
		// buff << x << y << lc::endRow;
		// where std::endl is used to signal the end of row
		DataBuffer& operator<<(const double value);

		// TODO description
		DataBuffer& operator<<(std::function<DataBuffer&(DataBuffer&)> f);

	private:

		friend DataBuffer& endRow(DataBuffer&);

	private:
		size_t m_cols = 0;
		std::vector<double> m_data;
		std::vector<double> m_tmpData;
	};
}

std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::DataBuffer& buffer);
std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::Color& color);

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

	////////////////////////////////////////////////////////////////
	template <std::convertible_to<double> Ty>
	void Gnuplotpp::plot(const std::vector<Ty>& data, SinglePlotOptions singlePlotOptions)
	{
		std::vector<double> v;
		for (const auto& y : data)
			v.push_back(y);
		this->plot(v, singlePlotOptions);
	}

	// ================================================================
	//                      GNUPLOT++ DATA BUFFER
	// ================================================================

	// ================================
	//          CONSTRUCTORS
	// ================================

	// ================================
	//             DATA
	// ================================

	////////////////////////////////////////////////////////////////
	template <std::ranges::range Container>
	requires (std::convertible_to<std::ranges::range_value_t<Container>, double>)
	inline void Gnuplotpp::DataBuffer::push_row(Container row)
	{
		// convert to a double lisr
		std::list<double> v;
		for (const auto& value : row)
			v.push_back(value);

		// call the double push_row() function
		this->push_row(v);
	}

	////////////////////////////////////////////////////////////////
	template <std::ranges::range Container>
	requires (std::convertible_to<std::ranges::range_value_t<Container>, double>)
	Gnuplotpp::DataBuffer& Gnuplotpp::DataBuffer::operator<<(Container row)
	{
		this->push_row(row);
		return *this;
	}
}