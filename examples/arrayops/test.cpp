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
#include "arrayops.hpp"
#include "utility.hpp"
using namespace hops;
#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

// -------------------- Reverse Test --------------------
std::array<int, LIST_LENGTH> reverse_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return reverse(IN);
}

int reverse_test(){
	std::array<int, LIST_LENGTH> input, output, gold;
	fill_normal(input);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		gold[i] = input[LIST_LENGTH-i-1];
	}

	std::cout << "Running reverse_synth test... ";
	output = reverse_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed reverse_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed reverse_synth test!" << std::endl;

	std::cout << "Passed Reverse Tests!" << std::endl;
	return 0;       
}
// -------------------- End Reverse Test --------------------

// -------------------- Range Test --------------------
std::array<std::size_t, LIST_LENGTH> range_synth(){
#pragma HLS PIPELINE
	return range<LIST_LENGTH>();
}

int range_test(){
	std::array<std::size_t, LIST_LENGTH> output, gold;
	
	for(std::size_t i = 0; i < LIST_LENGTH; ++i){
		gold[i] = i;
	}

	std::cout << "Running range_synth test... ";
	output = range_synth();
	if(check(output, gold)){
		std::cerr << "Failed range_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed range_synth test!" << std::endl;

	std::cout << "Passed Range Tests!" << std::endl;
	return 0;

}
// -------------------- End Range Test --------------------

// -------------------- Prepend Test --------------------
std::array<int, LIST_LENGTH> prepend_synth(int H, std::array<int, LIST_LENGTH-1> T){
#pragma HLS ARRAY_PARTITION complete VARIABLE=T._M_instance
#pragma HLS PIPELINE
	return prepend(H, T);
}

int prepend_test(){
	std::array<int, LIST_LENGTH - 1> input;
	std::array<int, LIST_LENGTH> output, gold;
	int val = 42;

	fill_normal(input);
	gold[0] = val;
	for(std::size_t i = 0; i < LIST_LENGTH; ++i){
		gold[i+1] = input[i];
	}

	std::cout << "Running prepend_synth test... ";
	output = prepend_synth(val, input);
	if(check(output, gold)){
		std::cerr << "Failed prepend_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed prepend_synth test!" << std::endl;

	std::cout << "Passed Prepend Tests!" << std::endl;
	return 0;
}
// -------------------- End Prepend Test --------------------

// -------------------- Append Test --------------------
std::array<int, LIST_LENGTH> append_synth(std::array<int, LIST_LENGTH-1> const& H, int T){
#pragma HLS ARRAY_PARTITION complete VARIABLE=H._M_instance
#pragma HLS PIPELINE
	return append(H, T);
}

int append_test(){
	std::array<int, LIST_LENGTH - 1> input;
	std::array<int, LIST_LENGTH> output, gold;
	int val = 42;

	fill_normal(input);
	for(std::size_t i = 0; i < LIST_LENGTH; ++i){
		gold[i] = input[i];
	}
	gold[LIST_LENGTH-1] = val;

	std::cout << "Running append_synth test... ";
	output = append_synth(input, val);
	if(check(output, gold)){
		std::cerr << "Failed append_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed append_synth test!" << std::endl;

	std::cout << "Passed Append Tests!" << std::endl;
	return 0;	
}
// -------------------- End Append Test --------------------

// -------------------- Concatenate Test --------------------
std::array<int, LIST_LENGTH> concatenate_synth(std::array<int, (LIST_LENGTH+1)/2> const& L, std::array<int, LIST_LENGTH/2> const& R){
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS PIPELINE
	return concatenate(L, R);
}

int concatenate_test(){
	std::array<int, LIST_LENGTH/2> linput, rinput;
	std::array<int, LIST_LENGTH> output, gold;

	fill_normal(linput);
	fill_normal(rinput);

	for(std::size_t i = 0; i < LIST_LENGTH/2; ++i){
		gold[i] = linput[i];
		gold[i + LIST_LENGTH / 2] = rinput[i];
	}

	std::cout << "Running concatenate_synth test... ";
	output = concatenate_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed concatenate_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed concatenate_synth test!" << std::endl;

	std::cout << "Passed Concatenate Tests!" << std::endl;
	return 0;	
}
// -------------------- End Concatenate Test --------------------

// -------------------- Split Test --------------------
std::pair<std::array<int, (LIST_LENGTH+1)/2>, std::array<int, LIST_LENGTH/2> > split_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	auto temp = split<(LIST_LENGTH+1)/2>(IN);
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.first._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.second._M_instance
	return temp;
}

int split_test(){
	std::array<int, LIST_LENGTH> input;
	std::array<int, LIST_LENGTH/2> lgold, rgold;
	std::pair<std::array<int, LIST_LENGTH/2>, std::array<int, LIST_LENGTH/2> > output;

	fill_normal(input);

	for(std::size_t i = 0; i < LIST_LENGTH/2; ++i){
		lgold[i] = input[i];
		rgold[i] = input[i + LIST_LENGTH/2];
	}

	std::cout << "Running split_synth test... ";

	output = split_synth(input);
	if(check(output.first, lgold) || check(output.second, rgold)){
		std::cerr << "Failed split_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed split_synth test!" << std::endl;

	std::cout << "Passed Split Tests!" << std::endl;
	return 0;	
}
// -------------------- End Split Test --------------------

// -------------------- Replicate Test --------------------
std::array<int, LIST_LENGTH> replicate_synth(int val){
#pragma HLS PIPELINE
	return replicate<LIST_LENGTH>(val);
}

int replicate_test(){
	std::array<int, LIST_LENGTH> output, gold;
	int val = 42;
	for(std::size_t i = 0; i < LIST_LENGTH; ++i){
		gold[i] = val;
	}

	std::cout << "Running replicate_synth test... ";

	output = replicate_synth(val);
	if(check(output, gold)){
		std::cerr << "Failed replicate_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed replicate_synth test!" << std::endl;

	std::cout << "Passed Replicate Tests!" << std::endl;
	return 0;	

}
// -------------------- End Replicate Test --------------------

// -------------------- Head Test --------------------
int head_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return head(IN);
}

int head_test(){
	std::array<int, LIST_LENGTH> input;
	int output, gold;
	fill_normal(input);
	gold  = input[0];
	
	std::cout << "Running head_synth test... ";
	output = head_synth(input);
	if(output != gold){
		std::cerr << "Failed head_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed head_synth test!" << std::endl;

	std::cout << "Passed Head Tests!" << std::endl;
	return 0;
}
// -------------------- End Head Test --------------------

// -------------------- At Test --------------------
int at_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return At<LOG_LIST_LENGTH>()(IN);
}

int at_test(){
	std::array<int, LIST_LENGTH> input;
	int output, gold;
	fill_normal(input);
	gold  = input[LOG_LIST_LENGTH];
	
	std::cout << "Running at_synth test... ";
	output = at_synth(input);
	if(output != gold){
		std::cerr << "Failed at_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed at_synth test!" << std::endl;

	std::cout << "Passed At Tests!" << std::endl;
	return 0;
}
// -------------------- End At Test --------------------

// -------------------- Last Test --------------------
int last_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return last(IN);
}

int last_test(){
	std::array<int, LIST_LENGTH> input;
	int output, gold;
	fill_normal(input);
	gold  = input[LIST_LENGTH-1];
	
	std::cout << "Running last_synth test... ";
	output = last_synth(input);
	if(output != gold){
		std::cerr << "Failed last_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed last_synth test!" << std::endl;

	std::cout << "Passed Last Tests!" << std::endl;
	return 0;
}
// -------------------- End Last Test --------------------

// -------------------- Tail Test --------------------
std::array<int, LIST_LENGTH - 1> tail_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return tail(IN);
}

int tail_test(){
	std::array<int, LIST_LENGTH> input;
	std::array<int, LIST_LENGTH-1> output, gold;

	fill_normal(input);
	
	for(int i = 0; i < LIST_LENGTH-1; ++i){
		gold[i] = input[i+1];
	}

	std::cout << "Running tail_synth test... ";
	output = tail_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed tail_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed tail_synth test!" << std::endl;

	std::cout << "Passed Tail Tests!" << std::endl;
	return 0;       
}
// -------------------- End Tail Test --------------------

// -------------------- Rshift Test --------------------
std::array<int, LIST_LENGTH> rshift_synth(int const& VAL, std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return rshift(VAL, IN);
}

int rshift_test(){
	std::array<int, LIST_LENGTH> input, output, gold;
	int val = 42;
	fill_normal(input);

	gold[0] = val;
	for(int i = 0; i < LIST_LENGTH-1; ++i){
		gold[i+1] = input[i];
	}

	std::cout << "Running rshift_synth test... ";
	output = rshift_synth(val, input);
	if(check(output, gold)){
		std::cerr << "Failed rshift_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed rshift_synth test!" << std::endl;

	std::cout << "Passed Rshift Tests!" << std::endl;
	return 0;
}
// -------------------- End Rshift Test --------------------

// -------------------- Rrotate Test --------------------
std::array<int, LIST_LENGTH> rrotate_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return rrotate(IN);
}

int rrotate_test(){
	std::array<int, LIST_LENGTH> input, output, gold;

	fill_normal(input);

	gold[0] = input[LIST_LENGTH - 1];
	for(int i = 0; i < LIST_LENGTH-1; ++i){
		gold[i+1] = input[i];
	}

	std::cout << "Running rrotate_synth test... ";
	output = rrotate_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed rrotate_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed rrotate_synth test!" << std::endl;

	std::cout << "Passed Rrotate Tests!" << std::endl;
	return 0;
}
// -------------------- End Rrotate Test --------------------

// -------------------- Lshift Test --------------------
std::array<int, LIST_LENGTH> lshift_synth(std::array<int, LIST_LENGTH> const& IN, int const& VAL){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return lshift(IN, VAL);
}

int lshift_test(){
	std::array<int, LIST_LENGTH> input, output, gold;
	int val = 42;
	fill_normal(input);

	gold[LIST_LENGTH - 1] = val;
	for(int i = 0; i < LIST_LENGTH-1; ++i){
		gold[i] = input[i+1];
	}

	std::cout << "Running lshift_synth test... ";
	output = lshift_synth(input, val);
	if(check(output, gold)){
		std::cerr << "Failed lshift_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed lshift_synth test!" << std::endl;

	std::cout << "Passed Lshift Tests!" << std::endl;
	return 0;
}
// -------------------- End Lshift Test --------------------

// -------------------- Lrotate Test --------------------
std::array<int, LIST_LENGTH> lrotate_synth(std::array<int, LIST_LENGTH> const& IN){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS PIPELINE
	return lrotate(IN);
}

int lrotate_test(){
	std::array<int, LIST_LENGTH> input, output, gold;

	fill_normal(input);

	gold[LIST_LENGTH - 1] = input[0];
	for(int i = 0; i < LIST_LENGTH-1; ++i){
		gold[i] = input[i+1];
	}

	std::cout << "Running lrotate_synth test... ";
	output = lrotate_synth(input);
	if(check(output, gold)){
		std::cerr << "Failed lrotate_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed lrotate_synth test!" << std::endl;

	std::cout << "Passed Lrotate Tests!" << std::endl;
	return 0;
}
// -------------------- End Lrotate Test --------------------

// -------------------- Construct Test --------------------
std::array<int, 2> construct_synth(int L, int R){
#pragma HLS PIPELINE
	return construct(L, R);
}

int construct_test(){
	int linput = 42, rinput = 21;
	std::array<int, 2> output, gold = {linput, rinput};

	std::cout << "Running construct_synth test... ";
	output = construct_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed construct_synth test!" << std::endl;
		exit(-1);
	}
	std::cout << "Passed construct_synth test!" << std::endl;

	std::cout << "Passed Construct Tests!" << std::endl;
	return 0;
}
// -------------------- End Construct Test --------------------

int main(){
	reverse_test();
	range_test();
	prepend_test();
	append_test();
	concatenate_test();
	split_test();
	replicate_test();
	head_test();
	at_test();
	last_test();
	tail_test();
	rrotate_test();
	rshift_test();
	lrotate_test();
	lshift_test();
	construct_test();
	std::cout << "Tests passed!" << std::endl;
	return 0;	
}

