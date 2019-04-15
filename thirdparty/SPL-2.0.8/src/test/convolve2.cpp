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
#include <string>
#include <SPL/Array2.hpp>
#include <SPL/Sequence2.hpp>
#include <SPL/Sequence1.hpp>

typedef SPL::Sequence2<double> RealSeq2;
typedef SPL::Sequence1<double> RealSeq1;

int main(int argc, char** argv)
{
	int convMode = SPL::ConvolveMode::full;

	int c;
	while ((c = getopt(argc, argv, "m:")) >= 0) {
		switch (c) {
		case 'm':
			convMode = atoi(optarg);
			break;
		default:
			abort();
			break;
		}
	}

	std::cout.precision(6);
	std::cerr << "convMode=" << convMode << "\n";

	for (;;) {

		RealSeq2 seq;
		RealSeq1 filter;

		if (!(std::cin >> seq)) {
			break;
		}

		if (!(std::cin >> filter)) {
			break;
		}

		RealSeq1 delta(0, 1, 1.0);
		RealSeq2 horzResult = convolveSeparable(seq, filter, delta, convMode);
		RealSeq2 vertResult = convolveSeparable(seq, delta, filter, convMode);
		RealSeq2 bothResult = convolveSeparable(seq, filter, filter, convMode);

		std::cout << "input:\n" << seq << "\n";
		std::cout << "filter:\n" << filter << "\n";
		std::cout << "output horz:\n" << horzResult << "\n";
		std::cout << "output vert:\n" << vertResult << "\n";
		std::cout << "output obth:\n" << bothResult << "\n";

	}

	return 0;

}

