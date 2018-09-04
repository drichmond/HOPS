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

#include "systolic.hpp"
#include "smith_waterman.hpp"
#include "arrayops.hpp"
#include "utility.hpp"
#include "map.hpp"

struct SmithWatermanUnit{
	// Base, recursive operator
	score operator()(hw_base const& read,
			sw_ref const& ref,
			std::array<std::array<score, SW_HIST>, 1> const& smatrix) const{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
		score ret = {0,0,0};
		return ret;
	}

	// Recursive operator
	template <std::size_t IDX>
	score operator()(hw_base const& read,
			sw_ref const& ref,
			std::array<std::array<score, SW_HIST>, IDX> const& smatrix) const{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
		score ret;
		char sigma;
		if(ref.last){
#ifdef BIT_ACCURATE
			ret = {(ap_int<hops::clog2(IDX)+1>)-(IDX),
			       (ap_int<hops::clog2(IDX)+1>)-(IDX),
			       (ap_int<hops::clog2(IDX)+1>)-(IDX)};
#else
			ret = {(char)-(IDX), (char)-(IDX), (char)-(IDX)};
#endif
		}else{
			sigma = (read == ref.b)? MATCH: -MATCH;
			ret.e = std::max(smatrix[0][0].v - ALPHA, 
					smatrix[0][0].e - BETA);
			ret.f = std::max(smatrix[1][0].v - ALPHA,
					smatrix[1][0].f - BETA);
			ret.v = std::max((char)(smatrix[1][1].v + sigma),
					std::max(ret.e, ret.f));
		}
		return ret;
	}

	score operator()(std::size_t const& IDX,
			hw_base const& read,
			sw_ref const& ref,
			std::array<std::array<score, SW_HIST>, READ_LENGTH> const& smatrix) const{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
		score ret;
		char sigma;
		if(IDX == 0){
			ret = {0,0,0};
		} else {
			if(ref.last){
				ret = {(char)-(IDX+1), (char)-(IDX+1), (char)-(IDX+1)};
			}else{
				sigma = (read == ref.b)? MATCH: -MATCH;
				ret.e = std::max(smatrix[IDX + 0][0].v - ALPHA,
						smatrix[IDX + 0][0].e - BETA);
				ret.f = std::max(smatrix[IDX - 1][0].v - ALPHA,
						smatrix[IDX - 1][0].f - BETA);
				ret.v = std::max((char)(smatrix[IDX - 1][1].v + sigma),
						std::max(ret.e, ret.f));
			}
		}
		return ret;
	}

} sw_op;

auto systolic_hop_synth(std::array<hw_base, READ_LENGTH> const& left,
			std::array<sw_ref, READ_LENGTH> const& top,
			matrix<score, SW_HIST, READ_LENGTH> const& smatrix)
	-> matrix<score, SW_HIST, READ_LENGTH> {
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=top._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=left._M_instance
	auto result = systolic(sw_op, left, top, smatrix);
#pragma HLS ARRAY_PARTITION complete VARIABLE=result._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=result[0]._M_instance
	return result;
}

auto systolic_loop_synth(std::array<hw_base, READ_LENGTH> const& left,
			std::array<sw_ref, READ_LENGTH> const& top,
			matrix<score, SW_HIST, READ_LENGTH> const& smatrix)
	-> matrix<score, SW_HIST, READ_LENGTH>{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=top._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=left._M_instance
	return sloop::systolic(sw_op, left, top, smatrix);
}

