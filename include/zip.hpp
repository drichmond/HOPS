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
//        zip.hpp
//
// DESCRIPTION :
//        This file implements the zip and unzip array manipulation
//        functions, as well as the zipWith Higher-Order Function.
//
// NAMESPACES :
//        hops
//        hops::loop
//        
// PUBLIC MODULES :
//        Zip
//        Unzip
//        ZipWith
// 
// PRIVATE MODULES :
//        XipHelper
//
// PUBLIC VARIABLES :
//        zip (an instance of Zip)
//        unzip (an instance of Unzip)
//        zipWith (an instance of ZipWith)
//
// NOTES :
//        This file requires a compiler implementing the C++11
//        standard.
//        
//        All classes and variables are defined in both the hops and
//        hops::loop namespaces
//
// AUTHOR : Dustin Richmond (drichmond)
//
// **********************************************************************
#ifndef __ZIP_HPP
#define __ZIP_HPP
#include <array>
#include <utility>
#include "arrayops.hpp"
namespace hops{
	// MODULE :
	//        struct XipHelper
	// 
	// DESCRIPTION :
	//        Implements the linear, loop-like structure using C++
	//        Template Meta-Programming and Partial Template
	//        Specialization.
	// 
	// PARAMETERS :
	//        LEN : std::size_t
	//            The length of the input list, and the iteration
	//            variable for C++ Template Meta-Programming
	// 
	// FUNCTIONS :
	//        auto zip(std::array<TL, LEN> const&,
	//                 std::array<TR, LEN> const&) const
	//        auto unzip(std::array<pair<TL, TR>, LEN> const&) const
	//        auto zipWith(std::array<TA, LEN> const&,
	//                     std::array<TB, LEN> const&) const
	//        auto zipWith(std::array<TA, LEN> const&,
	//                     std::array<TB, LEN> const&,
	//                     std::array<TC, LEN> const&) const
	//        auto zipWith(std::array<TA, LEN> const&,
	//                     std::array<TB, LEN> const&,
	//                     std::array<TC, LEN> const&,
	//                     std::array<TD, LEN> const&) const
	//        auto zipWith(std::array<TA, LEN> const&,
	//                     std::array<TB, LEN> const&,
	//                     std::array<TC, LEN> const&,
	//                     std::array<TD, LEN> const&,
	//                     std::array<TE, LEN> const&) const
	template <std::size_t LEN>
	struct XipHelper{
		// FUNCTION :
		//        zip
		// 
		// DESCRIPTION :
		//        Implements the zip function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TR : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		// ARGUMENTS :
		//        L : std::array<TL, LEN> const&
		//            Input array whose elements will be in
		//            the first position in the returned array
		//            of pairs.
		// 
		//        R : std::array<TR, LEN> const&
		//            Input array whose elements will be in
		//            the second position in the returned
		//            array of pairs.
		// 
		// RETURNS : std::array<std::pair<TL, TR>, LEN > 
		//        An array of pairs, where the first and
		//        second elements of each pair correspond to
		//        the elements at that location in the first
		//        and second array arguments
		// 
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename TL, typename TR>
		auto zip(std::array<TL, LEN> const& L, std::array<TR, LEN> const& R) const
			-> std::array<std::pair<TL, TR>, LEN >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
			return prepend({L[0], R[0]}, XipHelper<LEN-1>().zip(tail(L), tail(R)));
		}

