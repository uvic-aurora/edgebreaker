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

// This program designs several basic types of filters.

#include <iostream>
#include <algorithm>
#include <iterator>
#include <SPL/Sequence2.hpp>

typedef SPL::Sequence1<double> RealSeq1;

int doDownsample(std::istream& in, std::ostream& out)
{
	RealSeq1 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	int factor;
	if (!(in >> factor)) {
		std::cerr << "cannot get factor\n";
		return -1;
	};
	RealSeq1 result = SPL::downsample(seq, factor);
	out << result << "\n";
	return 0;
}

int doUpsample(std::istream& in, std::ostream& out)
{
	RealSeq1 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	int factor;
	int pad;
	if (!(in >> factor)) {
		std::cerr << "cannot get factor\n";
		return -1;
	};
	if (!(in >> pad)) {
		std::cerr << "cannot get pad\n";
		return -1;
	};
	RealSeq1 result = SPL::upsample(seq, factor, pad);
	out << result << "\n";
	return 0;
}

int main(int argc, char** argv)
{

	for (;;) {
		int result;

		std::string opName;

		if (!(std::cin >> opName)) {
			std::cerr << "cannot get operation\n";
			return -1;
		}

		if (opName == "end") {
			break;
		} else if (opName == "downsample") {
			result = doDownsample(std::cin, std::cout);
		} else if (opName == "upsample") {
			result = doUpsample(std::cin, std::cout);
		}

		if (result) {
			break;
		}

	}

}
