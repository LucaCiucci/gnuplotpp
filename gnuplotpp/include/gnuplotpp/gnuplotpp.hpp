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
#include <ostream>
#include <fstream>
#include <optional>
#include <variant>
#include <map>
#include <functional>
#include <optional>
#include <list>
#include <memory>
#include <filesystem>

#if __has_include(<concepts>)
#define _GNUPLOTPP_USE_CONCEPTS
#include <concepts>
#endif

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

		// https://github.com/LucaCiucci/LC/blob/master/include/LC/utils/types/ScopeGuard/ScopeGuard
		class ScopeGuard : NonCopyable
		{
		public:

			ScopeGuard(ScopeGuard&&) = default;
			ScopeGuard(std::function<void(void)> cleanup_function = []() {}) : m_cleanup_fcn(cleanup_function) {}
			~ScopeGuard() { if (m_cleanup_fcn) m_cleanup_fcn(); }

			void setFunction(std::function<void(void)> cleanup_function = []() {}) { m_cleanup_fcn = cleanup_function; }
			void release(void) { m_cleanup_fcn = {}; };

		private:
			std::function<void(void)> m_cleanup_fcn;
		};

		// ================================================================
		//                       PIPE Streambuf
		// ================================================================

		// https://en.cppreference.com/w/cpp/io/basic_streambuf
		// http://www.cplusplus.com/reference/ostream/ostream/
		// https://codereview.stackexchange.com/questions/185490/custom-ostream-for-a-println-like-function
		// https://siware.dev/007-cpp-custom-streambuf/
		// https://siware.dev/007-cpp-custom-streambuf/
		// https://blog.csdn.net/tangyin025/article/details/50487544
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

		// ================================================================
		//                         GNUPLOT PIPE
		// ================================================================

		// This class represents a pipe to gnuplot. It is a write only pipe
		// (ofstream). In the future it will be bidirectional.
		class GnuplotPipe : NonCopyable, public std::ostream
		{
		public:

			GnuplotPipe(bool persist);

		protected:

			// check if the pipe is open
			bool isOpen(void) const { return m_pipeStreamBuf.isOpen(); };

			// check if the pipe is open
			operator bool() const { return this->isOpen(); };

		private:
			PipeStreamBuf m_pipeStreamBuf;
		};

		
		// TODO on LC library
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

			//ostream& endl(ostream& os);
			MultiStream& operator<<(std::ostream& (*endl)(std::ostream& os))
			{
				for (auto& ostream : m_ostreams)
					ostream << endl;
				return *this;
			}

			void addRdbuf(std::streambuf* streambuf);
			
		private:
			std::list<std::ostream> m_ostreams;
		};
	}

	// ================================================================
	//                       GNUPLOT++ INTERFACE
	// ================================================================

	// This is the main Gnuplot++ class.s
	// TODO on a custom stream instead of pipe
	class Gnuplotpp : public _gnuplot_impl_::MultiStream
	{
	public:

		// ================================
		//        STATIC CONSTANTS
		// ================================

		// ====== special sequences =======

		// "e"
		static const char* Datablock_E;

		// "EOF"
		static const char* Datablock_EOF;

		// "EOD"
		static const char* Datablock_EOD;

		// ================================
		//        SUB-CLASSES/ENUMS
		// ================================
		// (see de definition of this stuff for additional info)

		// ========== geometry ============

		// If the LC library is included, Vector2d = lc::Vector2d, otherwise
		// we define a custom struct
#ifdef _GNUPLOTPP_USE_LC_LIBRARY
		using Vector2d = lc::Vector2d;
#else
		// This class represents a 2 dimensianl vector of double
		struct Vector2d
		{
			// ================================
			//           COORDINATES
			// ================================

			// TODO as access funcions
			double x = 0;
			double y = 0;
		};
#endif

#ifdef _GNUPLOTPP_USE_LC_LIBRARY
		using Vector2d = lc::Vector2i;
#else
		// This class represents a 2 dimensianl vector of int
		struct Vector2i
		{
			// ================================
			//           COORDINATES
			// ================================

			// TODO as access funcions
			int x = 0;
			int y = 0;
		};
#endif

		// ============== ID ==============

		// TODO lc::PrimiteveWrapper https://stackoverflow.com/questions/17793298/c-class-wrapper-around-fundamental-types

		struct CommandLineIDImpl final
		{
			size_t id = 0;

			std::function<void(void)> onExit = {};

			CommandLineIDImpl() = default;
			CommandLineIDImpl(const CommandLineIDImpl&) = default;
			CommandLineIDImpl(CommandLineIDImpl&&) = default;

			CommandLineIDImpl& operator=(const CommandLineIDImpl&) = default;
			CommandLineIDImpl& operator=(CommandLineIDImpl&&) = default;
		};

		class CommandLineID final : public std::shared_ptr<CommandLineIDImpl>
		{
		public:
			using shared_ptr::shared_ptr;

			CommandLineID(const CommandLineID&) = default;
			CommandLineID(CommandLineID&&) = default;

			CommandLineID& operator=(const CommandLineID&) = default;
			CommandLineID& operator=(CommandLineID&&) = default;

		private:
			CommandLineID() = delete;
			CommandLineID(std::shared_ptr<CommandLineIDImpl> id) : shared_ptr(id) {};

			friend class Gnuplotpp;
		};

		// =========== buffers ============

		class DataBuffer;

		// ============ ... ===============

		enum class ErrorBar
		{
			X,
			Y,
			XY,
		};

		struct PlotStyle
		{

		};

		struct Color
		{
			uint8_t r = 0;
			uint8_t g = 0;
			uint8_t b = 0;
			uint8_t transparency = 0;
		};

		enum class PointType : int
		{
			// gnuplot << "test"

			Empty = -1,
			Dot = 0,
			Cross = 1,
			X = 2,
			Star = 3,
			SquareDot = 4,
			SolidSquare = 5,
			Circle = 6,
			SolidCircle = 7,
			TriangleDot = 8,
			SolidDot = 9,
			InvTriangleDot = 10,
			InvSolidDot = 11,
			RhombusDot = 12,
			SolidRhombus = 13,
			HexagonDot = 14,
			SolidHexagon = 15,
		};

		struct GnuplotSerializable
		{
			virtual void prepare(Gnuplotpp& os) const = 0;
			virtual void print(Gnuplotpp& os) const = 0;
		};

		struct GnuplotSerializer : public GnuplotSerializable, _gnuplot_impl_::NonCopyable
		{
			// empty
		};

		struct Marker
		{
			std::optional<std::variant<int, PointType>> pointType = PointType::Cross;
			std::optional<double> pointSize = {};
		};

		struct MarkerSerializer : public GnuplotSerializer
		{
			const Marker& m_m;

			MarkerSerializer(const Marker& m) : m_m(m) {};

			void prepare(Gnuplotpp& os) const override;
			void print(Gnuplotpp& os) const override;
		};

		struct LineStyle
		{
			std::optional<int> copyFrom = {};
			std::optional<double> lineWidth = {};
			std::optional<std::variant<std::string, Color>> lineColor = {};
			std::optional<std::variant<int, std::string>> dashType = {};
			std::optional<Marker> marker = {};
		};

		struct LineStyleSerializer : public GnuplotSerializer
		{
			const LineStyle& m_ls;
			CommandLineID m_id;

			mutable std::unique_ptr<MarkerSerializer> m_pms;

			LineStyleSerializer(CommandLineID id, const LineStyle& ls) : m_id(id), m_ls(ls) {};

			void prepare(Gnuplotpp& os) const override;
			void print(Gnuplotpp& os) const override;
		};

		enum class PlotAxes
		{
			x1y1,
			x1y2,
			x2y1,
			x2y2,
		};

		struct PlotOptions
		{
			std::list<size_t> cols = { 0 };
			std::optional<std::string> title = {};
			PlotStyle style = {};
			std::optional<ErrorBar> errorBars = {};

			std::optional<LineStyle> lineStyle;
			std::optional<Marker> marker = Marker{};

			std::optional<PlotAxes> axes = {};
		};

		struct PlotOptionsSerializer : public GnuplotSerializer
		{
			// COPY
			mutable PlotOptions m_opt;

			mutable std::unique_ptr<MarkerSerializer> m_pms;

			mutable std::unique_ptr<LineStyleSerializer> m_lsSserializer;

			PlotOptionsSerializer(const PlotOptions& opt) : m_opt(opt) {};

			void prepare(Gnuplotpp& os) const override;
			void print(Gnuplotpp& os) const override;
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

			LineStyle majorLineStyle = { .lineColor = Color{ 0, 0, 0, 192 } };
			LineStyle minorLineStyle = { .lineColor = Color{ 0, 0, 0, 224 } };
		};

		struct Plot2d
		{
			std::unique_ptr<DataBuffer> pBuffer;
			std::unique_ptr<PlotOptions> pOptions;

			void render(Gnuplotpp& gp) const;

		private:
			std::unique_ptr<PlotOptionsSerializer> pSerializer;
			friend class Gnuplotpp;
		};

		enum class Terminal
		{
			None,
			Qt,
			PNG,
			JPEG
		};

		struct MultiplotGuard final : private _gnuplot_impl_::ScopeGuard
		{
			MultiplotGuard(Gnuplotpp* pGp) : ScopeGuard([pGp]() { pGp->endMultiplot(); }) {};
			operator bool() const { return true; };
			friend class Gnuplotpp;
		};

		// ================================
		//          CONSTRUCTORS
		// ================================

		// Default constructor
		Gnuplotpp(bool persist = true, size_t buffSize = 1 << 10);

		// use a file
		Gnuplotpp(std::ofstream&& oFileStream);

		// use a different ostream
		// not necessary, unique is convertible to shared...
		// TODO modify to avoid ambiguity
		//Gnuplotpp(std::unique_ptr<std::ostream>&& ostream);

		// use a different ostream
		Gnuplotpp(std::shared_ptr<std::ostream> ostream);

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

		void setTerminal(Terminal term = Terminal::None, std::optional<std::string> outFile = {}, std::optional<Vector2i> size = {});

		void setTitle(std::optional<std::string> title = {});

		void xLabel(std::optional<std::string> label = {});
		void yLabel(std::optional<std::string> label = {});

		struct SinglePlotOptions
		{
			// title of the plot
			std::optional<std::string> title = {};

			// Spacing between points, if not set it will be automatically
			// choosen by gnuplot (it should be 1)
			std::optional<double> spacing = {};

			// Style of the line connecting the points, if not set
			// no line will be drawn
			std::optional<LineStyle> lineStyle = {};

			// The marker type and size
			std::optional<Marker> marker = Marker{};

			std::optional<PlotAxes> axes = {};

			explicit operator PlotOptions() const
			{
				return {
					.title      = title,
					.lineStyle  = lineStyle,
					.marker     = marker,
					.axes       = axes
				};
			};
		};

		// Plot a vector of double values
		// Note that options.cols is ignored
		// examples:
		//  - gnuplot.plot(your_data)
		//  - gnuplot.plot(your_data, { .spacing=0.1 })
		//  - gnuplot.plot(your_data, { .spacing=0.1, .options={ .title="Hello There!" } })
		//  - gnuplot.plot({ 42, 40, 30, 20, 10, 0 }, { .spacing=0.1, .options={ .title="Hello There!" } })
		// TODO update examples
		// TODO STATIC
		Plot2d plot(const std::vector<double>& data, SinglePlotOptions singlePlotOptions = SinglePlotOptions{});

#if defined(_GNUPLOTPP_USE_CONCEPTS)
		// Plot a vector of real values
		// Note that options.cols is ignored
		// examples:
		//  - gnuplot.plot(your_data)
		//  - gnuplot.plot(your_data, { .spacing=0.1 })
		//  - gnuplot.plot(your_data, { .spacing=0.1, .options={ .title="Hello There!" } })
		//  - gnuplot.plot({ 42, 40, 30, 20, 10, 0 }, { .spacing=0.1, .options={ .title="Hello There!" } })
		// TODO update examples
		template <std::convertible_to<double> Ty>
		Plot2d plot(const std::vector<Ty>& data, SinglePlotOptions singlePlotOptions = {});
#endif

		Plot2d plot(const std::vector<double>& xData, const std::vector<double>& yData, SinglePlotOptions singlePlotOptions = SinglePlotOptions{});

#if defined(_GNUPLOTPP_USE_CONCEPTS)
		template <std::convertible_to<double> Ty>
		Plot2d plot(const std::vector<Ty>& xData, const std::vector<Ty>& yData, SinglePlotOptions singlePlotOptions = SinglePlotOptions{});
#endif

		struct ErrorbarData
		{
			// Y data of the errorbar plot
			// (required)
			const std::vector<double> y = {};

			// X data of the orrorbay plot
			std::vector<double> x = {};

			// Y errors, if a single value is present, errors will be considered homogeneus
			std::vector<double> yErr = {};

			// X errors, if a single value is present, errors will be considered homogeneus
			std::vector<double> xErr = {};
		};

		// Creates an errorbar plot.
		// Data can be passes through an ErrorbarData struct, at leas one of X or Y errrors
		// must be present
		Plot2d errorbar(ErrorbarData data, SinglePlotOptions singlePlotOptions = SinglePlotOptions{});

		using Plot2dRef = std::variant<std::reference_wrapper<Plot2d>, std::shared_ptr<Plot2d>>;

		// TOOD description ...
		void draw(const std::list<Plot2dRef>& plots);

		void setTicksOptions(std::optional<TicksOptions> options = {});

		void setGridOptions(std::optional<GridOptions> options = {});

		Vector2d getMouseClick(void);

		void setOrigin(const Vector2d& pos);
		void setOrigin(void);

		void setSize(const Vector2d& pos);
		void setSize(void);

		void beginMultiplot(size_t rows, size_t cols);
		void endMultiplot(void);

		MultiplotGuard multiplot(size_t rows, size_t cols);

#ifdef _GNUPLOTPP_USE_LC_LIBRARY

		// ...

#endif // _GNUPLOTPP_USE_LC_LIBRARY

		//void lineStyle(LineStyle style);

		CommandLineID createNewID(void);

		void cleanIDs(void);

		// impossible, see https://stackoverflow.com/questions/20774587/why-cant-stdostream-be-moved
		//void addOstream(std::ostream&& ostream);

		void addOstream(std::unique_ptr<std::ostream>&& pOstream);

		void addOstream(const std::shared_ptr<std::ostream>& pOstream);

		// already in MultiStream
		//void addRdbuf(std::streambuf* streambuf);

		void writeCommandsOnFile(const std::filesystem::path& path);

		void writeCommandsOnFile(std::ofstream&& file);

	private:

		// TODO implement and use
		std::shared_ptr<CommandLineIDImpl> createNewIDImpl(void);

	private:
		std::list<std::variant<
			std::unique_ptr<std::ostream>,
			std::shared_ptr<std::ostream>>
			> m_ostreams;
		std::map<size_t, std::weak_ptr<CommandLineIDImpl>> m_idMap;
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

#if defined(_GNUPLOTPP_USE_CONCEPTS)
		// Insert a row in the form of a container of values convertible to double.
		// The row size shall be equal to the column count
		// params:
		//  - [Constainer of convertible to double] row : the row to insert
		template <std::ranges::range Container>
		requires (std::convertible_to<std::ranges::range_value_t<Container>, double>)
		void push_row(Container row);
#endif

#if defined(_GNUPLOTPP_USE_CONCEPTS)
		// Insert a row in the form of a container of values convertible to double.
		// The row size shall be equal to the column count
		// params:
		//  - [Constainer of convertible to double] row : the row to insert
		template <std::ranges::range Container>
		requires (std::convertible_to<std::ranges::range_value_t<Container>, double>)
		DataBuffer& operator<<(Container row);
#endif

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
std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::Vector2d& v);

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

