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
#include <cmath>
#include <array>
#include <iostream>
#include <type_traits>
#include "divconq.hpp"
#include "arrayops.hpp"
#include "utility.hpp"
#include "zip.hpp"
#include "reduce.hpp"

#ifdef BIT_ACCURATE
#include "ap_int.h"
#endif

using namespace hops;
#define MULTCONST 35
#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

// -------------------- Min --------------------
int min_hop_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
	auto lambdamin = [](std::array<int, 1> const& L,
			std::array<int, 1> const& R){
#pragma HLS INLINE
		std::array<int, 1> ret = {L[0] < R[0] ? L[0] : R[0]};
#pragma HLS ARRAY_PARTITION complete VARIABLE=ret._M_instance	
		return ret;
	};
	return divconq(lambdamin, IN)[0];
}

int min_test(){
	std::array<int, LIST_LENGTH> input;
	int output, gold;

	fill_normal(input);

	for(int i = 0; i < LIST_LENGTH; ++i){
		if(gold > input[i]){
			gold = input[i];
		}
	}

	std::cout << "Running min_hop_synth test... ";
	output = min_hop_synth(input);
	if(std::abs(output - gold) > .1){
		std::cout << "Failed min_hop_synth test! Output: "
			  << output << ", Gold: " << gold << std::endl;
		exit(-1);
	}
	std::cout << "Passed min_hop_synth test!" << std::endl;
	
	std::cout << "Min Tests Passed!" << std::endl;
	return 0;
}
// -------------------- End Min --------------------

// -------------------- Sum --------------------
struct Sum{
	std::array<float,1> operator()(std::array<float, 1> const& L,
				std::array<float, 1> const& R) const{
		return {L[0] + R[0]};
	}
} sum;

float sum_hop_synth(std::array<float, LIST_LENGTH> const& in){
#pragma HLS PIPELINE
	return divconq(sum, in)[0];
}

float sum_test(){
	std::array<float, LIST_LENGTH> input;
	float output, gold = 0;

	fill_normal(input);

	for(int i = 0; i < LIST_LENGTH; ++i){
		gold += input[i];
	}

	std::cout << "Running sum_hop_synth test... ";
	output = sum_hop_synth(input);
	if(output != gold){
		std::cout << "Failed sum_hop_synth test! Output: "
			  << output << ", Gold: " << gold << std::endl;
		exit(-1);
	}
	std::cout << "Passed sum_hop_synth test!" << std::endl;
	
	std::cout << "Sum Tests Passed!" << std::endl;
	return 0;
}
// -------------------- End Sum --------------------

// -------------------- Begin Bit-reverse --------------------

struct Interleave{
	template <typename T, std::size_t LEN>
	std::array<T, 2*LEN> operator()(std::array<T, LEN> const& L,
					std::array<T, LEN> const& R) const{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
		std::array<T, 0> init;
		return lreduce(concatenate, init, loop::zipWith(construct, L, R));
	}
} interleave;

std::array<std::size_t, LIST_LENGTH> idx_bitreverse_hop_synth(std::array<std::size_t, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
	std::array<std::size_t, 0> init;
	auto fn = compose(partial(partial(lreduce, concatenate), init),
			partial(loop::zipWith, construct)); // Implements *interleave* above
	return divconq(fn, IN);
}

template<std::size_t BITS>
std::size_t bitreverse(std::size_t i){
#pragma HLS INLINE
	int b = 0;
	static const std::size_t W = clog2(BITS);
	for(int j = 0; j < W; j++){
#pragma HLS_UNROLL
		b = b << 1;
		b |= ((i >> j) &1);
	}
	return b;
}


// Recursive, won't synthesize...
template<typename T>
constexpr std::size_t bitreverse(std::size_t BITS, T INPUT){
	static_assert(std::is_integral<T>::value, "Integral Type Required.");
	return ((INPUT & 1) << BITS) | (BITS ? value_bitreverse(BITS-1, INPUT>>1) : 0);
}

template<typename T, std::size_t LEN>
std::array<T, LEN> bitreverse(std::array<T, LEN> const& IN){
	std::array<T, LEN> OUT;
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=OUT._M_instance
#pragma HLS INLINE
	for(std::size_t t = 0; t < LIST_LENGTH; t ++){
#pragma HLS UNROLL
		T temp;
		unsigned int b = bitreverse<LEN>(t);
		OUT[t] = IN[b];
	}
	return OUT;
}

std::array<std::size_t, LIST_LENGTH> idx_bitreverse_loop_synth(std::array<std::size_t, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
	return bitreverse(IN);
}

int index_bitreverse_test(){
	std::array<std::size_t, LIST_LENGTH> input, output;
	std::array<std::size_t, LIST_LENGTH> gold;

	input = range<LIST_LENGTH>();
	gold  = bitreverse(input);

	std::cout << "Running idx_bitreverse_hop_synth test... ";
	output = idx_bitreverse_hop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed idx_bitreverse_hop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed idx_bitreverse_hop_synth test!" << std::endl;
	
	std::cout << "Running idx_bitreverse_loop_synth test... ";
	output = idx_bitreverse_loop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed idx_bitreverse_loop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed idx_bitreverse_loop_synth test!" << std::endl;

	std::cout << "Passed Index Bitreverse tests!" << std::endl;
	return 0;
}
// -------------------- End Bit-Reverse --------------------

int main(){
	int err;
	min_test();
	sum_test();
	index_bitreverse_test();
	std::cout << "Tests passed!" << std::endl;
	return 0;	
}

