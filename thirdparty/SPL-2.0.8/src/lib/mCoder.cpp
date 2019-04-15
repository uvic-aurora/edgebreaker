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

////////////////////////////////////////////////////////////////////////////////
// Header Files
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <cassert>
#include <SPL/bitStream.hpp>
#include <SPL/mCoder.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// Common Encoder/Decoder Code
////////////////////////////////////////////////////////////////////////////////

const unsigned char MCoder::rangeTabLPS[64][4] = {
	{128, 176, 208, 240},
	{128, 167, 197, 227},
	{128, 158, 187, 216},
	{123, 150, 178, 205},
	{116, 142, 169, 195},
	{111, 135, 160, 185},
	{105, 128, 152, 175},
	{100, 122, 144, 166},
	{95, 116, 137, 158},
	{90, 110, 130, 150},
	{85, 104, 123, 142},
	{81, 99, 117, 135},
	{77, 94, 111, 128},
	{73, 89, 105, 122},
	{69, 85, 100, 116},
	{66, 80, 95, 110},
	{62, 76, 90, 104},
	{59, 72, 86, 99},
	{56, 69, 81, 94},
	{53, 65, 77, 89},
	{51, 62, 73, 85},
	{48, 59, 69, 80},
	{46, 56, 66, 76},
	{43, 53, 63, 72},
	{41, 50, 59, 69},
	{39, 48, 56, 65},
	{37, 45, 54, 62},
	{35, 43, 51, 59},
	{33, 41, 48, 56},
	{32, 39, 46, 53},
	{30, 37, 43, 50},
	{29, 35, 41, 48},
	{27, 33, 39, 45},
	{26, 31, 37, 43},
	{24, 30, 35, 41},
	{23, 28, 33, 39},
	{22, 27, 32, 37},
	{21, 26, 30, 35},
	{20, 24, 29, 33},
	{19, 23, 27, 31},
	{18, 22, 26, 30},
	{17, 21, 25, 28},
	{16, 20, 23, 27},
	{15, 19, 22, 25},
	{14, 18, 21, 24},
	{14, 17, 20, 23},
	{13, 16, 19, 22},
	{12, 15, 18, 21},
	{12, 14, 17, 20},
	{11, 14, 16, 19},
	{11, 13, 15, 18},
	{10, 12, 15, 17},
	{10, 12, 14, 16},
	{9, 11, 13 ,15},
	{9, 11, 12, 14},
	{8, 10, 12, 14},
	{8, 9, 11, 13},
	{7, 9, 11, 12},
	{7, 9, 10, 12},
	{7, 8, 10, 11},
	{6, 8, 9, 11},
	{6, 7, 9, 10},
	{6, 7, 8, 9},
	{2, 2, 2, 2}
};

const unsigned char MCoder::transIdxLPS[64] = {
	0, 0, 1, 2, 2, 4, 4, 5, 6, 7, 8, 9, 9, 11, 11, 12,
	13, 13, 15, 15, 16, 16, 18, 18, 19, 19, 21, 21, 22, 22, 23, 24,
	24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 30, 31, 32, 32, 33,
	33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 63
};

const unsigned char MCoder::transIdxMPS[64] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63
};


////////////////////////////////////////////////////////////
// Inline functions.
////////////////////////////////////////////////////////////

inline int MEncoder::writeBits(int v, int n)
{
	if (debugLevel_ > 0) {
		getDebugStream() << "writeBits(" << std::hex << v << std::dec
		  << "," << n << ")\n";
	}
	if (out_->putBits(v, n) < 0) {
		return -1;
	}
	return 0;
}

////////////////////////////////////////////////////////////
// Constructors and destructors
////////////////////////////////////////////////////////////

