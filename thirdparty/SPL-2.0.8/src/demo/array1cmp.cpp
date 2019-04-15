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
	  << "    " << cmdName
	  << " -f referenceFileName -F otherFileName [options]\n"
	  << "Options:\n"
	  << "-a ... Specify that input files are in WAV format.\n"
	  << "-m metric ... Specify the error metric.\n"
	  << "Valid values for the metric are:\n"
	  << "    pae  ... peak absolute error\n"
	  << "    mae  ... mean absolute error\n"
	  << "    mse  ... mean squared error\n"
	  << "    rmse ... root mean squared error\n"
	  ;
	exit(2);
}

struct Stats
{
	long totalCount;
	long diffCount;
	double meanAbsErr;
	double meanSqrErr;
	double maxAbsErr;
	double minAbsErr;
};

void getStats(const RealArray1& ref, const RealArray1& oth, Stats& stats)
{
	assert(ref.getSize() && oth.getSize());

	long diffCount = 0;
	long totalCount = 0;
	double sumAbsErr = 0.0;
	double sumSqrErr = 0.0;
	double minAbsErr = -1.0;
	double maxAbsErr = -1.0;

	int n = std::max(ref.getSize(), oth.getSize());
	for (int i = 0; i < n; ++i) {
		double refVal = (i < ref.getSize()) ? ref(i) : 0;
		double othVal = (i < oth.getSize()) ? oth(i) : 0;
		double absErr = SPL::absVal(othVal - refVal);
		sumAbsErr += absErr;
		sumSqrErr += SPL::sqr(absErr);
		if (refVal != othVal) {
			++diffCount;
		}
		if (minAbsErr < 0 || absErr < minAbsErr) {
			minAbsErr = absErr;
		}
		if (maxAbsErr < 0 || absErr > maxAbsErr) {
			maxAbsErr = absErr;
		}
		++totalCount;
	}

	stats.totalCount = totalCount;
	stats.diffCount = diffCount;
	stats.meanAbsErr = sumAbsErr / totalCount;
	stats.meanSqrErr = sumSqrErr / totalCount;
	stats.maxAbsErr = maxAbsErr;
	stats.minAbsErr = minAbsErr;
}

int main(int argc, char** argv)
{
	cmdName = argv[0];

	int verbose = 0;
	std::string refFileName;
	std::string othFileName;
	int refFmt = 0;
	int othFmt = 0;
	std::string metric("mse");

	int c;
	while ((c = getopt(argc, argv, "f:F:m:va")) != EOF) {
		switch (c) {
		case 'f':
			refFileName = optarg;
			break;
		case 'F':
			othFileName = optarg;
			break;
		case 'm':
			metric = optarg;
			break;
		case 'v':
			++verbose;
			break;
		case 'a':
			refFmt = 1;
			othFmt = 1;
			break;
		default:
			usage();
			break;
		}
	}

	if (!refFileName.size()) {
		std::cerr << "no reference data file specified\n";
		usage();
	}
	if (!othFileName.size()) {
		std::cerr << "no other data file specified\n";
		usage();
	}

	int refSampRate = 0;
	int othSampRate = 0;
	RealArray1 refData;
	RealArray1 othData;

	if (refFmt) {
		if (SPL::loadAudioFile(refFileName.c_str(), refSampRate, refData)) {
			std::cerr << "cannot read audio file " << refFileName << "\n";
			exit(1);
		}
	} else {
		if (refFileName == "-") {
			if (std::cin >> refData) {
				std::cerr << "cannot read reference data from standard input\n";
				exit(1);
			}
		} else {
			if (refData.load(refFileName.c_str())) {
				std::cerr << "cannot read reference data from " <<
				  refFileName << "\n";
				exit(1);
			}
		}
	}

	if (othFmt) {
		if (SPL::loadAudioFile(othFileName.c_str(), othSampRate, othData)) {
			std::cerr << "cannot read audio file " << othFileName << "\n";
			exit(1);
		}
	} else {
		if (othFileName == "-") {
			if (std::cin >> othData) {
				std::cerr << "cannot read other data from standard input\n";
				exit(1);
			}
		} else {
			if (othData.load(othFileName.c_str())) {
				std::cerr << "cannot read other data from " << othFileName <<
				  "\n";
				exit(1);
			}
		}
	}

	if (refSampRate <= 0) {
		refSampRate = othSampRate;
	} else if (othSampRate <= 0) {
		othSampRate = refSampRate;
	} else if (refSampRate != othSampRate) {
		std::cerr << "WARNING: The sampling rates do not match!\n";
		std::cerr << refSampRate << " != " << othSampRate << "\n";
	}

	if (verbose) {
		std::cout << "array size: " << refData.getSize() << "\n";
		if (refSampRate > 0) {
			std::cout
			  << "sampling rate: " << refSampRate << "\n";
		}
	}

	if (refData.getSize() != othData.getSize()) {
		std::cerr
		  << "WARNING: mismatch in number of samples "
		  << "(" << refData.getSize() << " != " << othData.getSize() << ")"
		  << "\n";
		std::cerr << "WARNING: The smaller array is being padded with zeros to match the size of the larger array.\n";
	}

	Stats stats;
	getStats(refData, othData, stats);

	std::cout.precision(16);

	if (verbose) {
		std::cout
		  << "total count: " << stats.totalCount << "\n"
		  << "difference count: " << stats.diffCount << "\n"
		  << "minimum absolute error: " << stats.minAbsErr << "\n"
		  << "maximum absolute error: " << stats.maxAbsErr << "\n"
		  << "mean absolute error: " << stats.meanAbsErr << "\n"
		  << "mean squared error: " << stats.meanSqrErr << "\n"
		  << "root mean squared error: " << sqrt(stats.meanSqrErr) << "\n"
		  ;
	}

	if (metric == "pae") {
		std::cout << stats.maxAbsErr << "\n";
	} else if (metric == "mae") {
		std::cout << stats.meanAbsErr << "\n";
	} else if (metric == "mse") {
		std::cout << stats.meanSqrErr << "\n";
	} else if (metric == "rmse") {
		std::cout << sqrt(stats.meanSqrErr) << "\n";
	} else {
		std::cout << "unknown metric\n";
	}

	return 0;
}
