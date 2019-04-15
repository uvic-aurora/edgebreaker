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
 * @file cgalUtil.hpp
 * @brief This file contains various CGAL utility code.
 */

#ifndef SPL_cgalUtil_hpp
#define SPL_cgalUtil_hpp

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <SPL/math.hpp>
#include <cmath>
#include <CGAL/Cartesian.h>
#include <CGAL/Vector_3.h>
#include <CGAL/Point_3.h>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

/**
 * @addtogroup CGAL_util
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Vector utility functions
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Compute the norm of a vector.
 */

template <class T>
inline typename T::FT norm(const typename CGAL::Vector_3<T>& v)
{
	return sqrt(v * v);
}

/*!
 * @brief Compute a unit vector.
 *
 * Compute a unit vector in the direction of the given vector.
 * If the zero vector is given, the zero vector is returned.
 */

template <class T>
inline typename T::Vector_3 normalize(const typename CGAL::Vector_3<T>& v)
{
	typename T::FT norm = sqrt(v * v);
	if (norm != 0.0) {
		return v / norm;
	} else {
		return v;
	}
}

/*!
 * @brief Compute the angle between two vectors.
 */

template <class T>
inline typename T::FT angleBetweenVectors(const typename CGAL::Vector_3<T>& u,
  const CGAL::Vector_3<T>& v)
{
	return acos((u * v) / (norm(u) * norm(v)));
}

////////////////////////////////////////////////////////////////////////////////
// Rotation class.
////////////////////////////////////////////////////////////////////////////////

//! A 3-D rotation.
template <class T>
struct Rotation_3
{
	//! The field type for the CGAL kernel.
	typedef typename T::FT Real;

	//! The 3-dimensional vector type.
	typedef typename T::Vector_3 Vector_3;

	/*!
	 * @brief Create a rotation.
	 */
	Rotation_3(const Vector_3& axis_, Real angle_) : axis(axis_),
	  angle(angle_) {}

	//! The axis of rotation
	Vector_3 axis;

	//! The angle of rotation
	Real angle;
};

////////////////////////////////////////////////////////////////////////////////
// Quaternion class.
////////////////////////////////////////////////////////////////////////////////

//! A quaternion represented in terms of its scalar and vector parts.
template <class T>
struct Quaternion
{
	//! The field type for the CGAL kernel.
	typedef typename T::FT Real;

	//! The 3-dimensional vector type.
	typedef typename CGAL::Vector_3<T> Vector_3;

	/*!
	 * Create a quaternion.
	 */
	Quaternion() : scalar(0), vector(0, 0, 0) {}

	/*!
	 * Create a quaternion with the specified scalar and vector parts.
	 */
	Quaternion(Real scalar_, const Vector_3& vector_) : scalar(scalar_),
	  vector(vector_) {}

	//! The scalar part of the quaternion.
	Real scalar;

	//! The vector part of the quaternion.
	Vector_3 vector;
};

/*!
 * @brief Compute the product of two quaternions.
 */

template <class T>
Quaternion<T> operator*(const Quaternion<T>& q, const Quaternion<T>& r)
{
	return Quaternion<T>(q.scalar * r.scalar - q.vector * r.vector,
	  q.scalar * r.vector + r.scalar * q.vector +
	  CGAL::cross_product(q.vector, r.vector));
}

/*!
 * @brief  Compute the quotient of two quaternions.
 */

template <class T>
Quaternion<T> operator/(const Quaternion<T>& q, const Quaternion<T>& r)
{
	typename T::FT sqrNorm = sqr(r.scalar) + r.vector * r.vector;
	return q * Quaternion<T>(r.scalar / sqrNorm, -r.vector / sqrNorm);
}

/*!
 * @brief Convert a rotation into its corresponding quaternion.
 */

template <class T>
Quaternion<T> rotationToQuaternion(const Rotation_3<T>& rot)
{
	typename T::FT theta = 0.5 * rot.angle;
	return Quaternion<T>(cos(theta), sin(theta) * normalize(rot.axis));
}

/*!
 * @brief Convert a unit-norm quaternion into its corresponding rotation.
 */

template <class T>
Rotation_3<T> quaternionToRotation(const Quaternion<T>& q)
{
	return Rotation_3<T>(normalize(q.vector), 2.0 * acos(q.scalar));
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/**
 * @}
 */

}

#endif
