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
#include <SPL/Sequence1.hpp>
#include <SPL/Sequence2.hpp>
#include <SPL/Array2.hpp>
#include <SPL/filterDesign.hpp>

char *cmdName = 0;

void usage()
{
	std::cerr
	  << "usage:\n"
	  << "    " << cmdName << " lowpass cutoff width [ripple attenuation]\n"
	  << "    " << cmdName << " highpass cutoff width [ripple attenutation]\n"
	  << "    " << cmdName << " bandpass cutoff0 cutoff1 width0 width1 [ripple attenuation]\n"
	  ;

	exit(2);
}

int main(int argc, char** argv)
{
	cmdName = argv[0];

	std::string filterType;
	int convMode;
	convMode = SPL::ConvolveMode::full;
	convMode = SPL::ConvolveMode::sameDomainZeroExt;
	convMode = SPL::ConvolveMode::sameDomainPerExt;
	convMode = SPL::ConvolveMode::sameDomainSymExt0;

	if (argc < 2) {
		usage();
	}

	filterType = argv[1];

	std::vector<std::string> args;
	std::copy(argv + 2, argv + argc, std::back_inserter(args));

	SPL::RealSequence1 filter;
	if (filterType == "lowpass") {
		if (args.size() < 2) {
			usage();
		}
		double cutoffFreq = atof(args[0].c_str());
		double transWidth = atof(args[1].c_str());
		double maxPassbandRipple = 0.1;
		double minStopbandAtten = 20.0;
		if (args.size() >= 4) {
			maxPassbandRipple = atof(args[2].c_str());
			minStopbandAtten = atof(args[3].c_str());
		}
		filter = SPL::lowpassFilter(cutoffFreq, transWidth, maxPassbandRipple,
		  minStopbandAtten);
	} else if (filterType == "highpass") {
		if (args.size() < 2) {
			usage();
		}
		double cutoffFreq = atof(args[0].c_str());
		double transWidth = atof(args[1].c_str());
		double maxPassbandRipple = 0.1;
		double minStopbandAtten = 20.0;
		if (args.size() >= 4) {
			maxPassbandRipple = atof(args[2].c_str());
			minStopbandAtten = atof(args[3].c_str());
		}
		filter = SPL::highpassFilter(cutoffFreq, transWidth, maxPassbandRipple,
		  minStopbandAtten);
	} else if (filterType == "bandpass") {
		if (args.size() < 4) {
			usage();
		}
		double cutoffFreq0 = atof(args[0].c_str());
		double cutoffFreq1 = atof(args[1].c_str());
		double transWidth0 = atof(args[2].c_str());
		double transWidth1 = atof(args[3].c_str());
		double maxPassbandRipple = 0.1;
		double minStopbandAtten = 20.0;
		if (args.size() >= 6) {
			maxPassbandRipple = atof(args[4].c_str());
			minStopbandAtten = atof(args[5].c_str());
		}
		filter = SPL::bandpassFilter(cutoffFreq0, cutoffFreq1, transWidth0,
		  transWidth1, maxPassbandRipple, minStopbandAtten);
	}

	SPL::RealArray2 data;
	int maxVal;
	bool sgnd;
	if (decodePgm(std::cin, data, maxVal, sgnd)) {
		std::cerr << "cannot read PGM data\n";
		exit(1);
	}
	SPL::RealSequence2 seq(data);
	SPL::RealSequence2 result = convolveSeparable(seq, filter, filter,
	  convMode);

	for (SPL::RealSequence2::Iterator i = result.begin(); i != result.end(); ++i) {
		*i = SPL::absVal(*i);
//		*i = std::max<double>(0, *i);
	}
	result /= result.max();
	result *= 255;

	if (encodePgm(std::cout, result.getArray(), maxVal, sgnd, false)) {
		std::cerr << "cannot write PGM data\n";
		exit(1);
	}

	return 0;
}
