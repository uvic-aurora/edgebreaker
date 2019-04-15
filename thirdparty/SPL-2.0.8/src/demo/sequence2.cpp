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
#include <sstream>
#include <algorithm>
#include <iterator>
#include <SPL/Sequence2.hpp>

typedef SPL::Sequence2<double> RealSeq2;
typedef SPL::Sequence1<double> RealSeq1;
typedef SPL::Array2<double> RealArray2;

int doDownsample(std::istream& in, std::ostream& out, bool checkResult)
{
	RealSeq2 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	int factorX;
	int factorY;
	if (!(in >> factorX)) {
		std::cerr << "cannot factorX\n";
		return -1;
	};
	if (!(in >> factorY)) {
		std::cerr << "cannot factorY\n";
		return -1;
	}
	out << "input: " << seq;
	out << "factors: " << factorX << " " << factorY << "\n";
	RealSeq2 result = SPL::downsample(seq, factorX, factorY);
	result.output(out, 4) << "\n";

	if (checkResult) {
		RealSeq2 result1;
		in >> result1;
		if (result1 != result) {
			std::cerr << "wrong answer\n";
			return -1;
		} else {
			std::cerr << "correct answer\n";
		}
	}
	return 0;
}

int doUpsample(std::istream& in, std::ostream& out, bool checkResult)
{
	RealSeq2 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	int factorX;
	int factorY;
	int padX;
	int padY;
	if (!(in >> factorX)) {
		std::cerr << "cannot get factorX\n";
		return -1;
	};
	if (!(in >> factorY)) {
		std::cerr << "cannot get factorY\n";
		return -1;
	}
	if (!(in >> padX)) {
		std::cerr << "cannot get padX\n";
		return -1;
	};
	if (!(in >> padY)) {
		std::cerr << "cannot get padY\n";
		return -1;
	};
	out << "input: " << seq;
	out << "factors: " << factorX << " " << factorY << "\n";
	out << "pads: " << padX << " " << padY << "\n";
	RealSeq2 result = SPL::upsample(seq, factorX, factorY, padX, padY);
	result.output(out, 4) << "\n";
	if (checkResult) {
		return -1;
	}
	return 0;
}

int doPolyphaseSplit(std::istream& in, std::ostream& out, bool checkResult)
{
	RealSeq2 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	int typeX;
	int typeY;
	int numPhasesX;
	int numPhasesY;
	if (!(in >> typeX)) {
		std::cerr << "cannot get typeX\n";
		return -1;
	};
	if (!(in >> typeY)) {
		std::cerr << "cannot get typeY\n";
		return -1;
	}
	if (!(in >> numPhasesX)) {
		std::cerr << "cannot get numPhasesX\n";
		return -1;
	};
	if (!(in >> numPhasesY)) {
		std::cerr << "cannot get numPhasesY\n";
		return -1;
	};
	out << "input: " << seq;
	out << "types: " << typeX << " " << typeY << "\n";
	out << "phases: " << numPhasesX << " " << numPhasesY << "\n";
	SPL::Array2<RealSeq2> result = SPL::polyphaseSplit(seq, typeX, numPhasesX,
	  typeY, numPhasesY);
	out << result << "\n";

	RealSeq2 result2 = SPL::polyphaseJoin(result, typeX, typeY);
	out << result2 << "\n";
	assert(result2 == seq);
	if (checkResult) {
		return -1;
	}
	return 0;
}

int doSubsequence(std::istream& in, std::ostream& out, bool checkResult)
{
	RealSeq2 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	int startX;
	int startY;
	int width;
	int height;
	if (!(in >> startX)) {
		std::cerr << "cannot get typeX\n";
		return -1;
	};
	if (!(in >> startY)) {
		std::cerr << "cannot get typeX\n";
		return -1;
	};
	if (!(in >> width)) {
		std::cerr << "cannot get typeX\n";
		return -1;
	};
	if (!(in >> height)) {
		std::cerr << "cannot get typeX\n";
		return -1;
	};
	out << "input: " << seq;
	out << "start: " << startX << " " << startY << "\n";
	out << "size: " << width << " " << height << "\n";
	RealSeq2 result = SPL::subsequence(seq, startX, startY, width, height);
	out << result << "\n";
	if (checkResult) {
		return -1;
	}
	return 0;
}

