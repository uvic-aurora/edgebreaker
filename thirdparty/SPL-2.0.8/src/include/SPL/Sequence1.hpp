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
 * @file Sequence1.hpp
 * @brief This file contains code for the Sequence1 template class.
 */

#ifndef SPL_Sequence1_hpp
#define SPL_Sequence1_hpp

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

//! Defining this symbol will enable extra code for debugging.
#define SPL_SEQUENCE1_DEBUG

//! Defining this symbol will enable the use of new convolution code.
#define SPL_SEQUENCE1_USE_NEW_CONV

#if defined(SPL_SEQUENCE1_DEBUG)
//! Prevent the inlining of functions.
#define SPL_SEQUENCE1_INLINE
#else
//! Allow the inlining of functions.
#define SPL_SEQUENCE1_INLINE inline
#endif

////////////////////////////////////////////////////////////////////////////////
// Header Files.
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <vector>
#include <SPL/Array1.hpp>
#include <SPL/math.hpp>
#include <SPL/Sequence.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// One-Dimensional Sequence Template Class.
////////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup Sequence1
 * @{
 */

/*!
 * @brief A one-dimensional sequence class with lazy copying and reference
 * counting.
 */

template <class T>
class Sequence1
{
public:

	////////////////////////////////////////////////////////////
	// Types
	////////////////////////////////////////////////////////////

	//! The type of the element in the sequence.
	typedef T ElemType;

	//! The const iterator for the sequence.
	typedef typename Array1<T>::ConstIterator ConstIterator;

	//! The mutable iterator for the sequence.
	typedef typename Array1<T>::Iterator Iterator;

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief The default constructor.
	 */
	Sequence1();

	/*!
	 * @brief Construct a sequence with the specified start index and size.
	 *
	 * Effects:
	 * A sequence with a starting index of startInd and size size is
	 * created.
	 * The elements in the sequence are default constructed!
	 */
	Sequence1(int startInd, int size);

	/*!
	 * @brief Construct a sequence with the specified start index and size,
	 * with all elements set to the given value.
	 *
	 * Effects:
	 * A sequence with a starting index of startInd and size size is
	 * created, with all elements initialized to the value value.
	 */
	Sequence1(int startInd, int size, const T& value);

	/*!
	 * @brief Construct a sequence with the specified start index and size,
	 * with the elements initialized to the data read from the given iterator.
	 *
	 * Effects:
	 * A sequence with a starting index of startInd and size size is
	 * created, with the elements being initialized by the data
	 * pointed to by data.
	 */
	template <class InputIterator>
	Sequence1(int startInd, int size, InputIterator data);

	/*!
	 * @brief The copy constructor.
	 */
	Sequence1(const Sequence1& f);

	/*!
	 * @brief Create a sequence from another sequence having elements of
	 * a different type.
	 */
	template <class OtherT>
	Sequence1(const Sequence1<OtherT>& f);

	/*!
	 * @brief Create a sequence from an array.
	 */
	Sequence1(const Array1<T>& data);

	/*!
	 * @brief Create a sequence from an array using the given starting index.
	 */
	Sequence1(int startInd, const Array1<T>& data);

	/*!
	 * @brief The destructor.
	 */
	~Sequence1();

	////////////////////////////////////////////////////////////
	// Assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief The assignment operator.
	 */
	Sequence1& operator=(const Sequence1& f);

	/*!
	 * @brief Assign another sequence with elements of a different type
	 * to this sequence.
	 *
	 * The type OtherT must be assignable to the type T.
	 */
	template <class OtherT>
	Sequence1& operator=(const Sequence1<OtherT>& f);

	////////////////////////////////////////////////////////////
	// Compound assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Add another sequence to this one.
	 *
	 * Effects:
	 * The sequence f is added to this sequence.
	 * Both sequences must have the same domain.
	 */
	Sequence1& operator+=(const Sequence1& f);

	/*!
	 * @brief Subtract another sequence from this one.
	 *
	 * Effects:
	 * The sequence f is subtracted from this sequence.
	 * Both sequences must have the same domain.
	 */
	Sequence1& operator-=(const Sequence1& f);

	/*!
	 * @brief Multiply elementwise this sequence by another one.
	 *
	 * Effects:
	 * This sequence is multiplied (element-wise) by the sequence f.
	 * Both sequences must have the same domain.
	 */
	Sequence1& operator*=(const Sequence1& f);

	/*!
	 * @brief Divide elementwise this sequence by another one.
	 *
	 * Effects:
	 * This sequence is divided (element-wise) by the sequence f.
	 * Both sequences must have the same domain.
	 */
	Sequence1& operator/=(const Sequence1& f);

	/*!
	 * @brief Add a value to each element of this sequence.
	 *
	 * Effects:
	 * The value value is added to each element of the sequence.
	 */
	Sequence1& operator+=(const T& value);

	/*!
	 * @brief Subtract a value from each element of this sequence.
	 *
	 * Effects:
	 * The value value is subtracted from each element of the sequence.
	 */
	Sequence1& operator-=(const T& value);

	/*!
	 * @brief Multiply each element of this sequence by the specified value.
	 *
	 * Effects:
	 * This sequence is multiplied by the element value value.
	 */
	Sequence1& operator*=(const T& value);

	/*!
	 * @brief Divide each element of the sequence by the given value.
	 *
	 * Effects:
	 * Each element of the sequence is divided by the value value.
	 */
	Sequence1& operator/=(const T& value);

	////////////////////////////////////////////////////////////
	// Basic queries
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get the start index for the sequence.
	 *
	 * Returns:
	 * The starting index for the sequence is returned.
	 */
	int getStartInd() const;

	/*!
	 * @brief Get the end index for the sequence.
	 *
	 * Returns:
	 * The ending index (i.e., one past the last valid index) is returned.
	 */
	int getEndInd() const;

	/*!
	 * @brief Get the length of the sequence.
	 *
	 * Returns:
	 * The number of elements in the sequence is returned.
	 * This value is equivalent to getEndInd() - getStartInd().
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
	 * @brief Get the specified element in the sequence.
	 *
	 * Returns:
	 * A reference to the i-th element in the sequence is returned.
	 */
	const T& operator()(int i) const;

	/*!
	 * @brief Get the specified element in the sequence.
	 *
	 * Returns:
	 * A reference to the i-th element in the sequence is returned.
	 */
	T& operator()(int i);

	////////////////////////////////////////////////////////////
	// Iterators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get an iterator referencing the first element in the sequence.
	 *
	 * Returns:
	 * An iterator referencing the first element in the sequence (i.e.,
	 * the element with index getStartInd()) is returned.
	 */
	ConstIterator begin() const;

	/*!
	 * @brief Get an iterator referencing the first element in the sequence.
	 *
	 * Returns:
	 * An iterator referencing the first element in the sequence (i.e.,
	 * the element with index getStartInd()) is returned.
	 */
	Iterator begin();

	/*!
	 * @brief Get an iterator referencing just after the last element in
	 * the sequence.
	 *
	 * Returns:
	 * An iterator for the end of the sequence (i.e., one past the last
	 * element) is returned.
	 */
	ConstIterator end() const;

	/*!
	 * @brief Get an iterator referencing just after the last element in
	 * the sequence.
	 *
	 * Returns:
	 * An iterator for the end of the sequence (i.e., one past the last
	 * element) is returned.
	 */
	Iterator end();

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

	////////////////////////////////////////////////////////////
	// Miscellaneous
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a copy of the underlying array.
	 */
	Array1<T> getArray() const;

	/*!
	 * @brief Swap the data for the underlying array and the specified array.
	 */
	void swapArray(Array1<T>& data);

	/*!
	 * @brief Set all of the elements in the sequence to the specified value.
	 *
	 * Effects:
	 * Each elements in the sequence is set to the value value.
	 */
	void fill(const T& value);

	/*!
	 * @brief Translate (i.e., shift) a sequence by the specified displacement.
	 */
	Sequence1& translate(int delta);

private:

	// Do not look at the private members.
	// Doing so will make your eyes bleed.

	template <class> friend class Sequence1;

	template <class U>
	friend std::ostream& operator<<(std::ostream& out, const Sequence1<U>&);
	template <class U>
	friend Sequence1<U> translate(const Sequence1<U>&, int i);

	//! The type of the underlying array.
	typedef Array1<T> Array;

	//! The index of the first element.
	int startInd_;

	//! The sequence element data.
	Array data_;

};

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1() : startInd_(0), data_()
{
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1(int startInd, int size) :
  startInd_(startInd), data_(size)
{
	assert(size >= 0);
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1(int startInd, int size,
  const T& data) : startInd_(startInd), data_(size, data)
{
	assert(size >= 0);
}

template <class T>
template <class InputIterator>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1(int startInd, int size,
  InputIterator data) : startInd_(startInd), data_(size, data)
{
	assert(size >= 0);
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1(const Array1<T>& f) :
  startInd_(0), data_(f)
{
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1(int startInd,
  const Array1<T>& data) : startInd_(startInd), data_(data)
{
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1(const Sequence1<T>& f) :
  startInd_(f.startInd_), data_(f.data_)
{
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>::~Sequence1()
{
}

template <class T>
template <class OtherT>
SPL_SEQUENCE1_INLINE Sequence1<T>::Sequence1(const Sequence1<OtherT>& f) :
  startInd_(f.startInd_), data_(f.data_)
{
}

////////////////////////////////////////////////////////////////////////////////
// Assignment operators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator=(
  const Sequence1<T>& f)
{
	startInd_ = f.startInd_;
	data_ = f.data_;
	return *this;
}

template <class T>
template <class OtherT>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator=(
  const Sequence1<OtherT>& f)
{
	startInd_ = f.startInd_;
	data_ = f.data_;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Compound assignment operators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator+=(
  const Sequence1<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((getSize() == 0 && f.getSize() == 0) ||
	  (getStartInd() == f.getStartInd() && getEndInd() == f.getEndInd()));

	data_ += f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator-=(
  const Sequence1<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((getSize() == 0 && f.getSize() == 0) ||
	  (getStartInd() == f.getStartInd() && getEndInd() == f.getEndInd()));

	data_ -= f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator*=(
  const Sequence1<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((getSize() == 0 && f.getSize() == 0) ||
	  (getStartInd() == f.getStartInd() && getEndInd() == f.getEndInd()));

	data_ *= f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator/=(
  const Sequence1<T>& f)
{
	// Ensure that both sequences have the same domain.
	assert((getSize() == 0 && f.getSize() == 0) ||
	  (getStartInd() == f.getStartInd() && getEndInd() == f.getEndInd()));

	data_ /= f.data_;
	return *this;
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator+=(const T& value)
{
	data_ += value;
	return *this;
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator-=(const T& value)
{
	data_ -= value;
	return *this;
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator*=(const T& value)
{
	data_ *= value;
	return *this;
}

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::operator/=(const T& value)
{
	data_ /= value;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Basic queries
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE int Sequence1<T>::getStartInd() const
{
	return startInd_;
}

template <class T>
SPL_SEQUENCE1_INLINE int Sequence1<T>::getEndInd() const
{
	return startInd_ + getSize();
}

template <class T>
SPL_SEQUENCE1_INLINE int Sequence1<T>::getSize() const
{
	return data_.getSize();
}

template <class T>
SPL_SEQUENCE1_INLINE bool Sequence1<T>::isShared() const
{
	return data_.isShared();
}

////////////////////////////////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE const T& Sequence1<T>::operator()(int i) const
{
	assert(i >= startInd_ && i < startInd_ + getSize());
	return data_(i - startInd_);
}

template <class T>
SPL_SEQUENCE1_INLINE T& Sequence1<T>::operator()(int i)
{
	assert(i >= startInd_ && i < startInd_ + getSize());
	return data_(i - startInd_);
}

////////////////////////////////////////////////////////////////////////////////
// Iterators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE typename Sequence1<T>::ConstIterator Sequence1<T>::begin()
  const
{
	return data_.begin();
}

template <class T>
SPL_SEQUENCE1_INLINE typename Sequence1<T>::ConstIterator Sequence1<T>::end()
  const
{
	return data_.end();
}

template <class T>
SPL_SEQUENCE1_INLINE typename Sequence1<T>::Iterator Sequence1<T>::begin()
{
	return data_.begin();
}

template <class T>
SPL_SEQUENCE1_INLINE typename Sequence1<T>::Iterator Sequence1<T>::end()
{
	return data_.end();
}

////////////////////////////////////////////////////////////////////////////////
// Resizing
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Get various statistics
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE T Sequence1<T>::min() const
{
	assert(getSize() > 0);
	return data_.min();
}

template <class T>
SPL_SEQUENCE1_INLINE T Sequence1<T>::max() const
{
	assert(getSize() > 0);
	return data_.max();
}

template <class T>
SPL_SEQUENCE1_INLINE T Sequence1<T>::sum() const
{
	return data_.sum();
}


////////////////////////////////////////////////////////////////////////////////
// Input/output
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Output a sequence to a stream.
 *
 * Effects:
 * The sequence f is written to the output stream out.
 * The output consists of the following information in order:
 * 1) the starting index of the sequence
 * 2) the size of the sequence
 * 3) the elements of the sequence in increasing order of index
 *
 * Returns:
 * A reference to the stream out is returned.
 */

template <class T>
std::ostream& operator<<(std::ostream& out, const Sequence1<T>& f)
{
	out << f.getStartInd() << " ";
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
std::istream& operator>>(std::istream& in, Sequence1<T>& f)
{
	int startInd;
	int size;
	if (!(in >> startInd)) {
		return in;
	}
	if (!(in >> size)) {
		return in;
	}
	if (size < 0) {
		in.setstate(std::ios::failbit);
		return in;
	}
	typename std::vector<T> data;
	data.reserve(size);
	for (int i = 0; i < size; ++i) {
		T value;
		if (!(in >> value)) {
			return in;
		}
		data.push_back(value);
	}
	f = Sequence1<T>(startInd, data.size(), data.begin());
	return in;
}

////////////////////////////////////////////////////////////////////////////////
// Miscellaneous
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_SEQUENCE1_INLINE void Sequence1<T>::swapArray(Array1<T>& data)
{
	data_.swap(data);
}

template<class T>
SPL_SEQUENCE1_INLINE void Sequence1<T>::fill(const T& value)
{
	data_.fill(value);
}

template<class T>
SPL_SEQUENCE1_INLINE Array1<T> Sequence1<T>::getArray() const
{
	return data_;
}

template<class T>
SPL_SEQUENCE1_INLINE Sequence1<T>& Sequence1<T>::translate(int delta)
{
	startInd_ += delta;
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
SPL_SEQUENCE1_INLINE Sequence1<T> operator+(const Sequence1<T>& f,
  const Sequence1<T>& g)
{
	return Sequence1<T>(f) += g;
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
SPL_SEQUENCE1_INLINE Sequence1<T> operator-(const Sequence1<T>& f,
  const Sequence1<T>& g)
{
	return Sequence1<T>(f) -= g;
}

/*!
 * @brief Compute the (element-wise) product of two sequences.
 *
 * Returns:
 * The element-wise product of the sequences f and g is returned.
 * Both sequences must have the same domain.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> operator*(const Sequence1<T>& f,
  const Sequence1<T>& g)
{
	return Sequence1<T>(f) *= g;
}

/*!
 * @brief Compute the (element-wise) quotient of two sequences.
 *
 * Returns:
 * The element-wise quotient of the sequences f and g is returned.
 * Both sequences must have the same domain.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> operator/(const Sequence1<T>& f,
  const Sequence1<T>& g)
{
	return Sequence1<T>(f) /= g;
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
Sequence1<T> add(const Sequence1<T>& f, const Sequence1<T>& g)
{
	int startInd;
	int endInd;
	if (!f.getSize()) {
		startInd = g.getStartInd();
		endInd = g.getEndInd();
	} else if (!g.getSize()) {
		startInd = f.getStartInd();
		endInd = f.getEndInd();
	} else {
		startInd = std::min(f.getStartInd(), g.getStartInd());
		endInd = std::max(f.getStartInd() + f.getSize(), g.getStartInd() +
		  g.getSize());
	}
	Sequence1<T> result(startInd, endInd - startInd, T(0));
	int i0 = std::max(f.getStartInd(), startInd);
	int i1 = std::min(f.getStartInd() + f.getSize(), endInd);
	for (int i = i0; i < i1; ++i) {
		result(i) += f(i);
	}
	i0 = std::max(g.getStartInd(), startInd);
	i1 = std::min(g.getStartInd() + g.getSize(), endInd);
	for (int i = i0; i < i1; ++i) {
		result(i) += g(i);
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
SPL_SEQUENCE1_INLINE Sequence1<T> operator+(const T& a, const Sequence1<T>& f)
{
	return Sequence1<T>(f) += a;
}

/*!
 * @brief Add a value to a sequence.
 *
 * Returns:
 * The sequence f with a added to each of its elements is returned.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> operator+(const Sequence1<T>& f, const T& a)
{
	return Sequence1<T>(f) += a;
}

/*!
 * @brief Subtract a value from a sequence.
 *
 * Returns:
 * The sequence f with a subtracted from each of its elements is returned.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> operator-(const Sequence1<T>& f, const T& a)
{
	return Sequence1<T>(f) -= a;
}

/*!
 * @brief Compute a scalar multiple of a sequence.
 *
 * Returns:
 * The sequence f multiplied by the value a is returned.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> operator*(const T& a, const Sequence1<T>& f)
{
	return Sequence1<T>(f) *= a;
}

/*!
 * @brief Compute a scalar multiple of a sequence.
 *
 * Returns:
 * The sequence f multiplied by the value a is returned.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> operator*(const Sequence1<T>& f, const T& a)
{
	return Sequence1<T>(f) *= a;
}

/*!
 * @brief Divide a sequence by a scalar.
 *
 * Returns:
 * The sequence f divided by the value a is returned.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> operator/(const Sequence1<T>& f, const T& a)
{
	return Sequence1<T>(f) /= a;
}

/*!
 * @brief Test two sequences for equality.
 *
 * In order for two sequences to be deemed equal, they must be defined
 * on the same domain and have their element values match everywhere
 * in this domain.
 */
template <class T>
SPL_SEQUENCE1_INLINE bool operator==(const Sequence1<T>& f,
  const Sequence1<T>& g)
{
	bool result;
	if (!f.getSize() && !g.getSize()) {
		result = true;
	} else if (f.getStartInd() != g.getStartInd() ||
	  f.getEndInd() != g.getEndInd()) {
		result = false;
	} else {
		result = std::equal(f.begin(), f.end(), g.begin());
	}
	return result;
}

/*!
 * @brief Test two sequences for inequality.
 */
template <class T>
SPL_SEQUENCE1_INLINE bool operator!=(const Sequence1<T>& f,
  const Sequence1<T>& g)
{
	return !(f == g);
}

/*!
 * @brief Test two sequences for approximate equality.
 */
template <class T>
SPL_SEQUENCE1_INLINE bool approxEqual(const Sequence1<T>& f,
  const Sequence1<T>& g, T threshold = 1e-9)
{
	bool result;
	if (!f.getSize() && !g.getSize()) {
		// Empty sequence.
		result = true;
	} else if (f.getStartInd() != g.getStartInd() ||
	  f.getEndInd() != g.getEndInd()) {
		// Domains do not match.
		result = false;
	} else {
		result = true;
		typename Sequence1<T>::ConstIterator src = f.begin();
		typename Sequence1<T>::ConstIterator dst = g.begin();
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
Sequence1<T> subsequence(const Sequence1<T>& f, int startInd, int size)
{
	assert(startInd >= f.getStartInd() && startInd + size <= f.getEndInd());
	Sequence1<T> result(startInd, size);
	for (int i = 0; i < size; ++i) {
		result(startInd + i) = f(startInd + i);
	}
	return result;
}

/*!
 * @brief Translate a sequence by the specified amount.
 *
 * Effects:
 * The sequence f is translated (i.e., time shifted) by i.
 * For example, if the sequence f is defined on the domain
 *     a, a+1, ..., b,
 * then the translated sequence will be defined on the domain
 *     a+i, a+i+1, ..., b+i.
 *
 * Returns:
 * The translated sequence is returned.
 */

template <class T>
SPL_SEQUENCE1_INLINE Sequence1<T> translate(const Sequence1<T>& f, int delta)
{
	return Sequence1<T>(f).translate(delta);
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
SPL_SEQUENCE1_INLINE T getExtSeqValue(const Sequence1<T>& f, int i, int mode)
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
		if (i >= f.getStartInd() && i < f.getEndInd()) {
			value = f(i);
		} else {
			value = T(0);
		}
		break;
	case ConvolveMode::sameDomainConstExt:
		if (i < f.getStartInd()) {
			i = f.getStartInd();
		} else if (i >= f.getEndInd()) {
			i = f.getEndInd() - 1;
		}
		assert(i >= f.getStartInd() && i < f.getEndInd());
		value = f(i);
		break;
	case ConvolveMode::sameDomainPerExt:
		if (i < f.getStartInd() || i >= f.getEndInd()) {
			i = f.getStartInd() + mod(i - f.getStartInd(), f.getSize());
		}
		assert(i >= f.getStartInd() && i < f.getEndInd());
		value = f(i);
		break;
	case ConvolveMode::sameDomainSymExt0:
		if (i < f.getStartInd() || i >= f.getEndInd()) {
			int tmp = mod(i - f.getStartInd(), 2 * f.getSize() - 2);
			i = f.getStartInd() + std::min(tmp, 2 * f.getSize() - 2 - tmp);
		}
		assert(i >= f.getStartInd() && i < f.getEndInd());
		value = f(i);
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
template <class T>
Sequence1<T> convolveFull(const Sequence1<T>& f, const Sequence1<T>& g)
{
	Sequence1<T> seq(f);
	Sequence1<T> filt(g);
	Sequence1<T> result(f.getStartInd() + g.getStartInd(),
	  f.getSize() + g.getSize() - 1);
	convolveHelper(seq.begin(), seq.end(), filt.begin(), filt.end(),
	  result.begin(), T());
	return result;
}
/// @endcond

/// @cond
template <class T>
Sequence1<T> convolveSameDomainZeroExt(const Sequence1<T>& f,
  const Sequence1<T>& g)
{
	Sequence1<T> result(f.getStartInd(), f.getSize());
	int skip;
	int shift;
	if (g.getStartInd() >= 0) {
		shift = g.getStartInd();
		skip = 0;
	} else {
		shift = 0;
		skip = -g.getStartInd();
	}
	convolveHelper2(f.begin(), f.end(), g.begin(), g.end(),
	  result.begin() + shift, skip, result.getSize() - shift, T());
	return result;
}
/// @endcond

/*!
 * @brief Compute the convolution of two sequences.
 *
 * Effects:
 * The convolution of the sequences f and g is computed.
 * The domain of the resulting sequence (as well as how boundaries are
 * handled) depends on the convolution mode mode.
 * The "full" mode is the same as the "full" mode in MATLAB.
 * The "sameDomainZeroExt" mode is the same as the "same" mode in MATLAB.
 *
 * Returns:
 * A sequence containing the convolution result is returned.
 */
template <class T>
Sequence1<T> convolve(const Sequence1<T>& f, const Sequence1<T>& g,
  int mode = ConvolveMode::full)
{
#if defined(SPL_SEQUENCE1_USE_NEW_CONV)
	switch (mode) {
	case ConvolveMode::sameDomainZeroExt:
		return convolveSameDomainZeroExt(f, g);
		break;
	case ConvolveMode::full:
		return convolveFull(f, g);
		break;
	default:
		assert(0);
		abort();
		break;
	}
#endif

	Sequence1<T> result;

	switch (mode) {
	case ConvolveMode::full:
		result = Sequence1<T>(f.getStartInd() + g.getStartInd(),
		  f.getSize() + g.getSize() - 1);
		break;
	case ConvolveMode::sameDomainZeroExt:
	case ConvolveMode::sameDomainConstExt:
	case ConvolveMode::sameDomainPerExt:
	case ConvolveMode::sameDomainSymExt0:
		result = Sequence1<T>(f.getStartInd(), f.getSize());
		break;
	default:
		assert(0);
		abort();
		break;
	}

	typename Sequence1<T>::Iterator resultIter = result.begin();
	for (int n = result.getStartInd(); n < result.getEndInd(); ++n) {
		T sum = T(0);
		for (int k = g.getStartInd(); k < g.getEndInd(); ++k) {
			sum += getExtSeqValue(f, n - k, mode) * g(k);
		}
		*resultIter = sum;
		++resultIter;
	}
	assert(resultIter == result.end());
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Downsampling and upsampling
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Downsample a sequence by the specified factor.
 *
 * Effects:
 * The sequence f is downsampled by the factor factor.
 *
 * Returns:
 * The downsampled sequence is returned.
 */

template <class T>
Sequence1<T> downsample(const Sequence1<T>& f, int factor)
{
	assert(factor >= 1);
	int startInd = ceilDiv(f.getStartInd(), factor) * factor;
	int n = ceilDiv(f.getEndInd() - startInd, factor);
	Sequence1<T> g(roundTowardZeroDiv(startInd, factor), n);
	typename Sequence1<T>::ConstIterator srcIter = f.begin() +
	  (startInd - f.getStartInd());
	typename Sequence1<T>::Iterator dstIter = g.begin();
	while (--n >= 0) {
		*dstIter = *srcIter;
		++dstIter;
		srcIter += factor;
	}
	return g;
}

/*!
 * @brief Upsample a sequence by the specified factor.
 *
 * Effects:
 * The sequence f is upsampled by the factor factor.
 * If pad is zero, new samples will only be added between the first and
 * last sample.
 * Up to (factor - 1) extra new samples can be added at the end of the new
 * sequence, by specifying a nonzero value for pad.
 * The default is no padding.
 *
 * Returns:
 * The upsampled sequence is returned.
 */

template <class T>
Sequence1<T> upsample(const Sequence1<T>& f, int factor, int pad = 0)
{
	assert(factor >= 1);
	assert(pad >= 0 && pad < factor);
	int n = f.getSize() + std::max((f.getSize() - 1), 0) * (factor - 1) + pad;
	int m = f.getSize();
	Sequence1<T> g(factor * f.getStartInd(), n);
	typename Sequence1<T>::ConstIterator srcIter = f.begin();
	typename Sequence1<T>::Iterator dstIter = g.begin();
	while (m > 1) {
		*dstIter = *srcIter;
		++dstIter;
		++srcIter;
		for (int i = factor - 1; i > 0; --i) {
			*dstIter = T(0);
			++dstIter;
		}
		--m;
	}
	if (m > 0) {
		*dstIter = *srcIter;
		++dstIter;
	}
	for (int i = pad; i > 0; --i) {
		*dstIter = T(0);
		++dstIter;
	}
	return g;
}

////////////////////////////////////////////////////////////////////////////////
// Polyphase Decompositions.
////////////////////////////////////////////////////////////////////////////////

/// @cond
/*!
 * @brief Get the coset offset for the specified type of polyphase
 * decomposition.
 *
 * DO NOT CALL THIS FUNCTION DIRECTLY!
 */

inline int getCosetOffset(int type, int numPhases, int index)
{
	int result;
	switch (type) {
	case 1:
		result = -index;
		break;
	case 2:
		result = index - (numPhases - 1);
		break;
	case 3:
		result = index;
		break;
	case 4:
		result = (numPhases - 1) - index;
		break;
	default:
		assert(0);
		abort();
		break;
	}
	return result;
}
/// @endcond

/*!
 * @brief Split a sequence into its polyphase components.
 *
 * Effects:
 * The polyphase decomposition of the sequence seq is computed.
 * In particular, the polyphase decomposition with numPhases phases
 * and type type is computed.
 *
 * Returns:
 * An array containing the polyphase components is returned.
 */

template <class T>
Array1<Sequence1<T> > polyphaseSplit(const Sequence1<T>& seq, int type,
  int numPhases)
{
	assert(numPhases >= 2);
	Array1<Sequence1<T> > result(numPhases);
	for (int i = 0; i < numPhases; ++i) {
		int offset = getCosetOffset(type, numPhases, i);
		result(i) = downsample(translate(seq, offset), numPhases);
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
Sequence1<T> polyphaseJoin(const Array1<Sequence1<T> >& comps, int type)
{
	const int numPhases = comps.getSize();
	assert(numPhases >= 2);
	Sequence1<T> seq;
	for (int i = 0; i < numPhases; ++i) {
		int offset = getCosetOffset(type, numPhases, i);
		seq = add(seq, translate(upsample(comps(i), numPhases), -offset));
	}
	return seq;
}

////////////////////////////////////////////////////////////////////////////////
// Types.
////////////////////////////////////////////////////////////////////////////////

//! Real sequence
typedef Sequence1<double> RealSequence1;

//! Integer sequence
typedef Sequence1<int> IntSequence1;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @}
 */

}

#endif
