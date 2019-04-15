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
 * @file arithCoder.hpp
 * @brief M-ary and Binary Arithmetic Coders.
 *
 * The arithmetic coding method implemented is essentially that presented in:
 *     I. H. Witten, R. M. Neal, and J. G. Cleary, "Arithmetic Coding for
 *     Data Compression", Communications of the ACM, 30(6), June 1987,
 *     pp. 520-540.
 */

#ifndef SPL_arithCoder_hpp
#define SPL_arithCoder_hpp

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <SPL/bitStream.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

// In the GIC code base, the following symbol was defined.
// The following symbol should probably not normally be defined.
#define SPL_ArithCoder_CodeSize32

#define SPL_ArithCoder_Entropy

#if defined(SPL_ArithCoder_CodeSize32)
#define SPL_ArithCoder_BigType unsigned long long
#define SPL_ArithCoder_NumBigBits 64
#define SPL_ArithCoder_CodeType unsigned long
#define SPL_ArithCoder_NumCodeBits 32
#define SPL_ArithCoder_FreqType unsigned long
#define SPL_ArithCoder_NumFreqBits 30
#else
#define SPL_ArithCoder_BigType unsigned long
#define SPL_ArithCoder_NumBigBits 32
#define SPL_ArithCoder_CodeType unsigned short
#define SPL_ArithCoder_NumCodeBits 16
#define SPL_ArithCoder_FreqType unsigned short
#define SPL_ArithCoder_NumFreqBits 14
#endif

// NOTE: This probably ought to be 255 (one less than a power of 2).
#define SPL_ArithCoder_DefaultMaxFreq 256

// These types need to be revisited.  Maybe they should be changed.
// For now, they have been flagged in the code by these special names.
#define SPL_ArithCoder_long long
#define SPL_ArithCoder_ulong unsigned long

namespace SPL {

/*!
 * @addtogroup ArithCoder
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Arithmetic Coder Core: Common Encoder/Decoder Class
////////////////////////////////////////////////////////////////////////////////

//! @cond PRIVATE

/*!
 * \brief Core arithmetic coder class.
 *
 * This class provides the core functionality of an arithmetic encoder.
 * The core functionality is provided in such a way as to be independent of
 * the alphabet size and completely decoupled from any probability model.
 */

class ArithCoder
{
public:

	//! The type for frequency values.
	typedef SPL_ArithCoder_FreqType Freq;

	//! The number of bits in a frequency value.
	static const int FreqBits = SPL_ArithCoder_NumFreqBits;

	//! The maximum allowable frequency value.
	static const Freq MaxFreq = (1ULL << FreqBits) - 1;

protected:

	// The type for a code word.
	typedef SPL_ArithCoder_CodeType Code;

	// A type providing sufficient dynamic range for code word computations.
	typedef SPL_ArithCoder_BigType BigCode;

	static const int BigBits = SPL_ArithCoder_NumBigBits;
	static const int CodeBits = SPL_ArithCoder_NumCodeBits;
	static const Code MaxCode = (1ULL << CodeBits) - 1;
	static const Code FirstQuart = (MaxCode >> 2) + 1;
	static const Code Half = 2 * FirstQuart;
	static const Code ThirdQuart = 3 * FirstQuart;
};

//! @endcond

////////////////////////////////////////////////////////////////////////////////
// M-ary Arithmetic Coder Core: Encoder Class
////////////////////////////////////////////////////////////////////////////////

//! @cond PRIVATE

/*!
 * \brief Core arithmetic encoder class.
 *
 * This class provides a very basic arithmetic encoding engine.
 * The engine is structured in such a way as to be independent of
 * the alphabet size and decoupled from any probability model.
 */

class ArithEncoder : public ArithCoder
{
public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Create an arithmetic encoder whose output will be sent to
	 * the specified bit stream.
	 *
	 * @param out  The output bit stream.
	 *
	 * This constructor creates an arithmetic encoder whose output will
	 * be sent to the specified bit stream.
	 */
	ArithEncoder(OutputBitStream* out = nullptr);

	/*!
	 * \brief Destroy an arithmetic encoder.
	 *
	 * \details
	 * This destructor destroys an arithmetic encoder.
	 */
	~ArithEncoder();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Set the bit stream to which arithmetic encoder output is
	 * to be sent.
	 *
	 * @param out  The output bit stream.
	 *
	 * \details
	 * This function sets the bit stream to which arithmetic encoder
	 * output is to be sent.
	 */
	void setOutput(OutputBitStream* out);

	/*!
	 * \brief Get the bit stream to which arithmetic encoder output is
	 * to be sent.
	 *
	 * \details
	 * This function gets the bit stream to which arithmetic encoder
	 * output is to be sent.
	 *
	 * \return
	 * A pointer to the output bit stream is returned.
	 */
	OutputBitStream* getOutput() const;

	/*!
	 * \brief Get the number of symbols encoded so far.
	 *
	 * \details
	 * This function gets the number of symbols encoded so far by the
	 * arithmetic encoder.
	 *
	 * \return
	 * The number of symbols output so far is returned.
	 */
	SPL_ArithCoder_ulong getSymCount() const;

