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
 * @file Array1.hpp
 * @brief This file contains the Array1 template class and supporting code.
 */

#ifndef SPL_Array1_hpp
#define SPL_Array1_hpp

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

//! Defining this symbol will enable extra code for debugging.
//#define SPL_ARRAY1_DEBUG

#if defined(SPL_ARRAY1_DEBUG)
//! Prevent the inlining of functions.
#define SPL_ARRAY1_INLINE
#else
//! Allow the inlining of functions.
#define SPL_ARRAY1_INLINE inline
#endif

////////////////////////////////////////////////////////////////////////////////
// Header Files.
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cassert>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <SPL/misc.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// One-Dimensional Array Template Class
// (with lazy copying and reference counting)
////////////////////////////////////////////////////////////////////////////////

/*!
 * @addtogroup Array1
 * @{
 */

template <class> class Array1;

/// @cond

/*
 * An array data block, which can be shared by multiple arrays.
 */

template <class T>
class ArrayRep1
{
private:

	template <class> friend class Array1;

	ArrayRep1(int size);

	ArrayRep1(int size, const T& value);

	template <class InputIterator>
	ArrayRep1(int size, InputIterator data);

	~ArrayRep1();

	// Prevent copy construction.
	// This function is never defined.
	ArrayRep1(const ArrayRep1&);

	// Prevent copy assignment.
	// This function is never defined.
	ArrayRep1& operator=(const ArrayRep1&);

	// Create a copy of this data block.
	ArrayRep1* clone() const;

	// Get the reference count for the underlying data.
	int getRefCount() const;

	// Increment the reference count for the underlying data.
	void hold();

	// Decrement the reference count for the underlying data, deallocating
	// the underlying data if the reference count reaches zero.
	void release();

	// The array data.
	std::vector<T> data_;

	// The number of arrays referencing the array data.
	int refCount_;
};

/// @endcond

/*!
 * @brief A one-dimensional array class with lazy copying and reference
 * counting.
 */

template <class T>
class Array1
{
public:

	////////////////////////////////////////////////////////////
	// Types
	////////////////////////////////////////////////////////////

	//! The type of the elements in the array.
	typedef T ElemType;

	//! A mutable iterator for the array elements.
	typedef typename std::vector<T>::iterator Iterator;

	//! A constant iterator for the array elements.
	typedef typename std::vector<T>::const_iterator ConstIterator;

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Create an empty array.
	 */
	Array1();

	/*!
	 * @brief Create an array of the specified size.
	 */
	explicit Array1(int size);

	/*!
	 * @brief Create an array of the given size with all elements
	 * initialized to the specified value.
	 */
	Array1(int size, const T& value);

	/*!
	 * @brief Create an array of the specified size with the elements
	 * initialized to the data obtained from the given input iterator.
	 */
	template <class InputIterator>
	Array1(int size, InputIterator data);

	/*!
	 * @brief Create a copy of an array.
	 */
	Array1(const Array1& a);

	/*!
	 * @brief Create a copy of an array with elements of arbitrary
	 * type.
	 *
	 * Note: The type OtherType must be assignable to the type T.
	 */
	template<class OtherType>
	Array1(const Array1<OtherType>& a);

	/*!
	 * @brief Destroy an array.
	 */
	~Array1();

	////////////////////////////////////////////////////////////
	// Assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Assign one array to another.
	 */
	Array1& operator=(const Array1& a);

	/*!
	 * @brief Assign an array with elements of arbitrary type to another
	 * array.
	 */
	template<class OtherType>
	Array1<T>& operator=(const Array1<OtherType>& a);

	////////////////////////////////////////////////////////////
	// Compound assignment operators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Add another array (elementwise) to this array.
	 */
	Array1& operator+=(const Array1& a);

	/*!
	 * @brief Subtract another array (elementwise) from this array.
	 */
	Array1& operator-=(const Array1& a);

	/*!
	 * @brief Multiply another array (elementwise) by this array.
	 */
	Array1& operator*=(const Array1& a);

	/*!
	 * @brief Divide this array (elementwise) by another array.
	 */
	Array1& operator/=(const Array1& a);

	/*!
	 * @brief Add the specified value to each element in the array.
	 */
	Array1& operator+=(const T& value);

	/*!
	 * @brief Subtract the specified value from each element in the array.
	 */
	Array1& operator-=(const T& value);

	/*!
	 * @brief Multiply each element in the array by the specified value.
	 */
	Array1& operator*=(const T& value);

	/*!
	 * @brief Divide each element in the array by the specified value.
	 */
	Array1& operator/=(const T& value);

