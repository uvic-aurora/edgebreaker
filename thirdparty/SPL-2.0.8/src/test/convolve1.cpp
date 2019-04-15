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
#include <unistd.h>
#include <SPL/Array1.hpp>
#include <SPL/Sequence1.hpp>

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

		RealSeq1 seq;
		RealSeq1 filt;

		if (!(std::cin >> seq)) {
			break;
		}

		if (!(std::cin >> filt)) {
			break;
		}

		RealSeq1 result = convolve(seq, filt, convMode);

		//std::cout << seq << "\n";
		//std::cout << filt << "\n";
		std::cout << result << "\n";

	}

	return 0;

}

