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
 * @file Sequence.hpp
 * @brief Common header for sequence classes.
 */

#ifndef SPL_Sequence_hpp
#define SPL_Sequence_hpp

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Constants identifying various convolution modes.
 */
struct ConvolveMode
{
	//! The full convolution result (i.e., the same as "full" in MATLAB)
	static const int full = 0;

	//! The same as "same" in MATLAB
	static const int sameDomainZeroExt = 1;

	//! Constant extension
	static const int sameDomainConstExt = 3;

	//! Periodic extension
	static const int sameDomainPerExt = 2;

	//! Symmetric periodic extension
	static const int sameDomainSymExt0 = 4;
};

/*!
 * @}
 * @}
 */

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/// @cond
/*!
 * @brief Calculate an inner product for convolution.
 *
 * seqStart iterator for start of sequence
 * seqEnd   iterator for end of sequence
 * filtStart iterator for start of filter
 * filtEnd iterator for end of filter
 * init value for initializing inner product
 */
template <class Iterator1, class Iterator2, class T>
T special_inner_product(Iterator1 seqStart, Iterator2 filtStart,
  Iterator2 filtEnd, T init)
{
	T result = init;
	while (filtStart != filtEnd) {
		--filtEnd;
//std::cerr << "result " << ((*seqStart) * (*filtEnd)) << "\n";
		result += (*seqStart) * (*filtEnd);
		++seqStart;
	}
	return result;
}
/// @endcond

/// @cond
/*!
 * @brief Compute a full convolution.
 */
template <class T, class SeqIterator, class FiltIterator, class ResultIterator>
void convolveHelper(SeqIterator seqStart, SeqIterator seqEnd,
  FiltIterator filtStart, FiltIterator filtEnd, ResultIterator resultStart,
  T dummy)
{
	if ((seqEnd - seqStart) >= (filtEnd - filtStart)) {

		// The number of elements in the range [seqStart, seqEnd) is not
		// less than the number of elements in [filtStart, filtEnd).

		SeqIterator seqStartIter1 = seqStart;
		FiltIterator filtStartIter1 = filtStart;
		FiltIterator filtEndIter1 = filtStartIter1 + 1;
		ResultIterator resultIter = resultStart;

		// Process the left boundary.
		int n = (filtEnd - filtStart) - 1;
		while (--n >= 0) {
			*resultIter = special_inner_product(seqStartIter1, filtStartIter1,
			  filtEndIter1, T(0));
			++filtEndIter1;
			++resultIter;
		}

		// Process the interior.
		n = (seqEnd - seqStart) - (filtEnd - filtStart) + 1;
		assert(filtStartIter1 == filtStart);
		assert(filtEndIter1 == filtEnd);
		while (--n >= 0) {
			*resultIter = special_inner_product(seqStartIter1, filtStartIter1,
			  filtEndIter1, T(0));
			++resultIter;
			++seqStartIter1;
		}

		// Process the right boundary.
		n = (filtEnd - filtStart) - 1;
		++filtStartIter1;
		assert(filtStartIter1 == filtStart + 1);
		assert(filtEndIter1 == filtEnd);
		while (--n >= 0) {
			*resultIter = special_inner_product(seqStartIter1, filtStartIter1,
			  filtEndIter1, T(0));
			++seqStartIter1;
			++filtStartIter1;
			++resultIter;
		}

	} else {

		convolveHelper(filtStart, filtEnd, seqStart, seqEnd, resultStart, dummy);

#if 0
		// The number of elements in the range [seqStart, seqEnd) is
		// less than the number of elements in [filtStart, filtEnd).
		// In principle, we need to swap these ranges.
		// We cannot actually do a swap since the iterators may not be
		// of the same type.
		// The code in this else clause is identical to the code in the
		// above if clause, except that we have reversed the role of
		// the two ranges.

		FiltIterator filtStartIter1 = filtStart;
		SeqIterator seqStartIter1 = seqStart;
		SeqIterator seqEndIter1 = seqStartIter1 + 1;
		ResultIterator resultIter = resultStart;

		// Process the left boundary.
		int n = (seqEnd - seqStart) - 1;
		while (--n >= 0) {
			*resultIter = special_inner_product(filtStartIter1, seqStartIter1,
			  seqEndIter1, T(0));
			++seqEndIter1;
			++resultIter;
		}

		// Process the interior.
		n = (filtEnd - filtStart) - (seqEnd - seqStart) + 1;
		assert(seqStartIter1 == seqStart);
		assert(seqEndIter1 == seqEnd);
		while (--n >= 0) {
			*resultIter = special_inner_product(filtStartIter1, seqStartIter1,
			  seqEndIter1, T(0));
			++resultIter;
			++filtStartIter1;
		}

		// Process the right boundary.
		n = (seqEnd - seqStart) - 1;
		++seqStartIter1;
		assert(seqStartIter1 == seqStart + 1);
		assert(seqEndIter1 == seqEnd);
		while (--n >= 0) {
			*resultIter = special_inner_product(filtStartIter1, seqStartIter1,
			  seqEndIter1, T(0));
			++filtStartIter1;
			++seqStartIter1;
			++resultIter;
		}
#endif
	}

}
/// @endcond