	/*!
	 * \brief Get the number of bits of output generated so far
	 * including bits awaiting output.
	 *
	 * \details
	 * This function gets the number of bits of output generated so far
	 * by the arithmetic encoder, including bits awaiting output.
	 *
	 * \return
	 * The number of bits output so far is returned.
	 */
	SPL_ArithCoder_ulong getBitCount() const;

	////////////////////////////////////////////////////////////
	// Encoding
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Start a code word.
	 *
	 * \details
	 * This function starts the encoding of a new code word.
	 * This function must be called before attempting to encode any symbols.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int start();

	/*!
	 * \brief Terminate the code word.
	 *
	 * \details
	 * This function terminates the encoding of a code word.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int terminate();

	/*!
	 * \brief Encode a symbol.
	 *
	 * \details
	 * The function encodes the symbol associated with the range specified.
	 * The symbol to be coded has the corresponding interval
	 * [lowFreq, highFreq) / totalFreq.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int encode(Freq lowFreq, Freq highFreq, Freq totalFreq);

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * \brief Set the stream to use for debugging output.
	 */
	static void setDebugStream(std::ostream& out);

	/*!
	 * \brief Get the stream being used for debugging output.
	 */
	static std::ostream& getDebugStream();

	/*!
	 * \brief Dump the encoder state information to the specified
	 * stream for debugging.
	 */
	void dump(std::ostream& out) const;

private:

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	ArithEncoder(const ArithEncoder&);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	ArithEncoder& operator=(const ArithEncoder&);

	// The debug level.
	static int debugLevel_;

	// The stream to use for debugging output.
	static std::ostream* debugStream_;

	inline int renorm();

	int bitPlusFollow(int bit);

	// The bit stream to which the encoder output is to be written.
	OutputBitStream* out_;

	// The low end of the current interval.
	Code low_;

	// The high end of the current interval.
	Code high_;

	// The number of bits yet to be output.
	unsigned long bitsOutstanding_;

	// The number of bits output so far.
	SPL_ArithCoder_ulong bitCount_;

	// The number of symbols encoded so far.
	SPL_ArithCoder_ulong symCount_;
};

inline void ArithEncoder::setOutput(OutputBitStream* out)
{
	out_ = out;
}

inline OutputBitStream* ArithEncoder::getOutput() const
{
	return out_;
}

inline SPL_ArithCoder_ulong ArithEncoder::getSymCount() const
{
	return symCount_;
}

inline SPL_ArithCoder_ulong ArithEncoder::getBitCount() const
{
	return bitCount_ + bitsOutstanding_;
}

inline void ArithEncoder::setDebugLevel(int debugLevel)
{
	debugLevel_ = debugLevel;
}

inline void ArithEncoder::setDebugStream(std::ostream& out)
{
	debugStream_ = &out;
}

inline std::ostream& ArithEncoder::getDebugStream()
{
	return debugStream_ ? (*debugStream_) : std::cerr;
}

//! @endcond

////////////////////////////////////////////////////////////////////////////////
// M-ary Arithmetic Coder Core: Decoder Class
////////////////////////////////////////////////////////////////////////////////

//! @cond PRIVATE

/*!
 * \brief Core arithmetic decoder class.
 *
 * This class provides a very basic arithmetic decoding engine.
 * The engine is structured in such a way as to be independent of
 * the alphabet size and decoupled from any probability model.
 */

class ArithDecoder : public ArithCoder
{
public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Create an arithmetic decoder that receives its input
	 * from the specified bit stream.
	 *
	 * \param in  The input bit stream.
	 *
	 * \details
	 * This constructor creates an arithmetic decoder whose input
	 * is read from the specified bit stream.
	 */
	ArithDecoder(InputBitStream* in = nullptr);

	/*!
	 * \brief Destroy an arithmetic decoder.
	 *
	 * \details
	 * This destructor destroys an arithmetic decoder.
	 */
	~ArithDecoder();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the bit stream from which input data is read.
	 *
	 * \details
	 * This function gets the bit stream from which decoder input
	 * is to be read.
	 *
	 * \return
	 * A pointer to the input bit stream is returned.
	 */
	InputBitStream* getInput() const;

	/*!
	 * \brief Set the bit stream from which input data is read.
	 *
	 * \param in  The input bit stream.
	 *
	 * \details
	 * This function sets the bit stream from which input data is to be read.
	 */
	void setInput(InputBitStream* in);

	/*!
	 * \brief Get the number of bits that have been read so far.
	 *
	 * \details
	 * This function gets the number of bits of input that have been
	 * read so far by the arithmetic decoder.
	 *
	 * \return
	 * The number of bits read so far is returned.
	 */
	SPL_ArithCoder_ulong getBitCount() const;

	/*!
	 * \brief Get the number of symbols decoded so far.
	 *
	 * \details
	 * This function get the number of symbols decoded so far.
	 *
	 * \return
	 * The number of symbols decoded so far is returned.
	 */
	SPL_ArithCoder_ulong getSymCount() const;

	////////////////////////////////////////////////////////////
	// Decoding
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Start a code word.
	 *
	 * \details
	 * This function starts the decoding of a new code word.
	 * This function must be called before attempting to decode any symbols.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int start();

