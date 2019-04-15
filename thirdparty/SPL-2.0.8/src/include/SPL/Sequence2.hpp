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
 * @file Sequence2.hpp
 * @brief This file contains code for the Sequence2 template class.
 */

#ifndef SPL_Sequence2_hpp
#define SPL_Sequence2_hpp

////////////////////////////////////////////////////////////////////////////////
// Configuration
////////////////////////////////////////////////////////////////////////////////

//! Defining this symbol will enable extra code for debugging.
//#define SPL_SEQUENCE2_DEBUG

//! Defining this symbol will enable some new code for convolution.
#define SPL_SEQUENCE2_USE_NEW_CONV

#if defined(SPL_SEQUENCE2_DEBUG)
//! Prevent the inlining of functions.
#define SPL_SEQUENCE2_INLINE
#else
//! Allow the inlining of functions.
#define SPL_SEQUENCE2_INLINE inline
#endif

////////////////////////////////////////////////////////////////////////////////
// Include files.
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <vector>
#include <SPL/Array2.hpp>
#include <SPL/Sequence.hpp>
#include <SPL/Sequence1.hpp>
#include <SPL/Sequence.hpp>
#include <SPL/math.hpp>

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// Types.
////////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup Sequence2
 * @{
 */

/*!
 * @brief A two-dimensional sequence class with lazy copying and reference
 * counting.
 */

template <class T>
class Sequence2
{
public:

	////////////////////////////////////////////////////////////
	// Types
	////////////////////////////////////////////////////////////

	//! The type of the element in the sequence.
	typedef T ElemType;

	//! The const iterator for all elements in the sequence.
	typedef typename Array2<T>::ConstIterator ConstIterator;

	//! The mutable iterator for all elements in the sequence.
	typedef typename Array2<T>::Iterator Iterator;

	//! The const iterator for the elements in a row of the sequence.
	typedef typename Array2<T>::ConstXIterator ConstXIterator;

	//! The mutable iterator for the elements in a row of the sequence.
	typedef typename Array2<T>::XIterator XIterator;

	//! The const iterator for the elements in a column of the sequence.
	typedef typename Array2<T>::ConstYIterator ConstYIterator;

	//! The mutable iterator for the elements in a column of the sequence.
	typedef typename Array2<T>::YIterator YIterator;

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief The default constructor.
	 */
	Sequence2();

	/*!
	 * @brief Construct a sequence with the specified start index and size.
	 */
	Sequence2(int startX, int startY, int width, int height);

	/*!
	 * @brief Construct a sequence with the specified start index and size,
	 * with all elements set to the given value.
	 */
	Sequence2(int startX, int startY, int width, int height, const T& data);

	/*!
	 * @brief Construct a sequence with the specified start index and size,
	 * with the elements initialized to the data read from the given iterator.
	 */
	template <class InputIterator>
	Sequence2(int startX, int startY, int width, int height,
	  InputIterator data);

	/*!
	 * @brief The copy constructor.
	 */
	Sequence2(const Sequence2& f);

	/*!
	 * @brief Create a sequence from another sequence having elements of
	 * a different type.
	 */
	template <class OtherT>
	Sequence2(const Sequence2<OtherT>& f);

	/*!
	 * @brief Create a sequence from an array.
	 */
	Sequence2(const Array2<T>& data);

	/*!
	 * @brief Create a sequence from an array using the given starting index.
	 */
	Sequence2(int startX, int startY, const Array2<T>& data);

	/*!
	 * @brief The destructor.
	 */
	~Sequence2();

	////////////////////////////////////////////////////////////
	// Assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief The assignment operator.
	 */
	Sequence2& operator=(const Sequence2& f);

	/*!
	 * @brief Assign another sequence with elements of a different type
	 * to this sequence.
	 *
	 * The type OtherT must be assignable to the type T.
	 */
	template <class OtherT>
	Sequence2& operator=(const Sequence2<OtherT>& f);

	////////////////////////////////////////////////////////////
	// Compound assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Add another sequence to this one.
	 */
	Sequence2& operator+=(const Sequence2& f);

	/*!
	 * @brief Subtract another sequence from this one.
	 */
	Sequence2& operator-=(const Sequence2& f);

	/*!
	 * @brief Multiply elementwise this sequence by another one.
	 */
	Sequence2& operator*=(const Sequence2& f);

	/*!
	 * @brief Divide elementwise this sequence by another one.
	 */
	Sequence2& operator/=(const Sequence2& f);

	/*!
	 * @brief Add a value to each element of this sequence.
	 */
	Sequence2& operator+=(const T& value);

	/*!
	 * @brief Subtract a value from each element of this sequence.
	 */
	Sequence2& operator-=(const T& value);

	/*!
	 * @brief Multiply each element of this sequence by the specified value.
	 */
	Sequence2& operator*=(const T& value);

	/*!
	 * @brief Divide each element of the sequence by the given value.
	 */
	Sequence2& operator/=(const T& value);

	////////////////////////////////////////////////////////////
	// Basic queries
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get the x-coordinate of the start index for the sequence.
	 */
	int getStartX() const;

	/*!
	 * @brief Get the y-coordinate of the start index for the sequence.
	 */
	int getStartY() const;

	/*!
	 * @brief Get the x-coordinate of the end index for the sequence.
	 */
	int getEndX() const;

	/*!
	 * @brief Get the y-coordinate of the end index for the sequence.
	 */
	int getEndY() const;

	/*!
	 * @brief Get the width of the sequence.
	 */
	int getWidth() const;

	/*!
	 * @brief Get the height of the sequence.
	 */
	int getHeight() const;

	/*!
	 * @brief Get the number of elements in the sequence.
	 */
	int getSize() const;

	/*!
	 * @brief Is the array for this sequence shared with another array?
	 */
	bool isShared() const;

	////////////////////////////////////////////////////////////
	// Accessors
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a mutable reference to the specified element in the sequence.
	 */
	T& operator()(int x, int y);

	/*!
	 * @brief Get a const reference to the specified element in the sequence.
	 */
	const T& operator()(int x, int y) const;

	////////////////////////////////////////////////////////////
	// Iterators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a const iterator for the first element in the sequence.
	 */
	ConstIterator begin() const;

	/*!
	 * @brief Get a mutable iterator for the first element in the sequence.
	 */
	Iterator begin();

	/*!
	 * @brief Get a const iterator for one past the last element in the
	 * sequence.
	 */
	ConstIterator end() const;

	/*!
	 * @brief Get a mutable iterator for one past the last element in the
	 * sequence.
	 */
	Iterator end();

	/*!
	 * @brief Get a const iterator for the first element in the specified
	 * row of the sequence.
	 */
	ConstXIterator rowBegin(int y) const;

	/*!
	 * @brief Get a mutable iterator for the first element in the specified
	 * row of the sequence.
	 */
	XIterator rowBegin(int y);

	/*!
	 * @brief Get a const iterator for one past the end in the specified row
	 * of the sequence.
	 */
	ConstXIterator rowEnd(int y) const;

	/*!
	 * @brief Get a mutable iterator for one past the end in the specified
	 * row of the sequence.
	 */
	XIterator rowEnd(int y);

	/*!
	 * @brief Get a const iterator for the first element in the specified
	 * column of the sequence.
	 */
	ConstYIterator colBegin(int x) const;

	/*!
	 * @brief Get a mutable iterator for the first element in the specified
	 * column of the sequence.
	 */
	YIterator colBegin(int x);

	/*!
	 * @brief Get a const iterator for one past the end in the specified
	 * column of the sequence.
	 */
	ConstYIterator colEnd(int x) const;

	/*!
	 * @brief Get a mutable iterator for one past the end in the specified
	 * column of the sequence.
	 */
	YIterator colEnd(int x);

	////////////////////////////////////////////////////////////
	// Resizing
	////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////
	// Get various statistics
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get the minimum element in the sequence.
	 *
	 * The sequence must contain at least one element.
	 */
	T min() const;

	/*!
	 * @brief Get the maximum element in the sequence.
	 *
	 * The sequence must contain at least one element.
	 */
	T max() const;

	/*!
	 * @brief Get the sum of the elements in the sequence.
	 */
	T sum() const;

	////////////////////////////////////////////////////////////
	// Input/ouput
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Output a sequence to the specified stream using the
	 * given field width for each sequence element.
	 */
	std::ostream& output(std::ostream& out, int fieldWidth) const;

	////////////////////////////////////////////////////////////
	// Miscellaneous
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a copy of the underlying array.
	 */
	Array2<T> getArray() const;

	/*!
	 * @brief Swap the data for the underlying array and the specified array.
	 */
	void swapArray(Array2<T>& data);

	/*!
	 * @brief Get a copy of the underlying array.
	 */
	void fill(const T& value);

	/*!
	 * @brief Translate (i.e., shift) a sequence by the specified displacement.
	 */
	Sequence2& translate(int x, int y);

private:

	template <class> friend class Sequence2;

	template <class U>
	friend std::ostream& operator<<(std::ostream& out, const Sequence2<U>&);
	template <class U>
	friend Sequence2<U> translate(const Sequence2<U>&, int x, int y);

	//! The type of the underlying array.
	typedef Array2<T> Array;

	//! The x-coordinate of the start index of the sequence.
	int startX_;

	//! The y-coordinate of the start index of the sequence.
	int startY_;

	//! The sequence element data.
	Array data_;

};

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/// @cond

/*!
 * @brief Find the bounding box of the union of the domains of two
 * sequences.
 *
 * DO NOT CALL THIS FUNCTION DIRECTLY!
 */
void combineDomains(int firstStartX, int firstStartY, int firstEndX,
  int firstEndY, int secondStartX, int secondStartY, int secondEndX,
  int secondEndY, int& startX, int& startY, int& endX, int& endY);

/// @endcond

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2() : startX_(0), startY_(0),
  data_()
{
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2(int startX, int startY,
  int width, int height) : startX_(startX), startY_(startY),
  data_(width, height)
{
	assert(width >= 0 && height >= 0);
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2(int startX, int startY,
  int width, int height, const T& value) : startX_(startX), startY_(startY),
  data_(width, height, value)
{
	assert(width >= 0 && height >= 0);
}

template <class T>
template <class InputIterator>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2(int startX, int startY,
  int width, int height, InputIterator data) : startX_(startX),
  startY_(startY), data_(width, height, data)
{
	assert(width >= 0 && height >= 0);
	assert(getWidth() == width && getHeight() == height);
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2(const Sequence2<T>& f) :
  startX_(f.startX_), startY_(f.startY_), data_(f.data_)
{
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2(const Array2<T>& data) :
  startX_(0), startY_(0), data_(data)
{
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2(int startX, int startY,
  const Array2<T>& data) :
  startX_(startX), startY_(startY), data_(data)
{
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>::~Sequence2()
{
}

template <class T>
template <class OtherT>
SPL_SEQUENCE2_INLINE Sequence2<T>::Sequence2(const Sequence2<OtherT>& f) :
  startX_(f.startX_), startY_(f.startY_), data_(f.data_)
{
}

////////////////////////////////////////////////////////////////////////////////
// Assignment operators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator=(
  const Sequence2<T>& f)
{
	startX_ = f.startX_;
	startY_ = f.startY_;
	data_ = f.data_;
	return *this;
}

template <class T>
template <class OtherT>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator=(
  const Sequence2<OtherT>& f)
{
	startX_ = f.startX_;
	startY_ = f.startY_;
	data_ = f.data_;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Compound assignment operators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator+=(
  const Sequence2<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((!getSize() && !f.getSize()) ||
	(getStartX() == f.getStartX() && getEndX() == f.getEndX() &&
	getStartY() == f.getStartY() && getEndY() == f.getEndY()));

	data_ += f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator-=(
  const Sequence2<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((!getSize() && !f.getSize()) ||
	(getStartX() == f.getStartX() && getEndX() == f.getEndX() &&
	getStartY() == f.getStartY() && getEndY() == f.getEndY()));

	data_ -= f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator*=(
  const Sequence2<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((!getSize() && !f.getSize()) ||
	(getStartX() == f.getStartX() && getEndX() == f.getEndX() &&
	getStartY() == f.getStartY() && getEndY() == f.getEndY()));

	data_ *= f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator/=(
  const Sequence2<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((!getSize() && !f.getSize()) ||
	(getStartX() == f.getStartX() && getEndX() == f.getEndX() &&
	getStartY() == f.getStartY() && getEndY() == f.getEndY()));

	data_ /= f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator+=(
  const T& value)
{
	data_ += value;
	return *this;
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator-=(
  const T& value)
{
	data_ -= value;
	return *this;
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator*=(
  const T& value)
{
	data_ *= value;
	return *this;
}

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::operator/=(
  const T& value)
{
	data_ /= value;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Basic queries
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE2_INLINE int Sequence2<T>::getStartX() const
{
	return startX_;
}

template <class T>
SPL_SEQUENCE2_INLINE int Sequence2<T>::getStartY() const
{
	return startY_;
}

template <class T>
SPL_SEQUENCE2_INLINE int Sequence2<T>::getEndX() const
{
	return startX_ + getWidth();
}

template <class T>
SPL_SEQUENCE2_INLINE int Sequence2<T>::getEndY() const
{
	return startY_ + getHeight();
}

template <class T>
SPL_SEQUENCE2_INLINE int Sequence2<T>::getWidth() const
{
	return data_.getWidth();
}

template <class T>
SPL_SEQUENCE2_INLINE int Sequence2<T>::getHeight() const
{
	return data_.getHeight();
}

template <class T>
SPL_SEQUENCE2_INLINE int Sequence2<T>::getSize() const
{
	return data_.getWidth() * data_.getHeight();
}

template <class T>
SPL_SEQUENCE2_INLINE bool Sequence2<T>::isShared() const
{
	return data_.isShared();
}

////////////////////////////////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE2_INLINE const T& Sequence2<T>::operator()(int x, int y) const
{
    assert(x >= startX_ && x < getEndX() && y >= startY_ && y < getEndY());
    return data_(x - startX_, y - startY_);
}

template <class T>
SPL_SEQUENCE2_INLINE T& Sequence2<T>::operator()(int x, int y)
{
    assert(x >= startX_ && x < getEndX() && y >= startY_ && y < getEndY());
    return data_(x - startX_, y - startY_);
}

////////////////////////////////////////////////////////////////////////////////
// Iterators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::ConstIterator Sequence2<T>::begin()
  const
{
	return data_.begin();
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::Iterator Sequence2<T>::begin()
{
	return data_.begin();
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::ConstIterator Sequence2<T>::end()
  const
{
	return data_.end();
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::Iterator Sequence2<T>::end()
{
	return data_.end();
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::ConstXIterator
  Sequence2<T>::rowBegin(int y) const
{
	assert(y >= startY_ && y < getEndY());
	return data_.rowBegin(y - startY_);
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::XIterator
  Sequence2<T>::rowBegin(int y)
{
	assert(y >= startY_ && y < getEndY());
	return data_.rowBegin(y - startY_);
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::ConstXIterator
  Sequence2<T>::rowEnd(int y) const
{
	assert(y >= startY_ && y < getEndY());
	return data_.rowEnd(y - startY_);
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::XIterator
  Sequence2<T>::rowEnd(int y)
{
	assert(y >= startY_ && y < getEndY());
	return data_.rowEnd(y - startY_);
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::ConstYIterator
  Sequence2<T>::colBegin(int x) const
{
	assert(x >= startX_ && x < getEndX());
	return data_.colBegin(x - startX_);
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::YIterator
  Sequence2<T>::colBegin(int x)
{
	assert(x >= startX_ && x < getEndX());
	return data_.colBegin(x - startX_);
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::ConstYIterator
  Sequence2<T>::colEnd(int x) const
{
	assert(x >= startX_ && x < getEndX());
	return data_.colEnd(x - startX_);
}

template <class T>
SPL_SEQUENCE2_INLINE typename Sequence2<T>::YIterator
  Sequence2<T>::colEnd(int x)
{
	assert(x >= startX_ && x < getEndX());
	return data_.colEnd(x - startX_);
}

////////////////////////////////////////////////////////////////////////////////
// Resizing
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Get various statistics
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE2_INLINE T Sequence2<T>::min() const
{
	assert(getSize() > 0);
	return data_.min();
}

template <class T>
SPL_SEQUENCE2_INLINE T Sequence2<T>::max() const
{
	assert(getSize() > 0);
	return data_.max();
}

template <class T>
SPL_SEQUENCE2_INLINE T Sequence2<T>::sum() const
{
	return data_.sum();
}

////////////////////////////////////////////////////////////////////////////////
// Input/output
////////////////////////////////////////////////////////////////////////////////

template <class T>
std::ostream& Sequence2<T>::output(std::ostream& out, int fieldWidth) const
{
	out << startX_ << " " << startY_ << " ";
	data_.output(out, fieldWidth);
	return out;
}

/*!
 * @brief Output a sequence to a stream.
 *
 * Effects:
 * The sequence f is written to the output stream out.
 * The output consists of the following information in order:
 * 1) the x-coordinate of the start index of the sequence
 * 2) the y-coordinate of the start index of the sequence
 * 3) the width of the sequence
 * 4) the height of the sequence
 * 5) the elements of the sequence in row-major order
 *
 * Returns:
 * A reference to the stream out is returned.
 */
template <class T>
std::ostream& operator<<(std::ostream& out, const Sequence2<T>& f)
{
	out << f.getStartX() << " " << f.getStartY() << " ";
	out << f.data_;
	return out;
}

/*!
 * @brief Input a sequence from a stream.
 *
 * Effects:
 * The sequence f is read from the input stream in.
 * The data is read in a format consistent with that used by operator<<.
 *
 * Returns:
 * A reference to the stream in is returned.
 */
template <class T>
std::istream& operator>>(std::istream& in, Sequence2<T>& f)
{
	int startX;
	int startY;
	int width;
	int height;
	if (!(in >> startX)) {
		return in;
	}
	if (!(in >> startY)) {
		return in;
	}
	if (!(in >> width)) {
		return in;
	}
	if (!(in >> height)) {
		return in;
	}
	typename std::vector<T> data;
	data.reserve(width * height);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			T value;
			if (!(in >> value)) {
				return in;
			}
			data.push_back(value);
		}
	}
	f = Sequence2<T>(startX, startY, width, height, data.begin());
	return in;
}

////////////////////////////////////////////////////////////////////////////////
// Miscellaneous
////////////////////////////////////////////////////////////////////////////////

template<class T>
SPL_SEQUENCE2_INLINE void Sequence2<T>::fill(const T& value)
{
	data_.fill(value);
}

template <class T>
SPL_SEQUENCE2_INLINE void Sequence2<T>::swapArray(Array2<T>& data)
{
	data_.swap(data);
}

template<class T>
SPL_SEQUENCE2_INLINE Array2<T> Sequence2<T>::getArray() const
{
	return data_;
}

template<class T>
SPL_SEQUENCE2_INLINE Sequence2<T>& Sequence2<T>::translate(int x, int y)
{
	startX_ += x;
	startY_ += y;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Compute the sum of two sequences.
 *
 * Returns:
 * The sum of the sequences f and g is returned.
 * Both sequences must have the same domain.
 */
template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator+(const Sequence2<T>& f,
  const Sequence2<T>& g)
{
	return Sequence2<T>(f) += g;
}

/*!
 * @brief Compute the difference of two sequences.
 *
 * Returns:
 * The difference between the sequence f and sequence g (i.e., f - g) is
 * returned.
 * Both sequences must have the same domain.
 */
template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator-(const Sequence2<T>& f,
  const Sequence2<T>& g)
{
	return Sequence2<T>(f) -= g;
}

/*!
 * @brief Compute the (element-wise) product of two sequences.
 *
 * Returns:
 * The element-wise product of the sequences f and g is returned.
 * Both sequences must have the same domain.
 */
template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator*(const Sequence2<T>& f,
  const Sequence2<T>& g)
{
	return Sequence2<T>(f) *= g;
}

/*!
 * @brief Compute the (element-wise) quotient of two sequences.
 *
 * Returns:
 * The element-wise quotient of the sequences f and g is returned.
 * Both sequences must have the same domain.
 */
template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator/(const Sequence2<T>& f,
  const Sequence2<T>& g)
{
	return Sequence2<T>(f) /= g;
}

/*!
 * @brief Compute the sum of two sequences with potentially differing domains.
 *
 * Effects:
 * The sum of the sequences f and g is computed.
 * The domain of the sum is taken to be the smallest domain that
 * contains the domains of both of the sequences being summed.
 *
 * Returns:
 * The sum is returned.
 */
template <class T>
Sequence2<T> add(const Sequence2<T>& f, const Sequence2<T>& g)
{
	int startX;
	int startY;
	int endX;
	int endY;
	combineDomains(f.getStartX(), f.getStartY(), f.getEndX(), f.getEndY(),
	  g.getStartX(), g.getStartY(), g.getEndX(), g.getEndY(), startX, startY,
	  endX, endY);
	Sequence2<T> result(startX, startY, endX - startX, endY - startY, T(0));
	int xMin = std::max(f.getStartX(), startX);
	int xMax = std::min(f.getEndX(), endX) - 1;
	int yMin = std::max(f.getStartY(), startY);
	int yMax = std::min(f.getEndY(), endY) - 1;
	for (int y = yMin; y <= yMax; ++y) {
		for (int x = xMin; x <= xMax; ++x) {
			result(x, y) += f(x, y);
		}
	}
	xMin = std::max(g.getStartX(), startX);
	xMax = std::min(g.getEndX(), endX) - 1;
	yMin = std::max(g.getStartY(), startY);
	yMax = std::min(g.getEndY(), endY) - 1;
	for (int y = yMin; y <= yMax; ++y) {
		for (int x = xMin; x <= xMax; ++x) {
			result(x, y) += g(x, y);
		}
	}
	return result;
}

/*!
 * @brief Add a value to a sequence.
 *
 * Returns:
 * The sequence f with a added to each of its elements is returned.
 */

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator+(const T& value,
  const Sequence2<T>& f)
{
	return Sequence2<T>(f) += value;
}

/*!
 * @brief Add a value to a sequence.
 *
 * Returns:
 * The sequence f with a added to each of its elements is returned.
 */

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator+(const Sequence2<T>& f,
  const T& value)
{
	return Sequence2<T>(f) += value;
}

/*!
 * @brief Subtract a value from a sequence.
 *
 * Returns:
 * The sequence f with a subtracted from each of its elements is returned.
 */

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator-(const Sequence2<T>& f,
  const T& value)
{
	return Sequence2<T>(f) -= value;
}

/*!
 * @brief Compute a scalar multiple of a sequence.
 *
 * Returns:
 * The sequence f multiplied by the value a is returned.
 */

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator*(const T& value,
  const Sequence2<T>& f)
{
	return Sequence2<T>(f) *= value;
}

/*!
 * @brief Compute a scalar multiple of a sequence.
 *
 * Returns:
 * The sequence f multiplied by the value a is returned.
 */

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator*(const Sequence2<T>& f,
  const T& value)
{
	return Sequence2<T>(f) *= value;
}

/*!
 * @brief Divide a sequence by a scalar.
 *
 * Returns:
 * The sequence f divided by the value a is returned.
 */

template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> operator/(const Sequence2<T>& f,
  const T& value)
{
	return Sequence2<T>(f) /= value;
}

/*!
 * @brief Test two sequences for equality.
 *
 * In order for two sequences to be deemed equal, they must be defined
 * on the same domain and have their element values match everywhere
 * in this domain.
 */

template <class T>
bool operator==(const Sequence2<T>& f, const Sequence2<T>& g)
{
	bool result;
	if (!f.getSize() && !g.getSize()) {
		// Both sequences are empty.
		result = true;
	} else if (f.getStartX() != g.getStartX() ||
		f.getEndX() != g.getEndX() || f.getStartY() != g.getStartY() ||
		  f.getEndY() != g.getEndY()) {
		// The sequences are not defined on the same domain.
		result = false;
	} else {
		// The sequences are defined on the same domain.
		result = std::equal(f.begin(), f.end(), g.begin());
	}
	return result;
}

/*!
 * @brief Test two sequences for inequality.
 */
template <class T>
SPL_SEQUENCE2_INLINE bool operator!=(const Sequence2<T>& f,
  const Sequence2<T>& g)
{
	return !(f == g);
}

/*!
 * @brief Test two sequences for approximate equality.
 */
template <class T>
SPL_SEQUENCE2_INLINE bool approxEqual(const Sequence2<T>& f,
  const Sequence2<T>& g, T threshold = 1e-9)
{
	bool result;
	if (!f.getSize() && !g.getSize()) {
		// Empty sequence.
		result = true;
	} else if (f.getStartX() != g.getStartX() ||
	  f.getStartY() != g.getStartY() ||
	  f.getEndX() != g.getEndX() ||
	  f.getEndY() != g.getEndY()) {
		// Domains do not match.
		result = false;
	} else {
		assert(f.getWidth() == g.getWidth() && f.getHeight() == g.getHeight());
		result = true;
		typename Sequence2<T>::ConstIterator src = f.begin();
		typename Sequence2<T>::ConstIterator dst = g.begin();
		while (src != f.end()) {
			if (absVal((*src) - (*dst)) > threshold) {
				result = false;
				break;
			}
			++src;
			++dst;
		}
	}
	return result;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Extract a subsequence from a sequence.
 *
 * Effects:
 * The subsequence with start index startInd and size size is extracted from
 * the sequence f.
 *
 * Returns:
 * The extracted subsequence is returned.
 */
template <class T>
Sequence2<T> subsequence(const Sequence2<T>& f, int startX, int startY,
  int width, int height)
{
	assert(startX >= f.getStartX() && startX + width <= f.getEndX() &&
	  startY >= f.getStartY() && startY + height <= f.getEndY());
	if (!width || !height) {
		width = 0;
		height = 0;
	}
	Sequence2<T> result(startX, startY, width, height);
	for (int i = 0; i < height; ++i) {
		assert(startX - f.getStartX() >= 0);
		typename Sequence2<T>::ConstXIterator src =
		  f.rowBegin(startY + i) + (startX - f.getStartX());
		typename Sequence2<T>::XIterator dst = result.rowBegin(startY + i);
		SPL::copy_n(src, width, dst);
	}
	return result;
}

/*!
 * @brief Translate a sequence by the specified amount.
 *
 * Effects:
 * The sequence f is translated by (deltaX, deltaY).
 *
 * Returns:
 * The translated sequence is returned.
 */
template <class T>
SPL_SEQUENCE2_INLINE Sequence2<T> translate(const Sequence2<T>& f,
  int deltaX, int deltaY)
{
	return Sequence2<T>(f).translate(deltaX, deltaY);
}

////////////////////////////////////////////////////////////////////////////////
// Convolution
////////////////////////////////////////////////////////////////////////////////

/// @cond

/*!
 * @brief Get the value of an element in the extended version of a sequence.
 *
 * DO NOT CALL THIS FUNCTION DIRECTLY!
 */
template<class T>
SPL_SEQUENCE2_INLINE T getExtSeqValue(const Sequence2<T>& f, int x, int y,
  int mode)
{
	assert(mode == ConvolveMode::full ||
	  mode == ConvolveMode::sameDomainConstExt ||
	  mode == ConvolveMode::sameDomainZeroExt ||
	  mode == ConvolveMode::sameDomainPerExt ||
	  mode == ConvolveMode::sameDomainSymExt0);
	T value;
	switch (mode) {
	case ConvolveMode::full:
	case ConvolveMode::sameDomainZeroExt:
		if (x >= f.getStartX() && x < f.getEndX() &&
		  y >= f.getStartY() && y < f.getEndY()) {
			value = f(x, y);
		} else {
			value = T(0);
		}
		break;
	case ConvolveMode::sameDomainConstExt:
		if (x < f.getStartX()) {
			x = f.getStartX();
		} else if (x >= f.getEndX()) {
			x = f.getEndX() - 1;
		}
		if (y < f.getStartY()) {
			y = f.getStartY();
		} else if (y >= f.getEndY()) {
			y = f.getEndY() - 1;
		}
		value = f(x, y);
		break;
	case ConvolveMode::sameDomainPerExt:
		if (x < f.getStartX() || x >= f.getEndX()) {
			x = f.getStartX() + mod(x - f.getStartX(), f.getWidth());
		}
		if (y < f.getStartY() || y >= f.getEndY()) {
			y = f.getStartY() + mod(y - f.getStartY(), f.getHeight());
		}
		assert(x >= f.getStartX() && x < f.getEndX());
		assert(y >= f.getStartY() && y < f.getEndY());
		value = f(x, y);
		break;
	case ConvolveMode::sameDomainSymExt0:
		if (x < f.getStartX() || x >= f.getEndX()) {
			int tmp = mod(x - f.getStartX(), 2 * f.getWidth() - 2);
			x = f.getStartX() + std::min(tmp, 2 * f.getWidth() - 2 - tmp);
		}
		if (y < f.getStartY() || y >= f.getEndY()) {
			int tmp = mod(y - f.getStartY(), 2 * f.getHeight() - 2);
			y = f.getStartY() + std::min(tmp, 2 * f.getHeight() - 2 - tmp);
		}
		assert(x >= f.getStartX() && x < f.getEndX());
		assert(y >= f.getStartY() && y < f.getEndY());
		value = f(x, y);
		break;
	default:
		assert(0);
		abort();
		break;
	}
	return value;
}
/// @endcond

/// @cond
/*!
 * @brief Compute a full separable convolution.
 *
 * DO NOT CALL THIS FUNCTION DIRECTLY!
 */
template <class T>
Sequence2<T> convolveSeparableFull(const Sequence2<T>& f,
  const Sequence1<T>& horzFilt, const Sequence1<T>& vertFilt)
{
	if (!horzFilt.getSize() || !vertFilt.getSize()) {
		return Sequence2<T>();
	}

	// Perform the horizontal filtering.
	Sequence2<T> tmpResult(f.getStartX() + horzFilt.getStartInd(),
	  f.getStartY(), f.getWidth() + horzFilt.getSize() - 1,
	  f.getHeight());
	for (int y = f.getStartY(); y < f.getEndY(); ++y) {
		convolveHelper(f.rowBegin(y), f.rowEnd(y), horzFilt.begin(),
		  horzFilt.end(), tmpResult.rowBegin(y), T());
	}

	// Perform the vertical filtering.
	Sequence2<T> result(tmpResult.getStartX(), tmpResult.getStartY() +
	  vertFilt.getStartInd(), tmpResult.getWidth(), tmpResult.getHeight() +
	  vertFilt.getSize() - 1);
	for (int x = tmpResult.getStartX(); x < tmpResult.getEndX(); ++x) {
		convolveHelper(tmpResult.colBegin(x), tmpResult.colEnd(x),
		  vertFilt.begin(), vertFilt.end(), result.colBegin(x), T());
	}

	return result;
}
/// @endcond

/// @cond
/*!
 * @brief Compute a same-domain zero-extended separable convolution.
 *
 * DO NOT CALL THIS FUNCTION DIRECTLY!
 */
template <class T>
Sequence2<T> convolveSeparableSameDomainZeroExt(const Sequence2<T>& f,
  const Sequence1<T>& horzFilt, const Sequence1<T>& vertFilt)
{
	if (!horzFilt.getSize() || !vertFilt.getSize()) {
		return Sequence2<T>();
	}

	int skip;
	int shift;

	// Perform the horizontal filtering.
	Sequence2<T> tmpResult(f.getStartX(), f.getStartY(), f.getWidth(),
	  f.getHeight());
	if (horzFilt.getStartInd() >= 0) {
		shift = horzFilt.getStartInd();
		skip = 0;
	} else {
		shift = 0;
		skip = -horzFilt.getStartInd();
	}
	for (int y = f.getStartY(); y < f.getEndY(); ++y) {
		convolveHelper2(f.rowBegin(y), f.rowEnd(y), horzFilt.begin(),
		  horzFilt.end(), tmpResult.rowBegin(y) + shift, skip,
		  tmpResult.getWidth() - shift, T());
	}

	// Perform the vertical filtering.
	if (vertFilt.getStartInd() >= 0) {
		shift = vertFilt.getStartInd();
		skip = 0;
	} else {
		shift = 0;
		skip = -vertFilt.getStartInd();
	}
	Sequence2<T> result(tmpResult.getStartX(), tmpResult.getStartY(),
	  tmpResult.getWidth(), tmpResult.getHeight());
	for (int x = tmpResult.getStartX(); x < tmpResult.getEndX(); ++x) {
		convolveHelper2(tmpResult.colBegin(x), tmpResult.colEnd(x),
		  vertFilt.begin(), vertFilt.end(), result.colBegin(x) + shift,
		  skip, result.getHeight() - shift, T());
	}

	return result;
}
/// @endcond

/*!
 * @brief Compute the convolution of two sequences.
 */
template<class T>
Sequence2<T> convolve(const Sequence2<T>& f, const Sequence2<T>& g, int mode)
{
	assert(mode == ConvolveMode::full ||
	  mode == ConvolveMode::sameDomainConstExt ||
	  mode == ConvolveMode::sameDomainZeroExt ||
	  mode == ConvolveMode::sameDomainPerExt ||
	  mode == ConvolveMode::sameDomainSymExt0);
	assert(f.getWidth() > 0 && f.getHeight() > 0);
	assert(g.getWidth() > 0 && g.getHeight() > 0);

	Sequence2<T> result;

	switch (mode) {
	case ConvolveMode::full:
		result = Sequence2<T>(f.getStartX() + g.getStartX(),
		  f.getStartY() + g.getStartY(), f.getWidth() + g.getWidth() - 1,
		  f.getHeight() + g.getHeight() - 1);
		break;
	case ConvolveMode::sameDomainConstExt:
	case ConvolveMode::sameDomainZeroExt:
	case ConvolveMode::sameDomainPerExt:
	case ConvolveMode::sameDomainSymExt0:
		result = Sequence2<T>(f.getStartX(), f.getStartY(), f.getWidth(),
		  f.getHeight());
		break;
	default:
		assert(0);
		abort();
		break;
	}

	for (int y = result.getStartY(); y < result.getEndY(); ++y) {
		for (int x = result.getStartX(); x < result.getEndX(); ++x) {
			T sum = T(0);
			for (int j = g.getStartY(); j < g.getEndY(); ++j) {
				for (int i = g.getStartX(); i < g.getEndX(); ++i) {
					sum += getExtSeqValue(f, x - i, y - j, mode) * g(i, j);
				}
			}
			result(x, y) = sum;
		}
	}

	return result;
}

/*!
 * @brief Compute the convolution of a sequence with two 1-D filters
 * (i.e., convolution with a separable filter).
 */
template <class T>
Sequence2<T> convolveSeparable(const Sequence2<T>& f,
  const Sequence1<T>& horzFilt, const Sequence1<T>& vertFilt,
  int mode = ConvolveMode::full)
{
#if defined(SPL_SEQUENCE2_USE_NEW_CONV)
	switch (mode) {
	case ConvolveMode::full:
		return convolveSeparableFull(f, horzFilt, vertFilt);
		break;
	case ConvolveMode::sameDomainZeroExt:
		return convolveSeparableSameDomainZeroExt(f, horzFilt, vertFilt);
		break;
	}
#endif
	// The following code is quite inefficient.  Rewrite it someday.
	Sequence2<T> horzFilt0(horzFilt.getStartInd(), 0, horzFilt.getSize(), 1,
	  horzFilt.begin());
	Sequence2<T> vertFilt0(0, vertFilt.getStartInd(), 1, vertFilt.getSize(),
	  vertFilt.begin());
	Sequence2<T> result = convolve(f, horzFilt0, mode);
	result = convolve(result, vertFilt0, mode);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Downsampling and upsampling
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Downsample a sequence in each of the horizontal and vertical
 * directions by the specified factors.
 */
template <class T>
Sequence2<T> downsample(const Sequence2<T>& f, int factorX, int factorY)
{
	assert(factorX >= 1 && factorY >= 1);
	int startX = ceilDiv(f.getStartX(), factorX) * factorX;
	int startY = ceilDiv(f.getStartY(), factorY) * factorY;
	int width = ceilDiv(f.getEndX() - startX, factorX);
	int height = ceilDiv(f.getEndY() - startY, factorY);
	Sequence2<T> result(roundTowardZeroDiv(startX, factorX),
	  roundTowardZeroDiv(startY, factorY), width, height);

	for (int i = 0; i < height; ++i) {
		typename Sequence2<T>::ConstXIterator srcIter =
		  f.rowBegin(startY + factorY * i) + (startX - f.getStartX());
		typename Sequence2<T>::XIterator dstIter =
		  result.rowBegin(result.getStartY() + i);
		int n = result.getWidth();
		while (--n >= 0) {
			*dstIter = *srcIter;
			++dstIter;
			srcIter += factorX;
		}
	}

	return result;
}

/*!
 * @brief Upsample a sequence in each of the horizontal and vertical
 * directions by the specified factors.
 *
 * The following condition should always be true:
 *     downsample(upsample(f, factorX, factorY), factorX, factorY) == f.
 */
template <class T>
Sequence2<T> upsample(const Sequence2<T>& f, int factorX, int factorY)
{
	return upsample(f, factorX, factorY, 0, 0);
}

/*!
 * @brief Upsample a sequence in each of the horizontal and vertical
 * directions by the specified factors.
 *
 * The following condition should always be true:
 *     downsample(upsample(f, factorX, factorY), factorX, factorY) == f.
 */
template <class T>
Sequence2<T> upsample(const Sequence2<T>& f, int factorX, int factorY,
  int padX, int padY)
{
	assert(factorX >= 1 && factorY >= 1);
	assert(padX >= 0 && padX < factorX);
	assert(padY >= 0 && padY < factorY);

	Sequence2<T> result(factorX * f.getStartX(), factorY * f.getStartY(),
	  f.getWidth() + std::max((f.getWidth() - 1), 0) * (factorX - 1) + padX,
	  f.getHeight() + std::max((f.getHeight() - 1), 0) * (factorY - 1) + padY);
	int srcY = f.getStartY();
	int dstY = result.getStartY();
	int rowCount = f.getHeight();

	while (rowCount > 0) {
		typename Sequence2<T>::ConstXIterator srcIter = f.rowBegin(srcY);
		typename Sequence2<T>::XIterator dstIter = result.rowBegin(dstY);
		int count = f.getWidth();

		while (count > 0) {
			*dstIter = *srcIter;
			++srcIter;
			++dstIter;
			for (int k = (count > 1) * (factorX - 1); k > 0; --k) {
				*dstIter = T(0);
				++dstIter;
			}
			--count;
		}
		for (int k = padX; k > 0; --k) {
			*dstIter = T(0);
			++dstIter;
		}
		++srcY;
		++dstY;

		for (int k = (rowCount > 1) * (factorY - 1); k > 0; --k) {
			std::fill_n(result.rowBegin(dstY), result.getWidth(), T(0));
			++dstY;
		}
		--rowCount;
	}

	for (int k = padY; k > 0; --k) {
		assert(dstY < result.getEndY());
		std::fill_n(result.rowBegin(dstY), result.getWidth(), T(0));
		++dstY;
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Polyphase decompositions
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Split a sequence into its polyphase components.
 *
 * Effects:
 * The polyphase decomposition of the sequence seq is computed.
 * In particular, the polyphase decomposition with numPhase phases
 * and type type is computed.
 *
 * Returns:
 * An array containing the polyphase components is returned.
 */

template <class T>
Array2<Sequence2<T> > polyphaseSplit(const Sequence2<T>& seq, int typeX,
  int numPhasesX, int typeY, int numPhasesY)
{
	assert(numPhasesX >= 2);
	assert(numPhasesY >= 2);
	Array2<Sequence2<T> > result(numPhasesX, numPhasesY);
	for (int j = 0; j < numPhasesY; ++j) {
		for (int i = 0; i < numPhasesX; ++i) {
			int offsetX = getCosetOffset(typeX, numPhasesX, i);
			int offsetY = getCosetOffset(typeY, numPhasesY, j);
			result(i, j) = downsample(translate(seq, offsetX, offsetY),
			  numPhasesX, numPhasesY);
		}
	}
	return result;
}

/*!
 * @brief Reassemble a sequence from its polyphase components.
 *
 * Effects:
 * A sequence is recomposed from its polyphase components comps.
 * A polyphase decomposition of type type is assumed.
 *
 * Returns:
 * The recomposed sequence is returned.
 */

template <class T>
Sequence2<T> polyphaseJoin(const Array2<Sequence2<T> >& comps, int typeX,
  int typeY)
{
	const int numPhasesX = comps.getWidth();
	const int numPhasesY = comps.getHeight();
	assert(numPhasesX >= 2);
	assert(numPhasesY >= 2);
	Sequence2<T> seq;
	for (int j = 0; j < numPhasesY; ++j) {
		for (int i = 0; i < numPhasesX; ++i) {
			int offsetX = getCosetOffset(typeX, numPhasesX, i);
			int offsetY = getCosetOffset(typeY, numPhasesY, j);
//std::cerr << "XXX " << i << " " << j << " " << upsample(comps(i, j), numPhasesX, numPhasesY) << "\n";
			seq = add(seq, translate(upsample(comps(i, j), numPhasesX,
			  numPhasesY), -offsetX, -offsetY));
		}
	}
	return seq;
}

////////////////////////////////////////////////////////////////////////////////
// Types.
////////////////////////////////////////////////////////////////////////////////

//! Real sequence
typedef Sequence2<double> RealSequence2;

//! Integer sequence
typedef Sequence2<int> IntSequence2;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/**
 * @}
 */

}

#endif

