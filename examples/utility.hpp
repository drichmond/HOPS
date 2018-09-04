// ----------------------------------------------------------------------
// Copyright (c) 2018, The Regents of the University of California All
// rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//
//     * Neither the name of The Regents of the University of California
//       nor the names of its contributors may be used to endorse or
//       promote products derived from this software without specific
//       prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REGENTS OF THE
// UNIVERSITY OF CALIFORNIA BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
// ----------------------------------------------------------------------
// **********************************************************************
// FILENAME :
//       utility.hpp
//
// DESCRIPTION :
//       This file implements several functions that are useful for
//       functional verification and testing.
//       
// FUNCTIONS :
//       void    fill_normal(std::array<T, LEN>)
//       int     check(std::array<T, LEN>, std::array<T, LEN>)
//
// NOTES :
//        This file requires a compiler implementing the C++11 standard.
//
// AUTHOR : Dustin Richmond (drichmond)
//
// **********************************************************************
#ifndef __UTILITY_HPP
#define __UTILITY_HPP
#include <random>
#include <algorithm>
#include <array>

// FUNCTION :
//       fill_normal
//
// DESCRIPTION :
//       Fills a std::array of length LEN from a normal distribution
//       with mean 0, and a standard deviation of LEN/2
//
// PARAMETERS :
//        SEED : int
//            The initialization value for the mersenne
//            twister. Defaults to 42.
//
//        T : typename
//            Type of the std::array to fill. Can be inferred
//            from the call-site.
//
//        LEN : std::size_t
//            Length of the std::array to fill. Can be inferred from
//            the call-site
//
// ARGUMENTS :
//        L : std::array<T, LEN>&
//            The array to fill with values.
// RETURNS :
//        Nothing (void)
template <int SEED = 42, typename T, std::size_t LEN>
void fill_normal(std::array<T, LEN> &L) {
	std::mt19937 gen(SEED);
	std::normal_distribution<float> distribution(0, LEN/2);
	for(std::size_t i = 0; i < LEN; i++) {
		L[i] = static_cast<T>(distribution(gen));
	}
}

// FUNCTION :
//       check
//
// DESCRIPTION :
//       Compares two arrays and returns a non-zero value if their
//       values do not match.
//
// PARAMETERS :
//        T : typename
//            Type of the std::array values in the input arrays. Can
//            be inferred from the call-site.
//
//        LEN : std::size_t
//            Length of the std::array objects to compare. Can be
//            inferred from the call-site.
//
// ARGUMENTS :
//        TGT : std::array<T, LEN>&
//            The target array to check
//
//        GOLD : std::array<T, LEN>&
//            The known-good (gold) array to check against
// 
// RETURNS :
//        int
//            0 if the arrays contain equal values at corresponding
//            locations. -(i + 1) if the values at corresponding
//            locations in two arrays are not equal, where i is the
//            index of the first non-equal values.
template <typename T, std::size_t LEN>
int check(std::array<T, LEN> &TGT, std::array<T, LEN> &GOLD){
	for(int i = 0; i < LEN; ++i){
		if(TGT[i] != GOLD[i]){
			return -(i + 1);
		}
	}
	return 0;
}
#endif // __UTILITY_HPP