MEncoder::MEncoder(int numContexts, OutputBitStream* out) :
  contexts_(numContexts)
{
	out_ = out;
	setNumContexts(numContexts);
	clearContexts();
	firstBitFlag_ = 0;
	codILow_ = 0;
	codIRange_ = 0;
	bitsOutstanding_ = 0;
	symCount_ = 0;
	bitCount_ = 0;
	initialized_ = false;
}

MEncoder::~MEncoder()
{
}

////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////

void MEncoder::setNumContexts(int numContexts) {
	if (contexts_.size() != numContexts) {
		contexts_.resize(numContexts);
		clearContexts();
	}
}

void MEncoder::clearContexts() {
	for (std::vector<Context>::iterator i = contexts_.begin();
	  i != contexts_.end(); ++i) {
		i->valMPS_ = 0;
		i->pStateIdx_ = 0;
	}
}

void MEncoder::start()
{
	codILow_ = 0;
	codIRange_ = 0x1fe;
	firstBitFlag_ = 1;
	bitsOutstanding_ = 0;
	symCount_ = 0;
	bitCount_ = 0;
	initialized_ = true;
}

////////////////////////////////////////////////////////////
// Encoding
////////////////////////////////////////////////////////////

int MEncoder::encodeRegular(int contextId, int binVal)
{
	assert(initialized_);
#if defined(SPL_MCoder_BypassAll)
	return encodeBypass(binVal);
#endif
	assert(contextId >= 0 && contextId < contexts_.size());
	if (debugLevel_ > 0) {
		getDebugStream() << "encodeRegular(" << contextId << "," <<
		  binVal << ")\n";
	}
	Context* context = &contexts_[contextId];
	unsigned short qCodIRangeIdx = (codIRange_ >> 6) & 3;
	unsigned short codIRangeLPS =
	  rangeTabLPS[context->pStateIdx_][qCodIRangeIdx];
	codIRange_ -= codIRangeLPS;
	if (binVal != context->valMPS_) {
		codILow_ += codIRange_;
		codIRange_ = codIRangeLPS;
		if (context->pStateIdx_ == 0) {
			context->valMPS_ ^= 1;
		}
		context->pStateIdx_ = transIdxLPS[context->pStateIdx_];
	} else {
		context->pStateIdx_ = transIdxMPS[context->pStateIdx_];
	}
	if (renormE()) {
		return -1;
	}
	++symCount_;
	return 0;
}

int MEncoder::encodeBypass(int binVal)
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "encodeBypass(" << binVal << ")\n";
	}
	codILow_ <<= 1;
	if (binVal != 0) {
		codILow_ += codIRange_;
	}
	if (codILow_ >= 0x400) {
		if (putBit(1)) {
			return -1;
		}
		codILow_ -= 0x400;
	} else {
		if (codILow_ < 0x200) {
			if (putBit(0)) {
				return -1;
			}
		} else {
			codILow_ -= 0x200;
			++bitsOutstanding_;
		}
	}
	++symCount_;
	return 0;
}

////////////////////////////////////////////////////////////
// Termination
////////////////////////////////////////////////////////////

int MEncoder::terminate()
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "terminate\n";
	}
	return encodeTerminate(1);
}

int MEncoder::encodeTerminate(int binVal)
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "encodeTerminate(" << binVal << ")\n";
	}
	codIRange_ -= 2;
	if (binVal != 0) {
		codILow_ += codIRange_;
		if (encodeFlush()) {
			return -1;
		}
	} else {
		if (renormE()) {
			return -1;
		}
	}
	++symCount_;
	return 0;
}

int MEncoder::encodeFlush()
{
	assert(initialized_);
	codIRange_ = 2;
	if (renormE()) {
		return -1;
	}
	if (putBit((codILow_ >> 9) & 1)) {
		return -1;
	}
	if (writeBits(((codILow_ >> 7) & 3) | 1, 2)) {
		return -1;
	}
	return 0;
}

////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////