int test_sw(){
	int err;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned char> distribution(0, 1<<(8*sizeof(unsigned char) - 1));
	
	std::array<base, READ_LENGTH> read;

	std::array<sw_ref, READ_LENGTH> soln_top;
	std::array<base, REF_LENGTH> soln_ref;
	matrix<score, REF_LENGTH, READ_LENGTH> soln;

	std::array<sw_ref, READ_LENGTH> top;
	std::array<base, REF_LENGTH> ref;
	matrix<score, SW_HIST, READ_LENGTH> smatrix;
	
	// Fill the read 
	read[0] = 'x';
	for(std::size_t i = 1; i < READ_LENGTH; ++i){
		read[i] = randtobase(distribution(gen));
	}

	// Fill the reference
	ref[0] = 'x';
	soln_ref[0] = 'x';
	for(std::size_t i = 1; i < REF_LENGTH; ++i){
		ref[i] = randtobase(distribution(gen));
		soln_ref[i] = ref[i];
	}

	// Initialize Solution Matrix
	for(int y = 0; y < READ_LENGTH; ++y){
		for(int x = 0; x < REF_LENGTH; ++x){
			soln[y][x] = {0,0,0};
		}
	}

	// Compute the entire solution matrix
	for(int cyc = 0; cyc < REF_LENGTH; ++cyc){
		for(int k = READ_LENGTH-1; k > 0; --k){
			soln_top[k] = soln_top[k-1];
		}

		soln_top[0] = {to_hw_base(soln_ref[cyc]), soln_ref[cyc]=='x'};
		for(int sr_idx = 0, sr_cyc = cyc; (sr_idx < READ_LENGTH) && (sr_cyc >= 0); ++sr_idx, sr_cyc--){
			if(sr_idx == 0){
				soln[sr_idx][cyc] = {0,0,0};
			} else{
				if(soln_top[sr_idx].last){
					soln[sr_idx][sr_cyc] = {(char)-(sr_idx+1), (char)-(sr_idx+1), (char)-(sr_idx+1)};
				} else {
					score temp;
					char sigma = (to_hw_base(read[sr_idx]) == soln_top[sr_idx].b)? 2: -2;
					temp.e = std::max(soln[sr_idx][sr_cyc-1].v - ALPHA, 
							soln[sr_idx][sr_cyc-1].e - BETA);
					temp.f = std::max(soln[sr_idx-1][sr_cyc].v - ALPHA, 
							soln[sr_idx-1][sr_cyc].f - BETA);
					temp.v = std::max((char)(soln[sr_idx-1][sr_cyc-1].v + sigma), 
							std::max(temp.e, temp.f));
					soln[sr_idx][sr_cyc] = temp;
				}
			}
		}
	}

	// Check recursive implementation
	// Initialize top
	for(int y = 0; y < READ_LENGTH; ++y){
			top[y] = {to_hw_base('x'),false};
	}

	// Initialize score matrix
	for(int y = 0; y < READ_LENGTH; ++y){
		for(int x = 0; x < SW_HIST; ++x){
			smatrix[y][x] = {0,0,0};
		}
	}

	for(int ref_i = 0; ref_i < REF_LENGTH; ++ref_i){
		// Adjust top on each iteration
		top = hops::rshift({to_hw_base(ref[ref_i]), ref[ref_i] == 'x'}, top);
		// Update smatrix with the implementation
		smatrix = systolic_hop_synth(hops::map(to_hw_base, read), top, smatrix);
		// Check the result
		for(int j = 0; j < READ_LENGTH; ++j){
			if(ref_i - 1 - j >= 0){
				if(smatrix[j][0].v != soln[j][ref_i - j].v){
					std::cerr << "Error! Loop Output does not match";
					std::cerr << "on index " << j << "cycle" << ref_i;
					std::cerr << "!" << std::endl;

					std::cerr << "Soln: " << soln[j][ref_i - j].v;
					std::cerr << " Output: " << smatrix[j][0].v << std::endl;

					std::cout << "Solution State:" << std::endl;
					print_state(read, soln_ref, soln);

					std::cout << "Implementation State:" << std::endl;
					print_state(read, top, smatrix);

					return -1;
				}
			}
		}
		
	}

	// Loop checking
	// Initalize Top
	for(int y = 0; y < READ_LENGTH; ++y){
			top[y] = {to_hw_base('x'),false};
	}

	// Initialize score matrix
	for(int y = 0; y < READ_LENGTH; ++y){
		for(int x = 0; x < SW_HIST; ++x){
			smatrix[y][x] = {0,0,0};
		}
	}

	for(int ref_i = 0; ref_i < REF_LENGTH; ++ref_i){
		// Adjust top on each iteration
		top = hops::rshift({to_hw_base(ref[ref_i]), ref[ref_i] == 'x'}, top);
		// Update smatrix with the implementation
		smatrix = systolic_loop_synth(hops::map(to_hw_base, read), top, smatrix);
		// Check the result
		for(int j = 0; j < READ_LENGTH; ++j){
			if(ref_i - 1 - j >= 0){
				if(smatrix[j][0].v != soln[j][ref_i - j].v){
					std::cerr << "Error! Loop Output does not match";
					std::cerr << "on index " << j << "cycle" << ref_i;
					std::cerr << "!" << std::endl;

					std::cerr << "Soln: " << soln[j][ref_i - j].v;
					std::cerr << " Output: " << smatrix[j][0].v << std::endl;

					std::cout << "Solution State:" << std::endl;
					print_state(read, soln_ref, soln);

					std::cout << "Implementation State:" << std::endl;
					print_state(read, top, smatrix);

					return -1;
				}
			}
		}
		
	}
	std::cout << "Smith-Waterman Test Passed!" << std::endl;
	return 0;
}

int main(){
	int err = 0;
	if((err = test_sw())){
		return err;
	}
	return 0;
}
