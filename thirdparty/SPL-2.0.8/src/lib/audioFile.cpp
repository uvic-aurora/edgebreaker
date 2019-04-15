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
#include <string>
#include <algorithm>
#if defined(SPL_HAVE_LIBSNDFILE)
#include <sndfile.h>
#endif
#include "SPL/Array1.hpp"
#include "SPL/Sequence1.hpp"
#include "SPL/audioFile.hpp"

namespace SPL {

#if defined(SPL_HAVE_LIBSNDFILE)

int loadAudioFile(const std::string& fileName, int& samplingRate,
  RealArray1& samples)
{
	// Open the audio file.
	SNDFILE* sfFile;
	SF_INFO sfInfo;
	if (!(sfFile = sf_open(fileName.c_str(), SFM_READ, &sfInfo))) {
		return -1;
	}
	samplingRate = sfInfo.samplerate;
	int numChannels = sfInfo.channels;

	if (numChannels > 1) {
		std::cerr << "warning: audio file has more than one channel\n";
		std::cerr << "warning: discarding all but channel zero\n";
	}

	// Read the audio data from the file.
	std::vector<double> data;
	int numItems = numChannels * 16384;
	std::vector<double> buf(numItems);
	int actualNumItems;
	while ((actualNumItems = sf_read_double(sfFile, &(buf[0]), numItems)) > 0) {
		int n = actualNumItems / numChannels;
		assert(!(actualNumItems % numChannels));
		for (int i = 0; i < n; ++i) {
			data.push_back(buf[numChannels * i]);
		}
	}

	// Ensure that the sample data is in the allowable range.
	if (data.size()) {
		double minVal = *std::min_element(data.begin(), data.end());
		double maxVal = *std::max_element(data.begin(), data.end());
		assert(minVal <= maxVal);
		if (minVal < -1.0 || maxVal > 1.0) {
			std::cerr << "warning: data out of range\n";
			std::cerr << "range: " << minVal << " " << maxVal << "\n";
			assert(minVal >= -1.0 && maxVal <= 1.0);
		}
	}

	// Initialize the sequence.
	samples = RealArray1(data.size(), &(data[0]));

	// Close the audio file.
	sf_close(sfFile);

	return 0;
}

int saveAudioFile(const std::string& fileName, int samplingRate,
  const RealArray1& samples)
{
	// Open the audio file.
	SF_INFO sfInfo;
	sfInfo.frames = samples.getSize();
	sfInfo.sections = 1;
	sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_32;
	//sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_DOUBLE;
	sfInfo.samplerate = samplingRate;
	sfInfo.channels = 1;
	SNDFILE* sfFile;
	if (!(sfFile = sf_open(fileName.c_str(), SFM_WRITE, &sfInfo))) {
		return -1;
	}

	// Ensure that the sample data is in the allowable range.
	bool enableScaling = false;
	double scaleFactor = 1.0;
	if (samples.getSize() > 0) {
		double minVal = *std::min_element(samples.begin(), samples.end());
		double maxVal = *std::max_element(samples.begin(), samples.end());
		assert(minVal <= maxVal);
		if (minVal < -1.0 || maxVal > 1.0) {
			std::cerr << "warning: data out of range\n";
			std::cerr << "range: " << minVal << " " << maxVal << "\n";
			assert(minVal >= -1.0 && maxVal <= 1.0);
			std::cerr << "warning: forcing rescaling of data\n";
			double maxMag = std::max(absVal(minVal), absVal(maxVal));
			scaleFactor = 0.95 * (1.0 / maxMag);
			enableScaling = true;
		}
	}

	// Write the audio data to the file.
	RealSequence1::ConstIterator dataIter = samples.begin();
	std::vector<double> buf(16384);
	int remainCount = samples.getSize();
	while (remainCount > 0) {
		int n = std::min<int>(buf.size(), remainCount);
		std::vector<double>::iterator bufIter = buf.begin();
		for (int i = 0; i < n; ++i) {
			double value = *dataIter;
			if (enableScaling) {
				value *= scaleFactor;
			}
			*bufIter = value;
			++bufIter;
			++dataIter;
		}
		if (sf_write_double(sfFile, &(buf[0]), n) != n) {
			sf_close(sfFile);
			return -1;
		}
		remainCount -= n;
	}

	// Close the audio file.
	sf_close(sfFile);

	return 0;
}

#else

int loadAudioFile(const std::string& fileName, int& samplingRate,
  RealArray1& samples)
{
	return -1;
}

int saveAudioFile(const std::string& fileName, int samplingRate,
  const RealArray1& samples)
{
	return -1;
}

#endif

}