		// FUNCTION :
		//        unzip
		// 
		// DESCRIPTION :
		//        Implements the unzip function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Type of the the element in the first
		//            position of each pair
		//
		//        TR : typename (Inferred)
		//            Type of the the element in the second
		//            position of each pair
		//
		// ARGUMENTS :
		//        IN : std::array<std::pair<TL, TR>, LEN> const&
		//            Input array of pairs whose elements will
		//            be split and returned at corresponding
		//            positions in the output arrays
		// 
		// RETURNS : std::pair<std::array<TL, LEN>,
		//                     std::array<TR, LEN> >
		//        A pair of arrays where the elements of the
		//        first and second arrays are the elements at
		//        the first and second position at the 
		//        corresponding position of the input array
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename TL, typename TR>
		auto unzip(std::array<std::pair<TL, TR>, LEN> const& IN) const
			-> std::pair<std::array<TL, LEN>, std::array<TR, LEN> >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			std::pair<std::array<TL, LEN - 1>, std::array<TR, LEN - 1> > t =
				XipHelper<LEN - 1>().unzip(tail(IN));
#pragma HLS ARRAY_PARTITION complete VARIABLE=t.first._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=t.second._M_instance
			return {prepend(IN[0].first, t.first), prepend(IN[0].second, t.second)};
		}

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<TB, LEN> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0])), LEN>
		//        An array where the output at each location
		//        is the result of F's operator() function applied to
		//        values at corresponding locations in the
		//        input arrays
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB>
		auto zipWith(FN const& F, std::array<TA, LEN> const& A,
			std::array<TB, LEN> const& B) const
			-> std::array<decltype(F(A[0], B[0])), LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS INLINE
			return prepend(F(A[0], B[0]),
				XipHelper<LEN-1>().zipWith(F, tail(A), tail(B)));
		}

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		//        TC : typename (Inferred)
		//            Type of the the std::array values in the
		//            third argument
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<TB, LEN> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		//        C : std::array<TC, LEN> const&
		//            Input array whose elements will be
		//            passed as the third argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0], C[0])), LEN>
		//        An array where the output at each location
		//        is the result of F's operator() function applied to
		//        values at corresponding locations in the
		//        input arrays
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB, typename TC>
		auto zipWith(FN const& F, std::array<TA, LEN> const& A,
			std::array<TB, LEN> const& B,
			std::array<TC, LEN> const& C) const
			-> std::array<decltype(F(A[0], B[0], C[0])), LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS INLINE
			return prepend(F(A[0], B[0], C[0]),
				XipHelper<LEN-1>().zipWith(F, tail(A), tail(B),
							tail(C)));
		}

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		//        TC : typename (Inferred)
		//            Type of the the std::array values in the
		//            third argument
		//
		//        TD : typename (Inferred)
		//            Type of the the std::array values in the
		//            fourth argument
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<TB, LEN> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		//        C : std::array<TC, LEN> const&
		//            Input array whose elements will be
		//            passed as the third argument to F's 
		//            operator() function
		// 
		//        D : std::array<TD, LEN> const&
		//            Input array whose elements will be
		//            passed as the fourth argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0], C[0], D[0])), LEN>
		//        An array where the output at each location
		//        is the result of F's operator() function applied to
		//        values at corresponding locations in the
		//        input arrays
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB, typename TC,
			 typename TD>
		auto zipWith(FN const& F, std::array<TA, LEN> const& A,
			std::array<TB, LEN> const& B,
			std::array<TC, LEN> const& C,
			std::array<TD, LEN> const& D) const
			-> std::array<decltype(F(A[0], B[0], C[0], D[0])), LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS INLINE
			return prepend(F(A[0], B[0], C[0], D[0]),
				XipHelper<LEN-1>().zipWith(F, tail(A), tail(B),
							tail(C), tail(D)));
		}

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		//        TC : typename (Inferred)
		//            Type of the the std::array values in the
		//            third argument
		//
		//        TD : typename (Inferred)
		//            Type of the the std::array values in the
		//            fourth argument
		//
		//        TE : typename (Inferred)
		//            Type of the the std::array values in the
		//            fifth argument
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<TB, LEN> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		//        C : std::array<TC, LEN> const&
		//            Input array whose elements will be
		//            passed as the third argument to F's 
		//            operator() function
		// 
		//        D : std::array<TD, LEN> const&
		//            Input array whose elements will be
		//            passed as the fourth argument to F's 
		//            operator() function
		// 
		//        E : std::array<TE, LEN> const&
		//            Input array whose elements will be
		//            passed as the fifth argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), LEN>
		//        An array where the output at each location
		//        is the result of F's operator() function applied to
		//        values at corresponding locations in the
		//        input arrays
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB, typename TC,
			 typename TD, typename TE>
		auto zipWith(FN const& F, std::array<TA, LEN> const& A,
			std::array<TB, LEN> const& B,
			std::array<TC, LEN> const& C,
			std::array<TD, LEN> const& D,
			std::array<TE, LEN> const& E) const
			-> std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=E._M_instance
