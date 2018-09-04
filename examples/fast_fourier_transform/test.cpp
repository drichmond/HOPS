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
#include <complex>
#include "utility.hpp"
#include "fft.hpp"
#ifdef BIT_ACCURATE
#include "hls_math.h"
#include "ap_fixed.h"
#include "ap_int.h"
#define DTYPE ap_fixed<32, 16, AP_RND>
#else 
#define DTYPE float
#endif

#define LOG_LIST_LENGTH 4
#define LIST_LENGTH (1<<LOG_LIST_LENGTH)

std::array<std::complex<DTYPE>, LIST_LENGTH> fft_loop_synth(std::array<std::complex<DTYPE>, LIST_LENGTH> const& IN){
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=IN._M_instance COMPLETE
	return loop::fft(IN);
}

std::array<std::complex<DTYPE>, LIST_LENGTH> bitreverse_loop_synth(std::array<std::complex<DTYPE>, LIST_LENGTH> const& IN){
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=IN._M_instance COMPLETE
	return loop::bitreverse(IN);
}

std::array<std::complex<DTYPE>, LIST_LENGTH> nptfft_loop_synth(std::array<std::complex<DTYPE>, LIST_LENGTH/2> const& L,
							std::array<std::complex<DTYPE>, LIST_LENGTH/2> const& R){
	return loop::nPtFFT(L, R);
}

std::array<std::complex<DTYPE>, LIST_LENGTH> fft_hop_synth(std::array<std::complex<DTYPE>, LIST_LENGTH> IN){
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=IN._M_instance COMPLETE
#pragma HLS_INLINE
	return fft(IN);
}

std::array<std::complex<DTYPE>, LIST_LENGTH> bitreverse_hop_synth(std::array<std::complex<DTYPE>, LIST_LENGTH> const& IN){
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=IN._M_instance COMPLETE
	return bitreverse(IN);
}

std::array<std::complex<DTYPE>, LIST_LENGTH> nptfft_hop_synth(std::array<std::complex<DTYPE>, LIST_LENGTH/2> L, std::array<std::complex<DTYPE>, LIST_LENGTH/2> R){
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=L._M_instance COMPLETE
#pragma HLS ARRAY_PARTITION variable=R._M_instance COMPLETE
	return nPtFFT(L, R);
}

void bit_reverse(DTYPE X_R[LIST_LENGTH], DTYPE X_I[LIST_LENGTH]){
	for(unsigned int t = 0; t < LIST_LENGTH; t++){
		DTYPE temp;
		unsigned int b = 0;
		for(int j = 0; j < LOG_LIST_LENGTH; j++){
			b = b << 1;
			b |= ((t >> j) &1);
		}
		if(t > b){
			temp = X_R[t];
			X_R[t] = X_R[b];
			X_R[b] = temp;
		
			temp = X_I[t];
			X_I[t] = X_I[b];
			X_I[b] = temp;
		}
	}
}

void fft(DTYPE X_R[LIST_LENGTH], DTYPE X_I[LIST_LENGTH])
{

	DTYPE temp_R;		/*temporary storage complex variable*/
	DTYPE temp_I;		/*temporary storage complex variable*/


	int i,j,k;			/* loop indexes */
	int i_lower;		/* Index of lower point in butterfly */
	int step;

	int stage;
	int DFTpts;
	int BFSkip;			/*Butterfly Width*/

	int N2 = LIST_LENGTH>>1;	/* N2=N>>1 */

	/*=====================BEGIN BIT REVERSAL===========================*/
	unsigned int t, b;
	DTYPE temp;
	bit_reverse(X_R, X_I);

	/*++++++++++++++++++++++END OF BIT REVERSAL++++++++++++++++++++++++++*/

	/*=======================BEGIN: FFT=========================*/
	// Do M stages of butterflies
	step=N2;
	DTYPE a, e, c, s;
	int counter;
stages:
	for(stage=1; stage<= LOG_LIST_LENGTH; ++stage){
		DFTpts = 1 << stage;		// DFT = 2^stage = points in sub DFT
		BFSkip = DFTpts/2; 			// Butterfly WIDTHS in sub-DFT
		k=0;
		
		e = -2*M_PI/DFTpts;

		a = 0.0;
		counter = 0;
		// Perform butterflies for j-th stage
	butterfly:
		for(j=0; j<BFSkip; ++j){
			c = cos((double)a);
			s = sin((double)a);
			// Compute butterflies that use same W**k
		DFTpts:
			for(i=j; i<LIST_LENGTH; i += DFTpts){
				counter ++;
				i_lower = i + BFSkip; //index of lower point in butterfly
				//printf("%d %d %d a:%4f \tr:%4f \ti:%4f \trl:%4f \til:%4f\n", stage, i, i_lower, a, X_R[i], X_I[i], X_R[i_lower], X_I[i_lower]);
				temp_R = X_R[i_lower]*c+ X_I[i_lower]*s;
				temp_I = X_I[i_lower]*c- X_R[i_lower]*s;

				X_R[i_lower] = X_R[i] - temp_R;
				X_I[i_lower] = X_I[i] - temp_I;
				X_R[i] = X_R[i] + temp_R;
				X_I[i] = X_I[i] + temp_I;
			}
			a = a + e;
			k+=step;
		}
		step=step/2;
	}
}

