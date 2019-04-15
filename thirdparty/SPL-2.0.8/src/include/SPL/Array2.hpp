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
 * @file Array2.hpp
 * @brief This file contains the Array2 template class and its supporting code.
 */

#ifndef SPL_Array2_hpp
#define SPL_Array2_hpp

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

//! Defining this symbol will enable extra code for debugging.
//#define SPL_ARRAY2_DEBUG

#if defined(SPL_ARRAY2_DEBUG)
//! Prevent the inlining of functions.
#define SPL_ARRAY2_INLINE
#else
//! Allow the inlining of functions.
#define SPL_ARRAY2_INLINE inline
#endif

////////////////////////////////////////////////////////////////////////////////
// Header Files.
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <boost/iterator/iterator_facade.hpp>
#include <SPL/pnmCodec.hpp>
#include <SPL/misc.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// Two-Dimensional Array Template Class
// (with lazy copying and reference counting)
////////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup Array2
 * @{
 */

template <class> class Array2;

/// @cond

/*
 * An array data block, which can be shared by multiple arrays.
 */

template <class T>
class ArrayRep2
{
private:
	template <class> friend class Array2;

	ArrayRep2(int width, int height);

	ArrayRep2(int width, int height, const T& value);

	template <class InputIterator>
	ArrayRep2(int width, int height, InputIterator data);

	~ArrayRep2();

	// Prevent copy construction.
	// This function is never defined.
	ArrayRep2(const ArrayRep2&);

	// Prevent assignment.
	// This function is never defined.
	ArrayRep2& operator=(const ArrayRep2&);

	// Create a copy of this data block.
	ArrayRep2* clone() const;

	/*!
	 * Get the reference count for the underlying data.
	 */
	int getRefCount() const;

	/*!
	 * Increment the reference count for the underlying data.
	 */
	void hold();

	/*!
	 * Decrement the reference count for the underlying data, dellocating
	 * the underlying data when the reference count reaches zero.
	 */
	void release();

	// Output information to a stream for debugging.
	void dump(std::ostream& out) const;

	// The width of the array.
	int width_;

	// The height of the array.
	int height_;

	// The array data.
	std::vector<T> data_;

	// The number of arrays referencing the array data.
	int refCount_;
};

/// @endcond

/*!
 * @brief
 * A two-dimensional array class with lazy copying and reference counting.
 */

template <class T>
class Array2
{
public:

	////////////////////////////////////////////////////////////
	// Types
	////////////////////////////////////////////////////////////

	//! The type of the elements in the array.
	typedef T ElemType;

	//! @cond
	// Provide a random access iterator for the elements of an array.
	// NOTE: It would be more elegant to have this class use
	// vector iterators instead of raw pointers.
	template <class Value>
	class YIter : public boost::iterator_facade<YIter<Value>, Value,
	  std::random_access_iterator_tag>
	{
	public:
		YIter() : step_(0), ptr_(0)
		{
		}
		explicit YIter(int step, Value* ptr) : step_(step), ptr_(ptr)
		{
		}
		template <class OtherValue>
		YIter(const YIter<OtherValue>& other) : step_(other.step_),
		  ptr_(other.ptr_)
		{
		}
	private:
		friend class boost::iterator_core_access;
		template <class> friend class YIter;
		template <class OtherValue>
		bool equal(const YIter<OtherValue>& other) const
		{
			return ptr_ == other.ptr_;
		}
		void increment()
		{
			ptr_ += step_;
		}
		void decrement()
		{
			ptr_ -= step_;
		}
		void advance(int n)
		{
			ptr_ += n * step_;
		}
		template <class OtherValue>
		int distance_to(const YIter<OtherValue>& other) const
		{
			return (other.ptr_ - ptr_) / step_;
		}
		Value& dereference() const {
			return *ptr_;
		}
		int step_;
		Value* ptr_;
	};
	//! @endcond

	//! A mutable iterator for all elements in the array.
	typedef typename std::vector<T>::iterator Iterator;

	//! A constant iterator for all elements in the array.
	typedef typename std::vector<T>::const_iterator ConstIterator;

	//! A mutable iterator for elements of a row in the array.
	typedef Iterator XIterator;

	//! A constant iterator for elements of a row in the array.
	typedef typename std::vector<T>::const_iterator ConstXIterator;

	//! A mutable iterator for elements of a column in the array.
	typedef YIter<T> YIterator;

	//! A constant iterator for elements of a column in the array.
	typedef YIter<const T> ConstYIterator;

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Create an empty array.
	 */
	Array2();

	/*!
	 * @brief Create an array of the specified width and height.
	 */
	Array2(int width, int height);

	/*!
	 * @brief Create an array of the specified width and height with the
	 * elements of the array initialized to the specified value.
	 */
	Array2(int width, int height, const T& value);

	/*!
	 * @brief Create an array of the specified width and height with the
	 * elements of the array initialized to the specified data.
	 */
	template <class InputIter>
	  Array2(int width, int height, InputIter data);

	/*!
	 * @brief The destructor.
	 */
	~Array2();

	/*!
	 * @brief The copy constructor.
	 */
	Array2(const Array2& a);

	/*!
	 * @brief Create an array from an array having elements of a different
	 * type.
	 */
	template <class OtherType>
	Array2(const Array2<OtherType>& a);

	////////////////////////////////////////////////////////////
	// Assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief The assignment operator.
	 */
	Array2& operator=(const Array2& a);

	/*!
	 * @brief Assign another array with elements of a different type
	 * to this array.
	 */
	template <class OtherType>
	Array2& operator=(const Array2<OtherType>& a);

	////////////////////////////////////////////////////////////
	// Compound assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Add another array (elementwise) to this array.
	 */
	Array2& operator+=(const Array2& a);

	/*!
	 * @brief Subtract another array (elementwise) from this array.
	 */
	Array2& operator-=(const Array2& a);

	/*!
	 * @brief Multiply another array (elementwise) by this array.
	 */
	Array2& operator*=(const Array2& a);

	/*!
	 * @brief Divide this array (elementwise) by another array.
	 */
	Array2& operator/=(const Array2& a);

	/*!
	 * @brief Add the specified value to each element in the array.
	 */
	Array2& operator+=(const T& a);

	/*!
	 * @brief Subtract the specified value from each element in the array.
	 */
	Array2& operator-=(const T& a);

	/*!
	 * @brief Multiply each element in the array by the specified value.
	 */
	Array2& operator*=(const T& a);

	/*!
	 * @brief Divide each element in the array by the specified value.
	 */
	Array2& operator/=(const T& a);

	////////////////////////////////////////////////////////////
	// Basic queries
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get the width of the array.
	 */
	int getWidth() const;

	/*!
	 * @brief Get the height of the array.
	 */
	int getHeight() const;

	/*!
	 * @brief Get the number of elements in the array.
	 */
	int getSize() const;

	/*!
	 * @brief
	 * Is the data for this array shared with another array?
	 *
	 * Under most normal circumstances, one should never need to call this
	 * function.  In some instances, however, it might be necessary to
	 * know whether data is shared in order to write more optimal code.
	 */
	bool isShared() const;

	/*!
	 * @brief
	 * Is the data for this array shared with the specified array?
	 */
	bool isSharedWith(const Array2& a) const;

	////////////////////////////////////////////////////////////
	// Accessors
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a mutable reference to the (x,y)-th element in the array.
	 */
	T& operator()(int x, int y);

	/*!
	 * @brief Get a const reference to the (x,y)-th element in the array.
	 */
	const T& operator()(int x, int y) const;

	/*!
	 * @brief Get a mutable reference to the i-th element in the array.
	 *
	 * The array must have either a width or height of one.
	 */
	T& operator()(int i);

	/*!
	 * @brief Get a const reference to the i-th element in the array.
	 *
	 * The array must have either a width or height of one.
	 */
	const T& operator()(int i) const;

	////////////////////////////////////////////////////////////
	// Iterators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a const iterator for the first element in the array.
	 */
	ConstIterator begin() const;

	/*!
	 * @brief Get a mutable iterator for the first element in the array.
	 */
	Iterator begin();

	/*!
	 * @brief Get a const iterator for one past the last element in the
	 * array.
	 */
	ConstIterator end() const;

	/*!
	 * @brief Get a mutable iterator for one past the last element in the
	 * array.
	 */
	Iterator end();

	/*!
	 * @brief
	 * Get a const iterator for the first element in the specified row of
	 * the array.
	 */
	ConstXIterator rowBegin(int y) const;

	/*!
	 * @brief
	 * Get a mutable iterator for the first element in the specified row of
	 * the array.
	 */
	XIterator rowBegin(int y);

	/*!
	 * @brief
	 * Get a const iterator for one past the end in the specified row of
	 * the array.
	 */
	ConstXIterator rowEnd(int y) const;

	/*!
	 * @brief
	 * Get a mutable iterator for one past the end in the specified row of
	 * the array.
	 */
	XIterator rowEnd(int y);

	/*!
	 * @brief
	 * Get a const iterator for the first element in the specified column of
	 * the array.
	 */
	ConstYIterator colBegin(int x) const;

	/*!
	 * @brief
	 * Get a mutable iterator for the first element in the specified column of
	 * the array.
	 */
	YIterator colBegin(int x);

	/*!
	 * @brief
	 * Get a const iterator for one past the end in the specified column of
	 * the array.
	 */
	ConstYIterator colEnd(int x) const;

	/*!
	 * @brief
	 * Get a mutable iterator for one past the end in the specified column of
	 * the array.
	 */
	YIterator colEnd(int x);

	////////////////////////////////////////////////////////////
	// Array resizing
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Change the size of the array.
	 *
	 * Effects:
	 * The array size is changed to the specified size.
	 * If the new size is the same as the old size, this function does
	 * nothing.
	 */
	void resize(int width, int height);

	/*!
	 * @brief Change the size of the array, initializing the resized
	 * array with the data obtained from the specified input iterator.
	 */
	template <class InputIterator>
	void resize(int width, int height, InputIterator data);

	////////////////////////////////////////////////////////////
	// Get various statistics
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get the maximum of the elements in the array.
	 *
	 * The array must contain at least one element.
	 */
	T max() const;

	/*!
	 * @brief Get the minimum of the elements in the array.
	 *
	 * The array must contain at least one element.
	 */
	T min() const;

	/*!
	 * @brief Get the sum of the elements in the array.
	 */
	T sum() const;

	////////////////////////////////////////////////////////////
	// Input/output
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Output an array to a stream using the specified field width
	 * for each array element.
	 */
	std::ostream& output(std::ostream& out, int fieldWidth) const;

	/*!
	 * @brief Load an array from the file with the specified name.
	 */
	int load(const char* fileName);

	/*!
	 * @brief Save an array to the file with the specified name.
	 */
	int save(const char* fileName) const;

	////////////////////////////////////////////////////////////
	// Miscellaneous
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Set all elements in the array to the specified value.
	 */
	void fill(const T& value = T(0));

	/*!
	 * @brief Flip the array upside down.
	 */
	Array2& flipud();

	/*!
	 * @brief Flip the array left to right.
	 */
	Array2& fliplr();

	/*!
	 * @brief Swap the array data with the data of the specified array.
	 */
	void swap(Array2& a);

	/*!
	 * @brief Output information about an array to a stream for debugging.
	 */
	void dump(std::ostream& out) const;

	/*!
	 * @brief Force the underlying data to be copied if the data is shared.
	 */
	void unshare() const;

private:

	template <class> friend class Array2;

	//! The type for representing the underlying array data.
	typedef ArrayRep2<T> Rep;

	/*!
	 * @brief Force the underlying data to be copied.
	 * The data is assumed to be shared.
	 */
	void copyOnWrite() const;

	//! The underlying data.
	mutable Rep* ptr_;
};

////////////////////////////////////////////////////////////////////////////////
// Code for ArrayRep2 class.
////////////////////////////////////////////////////////////////////////////////

/// @cond

template <class T>
SPL_ARRAY2_INLINE ArrayRep2<T>::ArrayRep2(int width, int height) :
  width_(width), height_(height), data_(width * height), refCount_(1)
{
	assert(width >= 0 && height >= 0);
}

template <class T>
SPL_ARRAY2_INLINE ArrayRep2<T>::ArrayRep2(int width, int height,
  const T& value) : width_(width), height_(height),
  data_(width * height, value), refCount_(1)
{
	assert(width >= 0 && height >= 0);
}

template <class T>
template <class InputIterator>
SPL_ARRAY2_INLINE ArrayRep2<T>::ArrayRep2(int width, int height,
  InputIterator data) : width_(width), height_(height), data_(), refCount_(1)
{
	assert(width >= 0 && height >= 0);
	int n = width * height;
	data_.reserve(n);
	SPL::copy_n(data, n, std::back_inserter(data_));
}

template <class T>
SPL_ARRAY2_INLINE ArrayRep2<T>::~ArrayRep2()
{
	// The destructor should only be called if nothing is referencing
	// this object.
	assert(!refCount_);
}

template <class T>
SPL_ARRAY2_INLINE ArrayRep2<T>* ArrayRep2<T>::clone() const
{
	return new ArrayRep2(width_, height_, data_.begin());
}

template <class T>
SPL_ARRAY2_INLINE void ArrayRep2<T>::hold()
{
	++refCount_;
}

template <class T>
SPL_ARRAY2_INLINE void ArrayRep2<T>::release()
{
	if (--refCount_ == 0) {
		delete this;
	}
}

template <class T>
SPL_ARRAY2_INLINE int ArrayRep2<T>::getRefCount() const
{
	return refCount_;
}

template <class T>
void ArrayRep2<T>::dump(typename std::ostream& out) const
{
	out << "Rep: "
	  << this << " "
	  << width_ << " " << height_ << " "
	  << (&*data_.begin()) << " "
	  << refCount_ << " ";
}

/// @endcond

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE Array2<T>::Array2()
{
	ptr_ = new Rep(0, 0);
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>::Array2(int width, int height)
{
	assert(width >= 0 && height >= 0);
	ptr_ = new Rep(width, height);
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>::Array2(const Array2<T>& a)
{
#if defined(SPL_ARRAY2_DEBUG)
	std::cerr << "Array2<T>::Array2(const Array2<T>&) "
	  << this << " " << (&a) << "\n";
#endif
	ptr_ = a.ptr_;
	ptr_->hold();
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>::Array2(int width, int height, const T& value)
{
	assert(width >= 0 && height >= 0);
	ptr_ = new Rep(width, height, value);
}

template <class T>
template <class InputIterator>
SPL_ARRAY2_INLINE Array2<T>::Array2(int width, int height, InputIterator data)
{
	assert(width >= 0 && height >= 0);
	ptr_ = new Rep(width, height, data);
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>::~Array2()
{
#if defined(SPL_ARRAY2_DEBUG)
		std::cerr << "Array2<T>::~Array2() " << this << " " << ptr_ << "\n";
#endif
	ptr_->release();
}

template <class T>
template <class OtherType>
SPL_ARRAY2_INLINE Array2<T>::Array2(const Array2<OtherType>& a)
{
#if defined(SPL_ARRAY2_DEBUG)
		std::cerr << "Array2::pseudo_copy_ctor " << this << " " << ptr_ << "\n";
#endif
	ptr_ = new Rep(a.getWidth(), a.getHeight(), a.begin());
}

////////////////////////////////////////////////////////////////////////////////
// Assignment operators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator=(const Array2<T>& a)
{
#if defined(SPL_ARRAY2_DEBUG)
	std::cerr << "Array2<T>& Array2<T>::operator=(const Array2<T>&) "
	  << this << " " << (&a) << "\n";
#endif
	if (this != &a) {
		// Not self assignment.
		ptr_->release();
		ptr_ = a.ptr_;
		ptr_->hold();
	}
	return *this;
}

template <class T>
template <class OtherType>
Array2<T>& Array2<T>::operator=(const Array2<OtherType>& a)
{
#if defined(SPL_ARRAY2_DEBUG)
	std::cerr << "Array2::operator= special\n";
#endif
	if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&a)) {
		resize(a.getWidth(), a.getHeight());
		unshare();
		std::copy(a.ptr_->data_.begin(), a.ptr_->data_.end(),
		  ptr_->data_.begin());
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Compound assignment operators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator+=(const Array2<T>& a)
{
	assert(getWidth() == a.getWidth() && getHeight() == a.getHeight());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::plus<T>());
	return *this;
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator-=(const Array2<T>& a)
{
	assert(getWidth() == a.getWidth() && getHeight() == a.getHeight());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::minus<T>());
	return *this;
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator*=(const Array2<T>& a)
{
	assert(getWidth() == a.getWidth() && getHeight() == a.getHeight());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::multiplies<T>());
	return *this;
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator/=(const Array2<T>& a)
{
	assert(getWidth() == a.getWidth() && getHeight() == a.getHeight());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::divides<T>());
	return *this;
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator+=(const T& value)
{
	unshare();
#if 0
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), std::bind2nd(std::plus<T>(), value));
#else
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), [=](auto x){return x + value;});
#endif
	return *this;
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator-=(const T& value)
{
	unshare();
#if 0
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), std::bind2nd(std::minus<T>(), value));
#else
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), [=](auto x){return x - value;});
#endif
	return *this;
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator*=(const T& value)
{
	unshare();
#if 0
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), std::bind2nd(std::multiplies<T>(), value));
#else
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), [=](auto x){return x * value;});
#endif
	return *this;
}