	/*!
	 * \brief Terminate a code word (to synchronize with the encoder).
	 *
	 * \details
	 * This function terminates the decoding of a code word.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int terminate();

	/*!
	 * \brief Decode a symbol.
	 *
	 * \param totalFreq  The total frequency count.
	 * \param target  The decoded code value.
	 *
	 * \details
	 * This function decodes a symbol.
	 * The reference parameter target is set to the decoded code value.
	 * This value can then be mapped to a particular symbol.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int decode(Freq totalFreq, Freq& target);

	/*!
	 * \brief Adjust the current interval after decoding a symbol.
	 *
	 * \param lowFreq  The low end of the interval.
	 * \param highFreq  The high end of the interval.
	 * \param totalFreq  The total frequency count.
	 *
	 * \details
	 * This function adjusts the current interval after decoding a
	 * symbol.
	 * This function must be called after each symbol is decoded.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int adjust(Freq lowFreq, Freq highFreq, Freq totalFreq);

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * \brief Set the stream to be used for debugging output.
	 */
	static void setDebugStream(std::ostream& out);

	/*!
	 * \brief Get the stream used for debugging output.
	 */
	std::ostream& getDebugStream();

	/*!
	 * \brief Dump the decoder state information to the specified
	 * stream for debugging.
	 */
	void dump(std::ostream& out) const;

private:

	// The debug level.
	static int debugLevel_;

	// The stream to be used for debugging output.
	static std::ostream* debugStream_;

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	ArithDecoder(const ArithDecoder&);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	ArithDecoder& operator=(const ArithDecoder&);

	inline int renorm();

	// The bit stream from which to read data to decode.
	InputBitStream* in_;

	// The code value.
	Code value_;

	// The low end of the current interval.
	Code low_;

	// The high end of the current interval.
	Code high_;

	// The number of symbols that have been decoded so far.
	SPL_ArithCoder_ulong symCount_;

	// The number of bits that have been read so far.
	SPL_ArithCoder_ulong bitCount_;
};

inline InputBitStream* ArithDecoder::getInput() const
{
	return in_;
}

inline SPL_ArithCoder_ulong ArithDecoder::getSymCount() const
{
	return symCount_;
}

inline SPL_ArithCoder_ulong ArithDecoder::getBitCount() const
{
	return bitCount_;
}

inline void ArithDecoder::setDebugLevel(int debugLevel)
{
	debugLevel_ = debugLevel;
}

inline void ArithDecoder::setDebugStream(std::ostream& out)
{
	debugStream_ = &out;
}

inline std::ostream& ArithDecoder::getDebugStream()
{
	return debugStream_ ? (*debugStream_) : std::cerr;
}

inline void ArithDecoder::setInput(InputBitStream* in)
{
	in_ = in;
}

//! @endcond

////////////////////////////////////////////////////////////////////////////////
// Simple Probability Model
////////////////////////////////////////////////////////////////////////////////

//! @cond PRIVATE

/*!
 * \brief Arithmetic coder source model class.
 */

class ArithCoderModel
{
public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Create a model with the given number of symbols and
	 * specified maximum frequency.
	 */
	ArithCoderModel(int numSyms, ArithCoder::Freq maxFreq);

	/*!
	 * \brief Destroy a model.
	 */
	~ArithCoderModel();

	////////////////////////////////////////////////////////////
	// Lookup and update
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Map a symbol to an interval.
	 *
	 * This function is only needed for encoding.
	 */
	void lookup(int sym, ArithCoder::Freq& lowFreq,
	  ArithCoder::Freq& highFreq, ArithCoder::Freq& totalFreq) const;

	/*!
	 * \brief Map a value to a symbol and corresponding interval.
	 *
	 * This function is only needed for decoding.
	 */
	void invLookup(ArithCoder::Freq targetFreq, int& sym,
	  ArithCoder::Freq& lowFreq, ArithCoder::Freq& highFreq,
	  ArithCoder::Freq& totalFreq) const;

	/*!
	 * \brief Update the probability model to reflect the encoding/decoding
	 * of the specified symbol.
	 */
	void update(int sym);

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the number of symbols.
	 */
	int getNumSyms() const;

	/*!
	 * \brief Set the adaptivity mode of the model.
	 */
	void setAdaptive(bool adaptive);

	/*!
	 * \brief Get the adaptivity mode of the model.
	 */
	bool getAdaptive() const;

	/*!
	 * \brief Get the number of times a particular symbol has been seen
	 * so far (via update).
	 */
	SPL_ArithCoder_ulong getSymCount(int sym) const;

	/*!
	 * \brief Get the total number of symbols seen so far (via update).
	 */
	SPL_ArithCoder_ulong getTotalSymCount() const;

	////////////////////////////////////////////////////////////
	// Set and get probabilities
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the normalizing factor for the frequencies.
	 */
	ArithCoder::Freq getTotalFreq() const;

	// NEW: 2015-10-23
	ArithCoder::Freq getMaxFreq() const;

	/*!
	 * \brief Set the probabilities for the model.
	 */
	void setProbs(const std::vector<ArithCoder::Freq>& freqs);

	/*!
	 * \brief Get the probabilities for the model.
	 */
	void getProbs(std::vector<ArithCoder::Freq>& freqs);

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * \brief Set the stream to use for debugging output.
	 */
	static void setDebugStream(std::ostream& out);

