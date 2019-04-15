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
#include <SPL/Array1.hpp>
#include <SPL/Sequence1.hpp>

typedef SPL::Array1<double> RealArray1;
typedef SPL::Array1<int> IntArray1;
typedef SPL::Sequence1<double> RealSeq1;
typedef SPL::Sequence1<int> IntSeq1;

void doTest()
{
	static RealArray1::ElemType realData[] = {
		0, 1, 2, 3, 4, 5
	};
	static IntArray1::ElemType intData[] = {
		0, 1, 2, 3, 4, 5
	};
	RealArray1 realArr(6, &realData[0]);

	assert(realArr.getSize() == 6);
	assert(realArr(0) == 0);
	assert(realArr(1) == 1);
	assert(realArr(2) == 2);
	assert(realArr(3) == 3);
	assert(realArr(4) == 4);
	assert(realArr(5) == 5);
	assert(!realArr.isShared());

	IntArray1 intArr(6, &intData[0]);
	assert(intArr(0) == 0);
	assert(intArr(1) == 1);
	assert(intArr(2) == 2);
	assert(intArr(3) == 3);
	assert(intArr(4) == 4);
	assert(intArr(5) == 5);
	assert(!intArr.isShared());

	{
		IntArray1 a0 = intArr;
		IntArray1 a1 = realArr;
	}
	//intArr.dump(std::cout);
	//realArr.dump(std::cout);

	{
		IntArray1 a0 = intArr;
		IntArray1 a1 = realArr;
		assert(a0 == a1);
		a1(0) = 1;
		assert(a0 != a1);
		a1 = realArr;
		assert(a0 == a1);
		a1(0) = 1;
		assert(a0 != a1);
	}

	{
		IntArray1 a0 = intArr;
		a0 += a0;
		for (int i = 0; i < 6; ++i) {
			assert(a0(i) == intData[i] * 2);
		}
		a0 = intArr;
		a0 -= a0;
		for (int i = 0; i < 6; ++i) {
			assert(a0(i) == 0);
		}
		a0 = intArr;
		a0 *= a0;
		for (int i = 0; i < 6; ++i) {
			assert(a0(i) == intData[i] * intData[i]);
		}
		a0 = intArr;
		a0 /= -1;
		for (int i = 0; i < 6; ++i) {
			assert(a0(i) == -intData[i]);
		}
	}

	{
		IntArray1 a0 = intArr;
		IntArray1 a1 = intArr;
		a1 *= 2;
		IntArray1 a2 = a1;
		a2 -= a0;
		for (int i = 0; i < 6; ++i) {
			assert(a2(i) == intData[i]);
		}
	}

	{
		RealArray1 a0 = realArr;
		assert(a0.min() == 0);
		assert(a0.max() == 5);
		assert(a0.sum() == 0 + 1 + 2 + 3 + 4 + 5);
//		assert(a0.median() == 2.5);
	}

	{
		RealArray1 a0 = realArr;
		a0.resize(3, &realData[1]);
		assert(a0(0) == 1);
		assert(a0(1) == 2);
		assert(a0(2) == 3);
		a0.resize(6);
	}

	{
		RealArray1 a0 = realArr;
		std::stringstream tmpStream;
		tmpStream << a0;
		assert(tmpStream);
		tmpStream.seekp(0);
		a0 = RealArray1();
		tmpStream >> a0;
		assert(tmpStream);
		assert(a0 == realArr);
	}

	{
		IntArray1 a0 = intArr;
		IntArray1 a1 = a0;
		a1 *= 2;
		IntArray1 a2 = a0;
		IntArray1 a3 = a1;
		std::cout << "a0 " << a0 << "\n";
		std::cout << "a1 " << a1 << "\n";
		std::cout << "a2 " << a2 << "\n";
		std::cout << "a3 " << a3 << "\n";
		a1.swap(a0);
		std::cout << "a0 " << a0 << "\n";
		std::cout << "a1 " << a1 << "\n";
		std::cout << "a2 " << a2 << "\n";
		std::cout << "a3 " << a3 << "\n";
		assert(a1 == intArr);
		assert(a2 == intArr);
		assert(a3 == a0);
	}

	{
		IntArray1 a0(5, 7);
		a0.fill(15);
	}


	{
		RealArray1 a1 = realArr;
		a1(0) = 1;
		RealArray1 a2;
		a2 = realArr;
		assert(realArr.isShared());
	}

	IntSeq1 intSeq(intArr);
	RealSeq1 realSeq(realArr);

	{
		IntArray1 a0(intSeq.getArray());
		assert(a0 == intArr);
	}

	{
		RealArray1 a0(realSeq.getArray());
		assert(a0 == realArr);
	}

	{
		RealArray1 a0 = realArr;
		RealSeq1 s0 = SPL::convolve(RealSeq1(a0), RealSeq1(a0));
		RealArray1 a1 = SPL::convolve(RealSeq1(a0), RealSeq1(a0)).getArray();
	}

}

void doTest1()
{

#if 0
	static RealArray1::ElemType realData[] = {
		0, 1, 2, 3, 4, 5
	};
	RealArray1 realArr(6, &realData[0]);
	RealArray1 a0 = realArr;
	RealSeq1 s0 = SPL::convolve(RealSeq1(a0), RealSeq1(a0));
	RealArray1 a1 = SPL::convolve(RealSeq1(a0), RealSeq1(a0)).array();
#endif

#if 0
	static RealArray1::ElemType realData[] = {
		0, 1, 2, 3, 4, 5
	};
	RealArray1 realArr(6, &realData[0]);

//RealArray1 a0 = realArr;
//RealArray1 a1 = realArr;
//RealArray1 a2 = realArr;
	RealSeq1 realSeq(0, 10);
	realSeq.fill(1);
	std::cout << "BEFORE SWAP\n";
	std::cout << "realArr " << realArr << " isShared " << realArr.isShared()
	  << "\n";
	std::cout << "realSeq " << realSeq << " isShared " << realSeq.isShared()
	  << "\n";

	realSeq.swapArray(realArr);
	std::cout << "AFTER FIRST SWAP\n";
	std::cout << "realArr " << realArr << " isShared " << realArr.isShared()
	  << "\n";
	std::cout << "realSeq " << realSeq << " isShared " << realSeq.isShared()
	  << "\n";

//	realSeq = SPL::convolve(realSeq, realSeq);
	realSeq += 1;

	realSeq.swapArray(realArr);
	std::cout << "AFTER SECOND SWAP\n";
	std::cout << "realArr " << realArr << " isShared " << realArr.isShared()
	  << "\n";
	std::cout << "realSeq " << realSeq << " isShared " << realSeq.isShared()
	  << "\n";
#endif

#if 1
	RealArray1 dummy;
	static RealArray1::ElemType realData[] = {
		0, 1, 2, 3, 4, 5
	};
	RealArray1 realArr(6, &realData[0]);
	std::cout << "START\n";
	SPL::convolve(RealSeq1(realArr), RealSeq1(realArr)).swapArray(dummy);
	std::cout << "END\n";

#endif


}

int main(int argc, char** argv)
{
	doTest();
//	doTest1();
	return 0;
}
