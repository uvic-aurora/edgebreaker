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
 * @file mCoder.hpp
 * @brief This file contains interface information for an implementation
 * of the M-Coder arithmetic coder from:
 *     ISO/IEC 14496-10:2008 (a.k.a. H.264)
 */

// Defining this symbol will cause all symbols to be processed in
// bypass mode.
// This symbol is intended for testing purposes only and SHOULD NOT
// normally be defined.
//#define SPL_MCoder_BypassAll

#ifndef SPL_mCoder_hpp
#define SPL_mCoder_hpp

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <vector>
#include <iostream>
#include <SPL/bitStream.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

/*!
 * @addtogroup MCoder
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// M-Coder:  Common Encoder/Decoder Information Class
////////////////////////////////////////////////////////////////////////////////

class MCoder
{
protected:

	struct Context;

	// Various lookup tables.
	static const unsigned char rangeTabLPS[64][4];
	static const unsigned char transIdxLPS[64];
	static const unsigned char transIdxMPS[64];
};

// Per-context state information
struct MCoder::Context {
	// The most probable symbol (MPS).
	unsigned char valMPS_;

	// The state index.
	unsigned char pStateIdx_;
};

////////////////////////////////////////////////////////////////////////////////
// M-Coder Encoder Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief The M-Coder (binary) arithmetic encoder class.
 */

class MEncoder : public MCoder
{
public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Create an encoder with a specified number of contexts that
	 * sends output to a given bit stream.
	 */
	MEncoder(int numContexts = 0, OutputBitStream* out = 0);

	/*!
	 * @brief Destroy an encoder.
	 */
	~MEncoder();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Set the number of contexts.
	 */
	void setNumContexts(int numContexts);

	/*!
	 * @brief Get the number of contexts.
	 */
	int getNumContexts() const;

	/*!
	 * @brief Clear the state of all of the contexts.
	 */
	void clearContexts();

	/*!
	 * @brief Set the bit stream to use for output.
	 */
	void setOutput(OutputBitStream* out);

	/*!
	 * @brief Get the bit stream being used for output.
	 */
	OutputBitStream* getOutput() const;

	/*!
	 * @brief Get the number of symbols that have been encoded so far.
	 */
	long getSymCount() const;

	/*!
	 * @brief Get the number of bits (of encoded data) that have been
	 * output to the underlying bit stream so far.
	 */
	long getBitCount() const;

	////////////////////////////////////////////////////////////
	// Encoding
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Start the arithmetic code word.
	 */
	void start();

	/*!
	 * @brief Terminate the arithmetic code word.
	 */
	int terminate();

	/*!
	 * @brief Encode a symbol in the specified context.
	 *
	 * The symbol binVal is encoded using context contextId.
	 */
	int encodeRegular(int contextId, int binVal);

	/*!
	 * @brief Encode a symbol in bypass mode
	 * (i.e., assuming that both symbols are equiprobable).
	 */
	int encodeBypass(int binVal);

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * @brief Set the stream for debugging output.
	 */
	static void setDebugStream(std::ostream& debugStream);

	/*!
	 * @brief Get the stream used for debugging output.
	 */
	static std::ostream& getDebugStream();

	/*!
	 * @brief Dump the internal state of the encoder for debugging.
	 */
	void dump(std::ostream& out) const;

private:

	static int debugLevel_;
	static std::ostream* debugStream_;

	int encodeTerminate(int binVal);
	int renormE();
	int putBit(int b);
	int encodeFlush();
	int writeBits(int v, int n);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	MEncoder(const MEncoder&);

	// The copy assignment operator is declared but not defined in order to
	// prevent copy assignment.
	MEncoder& operator=(const MEncoder&);

	// The bit stream for output.
	OutputBitStream* out_;

	// The per-context state information.
	std::vector<Context> contexts_;

	// Have any bits been coded yet?
	unsigned char firstBitFlag_;

	// Lower bound of current interval.
	unsigned short codILow_;

	// Size of current interval.
	unsigned short codIRange_;

