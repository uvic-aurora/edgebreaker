// Copyright (c) 2011 Michael D. Adams
// All rights reserved.

// __START_OF_LICENSE__
// 
// Copyright (c) 2015 Michael D. Adams
// All rights reserved.
// 
// This file is part of the Signal Processing Library (SPL).
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3,
// or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with this program; see the file LICENSE.  If not,
// see <http://www.gnu.org/licenses/>.
// 
// __END_OF_LICENSE__

/*!
 * @file math.hpp
 * @brief This file contains various mathematical functions/code.
 */

#ifndef SPL_math_hpp
#define SPL_math_hpp

////////////////////////////////////////////////////////////////////////////////
// Header Files.
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <cmath>
#include <cassert>
#if !defined(SPL_HAVE_STD_BESSEL)
#	if defined(SPL_HAVE_TR1_BESSEL)
#		include <tr1/cmath>
#	else
#		include <boost/tr1/cmath.hpp>
#	endif
#endif

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

/**
 * @addtogroup Math_util
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Basic Mathematical Functions.
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief The absolute value function.
 *
 * Returns:
 * The absolute value of the quantity x is returned.
 */

template <class T>
inline T absVal(T x)
{
	return (x >= 0) ? x : (-x);
}

/*!
 * @brief The signum function.
 *
 * Returns:
 * The signum of the quantity x is returned.
 */

template <class T>
inline T signum(T x)
{
	T result;
	if (x > T(0)) {
		result = T(1);
	} else if (x < T(0)) {
		result = T(-1);
	} else {
		result = T(0);
	}
	return result;
}

/*!
 * @brief The square function.
 *
 * Returns:
 * The square of the quantity x is returned.
 */

template <class T>
inline T sqr(const T& x)
{
	return x * x;
}

/*!
@brief The clip function.
*/
template <class T>
inline T clip(T x, T min, T max)
{
	assert(min <= max);
	T result = x;
	if (result < min) {
		result = min;
	}
	if (result > max) {
		result = max;
	}
	assert(result >= min && result <= max);
	return result;
}

/*!
 * @brief The cardinal sine function.
 *
 * Returns:
 * The sinc of x is returned.
 */

inline double sinc(double x)
{
	if (x) {
		return sin(x) / x;
	} else {
		return 1.0;
	}
}

/*!
 * @brief Compute a quotient with the result rounded towards zero.
 *
 * Returns:
 * The floor of x divided by y is returned.
 */
inline long roundTowardZeroDiv(long x, long y)
{
	return x / y;
}

/*!
 * @brief Compute the floor of a quotient.
 *
 * Returns:
 * The floor of x divided by y is returned.
 */

inline long floorDiv(long x, long y)
{
	assert(y);
	long result;
	if (y < 0) {
		x = -x;
		y = -y;
	}
	if (x >= 0) {
		result = x / y;
	} else {
		result = x / y + ((x % y) ? (-1) : 0);
	}
	return result;
}

/*!
 * @brief Compute the remainder after division.
 */
template <class T>
inline T mod(T x, T y)
{
	assert(y > 0);
	if (x < 0) {
		x += (((-x) / y) + 1) * y;
	}
	assert(x >= 0 && y > 0);
	return x % y;
}
#if 0
inline long mod(long x, long y)
{
	assert(y > 0);
	if (x < 0) {
		x += (((-x) / y) + 1) * y;
	}
	assert(x >= 0 && y > 0);
	return x % y;
}
#endif

/*!
 * @brief Compute the ceiling of a quotient.
 *
 * Returns:
 * The ceiling of x divided by y is returned.
 */

inline long ceilDiv(long x, long y)
{
	return floorDiv(x + y - 1, y);
}

/*!
 * @brief Convert from radians to degrees.
 *
 * Returns:
 * The quantity x converted (from radians) to degrees is returned.
 */

inline double radToDeg(double x)
{
	return x * 180.0 / M_PI;
}

/*!
 * @brief Convert from degrees to radians.
 *
 * Returns:
 * The quantity x converted (from degrees) to radians is returned.
 */

inline double degToRad(double x)
{
	return x * M_PI / 180.0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#if defined(SPL_HAVE_STD_BESSEL)
	using std::cyl_bessel_i;
#else
#	if defined(SPL_HAVE_TR1_BESSEL)
	// Use the bessel functions from TR1.
	using std::tr1::cyl_bessel_i;
#	else
	// Use the bessel functions from BOOST.
	using std::tr1::cyl_bessel_i;
#	endif
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @}
 */

}

#endif
