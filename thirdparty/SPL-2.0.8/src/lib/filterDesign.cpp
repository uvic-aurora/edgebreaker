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

#include <SPL/config.hpp>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <boost/math/special_functions.hpp>
#include "SPL/Sequence1.hpp"
#include "SPL/math.hpp"
#include "SPL/filterDesign.hpp"

namespace SPL {

// The filter design method employed herein is described in detail in:
// A. Antoniou, Digital Signal Processing: Signals, Systems, and Filters,
// McGraw-Hill, New York, NY, USA, 2006, pp. 445--453.

// The modified Bessel function of the first kind.
inline double modBesselFuncFirstKind(double x)
{
	return x ? boost::math::cyl_bessel_i(0.0, x) : 1.0;
//	return x ? cyl_bessel_i(0.0, x) : 1.0;
}

// The Kaiser window function.
inline double kaiser(int n, int en, double alpha)
{
	double beta = alpha * sqrt(1.0 - sqr(2 * n / (en - 1.0)));
	if (2 * absVal(n) <= en - 1) {
		return modBesselFuncFirstKind(beta) / modBesselFuncFirstKind(alpha);
	} else {
		return 0;
	}
}

// The ideal lowpass frequency response impulse response.
inline double lowpassFreqResp(int n, double ws, double wc)
{
	return 2.0 * wc / ws * sinc(2.0 * M_PI * wc * n / ws);
}

// The ideal highpass frequency response impulse response.
inline double highpassFreqResp(int n, double ws, double wc)
{
	return n ? -lowpassFreqResp(n, ws, wc) : (1.0 - lowpassFreqResp(n, ws, wc));
}

// The ideal bandpass frequency response impulse response.
inline double bandpassFreqResp(int n, double ws, double centerFreq,
  double bandwidth)
{
	return 2.0 * bandwidth / ws * sinc(M_PI * bandwidth * n / ws) *
	  cos(2.0 * M_PI * centerFreq * n / ws);
}

// Compute the parameters for the Kaiser window.
inline void computeKaiserParms(double ws, double bt, double ap0, double aa0, int& en, double& alpha)
{
	double delta = std::min((pow(10.0, 0.05 *ap0) - 1.0) /
	  (pow(10.0, 0.05 *ap0) + 1.0), pow(10.0, -0.05 * aa0));
	double aa = -20.0 * log10(delta);

	if (aa <= 21.0) {
		alpha = 0.0;
	} else if (aa > 21.0 && aa <= 50.0) {
		alpha = 0.5842 * pow(aa - 21.0, 0.4) + 0.07886 * (aa - 21.0);
	} else {
		alpha = 0.1102 * (aa - 8.7);
	}
	double d;
	if (aa <= 21.0) {
		d = 0.9222;
	} else {
		d = (aa - 7.95) / 14.36;
	}
	en = ceil(ws * d / bt + 1);
	if (!(en % 2)) {
		++en;
	}
//	std::cout << "delta=" << delta << "\n";
//	std::cout << "aa=" << aa << "\n";
//	std::cout << "d=" << d << "\n";
//	std::cout << "N=" << en << "\n";
}

RealSequence1 lowpassFilter0(double ws, double wp, double wa, double ap0,
  double aa0)
{
	assert(wp < wa);
	assert(aa0 > 0);
	assert(ap0 > 0);

	double wc = 0.5 * (wp + wa);
	int en;
	double alpha;
	computeKaiserParms(ws, wa - wp,  ap0, aa0, en, alpha);

	int m = (en - 1) / 2;
	RealSequence1 f(-m, 2 * m + 1);
	for (int i = -m; i <= m; ++i) {
		f(i) = kaiser(i, en, alpha) * lowpassFreqResp(i, ws, wc);
	}
	return f;
}

RealSequence1 lowpassFilter(double cutoffFreq, double transWidth,
  double maxPassbandRipple, double minStopbandAtten)
{
	assert(cutoffFreq >= 0.0 && cutoffFreq <= 1.0);
	assert(transWidth >= 0.0 && transWidth <= 1.0);
	assert(maxPassbandRipple >= 0.0);
	assert(minStopbandAtten >= 0.0);
	return lowpassFilter0(2.0, cutoffFreq - 0.5 * transWidth,
	  cutoffFreq + 0.5 * transWidth, maxPassbandRipple, minStopbandAtten);
}

RealSequence1 highpassFilter0(double ws, double wa, double wp, double ap0,
  double aa0)
{
	assert(wp > wa);
	assert(ap0 > 0.0);
	assert(aa0 > 0.0);
	double wc = 0.5 * (wp + wa);
	int en;
	double alpha;
	computeKaiserParms(ws, wp - wa, ap0, aa0, en, alpha);

	int m = (en - 1) / 2;
	RealSequence1 f(-m, 2 * m + 1);
	for (int i = -m; i <= m; ++i) {
		f(i) = kaiser(i, en, alpha) * highpassFreqResp(i, ws, wc);
	}
	return f;
}

RealSequence1 highpassFilter(double cutoffFreq, double transWidth,
  double maxPassbandRipple, double minStopbandAtten)
{
	assert(cutoffFreq >= 0.0 && cutoffFreq <= 1.0);
	assert(transWidth >= 0.0 && transWidth <= 1.0);
	assert(maxPassbandRipple >= 0.0);
	assert(minStopbandAtten >= 0.0);
	return highpassFilter0(2.0, cutoffFreq - 0.5 * transWidth,
	  cutoffFreq + 0.5 * transWidth, maxPassbandRipple, minStopbandAtten);
}

RealSequence1 bandpassFilter(double cutoffFreq0, double cutoffFreq1,
  double transWidth0, double transWidth1, double maxPassbandRipple,
  double minStopbandAtten)
{
	assert(cutoffFreq0 > 0.0 && cutoffFreq0 < 1.0);
	assert(cutoffFreq1 > 0.0 && cutoffFreq1 < 1.0);
	assert(transWidth0 >= 0.0 && transWidth0 <= 1.0);
	assert(transWidth1 >= 0.0 && transWidth1 <= 1.0);
	assert(maxPassbandRipple >= 0.0);
	assert(minStopbandAtten >= 0.0);
	// NOTE: Should I add an assertion here to check if the two transition bands overlap (which probably should never happen)?
//std::cerr << "wc0=" << cutoffFreq0 << " wc1=" << cutoffFreq1 << " B0=" << transWidth0 << " B1=" << transWidth1 << " " << maxPassbandRipple << " " << minStopbandAtten << "\n";
	int en;
	double alpha;
	double transWidth = std::min(transWidth0, transWidth1);
	computeKaiserParms(2.0, transWidth,  maxPassbandRipple, minStopbandAtten,
	  en, alpha);

	int m = (en - 1) / 2;
	RealSequence1 f(-m, 2 * m + 1);
//std::cerr << alpha << " " << en << " " << m << "\n";
	for (int i = -m; i <= m; ++i) {
		f(i) = kaiser(i, en, alpha) * bandpassFreqResp(i, 2.0, 0.5 * (cutoffFreq0 + cutoffFreq1), absVal(cutoffFreq0 - cutoffFreq1));
	}
	return f;
}

}