#pragma HLS INLINE
			return prepend(F(A[0], B[0], C[0], D[0], E[0]),
				XipHelper<LEN-1>().zipWith(F, tail(A), tail(B),
							tail(C), tail(D), tail(E)));
		}
	};

	// MODULE :
	//        struct XipHelper
	// 
	// DESCRIPTION :
	//        Base-case implementation of XipHelper for a
	//        loop-like structure using C++ Template
	//        Meta-Programming and Partial Template
	//        Specialization.
	// 
	// FUNCTIONS :
	//        auto zip(std::array<TL, 0> const&,
	//                 std::array<TR, 0> const&) const
	//        auto unzip(std::array<pair<TL, TR>, 0> const&) const
	//        auto zipWith(std::array<TL, 0> const&,
	//                     std::array<TR, 0> const&) const
	//        auto zipWith(std::array<TL, 0> const&,
	//                     std::array<TR, 0> const&,
	//                     std::array<TR, 0> const&) const
	//        auto zipWith(std::array<TL, 0> const&,
	//                     std::array<TR, 0> const&,
	//                     std::array<TR, 0> const&,
	//                     std::array<TR, 0> const&) const
	//        auto zipWith(std::array<TL, 0> const&,
	//                     std::array<TR, 0> const&,
	//                     std::array<TR, 0> const&,
	//                     std::array<TR, 0> const&,
	//                     std::array<TR, 0> const&) const
	template <>
	struct XipHelper<0>{
		// FUNCTION :
		//        zip
		// 
		// DESCRIPTION :
		//        Base-case definition of the recursive
		//        implementation of the zip function using C++
		//        Template Metaprogramming 
		// 
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TR : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		// ARGUMENTS :
		//        L : std::array<TL, 0> const&
		//            Input array whose elements will be in
		//            the first position in the returned array
		//            of pairs.
		// 
		//        R : std::array<TR, 0> const&
		//            Input array whose elements will be in
		//            the second position in the returned
		//            array of pairs.
		// 
		// RETURNS : std::array<std::pair<TL, TR>, 0 >
		//        A zero-length array of pairs, typed to make
		//        the compiler happy
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename TL, typename TR>
		auto zip(std::array<TL, 0> const& L, std::array<TR, 0> const& R) const
			-> std::array<std::pair<TL, TR>, 0>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
			std::array<std::pair<TL, TR>, 0> nil;
#pragma HLS ARRAY_PARTITION complete VARIABLE=nil._M_instance
			return nil;
		}

		// FUNCTION :
		//        unzip
		// 
		// DESCRIPTION :
		//        Base-case definition of the recursive
		//        implementation of the unzip function using C++
		//        Template Metaprogramming 
		// 
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Type of the the element in the first
		//            position of each pair
		//
		//        TR : typename (Inferred)
		//            Type of the the element in the second
		//            position of each pair
		//
		// ARGUMENTS :
		//        IN : std::array<std::pair<TL, TR>, 0> const&
		//            Empty input array of pairs whose
		//            elements will be split and returned at
		//            corresponding positions in the output
		//            arrays
		// 
		// RETURNS : std::pair<std::array<TL, 0>,
		//                     std::array<TR, 0> >
		//        A pair of empty arrays where the elements of
		//        the first and second arrays are the elements
		//        at the first and second position at the
		//        corresponding position of the input array
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename TL, typename TR>
		auto unzip(std::array<std::pair<TL, TR>, 0> const& IN) const
			-> std::pair<std::array<TL, 0>, std::array<TR, 0> >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			std::array<TL, 0> lnil;
			std::array<TR, 0> rnil;
#pragma HLS ARRAY_PARTITION complete VARIABLE=lnil._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=rnil._M_instance
			return {lnil, rnil};
		}

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function using C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		// ARGUMENTS : 
		//        F : class FN
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<T, 0> const&
		//            Nill array with Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<T, 0> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0])), 0>
		//        A zero-length (nil) array with the element
		//        type determined by applying F's operator()
		//        function to hypothetical values at A[0] and
		//        B[0]
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB>
		auto zipWith(FN const& F,
			std::array<TA, 0> const& A,
			std::array<TB, 0> const& B)
			-> std::array<decltype(F(A[0], B[0])), 0> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS INLINE
			std::array<decltype(F(A[0], B[0])), 0> nil;
#pragma HLS ARRAY_PARTITION complete VARIABLE=nil._M_instance
			return nil;
		}	  

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function using C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		//        TC : typename (Inferred)
		//            Type of the the std::array values in the
		//            third argument
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<TA, 0> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<TB, 0> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		//        C : std::array<TC, 0> const&
		//            Input array whose elements will be
		//            passed as the third argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0], C[0], D[0])), 0>
		//        A zero-length (nil) array with the element
		//        type determined by applying F's operator()
		//        function to hypothetical values at A[0], 
		//        B[0], and C[0]
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB, typename TC>
		auto zipWith(FN const& F,
			std::array<TA, 0> const& A,
			std::array<TB, 0> const& B,
			std::array<TC, 0> const& C)
			-> std::array<decltype(F(A[0], B[0], C[0])), 0> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS INLINE
			std::array<decltype(F(A[0], B[0], C[0])), 0> nil;
#pragma HLS ARRAY_PARTITION complete VARIABLE=nil._M_instance
			return nil;
		}

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function using C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		//        TC : typename (Inferred)
		//            Type of the the std::array values in the
		//            third argument
		//
		//        TD : typename (Inferred)
		//            Type of the the std::array values in the
		//            fourth argument
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<TA, 0> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<TB, 0> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		//        C : std::array<TC, 0> const&
		//            Input array whose elements will be
		//            passed as the third argument to F's 
		//            operator() function
		// 
		//        D : std::array<TD, 0> const&
		//            Input array whose elements will be
		//            passed as the fourth argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0], C[0], D[0])), 0>
		//        A zero-length (nil) array with the element
		//        type determined by applying F's operator()
		//        function to hypothetical values at A[0], 
		//        B[0], C[0], and D[0]
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB, typename TC, typename TD>
		auto zipWith(FN const& F,
			std::array<TA, 0> const& A,
			std::array<TB, 0> const& B,
			std::array<TC, 0> const& C,
			std::array<TD, 0> const& D)
			-> std::array<decltype(F(A[0], B[0], C[0], D[0])), 0> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS INLINE
			std::array<decltype(F(A[0], B[0], C[0], D[0])), 0> nil;
