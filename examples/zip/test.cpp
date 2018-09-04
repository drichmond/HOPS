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
#include "zip.hpp"
#include "arrayops.hpp"
#include "utility.hpp"
using namespace hops;
#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

// -------------------- Zip Test --------------------
auto zip_hop_synth(std::array<int, LIST_LENGTH> const& L,
		std::array<int, LIST_LENGTH> const& R)
	-> std::array<std::pair<int, int>, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	return zip(L, R);
}

auto zip_loop_synth(std::array<int, LIST_LENGTH> const& L,
		std::array<int, LIST_LENGTH> const& R)
	-> std::array<std::pair<int, int>, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	return loop::zip(L, R);
}

int zip_test(){
	std::array<int, LIST_LENGTH> linput, rinput;
	std::array<std::pair<int, int>, LIST_LENGTH> output, gold;
	fill_normal(linput);
	fill_normal(rinput);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		gold[i] = {linput[i], rinput[i]};
	}

	std::cout << "Running zip_hop_synth test... ";
	output = zip_hop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed zip_hop_synth test!" << std::endl;
		return -1;
	}
	std::cout << "Passed zip_hop_synth test!" << std::endl;

	std::cout << "Running zip_loop_synth test... ";
	output = zip_loop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed zip_loop_synth test" << std::endl;
		return -1;
	}
	std::cout << "Passed zip_loop_synth test!" << std::endl;

	std::cout << "Passed Zip Tests!" << std::endl;
	return 0;
}
// -------------------- End Zip Test --------------------

// -------------------- Unzip Test --------------------
auto  unzip_hop_synth(std::array<std::pair<int, int>, LIST_LENGTH> const& IN)
	->  std::pair<std::array<int, LIST_LENGTH>, std::array<int, LIST_LENGTH> >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
	auto temp = unzip(IN);
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.first._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.second._M_instance
	return temp;
}

auto unzip_loop_synth(std::array<std::pair<int, int>, LIST_LENGTH> const& IN)
	-> std::pair<std::array<int, LIST_LENGTH>, std::array<int, LIST_LENGTH> >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
	auto temp = loop::unzip(IN);
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.first._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.second._M_instance
	return temp;
}

int unzip_test(){
	std::array<int, LIST_LENGTH> linput, rinput;
	std::array<std::pair<int, int>, LIST_LENGTH> input;
	std::pair<std::array<int, LIST_LENGTH>, std::array<int, LIST_LENGTH> > output, gold;
	fill_normal(linput);
	fill_normal(rinput);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		input[i] = {linput[i], rinput[i]};
		gold.first[i] = linput[i];
		gold.second[i] = rinput[i];
	}

	std::cout << "Running unzip_hop_synth test... ";
	output = unzip_hop_synth(input);
	if(check(output.first, gold.first) ||
		check(output.second, gold.second)){
		std::cerr << "Failed unzip_hop_synth test!" << std::endl;
		return -1;
	}
	std::cout << "Passed unzip_hop_synth test!" << std::endl;

	std::cout << "Running unzip_loop_synth test... ";
	output = unzip_loop_synth(input);
	if(check(output.first, gold.first) ||
		check(output.second, gold.second)){
		std::cerr << "Failed unzip_loop_synth test" << std::endl;
		return -1;
	}
	std::cout << "Passed unzip_loop_synth test!" << std::endl;

	std::cout << "Passed Unzip Tests!" << std::endl;
	return 0;
}
// -------------------- End Unzip Test --------------------

// -------------------- ZipWith (Add) Test --------------------
auto zipadd_hop_synth(std::array<int, LIST_LENGTH> const& L,
		std::array<int, LIST_LENGTH> const& R)
	-> std::array<int, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	return zipWith(std::plus<int>(), L, R);
}

auto zipadd_loop_synth(std::array<int, LIST_LENGTH> const& L,
			std::array<int, LIST_LENGTH> const& R)
	-> std::array<int, LIST_LENGTH>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	return loop::zipWith(std::plus<int>(), L, R);
}

