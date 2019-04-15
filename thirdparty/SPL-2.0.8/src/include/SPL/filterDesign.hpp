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
 * @file filterDesign.hpp
 * @brief This file contains code for performing filter design.
 */

#ifndef SPL_filterDesign_hpp
#define SPL_filterDesign_hpp

////////////////////////////////////////////////////////////////////////////////
// Header Files.
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <SPL/Sequence1.hpp>

namespace SPL {

/**
 * @addtogroup Filter_design
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Basic Filter Design Functions.
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Design a zero-phase FIR lowpass filter.
 *
 * Effects:
 * A linear-phase FIR filter with zero group delay is designed with the
 * (normalized) cutoff frequency cutoffFreq, (normalized) transition width
 * transWidth, maximum peak-to-peak passband ripple maxPassbandRipple (in dB),
 * and minimum stopband attenuation minStopbandAtten (in dB).
 * Note: Frequencies are normalized such that a value of one
 * corresponds to the Nyquist frequency.
 *
 * Returns:
 * The impulse response of the designed filter is returned.
 */

RealSequence1 lowpassFilter(double cutoffFreq, double transWidth,
  double maxPassbandRipple = 0.1, double minStopbandAtten = 20.0);

/*!
 * @brief Design a zero-phase FIR highpass filter.
 *
 * Effects:
 * A linear-phase FIR filter with zero group delay is designed with the
 * (normalized) cutoff frequency cutoffFreq, (normalized) transition width
 * transWidth, maximum peak-to-peak passband ripple maxPassbandRipple (in dB),
 * and minimum stopband attenuation minStopbandAtten (in dB).
 * Note: Frequencies are normalized such that a value of one
 * corresponds to the Nyquist frequency.
 *
 * Returns:
 * The impulse response of the designed filter is returned.
 */

RealSequence1 highpassFilter(double cutoffFreq, double transWidth,
  double maxPassbandRipple = 0.1, double minStopbandAtten = 20.0);

/*!
 * @brief Design a zero-phase FIR bandpass filter.
 *
 * Effects:
 * A linear-phase FIR filter with zero group delay is designed with the
 * (normalized) lower cutoff frequency cutoffFreq0, (normalized)
 * upper cutoff frequency cutoffFreq1, (normalized) lower transition width
 * transWidth0, (normalized) upper transition width transWidth1,
 * maximum peak-to-peak passband ripple maxPassbandRipple (in dB), and
 * minimum stopband attenuation minStopbandAtten (in dB).
 * Note: Frequencies are normalized such that a value of one
 * corresponds to the Nyquist frequency.
 *
 * The cutoff frequencies cannot be 0 or 1.  In other words, this
 * function cannot be used to design a lowpass or highpass filter.
 *
 * Returns:
 * The impulse response of the designed filter is returned.
 */

RealSequence1 bandpassFilter(double cutoffFreq0, double cutoffFreq1,
  double transWidth0, double transWidth1, double maxPassbandRipple = 0.1,
  double minStopbandAtten = 20.0);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @}
 */

}

#endif