template <class T>
SPL_ARRAY2_INLINE Array2<T>& Array2<T>::operator/=(const T& value)
{
	unshare();
#if 0
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), std::bind2nd(std::divides<T>(), value));
#else
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  ptr_->data_.begin(), [=](auto x){return x / value;});
#endif
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Basic queries
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE int Array2<T>::getWidth() const
{
	return ptr_->width_;
}

template <class T>
SPL_ARRAY2_INLINE int Array2<T>::getHeight() const
{
	return ptr_->height_;
}

template <class T>
SPL_ARRAY2_INLINE int Array2<T>::getSize() const
{
	assert(ptr_->width_ * ptr_->height_ == ptr_->data_.size());
	return ptr_->data_.size();
}

template <class T>
SPL_ARRAY2_INLINE bool Array2<T>::isShared() const
{
	return ptr_->getRefCount() > 1;
}

template <class T>
SPL_ARRAY2_INLINE bool Array2<T>::isSharedWith(const Array2& a) const
{
	return ptr_ == a.ptr_;
}

////////////////////////////////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE T& Array2<T>::operator()(int x, int y)
{
	assert(x >= 0 && x < getWidth() && y >= 0 && y < getHeight());
	unshare();
	return ptr_->data_[y * getWidth() + x];
}

template <class T>
SPL_ARRAY2_INLINE const T& Array2<T>::operator()(int x, int y) const
{
	assert(x >= 0 && x < getWidth() && y >= 0 && y < getHeight());
	return ptr_->data_[y * getWidth() + x];
}

