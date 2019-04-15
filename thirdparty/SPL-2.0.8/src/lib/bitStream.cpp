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
#include <cstdlib>
#include <cassert>
#include <SPL/bitStream.hpp>

namespace SPL {

////////////////////////////////////////////////////////////////////////////////
// Input Bit Stream Class
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Constructors and destructor
////////////////////////////////////////////////////////////

InputBitStream::InputBitStream()
{
	setInput(0);
	readLimit_ = -1;
	readCount_ = 0;
}

InputBitStream::InputBitStream(std::istream& in)
{
	setInput(&in);
	readLimit_ = -1;
	readCount_ = 0;
}

InputBitStream::~InputBitStream()
{
}

////////////////////////////////////////////////////////////
// Set and get various state
////////////////////////////////////////////////////////////

void InputBitStream::setInput(std::istream* in)
{
	in_ = in;
	buf_ = 0;
	count_ = 0;
}

////////////////////////////////////////////////////////////
// Input and alignment
////////////////////////////////////////////////////////////

long InputBitStream::getBits(int numBits)
{
//	assert(isOkay());
	assert(numBits >= 1);
	assert(numBits < 8 * sizeof(long) - 1);
	if (readLimit_ >= 0 && numBits > readLimit_) {
		setIoStateBits(limitBit);
		return -1;
	}
#if defined(SPL_InputBitStream_Debug)
	std::cerr << "getBits(" << numBits << ")" << "... ";
#endif
	long data = 0;
	while (numBits > 0) {
		if (count_ <= 0) {
			if (!fillBuf()) {
// XXXXXXXX
				setIoStateBits(eofBit);
				return -1;
			}
		}
		int n = std::min<Size>(count_, numBits);
		int v = (buf_ >> (count_ - n)) & ((1 << n) - 1);
		data = (data << n) | v;
		numBits -= n;
		count_ -= n;
		if (readLimit_ >= 0) {
			readLimit_ = std::max<Offset>(readLimit_ - n, 0LL);
		}
		readCount_ += n;
	}
#if defined(SPL_InputBitStream_Debug)
	std::cerr << "returning " << std::hex << "0x" << data << std::dec << "\n";
#endif
	return data;
}

void InputBitStream::align()
{
//	assert(isOkay());
	count_ = 0;
	buf_ = 0;
}

bool InputBitStream::fillBuf()
{
	if (count_ <= 0) {
		char c;
		in_->get(c);
		if (*in_) {
			buf_ = c;
			count_ = 8;
		} else {
			if (in_->eof()) {
				setIoStateBits(eofBit);
			}
			setIoStateBits(badBit);
			buf_ = 0;
			count_ = 0;
		}
	}
	return isOkay();
}

////////////////////////////////////////////////////////////
// Debugging
////////////////////////////////////////////////////////////

void InputBitStream::dump(std::ostream& out) const
{
	out << "cnt=" << count_
	  << " buf=" << "0x" << std::hex << (unsigned int) buf_
	  << std::dec << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// Output Bit Stream Class
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Constructors and destructors
////////////////////////////////////////////////////////////

OutputBitStream::OutputBitStream()
{
	setOutput(0);
	writeCount_ = 0;
	writeLimit_ = -1;
}

OutputBitStream::OutputBitStream(std::ostream& out)
{
	setOutput(&out);
	writeCount_ = 0;
	writeLimit_ = -1;
}

OutputBitStream::~OutputBitStream()
{
	flushBuf();
}

////////////////////////////////////////////////////////////
// Set and get various state
////////////////////////////////////////////////////////////

void OutputBitStream::setOutput(std::ostream* out)
{
	out_ = out;
	buf_ = 0;
	count_ = 8;
}

std::ostream* OutputBitStream::getOutput() const
{
	return out_;
}

////////////////////////////////////////////////////////////
// Output
////////////////////////////////////////////////////////////

int OutputBitStream::putBits(long data, int numBits)
{
#if defined(SPL_OutputBitStream_Debug)
	std::cerr << "putbits(" << std::hex << "0x" << data << std::dec << "," << numBits << ")" << "\n";
#endif
//	assert(isOkay());
	assert(numBits >= 1);
	assert(numBits < 8 * sizeof(long) - 1);
	assert(data >= 0);
	assert((data & (~((1L << numBits) - 1))) == 0);
	if (writeLimit_ >= 0 && numBits > writeLimit_) {
		setIoStateBits(limitBit);
		return -1;
	}
	while (numBits > 0) {
		int n = std::min<Size>(count_, numBits);
		int v = (data >> (numBits - n)) & ((1 << n) - 1);
		buf_ = (buf_ & (~(((1 << n) - 1) << (count_ - n)))) | (v << (count_ - n));
		count_ -= n;
		numBits -= n;
		writeCount_ += n;
		if (writeLimit_ > 0) {
			writeLimit_ = std::max<Offset>(0LL, writeLimit_ - n);
		}
		if (count_ <= 0) {
			if (!flushBuf()) {
				return -1;
			}
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////
// Alignment and flushing
////////////////////////////////////////////////////////////

bool OutputBitStream::flushBuf()
{
	// If the buffer is not empty...
	if (count_ < 8) {
		out_->put(buf_);
		count_ = 8;
		buf_ = 0;
		if (!(*out_)) {
			setIoStateBits(badBit);
			return false;
		}
	}
	return true;
}

void OutputBitStream::flush()
{
//	assert(isOkay());
	flushBuf();
	out_->flush();
}

void OutputBitStream::align()
{
//	assert(isOkay());
	flushBuf();
}

////////////////////////////////////////////////////////////
// Debugging
////////////////////////////////////////////////////////////

void OutputBitStream::dump(std::ostream& out) const
{
	out << "cnt=" << count_ << " buf=" << "0x" << std::hex << (unsigned int) buf_ <<
	  std::dec << "\n";
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

}

