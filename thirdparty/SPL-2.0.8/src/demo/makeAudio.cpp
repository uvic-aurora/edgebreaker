// Copyright (c) 2011, 2012, 2013 Michael D. Adams
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

// This program makes an audio file.

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <SPL/Sequence1.hpp>
#include <SPL/audioFile.hpp>

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

typedef SPL::Sequence1<double> RealSequence1;

////////////////////////////////////////////////////////////////////////////////
// Sequence generation
////////////////////////////////////////////////////////////////////////////////

// A 1-periodic square wave function.
double squareWave(double t)
{
	int i = 2.0 * t;
	return (!(i % 2)) ? 1.0 : 0.0;
}

// A 1-periodic sinusoidal function.
double sinusoid(double t)
{
	return cos(2.0 * M_PI * t);
}

double signal(int id, double t)
{
	double x;
	switch (id) {
	default:
	case 0:
		x = sinusoid(440.0 * t);
		break;
	case 1:
		x = squareWave(440.0 * t);
		break;
	case 2:
		x = 0.5 * squareWave(220.0 * t) + sinusoid(440.0 * t);
		break;
	}
	return x;
}

// Construct a sequence corresponding the the specified function
// sampled at the given rate for a particular number of samples.
void makeSequence(int id, int sampRate, int numSamps, RealSequence1& seq)
{
	seq = RealSequence1(0, numSamps);
	for (RealSequence1::Iterator i = seq.begin(); i != seq.end(); ++i) {
		*i = signal(id, static_cast<double>(i - seq.begin()) / sampRate);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Main program
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	RealSequence1 seq;

	// Process the command line.
	if (argc < 5) {
		std::cerr << "usage: makeAudio signalId sampRate duration outputFile\n";
		exit(2);
	}
	int id = atoi(argv[1]);
	int sampRate = atoi(argv[2]);
	double duration = atof(argv[3]);
	std::string outFile(argv[4]);

	// Generate the sequence.
	makeSequence(id, sampRate, duration * sampRate, seq);

	// Scale the range of the sequence to fit the allowable dynamic range.
	if (seq.getSize() > 0) {
		double minVal = *std::min_element(seq.begin(), seq.end());
		double maxVal = *std::max_element(seq.begin(), seq.end());
		double maxMag = std::max(SPL::absVal(minVal), SPL::absVal(maxVal));
		double alpha = 0.95;
		double beta = alpha * 1.0 / maxMag;
		for (RealSequence1::Iterator i = seq.begin(); i != seq.end(); ++i) {
			*i *= beta;
		}
	}

	// Save the sequence in an audio file.
	if (SPL::saveAudioFile(outFile, sampRate, seq.getArray())) {
		std::cerr << "cannot write audio file\n";
	}

	return 0;
}
