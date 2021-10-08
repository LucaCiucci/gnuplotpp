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

		struct PlotStyle
		{

		};

		struct PlotOptions
		{
			std::list<size_t> cols = { 0 };
			std::optional<std::string> title = {};
			PlotStyle style = {};
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

		template <std::convertible_to<double> Ty>
		void plot(const std::vector<Ty>& data);

		void plot_tmp(const std::vector<double>& data)
		{
			m_plot(data);
		}

		void plot(DataBuffer& buffer, PlotOptions options = {});

#ifdef _GNUPLOTPP_USE_LC_LIBRARY

		// ...

#endif // _GNUPLOTPP_USE_LC_LIBRARY

	private:

		void m_plot(const std::vector<double>& data);

	private:
	};

	class Gnuplotpp::DataBuffer
	{
	public:

		DataBuffer(size_t cols = 1);
		DataBuffer(const DataBuffer&) = default;
		DataBuffer(DataBuffer&&) = default;

		size_t cols(void) const { return m_cols; }
		size_t rows(void) const { return m_data.size() / this->cols(); };

		double& at(size_t i, size_t j) { return m_data.at(i * this->cols() + j); };
		const double& at(size_t i, size_t j) const { return m_data.at(i * this->cols() + j); };

		void push_row(std::list<double> row);

		template <std::convertible_to<double> Ty>
		void push_row(std::list<Ty> row)
		{
			std::list<double> v;
			for (const auto& value : row)
				v.push_back(value);
			this->push_row(v);
		}

		DataBuffer& operator<<(std::list<double> row)
		{
			this->push_row(row);
			return *this;
		}
		template <std::convertible_to<double> Ty>
		DataBuffer& operator<<(std::list<Ty> row)
		{
			this->push_row(row);
			return *this;
		}

	private:
		size_t m_cols = 0;
		std::vector<double> m_data;
	};
}

std::ostream& operator<<(std::ostream& ostream, const lc::Gnuplotpp::DataBuffer& buffer);

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
}