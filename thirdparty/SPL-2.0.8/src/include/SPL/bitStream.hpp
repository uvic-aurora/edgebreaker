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

/*!
 * @file bitStream.hpp
 * @brief Bit Stream Classes
 */

#ifndef SPL_bitStream_hpp
#define SPL_bitStream_hpp

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <cassert>
#include <iostream>

namespace SPL {

/*!
 * @addtogroup BitStream
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Bit stream class
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief A common base class for the input and output bit stream classes.
 *
 * This class provides some error handling functionality common to the
 * input and output bit stream classes.
 */

class BitStream
{

public:

	/*!
	 * @brief The type used for the error state for a stream.
	 */
	typedef unsigned IoState;

	/*
	 * The I/O state bits.
	 */
	//! end of file (EOF) reached on input
	static const IoState eofBit = 1;
	//! read/write limit exceeded
	static const IoState limitBit = 2;
	//! I/O error
	static const IoState badBit = 4;
	//! all error bits
	static const IoState allIoBits = eofBit | limitBit | badBit;

	/*!
	 * @brief An unsigned integral type (used for sizes/counts).
	 */
	typedef unsigned long long Size;

	/*!
	 * @brief A signed integral type (used for differences).
	 */
	typedef long long Offset;

	/*!
	 * @brief
	 * Test if the bitstream in an okay (i.e., non-error) state.
	 *
	 * (This is similar in spirit to !basic_ios::fail.)
	 */
	bool isOkay() const;

	/*!
	 * @brief
	 * Test if the bitstream has encountered end-of-file (EOF).
	 *
	 * (This is similar in spirit to basic_ios::eof.)
	 */
	bool isEof() const;

	/*!
	 * @brief
	 * Test if the bitstream has encountered a read/write limit.
	 */
	bool isLimit() const;

	/*!
	 * @brief Get the I/O state of a bit stream.
	 *
	 * (This is similar in spirit to basic_ios::rdstate.)
	 */
	IoState getIoState() const;

	/*!
	 * @brief Set the I/O state of a bit stream.
	 *
	 * (This is similar in spirit to basic_ios::clear.)
	 */
	void setIoState(IoState state);

	/*!
	 * @brief Set the specified bits in the I/O state of a bit stream.
	 *
	 * (This is similar in spirit to basic_ios::setstate.)
	 */
	void setIoStateBits(IoState state);

	/*!
	 * @brief Clear the specified bits in the I/O state of a bit stream.
	 *
	 * If no parameter is provided, all bits are cleared.
	 */
	void clearIoStateBits(IoState state = allIoBits);

protected:

	// Default constructor
	BitStream() : ioState_(0) {}

	// I/O state of bit stream (a bitmask).
	IoState ioState_;

private:

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	BitStream(const BitStream&);

	// The copy assignment operator is declared but not defined in order to
	// prevent copy assignment.
	BitStream& operator=(const BitStream&);

};

inline bool BitStream::isOkay() const
{
	return !(ioState_ & (badBit | limitBit));
}

inline bool BitStream::isEof() const
{
	return ioState_ & eofBit;
}

inline bool BitStream::isLimit() const
{
	return ioState_ & limitBit;
}

inline void BitStream::setIoState(BitStream::IoState state)
{
	assert(!(state & (~allIoBits)));
	ioState_ = state;
}

inline BitStream::IoState BitStream::getIoState() const
{
	return ioState_;
}

inline void BitStream::clearIoStateBits(BitStream::IoState mask)
{
	assert(!(mask & (~allIoBits)));
	ioState_ &= ~mask;
}

inline void BitStream::setIoStateBits(BitStream::IoState mask)
{
	assert(!(mask & (~allIoBits)));
	ioState_ |= mask;
}

////////////////////////////////////////////////////////////////////////////////
// Input Bit Stream Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Input bit stream class
 */

class InputBitStream : public BitStream
{

public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Create a bit stream that is not initially bound to
	 * any (character) stream.
	 */
	InputBitStream();

	/*!
	 * @brief Create a bit stream that reads data from the specified
	 * (character) stream.
	 */
	InputBitStream(std::istream& in);

	/*!
	 * @brief Destroy a bit stream.
	 */
	~InputBitStream();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get the (character) stream from which data is read.
	 */
	std::istream* getInput() const;

	/*!
	 * @brief Set the (character) stream from which data is read.
	 */
	void setInput(std::istream* in);

	/*!
	 * @brief Get the number of bits that still may be read from the
	 * bit stream before the read limit is reached.
	 *
	 * If read-limit checking is enabled, the function returns the number of
	 * bits that can still be read before the read limit is reached.
	 * If read-limit checking is disabled, a negative value is returned.
	 */
	Offset getReadLimit() const;

	/*!
	 * @brief Specify the maximum allowable number of bits that may be
	 * read from the bit stream.
	 *
	 * If readLimit is nonnegative, read-limit checking is enabled and the
	 * current read limit is set to readLimit.
	 * If readLimit is negative, read-limit checking is disabled.
	 */
	void setReadLimit(Offset readLimit);

	/*!
	 * @brief Get the number of bits read from the bit stream so far.
	 */
	Size getReadCount() const;

	/*!
	 * @brief Set the read count to zero.
	 */
	void clearReadCount();