	/*!
	 * \brief Get the stream to use for debugging output.
	 */
	static std::ostream& getDebugStream();

	/*!
	 * \brief Dump the internal state of the model to the specified stream.
	 */
	void dump(std::ostream& out) const;

private:

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	ArithCoderModel(const ArithCoderModel&);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	ArithCoderModel& operator=(const ArithCoderModel&);

	// The debug level.
	static int debugLevel_;

	// The stream to use for debugging output.
	static std::ostream* debugStream_;

	// Lookup table that maps from symbols to indices.
	// The size of the vector is the number of symbols.
	std::vector<int> symToIdx_;

	// Lookup table that maps from indices to symbols.
	// The size of the vector is one greater than the number of symbols.
	std::vector<int> idxToSym_;

	// The symbol frequencies (ordered by symbol index).
	// The size of the vector is one greater than the number of symbols.
	// The element with index zero is always zero?
	std::vector<ArithCoder::Freq> freqs_;

	// The cumulative frequencies.
	// The size of the vector is one greater than the number of symbols.
	std::vector<ArithCoder::Freq> cumFreqs_;

	// The number of times each symbol has been seen so far.
	std::vector<SPL_ArithCoder_ulong> symCnts_;

	// The total number of symbols that have been seen so far.
	SPL_ArithCoder_ulong totalSymCnt_;

	// The normalizing denominator for frequencies.
	ArithCoder::Freq maxFreq_;

	// The adaptivity mode.
	bool adaptive_;
};

inline SPL_ArithCoder_ulong ArithCoderModel::getSymCount(int sym) const
{
	return symCnts_[sym];
}

inline SPL_ArithCoder_ulong ArithCoderModel::getTotalSymCount() const
{
	return totalSymCnt_;
}

inline void ArithCoderModel::setAdaptive(bool adaptive)
{
	adaptive_ = adaptive;
}

inline bool ArithCoderModel::getAdaptive() const
{
	return adaptive_;
}

// NEW: 2015-10-23
inline ArithCoder::Freq ArithCoderModel::getMaxFreq() const
{
	return maxFreq_;
}

inline ArithCoder::Freq ArithCoderModel::getTotalFreq() const
{
	return cumFreqs_[0];
}

inline int ArithCoderModel::getNumSyms() const
{
	return symToIdx_.size();
}

inline void ArithCoderModel::setDebugLevel(int debugLevel)
{
	debugLevel_ = debugLevel;
}

inline void ArithCoderModel::setDebugStream(std::ostream& out)
{
	debugStream_ = &out;
}

inline std::ostream& ArithCoderModel::getDebugStream()
{
	return debugStream_ ? (*debugStream_) : std::cerr;
}

//! @endcond

////////////////////////////////////////////////////////////////////////////////
// M-ary Arithmetic Coder: Encoder Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief M-ary arithmetic encoder class.
 */

class MultiArithEncoder
{
public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Create an encoder with the specified number of contexts
	 * that sends output to the given bit stream.
	 */
	MultiArithEncoder(int maxContexts, OutputBitStream* out = nullptr);

	/*!
	 * \brief Destroy an encoder.
	 */
	~MultiArithEncoder();

	////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the bit stream used for output.
	 */
	OutputBitStream* getOutput();

	/*!
	 * \brief Set the bit stream used for output.
	 */
	void setOutput(OutputBitStream* out);

	/*!
	 * \brief Get the number of symbols encoded so far.
	 */
	SPL_ArithCoder_ulong getSymCount() const;

	/*!
	 * \brief Get the number of bits of output generated so far including
	 * bits awaiting output.
	 */
	SPL_ArithCoder_ulong getBitCount() const;

	/*!
	 * \brief Get the maximum number of contexts.
	 */
	int getMaxContexts() const;

	/*!
	 * \brief Set the specified context to have the given number of
	 * symbols which are initially equiprobable.
	 */
	void setContext(int contextId, int numSyms);

	/*!
	 * \brief Set the specified context to have symbols with the given
	 * frequencies and the given adaptivity.
	 */
	void setContext(int contextId, const std::vector<ArithCoder::Freq>&
	  symFreqs, bool adaptive);

	////////////////////////////////////////////////////////////
	// Encoding
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Start a code word.
	 *
	 * This function must be called before attempting to encode any symbols.
	 */
	int start();

	/*!
	 * \brief Terminate the code word.
	 */
	int terminate();

	/*!
	 * \brief Encode the given symbol in the specified context.
	 */
	int encodeRegular(int contextId, int sym);

	/*!
	 * \brief Encode the given symbol in bypass mode (i.e., a fixed
	 * probablity distribution where all symbols are equiprobable).
	 */
	int encodeBypass(int numSyms, int sym);

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * \brief Set the stream for debugging output.
	 */
	static void setDebugStream(std::ostream& out);

	/*!
	 * \brief Get the stream for debugging output.
	 */
	static std::ostream& getDebugStream();

	/*!
	 * \brief Dump the internal state of the encoder to the specified
	 * stream for debugging purposes.
	 */
	void dump(std::ostream& out) const;

private:

	// The debug level.
	static int debugLevel_;

