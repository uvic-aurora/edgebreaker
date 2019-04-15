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
 * @file pnmCodec.cpp
 * @brief This file contains a PNM codec.
 */

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <SPL/pnmCodec.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// Encoder
////////////////////////////////////////////////////////////////////////////////

int pnmPutHeader(std::ostream& out, PnmHeader& header)
{
	if (pnmPutBinInt(out, 16, false, header.magic)) {
		return -1;
	}
	long maxVal = header.sgnd ? (-header.maxVal) : header.maxVal;

	out << "\n" << header.width << " " << header.height << "\n";
	if (pnmGetType(header.magic) != pnmTypePbm) {
		out << maxVal << "\n";
	}

	return !out.good();
}

int pnmPutBinInt(std::ostream& out, int wordSize, bool sgnd, long val)
{
	long tmp = val;
	if (sgnd && tmp < 0) {
		tmp = ((~(-tmp)) + 1) & pnmOnes(wordSize); 
	}
	int n = (wordSize + 7) / 8;
	tmp <<= 8 * (4 - n);
	while (--n >= 0) {
		int c = (tmp >> 24) & 0xff;
		out.put(c);
		if (!out.good()) {
			return -1;
		}
		tmp = (tmp << 8) & 0xffffffff;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Decoder
////////////////////////////////////////////////////////////////////////////////

int pnmGetHeader(std::istream& in, PnmHeader& header)
{
	int status;

	if ((header.magic = static_cast<PnmMagic>(pnmGetBinInt(in, 16, false, status)),
	  status) || (header.width = pnmGetTxtInt(in, false, status), status) ||
	  (header.height = pnmGetTxtInt(in, false, status), status)) {
		return -1;
	}
	if (pnmGetType(header.magic) != pnmTypePbm) {
		if ((header.maxVal = pnmGetTxtInt(in, true, status), status)) {
			return -1;
		}
		if (header.maxVal < 0) {
			header.maxVal = -header.maxVal;
			header.sgnd = true;
		} else {
			header.sgnd = false;
		}
	} else {
		header.maxVal = 1;
		header.sgnd = false;
	}

	return 0;
}

int pnmGetTxtBit(std::istream& in)
{
	int c;
	for (;;) {
		if ((c = in.get()) == std::char_traits<char>::eof()) {
			return -1;
		}
		if (c == '#') {
			// Discard characters until the end of line.
			do {
				if ((c = in.get()) == std::char_traits<char>::eof()) {
					return -1;
				}
			} while (c != '\n' && c != '\r');
		} else if (c == '0' || c == '1') {
			int val = c - '0';
			return val;
		}
	}
}

long pnmGetTxtInt(std::istream& in, bool sgnd, int& status)
{

	status = -1;
	int c;

#if 0
	// Discard any leading whitespace.
	do {
		if ((c = in.get()) == std::char_traits<char>::eof()) {
			return 0;
		}
	} while (isspace(c));
#endif
	do {
		if ((c = pnmGetChar(in)) == std::char_traits<char>::eof()) {
			return 0;
		}
	} while (isspace(c));

	// Get the number, allowing for a negative sign.
	int s = 0;
	if (sgnd) {
		if (c == '-') {
			s = 1;
			if ((c = in.get()) == std::char_traits<char>::eof()) {
				return 0;
			}
		} else if (c == '+') {
			if ((c = in.get()) == std::char_traits<char>::eof()) {
				return 0;
			}
		}
	}
	bool valid = false;
	long val = 0;
	while (isdigit(c)) {
		valid = true;
		val = 10 * val + c - '0';
		if ((c = in.get()) == std::char_traits<char>::eof()) {
			return 0;
		}
	}

	// The number must have at least one digit.
	if (!valid) {
		return 0;
	}

	// The number must be followed by whitespace.
	if (!isspace(c)) {
		return 0;
	}

	if (s) {
		val = -val;
	}

	status = 0;
	return val;
}

int pnmGetChar(std::istream& in)
{
	int c;
	for (;;) {
		if ((c = in.get()) == std::char_traits<char>::eof()) {
			return -1;
		}
		if (c == '#') {
			// Gobble characters until the end of line.
			do {
				if ((c = in.get()) == std::char_traits<char>::eof()) {
					return -1;
				}
			} while (c != '\n' && c != '\r');
		} else {
			return c;
		}
	}
}

long pnmGetBinInt(std::istream& in, int wordSize, bool sgnd, int& status)
{
	status = -1;

	int c;
	long val = 0;
	int n = (wordSize + 7) / 8;
	while (--n >= 0) {
		if ((c = in.get()) == std::char_traits<char>::eof()) {
			return 0;
		}
		val = (val << 8) | (c & 0xff);
	}
	val &= pnmOnes(wordSize);
	if (sgnd && (val & (1 << (wordSize - 1)))) {
		val = -(((~val) + 1) & pnmOnes(wordSize));
	}
	status = 0;
	assert(sgnd || (!sgnd && val >= 0));
	return val;

}

////////////////////////////////////////////////////////////////////////////////
// Common Encoder/Decoder
////////////////////////////////////////////////////////////////////////////////

PnmType pnmGetType(PnmMagic magic)
{
	PnmType type;
	switch (magic) {
	case pnmMagicTxtPpm:
	case pnmMagicBinPpm:
		type = pnmTypePpm;
		break;
	case pnmMagicTxtPgm:
	case pnmMagicBinPgm:
		type = pnmTypePgm;
		break;
	case pnmMagicTxtPbm:
	case pnmMagicBinPbm:
		type = pnmTypePbm;
		break;
	default:
		abort();
		break;
	}
	return type;
}

// Determine the PNM format (i.e., text or binary) from the magic number.
PnmFmt pnmGetFmt(PnmMagic magic)
{
	PnmFmt fmt;
	switch (magic) {
	case pnmMagicTxtPbm:
	case pnmMagicTxtPgm:
	case pnmMagicTxtPpm:
		fmt = pnmFmtTxt;
		break;
	case pnmMagicBinPbm:
	case pnmMagicBinPgm:
	case pnmMagicBinPpm:
		fmt = pnmFmtBin;
		break;
	default:
		abort();
		break;
	}
	return fmt;
}

int pnmGetNumComps(PnmType type)
{
	if (type == pnmTypePbm || type == pnmTypePgm) {
		return 1;
	} else {
		return 3;
	}
}

/* Determine the depth (i.e., precision) from the maximum value. */
int pnmMaxValToPrec(int maxVal)
{
	int n = 0;
	while (maxVal > 0) {
		maxVal >>= 1;
		++n;
	}
	return n;
}

int pnmMinVal(bool sgnd, int maxVal)
{
	return sgnd ? (-maxVal) : 0;
}

int pnmMaxVal(bool sgnd, int maxVal)
{
	return maxVal;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

}

