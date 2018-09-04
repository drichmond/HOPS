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
#include <array>

#include "reduce.hpp"
#include "arrayops.hpp"
#include "utility.hpp"
#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

#ifdef BIT_ACCURATE
#include "ap_int.h"
#endif

#ifdef BIT_ACCURATE
typedef ap_int<1> last_t;
#else
typedef bool last_t;
#endif

template <typename T>
using isort_t = std::pair<T, last_t>;

struct MinOp{
	template <typename T, size_t LEN>
	auto operator()(isort_t<T> const& me,
			std::array<isort_t<T>, LEN> const& past) const
		-> std::array<isort_t<T>, LEN+1> {

		isort_t<T> in, bump, newme;
		bool comparison;

		in = hops::head(past);

		comparison = (me.first < in.first) & !me.second;

		bump.first = comparison? in.first : me.first;
		bump.second = me.second;
		newme.first = comparison? me.first : in.first;
		newme.second = in.second;

		return hops::prepend(bump, hops::prepend(newme, hops::tail(past)));;
	}
} minOp;

isort_t<int> isort_hop_synth(isort_t<int> const& IN){
#pragma HLS PIPELINE
	static std::array<isort_t<int>, LIST_LENGTH> arr;
#pragma HLS ARRAY_PARTITION complete VARIABLE=arr._M_instance
	std::array<isort_t<int>, 1 > in = {IN};
#pragma HLS ARRAY_PARTITION complete VARIABLE=in._M_instance
	auto out = hops::rreduce(minOp, arr, in);
	arr = hops::tail(out);
	return hops::head(out);
}


isort_t<int> isort_loop_synth(isort_t<int> &IN){
#pragma HLS PIPELINE
	static std::array<isort_t<int>, LIST_LENGTH> arr;
#pragma HLS ARRAY_PARTITION complete VARIABLE=arr._M_instance

	isort_t<int> me, newme, bump = IN;
	bool comparison;
	for(int i = LIST_LENGTH-1 ; i >= 0; --i){
#pragma HLS UNROLL
		me = arr[i];
		comparison = (me.first < bump.first) & !me.second;

		newme.first = comparison? me.first : bump.first;
		newme.second = bump.second;

		bump.first = comparison? bump.first : me.first;
		bump.second = me.second;

		arr[i] = newme;
	}
	return bump;
}


int main(){
	std::array<int, LIST_LENGTH> input, output, gold;
	int last = 0;
	int boot;
	isort_t<int> in, out;

	fill_normal(input);
	gold = input;

	std::sort(gold.begin(), gold.end(), std::greater<int>());
	for (auto e : input) {
		std::cout << e << " ";
	}
	std::cout << std::endl;
	for (auto e : gold) {
		std::cout << e << " ";
	}
	std::cout << std::endl;

	for(int i = -1 ; i < LIST_LENGTH*2 + 1; ++i){
		if(i == -1){
			in.first = 0;
		} else {
			in.first = input[i];
		}
		if(i == LIST_LENGTH || i == -1){
			in.second = 1;
		} else {
			in.second = 0;
		}
		out = isort_hop_synth(in);
		if(LIST_LENGTH+1 <= i && i <= LIST_LENGTH*2){
			output[i-(LIST_LENGTH+1)] = out.first;
		}
	}

	if(check(output, gold)){
		std::cerr << "Failed isort_hop_synth test!" << std::endl;
		return -1;
	}
	
	for(int i = -1 ; i < LIST_LENGTH*2 + 1; ++i){
		if(i == -1){
			in.first = 0;
		} else {
			in.first = input[i];
		}
		if(i == LIST_LENGTH || i == -1){
			in.second = 1;
		} else {
			in.second = 0;
		}
		out = isort_loop_synth(in);
		if(LIST_LENGTH+1 <= i && i <= LIST_LENGTH*2){
			output[i-(LIST_LENGTH+1)] = out.first;
		}
	}
	if(check(output, gold)){
		std::cerr << "Failed isort_loop_synth test!" << std::endl;
		return -1;
	}
	std::cout << "Insertion Sort Tests passed!" << std::endl;
}
