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
#ifndef __SMITH_WATERMAN_HPP
#define __SMITH_WATERMAN_HPP
#include <stdio.h>
#include <array>

#define MATCH 2
#define ALPHA 2
#define BETA 1
#define REF_LENGTH 32
#define READ_LENGTH 16
#define SW_HIST 2

typedef char base;
#ifdef BIT_ACCURATE
#include "ap_int.h"
typedef ap_uint<2> hw_base;
#else
typedef char hw_base;
#endif


template<typename T>
base randtobase(T IN){
	switch(IN >> (8*sizeof(IN)-3)){
	case 0:return 'a';
	case 1:return 'c';
	case 2:return 't';
	case 3:return 'g';
	default: return 'x';
	}
}

struct toHwBase{
	hw_base operator()(base const& IN) const{
#ifdef BIT_ACCURATE
		switch(IN){
		case 'a':
			return 0;
		case 'c':
			return 1;
		case 't':
			return 2;
		case 'g':
			return 3;
		default:
			return 0;
		}
#else
		return IN;
#endif
	}
} to_hw_base;

struct toSwBase{
	base operator()(hw_base const& IN) const{
#ifdef BIT_ACCURATE
		switch(IN){
		case 0:
			return 'a';
		case 1:
			return 'c';
		case 2:
			return 't';
		case 3:
			return 'g';
		default:
			return 'x';
		}
#else
		return IN;
#endif
	}
} to_sw_base;


struct score{
	char v, e, f;
};

struct sw_ref{
	hw_base b;
	bool last;
};

template<std::size_t LEN>
void print_top(char const& matid, std::array<char, LEN> const& top){
	printf("%c", matid);
	for (auto t : top) {
		printf("%3c ", t);
	}
	printf("\n");
}

template<std::size_t LEN>
void print_top(char const& matid, std::array<sw_ref, LEN> const& top){
	printf("%c", matid);
	for (auto t : top) {
		printf("%3c ", to_sw_base(t.b));
	}
	printf("\n");
}

template<std::size_t LEN>
void print_row(char const& matid, base const& lbase, std::array<score, LEN> const& row){
	printf("%c", lbase);
	for (auto r : row) {
		switch(matid) {
		case 'E':
			printf("%3d ", r.e);
			break;
		case 'F':
			printf("%3d ", r.f);
			break;
		case 'V':
			printf("%3d ", r.v);
			break;
		default:
			break;
		}
	}
	printf("\n");
}


template<std::size_t SW, std::size_t W, std::size_t H, typename T>
void print_mat(char matid, std::array<base, H> left, std::array<T, W> top, matrix<score, SW, H> smatrix){
	print_top(matid, top);
	for(int j = 0; j < H; ++j){
		print_row(matid, left[j], smatrix[j]);
	}
	printf("\n");
}

template<std::size_t SW, std::size_t W, std::size_t H, typename T>
void print_state(std::array<base, H> left, std::array<T, W> top, matrix<score, SW, H> smatrix){
	print_mat('E', left, top, smatrix);
	print_mat('F', left, top, smatrix);
	print_mat('V', left, top, smatrix);
}

#endif //__SMITH_WATERMAN_HPP
