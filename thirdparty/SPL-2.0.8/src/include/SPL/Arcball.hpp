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
 * @file Arcball.hpp
 * @brief This file contains the Arcball class and related code.
 */

#ifndef SPL_Arcball_hpp
#define SPL_Arcball_hpp

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <CGAL/Plane_3.h>
#include <CGAL/Ray_3.h>
#include "cgalUtil.hpp"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

/*!
 * @addtogroup CGAL_util
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Geometry utility functions
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Compute the closest point on a ray to the specified point.
 */

template <class T>
typename T::Point_3 closestPointOnRay(const typename CGAL::Point_3<T>&
  rayOrigin, const typename CGAL::Vector_3<T>& rayDir,
  const typename CGAL::Point_3<T>& point)
{
	typename T::FT t = (rayDir * (point - rayOrigin)) / (rayDir * rayDir);
	if (t < 0) {
		t = 0;
	}
	return typename T::Point_3(rayOrigin + t * rayDir);
}

/*!
 * @brief Compute the intersection of a ray and a sphere.
 *
 * Compute the intersection of a ray and a sphere.
 * The return value is a pair.
 * The first element in the pair is a boolean value.
 * This value is true if an intersection point was found, and is false
 * otherwise.
 * The second element in the pair is the intersection point closest to the
 * ray's origin.
 * If no intersection was found, the point on the sphere that is closest
 * to the ray is returned.
 */

template <class T>
std::pair<bool, typename T::Point_3> findRaySphereIntersection(
  const typename CGAL::Point_3<T>& sphereCenter, typename T::FT sphereRadius,
  const typename CGAL::Point_3<T>& rayOrigin,
  const typename CGAL::Vector_3<T>& rayDir)
{
	typename T::FT a = rayDir * rayDir;
	typename T::Vector_3 delta = rayOrigin - sphereCenter;
	typename T::FT b = 2.0 * delta * rayDir;
	typename T::FT c = delta * delta - sphereRadius * sphereRadius;
	typename T::FT discrim = b * b - 4.0 * a * c;
	if (discrim < 0.0) {
		return std::pair<bool, typename T::Point_3>(false,
		  closestPointOnRay(rayOrigin, rayDir, sphereCenter));
	}
	typename T::FT d = sqrt(discrim);
	typename T::FT t0 = (-b - d) / (2.0 * a);
	typename T::FT t1 = (-b + d) / (2.0 * a);
	typename T::FT t = t0;
	if (t1 >= 0 && t1 < t0) {
		t = t1;
	}
	if (t < 0.0) {
		return std::pair<bool, typename T::Point_3>(false,
		  closestPointOnRay(rayOrigin, rayDir, sphereCenter));
	}
	return std::pair<bool, typename T::Point_3>(true, rayOrigin + t * rayDir);
}

/*!
 * @brief Compute the intersection of a ray and a plane.
 *
 * Compute the intersection of a ray and a plane.
 * The return value is a pair.
 * The first element of the pair is a boolean.
 * This value is true if an intersection point was found, and is false
 * otherwise.
 * The second element in the pair is the intersection point closest to
 * the ray's origin.
 * If no intersection point was found, the ray's origin is returned.
 */

template <class T>
std::pair<bool, typename T::Point_3> findRayPlaneIntersection(
  const typename CGAL::Point_3<T>& planePoint,
  const typename CGAL::Vector_3<T>& planeNormal,
  const typename CGAL::Point_3<T>& rayOrigin,
  const typename CGAL::Vector_3<T>& rayDir)
{
	typename T::Point_3 intersectPoint;
	typename T::Ray_3 intersectRay;
	CGAL::Object result = CGAL::intersection(typename T::Plane_3(planePoint,
	  planeNormal), typename T::Ray_3(rayOrigin, rayDir));
	if (CGAL::assign(intersectPoint, result)) {
		return std::pair<bool, typename T::Point_3>(true, intersectPoint);
	} else if (CGAL::assign(intersectRay, result)) {
		return std::pair<bool, typename T::Point_3>(true,
		  intersectRay.source());
	} else {
		return std::pair<bool, typename T::Point_3>(false, rayOrigin);
	}
}

/*!
 * @brief Arcball.
 *
 * References:
 * K. Shoemake, Arcball Rotation Control, Graphics Gems IV, 1994, pp. 175-192.
 */

template <class T>
class Arcball
{
public:

	//! The CGAL kernel.
	typedef T Kernel;

	//! The point type.
	typedef typename Kernel::Point_3 Point;

	//! The vector type.
	typedef typename Kernel::Vector_3 Vector;

	//! The representation of a rotation.
	typedef Rotation_3<Kernel> Rotation;

	/*!
	 * @brief Create an arcball.
	 */
	Arcball();

	/*!
	 * @brief Initialize the state of an arcball.
	 */
	void initialize(double arcBallRadius, const Point& eyePos,
	  const Vector& eyeDir, const Vector& eyeUp, const Point& sceneCenter);

	/*!
	 * @brief Set the arcball rotation mode.
	 */
	void setMode(int mode);

	/*!
	 * @brief Set the starting position for arcball movement.
	 */
	void start(const Point& pos);

	/*!
	 * @brief Set the current position for arcball movement.
	 */
	void move(const Point& pos);

	/*!
	 * @brief Clear the starting and current positions for the arcball.
	 */
	void clear();