int zipadd_test(){
	std::array<int, LIST_LENGTH> linput, rinput, output, gold;
	fill_normal(linput);
	fill_normal(rinput);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		gold[i] = linput[i] + rinput[i];
	}

	std::cout << "Running zipadd_hop_synth test... ";
	output = zipadd_hop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed zipadd_hop_synth test!" << std::endl;
		return -1;
	}
	std::cout << "Passed zipadd_hop_synth test!" << std::endl;

	std::cout << "Running zipadd_loop_synth test... ";
	output = zipadd_loop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed zipadd_loop_synth test" << std::endl;
		return -1;
	}
	std::cout << "Passed zipadd_loop_synth test!" << std::endl;

	std::cout << "Passed zipWith (Add) Tests!" << std::endl;
	return 0;
}
// -------------------- End ZipWith (Add) Test --------------------

// ------------------- ZipWith (Div) Test --------------------
struct Div{
	template <typename T>
	auto operator()(T const& L, T const& R) const 
		-> decltype(L/R) {
#pragma HLS INLINE
		return L / R;
	}
} divide;

auto zipdiv_hop_synth(std::array<int, LIST_LENGTH> const& L,
		std::array<int, LIST_LENGTH> const& R)
	-> std::array<int, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	return zipWith(divide, L, R);
}

auto zipdiv_loop_synth(std::array<int, LIST_LENGTH> const& L,
			std::array<int, LIST_LENGTH> const& R)
	-> std::array<int, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	return loop::zipWith(divide, L, R);
}

int zipdiv_test(){
	std::array<int, LIST_LENGTH> linput, rinput, output, gold;
	fill_normal(linput);
	fill_normal(rinput);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		linput[i] += (linput[i]==0);
		rinput[i] += (rinput[i]==0);
		gold[i] = linput[i] / rinput[i];
	}

	std::cout << "Running zipdiv_hop_synth test... ";
	output = zipdiv_hop_synth(linput, rinput);
	if(check(output, gold)){
		std::cout << "Failed zipdiv_hop_synth test!" << std::endl;
		return -1;
	}
	std::cout << "Passed zipdiv_hop_synth test!" << std::endl;

	std::cout << "Running zipdiv_loop_synth test... ";
	output = zipdiv_loop_synth(linput, rinput);
	if(check(output, gold)){
		std::cout << "Failed zipdiv_loop_synth test" << std::endl;
		return -1;
	}
	std::cout << "Passed zipdiv_loop_synth test!" << std::endl;

	std::cout << "Passed zipWith (Div) Tests!" << std::endl;
	return 0;
}
// -------------------- End ZipWith (Div) Test --------------------

// ------------------- ZipWith (Mul) Test --------------------

auto zipmul_hop_synth(std::array<int, LIST_LENGTH> const& L,
		std::array<int, LIST_LENGTH> const& R)
	-> std::array<int, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	auto lambda = [](int const& L, int const& R){
#pragma HLS INLINE
		return L * R;
	};
	return zipWith(lambda, L, R);
}

auto zipmul_loop_synth(std::array<int, LIST_LENGTH> const& L,
			std::array<int, LIST_LENGTH> const& R)
	-> std::array<int, LIST_LENGTH> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
	auto lambda = [](int const& L, int const& R){
#pragma HLS INLINE
		return L * R;
	};
	return loop::zipWith(lambda, L, R);
}

int zipmul_test(){
	std::array<int, LIST_LENGTH> linput, rinput, output, gold;
	fill_normal(linput);
	fill_normal(rinput);
	
	for(int i = 0; i < LIST_LENGTH; ++i){
		gold[i] = linput[i] * rinput[i];
	}

	std::cout << "Running zipmul_hop_synth test... ";
	output = zipmul_hop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed zipmul_hop_synth test!" << std::endl;
		return -1;
	}
	std::cout << "Passed zipmul_hop_synth test!" << std::endl;

	std::cout << "Running zipmul_loop_synth test... ";
	output = zipmul_loop_synth(linput, rinput);
	if(check(output, gold)){
		std::cerr << "Failed zipmul_loop_synth test" << std::endl;
		return -1;
	}
	std::cout << "Passed zipmul_loop_synth test!" << std::endl;

	std::cout << "Passed zipWith (Mul) Tests!" << std::endl;
	return 0;
}
// -------------------- End ZipWith (Mul) Test --------------------

int main(){
	zip_test();
	unzip_test();
	zipadd_test();
	zipmul_test();
	zipdiv_test();
	std::cout << "Tests passed!" << std::endl;
	return 0;	
}