	////////////////////////////////////////////////////////////
	// Basic queries
	////////////////////////////////////////////////////////////

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
    bool isSharedWith(const Array1& a) const;

	////////////////////////////////////////////////////////////
	// Accessors
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a mutable reference to the specified element in the array.
	 */
	T& operator()(int i);

	/*!
	 * @brief Get a const reference to the specified element in the array.
	 */
	const T& operator()(int i) const;

	////////////////////////////////////////////////////////////
	// Iterators
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get a const iterator referring to the first element in the
	 * array.
	 */
	ConstIterator begin() const;

	/*!
	 * @brief Get a mutable iterator referring to the first element in the
	 * array.
	 */
	Iterator begin();

	/*!
	 * @brief Get a const iterator referring to one past the last element
	 * in the array.
	 */
	ConstIterator end() const;

	/*!
	 * @brief Get a mutable iterator referring to one past the last element
	 * in the array.
	 */
	Iterator end();

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
	void resize(int size);

	/*!
	 * @brief Change the size of the array, initializing the resized
	 * array with the data obtained from the specified input iterator.
	 */
	template <class InputIterator>
	void resize(int size, InputIterator data);

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
	 * @brief Output an array to a stream with a particular field width
	 * to be used for each element.
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
	 * @brief Swap the contents of the array with the contents of another
	 * array.
	 */
	void swap(Array1& a);

	/*!
	 * @brief Output information about an array to a stream for debugging.
	 */
	void dump(std::ostream& out) const;

private:

	template <class> friend class Array1;
	typedef ArrayRep1<T> Rep;

	// Get the reference count for the underlying data.
	int getRefCount() const;

	// Force the underlying data to be copied if the data is shared.
	void unshare() const;

	// Force the underlying data to be copied.
	// The data is assumed to be shared.
	void copyOnWrite() const;

	// The underlying data.
	mutable Rep* ptr_;
};

////////////////////////////////////////////////////////////////////////////////
// Code for the ArrayRep1 class
////////////////////////////////////////////////////////////////////////////////

/// @cond

template <class T>
SPL_ARRAY1_INLINE int ArrayRep1<T>::getRefCount() const
{
	return refCount_;
}

template <class T>
SPL_ARRAY1_INLINE void ArrayRep1<T>::hold()
{
	++refCount_;
}

template <class T>
SPL_ARRAY1_INLINE void ArrayRep1<T>::release()
{
	if (--refCount_ == 0) {
		delete this;
	}
}

template <class T>
SPL_ARRAY1_INLINE ArrayRep1<T>::ArrayRep1(int size) : data_(size),
  refCount_(1)
{
	assert(size >= 0);
}

template <class T>
SPL_ARRAY1_INLINE ArrayRep1<T>::ArrayRep1(int size, const T& value) : data_(size, value),
  refCount_(1)
{
	assert(size >= 0);
}

template <class T>
template <class InputIterator>
SPL_ARRAY1_INLINE ArrayRep1<T>::ArrayRep1(int size, InputIterator data) :
  data_(), refCount_(1)
{
	assert(size >= 0);
	data_.reserve(size);
	SPL::copy_n(data, size, std::back_inserter(data_));
	assert(data_.size() == size);
}

template <class T>
SPL_ARRAY1_INLINE ArrayRep1<T>::~ArrayRep1()
{
	assert(!refCount_);
}

template <class T>
SPL_ARRAY1_INLINE ArrayRep1<T>* ArrayRep1<T>::clone() const
{
	return new ArrayRep1(data_.size(), data_.begin());
}

/// @endcond