int MEncoder::renormE()
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "renormE\n";
	}
	while (codIRange_ < 0x100) {
		if (codILow_ < 0x100) {
			if (putBit(0)) {
				return -1;
			}
		} else {
			if (codILow_ >= 0x200) {
				codILow_ -= 0x200;
				if (putBit(1)) {
					return -1;
				}
			} else {
				codILow_ -= 0x100;
				++bitsOutstanding_;
			}
		}
		codIRange_ <<= 1;
		codILow_ <<= 1;
	}
	return 0;
}

int MEncoder::putBit(int b)
{
	assert(initialized_);
	if (firstBitFlag_) {
		firstBitFlag_ = 0;
	} else {
		if (writeBits(b, 1)) {
			return -1;
		}
		++bitCount_;
	}
	while (bitsOutstanding_ > 0) {
		if (writeBits(b ^ 1, 1)) {
			return -1;
		}
		--bitsOutstanding_;
		++bitCount_;
	}
	return 0;
}

////////////////////////////////////////////////////////////
// Debugging support
////////////////////////////////////////////////////////////

int MEncoder::debugLevel_ = 0;
std::ostream* MEncoder::debugStream_ = 0;

void MEncoder::dump(std::ostream& out) const
{
	out << "codILow=" << std::hex << codILow_ << " codIRange=" << codIRange_ << "\n";
	out << "numContexts=" << contexts_.size() << "\n";
	for (int i = 0; i < contexts_.size(); ++i) {
		out << "contextId=" << i << " pStateIdx=" <<
		  int(contexts_[i].pStateIdx_) <<
		  " valMPS=" << int(contexts_[i].valMPS_) << std::dec << "\n";
	}
}

void MEncoder::setDebugLevel(int debugLevel)
{
	debugLevel_ = debugLevel;
}

void MEncoder::setDebugStream(std::ostream& debugStream)
{
	debugStream_ = &debugStream;
}

std::ostream& MEncoder::getDebugStream()
{
	return debugStream_ ? (*debugStream_) : std::cerr;
}

////////////////////////////////////////////////////////////////////////////////
// M-Coder Decoder Code
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Inline Functions
////////////////////////////////////////////////////////////

inline int MDecoder::readBits(int n)
{
	long d;
	if ((d = in_->getBits(n)) < 0) {
		return -1;
	}
	bitCount_ += n;
	return d;
}

////////////////////////////////////////////////////////////
// Constructors and destructors
////////////////////////////////////////////////////////////

MDecoder::MDecoder(int numContexts, InputBitStream* in) : contexts_(numContexts)
{
	in_ = in;
	codIRange_ = 0;
	codIOffset_ = 0;
	bitCount_ = 0;
	symCount_ = 0;
	clearContexts();
	initialized_ = false;
}

MDecoder::~MDecoder()
{
}

////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////

void MDecoder::setNumContexts(int numContexts) {
	if (numContexts != contexts_.size()) {
		contexts_.resize(numContexts);
		clearContexts();
	}
}

void MDecoder::clearContexts() {
	for (std::vector<Context>::iterator i = contexts_.begin();
	  i != contexts_.end(); ++i) {
		i->valMPS_ = 0;
		i->pStateIdx_ = 0;
	}
}

int MDecoder::start()
{
	if (debugLevel_ > 0) {
		getDebugStream() << "start()\n";
	}
	codIRange_ = 0x1fe;
	long value;
	if ((value = readBits(9)) < 0) {
		return -1;
	}
	codIOffset_ = value;
	initialized_ = true;
	return 0;
}

////////////////////////////////////////////////////////////
// Decoding
////////////////////////////////////////////////////////////

int MDecoder::decodeRegular(int contextIdx)
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "decodeRegular(" << contextIdx << ")\n";
	}
	if (debugLevel_ > 0) {
		dump(getDebugStream());
	}
#if defined(SPL_MCoder_BypassAll)
	return decodeBypass();
