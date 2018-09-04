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
#include <iostream>
#include <limits>
#include "utility.hpp"
#include "map.hpp"
#include "arrayops.hpp"
#include "divconq.hpp"
#ifdef BIT_ACCURATE
#include "ap_int.h"
typedef ap_int<1> last_t;
#else
typedef	bool last_t;
#endif

using namespace hops;

#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

// I'd actually prefer a tuple - but that's not synthesizable...
template <typename T>
struct argmin_t{
	T data;
	std::size_t idx, lev;
	argmin_t<T> operator()(T const& v) const {
#pragma HLS INLINE
		return {v, 0, 0};
	}
};

struct Argminop{
	template <typename T>
	argmin_t<T> operator()(argmin_t<T> const& L, argmin_t<T> const& R) const{
#pragma HLS INLINE
		bool b = R.data < L.data;
		std::size_t lev = L.lev;
		T res = b ? R.data : L.data;
		std::size_t idx = ((b & 1) << lev) | (b ? R.idx : L.idx);
		argmin_t<T> result = {res, idx, lev + 1};
		return result;
	}

	template <typename T>
 	argmin_t<T> operator()(std::array<argmin_t<T>, 1> const& L,
			std::array<argmin_t<T>, 1> const& R) const{
#pragma HLS INLINE
		return this->operator()(L[0], R[0]);
	}
};

Argminop argminop;

std::pair<int, std::size_t> argmin_loop_synth(std::array<int, LIST_LENGTH> IN){
#pragma HLS ARRAY_PARTITION variable=IN._M_instance
#pragma HLS INLINE
#pragma HLS PIPELINE
	static const std::size_t LEV = clog2(LIST_LENGTH);
	std::array<std::array<argmin_t<int>, (1<<LEV)>, LEV+1> temp;
#pragma HLS ARRAY_PARTITION variable=temp._M_instance	
#pragma HLS ARRAY_PARTITION variable=temp._M_instance[0]._M_instance
	for(std::size_t idx = 0; idx < (1<<LEV); ++idx){
#pragma HLS UNROLL
		temp[0][idx] = {IN[idx], idx, 0};
	}
	for(std::size_t lev = 1; lev <= LEV; ++lev){
		for(std::size_t idx = 0; idx < 1<<(LEV-lev); ++idx){
#pragma HLS UNROLL
			temp[lev][idx] = argminop(temp[lev-1][2*idx],
						temp[lev-1][2*idx+1]);
		}
	}
	return {temp[LEV][0].data, temp[LEV][0].idx};
}

std::pair<int, std::size_t> argmin_hop_synth(std::array<int, LIST_LENGTH> IN){
#pragma HLS ARRAY_PARTITION variable=IN._M_instance
#pragma HLS INLINE
#pragma HLS PIPELINE
	argmin_t<int> out = divconq(argminop, map(argmin_t<int>(), IN));
	return std::make_pair(out.data, out.idx);
}

int argmin_test(){
	std::array<int, LIST_LENGTH> input;
	std::pair<int, std::size_t> gold, output;
	output = {std::numeric_limits<int>::min(), 0};

	fill_normal(input);
	for(int i = 0; i < LIST_LENGTH; ++i){
		if(input[i] < gold.first){
			gold.first = input[i];
			gold.second = i;
		}
	}

	output = argmin_loop_synth(input);
	if(output.first != gold.first){
		std::cout << "Failed argmin_loop_synth (value) test! Output: "
			  << output.first << ", Gold: " << gold.first
			  << std::endl;
		exit(-1);
	}
	if(output.second != gold.second){
		std::cout << "Failed argmin_loop_synth (index) test! Output: "
			  << output.second << ", Gold: " << gold.second
			  << std::endl;
		exit(-1);
	}
	std::cout << "argmin_loop_synth test passed" << std::endl;

	output = argmin_hop_synth(input);
	if(output.first != gold.first){
		std::cout << "Failed argmin_hop_synth (value) test! Output: "
			  << output.first << ", Gold: " << gold.first
			  << std::endl;
		exit(-1);
	}
	if(output.second != gold.second){
		std::cout << "Failed argmin_hop_synth (index) test! Output: "
			  << output.second << ", Gold: " << gold.second
			  << std::endl;
		exit(-1);
	}
	std::cout << "argmin_hop_synth test passed" << std::endl;

	printf("ArgminTests Passed!\n");
	return 0;
}



int main(){
	int err = 0;
	if((err=argmin_test())){
		return err;
	}
	printf("Argminmax test passed!\n");
}
