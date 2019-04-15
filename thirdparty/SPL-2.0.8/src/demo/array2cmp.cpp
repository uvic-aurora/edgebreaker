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

// This program computes the difference between two 2-dimensional arrays
// using various metrics.

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <SPL/math.hpp>
#include <SPL/Array2.hpp>
#include <SPL/Sequence2.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

typedef SPL::Array2<double> RealArray2;
typedef SPL::Array2<int> IntArray2;
using SPL::absVal;
using SPL::pnmMaxValToPrec;
using SPL::decodePnm;
using SPL::sqr;
using SPL::encodePnm;

struct Stats
{
	double minErr;
	double maxErr;
	double minAbsErr;
	double maxAbsErr;
	int maxAbsErrPosX;
	int maxAbsErrPosY;
	double meanAbsErr;
	double meanSqrErr;
	long posErrCount;
	long negErrCount;
	long zeroErrCount;
	long totalCount;
};

const int showSignOnly = 1;
const int showEqOnly = 2;
const int ignoreSgn = 4;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

char *cmdName = 0;

inline static int sgn(int x)
{
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

inline double mseToPsnr(double mse, double maxVal)
{
	return 20.0 * log10(maxVal / sqrt(mse));
}

void makeDiffImage(const std::string fileName, const RealArray2& refData,
  const RealArray2& othData, int prec, int flags)
{
	int width = refData.getWidth();
	int height = refData.getHeight();

	std::vector<IntArray2> comps;
	for (int i = 0; i < 3; ++i) {
		comps.push_back(IntArray2(width, height));
	}

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			double err = othData(x, y) - refData(x, y);
			double absErr = absVal(err);
			double sgnErr = sgn(err);
			absErr = std::min(absErr, 255.0);
			absErr = std::max(absErr, 0.0);
			if (flags & showSignOnly) {
				absErr = 255.0;
			}
			if (flags & ignoreSgn) {
				sgnErr = absVal(sgnErr);
			}
			int extra;
			//refVal = ref(x, y);
			extra = 0;
			if (sgnErr < 0) {
				comps[0](x, y) = absErr;
				comps[1](x, y) = extra;
				comps[2](x, y) = extra;
			} else if (sgnErr > 0) {
				comps[0](x, y) = extra;
				comps[1](x, y) = absErr;
				comps[2](x, y) = extra;
			} else {
				comps[0](x, y) = extra;
				comps[1](x, y) = extra;
				comps[2](x, y) = extra;
			}
		}
	}

	std::ofstream outStream(fileName.c_str(), std::ios::out | std::ios::trunc |
	  std::ios::binary);
	encodePnm(outStream, comps, 255, false);
}

void selectBorder(IntArray2& selMask, int borderSize)
{
	selMask.fill(0);
	int width = selMask.getWidth();
	int height = selMask.getHeight();
	for (int i = 0; i < borderSize; ++i) {
		for (int x = 0; x < width; ++x) {
			selMask(x, i) = 1;
			selMask(x, height - 1 - i) = 1;
		}
		for (int y = 0; y < height; ++y) {
			selMask(0 + i, y) = 1;
			selMask(width - 1 - i, y) = 1;
		}
	}
}

