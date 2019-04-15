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
 * @file audioFile.hpp
 * @brief This file contains code for performing reading and writing of
 * audio files in WAV format.
 */

#ifndef SPL_audioFile_hpp
#define SPL_audioFile_hpp

////////////////////////////////////////////////////////////////////////////////
// Header Files.
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#if defined(SPL_HAVE_LIBSNDFILE)
#include <sndfile.h>
#endif
#include <SPL/Array1.hpp>

namespace SPL {

/*!
 * @addtogroup Audio_IO
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Functions for Loading/Saving Audio Files.
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Read audio data from a file in WAV format.
 *
 * Effects:
 * The audio signal from the file named fileName in WAV format
 * is read.  The sample data is placed in the array samples and
 * the sampling rate is placed in samplingRate.
 * If the file has more than one channel, only the first channel
 * is read.
 * The sample data will always lie in the range [-1.0, 1.0].
 *
 * Returns:
 * On success, zero is returned.
 * On failure, a nonzero value is returned.
 */

int loadAudioFile(const std::string& fileName, int& samplingRate,
  RealArray1& samples);

/*!
 * @brief Write a sequence to a file in WAV format.
 *
 * Effects:
 * The sequence seq with sampling rate samplingRate is written
 * to the file named fileName in WAV format.
 * The sample data must lie in the range [-1.0, 1.0].
 *
 * Returns:
 * On success, zero is returned.
 * On failure, a nonzero value is returned.
 */

int saveAudioFile(const std::string& fileName, int samplingRate,
  const RealArray1& samples);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*!
 * @}
 */

}

#endif