	// The number of bits to follow the next bit of output.
	unsigned long bitsOutstanding_;

	// The number of symbols output.
	long symCount_;

	// The number of bits output so far.
	long bitCount_;

	// Has a code word been started?
	bool initialized_;

};

inline long MEncoder::getSymCount() const
{
	return symCount_;
}

inline long MEncoder::getBitCount() const
{
	return bitCount_ + bitsOutstanding_;
}

inline int MEncoder::getNumContexts() const {
	return contexts_.size();
}

inline OutputBitStream* MEncoder::getOutput() const {
	return out_;
}

inline void MEncoder::setOutput(OutputBitStream* out) {
	out_ = out;
}

////////////////////////////////////////////////////////////////////////////////
// M-Coder Decoder Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief The M-Coder (binary) arithmetic decoder class.
 */

class MDecoder : public MCoder {
public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Create a decoder with the specified number of contexts
	 * that reads input from the given bit stream.
	 */
	MDecoder(int numContexts = 0, InputBitStream* in = 0);

	/*!
	 * @brief Destroy a decoder.
	 */
	~MDecoder();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Set the number of contexts.
	 */
	void setNumContexts(int numContexts);

	/*!
	 * @brief Get the number of contexts.
	 */
	int getNumContexts() const;

	/*!
	 * @brief Set the input bit stream (i.e., the bit stream from which
	 * encoded data is to be read).
	 */
	void setInput(InputBitStream* in);

	/*!
	 * @brief Get the input bit stream (i.e., the bit stream from which
	 * encoded data is to be read).
	 */
	InputBitStream* getInput() const;

	/*!
	 * @brief Clear the state of all of the contexts.
	 */
	void clearContexts();

	/*!
	 * @brief Get the number of bits read so far.
	 */
	long getBitCount() const;

	/*!
	 * @brief Get the number of symbols decoded so far.
	 */
	long getSymCount() const;

	////////////////////////////////////////////////////////////
	// Decoding
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Prepare to decode an arithmetic code word.
	 *
	 * Note: This function must be called before attempting to decode
	 * any symbols.
	 */
	int start();

	/*!
	 * @brief Terminate the arithmetic code word.
	 */
	int terminate();

	/*!
	 * @brief Decode a symbol in the specified context.
	 */
	int decodeRegular(int contextId);

	/*!
	 * @brief Decode a symbol in bypass mode
	 * (i.e., assuming both symbols are equiprobable).
	 */
	int decodeBypass();

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * @brief Dump the internal state information for the decoder
	 * to a stream (for debugging).
	 */
	void dump(std::ostream& out) const;

	/*!
	 * @brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * @brief Set the stream to use for debugging output.
	 */
	static void setDebugStream(std::ostream& debugStream);

	/*!
	 * @brief Get the stream used for debugging output.
	 */
	static std::ostream& getDebugStream();

private:

	static int debugLevel_;
	static std::ostream* debugStream_;
	int decodeTerminate();
	int readBits(int n);
	int renormD();

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	MDecoder(const MDecoder&);

	// The copy assignment operator is declared but not defined in order to
	// prevent copy assignment.
	MDecoder& operator=(const MDecoder&);

	// The bit stream for input.
	InputBitStream* in_;

	// The current code range.
	unsigned short codIRange_;

	// The current code offset.
	unsigned short codIOffset_;

	// The per-context state information.
	std::vector<Context> contexts_;

	// The number of bits read so far.
	unsigned long bitCount_;

	// The number of symbols decoded so far.
	unsigned long symCount_;

	// Has a code word been started?
	bool initialized_;

};

inline long MDecoder::getBitCount() const {
	return bitCount_;
}

inline long MDecoder::getSymCount() const {
	return symCount_;
}

inline int MDecoder::getNumContexts() const {
	return contexts_.size();
}

inline InputBitStream* MDecoder::getInput() const {
	return in_;
}

inline void MDecoder::setInput(InputBitStream* in) {
	in_ = in;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @}
 */

}

#endif
