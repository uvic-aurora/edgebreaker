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

/*!
 * @file pnmCodec.hpp
 * @brief This file contains a PNM codec.
 */

#ifndef SPL_pnmCodec_hpp
#define SPL_pnmCodec_hpp

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

//! The signature values that can appear at the start of the header.
enum PnmMagic {
	pnmMagicInvalid = 0,
	pnmMagicTxtPbm = 0x5031,
	pnmMagicTxtPgm = 0x5032,
	pnmMagicTxtPpm = 0x5033,
	pnmMagicBinPbm = 0x5034,
	pnmMagicBinPgm = 0x5035,
	pnmMagicBinPpm = 0x5036,
	pnmMagicPam = 0x5037
};

//! The type of the PNM data.
enum PnmType {
	pnmTypeInvalid = 0,
	pnmTypePbm,
	pnmTypePgm,
	pnmTypePpm
};

//! The format of the PNM data (i.e., binary or text).
enum PnmFmt {
	pnmFmtInvalid = 0,
	pnmFmtTxt,
	pnmFmtBin
};

//! The maximum line length to be produced when encoding in text format.
const int pnmMaxLineLen = 80;

//! The header information for PNM data.
struct PnmHeader
{
	//! The magic number.
	PnmMagic magic;

	//! The image width.
	int width;

	//! The image height.
	int height;

	//! The maximum sample value.
	int maxVal;

	//! The signedness of the sample data.
	bool sgnd;
};

/*!
 * @brief Determine the type (i.e., PGM or PPM) from the magic number.
 */
PnmType pnmGetType(PnmMagic magic);

/*!
 * @brief Determine the format (i.e., text or binary) from magic number.
 */
PnmFmt pnmGetFmt(PnmMagic magic);

/*!
 * @brief Determine the precision from the maximum value.
 */
int pnmMaxValToPrec(int maxVal);

/*!
 * @brief Get the number of components from the PNM type.
 */
int pnmGetNumComps(PnmType type);

int pnmMinVal(bool sgnd, int maxVal);
int pnmMaxVal(bool sgnd, int maxVal);

/*!
 * @brief Get an integer whose representation in binary consists of
 * the specified number of ones.
 */
inline long pnmOnes(int n)
{
	return (1UL << n) - 1;
}