#endif
	int binVal;
	Context* context = &contexts_[contextIdx];
	unsigned short qCodIRangeIdx = (codIRange_ >> 6) & 3;
	unsigned short codIRangeLPS =
	  rangeTabLPS[context->pStateIdx_][qCodIRangeIdx];
	codIRange_ -= codIRangeLPS;
	if (codIOffset_ >= codIRange_) {
		binVal = context->valMPS_ ^ 1;
		codIOffset_ -= codIRange_;
		codIRange_ = codIRangeLPS;
		if (!context->pStateIdx_) {
			context->valMPS_ ^= 1;
		}
		context->pStateIdx_ = transIdxLPS[context->pStateIdx_];
	} else {
		binVal = context->valMPS_;
		context->pStateIdx_ = transIdxMPS[context->pStateIdx_];
	}
	if (renormD()) {
		return -1;
	}
	++symCount_;
	return binVal;
}

int MDecoder::decodeBypass()
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "decodeBypass()\n";
	}
	if (debugLevel_ > 0) {
		dump(getDebugStream());
	}
	int binVal;
	int b;
	if ((b = readBits(1)) < 0) {
		return -1;
	}
	codIOffset_ = (codIOffset_ << 1) | b;
	if (codIOffset_ >= codIRange_) {
		binVal = 1;
		codIOffset_ -= codIRange_;
	} else {
		binVal = 0;
	}
	++symCount_;
	return binVal;
}

////////////////////////////////////////////////////////////
// Termination
////////////////////////////////////////////////////////////

int MDecoder::terminate()
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "terminate()\n";
	}
	if (debugLevel_ > 0) {
		dump(getDebugStream());
	}
	int val;
	if ((val = decodeTerminate()) < 0) {
		return -1;
	}
	if (val != 1) {
		return -1;
	}
	return 0;
}

int MDecoder::decodeTerminate()
{
	assert(initialized_);
	if (debugLevel_ > 0) {
		getDebugStream() << "decodeTerminate()\n";
	}
	if (debugLevel_ > 0) {
		dump(getDebugStream());
	}
	int binVal;
	codIRange_ -= 2;
	if (codIOffset_ >= codIRange_) {
		binVal = 1;
	} else {
		binVal = 0;
		if (renormD()) {
			return -1;
		}
	}
	++symCount_;
	return binVal;
}

////////////////////////////////////////////////////////////
// Helper functions.
////////////////////////////////////////////////////////////

int MDecoder::renormD()
{
	assert(initialized_);
	while (codIRange_ < 0x100) {
		codIRange_ <<= 1;
		int b;
		if ((b = readBits(1)) < 0) {
			return -1;
		}
		codIOffset_ = (codIOffset_ << 1) | b;
	}
	return 0;
}

////////////////////////////////////////////////////////////
// Debugging support
////////////////////////////////////////////////////////////

int MDecoder::debugLevel_ = 0;
std::ostream* MDecoder::debugStream_ = 0;

void MDecoder::dump(std::ostream& out) const
{
	out << "codIOffset=" << std::hex << codIOffset_ << " codIRange=" << codIRange_
	  << " pStateIdx=" << int(contexts_[0].pStateIdx_) << " valMPS=" <<
	  int(contexts_[0].valMPS_) << std::dec << "\n";
	out << "numContexts=" << getNumContexts() << "\n";
	for (int i = 0; i < getNumContexts(); ++i) {
		out << "context " << i << " pStateIdx=" << int(contexts_[i].pStateIdx_)
		  << " valMPS=" << int(contexts_[i].valMPS_) << "\n";
	}
}

void MDecoder::setDebugLevel(int debugLevel)
{
	debugLevel_ = debugLevel;
}

void MDecoder::setDebugStream(std::ostream& debugStream)
{
	debugStream_ = &debugStream;
}

std::ostream& MDecoder::getDebugStream()
{
	return debugStream_ ? *debugStream_ : std::cerr;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

}

