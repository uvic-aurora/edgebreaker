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
#include <SPL/filterDesign.hpp>

using SPL::Sequence1;
typedef SPL::RealSequence1 RealSeq1;

template <class T, class OtherT>
void doTest()
{
	static typename Sequence1<T>::ElemType data[] = {
		1, 2, 3, 4, 5, 6, 7, 8
	};
	static typename Sequence1<OtherT>::ElemType othData[] = {
		1, 2, 3, 4, 5, 6, 7, 8
	};
	Sequence1<T> seq(1, 8, data);
	Sequence1<OtherT> othSeq(1, 8, othData);

	for (int i = 1; i <= 8; ++i) {
		assert(seq(i) == i);
		assert(othSeq(i) == i);
	}
	assert(seq == Sequence1<T>(othSeq));
	assert(Sequence1<T>(othSeq) == seq);
	assert(Sequence1<OtherT>(seq) == othSeq);
	assert(othSeq == Sequence1<OtherT>(seq));

	{
		Sequence1<T> f0;
		assert(f0.getSize() == 0);
		static typename Sequence1<T>::ElemType data[] = {0, 1, 2};
		Sequence1<T> f1(1, 3, data);
		assert(f1(1) == 0);
		assert(f1(2) == 1);
		assert(f1(3) == 2);
		Sequence1<T> f2(-1, 2);
		f2.fill(3);
		assert(f2(-1) == 3);
		assert(f2(0) == 3);
		Sequence1<T> f3(-1, 2, T(7));
		assert(f3(-1) == 7);
		assert(f3(0) == 7);
		Sequence1<T> f4(f3);
		assert(f4 == f3);
		f4 = f3;
		assert(f4 == f3);
	}

	{
		static typename Sequence1<T>::ElemType data[] = {0, 1, 2};
		Sequence1<T> f1(1, 3, data);
		Sequence1<T> f2(f1);
		assert(f2 == f1);
		f2 = f1;
		assert(f2 == f1);
	}

	{
		Sequence1<T> f0(0, 2);
		f0.fill(0);
		Sequence1<T> f1 = f0;
		f1 += 1;
		assert(f1 == Sequence1<T>(0, 2, T(1)));
		f1 *= 6;
		assert(f1 == T(6) * Sequence1<T>(0, 2, T(1)));
		f1 /= 3;
		assert(f1 == T(2) * Sequence1<T>(0, 2, T(1)));
		assert(f1(0) == 2);
		assert(f1(1) == 2);
	}

}

void doTest1(int n)
{
		static RealSeq1::ElemType seqData[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		static RealSeq1::ElemType filtData[] = {1, 1, 1, 1};
		//static RealSeq1::ElemType filtData[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
		RealSeq1 seq(0, sizeof(seqData) / sizeof(RealSeq1::ElemType), seqData);
		RealSeq1 filt(0, sizeof(filtData) / sizeof(RealSeq1::ElemType), filtData);
		seq = translate(seq, 0);
		filt = translate(filt, n);
		std::cout << "seq " << seq << "\n";
		std::cout << "filt " << filt << "\n";
		int mode = SPL::ConvolveMode::full;
		mode = SPL::ConvolveMode::sameDomainZeroExt;
//		mode = SPL::ConvolveMode::full;
		RealSeq1 result = convolve(seq, filt, mode);
		std::cout << result << "\n";
}

void doTest2()
{
		static RealSeq1::ElemType seqData[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		static RealSeq1::ElemType filtData[] = {1, 1, 1, 1};
		//static RealSeq1::ElemType filtData[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
//		RealSeq1 seq(0, sizeof(seqData) / sizeof(RealSeq1::ElemType), seqData);
//		RealSeq1 filt(0, sizeof(filtData) / sizeof(RealSeq1::ElemType), filtData);

		RealSeq1 seq(0, 441000, 1.0);
		seq = RealSeq1(0, 10, 1.0);
		for (RealSeq1::Iterator i = seq.begin(); i != seq.end(); ++i) {
			*i = drand48();
//*i =  rand() % 20;
		}
		std::cout << "seq:\n" << seq << "\n";
		RealSeq1 filt(-206, 413, 2.0);
		filt = RealSeq1(-5, 10, 1.0);
		for (RealSeq1::Iterator i = filt.begin(); i != filt.end(); ++i) {
			*i = drand48();
//*i =  rand() % 20;
		}
		std::cout << "filt:\n" << filt << "\n";

//		for (int i = -1; i < 1; ++i) {
//std::cerr << "i " << i << "\n";
//			for (int j = -300; j < 300; ++j) {
//				seq = translate(seq, i);
//				filt = translate(filt, j);
				int mode;
				mode = SPL::ConvolveMode::sameDomainZeroExt;
				RealSeq1 result = convolve(seq, filt, mode);
//			}
//		}
}

void doTest3()
{
	RealSeq1 seq(-1, 5, 7.0);
	std::cout << "seq " << seq << "\n";
	std::cout << "down " << downsample(seq, 2) << "\n";
	std::cout << "XXX " << (-5/2) << "\n";
}

int main(int argc, char** argv)
{
#ifdef NDEBUG
	abort();
#endif
	doTest<int, double>();
	doTest<double, int>();
doTest2();
doTest3();
//	doTest1(atoi(argv[1]));
	std::cerr << "ALL TESTS PASSED\n";
	return 0;
}