template <class T>
T pnmClipVal(T x, T minVal, T maxVal)
{
	assert(minVal <= maxVal);
	if (x < minVal) {
		x = minVal;
	} else if (x > maxVal) {
		x = maxVal;
	}
	return x;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Read a PNM header from the specified stream.
 */
int pnmGetHeader(std::istream& in, PnmHeader& header);

/*!
 * @brief Write a PNM header to the specified stream.
 */
int pnmPutHeader(std::ostream& out, PnmHeader& header);

/*!
 * @brief Read a character from the specified stream.
 */
int pnmGetChar(std::istream& in);

/*!
 * @brief Read a bit from the specified stream.
 */
int pnmGetTxtBit(std::istream& in);

/*!
 * @brief Read an integer from the specified stream.
 */
long pnmGetTxtInt(std::istream& in, bool sgnd, int& status);

/*!
 * @brief Read an integer from the specified stream.
 */
long pnmGetBinInt(std::istream& in, int wordSize, bool sgnd, int& status);

/*!
 * @brief Write an integer from the specified stream.
 */
int pnmPutBinInt(std::ostream& out, int wordSize, bool sgnd, long val);

////////////////////////////////////////////////////////////////////////////////
// Encoder
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Write data encoded in the PNM format to the specified stream.
 */
template <class GetData>
int pnmEncode(std::ostream& outStream, int width, int height, int numComps,
  int maxVal, bool sgnd, GetData& getData, bool binaryFormat)
{
	PnmHeader header;
	if (numComps == 1) {
		if (maxVal == 1) {
			header.magic = binaryFormat ? pnmMagicBinPbm : pnmMagicTxtPbm;
		} else {
			header.magic = binaryFormat ? pnmMagicBinPgm : pnmMagicTxtPgm;
		}
	} else if (numComps == 3) {
		header.magic = binaryFormat ? pnmMagicBinPpm : pnmMagicTxtPpm;
	} else {
		return -1;
	}
	header.width = width;
	header.height = height;
	header.maxVal = maxVal;
	header.sgnd = false;

	if (pnmPutHeader(outStream, header)) {
		return -1;
	}
	if (putData(outStream, header, getData)) {
		return -1;
	}
	return 0;
}

/*!
 * @brief Write the actual image data to a stream.
 */
template <class GetData>
int putData(std::ostream& out, PnmHeader& header, GetData& getData)
{
	int prec = pnmMaxValToPrec(header.maxVal);
	int fmt = pnmGetFmt(header.magic);
	PnmType type = pnmGetType(header.magic);
	int numComps = pnmGetNumComps(type);

	int lineLen = 0;
	for (int y = header.height - 1; y >= 0; --y) {
		if (fmt == pnmFmtBin) {
			if (type == pnmTypePbm) {
				int val = 0;
				int numBits = 0;
				for (int x = 0; x < header.width; ++x) {
					long b = getData();
					val = (val << 1) | (b & 1);
					++numBits;
					if (numBits >= 8) {
						if (!out.put(static_cast<unsigned char>(val))) {
							return -1;
						}
						val = 0;
						numBits = 0;
					}
				}
				if (numBits > 0) {
					assert(numBits <= 8);
					val <<= 8 - numBits;
					if (!out.put(static_cast<unsigned char>(val))) {
						return -1;
					}
				}
			} else {
				for (int x = 0; x < header.width; ++x) {
					for (int c = 0; c < numComps; ++c) {
						//long val = img.get(c, x, y);
						long val = getData();
						if (pnmPutBinInt(out, prec, header.sgnd, val)) {
							return -1;
						}
					}
				}
			}
		} else {
			for (int x = 0; x < header.width; ++x) {
				for (int c = 0; c < numComps; ++c) {
					//long val = img.get(c, x, y);
					long val = getData();
					std::ostringstream buf;
					buf << val;
					int n = buf.str().length();
					if (lineLen && lineLen + n + 2 > pnmMaxLineLen) {
						out << "\n";
						lineLen = 0;
					}
					if (lineLen && prec > 1) {
						out << " ";
						++lineLen;
					}
					out << buf.str();
					lineLen += n;
				}
			}
		}
		if (fmt == pnmFmtTxt && lineLen) {
			out << "\n";
			lineLen = 0;
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Decoder
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Read data encoded in the PNM format from the specified stream.
 */
template <class Initialize>
int pnmDecode(std::istream& inStream, Initialize& initialize)
{
	PnmHeader header;
	if (pnmGetHeader(inStream, header)) {
		return -1;
	}
#if defined(PNM_DEBUG)
	std::cerr
	  << "magic " << std::hex << header.magic << " "
	  << std::dec << " width " << header.width << " "
	  << "height " << header.height << " "
	  << "maxVal " << header.maxVal << "\n"
	  ;
#endif
	int numComps = pnmGetNumComps(pnmGetType(header.magic));
	typename Initialize::PutData putData;
	initialize(header.width, header.height, numComps, header.maxVal,
	  header.sgnd, putData);
	if (getData(inStream, header, putData)) {
		return -1;
	}
	return 0;
}

/*!
 * @brief Read the actual image data from the specified stream.
 */
template <class PutData>
int getData(std::istream& in, PnmHeader& header, PutData& putData)
{
	PnmType type = pnmGetType(header.magic);
	PnmFmt fmt = pnmGetFmt(header.magic);
	int numComps = pnmGetNumComps(type);
	int prec = pnmMaxValToPrec(header.maxVal);
#if 1
	int minVal = pnmMinVal(header.sgnd, header.maxVal);
	int maxVal = pnmMaxVal(header.sgnd, header.maxVal);
#endif
	for (int y = header.height - 1; y >= 0; --y) {
		if (type == pnmTypePbm) {
			if (fmt == pnmFmtBin) {
				for (int x = 0; x < header.width;) {
					int c;
					if ((c = in.get()) == std::char_traits<char>::eof()) {
						return -1;
					}
					int n = 8;
					while (n > 0 && x < header.width) {
						//img.set(0, x, y, (c >> 7) & 1);
						putData((c >> 7) & 1);
						c <<= 1;
						--n;
						++x;
					}
				}
			} else {
				for (int x = 0; x < header.width; ++x) {
					long val;
					if ((val = pnmGetTxtBit(in)) < 0) {
						return -1;
					}
					//img.set(0, x, y, val);
					putData(val);
				}
			}
		} else {
			if (fmt == pnmFmtBin) {
				for (int x = 0; x < header.width; ++x) {
					for (int c = 0; c < numComps; ++c) {
						long val;
						int status;
						if (val = pnmGetBinInt(in, prec, header.sgnd, status),
						  status) {
							return -1;
						}
						// NOTE: Should I add range clipping here?
#if 0
						if (val < minVal || val > maxVal) {
							std::cerr << "warning: clipping out of range sample value\n";
						}
						val = pnmClipVal<int>(val, minVal, maxVal);
#else
						assert(val >= minVal && val <= maxVal);
#endif
						//img.set(c, x, y, val);
						putData(val);
					}
				}
			} else {
				for (int x = 0; x < header.width; ++x) {
					for (int c = 0; c < numComps; ++c) {
						long val;
						int status;
						if (val = pnmGetTxtInt(in, header.sgnd, status), status) {
							return -1;
						}
						// NOTE: Should I add range clipping here?
#if 0
						if (val < minVal || val > maxVal) {
							std::cerr << "warning: clipping out of range sample value\n";
						}
						val = pnmClipVal<int>(val, minVal, maxVal);
#else
						assert(val >= minVal && val <= maxVal);
#endif
						//img.set(c, x, y, val);
						putData(val);
					}
				}
			}
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

}

#endif