int doTranslate(std::istream& in, std::ostream& out, bool checkResult)
{
	RealSeq2 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	int deltaX;
	int deltaY;
	if (!(in >> deltaX)) {
		std::cerr << "cannot get deltaX\n";
		return -1;
	};
	if (!(in >> deltaY)) {
		std::cerr << "cannot get deltaY\n";
		return -1;
	};
	out << "input: " << seq;
	out << "delta: " << deltaX << " " << deltaY << "\n";
	RealSeq2 result = SPL::translate(seq, deltaX, deltaY);
	out << result << "\n";
	if (checkResult) {
		return -1;
	}
	return 0;
}

int doConvolveSeparable(std::istream& in, std::ostream& out, bool checkResult)
{
	bool verbose = !checkResult;
	verbose = true;
	RealSeq2 seq;
	if (!(in >> seq)) {
		std::cerr << "cannot get sequence\n";
		return -1;
	}
	RealSeq1 horzFilt;
	if (!(in >> horzFilt)) {
		std::cerr << "cannot get horizontal filter\n";
		return -1;
	}
	RealSeq1 vertFilt;
	if (!(in >> vertFilt)) {
		std::cerr << "cannot get vertical filter\n";
		return -1;
	}
	int convMode;
	if (!(in >> convMode)) {
		std::cerr << "cannot get convolution mode\n";
		return -1;
	};
	if (verbose) {
		out << "input: " << seq;
		out << "horz filter: " << horzFilt << "\n";
		out << "vert filter: " << vertFilt << "\n";
		out << "mode: " << convMode << "\n";
	}
	RealSeq2 result = SPL::convolveSeparable(seq, horzFilt, vertFilt, convMode);
	if (verbose) {
		out << result << "\n";
	}
	if (checkResult) {
		RealSeq2 result1;
		if (!(in >> result1)) {
			std::cerr << "cannot get result\n";
			return -1;
		}
		if (result != result1) {
			std::cerr << "wrong result\n";
			return -1;
		} else {
			std::cerr << "correct result\n";
		}
	}
	return 0;
}

int preprocess(std::istream& in, std::stringstream& out)
{
	bool inComment = false;
	for (;;) {
		char c;
		in.get(c);
		if (!in) {
			return 0;
		}
		if (inComment) {
			if (c == '\n') {
				out.put(c);
				inComment = false;
			}
		} else {
			if (c == '/') {
				in.get(c);
				if (!in) {
					return -1;
				}
				if (c == '/') {
					inComment = true;
				} else {
					out.put('/');
					out.put(c);
				}
			} else {
				out.put(c);
			}
		}
	}
	return 0;
}

int main(int argc, char** argv)
{

	bool checkResult = true;

	std::stringstream dataStream;
	preprocess(std::cin, dataStream);
	dataStream.seekp(0, std::ios_base::beg);

	for (;;) {
		int result;

		std::string opName;

		if (!(dataStream >> opName)) {
			break;
		}

		std::cout << "operation: " << opName << "\n";

		if (opName == "end") {
			break;
		} else if (opName == "downsample") {
			result = doDownsample(dataStream, std::cout, checkResult);
		} else if (opName == "upsample") {
			result = doUpsample(dataStream, std::cout, checkResult);
		} else if (opName == "polyphaseSplit") {
			result = doPolyphaseSplit(dataStream, std::cout, checkResult);
		} else if (opName == "subsequence") {
			result = doSubsequence(dataStream, std::cout, checkResult);
		} else if (opName == "translate") {
			result = doTranslate(dataStream, std::cout, checkResult);
		} else if (opName == "convolveSeparable") {
			result = doConvolveSeparable(dataStream, std::cout, checkResult);
		} else {
			std::cerr << "unknown operation\n";
			result = -1;
		}

		if (result) {
			std::cerr << "test case failed\n";
			exit(1);
			break;
		}

	}

	return 0;
}
