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

// This program illustrates the use of timers and memory usage functions.

#include <iostream>
#include <vector>
#include <unistd.h>
#include <SPL/Timer.hpp>

using SPL::Timer;
using SPL::getCurrentMemUsage;
using SPL::getPeakMemUsage;

int main(int argc, char** argv)
{
	Timer timer;
	timer.start();
	sleep(1);
	timer.stop();
	std::cout << "elapsed time " << timer.get() << "\n";

	double memUsage0 = getCurrentMemUsage();
	std::cout << "memory usage " << memUsage0 << "\n";
	char* data = new char[2 * 1024 * 1024];
	double memUsage1 = getCurrentMemUsage();
	std::cout << "memory usage " << memUsage1 << "\n";
	std::cout << "memory usage difference " << (memUsage1 - memUsage0) << "\n";

	double peakMemUsage = getPeakMemUsage();
	std::cout << "peak memory usage " << peakMemUsage << "\n";

	return 0;
}
