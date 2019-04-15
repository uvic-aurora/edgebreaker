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

// Generate a Mandelbrot dataset in PNM format.

////////////////////////////////////////////////////////////////////////////////
// Header files
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <complex>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <SPL/Array2.hpp>
#include <SPL/Sequence2.hpp>
#include <SPL/Timer.hpp>

////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////

typedef SPL::Array2<double> RealArray2;
typedef SPL::Array2<int> IntArray2;
typedef std::complex<double> Complex;

////////////////////////////////////////////////////////////////////////////////
// Mandelbrot dataset generation
////////////////////////////////////////////////////////////////////////////////

// Compute function representing Mandelbrot dataset.
RealArray2 mandelbrot(int width, int height, const Complex& botLeft,
  const Complex& topRight)
{
	const int maxIters = 128;

	RealArray2 result(width, height);
	result.fill(0);

	double stepX = (topRight.real() - botLeft.real()) / (width - 1);
	double stepY = (topRight.imag() - botLeft.imag()) / (height - 1);

	for (int y = 0; y < height; ++y) {
		for (RealArray2::XIterator i = result.rowBegin(y);
		  i != result.rowEnd(y); ++i) {
			Complex c = botLeft + Complex((i - result.rowBegin(y)) *
			  stepX, y * stepY);
			int n = 0;
			Complex z(0.0);
			while (std::abs(z) < 2.0 && n < maxIters) {
				z = z * z + c;
				++n;
			}
			*i = static_cast<double>(n) / maxIters;
		}
	}
	return result;
}

// Randomly permute the gray levels in an image.
void permuteGrayLevels(IntArray2& image, int numGrayLevels)
{
	// Create a lookup table for randomly permuting sample values.
	std::vector<int> lookupTable(numGrayLevels);
	for (std::vector<int>::iterator i = lookupTable.begin();
	  i != lookupTable.end(); ++i) {
		*i = i - lookupTable.begin();
	}
	std::random_shuffle(lookupTable.begin(), lookupTable.end());

	// Remap the gray levels in the image.
	for (IntArray2::Iterator i = image.begin();
	  i != image.end(); ++i) {
		assert(*i >= 0 && *i < numGrayLevels);
		*i = lookupTable[*i];
	}
}

////////////////////////////////////////////////////////////////////////////////
// Main program.
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	Complex botLeft(-2.05, -1.2);
	Complex topRight(0.55, 1.2);
	int width = 1024;
	int height = 1024;
	int maxValue = 255;

	// Process the command line.
	if (argc > 1) {
		if (argc != 5) {
			std::cerr << "usage: mandelbrot [xmin ymin xmax ymax]\n";
			exit(2);
		}
		botLeft = Complex(atof(argv[1]), atof(argv[2]));
		topRight = Complex(atof(argv[3]), atof(argv[4]));
	}

	// Compute the function representing the mandelbrot dataset.
	SPL::Timer timer;
	timer.start();
	RealArray2 func = mandelbrot(width, height, botLeft, topRight);
	timer.stop();

	// Convert the real data to integer data.
	func *= static_cast<double>(maxValue);
	IntArray2 image(func);

	// Permute the gray levels (for aesthetic reasons).
	permuteGrayLevels(image, maxValue + 1);

	// Output image in PNM format.
	if (SPL::encodePgm(std::cout, image, maxValue, false)) {
		std::cerr << "cannot write output image\n";
		exit(1);
	}

	std::cerr << "Mandelbrot computation time (seconds): " << timer.get() << "\n";

	exit(0);
}