void calcStats(const RealArray2& ref, const RealArray2& oth,
  const IntArray2& selMask, Stats& stats)
{
	int width = ref.getWidth();
	int height = ref.getHeight();

	int totalCount = 0;
	int zeroErrCount = 0;
	int negErrCount = 0;
	int posErrCount = 0;
	int minAbsErr = -1;
	int maxAbsErr = -1;
	double sumAbsErr = 0;
	double sumSqrErr = 0;
	double minErr = 0;
	double maxErr = 0;

	int maxAbsErrPosX = -1;
	int maxAbsErrPosY = -1;

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (selMask(x, y)) {

				double refVal = ref(x, y);
				double othVal = oth(x, y);
				double err = othVal - refVal;
				double absErr = absVal(err);

				if (minAbsErr < 0 || absErr < minAbsErr) {
					minAbsErr = absErr;
				}
				if (maxAbsErr < 0 || absErr > maxAbsErr) {
					maxAbsErr = absErr;
					maxAbsErrPosX = x;
					maxAbsErrPosY = y;
				}

				if (!totalCount || err < minErr) {
					minErr = err;
				}
				if (!totalCount || err > maxErr) {
					maxErr = err;
				}

				sumAbsErr += absErr;
				sumSqrErr += sqr(absErr);

				if (err < 0.0) {
					++negErrCount;
				} else if (err > 0.0) {
					++posErrCount;
				} else {
					++zeroErrCount;
				}

				++totalCount;

			}
		}
	}

	stats.minErr = minErr;
	stats.maxErr = maxErr;
	stats.minAbsErr = minAbsErr;
	stats.maxAbsErr = maxAbsErr;
	stats.maxAbsErrPosX = maxAbsErrPosX;
	stats.maxAbsErrPosY = maxAbsErrPosY;
	stats.meanAbsErr = sumAbsErr / totalCount;
	stats.meanSqrErr = sumSqrErr / totalCount;
	stats.posErrCount = posErrCount;
	stats.negErrCount = negErrCount;
	stats.zeroErrCount = zeroErrCount;
	stats.totalCount = totalCount;

}

void usage()
{
	std::cerr
	  << "usage:\n"
	  << "    " << cmdName
	  << " -f referenceFileName -F otherFileName [options]\n"
	  << "options:\n"
	  << "-v ... Enable verbose mode\n"
	  << "-m metric ... Specify error metric.\n"
	  << "-o diffFile ... Difference file in PNM format.\n"
	  << "-p ... Read arrays as PNM images.\n"
	  << "Valid values for the error metric are:\n"
	  << "    pae .... peak absolute error\n"
	  << "    mae .... mean absolute error\n"
	  << "    rmse ... root mean squared error\n"
	  << "    mse .... mean squared error\n"
	  << "    psnr ... peak signal to noise ratio\n"
	  ;
	exit(2);
}

