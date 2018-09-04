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
#include "functools.hpp"
#include "utility.hpp"
#include "arrayops.hpp"
using namespace hops;
#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)
#define CONST_A 42
#define CONST_B 94

// -------------------- Partial Test --------------------
int partial_synth(int IN){
#pragma HLS PIPELINE
	return partial(std::multiplies<int>(),
		std::integral_constant<int, CONST_A>{})(IN);
}

int partial_test(){
	std::array<int, 1> temp;
	int input, output, gold;
	fill_normal(temp);
	input = temp[0];

	gold = CONST_A * input;
	std::cout << "Running partial_synth test... ";
	output = partial_synth(input);
	if(output != gold){
		std::cerr << "Failed partial_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed partial_synth test!" << std::endl;

	std::cout << "Passed Partial Tests!" << std::endl;
	return 0;
}
// -------------------- End Partial Test --------------------
// -------------------- Compose Test --------------------
int compose_synth(int IN){
#pragma HLS PIPELINE
	return compose(partial(std::multiplies<int>(), std::integral_constant<int, CONST_A>{}),
		partial(std::plus<int>(), std::integral_constant<int, CONST_B>{}))(IN);
}

int compose_test(){
	std::array<int, 1> temp;
	int input, output, gold;
	fill_normal(temp);
	input = temp[0];

	gold = CONST_A * (input + CONST_B);

	std::cout << "Running compose_synth test... ";
	output = compose_synth(input);
	if(output != gold){
		std::cerr << "Failed compose_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed compose_synth test!" << std::endl;


	std::cout << "Passed Compose Tests!" << std::endl;
	return 0;       

	std::cout << "Passed Compose Tests!" << std::endl;
	return 0;       
}
// -------------------- End Compose Test --------------------

// -------------------- Curry Test --------------------
std::array<int, LIST_LENGTH> curry_synth(std::array<int, LIST_LENGTH/2> const& L, std::array<int, LIST_LENGTH/2> const& R){
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	return curry(uncurry(concatenate))(L, R);
}

int curry_test(){
	std::array<int, LIST_LENGTH/2> linput, rinput;
	std::array<int, LIST_LENGTH> output, gold;
	fill_normal(linput);
	fill_normal(rinput);

	for(int i = 0; i < LIST_LENGTH/2; ++i){
		gold[i] = linput[i];
		gold[i + LIST_LENGTH/2] = rinput[i];
	}

	std::cout << "Running curry_synth test... ";
	output = curry_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed curry_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed curry_synth test!" << std::endl;

	std::cout << "Passed Curry Tests!" << std::endl;
	return 0;       
}
// -------------------- End Curry Test --------------------

// -------------------- Uncurry Test --------------------
int uncurry_synth(std::pair<int, int> const& IN){
#pragma HLS PIPELINE
	return uncurry(std::multiplies<int>{})(IN);
}

int uncurry_test(){
	std::array<int, 2> temp;
	std::pair<int, int> input;
	int output, gold;

	fill_normal(temp);
	input = {temp[0], temp[1]};

	gold = temp[0] * temp[1];

	std::cout << "Running uncurry_synth test... ";
	output = uncurry_synth(input);
	if(output != gold){
		std::cerr << "Failed uncurry_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed uncurry_synth test!" << std::endl;

	std::cout << "Passed Uncurry Tests!" << std::endl;
	return 0;       
}
// -------------------- End Uncurry Test --------------------

// -------------------- Flip Test --------------------
float flip_synth(float const& L, float const& R){
#pragma HLS PIPELINE
	return flip(std::divides<float>{})(L, R);
}

int flip_test(){
	std::array<int, 2> temp;
	float linput, rinput, output, gold;

	fill_normal(temp);
	linput = static_cast<float>(temp[0]);
	rinput = static_cast<float>(temp[1]);

	gold = rinput / linput;

	std::cout << "Running flip_synth test... ";
	output = flip_synth(linput, rinput);
	if(output != gold){
		std::cerr << "Failed flip_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed flip_synth test!" << std::endl;

	std::cout << "Passed Flip Tests!" << std::endl;
	return 0;       
}
// -------------------- End Flip Test --------------------

int main(){
	partial_test();
	compose_test();
	curry_test();
	uncurry_test();
	flip_test();
	std::cout << "Tests passed!" << std::endl;
	return 0;	
}

