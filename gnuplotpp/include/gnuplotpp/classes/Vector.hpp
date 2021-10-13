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

#if defined(_GNUPLOTPP_USE_LC_LIBRARY)
#include <LC/math/geometry/Vector.hpp>
#endif

namespace lc
{
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
		using _gnuplot_impl_::VectorImpl::VectorImpl;
		using _gnuplot_impl_::VectorImpl::operator=;

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