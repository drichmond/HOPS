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
#include <array>
#include <cmath>
#include <iostream>
#include <functional>
#include <type_traits>
#include "reduce.hpp"
#include "arrayops.hpp"
#include "utility.hpp"
#include "zip.hpp"
#include "functools.hpp"

#ifdef BIT_ACCURATE
#include "ap_int.h"
#endif

using namespace hops;
#define MULTCONST 35
#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

// -------------------- Min Test --------------------
int minleft_hop_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	auto lambdamin = [](int const& L, int const& R){
#pragma HLS INLINE
		return L < R ? L : R;
	};
	return lreduce(lambdamin, 1001, IN);
}

int minright_hop_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	auto lambdamin = [](int const& L, int const& R){
#pragma HLS INLINE
		return L < R ? L : R;
	};
	return rreduce(lambdamin, IN, 1001);
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
	std::cout << "Running minright_hop_synth test... ";
	output = minright_hop_synth(input);
	if(output != gold){
		std::cout << "Failed minright_hop_synth test! Output: "
			  << output << ", Gold: " << gold << std::endl;
		exit(-1);
	}
	std::cout << "Passed minright_hop_synth test!" << std::endl;
	
	std::cout << "Running minleft_hop_synth test... ";
	output = minleft_hop_synth(input);
	if(output != gold){
		std::cout << "Failed minleft_hop_synth test! Output: "
			  << output << ", Gold: " << gold << std::endl;
		exit(-1);
	}
	std::cout << "Passed minleft_hop_synth test!" << std::endl;	

	std::cout << "Min Tests Passed!" << std::endl;
	return 0;
}
// -------------------- End Min Test --------------------

// -------------------- Sum Test --------------------

float sumleft_hop_synth(std::array<float, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	#pragma HLS 
	return lreduce(std::plus<float>(), 0.0f, IN);
}

float sumright_hop_synth(std::array<float, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return rreduce(std::plus<float>(), IN, 0.0f);
}

float sum_test(){
	std::array<float, LIST_LENGTH> input;
	float output, gold = 0;

	fill_normal(input);

	for(int i = 0; i < LIST_LENGTH; ++i){
		gold += input[i];
	}

	std::cout << "Running sumright_hop_synth test... ";
	output = sumright_hop_synth(input);
	if(std::abs(output - gold) > .1){
		std::cout << "Failed sumright_hop_synth test! Output: "
			  << output << ", Gold: " << gold << std::endl;
		exit(-1);
	}
	std::cout << "Passed sumright_hop_synth test!" << std::endl;
	
	std::cout << "Running sumleft_hop_synth test... ";
	output = sumleft_hop_synth(input);
	if(std::abs(output - gold) > .1){
		std::cout << "Failed sumleft_hop_synth test! Output: "
			  << output << ", Gold: " << gold << std::endl;
		exit(-1);
	}
	std::cout << "Passed sumleft_hop_synth test!" << std::endl;	

	std::cout << "Sum Tests Passed!" << std::endl;
	return 0;
}
// -------------------- End Sum Test --------------------

// -------------------- Transform Test --------------------
struct Transform{
	template<class FN, typename TA, typename TI, std::size_t LEN>
	auto operator()(FN const& F, TA CUR, std::array<TI, LEN> const& INIT) const
		-> std::array<decltype(F(CUR)), LEN+1> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=INIT._M_instance
#pragma HLS INLINE
		return prepend(F(CUR), INIT);
	}
} transform;

auto trans_hop_synth(std::array<int, LIST_LENGTH> const& IN)
	-> std::array<int, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	std::array<int, 0> init;
	auto f = partial(transform, partial(std::multiplies<int>(), std::integral_constant<int, MULTCONST>{}));
	return rreduce(f, IN, init);
}

int trans_test(){
	std::array<int, LIST_LENGTH> input, output, gold;
	fill_normal(input);

	for(int i = 0; i < LIST_LENGTH; ++i){
		gold[i] = MULTCONST*input[i];
	}

	std::cout << "Running trans_hop_synth test... ";
	output = trans_hop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed trans_hop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed trans_hop_synth test!" << std::endl;
	
	std::cout << "Trans Tests Passed!" << std::endl;
	return 0;
}

// -------------------- End Transform Test --------------------

