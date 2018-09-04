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
// **********************************************************************
// FILENAME :
//        arrayops.hpp
//
// DESCRIPTION :
//        This file implements a collection of array manipulation
//        operations that are useful for implementing Higher-Order
//        Functions.
//       
// NAMESPACES :
//        hops
//
// PUBLIC MODULES :
//        Range
//        Reverse
//        Prepend
//        Append
//        Concatenate
//        Split
//        Head
//        Tail
//        Rrotate
//        Rshift
//        Lrotate
//        Lshift
//        Construct
//
// PUBLIC FUNCTIONS :
//        log2
//        clog2
//        range
//        replicate
//        split
//
// PUBLIC VARIABLES :
//        reverse (Instance of Reverse)
//        prepend (Instance of Prepend)
//        append (Instance of Append)
//        concatenate (Instance of Concatenate)
//        head (Instance of Head)
//        last (Instance of Last)
//        tail (Instance of Tail)
//        rrotate (Instance of Rrotate)
//        rshift (Instance of Lshift)
//        lrotate (Instance of Lrotate)
//        lshift (Instance of Lshift)
//        construct (Instance of Construct)
//
// NOTES :
//        This file requires a compiler implementing the C++11
//        standard.
//
// AUTHOR : Dustin Richmond (drichmond)
//
// **********************************************************************
#ifndef __ARRAYOPS_HPP
#define __ARRAYOPS_HPP
#include <array>
#include "functools.hpp"
namespace hops {
	// FUNCTION :
	//        log2
	// 
	// DESCRIPTION :
	//        Computes the truncated base-2 logarithm of a value,
	//        where any input value less than 2 returns 1.
	// 
	// ARGUMENTS :
	//        n : std::size_t
	//            Input value to compute the logarithm of
	// 
	// RETURNS : std::size_t
	//        The truncated logarithm of the input value.
	//
	// NOTES :
	//        Any input value less than 2 returns 1.
	constexpr std::size_t log2(std::size_t n)
	{
		return ((n <= 2) ? 1 : 1 + log2(n / 2));
	}

	// FUNCTION :
	//        clog2
	// 
	// DESCRIPTION :
	//        Computes the "ceiling" base-2 logarithm of a value.
	//        Any input value less than 2 returns 1, and when an
	//        input value is a non-power of 2, the output is
	//        rounded to the next largest integer.
	// 
	// ARGUMENTS :
	//        n : std::size_t
	//            Input value to compute the logarithm of
	// 
	// RETURNS : std::size_t
	//        The ceiling logarithm of the input value.
	//
	// NOTES :
	//        Any input value less than 2 returns 1.
	constexpr std::size_t clog2(std::size_t n)
	{
		return log2(n) + (n > (1 << log2(n)));
	}

	// FUNCTION :
	//        range
	// 
	// DESCRIPTION :
	//        Returns an array with length equal to
	//        (STOP-START)/STEP, first element equal to START, and
	//        subsequent values spaced by STEP
	// 
	// PARAMETERS :
	//        STOP : std::size_t
	//            Greater than or equal to the maximum value in
	//            the returned array.
	// 
	//        START : std::size_t
	//            The value at 0th index in the returned array.
	// 
	//        STEP : std::size_t
	//            The linear space between values in the array.
	// 
	// RETURNS : std::array<std::size_t, (STOP-START)/STEP>
	//        The returned array has length equal to
	//        (STOP-START)/STEP, with the first element equal to
	//        START and subsequent values spaced by STEP
	//
	// NOTES :
	//        No bounds checking is performed. When STOP > START,
	//        the compiler will fail.
	template<std::size_t STOP, std::size_t START = 0, std::size_t STEP = 1>
	auto range() -> std::array<std::size_t, (STOP-START)/STEP>{
#pragma HLS INLINE
		std::array<std::size_t, (STOP-START)/STEP> temp;
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
	range_loop:
		for(std::size_t v = START, idx = 0; v < STOP; v +=STEP, ++idx){
#pragma HLS UNROLL
			temp[idx] = v;
		}
		return temp;
	};
	