////////////////////////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY1_INLINE Array1<T>::Array1()
{
	ptr_ = new Rep(0);
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::default_ctor() "
	  << "["
	  << this << " "
	  << ptr_
	  << "]"
	  << "\n";
#endif
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>::Array1(int size)
{
	ptr_ = new Rep(size);
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::ctor(" << size << ") "
	  << "["
	  << this << " "
	  << ptr_
	  << "]"
	  << "\n";
#endif
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>::Array1(const Array1<T>& a)
{
	ptr_ = a.ptr_;
	ptr_->hold();
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::copy_ctor(" << (&a) << ") "
	  << "["
	  << this << " "
	  << ptr_
	  << "]"
	  << "\n";
#endif
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>::Array1(int size, const T& value)
{
	ptr_ = new Rep(size, value);
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::ctor(" << size << ", " << value << ") "
	  << "["
	  << this << " "
	  << ptr_
	  << "]"
	  << "\n";
#endif
}

template <class T>
template <class InputIterator>
SPL_ARRAY1_INLINE Array1<T>::Array1(int size, InputIterator data)
{
	ptr_ = new Rep(size, data);
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::ctor(" << size << ", " << "InputIterator" << ") "
	  << "["
	  << this << " "
	  << ptr_
	  << "]"
	  << "\n";
#endif
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>::~Array1()
{
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::dtor() "
	  << "["
	  << this << " "
	  << ptr_
	  << "]"
	  << "\n";
#endif
	ptr_->release();
}

template <class T>
template <class OtherType>
SPL_ARRAY1_INLINE Array1<T>::Array1(const Array1<OtherType>& a)
{
	ptr_ = new Rep(a.getSize(), a.begin());
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "pseudo_copy_ctor(" << this << ") "
	  << this << " " << ptr_ << "\n";
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Assignment operators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator=(const Array1<T>& a)
{
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::operator=(" << (&a) << ") "
	  << "["
	  << this << " " << ptr_ << " "
	  << (&a) << " " << a.ptr_ << ""
	  << "]"
	  << "\n";
#endif
	if (this != &a) {
		ptr_->release();
		ptr_ = a.ptr_;
		ptr_->hold();
	}
	return *this;
}

template <class T>
template <class OtherType>
Array1<T>& Array1<T>::operator=(const Array1<OtherType>& a)
{
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "Array1::operator= special\n";
#endif
	if (reinterpret_cast<void*>(this) != reinterpret_cast<const void*>(&a)) {
		resize(a.getSize());
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
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator+=(const Array1<T>& a)
{
	assert(getSize() == a.getSize());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::plus<T>());
	return *this;
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator-=(const Array1<T>& a)
{
	assert(getSize() == a.getSize());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::minus<T>());
	return *this;
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator*=(const Array1<T>& a)
{
	assert(getSize() == a.getSize());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::multiplies<T>());
	return *this;
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator/=(const Array1<T>& a)
{
	assert(getSize() == a.getSize());
	unshare();
	std::transform(ptr_->data_.begin(), ptr_->data_.end(),
	  a.ptr_->data_.begin(), ptr_->data_.begin(), std::divides<T>());
	return *this;
}

template <class T>
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator+=(const T& value)
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
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator-=(const T& value)
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
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator*=(const T& value)
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
SPL_ARRAY1_INLINE Array1<T>& Array1<T>::operator/=(const T& value)
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
SPL_ARRAY1_INLINE int Array1<T>::getSize() const
{
	return ptr_->data_.size();
}

template <class T>
SPL_ARRAY1_INLINE bool Array1<T>::isShared() const
{
	return ptr_->getRefCount() > 1;
}

template <class T>
SPL_ARRAY1_INLINE bool Array1<T>::isSharedWith(const Array1& a) const
{
	return ptr_ == a.ptr_;
}

////////////////////////////////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY1_INLINE T& Array1<T>::operator()(int i)
{
	assert(i >= 0 && i < getSize());
	unshare();
	return ptr_->data_[i];
}

template <class T>
SPL_ARRAY1_INLINE const T& Array1<T>::operator()(int i) const
{
	assert(i >= 0 && i < getSize());
	return ptr_->data_[i];
}

////////////////////////////////////////////////////////////////////////////////
// Iterators
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY1_INLINE typename Array1<T>::Iterator Array1<T>::begin()
{
	unshare();
	return ptr_->data_.begin();
}

template <class T>
SPL_ARRAY1_INLINE typename Array1<T>::Iterator Array1<T>::end()
{
	unshare();
	return ptr_->data_.end();
}

template <class T>
SPL_ARRAY1_INLINE typename Array1<T>::ConstIterator Array1<T>::begin() const
{
	return ptr_->data_.begin();
}

template <class T>
SPL_ARRAY1_INLINE typename Array1<T>::ConstIterator Array1<T>::end() const
{
	return ptr_->data_.end();
}

////////////////////////////////////////////////////////////////////////////////
// Resizing
////////////////////////////////////////////////////////////////////////////////

template <class T>
void Array1<T>::resize(int size)
{
	assert(size >= 0);
	if (getSize() != size) {
		ptr_->release();
		ptr_ = new Rep(size);
	}
}

template <class T>
template <class InputIterator>
void Array1<T>::resize(int size, InputIterator data)
{
	assert(size >= 0);
	if (getSize() == size && ptr_->getRefCount() == 1) {
		SPL::copy_n(data, size, ptr_->data_.begin());
	} else {
		ptr_->release();
		ptr_ = new Rep(size, data);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Get various statistics
////////////////////////////////////////////////////////////////////////////////

template<class T>
SPL_ARRAY1_INLINE T Array1<T>::max() const
{
	assert(getSize() > 0);
	return *std::max_element(begin(), end());
}

template<class T>
SPL_ARRAY1_INLINE T Array1<T>::min() const
{
	assert(getSize() > 0);
	return *std::min_element(begin(), end());
}

template<class T>
SPL_ARRAY1_INLINE T Array1<T>::sum() const
{
	return std::accumulate(begin(), end(), T(0));
}

////////////////////////////////////////////////////////////////////////////////
// Input/output
////////////////////////////////////////////////////////////////////////////////

template<class T>
std::ostream& Array1<T>::output(std::ostream& out, int fieldWidth) const
{
	out << getSize() << "\n";
	for (int i = 0; i < getSize(); ++i) {
		T val = operator()(i);
		std::stringstream str;
		str << std::setw(fieldWidth) << val;
		std::string buf = str.str();
		if (buf.size() > fieldWidth) {
			buf = buf.substr(0, fieldWidth);
		}
		out << buf;
		if (i < getSize() - 1) {
			out << " ";
		}
	}
	out << "\n";
	return out;
}

template<class T>
int Array1<T>::load(const char* fileName)
{
	std::ifstream in(fileName);
	in >> *this;
	if (!in) {
		return -1;
	}
	return 0;
}

template<class T>
int Array1<T>::save(const char* fileName) const
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
std::ostream& operator<<(std::ostream& out, const Array1<T>& a)
{
	out << a.getSize() << " ";
	typename Array1<T>::ConstIterator dataIter = a.begin();
	for (int i = 0; i < a.getSize(); ++i) {
		if (i) {
			out << " ";
		}
		out << *dataIter;
		++dataIter;
	}
//	out << " ";
	return out;
}

/*!
 * @brief Input an array from the specified stream.
 */
template<class T>
std::istream& operator>>(std::istream& in, Array1<T>& a)
{
	int size;
	in >> size;
	if (!in) {
		return in;
	}
	a.resize(size);
	for (int i = 0; i < a.getSize(); ++i) {
		T value;
		in >> value;
		if (!in) {
			return in;
		}
		a(i) = value;
	}
	return in;
}

////////////////////////////////////////////////////////////////////////////////
// Miscellaneous
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY1_INLINE void Array1<T>::swap(Array1<T>& a)
{
	if (this != &a) {
		// Both reference counts increase and then decrease, for no
		// overall net change.
		std::swap(ptr_, a.ptr_);
	}
}

template <class T>
SPL_ARRAY1_INLINE void Array1<T>::fill(const T& value)
{
	unshare();
	std::fill(begin(), end(), value);
}

/*!
 * @brief Test two arrays for equality.
 */
template<class T>
bool operator==(const Array1<T>& a, const Array1<T>& b)
{
	if (a.getSize() != b.getSize()) {
		return false;
	} else {
		if (a.isSharedWith(b)) {
			return true;
		}
		return std::equal(a.begin(), a.end(), b.begin());
	}
}

/*!
 * @brief Test two arrays for inequality.
 */
template<class T>
SPL_ARRAY1_INLINE bool operator!=(const Array1<T>& a, const Array1<T>& b)
{
	return !(a == b);
}

template<class T>
void Array1<T>::dump(std::ostream& out) const
{
	out
	  << "refCount=" << ptr_->getRefCount() << " "
	  << "size=" << getSize() << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// Private code
////////////////////////////////////////////////////////////////////////////////

template <class T>
SPL_ARRAY1_INLINE void Array1<T>::copyOnWrite() const
{
#if defined(SPL_ARRAY1_DEBUG)
	std::cerr << "copyOnWrite "
	  << "["
	  << this << " "
	  << ptr_ << " "
	  << ptr_->getRefCount()
	  << "]"
	  << "\n";
	std::cerr << "copied array size " << getSize() << "\n";
#endif
	assert(ptr_->getRefCount() > 1);
	ptr_->release();
	//const_cast<Array1*>(this)->ptr_ = ptr_->clone();
	this->ptr_ = ptr_->clone();
}

template <class T>
SPL_ARRAY1_INLINE void Array1<T>::unshare() const
{
	if (ptr_->getRefCount() > 1) {
		copyOnWrite();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Basic types
////////////////////////////////////////////////////////////////////////////////

//! A one-dimensional array with real elements.
typedef Array1<double> RealArray1;

//! A one-dimensional array with integer elements.
typedef Array1<int> IntArray1;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @}
 */

}

#endif