#pragma HLS ARRAY_PARTITION complete VARIABLE=nil._M_instance
			return nil;
		}

		// FUNCTION :
		//        zipWith
		// 
		// DESCRIPTION :
		//        Implements the zipWith function using C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
		//        TC : typename (Inferred)
		//            Type of the the std::array values in the
		//            third argument
		//
		//        TD : typename (Inferred)
		//            Type of the the std::array values in the
		//            fourth argument
		//
		//        TE : typename (Inferred)
		//            Type of the the std::array values in the
		//            fifth argument
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<TA, 0> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		//        B : std::array<TB, 0> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		//        C : std::array<TC, 0> const&
		//            Input array whose elements will be
		//            passed as the third argument to F's 
		//            operator() function
		// 
		//        D : std::array<TD, 0> const&
		//            Input array whose elements will be
		//            passed as the fourth argument to F's 
		//            operator() function
		// 
		//        E : std::array<TE, 0> const&
		//            Input array whose elements will be
		//            passed as the fifth argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), 0>
		//        A zero-length (nil) array with the element
		//        type determined by applying F's operator()
		//        function to hypothetical values at A[0], 
		//        B[0], C[0], D[0], and E[0]
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB, typename TC, typename TD, typename TE>
		auto zipWith(FN const& F,
			std::array<TA, 0> const& A,
			std::array<TB, 0> const& B,
			std::array<TC, 0> const& C,
			std::array<TD, 0> const& D,
			std::array<TE, 0> const& E)
			-> std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), 0> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=E._M_instance
#pragma HLS INLINE
			std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), 0> nil;