	// MODULE :
	//        struct Range
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the range
	//        function described above, as the operator() function.
	// 
	// PARAMETERS :
	//        STOP : std::size_t
	//            Greater than or equal to the maximum value in
	//            the returned array.
	// 
	//        START : std::size_t
	//            The value at 0th index in the returned array.
	// 
	//        STEP : std::size_t
	//            The linear space between values in the array.
	//
	// FUNCTIONS :
	//        auto operator()() const
	template<int STOP, int START = 0, int STEP = 1>
	struct Range{
		// FUNCTION :
		//        operator() const
		// 
		// DESCRIPTION : 
		//        Returns an array with length equal to
		//        (STOP-START)/STEP, first element equal to
		//        START, and subsequent values spaced by STEP
		// 
		// RETURNS : std::array<std::size_t,
		//        (STOP-START)/STEP> The returned array has
		//        length equal to (STOP-START)/STEP, with the
		//        first element equal to START and subsequent
		//        values spaced by STEP
		//
		// NOTES :
		//        No bounds checking is performed. When STOP >
		//        START, the compiler will fail.
		//
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		auto operator()() const
			-> decltype(range<STOP, START, STEP>()) {
#pragma HLS INLINE
			auto temp = range<STOP, START, STEP>();
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
			return temp;
		}
	};

	// MODULE :
	//        struct Reverse
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the reverse
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Reverse{
		// FUNCTION :
		//        operator() const
		// 
		// DESCRIPTION :
		//        Returns a new array with the elements of the
		//        input array in reverse order.
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		// 
		// RETURNS : std::array<T, LEN>
		//            A new array with elements from the first
		//            array in reverse order.
		// 
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		auto operator()(std::array<T, LEN> const& IN) const
			-> std::array<T, LEN> {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
			std::array<T, LEN> res;
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
		reverse_loop:
			for(int idx = 0; idx < LEN; ++idx){
#pragma HLS UNROLL
				res[idx] = IN[LEN-(idx+1)];
			}
			return res;
		}
	};
	// VARIABLE :
	//        reverse (Instance of Reverse)
	// 
	// DESCRIPTION :
	//        Instantion of the Reverse struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 2> a = {1, 2}, b;
	//            b = reverse(a); // b === {2, 1}
	Reverse reverse;