/// @cond
/*!
 * @brief Compute part of a full convolution.
 *
 */
template <class T, class SeqIterator, class FiltIterator, class ResultIterator>
void convolveHelper2(SeqIterator seqStart, SeqIterator seqEnd,
  FiltIterator filtStart, FiltIterator filtEnd, ResultIterator resultStart,
  int skip, int count, T dummy)
{
	if (count <= 0) {
		return;
	}
	if ((seqEnd - seqStart) >= (filtEnd - filtStart)) {
		assert(count >= 0 && count <= (seqEnd - seqStart));

		SeqIterator seqStartIter1 = seqStart;
		FiltIterator filtStartIter1 = filtStart;
		FiltIterator filtEndIter1 = filtStartIter1 + 1;
		ResultIterator resultIter = resultStart;

		// Process the left boundary.
		int n = (filtEnd - filtStart) - 1;
		int adjust = std::min(n, skip);
		assert(adjust >= 0);
		skip -= adjust;
		n -= adjust;
		filtEndIter1 += adjust;
		while (--n >= 0) {
			if (--count < 0) {
				return;
			}
			*resultIter = special_inner_product(seqStartIter1, filtStartIter1,
			  filtEndIter1, T(0));
			++filtEndIter1;
			++resultIter;
		}

		// Process the interior.
		n = (seqEnd - seqStart) - (filtEnd - filtStart) + 1;
		filtStartIter1 = filtStart;
		filtEndIter1 = filtEnd;
		adjust = std::min(n, skip);
		assert(adjust >= 0);
		skip -= adjust;
		n -= adjust;
		seqStartIter1 += adjust;
		while (--n >= 0) {
			if (--count < 0) {
				return;
			}
			*resultIter = special_inner_product(seqStartIter1, filtStartIter1,
			  filtEndIter1, T(0));
			++resultIter;
			++seqStartIter1;
		}

		// Process the right boundary.
		n = (filtEnd - filtStart) - 1;
		filtStartIter1 = filtStart + 1;
		filtEndIter1 = filtEnd;
		adjust = std::min(n, skip);
		assert(adjust >= 0);
		skip -= adjust;
		n -= adjust;
		filtStartIter1 += adjust;
		while (--n >= 0) {
			if (--count < 0) {
				return;
			}
			*resultIter = special_inner_product(seqStartIter1, filtStartIter1,
			  filtEndIter1, T(0));
			++seqStartIter1;
			++filtStartIter1;
			++resultIter;
		}

	} else {
		convolveHelper2(filtStart, filtEnd, seqStart, seqEnd, resultStart,
		  skip, count, dummy);
#if 0
		assert(count >= 0 && count <= (filtEnd - filtStart));

		FiltIterator filtStartIter1 = filtStart;
		SeqIterator seqStartIter1 = seqStart;
		SeqIterator seqEndIter1 = seqStartIter1 + 1;
		ResultIterator resultIter = resultStart;

		// Process the left boundary.
		int n = (seqEnd - seqStart) - 1;
		int adjust = std::min(n, skip);
		assert(adjust >= 0);
		skip -= adjust;
		n -= adjust;
		seqEndIter1 += adjust;
		while (--n >= 0) {
			if (--count < 0) {
				return;
			}
			*resultIter = special_inner_product(filtStartIter1, seqStartIter1,
			  seqEndIter1, T(0));
			++seqEndIter1;
			++resultIter;
		}

		// Process the interior.
		n = (filtEnd - filtStart) - (seqEnd - seqStart) + 1;
		seqStartIter1 = seqStart;
		seqEndIter1 = seqEnd;
		adjust = std::min(n, skip);
		assert(adjust >= 0);
		skip -= adjust;
		n -= adjust;
		filtStartIter1 += adjust;
		while (--n >= 0) {
			if (--count < 0) {
				return;
			}
			*resultIter = special_inner_product(filtStartIter1, seqStartIter1,
			  seqEndIter1, T(0));
			++resultIter;
			++filtStartIter1;
		}

		// Process the right boundary.
		n = (seqEnd - seqStart) - 1;
		seqStartIter1 = seqStart + 1;
		seqEndIter1 = seqEnd;
		adjust = std::min(n, skip);
		assert(adjust >= 0);
		skip -= adjust;
		n -= adjust;
		seqStartIter1 += adjust;
		while (--n >= 0) {
			if (--count < 0) {
				return;
			}
			*resultIter = special_inner_product(filtStartIter1, seqStartIter1,
			  seqEndIter1, T(0));
			++filtStartIter1;
			++seqStartIter1;
			++resultIter;
		}
#endif
	}

}
/// @endcond

}

#endif
