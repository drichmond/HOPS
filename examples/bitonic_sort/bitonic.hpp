// ----------------------------------------------------------------------
// Copyright (c) 2016, The Regents of the University of California All
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
#ifndef __BITONIC_HPP
#define __BITONIC_HPP
#include <array>
#include <utility>
#include "divconq.hpp"
#include "zip.hpp"
#include "map.hpp"
#include "arrayops.hpp"
#include "reduce.hpp"

template <bool min>
struct CAS;

template <>
struct CAS<true>{
	template <typename T>
	std::pair<T, T> operator()(T const& l, T const& r) const{
#pragma HLS INLINE
		T lo, ro;
		if(l <= r){
			lo = r; ro = l;
		} else {
			lo = l; ro = r;
		}
		return std::pair<T, T>{lo,ro};
	}
};

template <>
struct CAS<false>{
	template <typename T>
	std::pair<T, T> operator()(T const& l, T const& r) const{
#pragma HLS INLINE
		T lo, ro;
		if(l >= r){
			lo = r; ro = l;
		} else {
			lo = l; ro = r;
		}
		return std::pair<T, T>{lo,ro};
	}
};

CAS<true> min;
CAS<false> max;

struct Interleave{
	template <typename T, std::size_t LEN>
	auto operator()(std::array<T, LEN> const& L, std::array<T, LEN> const& R) const
		-> std::array<T, 2*LEN> {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
		std::array<T, 0> init; 		
#pragma HLS ARRAY_PARTITION complete VARIABLE=init._M_instance
		return hops::rreduce(hops::concatenate,
				hops::zipWith(hops::construct, L, R), init);
	}
} interleave;

struct Bitreverse{
	template <typename T, std::size_t LEN>
	auto operator()(std::array<T, LEN> const& IN) const
		-> std::array<T, LEN> {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
		return hops::divconq(interleave, IN);
	}
} bitreverse;

struct BitonicOperator{
	template <typename T>
	std::array<T, 2> operator()(std::array<T,1> const& L, std::array<T,1> const& R) const {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
		return hops::uncurry(hops::concatenate)(hops::unzip(hops::zipWith(min, L, R)));
	}

	template <typename T, std::size_t LEN>
	std::array<T, 2*LEN> operator()(std::array<T,LEN> const& L, std::array<T,LEN> const& R) const {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
		// calls divconq on each sub-branch
		auto p = hops::unzip(hops::zipWith(min, L, hops::reverse(R)));
		auto swap = hops::compose(hops::uncurry(hops::concatenate), hops::compose(hops::unzip, hops::partial(hops::zipWith, min)));
		auto l = hops::divconq(swap, bitreverse(p.first));
		auto r = hops::divconq(swap, bitreverse(p.second));
#pragma HLS ARRAY_PARTITION complete VARIABLE=l._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=r._M_instance
		return hops::concatenate(bitreverse(l), bitreverse(r));
	}
} bitonicOperator;

template <typename T, std::size_t LEN>
std::array<T, LEN> bitonic_sort(std::array<T,LEN> const& IN){
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
	return hops::divconq(bitonicOperator, IN);
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
#pragma HLS UNROLL
			unsigned int b = 0;
			for(std::size_t j = 0; j < LEV; j++){
#pragma HLS UNROLL
				b = b << 1;
				b |= ((t >> j) &1);
			}
			out[t] = IN[b];
			out[b] = IN[t];
		}
		return out;
	}

	template <class FN, std::size_t LEN, typename T>
	std::array<T, LEN> butterfly(FN const& F, std::array<T, LEN> const& IN){
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
		static const int LEV = hops::clog2(LEN);
		std::array<std::array<T, LEN>, LEV + 1> stagearr;
#pragma HLS ARRAY_PARTITION complete VARIABLE=stagearr._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=stagearr[0]._M_instance
	bfly_in:
		for(int i = 0; i< LEN; ++i){
			stagearr[0][i] = IN[i];
		}
		
	bfly_level:
		for(int l = 0; l < LEV; ++l){
			std::size_t dftp = (1<<(l+1));
			std::size_t stride = 1<<l;
			std::size_t mask = stride - 1;
		bfly_pairs:
			for(int i = 0; i < (1<<(LEV-1)); ++i){
				std::size_t grp = i >> l;
				std::size_t top = i & mask;
				std::size_t base = top + (grp<<(l+1));

				std::pair<T, T> o = F(stagearr[l][base], 
						stagearr[l][base + stride]);
				stagearr[l+1][base]= o.first;
				stagearr[l+1][base + stride] = o.second;
			}
		}
		return stagearr[LEV];
	}

	struct BitonicStage{
		template <typename T>
		std::array<T, 2*1> operator()(std::array<T,1> const& L, std::array<T,1> const& R) const {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
			return hops::concatenate(L, R);
		}

		template <typename T, std::size_t LEN>
		std::array<T, 2*LEN> operator()(std::array<T,LEN> const& L, std::array<T,LEN> const& R) const {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
			auto lsub = bitreverse(butterfly(max, bitreverse(L)));
			auto rsub = bitreverse(butterfly(min, bitreverse(R)));
#pragma HLS ARRAY_PARTITION complete VARIABLE=lsub._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=rsub._M_instance
			return hops::concatenate(lsub, rsub);
		}
	} bitonicStage;

	template <typename T, std::size_t LEN>
	std::array<T, LEN> bitonic_sort(std::array<T,LEN> const& IN){
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
		return bitreverse(butterfly(min, bitreverse(hops::divconq(bitonicStage, IN))));
	}
}
#endif
