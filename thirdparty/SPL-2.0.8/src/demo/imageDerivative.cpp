// Copyright (c) 2018 Michael D. Adams
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

// This program designs several basic types of filters.

#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <SPL/Sequence1.hpp>
#include <SPL/Sequence2.hpp>
#include <SPL/Array2.hpp>
#include <SPL/filterDesign.hpp>

char *cmdName = 0;

void usage()
{
	std::cerr << "bad usage\n";
	std::cerr <<
	  "options:\n"
	  "-x x_order\n"
	  "    Set the order of the derivative with respect to x.\n"        
	  "-y y_order\n"
	  "    Set the order of the derivative with respect to y.\n"        
	  "-m filtering_method\n"
	  "    Set the filtering method.\n"
	  "-C conv_mode\n"
	  "    Set the convolution mode for all filtering operations.\n"
	  "-S smooth_conv_mode\n"
	  "    Set the convolution mode for smoothing operations.\n"
	  "-D deriv_conv_mode\n"
	  "    Set the convolution mode for derivative operations.\n"
	  "-v\n"
	  "    Increase the verbosity level of the output.\n"
	  "-I\n"
	  "    Format standard output as an image.\n"
	  ;
	std::exit(2);
}

template <class T>
T clip(T x, T min, T max)
{
	if (x < min) {
		return min;
	} else if (x > max) {
		return max;
	} else {
		return x;
	}
}

std::string conv_mode_name(int conv_mode)
{
	switch (conv_mode) {
	case SPL::ConvolveMode::full:
		return "full";
		break;
	case SPL::ConvolveMode::sameDomainZeroExt:
		return "sameDomainZeroExt";
		break;
	case SPL::ConvolveMode::sameDomainConstExt:
		return "sameDomainConstExt";
		break;
	case SPL::ConvolveMode::sameDomainPerExt:
		return "sameDomainPerExt";
		break;
	case SPL::ConvolveMode::sameDomainSymExt0:
		return "sameDomainSymExt0";
		break;
	default:
		return "";
		break;
	}
}

int string_to_conv_mode(const std::string& s)
{
	if (s == "full") {
		return SPL::ConvolveMode::full;
	} else if (s == "same_zero_ext") {
		return SPL::ConvolveMode::sameDomainZeroExt;
	} else if (s == "same_const_ext") {
		return SPL::ConvolveMode::sameDomainConstExt;
	} else if (s == "same_per_ext") {
		return SPL::ConvolveMode::sameDomainPerExt;
	} else if (s == "same_sym_ext") {
		return SPL::ConvolveMode::sameDomainSymExt0;
	} else {
		abort();
	}
}

template <class T>
SPL::Sequence1<T> make_sequence1(int offset, int length,
  std::initializer_list<T> values)
{
	std::vector<T> data(values);
	return SPL::Sequence1<T>(offset, length, data.data());
}

// NOTE: This function is not fully implemented.
void get_smoothing_and_derivative_filters(
  int kernel_size, int x_order, int y_order,
  SPL::RealSequence1& horz_smooth_filter,
  SPL::RealSequence1& vert_smooth_filter,
  SPL::RealSequence1& horz_deriv_filter,
  SPL::RealSequence1& vert_deriv_filter)
{
	assert(kernel_size >= 0);
	assert(x_order >= 0 && y_order >= 0);

	// For now, different kernel sizes are not supported.
	assert(kernel_size == 0);

	if (x_order == 1 && y_order == 0) {
		auto smooth = make_sequence1(-1, 3, {0.25, 0.5, 0.25});
		auto deriv = make_sequence1(-1, 3, {-1.0, 0.0, 1.0});
		auto delta = make_sequence1(0, 1, {1.0});
		horz_smooth_filter = smooth;
		vert_smooth_filter = smooth;
		horz_deriv_filter = deriv;
		vert_deriv_filter = delta;
	} else if (x_order == 2 && y_order == 0) {
		auto smooth = make_sequence1(-1, 3, {0.25, 0.5, 0.25});
		auto deriv = make_sequence1(-1, 3, {1.0, -2.0, 1.0});
		auto delta = make_sequence1(0, 1, {1.0});
		horz_smooth_filter = smooth;
		vert_smooth_filter = smooth;
		horz_deriv_filter = deriv;
		vert_deriv_filter = delta;
	} else {
		// Not currently implemented.
		assert(false);
	}
}