// -------------------- Reverse Test --------------------
std::array<float, LIST_LENGTH> reverse_hop_synth(std::array<float, LIST_LENGTH> const& in){
#pragma HLS ARRAY_PARTITION complete VARIABLE=in._M_instance
#pragma HLS INLINE
#pragma HLS PIPELINE
	std::array<float, 0> init;
	std::array<float, LIST_LENGTH> out = lreduce(flip(prepend), init, in);
#pragma HLS ARRAY_PARTITION complete VARIABLE=out._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=init._M_instance
	return out;
}

std::array<float, LIST_LENGTH> reverse_loop_synth(std::array<float, LIST_LENGTH> const& in){
#pragma HLS ARRAY_PARTITION complete VARIABLE=in._M_instance
#pragma HLS INLINE
#pragma HLS PIPELINE
	std::array<float, 0> init;
	std::array<float, LIST_LENGTH> out;
	for(std::size_t i = 0; i < LIST_LENGTH; ++i){
		out[i] = in[LIST_LENGTH-i-1];
	}
#pragma HLS ARRAY_PARTITION complete VARIABLE=init._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=out._M_instance
	return out;
}

int reverse_test(){
	std::array<float, LIST_LENGTH> input, output;
	fill_normal(input);
	std::cout << "Running reverse_hop_synth test... ";
	output = reverse_hop_synth(input);
	for(int i = 0; i < LIST_LENGTH; ++i){
		if(output[i] != input[LIST_LENGTH - i - 1]){
			std::cerr << "Failed reverse_hop_synth test!" << std::endl;
			exit(-1);

		}
	}
	std::cout << "Passed reverse_hop_synth test!" << std::endl;

	std::cout << "Running reverse_loop_synth test... ";
	output = reverse_loop_synth(input);
	for(int i = 0; i < LIST_LENGTH; ++i){
		if(output[i] != input[LIST_LENGTH - i - 1]){
			std::cerr << "Failed reverse_loop_synth test!" << std::endl;
			exit(-1);

		}
	}
	std::cout << "Passed reverse_loop_synth test!" << std::endl;

	std::cout << "Reverse Tests Passed!" << std::endl;
	return 0;
}
// -------------------- End Reverse Test --------------------

// -------------------- Interleave Test --------------------
template <typename T, std::size_t LEN>
std::array<T, 2*LEN> _interleave(std::array<T, LEN> L, std::array<T, LEN> R){
	std::array<T, 2*LEN> out;
#pragma HLS ARRAY_PARTITION complete VARIABLE=out._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
	for(int i = 0; i < LEN; ++i){
#pragma HLS UNROLL
		out[2*i    ] = L[i];
		out[2*i + 1] = R[i];
	}
	return out;
}

std::array<std::size_t, 2*LIST_LENGTH> interleave_hop_synth(std::array<std::size_t, LIST_LENGTH> L, std::array<std::size_t, LIST_LENGTH> R){
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
	std::array<std::size_t, 0> init; 
#pragma HLS ARRAY_PARTITION complete VARIABLE=init._M_instance
	return rreduce(concatenate, zipWith(construct, L, R), init);
}

std::array<std::size_t, 2*LIST_LENGTH> interleave_loop_synth(std::array<std::size_t, LIST_LENGTH> L, std::array<std::size_t, LIST_LENGTH> R){
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
	return _interleave(L, R);
}

int interleave_test(){
	std::array<std::size_t, LIST_LENGTH> linput, rinput;
	std::array<std::size_t, 2*LIST_LENGTH> output, gold;
	
	fill_normal(linput);
	fill_normal(rinput);
	for(std::size_t i = 0 ; i < LIST_LENGTH; ++i){
		gold[2*i    ] = linput[i];
		gold[2*i + 1] = rinput[i];
	}
	std::cout << "Running interleave_hop_synth test... ";
	output = interleave_hop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed truncate_hop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed interleave_hop_synth test!" << std::endl;

	std::cout << "Running interleave_loop_synth test... ";
	output = interleave_loop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed truncate_hop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed interleave_loop_synth test!" << std::endl;

	std::cout << "Interleave Tests Passed!" << std::endl;
	return 0;
}

// -------------------- End Interleave Test --------------------

int main(){
	int err;
	min_test();
	sum_test();
	trans_test();
	reverse_test();
	interleave_test();
	std::cout << "Tests passed!" << std::endl;
	return 0;	
}