	/*!
	 * @brief Get the rotation required to turn the arcball from the
	 * starting position to the current position.
	 */
	Rotation getRotation() const;

	/*!
	 * @brief Combine two rotations.
	 */
	static Rotation combineRotations(const Rotation&, const Rotation&);

	/*!
	 * @brief For debugging...
	 */
	void setDebugLevel(int debugLevel) const;

private:

	// The arcball rotation mode.
	int mode_;

	// The arcball radius.
	double arcBallRadius_;

	// The eye position.
	Point eyePos_;

	// The eye up direction.
	Vector eyeUp_;

	// Indicates if the start position has been initialized.
	bool startPosValid_;

	// The start position for arcball movement.
	Point startPos_;

	// The current position for arcball movement.
	Point curPos_;

	// The scene center.
	Point sceneCenter_;

	// The level of debugging output.
	int debugLevel_;

};

template <class T>
Arcball<T>::Arcball() : mode_(0), arcBallRadius_(1), eyePos_(0, 0, 0),
  eyeUp_(0, 0, 0), startPosValid_(false), startPos_(0, 0, 0),
  curPos_(0, 0, 0), sceneCenter_(0, 0, 0), debugLevel_(0)
{
}

template <class T>
void Arcball<T>::initialize(double arcBallRadius,
  const typename T::Point_3& eyePos,
  const typename T::Vector_3& eyeDir,
  const typename T::Vector_3& eyeUp,
  const typename T::Point_3& sceneCenter)
{
	mode_ = 0;
	arcBallRadius_ = arcBallRadius;
	eyePos_ = eyePos;
	sceneCenter_ = sceneCenter;
	eyeUp_ = eyeUp;
	clear();
}

template <class T>
void Arcball<T>::setMode(int mode)
{
	mode_ = mode;
}

template <class T>
void Arcball<T>::start(const Arcball<T>::Point& pos)
{
	startPos_ = pos;
	curPos_ = pos;
	startPosValid_ = true;
	if (debugLevel_ > 0) {
		std::cerr
		  << "start: startPos=" << startPos_ << " curPos=" << curPos_ << "\n";
	}
}

template <class T>
void Arcball<T>::move(const Arcball<T>::Point& pos)
{
	curPos_ = pos;
	if (debugLevel_ > 0) {
		std::cerr
		  << "move: startPos=" << startPos_ << " curPos=" << curPos_ << "\n";
	}
}

template <class T>
void Arcball<T>::clear()
{
	startPosValid_ = false;
	startPos_ = typename T::Point_3(0, 0, 0);
	curPos_ = startPos_;
	if (debugLevel_ > 0) {
		std::cerr
		  << "clear: startPos=" << startPos_ << " curPos=" << curPos_ << "\n";
	}
}

template <class T>
typename Arcball<T>::Rotation Arcball<T>::getRotation() const
{
	// Ensure that the starting position has been initialized by the user.
	assert(startPosValid_);

	if (debugLevel_ > 0) {
		std::cerr << "startPos=" << startPos_ << " curPos=" << curPos_ << "\n";
	}

	// Calculate the required rotation based on the mouse movement.
	std::pair<bool, typename T::Point_3> oldResult;
	std::pair<bool, typename T::Point_3> curResult;
	if (mode_ == 0) {
		oldResult = findRaySphereIntersection<T>(sceneCenter_,
		  arcBallRadius_, eyePos_, startPos_ - eyePos_);
		curResult = findRaySphereIntersection<T>(sceneCenter_,
		  arcBallRadius_, eyePos_, curPos_ - eyePos_);
		if (!oldResult.first && !curResult.first) {
			oldResult = findRayPlaneIntersection<T>(sceneCenter_,
			  eyePos_ - sceneCenter_, eyePos_, startPos_ - eyePos_);
			curResult = findRayPlaneIntersection<T>(sceneCenter_,
			  eyePos_ - sceneCenter_, eyePos_, curPos_ - eyePos_);
			oldResult.first = true;
			curResult.first = true;
		}
	} else {
		typename T::Vector_3 planeNormal(eyePos_ - sceneCenter_);
		oldResult = findRayPlaneIntersection<T>(sceneCenter_, planeNormal,
		  eyePos_, startPos_ - eyePos_);
		curResult = findRayPlaneIntersection<T>(sceneCenter_, planeNormal,
		  eyePos_, curPos_ - eyePos_);
	}

	// If the above calculation succeeded, apply the rotation.
	Rotation result(typename T::Vector_3(0, 0, 1), 0);
	if (oldResult.first && curResult.first) {
		typename T::Vector_3 curVec = normalize(curResult.second -
		  sceneCenter_);
		typename T::Vector_3 oldVec = normalize(oldResult.second -
		  sceneCenter_);
		if (norm(curVec - oldVec) > 1e-20) {
			Quaternion<T> q = Quaternion<T>(0, curVec) / Quaternion<T>(0,
			  oldVec);
			result = quaternionToRotation(q);
		}
	}

	if (debugLevel_ > 0) {
		std::cerr << "result=" << result.axis << " " << result.angle << "\n";
	}
	return result;
}

template <class T>
typename Arcball<T>::Rotation Arcball<T>::combineRotations(
  const Arcball<T>::Rotation& r0, const Arcball<T>::Rotation& r1)
{
	return quaternionToRotation(rotationToQuaternion(r1) *
	  rotationToQuaternion(r0));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*!
 * @}
 */

}

#endif