	// The stream to use for debugging output.
	static std::ostream* debugStream_;

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	MultiArithEncoder(const MultiArithEncoder&);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	MultiArithEncoder& operator=(const MultiArithEncoder&);

	// The arithmetic coding engine.
	ArithEncoder enc_;

	// The context information.
	std::vector<ArithCoderModel*> contexts_;
};

inline std::ostream& MultiArithEncoder::getDebugStream()
{
	return (debugStream_) ? (*debugStream_) : std::cerr;
}

inline OutputBitStream* MultiArithEncoder::getOutput()
{
	return enc_.getOutput();
}

inline void MultiArithEncoder::setOutput(OutputBitStream* out)
{
	enc_.setOutput(out);
}

inline SPL_ArithCoder_ulong MultiArithEncoder::getSymCount() const
{
	return enc_.getSymCount();
}

inline SPL_ArithCoder_ulong MultiArithEncoder::getBitCount() const
{
	return enc_.getBitCount();
}

inline void MultiArithEncoder::setDebugLevel(int debugLevel)
{
	debugLevel_ = debugLevel;
}

inline void MultiArithEncoder::setDebugStream(std::ostream& debugStream)
{
	debugStream_ = &debugStream;
}

inline int MultiArithEncoder::getMaxContexts() const
{
	return contexts_.size();
}

////////////////////////////////////////////////////////////////////////////////
// M-ary Arithmetic Coder: Decoder Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief M-ary arithmetic decoder class.
 */

class MultiArithDecoder
{
public:

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Create a decoder with the specified maximum number of
	 * contexts that sends output to the given bit stream.
	 */
	MultiArithDecoder(int maxContexts, InputBitStream* in = nullptr);

	/*!
	 * \brief Destroy the decoder.
	 */
	~MultiArithDecoder();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the bit stream from which to read encoded data.
	 */
	InputBitStream* getInput() const;

	/*!
	 * \brief Set the bit stream from which to read encoded data.
	 */
	void setInput(InputBitStream* in);

	/*!
	 * \brief Get the number of bits read so far.
	 */
	SPL_ArithCoder_ulong getBitCount() const;

	/*!
	 * \brief Get the number of symbols decoded so far.
	 */
	SPL_ArithCoder_ulong getSymCount() const;

	////////////////////////////////////////////////////////////
	// Context management
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the maximum number of contexts.
	 */
	int getMaxContexts() const;

	/*!
	 * \brief Set the specified context to have the given number of
	 * symbols which are initially equiprobable.
	 */
	void setContext(int contextId, int numSyms);

	/*!
	 * \brief Set the specified context to have symbols with the given
	 * frequencies and the given adaptivity.
	 */
	void setContext(int contextId, const std::vector<ArithCoder::Freq>&
	  symFreqs, bool adaptive);

	////////////////////////////////////////////////////////////
	// Decoding
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Start a code word.
	 *
	 * This function must be called before attempting to decode any symbols.
	 */
	int start();

	/*!
	 * \brief Terminate a code word (for synchronization with encoder).
	 */
	int terminate();

	/*!
	 * \brief Decode a symbol using the given context.
	 */
	int decodeRegular(int contextId);

	/*!
	 * \brief Decode a symbol in bypass mode (i.e., all
	 * symbols equiprobable).
	 */
	int decodeBypass(int numSyms);

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * \brief Set the stream to use for debugging output.
	 */
	static void setDebugStream(std::ostream& out);

	/*!
	 * \brief Get the stream used for debugging output.
	 */
	static std::ostream& getDebugStream();

	/*!
	 * \brief Dump the internal state of the decoder to the specified
	 * stream for debugging purposes.
	 */
	void dump(std::ostream& out) const;

private:

	// The debug level.
	static int debugLevel_;

	// The stream to use for debugging output.
	static std::ostream* debugStream_;

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	MultiArithDecoder(const MultiArithDecoder&);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	MultiArithDecoder& operator=(const MultiArithDecoder&);

	// The arithmetic decoding engine.
	ArithDecoder dec_;

	// The context information.
	std::vector<ArithCoderModel*> contexts_;
};

inline std::ostream& MultiArithDecoder::getDebugStream()
{
	return debugStream_ ? (*debugStream_) : std::cerr;
}

inline InputBitStream* MultiArithDecoder::getInput() const
{
	return dec_.getInput();
}

inline void MultiArithDecoder::setInput(InputBitStream* in)
{
	dec_.setInput(in);
}

inline SPL_ArithCoder_ulong MultiArithDecoder::getBitCount() const
{
	return dec_.getBitCount();
}

inline SPL_ArithCoder_ulong MultiArithDecoder::getSymCount() const
{
	return dec_.getSymCount();
}

inline void MultiArithDecoder::setDebugLevel(int debugLevel)
{
	debugLevel_ = debugLevel;
}

inline void MultiArithDecoder::setDebugStream(std::ostream& debugStream)
{
	debugStream_ = &debugStream;
}

inline int MultiArithDecoder::getMaxContexts() const
{
	return contexts_.size();
}

////////////////////////////////////////////////////////////////////////////////
// Binary Arithmetic Coder
////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Binary Arithmetic Coder Context Statistics Class
 */

struct BinArithCoderContextStat
{
	SPL_ArithCoder_ulong numOnes;
	SPL_ArithCoder_ulong numSyms;
};

////////////////////////////////////////////////////////////////////////////////
// Binary Arithmetic Encoder
////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Binary arithmetic encoder class.
 */

class BinArithEncoder
{
public:

