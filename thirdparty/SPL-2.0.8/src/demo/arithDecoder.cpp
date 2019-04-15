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
#include <cassert>
#include <cstdlib>
#include <SPL/bitStream.hpp>
#include <SPL/mCoder.hpp>
#include <SPL/arithCoder.hpp>
#include "arithCoder.hpp"

#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
typedef SPL::MultiArithDecoder MyArithDecoder;
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_BIN)
typedef SPL::BinArithDecoder MyArithDecoder;
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_M)
typedef SPL::MDecoder MyArithDecoder;
#endif

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	SPL::InputBitStream in(std::cin);

	int numBlks = in.getBits(16);
	int blkSize = in.getBits(16);

#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
std::cerr << "using MultiArithCoder\n";
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_BIN)
std::cerr << "using BinArithCoder\n";
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_M)
std::cerr << "using MCoder\n";
#endif

#if defined(ARITHCODER_DEBUG)
	fstream debugStream;
	debugStream.open("/tmp/arithdec.log", ios::out | ios::trunc);
	debugStream.rdbuf()->pubsetbuf(0, 0);
	MyArithDecoder::setDebugLevel(1);
	MyArithDecoder::setDebugStream(debugStream);
#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI) || (ARITHCODER_METHOD == ARITHCODER_METHOD_BIN)
	ArithCoderModel::setDebugLevel(1);
	ArithCoderModel::setDebugStream(debugStream);
#elif (ARITHCODER_METHOD == ARITHCODER_METHOD_M)
#endif
#endif

	for (int blkNo = 0; blkNo < numBlks; ++blkNo) {

		MyArithDecoder dec(1, &in);
#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
		dec.setContext(0, 5);
#endif

		dec.start();

		for (int i = 0; i < blkSize; ++i) {

			int b;
			if ((b = dec.decodeRegular(0)) < 0) {
				std::cerr << "decodeRegular failed " << i << "\n";
				abort();
			}

			int retVal;
#if (ARITHCODER_METHOD == ARITHCODER_METHOD_MULTI)
			retVal = dec.decodeBypass(5);
#else
			retVal = dec.decodeBypass();
#endif
			if (retVal < 0) {
				std::cerr << "decodeBypass failed\n";
				abort();
			}

#if defined(ARITHCODER_DEBUG)
			debugStream << "AFTER DECODE " << b << "\n";
			dec.dump(debugStream);
			debugStream.flush();
#endif
			std::cout << char(b + '0');
		}
		if (dec.terminate() < 0) {
			std::cerr << "decodeTerminate failed\n";
			abort();
		}
#if defined(ARITHCODER_DEBUG)
		debugStream << "AFTER DECODE TERMINATE\n";
		dec.dump(debugStream);
		debugStream.flush();
#endif
		std::cout << "\n";

		// NOTE: Byte-alignment is not necessary.
		//in.flush();

	}

}