	////////////////////////////////////////////////////////////
	// Input
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Read the specified number of bits from the bit stream.
	 *
	 * The bits that are read from the bit stream are assigned to the
	 * returned integer value in most-significant to least-significant
	 * order.
	 */
	long getBits(int numBits);

	////////////////////////////////////////////////////////////
	// Alignment
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Force byte-alignment of the bit stream.
	 *
	 * The bit stream position is moved forward to the nearest
	 * byte (i.e., multiple of 8 bits) boundary.
	 */
	void align();

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Dump the internal state of the bit stream to a
	 * (character) stream for debugging purposes.
	 */
	void dump(std::ostream& out) const;

private:

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	InputBitStream(const InputBitStream&);

	// The copy assignment operator is declared but not defined in order to
	// prevent copy assignment.
	InputBitStream& operator=(const InputBitStream&);
	
	bool fillBuf();

	// The underlying character stream.
	std::istream* in_;

	// The input buffer.
	unsigned char buf_;

	// The number of bits remaining before the buffer is empty.
	int count_;

	// The maximum number of bits that may still be read from the bit
	// stream.  If this quantity is negative, there is no limit.
	Offset readLimit_;

	// The number of bits read from the bit stream so far.
	Size readCount_;

};

inline void InputBitStream::clearReadCount()
{
	readCount_ = 0;
}

inline InputBitStream::Size InputBitStream::getReadCount() const
{
	return readCount_;
}

inline void InputBitStream::setReadLimit(InputBitStream::Offset readLimit)
{
	readLimit_ = readLimit;
}

inline InputBitStream::Offset InputBitStream::getReadLimit() const
{
	return readLimit_;
}

////////////////////////////////////////////////////////////////////////////////
// Output Bit Stream Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Output bit stream class
 */

class OutputBitStream : public BitStream
{

public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Create a bit stream that is not initially bound to
	 * any (character) stream.
	 */
	OutputBitStream();

	/*!
	 * @brief Create a bit stream that sends its output to the specified
	 * (character) stream.
	 */
	OutputBitStream(std::ostream& out);

	/*!
	 * @brief Destroy a bit stream.
	 */
	~OutputBitStream();

	////////////////////////////////////////////////////////////
	// Get and set various bit stream state.
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Get the output (character) stream associated with the
	 * bit stream.
	 */
	std::ostream* getOutput() const;

	/*!
	 * @brief Set the output (character) stream associated with the
	 * bit stream.
	 */
	void setOutput(std::ostream* out);

	/*!
	 * @brief Clear the count of the number of bits written to the bit stream.
	 */
	void clearWriteCount();

	/*!
	 * @brief Get the number of bits written to the bit stream.
	 */
	Size getWriteCount() const;

	/*!
	 * @brief Set the number of bits that may still be written to the bit
	 * stream.
	 *
	 * If writeLimit is nonnegative, write-limit checking is enabled and the
	 * current write limit is set to writeLimit.
	 * If writeLimit is negative, write-limit checking is disabled.
	 */
	void setWriteLimit(Offset writeLimit);

	/*!
	 * @brief Get the number of bits that may still be written to the
	 * underlying (character) stream.
	 *
	 * If write-limit checking is enabled, the function returns the number of
	 * bits that can still be written before the write limit is reached.
	 * If write-limit checking is disabled, a negative value is returned.
	 */
	Offset getWriteLimit() const;

	////////////////////////////////////////////////////////////
	// Output
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Output the specified number of bits to the bit stream.
	 *
	 * This function returns a nonnegative value upon success and a
	 * negative value if an error is encountered.
	 */
	int putBits(long data, int numBits);

	////////////////////////////////////////////////////////////
	// Alignment and flushing
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Align the bit stream output position to the nearest
	 * byte boundary.
	 */
	void align();

	/*!
	 * @brief Flush any pending output to the underlying (character) stream.
	 *
	 * The bit stream is aligned to the nearest byte boundary and any
	 * pending output is flushed to the underlying (character) stream.
	 */
	void flush();

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Dump the internal state of the bit stream to the specified
	 * (character) stream for debugging purposes.
	 */
	void dump(std::ostream& out) const;

private:

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	OutputBitStream(const OutputBitStream&);

	// The copy assignment operator is declared but not defined in order to
	// prevent copy assignment.
	OutputBitStream& operator=(const OutputBitStream&);
	
	bool flushBuf();

	// The underlying character stream.
	std::ostream* out_;

	// The output buffer.
	unsigned char buf_;

	// The number of bits remaining before the buffer is full.
	int count_;

	// The number of bits written to the underlying stream.
	Size writeCount_;

	// The maximum number of bits that may still be written to the underlying
	// stream.  If this quantity is negative, there is no limit.
	Offset writeLimit_;

};

inline OutputBitStream::Size OutputBitStream::getWriteCount() const
{
	return writeCount_;
}

inline void OutputBitStream::clearWriteCount()
{
	writeCount_ = 0;
}

inline OutputBitStream::Offset OutputBitStream::getWriteLimit() const
{
	return writeLimit_;
}

inline void OutputBitStream::setWriteLimit(OutputBitStream::Offset writeLimit)
{
	writeLimit_ = writeLimit;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @}
 */

}

#endif