	// MODULE :
	//        struct Prepend
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the prepend
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(T const&, std::array<T, LEN> const&) const
	struct Prepend{
		// FUNCTION :
		//        operator() const
		// 
		// DESCRIPTION :
		//        Given a value and an N-element array, return
		//        a N+1-element array with the new value at
		//        as the first element
		//
		// PARAMETERS : (Inferred)
		//        T : typename
		//            Type of the input element and elements
		//            in the input std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        V : T const&
		//            The input value to prepend
		//
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : std::array<T, LEN+1>
		//        An array with the new element appended to
		//        the front
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		auto operator()(T const& V, std::array<T, LEN> const& IN) const
			-> std::array<T, LEN+1> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			std::array<T, LEN+1> res;
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
			res[0] = V;
		prep_loop:
			for(int idx = 0; idx < LEN; ++idx){
#pragma HLS UNROLL
				res[idx+1] = IN[idx];
			}
			return res;
		}
	};
	// VARIABLE :
	//        prepend (Instance of Prepend)
	// 
	// DESCRIPTION :
	//        Instantion of the Prepend struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//            int a = 0;
	//            std::array<int, 2> b = {1, 2};
	//            std::array<int, 3> c;
	//            c = prepend(a, b); // c === {0, 1, 2}
	Prepend prepend;

	// MODULE :
	//        struct Append
	//
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the append
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&, T const&) const
	struct Append{
		// FUNCTION :
		//        operator() const
		// 
		// DESCRIPTION :
		//        Given an N-element array and a value, return
		//        a N+1-element array with the new value as the
		//        last element
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the input element and elements
		//            in the input std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		//        V : T const&
		//            The input value to append
		//
		// RETURNS : std::array<T, LEN+1>
		//        An array with the new element appended to
		//        the end
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		auto operator()(std::array<T, LEN> const& IN, T const& V) const
			-> std::array<T, LEN+1> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			std::array<T, LEN+1> res;
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
		append_loop:
			for(int idx = 0; idx < LEN; ++idx){
#pragma HLS UNROLL
				res[idx] = IN[idx];
			}
			res[LEN] = V;
			return res;
		}
	};	
	// VARIABLE :
	//        append (Instance of Append)
	// 
	// DESCRIPTION :
	//        Instantion of the Append struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 2> a = {1, 2};
	//            int b = 3;
	//            std::array<int, 3> c;
	//            c = append(a, b); // c === {1, 2, 3}
	Append append;

	// MODULE :
	//        struct Concatenate
	//
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the concatenate
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LLEN> const&,
	//                        std::array<T, RLEN> const&) const
	struct Concatenate{
		// FUNCTION :
		//        operator() const
		// 
		// DESCRIPTION : 
		//        Given an N-element array L, and an
		//        M-element array R, return an N+M-element
		//        array where values from L preceed the values
		//        from R
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the input element and elements
		//            in the input std::array
		//
        	//        LLEN : std::size_t (Inferred)
        	//            Length of the first std::array input argument
		//
        	//        RLEN : std::size_t (Inferred)
        	//            Length of the second std::array input argument
		//
		// ARGUMENTS :
		//        L : std::array<T, LLEN> const&
		//            The first input array
		//
		//        L : std::array<T, RLEN> const&
		//            The second input array
		//
		// RETURNS : std::array<T, LLEN+RLEN>
		//        An array of length LLEN+RLEN where the
		//        values from L preceed the values from R
		// 
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LLEN, std::size_t RLEN>
		auto operator()(std::array<T, LLEN> const& L,
				std::array<T, RLEN> const& R) const
			-> std::array<T, LLEN+RLEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
			std::array<T, LLEN+RLEN> res;
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
		concatenate_left_loop:
			for(int idx = 0; idx < LLEN; ++idx){
#pragma HLS UNROLL
				res[idx] = L[idx];
			}
		concatenate_right_loop:
			for(int ridx = LLEN, idx=0; idx < RLEN; ++ridx, ++idx){
#pragma HLS UNROLL
				res[ridx] = R[idx];
			}
			return res;
		}
	};
	// VARIABLE :
	//        concatenate (Instance of Concatenate)
	// 
	// DESCRIPTION :
	//        Instantion of the Concatenate struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 2> a = {1, 2};
	//            std::array<int, 3> b = {3, 4, 5};
	//            std::array<int, 5> c;
	//            c = concatenate(a, b);
	//            c = {1, 2, 3, 4, 5}
	Concatenate concatenate;

	// FUNCTION :
	//        replicate
	// 
	// DESCRIPTION :
	//        Return a LEN-element array with all elements equal
	//        to the input value.
	// 
	// PARAMETERS :
       	//        LEN : std::size_t
       	//            Length of the generated std::array
	// 
	//        T : typename (Inferred)
	//            Type of the input value and elements in the
	//            output std::array
	//
	// ARGUMENTS :
	//        V : std::size_t
	//            Input value to compute the logarithm of
	//
	// RETURNS : std::array<T, LEN>
	//        Returns a LEN-element array where all elements are
	//        value V
	template<std::size_t LEN, typename T>
	auto replicate(T const& V)
		-> std::array<T, LEN> {
#pragma HLS INLINE
		std::array<T, LEN> res;
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
	replicate_loop:
		for(int idx = 0; idx < LEN; ++idx){
#pragma HLS UNROLL
			res[idx] = V;
		}
		return res;
	}

	// MODULE :
	//        struct Replicate
	//
	// DESCRIPTION :
	//        Function-wrapped-struct wrapping the replicate
	//        function (described above) using the operator()
	//        function.
	//
	// PARAMETERS :
	//        LEN : std::size_t
	//            Length of the generated std::array
	// 
	// FUNCTIONS :
	//        auto operator()(T const&) const
	template<std::size_t LEN>
	struct Replicate{
		// FUNCTION :
		//        operator() const
		// 
		// DESCRIPTION :
		//        Return a LEN-element array with all elements
		//        equal to the input value.
		// 
		// PARAMETERS :
		//        LEN : std::size_t
		//            Length of the generated std::array
		// 
		//        T : typename (Inferred)
		//            Type of the input value and elements in
		//            the output std::array
		//
		// ARGUMENTS :
		//        V : std::size_t
		//            Input value to compute the logarithm of
		//
		// RETURNS : std::array<T, LEN>
		//        Returns a LEN-element array where all
		//        elements are value V
		//
		// EXAMPLE :
		//            int a = 42;
		//            std::array<int, 10> b, c;
		//            b = replicate<10>()(a);
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T>
		auto operator()(T const& V) const 
			-> std::array<T, LEN> {
#pragma HLS INLINE
			return replicate<LEN>(V);
		}
	};

	// FUNCTION :
	//        split
	// 
	// DESCRIPTION :
	//        Given an input array, split the input array into two
	//        arrays, one with all values in indicies less than
	//        the given index, and the other with all values
	//        greater than the given index
	// 
	// PARAMETERS :
	//        IDX : std::size_t
	//            The split index
	//
	//        LEN : std::size_t (Inferred)
	//            Length of the generated std::array
	// 
	//        T : typename (Inferred)
	//            Type of the input value and elements in
	//            the output std::array
	//
	// ARGUMENTS :
	//        IN : std::array<TA, LEN>
	//            Input array to split
	//
	// RETURNS : std::pair<std::array<T, (IDX > LEN)? LEN : IDX>,
	//	               std::array<T, (IDX > LEN)? 0 : LEN - IDX> >
	//	  Returns a pair of arrays, where the first array is
	//	  contains the values from all indicies less than the
	//	  split index, and the second array contains all other
	//	  values
	//
	// EXAMPLE :
	//             std::array<int, 10> a;
	//             std::array<int, 4> b;
	//             std::array<int, 6> c;
	//             {b, c} = split<4>(a);
	template<std::size_t IDX, typename T, std::size_t LEN>
	auto split(std::array<T, LEN> const& IN) ->
		std::pair<std::array<T, (IDX > LEN)? LEN : IDX>,
		          std::array<T, (IDX > LEN)? 0 : LEN - IDX> >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
		static const std::size_t llen = (IDX > LEN)? LEN : IDX;
		static const std::size_t rlen = (IDX > LEN)? 0 : LEN - IDX;
		std::array<T, llen> l;
