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

#include <streambuf>

// TODO https://github.com/LucaCiucci/LC/issues/22
#if defined(_GNUPLOTPP_USE_LC_LIBRARY)
#include <LC/utils/types/NonCopyable.hpp>
#include <LC/utils/types/ScopeGuard.hpp>
#include <LC/math/geometry/Vector.hpp>
#endif

namespace lc
{
	// ================================================================
	//                       NON - COPYABLE
	// ================================================================
#ifndef _GNUPLOTPP_USE_LC_LIBRARY

	// Base class for non copyable objects. Intuitively for a non-copyable class id does
	// not make sense to be copyied or it could generare erroneus or undefined behaviour.
	// Copyied from the LC library
	// https://github.com/LucaCiucci/LC/blob/d709ae3e40c469be148c4d71c8b77eb8993151dc/include/LC/utils/types/NonCopyable/NonCopyable#L7
	class NonCopyable
	{
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator=(const NonCopyable&) = delete;
	public:
		NonCopyable() = default;
		NonCopyable(NonCopyable&&) = default;
	};
#else // !_GNUPLOTPP_USE_LC_LIBRARY

	// Using LC's definition for NonCopyable
	using ::lc::NonCopyable;

#endif // !_GNUPLOTPP_USE_LC_LIBRARY

	// ================================================================
	//                          SCOPE GUARD
	// ================================================================
#ifndef _GNUPLOTPP_USE_LC_LIBRARY

	// This is a class That executes some it is destroyed (for example if it goes out of scope)
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
#else // !_GNUPLOTPP_USE_LC_LIBRARY

	// Using LC's definition for ScopeGuard
	using ::lc::ScopeGuard;

#endif // !_GNUPLOTPP_USE_LC_LIBRARY

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

	// ================================================================
	//                         MULTI-STREAM
	// ================================================================

	// This class allows us to use the << and >> operators on multiple streambuffers
	// TODO add to LC library
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

		MultiStream& operator<<(std::ostream& (*endl)(std::ostream& os))
		{
			for (auto& ostream : m_ostreams)
				ostream << endl;
			return *this;
		}

		// Add a streambuffer to the multistream
		void addRdbuf(std::streambuf* streambuf);

	private:
		std::list<std::ostream> m_ostreams;
	};

	// ================================================================
	//                           VECTORS
	// ================================================================
#ifndef _GNUPLOTPP_USE_LC_LIBRARY

	namespace _gnuplot_impl_
	{
		template <size_t N, class T>
		class VectorImpl
		{
		public:
			VectorImpl() : X{} {};
			VectorImpl(const T& x, const T& y) : X{ x, y } {};
			VectorImpl(const VectorImpl&) = default;
			VectorImpl(VectorImpl&&) = default;

			VectorImpl& operator=(const VectorImpl&) = default;
			VectorImpl& operator=(VectorImpl&&) = default;

		public:
			T X[N];
		};
	}

	template <size_t N, class T>
	class Vector : _gnuplot_impl_::VectorImpl<N, T>
	{
	public:
		using VectorImpl::VectorImpl;
		using VectorImpl::operator=;

		T& x(void) { return this->X[0]; }
		const T& x(void) const { return this->X[0]; }

		T& y(void) { return this->X[1]; }
		const T& y(void) const { return this->X[1]; }

		T& z(void) { return this->X[2]; }
		const T& z(void) const { return this->X[2]; }
	};

	template <class T>
	class Vector<1, T> : _gnuplot_impl_::VectorImpl<1, T>
	{
	public:
		using VectorImpl::VectorImpl;
		using VectorImpl::operator=;

		T& x(void) { return this->X[0]; }
		const T& x(void) const { return this->X[0]; }
	};

	template <class T>
	class Vector<2, T> : _gnuplot_impl_::VectorImpl<1, T>
	{
	public:
		using VectorImpl::VectorImpl;
		using VectorImpl::operator=;

		T& x(void) { return this->X[0]; }
		const T& x(void) const { return this->X[0]; }

		T& y(void) { return this->X[1]; }
		const T& y(void) const { return this->X[1]; }
	};

	template <class T>
	class Vector<3, T> : _gnuplot_impl_::VectorImpl<1, T>
	{
	public:
		using VectorImpl::VectorImpl;
		using VectorImpl::operator=;

		T& x(void) { return this->X[0]; }
		const T& x(void) const { return this->X[0]; }

		T& y(void) { return this->X[1]; }
		const T& y(void) const { return this->X[1]; }

		T& z(void) { return this->X[2]; }
		const T& z(void) const { return this->X[2]; }
	};

	template <class T> using Vector1 = Vector<1, T>;
	template <class T> using Vector2 = Vector<2, T>;
	template <class T> using Vector3 = Vector<3, T>;

	using Vector1d = Vector1<double>;
	using Vector2d = Vector2<double>;
	using Vector3d = Vector3<double>;

	using Vector1f = Vector1<float>;
	using Vector2f = Vector2<float>;
	using Vector3f = Vector3<float>;

	using Vector1i = Vector1<int>;
	using Vector2i = Vector2<int>;
	using Vector3i = Vector3<int>;

	using Vector1u = Vector1<unsigned>;
	using Vector2u = Vector2<unsigned>;
	using Vector3u = Vector3<unsigned>;
#else // !_GNUPLOTPP_USE_LC_LIBRARY

	// Using LC's definition for Vector

#endif // !_GNUPLOTPP_USE_LC_LIBRARY


}

#ifndef _GNUPLOTPP_USE_LC_LIBRARY
std::ostream& operator<<(std::ostream& ostream, const lc::Vector2d& v);
#endif // !_GNUPLOTPP_USE_LC_LIBRARY