	using Freq = ArithCoder::Freq;

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Create an arithmetic encoder with the specified number of
	 * contexts that sends output to the given bit stream.
	 *
	 * \param numContexts  The number of contexts.
	 * \param out  The output bit stream.
	 *
	 * \details
	 * This constructor creates an arithmetic encoder with the
	 * number of contexts being \p numContexts that sends output to
	 * the bit stream \p out.
	 */
	BinArithEncoder(int numContexts, OutputBitStream* out = nullptr);

	/*!
	 * \brief Destroy an arithmetic encoder.
	 *
	 * \details
	 * This destructor destroys an arithmetic encoder.
	 */
	~BinArithEncoder();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the number of contexts.
	 *
	 * \details
	 * This function gets the number of contexts employed by the
	 * arithmetic encoder.
	 *
	 * \return
	 * The number of contexts is returned.
	 */
	int getNumContexts() const;

	/*!
	 * \brief Get the number of symbols output so far.
	 *
	 * \details
	 * This function gets the number of symbols output so far by the
	 * arithmetic encoder.
	 *
	 * \return
	 * The number of symbols output so far is returned.
	 */
	SPL_ArithCoder_ulong getSymCount() const;

	/*!
	 * \brief Get the number of bits output so far.
	 *
	 * \details
	 * This function gets the number of bits output so far by the
	 * arithmetic encoder.
	 *
	 * \return
	 * The number of bits output so far is returned.
	 */
	SPL_ArithCoder_ulong getBitCount() const;

	/*!
	 * \brief Set the bit stream to which encoded data should be written.
	 *
	 * \param out  The output bit stream.
	 *
	 * \details
	 * This function sets the bit stream to which encoded data should
	 * be written.
	 */
	void setOutput(OutputBitStream* out);

	/*!
	 * \brief Get the bit stream to which encoded data should be written.
	 *
	 * \details
	 * This function gets the bit stream to which encoded data should be
	 * written.
	 *
	 * \return
	 * A pointer to the output bit stream is returned.
	 */
	OutputBitStream* getOutput() const;

	////////////////////////////////////////////////////////////
	// Context management
	////////////////////////////////////////////////////////////

	/*!
	 * \brief
	 */
	void getContextStats(std::vector<BinArithCoderContextStat>& stats) const;

	/*!
	 * \brief Set the symbol probabilities and adaptivity for the
	 * specified context.
	 */
	void setContextState(int contextId, ArithCoder::Freq oneFreq,
	  ArithCoder::Freq totalFreq, ArithCoder::Freq maxFreq, bool adaptive);

	/*!
	 * \brief Get the symbol probabilities and adaptivity for the
	 * specified context.
	 *
	 * \param contextId  The ID of the context to query.
	 * \param[out] oneFreq  The frequency count for a one symbol.
	 * \param[out] totalFreq  The normalizing frequency count for all symbols.
	 * \param[out] maxFreq  The maximum normalizing frequency count.
	 * \param[out] adaptive  The adaptivity flag.
	 *
	 * \details
	 * This function queries the state of the context with the
	 * context ID \p contextId, and sets
	 * the parameters \p oneFreq, \p totalFreq, \p maxFreq, and \p adaptive
	 * appropriately.
	 * The probability of a one symbol is given by \p oneFreq / \p totalFreq,
	 * while the probability of a zero symbol is given by
	 * 1 - \p oneFreq / \p totalFreq.
	 */
	void getContextState(int contextId, ArithCoder::Freq& oneFreq,
	  ArithCoder::Freq& totalFreq, ArithCoder::Freq& maxFreq, bool& adaptive);

	////////////////////////////////////////////////////////////
	// Encoding
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Start a code word.
	 *
	 * \details
	 * This function starts the encoding of a new code word.
	 * This function must be called before attempting to encode any symbols.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int start();

	/*!
	 * \brief Encode the specified symbol in the given context.
	 *
	 * \param contextId  The ID of the context to be used for encoding.
	 * \param binVal  The symbol to be encoded.
	 *
	 * \details
	 * This function encodes the symbol \p binVal using the context
	 * specified by \p contextId.
	 * The value of \p contextId must be from 0 to n - 1 (inclusive),
	 * where n is the number of contexts employed by the arithmetic encoder.
	 * The symbol to be encoded must be either 0 or 1.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int encodeRegular(int contextId, int binVal);

	/*!
	 * \brief Encode the specified symbol in bypass mode (i.e., using
	 * a fixed probability distribution with all symbols being equiprobable).
	 *
	 * \param binVal  The symbol to be encoded.
	 *
	 * \details
	 * This function encodes the symbol \p binVal in bypass mode
	 * (i.e., using a fixed probability distribution with all symbols
	 * being equiprobable).
	 * The symbol to be encoded must be either 0 or 1.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int encodeBypass(int binVal);

	/*!
	 * \brief Terminate the code word.
	 *
	 * \details
	 * This function terminates the encoding of the current arithmetic
	 * code word.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int terminate();

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Dump the internal encoder state to the specified
	 * output stream for debugging purposes.
	 */
	void dump(std::ostream& out) const;

