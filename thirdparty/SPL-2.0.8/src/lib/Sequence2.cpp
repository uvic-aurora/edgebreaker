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
 * @file Sequence2.cpp
 * @brief This file contains code for the Sequence2 template class.
 */

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <SPL/Sequence2.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Find the bounding box of the union of the domains of two
 * sequences.
 */
void combineDomains(int firstStartX, int firstStartY, int firstEndX,
  int firstEndY, int secondStartX, int secondStartY, int secondEndX,
  int secondEndY, int& startX, int& startY, int& endX, int& endY)
{
	if (firstStartX >= firstEndX || firstStartY >= firstEndY) {
		firstStartX = 0;
		firstEndX = 0;
		firstStartY = 0;
		firstEndY = 0;
	}
	if (secondStartX >= secondEndX || secondStartY >= secondEndY) {
		secondStartX = 0;
		secondEndX = 0;
		secondStartY = 0;
		secondEndY = 0;
	}
	startX = std::min(firstStartX, secondStartX);
	startY = std::min(firstStartY, secondStartY);
	endX = std::max(firstEndX, secondEndX);
	endY = std::max(firstEndY, secondEndY);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

}
