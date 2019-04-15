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

#include <iostream>
#include <SPL/pnmCodec.hpp>
#include <SPL/Array2.hpp>

using SPL::Array2;

template <class T>
int doTest()
{
	int maxVal;
	bool sgnd;
	std::vector<Array2<T> > comps;
	if (decodePnm(std::cin, comps, maxVal, sgnd)) {
		std::cerr << "cannot decode PNM data\n";
		return -1;
	}
	assert(comps.size() > 0);

	std::cerr
	  << "numComps " << comps.size() << " "
	  << "maxVal " << maxVal << " "
	  << "sgnd " << sgnd << "\n";

	if (encodePnm(std::cout, comps, maxVal, sgnd, true)) {
		std::cerr << "cannot encode PNM data\n";
		return -1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	doTest<int>();
//	doTest<double>();

	return 0;
}

