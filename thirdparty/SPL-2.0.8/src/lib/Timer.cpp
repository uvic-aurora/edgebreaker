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
 * @file Timer.cpp
 * @brief The file contains code for obtaining timing/memory usage information.
 */

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <SPL/Timer.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <iterator>
#include <vector>
#include <string>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

namespace SPL {

#if defined(SPL_HAVE_PROCFS)

static int getProcStatusInfo(const std::string& tag,
  std::vector<std::string>& values)
{
	values.clear();

	std::string trueTag = tag + ":";

	// Get the process ID.
	int pid = getpid();

	// Read the total memory size (in pages) from /proc/$pid/statm.
	std::string fileName = std::string("/proc/") +
	  boost::lexical_cast<std::string>(pid) + std::string("/") +
	  std::string("status");
	std::ifstream inStream(fileName.c_str(), std::ifstream::in);

	std::vector<std::string> tokens;
	for (;;) {

		tokens.clear();

		std::string lineBuf;
		getline(inStream, lineBuf);
		if (!inStream) {
			return -1;
		}

		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

		boost::char_separator<char> sep(" \t");
		tokenizer tok(lineBuf, sep);
		tokenizer::iterator i = tok.begin();
		for (tokenizer::iterator i = tok.begin(); i != tok.end(); ++i) {
			tokens.push_back(*i);
		}

		if (tokens.size() > 0 && tokens[0] == trueTag) {
			break;
		}
	}

	values.clear();
	std::copy(tokens.begin() + 1, tokens.end(), std::back_inserter(values));

#if 0
	for (std::vector<std::string>::const_iterator i = values.begin();
	  i != values.end(); ++i) {
		std::cerr << "XXX " << (i - values.begin()) << " " << *i << "\n";
	}
#endif

	return 0;
}

double getPeakMemUsage()
{
	std::vector<std::string> values;
	if (getProcStatusInfo(std::string("VmPeak"), values)) {
		return -1;
	}
	if (values.size() < 2) {
		return -1;
	}
	if (values[1] != "kB") {
		std::cerr << "unknown units " << values[1] << "\n";
		return -1;
	}
	return static_cast<double>(atol(values[0].c_str())) * 1024;
}

double getCurrentMemUsage()
{
	// Get the process ID.
	int pid = getpid();

	// Read the total memory size (in pages) from /proc/$pid/statm.
	std::string fileName = std::string("/proc/") +
	  boost::lexical_cast<std::string>(pid) + std::string("/") +
	  std::string("statm");
	std::ifstream inStream(fileName.c_str(), std::ifstream::in);
	long numPages;
	inStream >> numPages;
	if (!inStream) {
		// This probably should never occur on a Linux system.
		return -1;
	}

	// Return the memory size in bytes.
	return numPages * getpagesize();
}

#else

double getPeakMemUsage()
{
	return -1;
}

double getCurrentMemUsage()
{
	return -1;
}

#endif

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

}

