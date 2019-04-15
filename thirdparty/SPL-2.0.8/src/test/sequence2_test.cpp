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

#include <iostream>
#include <SPL/Array2.hpp>
#include <SPL/Sequence2.hpp>
#include <SPL/filterDesign.hpp>

typedef SPL::Array2<double> RealArray2;
typedef SPL::Sequence2<double> RealSeq2;
typedef SPL::Sequence1<double> RealSeq1;

RealArray2 getArray(int width, int height)
{
	int n = 0;
	RealArray2 a(width, height);
	for (int y = 0; y < height; ++y) {
		RealArray2::XIterator i = a.rowBegin(y);
		for (int x = 0; x < width; ++x) {
			*i = n;
			++n;
			++i;
		}
	}
	return a;
}

RealArray2 getArray2(int width, int height)
{
	int n = 0;
	RealArray2 a(width, height);
	for (int x = 0; x < width; ++x) {
		RealArray2::YIterator i = a.colBegin(x);
		for (int y = 0; y < height; ++y) {
			*i = n;
			++n;
			++i;
		}
	}
	return a;
}

void doTest()
{

	{
		RealArray2 x(13, 17);
		RealArray2 y = x;
		assert(y == x);
	}

	{
		RealArray2 x = getArray(2, 3);
		assert(x(0, 0) == 0);
		assert(x(1, 0) == 1);
		assert(x(0, 1) == 2);
	}

	{
		RealArray2 x = getArray(3, 2);
		RealArray2 x1 = x;
		RealArray2 x2 = x;
		RealArray2 x3 = x;
		RealArray2 x4 = x;
		x1(0, 0) = 1;
		x2(0, 0) = 2;
		x3(0, 0) = 3;
		x4(0, 0) = 4;
		assert(x(0, 0) == 0);
	}

	{
		RealArray2 x = getArray(3, 2);
		RealArray2 y = getArray(3, 2);
		x += y;
		x *= y;
		x -= y;
		std::cout << "x " << x << "\n";
		std::cout << "y " << y << "\n";
	}

	{
		std::cout << "test\n";
		RealArray2 x(2, 2, 0.0);
		x += 2;
		std::cout << "x " << x << "\n";
		x -= 1;
		std::cout << "x " << x << "\n";
		x *= 2;
		std::cout << "x " << x << "\n";
		x /= 2;
		std::cout << "x " << x << "\n";
	}

}

void doTest2()
{
	RealArray2 x;
	std::cin >> x;
	std::cout << x;
}

void doTest9()
{
		static RealSeq2::ElemType seqData[] = {
			1, 1, 1, 1,
			1, 2, 2, 1,
			1, 2, 2, 1,
			1, 1, 1, 1
		};
		static RealSeq1::ElemType filtData0[] = {1, 1};
		static RealSeq1::ElemType filtData1[] = {1};
		//static RealSeq1::ElemType filtData[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
		RealSeq2 seq(0, 0, 4, 4, seqData);
		RealSeq1 horzFilt(0, sizeof(filtData0) / sizeof(RealSeq1::ElemType),
		  filtData0);
		RealSeq1 vertFilt(0, sizeof(filtData1) / sizeof(RealSeq1::ElemType),
		  filtData1);
		vertFilt = RealSeq1();
//		seq = translate(seq, -5);
		horzFilt = translate(horzFilt, 10);
		vertFilt = translate(vertFilt, -10);
		std::cout << "seq " << seq << "\n";
		std::cout << "horzFilt " << horzFilt << "\n";
		std::cout << "vertFilt " << vertFilt << "\n";
		RealSeq2 result1 = convolveSeparable(seq, horzFilt, vertFilt);
		std::cout << result1 << "\n";
}

int main(int argc, char** argv)
{
doTest9();
//	doTest();
//	doTest2();
	return 0;
}