	/*!
	 * \brief Dump the internal encoder context state to the specified
	 * output stream for debugging purposes.
	 */
	void dumpModels(std::ostream& out) const;

	/*!
	 * \brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * \brief Set the stream to use for debugging output.
	 */
	static void setDebugStream(std::ostream& out);

	/*!
	 * \brief Get the stream used for debugging output.
	 */
	static std::ostream& getDebugStream();

#if defined(SPL_ArithCoder_Entropy)
	long double getEntropy() const;
	void clearEntropy();
#endif

private:

	// The debug level.
	static int debugLevel_;

	// The stream to use for debugging output.
	static std::ostream* debugStream_;

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	BinArithEncoder(const BinArithEncoder&);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	BinArithEncoder& operator=(const BinArithEncoder&);

	// The arithmetic coding engine.
	ArithEncoder enc_;

	// The context information.
	std::vector<ArithCoderModel*> models_;

	// The number of one symbols encoded in bypass mode.
	SPL_ArithCoder_ulong bypassOneCnt_;

	// The total number of symbols encoded in bypass mode.
	SPL_ArithCoder_ulong bypassSymCnt_;

#if defined(SPL_ArithCoder_Entropy)
	long double entropy_;
#endif
};

#if defined(SPL_ArithCoder_Entropy)
inline long double BinArithEncoder::getEntropy() const
{
	return entropy_;
}
inline void BinArithEncoder::clearEntropy()
{
	entropy_ = 0;
}
#endif

inline SPL_ArithCoder_ulong BinArithEncoder::getSymCount() const
{
	return enc_.getSymCount();
}

inline SPL_ArithCoder_ulong BinArithEncoder::getBitCount() const
{
	return enc_.getBitCount();
}

inline OutputBitStream* BinArithEncoder::getOutput() const
{
	return enc_.getOutput();
}

inline int BinArithEncoder::getNumContexts() const
{
	return models_.size();
}

inline void BinArithEncoder::setDebugStream(std::ostream& out)
{
	ArithEncoder::setDebugStream(out);
	debugStream_ = &out;
}

inline std::ostream& BinArithEncoder::getDebugStream()
{
	return debugStream_ ? (*debugStream_) : std::cerr;
}

inline void BinArithEncoder::setDebugLevel(int debugLevel)
{
	ArithEncoder::setDebugLevel(debugLevel >> 8);
	debugLevel_ = debugLevel;
}

inline void BinArithEncoder::setOutput(OutputBitStream* out)
{
	enc_.setOutput(out);
}

////////////////////////////////////////////////////////////////////////////////
// Binary Arithmetic Coder: Decoder Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Binary arithmetic decoder class.
 */

class BinArithDecoder
{
public:

	using Freq = ArithCoder::Freq;

	////////////////////////////////////////////////////////////
	// Constructors and destructor
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Create a decoder with the specified number of contexts
	 * that receives input from the given bit stream.
	 *
	 * \param numContexts  The number of contexts.
	 * \param in  The input bit stream.
	 *
	 * \details
	 * This function creates an arithmetic decoder with the
	 * number of contexts being \p numContexts that receives input
	 * from the bit stream \p in.
	 */
	BinArithDecoder(int numContexts, InputBitStream* in = nullptr);

	/*!
	 * \brief Destroy a decoder.
	 *
	 * \details
	 * This function destroys an arithmetic decoder.
	 */
	~BinArithDecoder();

	////////////////////////////////////////////////////////////
	// Set and get various state
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the number of symbols decoded so far.
	 *
	 * \details
	 * This function gets the number of symbols decoded so far by the
	 * arithmetic decoder.
	 *
	 * \return
	 * The number of symbols decoded so far is returned.
	 */
	SPL_ArithCoder_ulong getSymCount() const;

	/*!
	 * \brief Get the number of bits read so far.
	 *
	 * \details
	 * This function gets the number of bits read so far by the arithmetic
	 * decoder.
	 *
	 * \return
	 * The number of bits read so far is returned.
	 */
	SPL_ArithCoder_ulong getBitCount() const;

	/*!
	 * \brief Set the bit stream from which to read encoded data.
	 *
	 * \param in  The input bit stream.
	 *
	 * \details
	 * This function sets the bit stream from which to read encoded data
	 * to \p in.
	 */
	void setInput(InputBitStream* in);

	/*!
	 * \brief Get the bit stream from which to read encoded data.
	 *
	 * \details
	 * This function gets the bit stream from which to read encoded data.
	 *
	 * \return
	 * A pointer to the bit stream is returned.
	 */
	InputBitStream* getInput() const;

	////////////////////////////////////////////////////////////
	// Context management
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Get the number of contexts.
	 *
	 * \details
	 * This function gets the number of contexts employed by the arithmetic
	 * decoder.
	 *
	 * \return
	 * The number of contexts is returned.
	 */
	int getNumContexts() const;

	/*!
	 * \brief Set the symbol probabilities and adaptivity for the
	 * specified context.
	 */
	void setContextState(int contextId, ArithCoder::Freq oneFreq,
	  ArithCoder::Freq totalFreq, ArithCoder::Freq maxFreq, bool adaptive);