#pragma HLS ARRAY_PARTITION complete VARIABLE=l._M_instance
		std::array<T, rlen> r;
#pragma HLS ARRAY_PARTITION complete VARIABLE=r._M_instance
	split_left_loop:
		for(int idx = 0; idx < llen; ++idx){
#pragma HLS UNROLL
			l[idx] = IN[idx];
		}
	split_right_loop:
		for(int ridx = llen, idx=0; idx < rlen; ++ridx, ++idx){
#pragma HLS UNROLL
			r[idx] = IN[ridx];
		}
		return {l, r};
	}

	// MODULE :
	//        struct Split
	//
	// DESCRIPTION :
	//        Function-wrapped-struct wrapping the split
	//        function (described above) using the operator()
	//        function.
	//
	// PARAMETERS :
	//        IDX : std::size_t
	//            The split index
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<std::size_t, T> const&) const
	template<typename std::size_t IDX>
	struct Split{
		// FUNCTION :
		//        operator() const
		// 
		// DESCRIPTION :
		//        Given an input array, split the input array
		//        into two arrays, one with all values in
		//        indicies less than the given index, and the
		//        other with all values greater than the given
		//        index
		// 
		// PARAMETERS :
		//        LEN : std::size_t (Inferred)
		//            Length of the generated std::array
		// 
		//        T : typename (Inferred)
		//            Type of the input value and elements in
		//            the output std::array
		//
		// ARGUMENTS :
		//        IN : std::array<TA, LEN>
		//            Input array to split
		//
		// RETURNS : std::pair<std::array<T, (IDX > LEN)? LEN : IDX>,
		//	               std::array<T, (IDX > LEN)? 0 : LEN - IDX> >
		//	  Returns a pair of arrays, where the first array is
		//	  contains the values from all indicies less than the
		//	  split index, and the second array contains all other
		//	  values
		//
		// EXAMPLE :
		//             std::array<int, 10> a;
		//             std::array<int, 4> b;
		//             std::array<int, 6> c;
		//             {b, c} = Split<4>()(a);
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		auto operator()(std::array<T, LEN> const& IN) const
			-> decltype(split<IDX, T, LEN>(IN)) {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			auto temp {split<IDX, T, LEN>(IN)};
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.first._M_instance 
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.second._M_instance
			return temp;
		}
	};

	// MODULE :
	//        struct Head
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the head
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Head{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Returns a the element at the first index in
		//        an array
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : T
		//        Returns the first element in the input array
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		T operator()(std::array<T, LEN> const& t) const{
#pragma HLS INLINE
			return t[0];
		}
	};
	// VARIABLE :
	//        head (Instance of Head)
	// 
	// DESCRIPTION :
	//        Instantion of the Head struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 3> a = {1, 2, 3};
	//            int b;
	//            b = head(a); // b === 1
	Head head;

	// MODULE :
	//        struct At
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the an
	//        indexing-like operation on arrays as the operator()
	//        function.
	// 
	// PARAMETERS :
	//        IDX : std::size_t
	//            The index in the input array to read and return
	//
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	//
	// EXAMPLE :
	//            std::array<int, 3> a = {1, 2, 3};
	//            int b;
	//            b = At<1>(a); // b === 2
	//
	//        OR
	//            std::array<9, std::array<9, T> > a;
	//            std::array<9, T> b;
	//            b = map(At<1>(), a);
	// 
	// NOTES:
	//        At is useful for getting the value at an index when
	//        you want to specify the index through a Template
	//        parameter. The second example above demonstrates how
	//        At can be used by map to get the value at index 1 of
	//        a 2-d array.
	template <std::size_t IDX>
	struct At{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Given an array, returns a value at a
		//        specific static index in the array.
		// 
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : T
		//        The value at the specified index
		//
		// NOTES :
		//        No bounds checking is performed. When STOP > START,
		//        the compiler will fail.
		//
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		T operator()(std::array<T, LEN> const& IN) const{
#pragma HLS INLINE
			return IN[IDX];
		}
	};

	// MODULE :
	//        struct Last
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the last
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Last{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Returns a the element at the last index in
		//        an array
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : T
		//        Returns the last element in the input array
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		T operator()(std::array<T, LEN> const& t){
#pragma HLS INLINE
			return t[LEN-1];
		}
	};
	// VARIABLE :
	//        last (Instance of Last)
	// 
	// DESCRIPTION :
	//        Instantion of the Last struct that provides a
	//        function-like API using the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 3> a = {1, 2, 3};
	//            int b;
	//            b = last(a); // b === 3
	Last last;

	// MODULE :
	//        struct Tail
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the tail
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Tail{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Given an N-element array, return an N-1 element
		//        array with all non-head elements
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : T
		//        Returns an array with all elements except
		//        the first element.
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T, std::size_t LEN>
		auto operator()(std::array<T, LEN> const& IN) const
			-> std::array<T, LEN-1> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			std::array<T, LEN-1> res;
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
		tail_loop:
			for(int idx = 0; idx < LEN-1; ++idx){
#pragma HLS UNROLL
				res[idx] = IN[idx+1];
			}
			return res;
		}
	};
	// VARIABLE :
	//        tail (Instance of Tail)
	// 
	// DESCRIPTION :
	//        Instantion of the Tail struct that provides a
	//        function-like API using the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 3> a = {1, 2, 3};
	//            std::array<int, 2> b;
	//            b = tail(a); // b === {2, 3}
	Tail tail;

	// MODULE :
	//        struct Rrotate
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the rrotate
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Rrotate{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION : 
		//        Given an array return a new array with the
		//        last element moved to the front, a
		//        right-rotation.
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : std::array<T, LEN>
		//        Returns an array with the last element
		//        moved to the front and all other elements
		//        moved to the next-greater index.
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename T, std::size_t LEN> 
		auto operator()(std::array<T, LEN> const& IN) const
			-> std::array<T, LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return uncurry(flip(concatenate))(split<LEN-1>(IN));
		}
	};
	// VARIABLE :
	//        rrotate (Instance of Rrotate)
	// 
	// DESCRIPTION :
	//        Instantion of the Rrotate struct that provides a
	//        function-like API using the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 3> a = {1, 2, 3}, b;
	//            b = rrotate(a); // b === {3, 1, 2}
	Rrotate rrotate;

	// MODULE :
	//        struct Rshift
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the rshift
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Rshift{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION : 
		//        Given a value and an array, return an array
		//        with the the value at the first index, and
		//        filled with all the remaining values in the
		//        array except the last
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        V : T const&
		//            Input value to "shift" into the first
		//            index of the array
		//
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : std::array<T, LEN>
		//        Returns an array with the first argument at
		//        the first index, and filled with the
		//        all-except-the-last values in the input
		//        array
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename T, std::size_t LEN> 
		auto operator()(T const& V, std::array<T, LEN> const& IN) const
			-> std::array<T, LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return prepend(V, split<LEN-1>(IN).first);
		}
	};
	// VARIABLE :
	//        Rshift (Instance of Rshift)
	// 
	// DESCRIPTION :
	//        Instantion of the Rshift struct that provides a
	//        function-like API using the operator() function.
	//
	// EXAMPLE :
	//            int a = 0;
	//            std::array<int, 3> b = {1, 2, 3}, c;
	//            c = rshift(a, b); // c === {0, 1, 2}
	Rshift rshift;

	// MODULE :
	//        struct Lshift
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the lshift
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Lshift{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION : 
		//        Given an array and a value, return an array
		//        where the tail of the input array precedes
		//        the value at the last index.
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		//        V : T const&
		//            Input value to "shift" into the last
		//            index of the array
		//
		// RETURNS : std::array<T, LEN>
		//        Returns an array where the tail of the input
		//        array precedes the value at the last index.
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename T, std::size_t LEN> 
		auto operator()(std::array<T, LEN> const& IN, T const& V) const
			-> std::array<T, LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance			
#pragma HLS INLINE
			return append(tail(IN), V);
		}
	};
	// VARIABLE :
	//        Lshift (Instance of Lshift)
	// 
	// DESCRIPTION :
	//        Instantion of the Lshift struct that provides a
	//        function-like API using the operator() function.
	//
	// EXAMPLE :
	//            int a = 4;
	//            std::array<int, 3> b = {1, 2, 3}, c;
	//            c = lshift(b, a); // c === {2, 3, 4}
	Lshift lshift;

	// MODULE :
	//        struct Lrotate
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the lrotate
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, LEN> const&) const
	struct Lrotate{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION : 
		//        Given an array return a new array with the
		//        first element moved to the end, and all
		//        subsequent elements moved to the next-lesser
		//        index, a left-rotation.
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the elements in the input
		//            std::array
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<T, LEN> const&
		//            The input array
		//
		// RETURNS : std::array<T, LEN>
		//        Returns an array with the first element
		//        moved to the last and all other elements
		//        moved to the next-lesser index.
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename T, std::size_t LEN> 
		auto operator()(std::array<T, LEN> const& IN) const
			-> std::array<T, LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return uncurry(flip(concatenate))(split<1>(IN));
		}
	};
	// VARIABLE :
	//        lrotate (Instance of Lrotate)
	// 
	// DESCRIPTION :
	//        Instantion of the Lrotate struct that provides a
	//        function-like API using the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 3> a = {1, 2, 3}, b;
	//            b = lrotate(a); // b === {2, 3, 1}
	Lrotate lrotate;

	// MODULE :
	//        struct Construct
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the lrotate
	//        function on arrays as the operator() function.
	// 
	// FUNCTIONS :
	//        auto operator()(T const&, T const&) const
	//        auto operator()(T const&, T const&,
	//                        T const&) const
	//        auto operator()(T const&, T const&,
	//                        T const&, T const&) const
	//        auto operator()(T const&, T const&, T const&,
	//                        T const&, T const&) const
	struct Construct{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Construct a two-element array from two
		//        values;
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the input values
		//
		// ARGUMENTS :
		//        A : T
		//            The first input value
		//
		//        B : T
		//            The second input value
		// 
		// RETURNS : std::array<T, 2>
		//        Returns an array with the first and second
		//        argument in the first and second index.
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T>
		auto operator()(T const& A, T const& B) const
			-> std::array<T, 2> {
#pragma HLS INLINE
			std::array<T, 2> res = {A, B};
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
			return res;
		}

		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Construct a three-element array from three
		//        values
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the input values
		//
		// ARGUMENTS :
		//        A : T
		//            The first input value
		//
		//        B : T
		//            The second input value
		// 
		//        C : T
		//            The third input value
		// 
		// RETURNS : std::array<T, 3>
		//        Returns an array inputs arguments at
		//        corresponding locations in the array;
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T>
		auto operator()(T const& A, T const& B, T const& C) const
			-> std::array<T, 3> {
#pragma HLS INLINE
			std::array<T, 3> res = {A, B, C};
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
			return res;
		}

		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Construct a four-element array from four
		//        values
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the input values
		//
		// ARGUMENTS :
		//        A : T
		//            The first input value
		//
		//        B : T
		//            The second input value
		// 
		//        C : T
		//            The third input value
		// 
		//        D : T
		//            The fourth input value
		// 
		// RETURNS : std::array<T, 4>
		//        Returns an array inputs arguments at
		//        corresponding locations in the array;
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T>
		auto operator()(T const& A, T const& B,
				T const& C, T const& D) const
			-> std::array<T, 4> {
#pragma HLS INLINE
			std::array<T, 4> res = {A, B, C, D};
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
			return res;
		}

		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Construct a five-element array from five
		//        values
		//
		// PARAMETERS :
		//        T : typename (Inferred)
		//            Type of the input values
		//
		// ARGUMENTS :
		//        A : T
		//            The first input value
		//
		//        B : T
		//            The second input value
		// 
		//        C : T
		//            The third input value
		// 
		//        D : T
		//            The fourth input value
		// 
		//        e : T
		//            The fifth input value
		// 
		// RETURNS : std::array<T, 5>
		//        Returns an array inputs arguments at
		//        corresponding locations in the array;
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename T>
		auto operator()(T const& A, T const& B,
				T const& C, T const& D,
				T const& E) const
			-> std::array<T, 5> {
#pragma HLS INLINE
			std::array<T, 5> res = {A, B, C, D, E};
#pragma HLS ARRAY_PARTITION complete VARIABLE=res._M_instance
			return res;
		}
	};
	// VARIABLE :
	//        construct (Instance of Construct)
	// 
	// DESCRIPTION :
	//        Instantion of the Construct struct that provides a
	//        function-like API using the operator() function.
	//
	// EXAMPLE :
	//            std::array<int, 2> a;
	//            a = construct(1, 2); // a === {1, 2}
	Construct construct;
} // namespace hops
#endif // __ARRAYOPS_HPP


