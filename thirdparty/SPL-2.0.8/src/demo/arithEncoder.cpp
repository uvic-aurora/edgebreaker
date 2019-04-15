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

#include <SPL/config.hpp>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <SPL/bitStream.hpp>
#include <SPL/arithCoder.hpp>
#include <SPL/mCoder.hpp>
#include "arithCoder.hpp"

#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
typedef SPL::MultiArithEncoder MyArithEncoder;
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_BIN)
typedef SPL::BinArithEncoder MyArithEncoder;
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_M)
typedef SPL::MEncoder MyArithEncoder;
#endif

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	if (argc < 3) {
		std::cerr << "usage: " << argv[0] << " datFile numBlks blkSize fracOnes [seed]\n";
		exit(2);
	}
	std::basic_string<char> dataFile(argv[1]);
	int numBlks = atoi(argv[2]);
	int blkSize = atoi(argv[3]);
	double fracOnes = atof(argv[4]);
	int seed = getpid();

	if (argc >= 6) {
		seed = atoi(argv[5]);
	}

#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
std::cerr << "using MultiArithCoder\n";
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_BIN)
std::cerr << "using BinArithCoder\n";
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_M)
std::cerr << "using MCoder\n";
#endif

	std::fstream dataStream;
	dataStream.open(dataFile.c_str(), std::ios::out | std::ios::trunc);

#if defined(ARITHCODER_DEBUG)
	std::fstream debugStream;
	debugStream.open("/tmp/arithenc.log", std::ios::out | std::ios::trunc);
	debugStream.rdbuf()->pubsetbuf(0, 0);
	MyArithEncoder::setDebugLevel(1);
	MyArithEncoder::setDebugStream(debugStream);
#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI) || (ARITHCODER_METHOD == ARITHCODER_METHOD_BIN)
	ArithCoderModel::setDebugLevel(1);
	ArithCoderModel::setDebugStream(debugStream);
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_M)
#endif

#endif

#if 1
	std::cerr << "numBlks=" << numBlks << " blkSize=" << blkSize << "\n";
	std::cerr << "seed=" << seed << "\n";
#endif

	srand48(seed);

	SPL::OutputBitStream out(std::cout);
	out.putBits(numBlks, 16);
	out.putBits(blkSize, 16);

	for (int blkNo = 0; blkNo < numBlks; ++blkNo) {

		MyArithEncoder enc(1, &out);

#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
		enc.setContext(0, 5);
#endif
		enc.start();

		for (int i = 0; i < blkSize; ++i) {

			int b;
			b = (drand48() < fracOnes);
			dataStream << char('0' + b);
			dataStream.flush();

			int retVal;
			retVal = enc.encodeRegular(0, b);
			if (retVal) {
				std::cerr << "encodeRegular failed\n";
				abort();
			}
#if 0
			// The following is some code for testing.
			{
				MyArithEncoder::Freq oneFreq;
				MyArithEncoder::Freq zeroFreq;
				MyArithEncoder::Freq totalFreq;
				MyArithEncoder::Freq maxFreq;
				bool adaptive;
				std::cerr << "CODED SYMBOL " << b << "\n";
				enc.dump(std::cerr);
				enc.dumpModels(std::cerr);
				enc.getContextState(0, oneFreq, totalFreq, maxFreq, adaptive);
				zeroFreq = totalFreq - oneFreq;
				std::cerr << "context_state " << zeroFreq << "/" << totalFreq << " " << oneFreq << "/" << totalFreq << " " << maxFreq << " " << adaptive << "\n";
			}
#endif

#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
			retVal = enc.encodeBypass(5, b);
#else
			retVal = enc.encodeBypass(b);
#endif

			if (retVal) {
				std::cerr << "encodeBypass failed\n";
				abort();
			}

#if defined(ARITHCODER_DEBUG)
			debugStream << "AFTER ENCODE " << b << "\n";
			enc.dump(debugStream);
#endif

		}

		if (enc.terminate()) {
			std::cerr << "encodeTerminate failed\n";
			abort();
		}
#if defined(ARITHCODER_DEBUG)
		debugStream << "AFTER ENCODE TERMINATE\n";
		enc.dump(debugStream);
#endif

		dataStream << "\n";
		dataStream.flush();

		// NOTE: Byte-alignment is not necessary.
		// out.flush();

	}

	return 0;
}