int main(int argc, char** argv)
{
	cmdName = argv[0];

	int pnmFmt = 0;
	std::string refFileName;
	std::string othFileName;
	std::string diffFileName;
	std::string metricName("mse");
	int flags = ignoreSgn;
	int borderSize = 0;
	int invert = 0;
	int testMode = 0;
	int verbose = 0;

	int c;
	while ((c = getopt(argc, argv, "pTf:F:o:S:E:s:B:Ivm:")) >= 0) {
		switch (c) {
		case 'f':
			refFileName = optarg;
			break;
		case 'F':
			othFileName = optarg;
			break;
		case 'm':
			metricName = optarg;
			break;
		case 'o':
			diffFileName = optarg;
			break;
		case 'S':
			flags = (flags & ~showSignOnly) | (atoi(optarg) ? showSignOnly : 0);
			break;
		case 'E':
			flags = (flags & ~showEqOnly) | (atoi(optarg) ? showEqOnly : 0);
			break;
		case 's':
			flags = (flags & ~ignoreSgn) | (atoi(optarg) ? ignoreSgn : 0);
			break;
		case 'B':
			borderSize = atoi(optarg);
			break;
		case 'I':
			invert = 1;
			break;
		case 'T':
			testMode = 1;
			break;
		case 'p':
			pnmFmt = 1;
			break;
		case 'v':
			++verbose;
			break;
		default:
			usage();
			break;
		}
	}

	if (!othFileName.size()) {
		std::cerr << "missing other file name\n";
		usage();
	}

	if (verbose) {
		std::cout << "reference: " << refFileName << "\n";
		std::cout << "other: " << othFileName << "\n";
	}

	RealArray2 othData;
	double maxVal;
	{
		std::ifstream inStream0;
		if (othFileName != "-") {
			inStream0.open(othFileName.c_str());
		}
		std::istream& inStream = ((othFileName == "-") ? std::cin : inStream0);
		if (pnmFmt) {
			int tmpMaxVal;
			bool sgnd;
			if (decodePgm(inStream, othData, tmpMaxVal, sgnd)) {
				std::cerr << "cannot read image from " << othFileName << "\n";
				exit(1);
			}
			maxVal = tmpMaxVal;
		} else {
			if (!(inStream >> othData)) {
				std::cerr << "cannot read array from " << othFileName << "\n";
				exit(1);
			}
			maxVal = std::max(absVal(othData.min()),
			  absVal(othData.max()));
		}
	}
	int prec = pnmMaxValToPrec(ceil(maxVal));

	int width = othData.getWidth();
	int height = othData.getHeight();

	RealArray2 refData;
	{
		if (refFileName.size()) {
			std::ifstream inStream0;
			if (refFileName != "-") {
				inStream0.open(refFileName.c_str());
			}
			std::istream& inStream = ((refFileName == "-") ? std::cin :
			  inStream0);
			if (pnmFmt) {
				int tmpMaxVal;
				bool sgnd;
				if (decodePgm(inStream, refData, tmpMaxVal, sgnd)) {
					std::cerr << "cannot read image from " << refFileName << "\n";
					exit(1);
				}
				maxVal = tmpMaxVal;
			} else {
				if (!(inStream >> refData)) {
					std::cerr << "cannot read array from " << refFileName << "\n";
					exit(1);
				}
				maxVal = std::max(absVal(refData.min()),
				  absVal(refData.max()));
			}
		} else {
			std::cerr << "reference is zero\n";
			refData.resize(width, height);
			refData.fill(0);
		}
	}

	IntArray2 selMask(width, height);
	selMask.fill(1);
	if (borderSize > 0) {
		selectBorder(selMask, borderSize);
	}
	if (invert) {
		selMask *= -1;
		selMask += 1;
	}

	if (diffFileName.size()) {
		makeDiffImage(diffFileName, refData, othData, prec, flags);
	}

	Stats stats;
	calcStats(refData, othData, selMask, stats);

	std::cout.precision(16);

	if (verbose) {
		std::cout
		  << "skipped count: " << (width * height - stats.totalCount) << "\n"
		  << "total count: " << stats.totalCount << "\n"
		  << "zero error count: " << stats.zeroErrCount << "\n"
		  << "negative error count: " << stats.negErrCount << "\n"
		  << "positive error count: " << stats.posErrCount << "\n"
		  << "nonzero error count: " << stats.zeroErrCount << "\n"
		  << "minimum error: " << stats.minErr << "\n"
		  << "maximum error: " << stats.maxErr << "\n"
		  << "minimum absolute error: " << stats.minAbsErr << "\n"
		  << "maximum absolute error: " << stats.maxAbsErr << " at "
		  << "(" << stats.maxAbsErrPosX << ", " << stats.maxAbsErrPosY << ")"
		  << "\n"
		  << "mean squared error: " << stats.meanSqrErr << "\n"
		  << "root mean squared error: " << sqrt(stats.meanSqrErr) << "\n"
		  << "PSNR (dB): " << mseToPsnr(stats.meanSqrErr, maxVal) << "\n"
		  ;
	}

	if (metricName == "pae") {
		std::cout << stats.maxAbsErr << "\n";
	} else if (metricName == "mae") {
		std::cout << stats.meanAbsErr << "\n";
	} else if (metricName == "mse") {
		std::cout << stats.meanSqrErr << "\n";
	} else if (metricName == "rmse") {
		std::cout << sqrt(stats.meanSqrErr) << "\n";
	} else if (metricName == "psnr") {
		std::cout << mseToPsnr(stats.meanSqrErr, maxVal) << "\n";
	} else {
		std::cerr << "unknown error metric\n";
		usage();
	}

	return 0;
}

