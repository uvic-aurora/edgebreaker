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
#include <fstream>
#include <SPL/Array2.hpp>

typedef SPL::Array2<double> RealArray2;
typedef SPL::Array2<int> IntArray2;

void doTest()
{
	static RealArray2::ElemType realData[] = {
		0, 1, 2,
		3, 4, 5
	};
	static RealArray2::ElemType realData2[] = {
		1, 2, 3,
		4, 5, 6
	};
	static IntArray2::ElemType intData[] = {
		0, 1, 2,
		3, 4, 5
	};
	RealArray2 realArr(3, 2, &realData[0]);

	assert(realArr.getSize() == 6);
	assert(realArr(0, 0) == 0);
	assert(realArr(1, 0) == 1);
	assert(realArr(2, 0) == 2);
	assert(realArr(0, 1) == 3);
	assert(realArr(1, 1) == 4);
	assert(realArr(2, 1) == 5);
	assert(!realArr.isShared());

	IntArray2 intArr(3, 2, &intData[0]);
	assert(intArr(0, 0) == 0);
	assert(intArr(1, 0) == 1);
	assert(intArr(2, 0) == 2);
	assert(intArr(0, 1) == 3);
	assert(intArr(1, 1) == 4);
	assert(intArr(2, 1) == 5);
	assert(!intArr.isShared());

	{
		IntArray2 a0 = intArr;
		IntArray2 a1 = realArr;
	}

	{
		IntArray2 a0 = intArr;
		IntArray2::ConstXIterator i;
		i = a0.rowBegin(1);
		//////////
		std::cout << *i << "\n";
		assert(i != a0.rowEnd(1));
		assert(*i == 3);
		++i;
		//////////
		std::cout << *i << "\n";
		assert(i != a0.rowEnd(1));
		assert(*i == 4);
		++i;
		//////////
		std::cout << *i << "\n";
		assert(i != a0.rowEnd(1));
		assert(*i == 5);
		++i;
		//////////
		assert(i == a0.rowEnd(1));
	}

	std::cout << "\n";

	{
		IntArray2 a0 = intArr;
		std::cout << "start\n";
		IntArray2::ConstYIterator i;
		i = a0.colBegin(1);
		//////////
		std::cout << *i << "\n";
		assert(i != a0.colEnd(1));
		assert(*i == 1);
		++i;
		//////////
		std::cout << *i << "\n";
		assert(i != a0.colEnd(1));
		assert(*i == 4);
		++i;
		//////////
		assert(i == a0.colEnd(1));
		std::cout << "end\n";
	}

	{
		IntArray2 a0 = intArr;
		IntArray2 a1 = realArr;
		assert(a0 == a1);
		a1(0, 0) = 1;
		assert(a0 != a1);
		a1 = realArr;
		assert(a0 == a1);
		a1(0, 0) = 1;
		assert(a0 != a1);
	}

	{
		IntArray2 a0 = intArr;
		a0 += a0;
		assert(a0(1, 0) == 2);
		a0 = intArr;
		a0 -= a0;
		assert(a0(1, 0) == 0);
		a0 = intArr;
		a0 *= a0;
		assert(a0(1, 0) == 1);
		a0 = intArr;
		a0 /= -1;
		assert(a0(1, 0) == -1);
	}

	{
		IntArray2 a0 = intArr;
		IntArray2 a1 = intArr;
		a1 *= 2;
		IntArray2 a2 = a1;
		a2 -= a0;
		int k = 0;
		for (int j = 0; j < 2; ++j) {
			for (int i = 0; i < 3; ++i) {
				assert(a2(i, j) == intData[k]);
				++k;
			}
		}
	}

	{
		RealArray2 a0 = realArr;
		assert(a0.min() == 0);
		assert(a0.max() == 5);
		assert(a0.sum() == 0 + 1 + 2 + 3 + 4 + 5);
//		assert(a0.median() == 2.5);
	}

	{
		RealArray2 a0 = realArr;
		a0.resize(2, 3, &realData2[0]);
		assert(a0(0, 0) == 1);
		assert(a0(1, 0) == 2);
		assert(a0(0, 1) == 3);
		assert(a0(1, 1) == 4);
		assert(a0(0, 2) == 5);
		assert(a0(1, 2) == 6);
		a0.resize(3, 2);
	}

	{
		RealArray2 a0 = realArr;
		std::stringstream tmpStream;
		tmpStream << a0;
		assert(tmpStream);
		a0 = RealArray2();
		tmpStream >> a0;
		assert(tmpStream);
		assert(a0 == realArr);
	}

	{
		IntArray2 a0 = intArr;
		IntArray2 a1 = a0;
		a1 *= 2;
		IntArray2 a2 = a0;
		IntArray2 a3 = a1;
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
		IntArray2 a0 = intArr;
		IntArray2 a1;
		IntArray2 a2;
		a2 = a1 = a0;
		assert(a0 == intArr);
		assert(a1 == intArr);
		assert(a2 == intArr);
	}

	{
		IntArray2 a0 = intArr;
		RealArray2 a1 = a0;
		assert(a0 == intArr);
		assert(a1 == realArr);
		RealArray2 a2;
		a2 = a1 = a0;
		assert(a0 == intArr);
		assert(a1 == realArr);
		assert(a2 == realArr);
	}

	{
		IntArray2 a0(5, 7);
		a0.fill(15);
	}


	{
		RealArray2 a1 = realArr;
		a1(0, 0) = 1;
		RealArray2 a2;
		a2 = realArr;
		assert(realArr.isShared());
	}

	{
		IntArray2 a0 = intArr;
		a0.flipud();
		assert(a0(0, 1-0) == 0);
		assert(a0(1, 1-0) == 1);
		assert(a0(2, 1-0) == 2);
		assert(a0(0, 1-1) == 3);
		assert(a0(1, 1-1) == 4);
		assert(a0(2, 1-1) == 5);
		a0 = intArr;
std::cout << "a0 " << a0 << "\n";
		a0.fliplr();
std::cout << "a0 " << a0 << "\n";
		assert(a0(2-0, 0) == 0);
		assert(a0(2-1, 0) == 1);
		assert(a0(2-2, 0) == 2);
		assert(a0(2-0, 1) == 3);
		assert(a0(2-1, 1) == 4);
		assert(a0(2-2, 1) == 5);
	}

	for (int binaryFormat = 1; binaryFormat >= 0; --binaryFormat)
	{
		static IntArray2::ElemType data[] = {
			0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1,
			0, 1, 0, 1, 0, 1, 0, 1,
		};
		IntArray2 a0(9, 6, &data[0]);
		IntArray2 a1;
		int maxVal;
		bool sgnd;

		{
			std::stringstream s;
			std::cout << "start\n";
			s.seekp(0);
			if (encodePbm(s, a0, binaryFormat)) {
				assert(0);
			}
			std::cout << "a0 " << a0 << "\n";
			s.seekp(0);
			if (decodePbm(s, a1)) {
				assert(0);
			}
			std::cout << "a1 " << a1 << "\n";
			assert(a0 == a1);
			std::cout << "end\n";
		}

		{
			std::stringstream s;
			std::cout << "start\n";
			s.seekp(0);
			if (encodePgm(s, a0, 255, false, binaryFormat)) {
				assert(0);
			}
			std::cout << "a0 " << a0 << "\n";
			s.seekp(0);
			if (decodePgm(s, a1, maxVal, sgnd)) {
				assert(0);
			}
			std::cout << "a1 " << a1 << "\n";
			assert(a0 == a1);
			std::cout << "end\n";
		}

		{
			std::stringstream s;
			std::cout << "start\n";
			s.seekp(0);
			if (encodePpm(s, a0, a0, a0, 255, false, binaryFormat)) {
				assert(0);
			}
			std::cout << "a0 " << a0 << "\n";
			s.seekp(0);
			if (decodePpm(s, a0, a0, a0, maxVal, sgnd)) {
				assert(0);
			}
			std::cout << "a1 " << a1 << "\n";
			assert(a0 == a1);
			std::cout << "end\n";
		}

	}

	{
		static IntArray2::ElemType data0[] = {
			0, 1, 2,
			3, 4, 5
		};
		static IntArray2::ElemType data1[] = {
			0, 3,
			1, 4,
			2, 5
		};
		IntArray2 a0(3, 2, data0);
		IntArray2 a1(2, 3, data1);
		std::cout << "a0 " << a0 << "\n";
		std::cout << "a1 " << a1 << "\n";
		assert(a1 == transpose(a0));
	}


}

int main(int argc, char** argv)
{
	doTest();
	std::cerr << "ALL TESTS PASSED\n";
	return 0;
}