	/*!
	 * \brief Get the symbol probabilities and adaptivity for the
	 * specified context.
	 *
	 * \param contextId  The ID of the context to query.
	 * \param[out] oneFreq  The frequency count for a one symbol.
	 * \param[out] totalFreq  The normalizing frequency count for all symbols.
	 * \param[out] maxFreq  The maximum normalizing frequency count.
	 * \param[out] adaptive  The adaptivity flag.
	 *
	 * \details
	 * This function queries the state of the context with the
	 * context ID \p contextId, and sets
	 * the parameters \p oneFreq, \p totalFreq, \p maxFreq, and \p adaptive
	 * appropriately.
	 * The probability of a one symbol is given by \p oneFreq / \p totalFreq,
	 * while the probability of a zero symbol is given by
	 * 1 - \p oneFreq / \p totalFreq.
	 */
	void getContextState(int contextId, ArithCoder::Freq& oneFreq,
	  ArithCoder::Freq& totalFreq, ArithCoder::Freq& maxFreq, bool& adaptive);

	////////////////////////////////////////////////////////////
	// Decoding
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Start a code word.
	 *
	 * \details
	 * This function starts the decoding of a new arithmetic code word.
	 * This function must be called before attempting to decode any symbols.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int start();

	/*!
	 * \brief Terminate the code word (for synchonization with the encoder).
	 *
	 * \details
	 * This function terminates the decoding of the current arithmetic code
	 * word.
	 *
	 * \return
	 * Upon success, zero is returned; otherwise, a nonzero value is returned.
	 */
	int terminate();

	/*!
	 * \brief Decode a symbol in the specified context.
	 *
	 * \param contextId  The ID of the context to be used for decoding.
	 *
	 * \details
	 * The function decodes a symbol using the context specified by
	 * \p contextId.
	 * The value of \p contextId must be from 0 to n - 1 (inclusive),
	 * where n is the number of contexts employed by the arithmetic decoder.
	 *
	 * \return
	 * Upon success, the decoded symbol is returned (which is either 0 or 1);
	 * otherwise, a negative value is returned.
	 */
	int decodeRegular(int contextId);

	/*!
	 * \brief Decode a symbol in bypass mode (i.e., using a fixed
	 * probability distribution with all symbols being equiprobable).
	 *
	 * \details
	 * This function decodes a symbol in bypass mode (i.e., using a fixed
	 * probability distribution with all symbols being equiprobable).
	 *
	 * \return
	 * Upon success, the decoded symbol is returned (which is either 0 or 1);
	 * otherwise, a negative value is returned.
	 */
	int decodeBypass();

	////////////////////////////////////////////////////////////
	// Debugging
	////////////////////////////////////////////////////////////

	/*!
	 * \brief Dump the internal decoder state to the specified stream
	 * for debugging purposes.
	 */
	void dump(std::ostream& out) const;

	/*!
	 * \brief Set the debug level.
	 */
	static void setDebugLevel(int debugLevel);

	/*!
	 * \brief Set the stream to be used for debugging output.
	 */
	static void setDebugStream(std::ostream& out);

	/*!
	 * \brief Get the stream used for debugging output.
	 */
	static std::ostream& getDebugStream();

#if defined(SPL_ArithCoder_Entropy)
	long double getEntropy() const;
	void clearEntropy();
#endif

private:

	// The debug level.
	static int debugLevel_;

	// The stream used for debugging output.
	static std::ostream* debugStream_;

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	BinArithDecoder(const BinArithDecoder&);

	// The copy constructor is declared but not defined in order to
	// prevent copying.
	BinArithDecoder& operator=(const BinArithDecoder&);

	// The arithmetic decoding engine.
	ArithDecoder dec_;

	// The context information.
	std::vector<ArithCoderModel*> models_;

#if defined(SPL_ArithCoder_Entropy)
	long double entropy_;
#endif

};

#if defined(SPL_ArithCoder_Entropy)
inline long double BinArithDecoder::getEntropy() const
{
	return entropy_;
}
inline void BinArithDecoder::clearEntropy()
{
	entropy_ = 0;
}
#endif

inline SPL_ArithCoder_ulong BinArithDecoder::getSymCount() const
{
	return dec_.getSymCount();
}

inline SPL_ArithCoder_ulong BinArithDecoder::getBitCount() const
{
	return dec_.getBitCount();
}

inline InputBitStream* BinArithDecoder::getInput() const
{
	return dec_.getInput();
}

inline int BinArithDecoder::getNumContexts() const
{
	return models_.size();
}

inline void BinArithDecoder::setDebugStream(std::ostream& out)
{
	ArithDecoder::setDebugStream(out);
	debugStream_ = &out;
}

inline std::ostream& BinArithDecoder::getDebugStream()
{
	return debugStream_ ? (*debugStream_) : std::cerr;
}

inline void BinArithDecoder::setDebugLevel(int debugLevel)
{
	ArithDecoder::setDebugLevel(debugLevel >> 8);
	debugLevel_ = debugLevel;
}

inline void BinArithDecoder::setInput(InputBitStream* in)
{
	dec_.setInput(in);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/*!
 * @}
 */

}

#endif
