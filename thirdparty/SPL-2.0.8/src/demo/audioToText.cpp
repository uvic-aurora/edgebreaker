// Copyright (c) 2013 Michael D. Adams
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

// This program computes the difference between two 1-dimensional arrays
// using various metrics.

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <SPL/Array1.hpp>
#include <SPL/math.hpp>
#include <SPL/audioFile.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

typedef SPL::Array1<double> RealArray1;

char *cmdName = 0;

void usage()
{
	std::cerr
	  << "usage:\n"
	  << "    " << cmdName << " audioFile\n"
	  ;
	exit(2);
}

int main(int argc, char** argv)
{
	cmdName = argv[0];

	int verbose = 0;
	std::string inFileName;
	int sampRate;
	RealArray1 data;

	int c;
	while ((c = getopt(argc, argv, "v")) != EOF) {
		switch (c) {
		case 'v':
			++verbose;
			break;
		default:
			usage();
			break;
		}
	}

	if (optind >= argc) {
		usage();
	}
	inFileName = argv[optind];
	++optind;

	if (!inFileName.size()) {
		std::cerr << "no reference data file specified\n";
		usage();
	}

	if (SPL::loadAudioFile(inFileName.c_str(), sampRate, data)) {
		std::cerr << "cannot read audio file " << inFileName << "\n";
		exit(1);
	}

	std::cout << sampRate << "\n";
	std::cout << data << "\n";

	return 0;
}