int fft_test(){
	std::array<std::complex<DTYPE>, LIST_LENGTH> in, out;
	DTYPE gold_real[LIST_LENGTH];
	DTYPE gold_imag[LIST_LENGTH];

	for(int i = 0; i < LIST_LENGTH; i ++){
		in[i] = {(DTYPE)(i + 1), (DTYPE)0};
		gold_real[i] = i + 1;
		gold_imag[i] = 0;
	}

	fft(gold_real, gold_imag);

	out = fft_hop_synth(in);
	for(int i = 0; i < LIST_LENGTH; i ++){
		if(std::abs(float(gold_real[i] - out[i].real())) > .25){
			std::cout << "Error! Recursive Real FFT Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
		if(std::abs(float(gold_imag[i] - out[i].imag())) > .25){
			std::cout << "Error! Recursive Imaginary FFT Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
	}
	out = fft_loop_synth(in);
	for(int i = 0; i < LIST_LENGTH; i ++){
		if(std::abs(float(gold_real[i] - out[i].real())) > .25){
			std::cout << "Error! Loop Real FFT Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
		if(std::abs(float(gold_imag[i] - out[i].imag())) > .25){
			std::cout << "Error! Loop Imaginary FFT Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
	}
	std::cout << "Passed FFT tests!" << std::endl;
	return 0;
}

int bitreverse_test(){
	std::array<std::complex<DTYPE>, LIST_LENGTH> in, out;
	DTYPE gold_real[LIST_LENGTH];
	DTYPE gold_imag[LIST_LENGTH];

	for(int i = 0; i < LIST_LENGTH; i ++){
		in[i] = {(DTYPE)(i + 1), (DTYPE)0};
		gold_real[i] = i + 1;
		gold_imag[i] = 0;
	}

	bit_reverse(gold_real, gold_imag);

	out = bitreverse_hop_synth(in);
	for(int i = 0; i < LIST_LENGTH; i ++){
		if(std::abs(float(gold_real[i] - out[i].real())) > .25){
			std::cout << "Error! Loop Real Bitreverse Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
		if(std::abs(float(gold_imag[i] - out[i].imag())) > .25){
			std::cout << "Error! Loop Imaginary Bitreverse Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
	}

	out = bitreverse_loop_synth(in);
	for(int i = 0; i < LIST_LENGTH; i ++){
		if(std::abs(float(gold_real[i] - out[i].real())) > .25){
			std::cout << "Error! Loop Real Bitreverse Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
		if(std::abs(float(gold_imag[i] - out[i].imag())) > .25){
			std::cout << "Error! Loop Imaginary Bitreverse Values at index "
				  << i << "did not match" << std::endl;
			return -1;
		}
	}
	std::cout << "Passed Bitreverse tests!" << std::endl;
	return 0;
}


int main(){
	int err;
	if((err = bitreverse_test())){
		return err;
	}
	if((err = fft_test())){
		return err;
	}
	std::cout << "FFT Tests Passed!" << std::endl;
	return 0;	
}

