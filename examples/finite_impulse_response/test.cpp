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
#include <functional>
#include <array>
#include <iostream>

#include "arrayops.hpp"
#include "reduce.hpp"
#include "zip.hpp"
#include "utility.hpp"

#include "fir.hpp"

#ifdef BIT_ACCURATE
#include "ap_int.h"
typedef ap_fixed<32, 4> coef_t;
typedef ap_fixed<32, 8> data_t;
typedef ap_fixed<32, 8> acc_t;
#else
typedef float coef_t;
typedef float data_t;
typedef float acc_t;
#endif

data_t gold_fir(data_t signal){
	static std::array<data_t, C_NUM_TAPS> buffer;
	static const std::array<data_t, C_NUM_TAPS> coeffs = {COEFFS_INIT};
	data_t acc = 0;
	buffer = hops::rshift(signal, buffer);
	for(int i = 0; i < C_NUM_TAPS; i++){
		acc += buffer[i]*coeffs[i];
	}
	return acc;
}

data_t fir_hop_synth(data_t const& IN){
#pragma HLS PIPELINE
	static std::array<data_t, C_NUM_TAPS> sig;
	static const std::array<data_t, C_NUM_TAPS> coeffs = {COEFFS_INIT};
#pragma HLS ARRAY_PARTITION variable=sig._M_instance COMPLETE
#pragma HLS ARRAY_PARTITION variable=coeffs._M_instance COMPLETE
	sig = hops::rshift(IN, sig);
	return hops::rreduce(std::plus<data_t>(),
			hops::zipWith(std::multiplies<data_t>(),
				sig, coeffs), 0);
}

data_t fir_loop_synth(data_t const& IN){
#pragma HLS PIPELINE
	static std::array<data_t, C_NUM_TAPS> sig;
	static const std::array<data_t, C_NUM_TAPS> coeffs = {COEFFS_INIT};	
#pragma HLS ARRAY_PARTITION variable=sig._M_instance COMPLETE
#pragma HLS ARRAY_PARTITION variable=coeffs._M_instance COMPLETE
	data_t acc = 0;
	for(std::size_t i = C_NUM_TAPS - 1; i > 0; --i){
#pragma HLS UNROLL
		sig[i] = sig[i - 1];
		acc += sig[i] * coeffs[i];
	}
	sig[0] = IN;
	acc += sig[0] * coeffs[0];
	return acc;
}

int main () {
	std::array<int, C_NUM_SAMPLES> input;
	fill_normal(input); 
	data_t out, gold;
  
	for (int i = 0; i < C_NUM_SAMPLES; ++i) {
		out = fir_hop_synth(input[i]);
		gold = gold_fir(input[i]);
		if(std::abs((float)out - (float)gold) > .1){
			std::cout << "Error! HOP Output does not match "
				  << "at iteration  "<< i << std::endl;
			std::cout << "Input: " << input[i] << " Out: "
				  << out << " Gold: " << gold << std::endl;
			return -1;
		}
		out = fir_loop_synth(input[i]);
		if(std::abs((float)out - (float)gold) > .1){
			std::cout << "Error! Loop Output does not match "
				  << "at iteration  "<< i << std::endl;
			std::cout << "Input: " << input[i] << " Out: "
				  << out << " Gold: " << gold << std::endl;
			return -1;
		}
	}
	std::cout << "FIR Test passed!" << std::endl;
}