bool output_as_image(const std::string& filename, SPL::RealSequence2& seq,
  double scale, int maxVal)
{
	bool sgnd = false;
	SPL::RealSequence2 image_seq = seq;
	for (SPL::RealSequence2::Iterator i = image_seq.begin();
	  i != image_seq.end(); ++i) {
		// Take the absolute value since the derivatives can be
		// negative (or positive).
		*i = SPL::absVal(*i);
		//*i = clip(*i, 0.0, 255.0);
	}
	// Scale so that a low-intensity image will still be visible.
	if (scale < 0) {
		image_seq /= image_seq.max();
	} else {
		image_seq *= scale;
	}
	image_seq *= maxVal;
	for (SPL::RealSequence2::Iterator i = image_seq.begin();
	  i != image_seq.end(); ++i) {
		*i = clip<int>(*i, 0, maxVal);
	}
	std::ofstream out(filename.c_str());
	if (encodePgm(out, image_seq.getArray(), maxVal, sgnd, false)) {
		std::cerr << "cannot write PGM data\n";
		return false;
	}
	return true;
}

int main(int argc, char** argv)
{
	bool format_as_image = false;
	int conv_mode = SPL::ConvolveMode::sameDomainZeroExt;
	int smooth_conv_mode = SPL::ConvolveMode::sameDomainZeroExt;
	int deriv_conv_mode = SPL::ConvolveMode::sameDomainZeroExt;
	int verbose = 0;
	int method = 0;
	int x_order = -1;
	int y_order = -1;
	double scale = -1.0;
	std::string intermediate_filename;
	std::string final_filename;
	int kernel_size = 0;

	int c;
	while ((c = getopt(argc, argv, "i:f:C:S:D:Im:vx:y:s:")) >= 0) {
		switch (c) {
		case 'v':
			++verbose;
			break;
		case 'I':
			format_as_image = 1;
			break;
		case 'C':
			conv_mode = string_to_conv_mode(optarg);
			smooth_conv_mode = conv_mode;
			deriv_conv_mode = conv_mode;
			break;
		case 'S':
			smooth_conv_mode = string_to_conv_mode(optarg);
			break;
		case 'D':
			deriv_conv_mode = string_to_conv_mode(optarg);
			break;
		case 'm':
			method = atoi(optarg);
			break;
		case 'x':
			x_order = atoi(optarg);
			break;
		case 'y':
			y_order = atoi(optarg);
			break;
		case 's':
			scale = atof(optarg);
			break;
		case 'i':
			intermediate_filename = optarg;
			break;
		case 'f':
			final_filename = optarg;
			break;
		default:
			usage();
			break;
		}
	}

	if (x_order < 0 || y_order < 0) {
		usage();
	}
	if (!((x_order == 1 || x_order == 2) && y_order == 0)) {
		std::cerr << "not yet implemented\n";
		usage();
	}

	SPL::RealSequence1 horz_smooth_filter;
	SPL::RealSequence1 vert_smooth_filter;
	SPL::RealSequence1 horz_deriv_filter;
	SPL::RealSequence1 vert_deriv_filter;

	get_smoothing_and_derivative_filters(kernel_size, x_order, y_order,
	  horz_smooth_filter, vert_smooth_filter, horz_deriv_filter,
	  vert_deriv_filter);

	SPL::RealSequence1 horz_filter = SPL::convolve(horz_smooth_filter,
	  horz_deriv_filter, SPL::ConvolveMode::full);
	SPL::RealSequence1 vert_filter = SPL::convolve(vert_smooth_filter,
	  vert_deriv_filter, SPL::ConvolveMode::full);

	std::cerr
	  << "smooth conv mode: " << conv_mode_name(smooth_conv_mode) << '\n'
	  << "deriv  conv mode: " << conv_mode_name(deriv_conv_mode) << '\n'
	  ;
	std::cerr
	  << "horizontal smoothing  filter: " << horz_smooth_filter << '\n'
	  << "vertical   smoothing  filter: " << vert_smooth_filter << '\n'
	  << "horizontal derivative filter: " << horz_deriv_filter << '\n'
	  << "vertical   derivative filter: " << vert_deriv_filter << '\n'
	  << "horizontal            filter: " << horz_filter << '\n'
	  << "vertical              filter: " << vert_filter << '\n'
	  ;

	SPL::RealArray2 data;
	int maxVal;
	bool sgnd;
	if (decodePgm(std::cin, data, maxVal, sgnd)) {
		std::cerr << "cannot read PGM data\n";
		return 1;
	}

	SPL::RealSequence2 seq(data);

	SPL::RealSequence2 intermediate_result;
	SPL::RealSequence2 final_result;
	switch (method) {
	case 0:
		// Apply the smoothing and derivative filters together.
		std::cerr
		  << "Applying the smoothing and derivative filters together.\n";
		intermediate_result = convolveSeparable(seq, horz_filter, vert_filter,
		  conv_mode);
		final_result = intermediate_result;
		break;
	case 1:
		// Apply the smoothing filter and then the derivative filter.
		std::cerr
		  << "Applying the smoothing filter and then the derivative filter.\n";
		intermediate_result = convolveSeparable(seq, horz_smooth_filter,
		  vert_smooth_filter, smooth_conv_mode);
		final_result = convolveSeparable(intermediate_result,
		  horz_deriv_filter, vert_deriv_filter, deriv_conv_mode);
		break;
	case 2:
		// Apply the derivative filter and then the smoothing filter.
		std::cerr
		  << "Applying the derivative filter and then the smoothing filter.\n";
		intermediate_result = convolveSeparable(seq, horz_deriv_filter,
		  vert_deriv_filter, deriv_conv_mode);
		final_result = convolveSeparable(intermediate_result,
		  horz_smooth_filter, vert_smooth_filter, smooth_conv_mode);
		break;
	}

	if (verbose) {
		std::cerr << "A:\n" << intermediate_result << '\n';
	}

	if (intermediate_filename.size()) {
		std::cerr << "saving intermediate result as image\n";
		if (!output_as_image(intermediate_filename, intermediate_result, scale,
		  maxVal)) {
			std::cerr << "cannot write image file\n";
			return 1;
		}
	}
	if (final_filename.size()) {
		std::cerr << "saving final result as image\n";
		if (!output_as_image(final_filename, final_result, scale, maxVal)) {
			std::cerr << "cannot write image file\n";
			return 1;
		}
	}

	if (verbose) {
		for (int j = 0; j < final_result.getHeight(); j++) {
			for (int i = 0; i < final_result.getWidth(); ++i) {
				std::cout << std::setw(6) << std::setprecision(3)
				  << final_result(i, j);
			}
			std::cout << '\n';
		}
	}

	if (true) {
		std::vector<double> values;
		for (int i = 0; i < final_result.getWidth(); ++i) {
			values.push_back(final_result(i, 0));
			values.push_back(final_result(i, final_result.getHeight() - 1));
		}
		for (int j = 1; j < final_result.getHeight() - 1; j++) {
			values.push_back(final_result(0, j));
			values.push_back(final_result(final_result.getWidth() - 1, j));
		}
		for (auto&& x : values) {
			x = std::fabs(x);
		}
		double sum = 0;
		for (auto x : values) {
			sum += x;
		}
		std::sort(values.begin(), values.end());
		std::cerr
		  << "average magnitude " << sum / values.size() << "\n"
		  << "approximate median magnitude " << values[values.size() / 2]
		  << "\n";
	}

	return 0;
}