#if defined(_GNUPLOTPP_USE_CONCEPTS)
	////////////////////////////////////////////////////////////////
	template <std::convertible_to<double> Ty>
	Gnuplotpp::Plot2d Gnuplotpp::plot(const std::vector<Ty>& data, SinglePlotOptions singlePlotOptions)
	{
		// TODO reserve
		std::vector<double> v;
		for (const auto& y : data)
			v.push_back(y);
		return this->plot(v, singlePlotOptions);
	}
#endif

#if defined(_GNUPLOTPP_USE_CONCEPTS)
	////////////////////////////////////////////////////////////////
	template <std::convertible_to<double> Ty>
	Gnuplotpp::Plot2d Gnuplotpp::plot(const std::vector<Ty>& xData, const std::vector<Ty>& yData, SinglePlotOptions singlePlotOptions)
	{
		// TODO reserve
		std::vector<double> vx, vy;
		for (const auto& x : xData)
			vx.push_back(x);
		for (const auto& y : yData)
			vy.push_back(y);
		return this->plot(vx, vy, singlePlotOptions);
	}
#endif

	// ================================================================
	//                      GNUPLOT++ DATA BUFFER
	// ================================================================

	// ================================
	//          CONSTRUCTORS
	// ================================

	// ================================
	//             DATA
	// ================================

#if defined(_GNUPLOTPP_USE_CONCEPTS)
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
#endif

#if defined(_GNUPLOTPP_USE_CONCEPTS)
	////////////////////////////////////////////////////////////////
	template <std::ranges::range Container>
	requires (std::convertible_to<std::ranges::range_value_t<Container>, double>)
	Gnuplotpp::DataBuffer& Gnuplotpp::DataBuffer::operator<<(Container row)
	{
		this->push_row(row);
		return *this;
	}
#endif
}