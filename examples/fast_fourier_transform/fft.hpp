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
#ifndef __FFT_HPP
#define __FFT_HPP
#include <complex>
#include <cmath>
#include "zip.hpp"
#include "arrayops.hpp"
#include "divconq.hpp"
#include "reduce.hpp"

template <typename T>
using FFT_t = std::complex<T>;

typedef std::pair<std::size_t, std::size_t> ctx_t;

template <typename T>
using data_t = std::pair<FFT_t<T>, FFT_t<T>>;

struct FFTOP{
	template <typename T> 
	data_t<T> operator()(ctx_t const& CTX, data_t<T> const& DATA) const {
#pragma HLS INLINE
		std::size_t LEV = CTX.first;
		std::size_t IDX = CTX.second;
		FFT_t<T> ti = DATA.first, bi = DATA.second, to, bo;
		T angle = (-2*M_PI*IDX)/(1<<(LEV));
		T c = cos((-2*M_PI*IDX)/(1<<(LEV)));
		T s = sin((-2*M_PI*IDX)/(1<<(LEV)));
		T temp_r = c*std::real(bi) + s*std::imag(bi);
		T temp_i = c*std::imag(bi) - s*std::real(bi);
		to.real(ti.real() + temp_r);
		to.imag(ti.imag() + temp_i);
		bo.real(ti.real() - temp_r);
		bo.imag(ti.imag() - temp_i);
		data_t<T> out = {to, bo};
		return out;
	}
} fftop;

struct NPtFFT{
	template <typename T, std::size_t LEN>
	auto operator()(std::array<FFT_t<T>, LEN> const& L,
			std::array<FFT_t<T>, LEN> const& R) const
		-> std::array<FFT_t<T>, 2*LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
		auto depth = hops::replicate<LEN, unsigned long long>(log2(LEN) + 1);
		auto indicies = hops::range<LEN>();
		auto contexts = hops::zip(depth, indicies);
		auto inputs = hops::zip(L, R);
		auto outputs = hops::unzip(hops::zipWith(fftop, contexts, inputs));
		return hops::concatenate(outputs.first, outputs.second);
	}
} nPtFFT;

struct Interleave{
	template <typename T, std::size_t LEN>
	auto operator()(std::array<T, LEN> const& L, std::array<T, LEN> const& R) const
		-> std::array<T, 2*LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
		std::array<T, 0> init; 		
		return hops::rreduce(hops::concatenate,
				hops::zipWith(hops::construct, L, R), init);
	}
} interleave;

struct Bitreverse{
	template <typename T, std::size_t LEN>
	auto operator()(std::array<T, LEN> const& IN) const
		-> std::array<T, LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
		return hops::divconq(interleave, IN);
	}
} bitreverse;

template<typename T, std::size_t LEN>
auto fft(std::array<std::complex<T>, LEN> const& IN)
	-> std::array<std::complex<T>, LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
	return hops::divconq(nPtFFT, bitreverse(IN));
}

namespace loop{
	template <typename T, std::size_t LEN>
	std::array<T, LEN> bitreverse(std::array<T, LEN> const& IN){
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
		static const size_t LEV = hops::log2(LEN);
		std::array<T, LEN> out;
#pragma HLS ARRAY_PARTITION complete VARIABLE=out._M_instance
		for(std::size_t t = 0; t < LEN; t++){
			unsigned int b = 0;
			for(std::size_t j = 0; j < LEV; j++){
				b = b << 1;
				b |= ((t >> j) &1);
			}
			out[t] = IN[b];
			out[b] = IN[t];
		}
		return out;
	}

	template <typename T, std::size_t LEN>
	std::array<T, LEN*2> nPtFFT(std::array<T, LEN> const& L,
				std::array<T, LEN> const& R){
		std::array<T, LEN*2> in;
		std::array<T, LEN*2> out;
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=L._M_instance COMPLETE
#pragma HLS ARRAY_PARTITION variable=R._M_instance COMPLETE
#pragma HLS ARRAY_PARTITION variable=in._M_instance COMPLETE
#pragma HLS ARRAY_PARTITION variable=out._M_instance COMPLETE
		for(std::size_t i = 0; i < LEN; ++i){
#pragma HLS UNROLL
			in[i] = L[i];
			in[i + LEN] = R[i];
		}
		std::size_t l = hops::clog2(LEN);
		std::size_t dftp = (1<<(l + 1));
		std::size_t stride = 1<<l;
		std::size_t mask = stride - 1;
	bfly_pair:
		for(std::size_t i = 0; i < (1 << hops::clog2(LEN)); ++i){
#pragma HLS UNROLL
			std::size_t grp = i >> l;
			std::size_t top = i & mask;
			std::size_t base = top + (grp<<(l + 1));
			std::pair<T, T> d = {in[base], in[base + stride]};
			std::pair<std::size_t, std::size_t> ctx = {l + 1, top};
			std::pair<T, T> o = fftop(ctx, d);
			out[base]= o.first;
			out[base + stride] = o.second;
		}
		return out;

	}

	template <class FN, std::size_t LEN, typename T>
	std::array<T, LEN> butterfly(FN const& F, std::array<T, LEN> IN){
#pragma HLS INLINE

		static const int LEV = hops::clog2(LEN);
		std::array<std::array<T, LEN>, LEV + 1> stagearr;
#pragma HLS ARRAY_PARTITION complete VARIABLE=stagearr._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=stagearr._M_instance[0]
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
		stagearr[0]  = IN;
	bfly_level:
		for(std::size_t l = 0; l < LEV; ++l){
#pragma HLS UNROLL
			std::size_t dftp = (1<<(l + 1));
			std::size_t stride = 1<<l;
			std::size_t mask = stride - 1;
		bfly_pair:
			for(std::size_t i = 0; i < (1 << (LEV - 1)); ++i){
#pragma HLS UNROLL
				std::size_t grp = i >> l;
				std::size_t top = i & mask;
				std::size_t base = top + (grp<<(l + 1));
				std::pair<T, T> in = {stagearr[l][base], stagearr[l][base + stride]};
				std::pair<std::size_t, std::size_t> ctx = {l + 1, top};
				std::pair<T, T> o = FN()(ctx, in);
				stagearr[l+1][base]= o.first;
				stagearr[l+1][base + stride] = o.second;
			}
		}
		return stagearr[LEV];
	}

	template<typename T, std::size_t LEN>
	std::array<std::complex<T>, LEN> fft(std::array<std::complex<T>, LEN> IN){
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
		auto res = butterfly(fftop, loop::bitreverse(IN));
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
		return res;
	}
}
#endif
