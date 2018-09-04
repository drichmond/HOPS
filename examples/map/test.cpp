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
#include <iostream>
#include <functional>
#include "map.hpp"
#include "arrayops.hpp"
#include "utility.hpp"
using namespace hops;
#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

struct Truncate{
	char operator()(int const& i) const{
#pragma HLS INLINE
		return (char)i;
	}
} trun;

std::array<char, LIST_LENGTH> truncate_hop_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return map(trun, IN);
}

std::array<char, LIST_LENGTH> truncate_loop_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return loop::map(trun, IN);
}

int truncate_test(){
	std::array<int, LIST_LENGTH> input;
	std::array<char, LIST_LENGTH> output, gold;
	fill_normal(input);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		gold[i] = (char)(input[i] & 0xff);
	}

	std::cout << "Running truncate_hop_synth test... ";
	output = truncate_hop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed truncate_hop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed truncate_hop_synth test!" << std::endl;

	std::cout << "Running truncate_loop_synth test... ";
	output = truncate_loop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed truncate_loop_synth test" << std::endl;
		exit(-1);
	}
	std::cout << "Passed truncate_loop_synth test!" << std::endl;

	std::cout << "Passed Truncate Tests!" << std::endl;
	return 0;
}

unsigned int bitreverse(unsigned int const& i) { 
#pragma HLS INLINE
	unsigned int o = 0;
	for(int j = 0; j < 32; ++j){
#pragma HLS UNROLL
		o = (o << 1) | ((i >> j)&1);
	}
	return o;
}

std::array<unsigned int, LIST_LENGTH> bitreverse_hop_synth(std::array<unsigned int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	auto lambda = [](unsigned int const& i) {
#pragma HLS INLINE
		return bitreverse(i);
	};
	return map(lambda, IN);
}

std::array<unsigned int, LIST_LENGTH> bitreverse_loop_synth(std::array<unsigned int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	auto lambda = [](unsigned int const& i) {
#pragma HLS INLINE
		return bitreverse(i);
	};
	return loop::map(lambda, IN);
}

int bitreverse_test(){
	std::array<unsigned int, LIST_LENGTH> input, output, gold;
	
	for(unsigned int i= 0; i <= LIST_LENGTH; ++i){
		input[i] = i;
		gold[i] = bitreverse(i);
	}
	
	std::cout << "Running bitreverse_hop_synth test... ";
	output = bitreverse_hop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed bitreverse_hop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed bitreverse_hop_synth test!" << std::endl;

	std::cout << "Running bitreverse_loop_synth test... ";
	output = bitreverse_loop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed bitreverse_loop_synth test" << std::endl;
		exit(-1);
	}
	std::cout << "Passed bitreverse_loop_synth test!" << std::endl;

	std::cout << "Passed BitReverse Tests!" << std::endl;
	return 0;
}

std::array<int, LIST_LENGTH> negate_hop_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return map(std::negate<int>(), IN);
}

std::array<int, LIST_LENGTH> negate_loop_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return loop::map(std::negate<int>(), IN);
}

int negate_test(){
	std::array<int, LIST_LENGTH> input, output, gold;
	fill_normal(input);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		gold[i] = -input[i];
	}
	
	std::cout << "Running negate_hop_synth test... ";
	output = negate_hop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed negate_hop_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed negate_hop_synth test!" << std::endl;

	std::cout << "Running negate_loop_synth test... ";
	output = negate_loop_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed negate_loop_synth test" << std::endl;
		exit(-1);
	}
	std::cout << "Passed negate_loop_synth test!" << std::endl;

	std::cout << "Passed Negate Tests!" << std::endl;
	return 0;
}

int main(){
	truncate_test();
	bitreverse_test();
	negate_test();
	std::cout << "Tests passed!" << std::endl;
	return 0;	
}