template <class T>
SPL_ARRAY2_INLINE T& Array2<T>::operator()(int i)
{
	assert(getWidth() == 1 || getHeight() == 1);
	assert(i >= 0 && i < getSize());
	unshare();
	return ptr_->data_[i];
}

template <class T>
SPL_ARRAY2_INLINE const T& Array2<T>::operator()(int i) const
{
	assert(getWidth() == 1 || getHeight() == 1);
	assert(i >= 0 && i < getSize());
	return ptr_->data_[i];
}

////////////////////////////////////////////////////////////////////////////////
// Iterators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::ConstIterator Array2<T>::begin() const
{
	return ptr_->data_.begin();
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::Iterator Array2<T>::begin()
{
	unshare();
	return ptr_->data_.begin();
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::ConstIterator Array2<T>::end() const
{
	return ptr_->data_.end();
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::Iterator Array2<T>::end()
{
	unshare();
	return ptr_->data_.end();
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::ConstXIterator Array2<T>::rowBegin(int y) const
{
	assert(y >= 0 && y < getHeight());
	return begin() + (y * getWidth());
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::XIterator Array2<T>::rowBegin(int y)
{
	assert(y >= 0 && y < getHeight());
	unshare();
	return begin() + (y * getWidth());
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::ConstXIterator Array2<T>::rowEnd(int y) const
{
	assert(y >= 0 && y < getHeight());
	return begin() + ((y + 1) * getWidth());
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::XIterator Array2<T>::rowEnd(int y)
{
	assert(y >= 0 && y < getHeight());
	unshare();
	return begin() + ((y + 1) * getWidth());
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::ConstYIterator Array2<T>::colBegin(int x) const
{
	assert(x >= 0 && x < getWidth());
	return ConstYIterator(getWidth(), &(*(begin() + x)));
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::YIterator Array2<T>::colBegin(int x)
{
	assert(x >= 0 && x < getWidth());
	unshare();
	return YIterator(getWidth(), &(*(begin() + x)));
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::ConstYIterator Array2<T>::colEnd(int x) const
{
	assert(x >= 0 && x < getWidth());
	return ConstYIterator(getWidth(), &(*(begin() + (getSize() + x))));
}

template <class T>
SPL_ARRAY2_INLINE typename Array2<T>::YIterator Array2<T>::colEnd(int x)
{
	assert(x >= 0 && x < getWidth());
	unshare();
	return YIterator(getWidth(), &(*(begin() + (getSize() + x))));
}

////////////////////////////////////////////////////////////////////////////////
// Resizing
////////////////////////////////////////////////////////////////////////////////

template <class T>
void Array2<T>::resize(int width, int height)
{
	assert(width >= 0 && height >= 0);
	if (getWidth() != width || getHeight() != height) {
		ptr_->release();
		ptr_ = new Rep(width, height);
	}
}

template <class T>
template <class InputIterator>
void Array2<T>::resize(int width, int height, InputIterator data)
{
	assert(width >= 0 && height >= 0);
	if (getWidth() == width && getHeight() == height && ptr_->getRefCount() == 1) {
		SPL::copy_n(data, getSize(), begin());
	} else {
		ptr_->release();
		ptr_ = new Rep(width, height, data);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Get various statistics
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE T Array2<T>::max() const
{
	assert(getSize() > 0);
	return *std::max_element(begin(), end());
}

template <class T>
SPL_ARRAY2_INLINE T Array2<T>::min() const
{
	assert(getSize() > 0);
	return *std::min_element(begin(), end());
}

template <class T>
SPL_ARRAY2_INLINE T Array2<T>::sum() const
{
	return std::accumulate(begin(), end(), T(0));
}

////////////////////////////////////////////////////////////////////////////////
// Input/output
////////////////////////////////////////////////////////////////////////////////

template <class T>
std::ostream& Array2<T>::output(std::ostream& out, int fieldWidth) const
{
	out << getWidth() << " " << getHeight() << "\n";
	for (int j = 0; j < getHeight(); ++j) {
		for (int i = 0; i < getWidth(); ++i) {
			T val = operator()(i, j);
			std::stringstream str;
			str << std::setw(fieldWidth) << val;
			std::string buf = str.str();
			if (buf.size() > fieldWidth) {
				buf = buf.substr(0, fieldWidth);
			}
			//out << std::setw(fieldWidth) << val;
			out << buf;
			if (i < getWidth() - 1) {
				out << " ";
			}
		}
		out << "\n";
	}
	return out;
}

template<class T>
int Array2<T>::load(const char* fileName)
{
	std::ifstream in(fileName);
	in >> *this;
	if (!in) {
		return -1;
	}
	return 0;
}

template<class T>
int Array2<T>::save(const char* fileName) const
{
	std::ofstream out(fileName);
	out << *this;
	if (!out) {
		return -1;
	}
	out.close();
	return 0;
}

/*!
 * @brief Output an array to the specified stream.
 */
template <class T>
std::ostream& operator<<(std::ostream& out, const Array2<T>& a)
{
	out << a.getWidth() << " " << a.getHeight() << "\n";
	for (int y = 0; y < a.getHeight(); ++y) {
		typename Array2<T>::ConstXIterator src = a.rowBegin(y);
		for (int x = 0; x < a.getWidth(); ++x) {
			if (x) {
				out << " ";
			}
			out << *src;
			++src;
		}
		out << "\n";
	}
	return out;
}

/*!
 * @brief Input an array from the specified stream.
 */
template <class T>
std::istream& operator>>(std::istream& in, Array2<T>& a)
{
	int width;
	if (!(in >> width)) {
		return in;
	}
	int height;
	if (!(in >> height)) {
		return in;
	}
	if (width < 0 || height < 0) {
		in.setstate(std::ios::failbit);
		return in;
	}

	a = Array2<T>(width, height);
	for (int y = 0; y < a.getHeight(); ++y) {
		typename Array2<T>::XIterator dst = a.rowBegin(y);
		for (int x = 0; x < a.getWidth(); ++x) {
			T value;
			if (!(in >> value)) {
				return in;
			}
			*dst = value;
			++dst;
		}
	}
	return in;
}

////////////////////////////////////////////////////////////////////////////////
// Miscellaneous
////////////////////////////////////////////////////////////////////////////////

template <class T>
void Array2<T>::swap(Array2<T>& a)
{
	if (this != &a) {
		// Overall, the reference counts do not change.
		// Technically, both reference counts increase and then decrease,
		// for no net change.
		std::swap(ptr_, a.ptr_);
	}
}

template <class T>
SPL_ARRAY2_INLINE void Array2<T>::fill(const T& value)
{
	unshare();
	std::fill(begin(), end(), value);
}

template <class T>
Array2<T>& Array2<T>::flipud()
{
	unshare();
	for (int k = 0; k < getHeight() / 2; ++k) {
		XIterator i = rowBegin(k);
		XIterator j = rowBegin(getHeight() - 1 - k);
		for (int n = getWidth(); n > 0; --n) {
			std::swap(*i, *j);
			++i;
			++j;
		}
	}
	return *this;
}

template <class T>
Array2<T>& Array2<T>::fliplr()
{
	unshare();
	for (int y = 0; y < getHeight(); ++y) {
		XIterator i = rowBegin(y);
		XIterator j = i + (getWidth() - 1);
		for (int n = getWidth() / 2; n > 0; --n) {
			std::swap(*i, *j);
			++i;
			--j;
		}
	}
	return *this;
}

/*!
 * @brief Get the transpose of the array.
 */
template <class T>
Array2<T> transpose(const Array2<T>& a)
{
	int width = a.getWidth();
	int height = a.getHeight();
	Array2<T> result(height, width);
	for (int y = 0; y < width; ++y) {
		for (int x = 0; x < height; ++x) {
			result(x, y) = a(y, x);
		}
	}
	return result;
}

/*!
 * @brief Test two arrays for equality.
 */
template <class T>
bool operator==(const Array2<T>& a, const Array2<T>& b)
{
	bool result;
	if (a.getWidth() == b.getWidth() && a.getHeight() == b.getHeight()) {
		result = true;
		for (int y = 0; y < a.getHeight(); ++y) {
			typename Array2<T>::ConstXIterator src = a.rowBegin(y);
			typename Array2<T>::ConstXIterator dst = b.rowBegin(y);
			for (int x = 0; x < a.getWidth(); ++x) {
				if (*src != *dst) {
					result = false;
					y = a.getHeight();
					break;
				}
				++src;
				++dst;
			}
		}
	} else {
		result = false;
	}
	return result;
}

/*!
 * @brief Test two arrays for inequality.
 */
template <class T>
bool operator!=(const Array2<T>& a, const Array2<T>& b)
{
	return !(a == b);
}

/*!
 * @brief Ouput information about an array to the specified stream for
 * debugging purposes.
 */
template <class T>
void Array2<T>::dump(std::ostream& out) const
{
	out << "Array2<T> " << this << " ";
	ptr_->dump(out);
	out << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// Private code
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY2_INLINE void Array2<T>::copyOnWrite() const
{
#if defined(SPL_ARRAY2_DEBUG)
	std::cerr << "Array2<T>::copyOnWrite() " << this << "\n";
	dump(std::cerr);
#endif
	// The underlying data must be shared.
	assert(ptr_->getRefCount() > 1);
	ptr_->release();
	//const_cast<Array2*>(this)->ptr_ = ptr_->clone();
	this->ptr_ = ptr_->clone();
}

template <class T>
SPL_ARRAY2_INLINE void Array2<T>::unshare() const
{
	if (ptr_->getRefCount() > 1) {
		copyOnWrite();
	}
}

/**
 * @}
 */

////////////////////////////////////////////////////////////////////////////////
// PNM Support
////////////////////////////////////////////////////////////////////////////////

/*!
 * @addtogroup Image_IO
 * @{
 */

/// @cond

template <class T>
class PnmPutData
{
public:

	typedef Array2<T> Array;
	typedef typename Array::XIterator ArrayIter;

	PnmPutData() : comps_(0)
	{
	}

	void initialize(typename std::vector<Array>& comps)
	{
		comps_ = &comps;
		assert(comps.size() >= 0);
		dataIters_.reserve(3);
		y_ = (*comps_)[0].getHeight() - 1;
		remaining_ = (*comps_)[0].getWidth();
		for (typename std::vector<Array>::iterator i = comps_->begin();
		  i != comps_->end(); ++i) {
			assert(!i->isShared());
			dataIters_.push_back(i->rowBegin(y_));
		}
		comp_ = 0;
	}

	void operator()(int value)
	{
		assert(y_ >= 0);
		ArrayIter& iter = dataIters_[comp_];
//std::cerr << "writing " << y_ << " " << remaining_ << " " << value << "\n";
		*iter = value;
		++iter;
		++comp_;

		if (comp_ >= comps_->size()) {
			comp_ = 0;
			--remaining_;
			if (remaining_ <= 0) {
				remaining_ = (*comps_)[0].getWidth();
				--y_;
				if (y_ >= 0) {
					typename std::vector<Array>::iterator comp =
					  (*comps_).begin();
					for (typename std::vector<ArrayIter>::iterator i =
					  dataIters_.begin(); i != dataIters_.end(); ++i) {
						*i = comp->rowBegin(y_);
						++comp;
					}
				}
			}
		}
	}
private:
	std::vector<Array>* comps_;
	typename std::vector<ArrayIter> dataIters_;
	int y_;
	int remaining_;
	int comp_;
};

template <class T>
class PnmInitialize
{
public:
	typedef PnmPutData<T> PutData;
	void operator()(int width, int height, int numComps, int maxVal,
	  bool sgnd, PutData& putData) {
		comps_.clear();
		for (int i = 0; i < numComps; ++i) {
			comps_.push_back(Array2<T>(width, height));
		}
		putData.initialize(comps_);
		maxVal_ = maxVal;
		sgnd_ = sgnd;
	}
	typename std::vector<Array2<T> > comps_;
	int maxVal_;
	bool sgnd_;
};

template <class T>
class PnmGetData
{
public:

	typedef Array2<T> Array;
	typedef typename Array::ConstXIterator ConstArrayIter;

	PnmGetData(const typename std::vector<Array>& comps, int maxVal,
	  bool sgnd) : comps_(comps)
	{
		assert(comps.size() >= 0);
		dataIters_.reserve(3);
		int width = comps[0].getWidth();
		int height = comps[0].getHeight();
		y_ = height - 1;
		remaining_ = width;
		for (typename std::vector<Array>::const_iterator i = comps.begin();
		  i != comps.end(); ++i) {
			dataIters_.push_back(i->rowBegin(y_));
		}
		comp_ = 0;
	}

	int operator()()
	{
		assert(y_ >= 0);
		ConstArrayIter& iter = dataIters_[comp_];
		int value = *iter;
		++iter;
		++comp_;

		if (comp_ >= comps_.size()) {
			comp_ = 0;
			--remaining_;
			if (remaining_ <= 0) {
				remaining_ = comps_[0].getWidth();
				--y_;
				if (y_ >= 0) {
					typename std::vector<Array>::const_iterator comp =
					  comps_.begin();
					for (typename std::vector<ConstArrayIter>::iterator i =
					  dataIters_.begin(); i != dataIters_.end(); ++i) {
						*i = comp->rowBegin(y_);
						++comp;
					}
				}
			}
		}

		return value;
	}

private:
	const std::vector<Array>& comps_;
	typename std::vector<ConstArrayIter> dataIters_;
	int y_;
	int remaining_;
	int comp_;
};

/// @endcond

/*!
 * @brief Output the array as an image in the PNM format.
 *
 * Effects:
 * The image components comps are written to the stream outStream in
 * the PNM format, where the red, green, and blue color componenets
 * are given by comps[0], comps[1], and comps[2], respectively.
 * If the parameter binaryFormat is true, the binary variant of the
 * PNM format is used; otherwise, the text variant is used.
 * The parameter maxVal specifies the maximum value for sample data.
 * The parameter sgnd specified if the sample data is signed.
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int encodePnm(std::ostream& outStream, const std::vector<Array2<T> >& comps,
  int maxVal, bool sgnd, bool binaryFormat = true)
{
	PnmGetData<T> getData(comps, maxVal, sgnd);
	return pnmEncode(outStream, comps[0].getWidth(), comps[0].getHeight(),
	  comps.size(), maxVal, sgnd, getData, binaryFormat);
}

/*!
 * @brief Output the array as an image in the PNM format (PBM type).
 *
 * Effects:
 * The binary image in the array bits is written to the stream outStream.
 * If the parameter binaryFormat is true, the binary variant of the PBM
 * format is used; otherwise, the text variant is employed.
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int encodePbm(std::ostream& outStream, const Array2<T>& bits, bool binaryFormat = true)
{
	return encodePnm(outStream, std::vector<Array2<T> >(1, bits), 1, binaryFormat);
}

/*!
 * @brief Output the array as an image in the PNM format (PGM type).
 *
 * Effects:
 * The grayscale image in the array gray is written to the stream outStream.
 * If the parameter binaryFormat is true, the binary variant of the PBM
 * format is used; otherwise, the text variant is employed.
 * The parameter maxVal specifies the maximum value for sample data.
 * The parameter sgnd specified if the sample data is signed.
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int encodePgm(std::ostream& outStream, const Array2<T>& gray, int maxVal,
  bool sgnd, bool binaryFormat = true)
{
	return encodePnm(outStream, std::vector<Array2<T> >(1, gray), maxVal,
	  sgnd, binaryFormat);
}

/*!
 * @brief Output the array as an image in the PNM format (PPM type).
 *
 * Effects:
 * The color image with RGB color planes in the arrays red, green, and blue,
 * respectively, is written to the stream outStream.
 * If the parameter binaryFormat is true, the binary variant of the PBM
 * format is used; otherwise, the text variant is employed.
 * The parameter maxVal specifies the maximum value for sample data.
 * The parameter sgnd specified if the sample data is signed.
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int encodePpm(std::ostream& outStream, const Array2<T>& red,
  const Array2<T>& green, const Array2<T>& blue, int maxVal, bool sgnd,
  bool binaryFormat = true)
{
	std::vector<Array2<T> > comps;
	comps.push_back(red);
	comps.push_back(green);
	comps.push_back(blue);
	return encodePnm(outStream, comps, maxVal, sgnd, binaryFormat);
}

/*!
 * @brief Input an array as an image in the PNM format.
 *
 * Effects:
 * An image in the PNM format is read from the stream inStream.
 * The parameter comps is updated to hold the components of the image read,
 * where the red, green, and blue color components are placed in
 * comps[0], comps[1], and comps[2], respectively.
 * The parameter maxVal is set to the maximum value for the image sample data.
 * The parameter sgnd is set to indicate whether the image data is signed.
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int decodePnm(std::istream& inStream, std::vector<Array2<T> >& comps,
  int& maxVal, bool& sgnd)
{
	PnmInitialize<T> initialize;
	int ret;
	if ((ret = pnmDecode(inStream, initialize))) {
		return ret;
	}
	comps = initialize.comps_;
	maxVal = initialize.maxVal_;
	sgnd = initialize.sgnd_;
	return 0;
}

/*!
 * @brief Input an array as an image in the PNM format.
 *
 * Effects:
 * A binary image in the PBM format is read from the stream inStream.
 * The parameter bits is set to the image read.
 * The image to be read must be of the PBM type (i.e., binary).
 *
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int decodePbm(std::istream& inStream, Array2<T>& bits)
{
	int ret;
	int maxVal;
	bool sgnd;
	std::vector<Array2<T> > comps;
	if ((ret = decodePnm(inStream, comps, maxVal, sgnd))) {
		return ret;
	}
	assert(maxVal == 1);
	assert(!sgnd);
	bits = comps[0];
	return 0;
}

/*!
 * @brief Input an array as an image in the PNM format.
 *
 * Effects:
 * A grayscale image in the PGM format is read from the stream inStream.
 * The parameter gray is updated to hold the image read.
 * The parameter maxVal is set to the maximum value for the image sample data.
 * The parameter sgnd is set to indicate whether the image data is signed.
 * The image to be read must be of the PGM type (i.e., grayscale).
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int decodePgm(std::istream& inStream, Array2<T>& gray, int& maxVal, bool& sgnd)
{
	int ret;
	std::vector<Array2<T> > comps;
	if ((ret = decodePnm(inStream, comps, maxVal, sgnd))) {
		return ret;
	}
	gray = comps[0];
	return 0;
}

/*!
 * @brief Input an array as an image in the PNM format.
 *
 * Effects:
 * A color image in the PPM format is read from the stream inStream.
 * The parameters red, green, and blue are set to RGB components of the
 * image read.
 * The parameter maxVal is set to the maximum value for the image sample data.
 * The parameter sgnd is set to indicate whether the image data is signed.
 * The image to be read must be of the PPM type (i.e., RGB color).
 *
 * Returns:
 * Upon success, zero is returned; otherwise, a nonzero value is returned.
 */
template <class T>
int decodePpm(std::istream& inStream, Array2<T>& red, Array2<T>& green,
  Array2<T>& blue, int& maxVal, bool& sgnd)
{
	int ret;
	std::vector<Array2<T> > comps;
	if ((ret = decodePnm(inStream, comps, maxVal, sgnd))) {
		return ret;
	}
	red = comps[0];
	green = comps[1];
	blue = comps[2];
	return 0;
}

/**
 * @}
 */

////////////////////////////////////////////////////////////////////////////////
// Basic type defintions.
////////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup Array2
 * @{
 */

//! A two-dimensional array with real elements.
typedef Array2<double> RealArray2;

//! A two-dimensional array with integer elements.
typedef Array2<int> IntArray2;

/**
 * @}
 */

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

}

#endif
