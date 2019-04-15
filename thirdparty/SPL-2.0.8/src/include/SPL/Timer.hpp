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
 * @file Timer.hpp
 * @brief This file contains code for the Timer class.
 */

#ifndef SPL_Timer_hpp
#define SPL_Timer_hpp

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#include <SPL/config.hpp>
#include <iostream>
#include <cstdlib>
#if defined(SPL_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

namespace SPL {

/**
 * @addtogroup Profiling
 * @{
 */

////////////////////////////////////////////////////////////////////////////////
// Timer Class
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief A class for making timing measurements.
 */

class Timer
{
public:

	/**
	 * @brief Start the timer.
	 *
	 * Effects:
	 * Starts the timer.
	 * The timer should not already be running.
	 */
	void start();

	/**
	 * @brief Stop the timer.
	 *
	 * Effects:
	 * Stops the timer.
	 * The timer should already be running.
	 */
	void stop();

	/**
	 * @brief Get the timer value.
	 *
	 * Effects:
	 * Query the elapsed time measured by the timer.
	 *
	 * Returns:
	 * The elapsed time in seconds is returned.
	 * The resolution of the timer depends on the particular platform
	 * (e.g., operating system, hardware, etc.).
	 * For most mainstream platforms, the resolution of the timer is
	 * typically microseconds.
	 */
	double get() const;

private:
	// Do not look at private members.  Doing so will make your eyes bleed!
#if defined(SPL_HAVE_GETTIMEOFDAY)
	struct timeval startTime_;
	struct timeval stopTime_;
#endif
};

#if defined(SPL_HAVE_GETTIMEOFDAY)

inline void Timer::start()
{
	if (gettimeofday(&startTime_, 0)) {
		// The gettimeofday function has failed.
		// Ensure that the timer is initialized to some sane state.
		std::cerr << "warning: gettimeofday failed\n";
		startTime_.tv_sec = 0;
		startTime_.tv_usec = 0;
	}
}

inline void Timer::stop()
{
	if (gettimeofday(&stopTime_, 0)) {
		// The gettimeofday function has failed.
		// Ensure that the timer is initialized to some sane state.
		std::cerr << "warning: gettimeofday failed\n";
		stopTime_.tv_sec = 0;
		stopTime_.tv_usec = 0;
	}
}

inline double Timer::get() const
{
	double t0;
	double t1;
	t0 = ((double) startTime_.tv_sec) + ((double) startTime_.tv_usec) / 1e6;
	t1 = ((double) stopTime_.tv_sec) + ((double) stopTime_.tv_usec) / 1e6;
	return t1 - t0;
}

#endif

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get the amount of memory currently being used by the process.
 *
 * Effects:
 * Query the total amount of memory currently being used by the process.
 *
 * Returns:
 * The amount of memory (in bytes) currently being used by the process
 * is returned.
 */

double getCurrentMemUsage();

/**
 * @brief Get the peak memory usage for the process.
 *
 * Effects:
 * Query the peak memory usage for the process.
 *
 * Returns:
 * The peak memory usage for the process (in bytes) is returned.
 */

double getPeakMemUsage();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @}
 */

}

#endif