#pragma HLS ARRAY_PARTITION complete VARIABLE=nil._M_instance
			return nil;
		}
	};

	// MODULE :
	//        struct Zip
	// 
	// DESCRIPTION :
	//        Wraps the zip function of the linear recursive
	//        structure implemented by XipHelper with the ()
	//        operator to provide a function-like API.
	// 
	// FUNCTIONS :
	//        std::array<std::pair<TL, TR>, LEN >
	//            operator()(std::array<TL, LEN> const&,
	//                       std::array<TR, LEN> const&)
	//
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	struct Zip{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Provides a function-like interface to the
		//        linear recurisve structure defined by the
		//        zip function of XipHelper
		// 
        	// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
        	// 
        	//        T : typename (Inferred)
        	//            Type of the std::array values in the input array.
        	//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input arguments
        	// 
		// ARGUMENTS :
		//        L : std::array<TL, LEN> const&
		//            Input array whose elements will be in
		//            the first position in the returned array
		//            of pairs.
		// 
		//        R : std::array<TR, LEN> const&
		//            Input array whose elements will be in
		//            the second position in the returned
		//            array of pairs.
		// 
		// RETURNS : std::array<pair<TL, TR>, LEN>
		//        An array of pairs, where the first and
		//        second elements of each pair correspond to
		//        the elements at that location in the first
		//        and second array arguments
		//
		// NOTES :
		//        The operator() function can only operate on
		//        power-of-two-legnth arrays
		//
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename TL, typename TR, std::size_t LEN>
		auto operator()(std::array<TL, LEN> const& L, std::array<TR, LEN> const& R)
			-> std::array<std::pair<TL, TR>, LEN >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
			return XipHelper<LEN>().zip(L, R);
		}
	};

	// MODULE :
	//        struct ZipWith
	// 
	// DESCRIPTION :
	//        Wraps the ZipWith function of the linear recursive
	//        structure implemented by XipHelper with the operator()
	//        function to provide a function-like API.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<TA, LEN> const&,
	//                        std::array<TB, LEN> const&) const
	//        auto operator()(std::array<TA, LEN> const&,
	//                        std::array<TB, LEN> const&,
	//                        std::array<TC, LEN> const&) const
	//        auto operator()(std::array<TA, LEN> const&,
	//                        std::array<TB, LEN> const&,
	//                        std::array<TC, LEN> const&,
	//                        std::array<TD, LEN> const&) const
	//        auto operator()(std::array<TA, LEN> const&,
	//                        std::array<TB, LEN> const&,
	//                        std::array<TC, LEN> const&,
	//                        std::array<TD, LEN> const&,
	//                        std::array<TE, LEN> const&) const
	//
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	struct ZipWith{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Provides a function-like interface to the
		//        linear recurisve structure defined by the
		//        zipWith function of XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
		//
		//        TA : typename (Inferred)
		//            Type of the the std::array values in the
		//            first argument
		//
		//        TB : typename (Inferred)
		//            Type of the the std::array values in the
		//            second argument
		//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input arguments
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function used to compute an output value
		//            at each array index
		//
		//        A : std::array<T, 0> const&
		//            Nil array with Input array whose
		//            elements will be passed as the first
		//            argument to F's operator() function
		// 
		//        B : std::array<T, 0> const&
		//            Input array whose elements will be
		//            passed as the second argument to F's 
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(A[0], B[0])), LEN>
		//        An array where the output at each location
		//        is the result of F's operator() function applied to
		//        values at corresponding locations in the
		//        input arrays
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA, typename TB, std::size_t LEN>
		auto operator()(FN const& F,
				std::array<TA, LEN> const& A,
				std::array<TB, LEN> const& B) const
			-> std::array<decltype(F(A[0], B[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS INLINE
			return XipHelper<LEN>().zipWith(F, A, B);
		}

		template<class FN, typename TA, typename TB, typename TC,
			 std::size_t LEN>
		auto operator()(FN const& F,
				std::array<TA, LEN> const& A,
				std::array<TB, LEN> const& B,
				std::array<TC, LEN> const& C) const
			-> std::array<decltype(F(A[0], B[0], C[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS INLINE
			return XipHelper<LEN>().zipWith(F, A, B, C);
		}
		template<class FN, typename TA, typename TB, typename TC,
			 typename TD, std::size_t LEN>
		auto operator()(FN const& F,
				std::array<TA, LEN> const& A,
				std::array<TB, LEN> const& B,
				std::array<TC, LEN> const& C,
				std::array<TD, LEN> const& D) const
			-> std::array<decltype(F(A[0], B[0], C[0], D[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS INLINE
			return XipHelper<LEN>().zipWith(F, A, B, C, D);
		}
		template<class FN, typename TA, typename TB, typename TC,
			 typename TD, typename TE, std::size_t LEN>
		auto operator()(FN const& F,
				std::array<TA, LEN> const& A,
				std::array<TB, LEN> const& B,
				std::array<TC, LEN> const& C,
				std::array<TD, LEN> const& D,
				std::array<TE, LEN> const& E)
			-> std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=E._M_instance
#pragma HLS INLINE
			return XipHelper<LEN>().zipWith(F, A, B, C, D, E);
		}
	};

	// MODULE :
	//        struct Unzip
	// 
	// DESCRIPTION :

	//        Wraps the Unzip function of the linear recursive structure
	//        implemented by XipHelper with the operator() function to
	//        provide a function-like API.
	// 
	// FUNCTIONS :
	//        std::pair<std::array<TL, LEN>, std::array<TR, LEN> >
	//            operator()(std::array<std::pair<TL, TR>, LEN> const&) const
	//
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	struct Unzip{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Implements the unzip function using C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Type of the the element in the first
		//            position of each pair
		//
		//        TR : typename (Inferred)
		//            Type of the the element in the second
		//            position of each pair
		// 
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
		//
		// ARGUMENTS :
		//        IN : std::array<std::pair<TL, TR>, LEN> const&
		//            Input array of pairs whose elements will
		//            be split and returned at corresponding
		//            positions in the output arrays
		// 
		// RETURNS : std::pair<std::array<TL, LEN>,
		//                     std::array<TR, LEN> >
		//        A pair of arrays where the elements of the
		//        first and second arrays are the elements at
		//        the first and second position at the 
		//        corresponding position of the input array
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<typename TL, typename TR, std::size_t LEN>
		auto operator()(std::array<std::pair<TL, TR>, LEN> const& IN) const
			-> std::pair<std::array<TL, LEN>, std::array<TR, LEN> >{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			auto temp = XipHelper<LEN>().unzip(IN);
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.first._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp.second._M_instance
			return temp;
		}
	};

	// VARIABLE :
	//        zip (instance of Zip)
	// 
	// DESCRIPTION :
	//        Instantion of the Zip struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//
	//            std::array<int, 2> a = {1, 2}, b = {3, 4};
	//            zip(a, b); // {{1, 3}, {2, 4}}
	//        
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	Zip zip;

	// VARIABLE :
	//        zipWith (instance of ZipWith)
	// 
	// DESCRIPTION :
	//        Instantion of the ZipWith struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//
	//            std::array<int, 2> a = {1, 2}, b = {3, 4}, c;
	//            c = zipWith(std::plus<int>(), a, b);
	//            // c = {4, 6}
	// 
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	ZipWith zipWith;

	// VARIABLE :
	//        unzip (instance of Unzip)
	// 
	// DESCRIPTION :
	//        Instantion of the Unzip struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//
	//        std::array<int, 2> a = {1, 2}, b = {3, 4};
	//        std::pair<std::array<int, 2>,
	//                  std::array<int, 2> > p = {a, b}
	//        unzip(p);
	//        
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	Unzip unzip;

	namespace loop {
		// MODULE :
		//        struct Unzip
		// 
		// DESCRIPTION :
		//        Provides a struct-wrapped, loop-based
		//        implementation of the unzip function
		// 
		// FUNCTIONS :
		//        std::pair<std::array<TL, LEN>, std::array<TR, LEN> >
		//            operator()(std::array<std::pair<TL, TR>, LEN> const&) const
		//
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops namespace
		struct Unzip{
			// FUNCTION :
			//        operator()
			// 
			// DESCRIPTION :
			//        Implements the unzip function using an
			//        unrolled for-loop 
			// 
			// PARAMETERS :
			//        TL : typename (Inferred)
			//            Type of the the element in the first
			//            position of each pair
			//
			//        TR : typename (Inferred)
			//            Type of the the element in the second
			//            position of each pair
			// 
			//        LEN : std::size_t (Inferred)
			//            Length of the std::array input argument
			//
			// ARGUMENTS :
			//        IN : std::array<std::pair<TL, TR>, LEN> const&
			//            Input array of pairs whose elements will
			//            be split and returned at corresponding
			//            positions in the output arrays
			// 
			// RETURNS : std::pair<std::array<TL, LEN>,
			//                     std::array<TR, LEN> >
			//        A pair of arrays where the elements of the
			//        first and second arrays are the elements at
			//        the first and second position at the 
			//        corresponding position of the input array
			//
			// NOTES :
			//        The trailing const declaration on the
			//        operator() function is required by our
			//        library, and indicates to the compiler that
			//        the method modifies no internal state
			template <typename TL, typename TR, std::size_t LEN>
			auto operator()(std::array<std::pair<TL, TR>, LEN> const& IN) const
				-> std::pair<std::array<TL, LEN>, std::array<TR, LEN> > {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
				std::array<TL, LEN> left, right;
#pragma HLS ARRAY_PARTITION complete VARIABLE=left._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=right._M_instance
#pragma HLS INLINE
			unzip_loop:
				for(int i = 0; i < LEN; ++i){
#pragma HLS UNROLL
					left[i] = IN[i].first;
					right[i] = IN[i].second;
				}
				return {left, right};
			}
		};

		// MODULE :
		//        struct Zip
		// 
		// DESCRIPTION :
		//        Provides a struct-wrapped, loop-based
		//        implementation of the zip function
		// 
		// FUNCTIONS :
		//        std::array<std::pair<TL, TR>, LEN >
		//            operator()(std::array<TL, LEN> const&,
		//                       std::array<TR, LEN> const&) const
		//
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops::loop namespace
		struct Zip{
			// FUNCTION :
			//        operator()
			// 
			// DESCRIPTION :
			//        Implements the zip function using an
			//        unrolled for-loop 
			// 
			// PARAMETERS :
			//        FN : class (Inferred)
			//            Typename of a class-wrapped function. 
			//
			//        TL : typename (Inferred)
			//            Type of the the std::array values in the
			//            first argument
			//
			//        TR : typename (Inferred)
			//            Type of the the std::array values in the
			//            second argument
			//
			//        LEN : std::size_t (Inferred)
			//            Length of the std::array input arguments
			//
			// ARGUMENTS : 
			//        L : std::array<T, 0> const&
			//            Nil array with Input array whose
			//            elements will be passed as the
			//            first argument to F's
			//            operator() function
			// 
			//        R : std::array<T, 0> const&
			//            Input array whose elements will be
			//            passed as the second argument to F's
			//            operator() function
			// 
			// RETURNS : std::array<decltype(F(L[0], R[0])), LEN>
			//        An array where the output at each location
			//        is the result of F's operator() function applied to
			//        values at corresponding locations in the
			//        input arrays
			//
			// NOTES :
			//        The trailing const declaration on the
			//        operator() function is required by our
			//        library, and indicates to the compiler that
			//        the method modifies no internal state
			template <typename TL, typename TR, std::size_t LEN>
			auto operator()(std::array<TL, LEN> const& L,
					std::array<TR, LEN> const& R) const
				-> std::array<std::pair<TL, TR>, LEN> {
#pragma HLS ARRAY_PARTITION complete VARIABLE=L._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=R._M_instance
#pragma HLS INLINE
				std::array<std::pair<TL, TR>, LEN> temp;
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
			zip_loop:
				for(int i = 0; i < LEN; ++i){
#pragma HLS UNROLL
					temp[i] = std::pair<TL, TR>(L[i], R[i]);//std::make_pair(L[i], R[i]);
				}
				return temp;
			}
		};


		// MODULE :
		//        struct ZipWith
		// 
		// DESCRIPTION :
		//        Provides a struct-wrapped, loop-based
		//        implementation of the zipWith function
		// 
		// FUNCTIONS :
		//        auto operator()(std::array<TL, LEN> const&,
		//                        std::array<TR, LEN> const&) const
		//
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops namespace
		struct ZipWith{
			// FUNCTION :
			//        operator()
			// 
			// DESCRIPTION :
			//        Implements the zipWith function using an
			//        unrolled for-loop 
			// 
			// PARAMETERS :
			//        FN : class (Inferred)
			//            Typename of a class-wrapped function. 
			//
			//        TA : typename (Inferred)
			//            Type of the the std::array values in the
			//            first argument
			//
			//        TB : typename (Inferred)
			//            Type of the the std::array values in the
			//            second argument
			//
			//        LEN : std::size_t (Inferred)
			//            Length of the std::array input arguments
			//
			// ARGUMENTS : 
			//        F : FN const&
			//            Instantiation of the class-wrapped
			//            function used to compute an output value
			//            at each array index
			//
			//        A : std::array<T, LEN> const&
			//            Nil array with Input array
			//            whose elements will be passed as
			//            the first argument to F's
			//            operator() function
			// 
			//        B : std::array<T, LEN> const&
			//            Input array whose elements will be
			//            passed as the second argument to F's
			//            operator() function
			// 
			// RETURNS : std::array<decltype(F(A[0], B[0])), LEN>
			//        An array where the output at each location
			//        is the result of F's operator() function applied to
			//        values at corresponding locations in the
			//        input arrays
			//
			// NOTES :
			//        The trailing const declaration on the
			//        operator() function is required by our
			//        library, and indicates to the compiler that
			//        the method modifies no internal state
			template <class FN, typename TA, typename TB, std::size_t LEN>
			auto operator()(FN const& F,
					std::array<TA, LEN> const& A,
					std::array<TB, LEN> const& B) const
				-> std::array<decltype(F(A[0], B[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS INLINE
				std::array<decltype(F(A[0], B[0])), LEN> temp;
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
			zip_loop:
				for(int i = 0; i < LEN; ++i){
#pragma HLS UNROLL
					temp[i] = F(A[i], B[i]);
				}
				return temp;
			}

			// FUNCTION :
			//        operator()
			// 
			// DESCRIPTION :
			//        Provides a function-like interface to the
			//        loop::ZipWith class
			// 
			// PARAMETERS :
			//        FN : class (Inferred)
			//            Typename of a class-wrapped function. 
			//
			//        TA : typename (Inferred)
			//            Type of the the std::array values in the
			//            first argument
			//
			//        TB : typename (Inferred)
			//            Type of the the std::array values in the
			//            second argument
			//
			//        TC : typename (Inferred)
			//            Type of the the std::array values in the
			//            third argument
			//
			//        LEN : std::size_t (Inferred)
			//            Length of the std::array input arguments
			//
			// ARGUMENTS : 
			//        F : FN const&
			//            Instantiation of the class-wrapped
			//            function used to compute an output value
			//            at each array index
			//
			//        A : std::array<T, LEN> const&
			//            Nil array with Input array whose
			//            elements will be passed as the first
			//            argument to F's operator() function
			// 
			//        B : std::array<T, LEN> const&
			//            Input array whose elements will be
			//            passed as the second argument to F's 
			//            operator() function
			// 
			//        C : std::array<TC, LEN> const&
			//            Input array whose elements will be
			//            passed as the third argument to F's 
			//            operator() function
			// 
			// RETURNS : std::array<decltype(F(A[0], B[0], C[0])), LEN>
			//        An array where the output at each location
			//        is the result of F's operator() function applied to
			//        values at corresponding locations in the
			//        input arrays
			//
			// NOTES :
			//        The trailing const declaration on the
			//        operator() function is required by our
			//        library, and indicates to the compiler that
			//        the method modifies no internal state
			template <class FN, typename TA, typename TB, 
				  typename TC, std::size_t LEN>
			auto operator()(FN const& F,
					std::array<TA, LEN> const& A,
					std::array<TB, LEN> const& B,
					std::array<TC, LEN> const& C) const
				-> std::array<decltype(F(A[0], B[0], C[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS INLINE
				std::array<decltype(F(A[0], B[0], C[0])), LEN> temp;
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
			zip_loop:
				for(int i = 0; i < LEN; ++i){
#pragma HLS UNROLL
					temp[i] = F(A[i], B[i], C[i]);
				}
				return temp;
			}

			// FUNCTION :
			//        operator()
			// 
			// DESCRIPTION :
			//        Provides a function-like interface to the
			//        loop::ZipWith class
			// 
			// PARAMETERS :
			//        FN : class (Inferred)
			//            Typename of a class-wrapped function. 
			//
			//        TA : typename (Inferred)
			//            Type of the the std::array values in the
			//            first argument
			//
			//        TB : typename (Inferred)
			//            Type of the the std::array values in the
			//            second argument
			//
			//        TC : typename (Inferred)
			//            Type of the the std::array values in the
			//            third argument
			//
			//        TD : typename (Inferred)
			//            Type of the the std::array values in the
			//            fourth argument
			//
			//        LEN : std::size_t (Inferred)
			//            Length of the std::array input arguments
			//
			// ARGUMENTS : 
			//        F : FN const&
			//            Instantiation of the class-wrapped
			//            function used to compute an output value
			//            at each array index
			//
			//        A : std::array<T, LEN> const&
			//            Nil array with Input array whose
			//            elements will be passed as the first
			//            argument to F's operator() function
			// 
			//        B : std::array<T, LEN> const&
			//            Input array whose elements will be
			//            passed as the second argument to F's 
			//            operator() function
			// 
			//        C : std::array<TC, LEN> const&
			//            Input array whose elements will be
			//            passed as the third argument to F's 
			//            operator() function
			// 
			//        D : std::array<TD, LEN> const&
			//            Input array whose elements will be
			//            passed as the fourth argument to F's 
			//            operator() function
			// 
			// RETURNS : std::array<decltype(F(A[0], B[0], C[0], D[0])), LEN>
			//        An array where the output at each location
			//        is the result of F's operator() function applied to
			//        values at corresponding locations in the
			//        input arrays
			//
			// NOTES :
			//        The trailing const declaration on the
			//        operator() function is required by our
			//        library, and indicates to the compiler that
			//        the method modifies no internal state
			template <class FN, typename TA, typename TB,
				  typename TC, typename TD, std::size_t LEN>
			auto operator()(FN const& F,
					std::array<TA, LEN> const& A,
					std::array<TB, LEN> const& B,
					std::array<TC, LEN> const& C,
					std::array<TD, LEN> const& D) const
				-> std::array<decltype(F(A[0], B[0], C[0], D[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS INLINE
				std::array<decltype(F(A[0], B[0], C[0], D[0])), LEN> temp;
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
			zip_loop:
				for(int i = 0; i < LEN; ++i){
#pragma HLS UNROLL
					temp[i] = F(A[i], B[i], C[i], D[i]);
				}
				return temp;
			}

			// FUNCTION :
			//        operator()
			// 
			// DESCRIPTION :
			//        Provides a function-like interface to the
			//        loop::ZipWith class
			// 
			// PARAMETERS :
			//        FN : class (Inferred)
			//            Typename of a class-wrapped function. 
			//
			//        TA : typename (Inferred)
			//            Type of the the std::array values in the
			//            first argument
			//
			//        TB : typename (Inferred)
			//            Type of the the std::array values in the
			//            second argument
			//
			//        TC : typename (Inferred)
			//            Type of the the std::array values in the
			//            third argument
			//
			//        TD : typename (Inferred)
			//            Type of the the std::array values in the
			//            fourth argument
			//
			//        TE : typename (Inferred)
			//            Type of the the std::array values in the
			//            fifth argument
			//
			//        LEN : std::size_t (Inferred)
			//            Length of the std::array input arguments
			//
			// ARGUMENTS : 
			//        F : FN const&
			//            Instantiation of the class-wrapped
			//            function used to compute an output value
			//            at each array index
			//
			//        A : std::array<T, LEN> const&
			//            Nil array with Input array whose
			//            elements will be passed as the first
			//            argument to F's operator() function
			// 
			//        B : std::array<T, LEN> const&
			//            Input array whose elements will be
			//            passed as the second argument to F's 
			//            operator() function
			// 
			//        C : std::array<TC, LEN> const&
			//            Input array whose elements will be
			//            passed as the third argument to F's 
			//            operator() function
			// 
			//        D : std::array<TD, LEN> const&
			//            Input array whose elements will be
			//            passed as the fourth argument to F's 
			//            operator() function
			// 
			//        E : std::array<TE, LEN> const&
			//            Input array whose elements will be
			//            passed as the fifth argument to F's 
			//            operator() function
			// 
			// RETURNS : std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), LEN>
			//        An array where the output at each location
			//        is the result of F's operator() function applied to
			//        values at corresponding locations in the
			//        input arrays
			//
			// NOTES :
			//        The trailing const declaration on the
			//        operator() function is required by our
			//        library, and indicates to the compiler that
			//        the method modifies no internal state
			template <class FN, typename TA, typename TB,
				  typename TC, typename TD, typename TE,
				  std::size_t LEN>
			auto operator()(FN const& F,
					std::array<TA, LEN> const& A,
					std::array<TB, LEN> const& B,
					std::array<TC, LEN> const& C,
					std::array<TD, LEN> const& D,
					std::array<TE, LEN> const& E) const
				-> std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=A._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=B._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=C._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=D._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=E._M_instance
#pragma HLS INLINE
				std::array<decltype(F(A[0], B[0], C[0], D[0], E[0])), LEN> temp;
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
			zip_loop:
				for(int i = 0; i < LEN; ++i){
#pragma HLS UNROLL
					temp[i] = F(A[i], B[i], C[i], D[i], E[i]);
				}
				return temp;
			}
		};

		// VARIABLE :
		//        zip (instance of Zip)
		// 
		// DESCRIPTION :
		//        Instantion of the Zip struct that provides a
		//        function-like API with the operator() function.
		//
		// EXAMPLE :
		//
		//        std::array<int, 2> a = {1, 2}, b = {3, 4};
		//        zip(a, b); // {{1, 3}, {2, 4}}
		// 
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops namespace
		Zip zip;

		// VARIABLE :
		//        zipWith (instance of ZipWith)
		// 
		// DESCRIPTION :
		//        Instantion of the ZipWith struct that provides a
		//        function-like API with the operator() function.
		//
		// EXAMPLE :
		//
		//        std::array<int, 2> a = {1, 2}, b = {3, 4}, c;
		//        c = zipWith(std::plus<int>(), a, b);
		//        // c = {4, 6}
		// 
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops namespace
		ZipWith zipWith;

		// VARIABLE :
		//        unzip (instance of Unzip)
		// 
		// DESCRIPTION :
		//        Instantion of the Unzip struct that provides a
		//        function-like API with the operator() function.
		//
		// EXAMPLE :
		//
		//        std::array<int, 2> a = {1, 2}, b = {3, 4};
		//        std::pair<std::array<int, 2>,
		//                  std::array<int, 2> > p = {a, b}
		//        unzip(p);
		// 
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops namespace
		Unzip unzip;
	} // namespace loop
} // namespace hops
#endif // __ZIP_HPP
