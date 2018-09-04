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
//        divconq.hpp
//
// DESCRIPTION :
//        This file implements the divconq Higher-Order Function that
//        is useful for implementing explicit recursive tree
//        algorithms.
//       
// NAMESPACES :
//        hops
//
// PUBLIC MODULES :
//        Divconq
// 
// PRIVATE MODULES :
//        DivconqHelper
//
// PUBLIC VARIABLES :
//        divconq (an instance of Divconq)
//
// NOTES :
//        This file requires a compiler implementing the C++11 standard.
//
// AUTHOR : Dustin Richmond (drichmond)
//
// **********************************************************************
#ifndef __DIVCONQ_HPP
#define __DIVCONQ_HPP
#include <array>
#include "arrayops.hpp"
namespace hops{
	// MODULE :
	//        struct DivconqHelper
	// 
	// DESCRIPTION :
	//        Implements the Divide-and-Conquer recurisve
	//        structure using C++ Template Meta-Programming and
	//        Partial Template Specialization
	// 
	// PARAMETERS :
	//        LEV : std::size_t
	//            Iteration variable for C++ Template
	//            Meta-Programming.
	// 
	// FUNCTIONS :
	//        auto operator()(std::array<T, (1<<LEV)>) const
	//
	// NOTES :
	//        The operator() function can only operate on
	//        power-of-two-length arrays
	template <std::size_t LEV>
	struct DivconqHelper{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Implements the Divide-and-Conquer function
		//        application using the recursive
		//        structure defined by DivconqHelper.
		// 
		// PARAMETERS :
		//        FN : class
		//            Typename of a class-wrapped function. 
		// 
		//        T : typename
		//            Type of the std::array values in the
		//            input array to the operator() function.
		//
		// ARGUMENTS :
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function to apply to the input array.
		//
		//        IN : std::array<T, (1<<LEV)> const&
		//            The input array to apply the
		//            Divide-and-Conquer function on.
		// 
		// RETURNS : auto
		//        The return type is determined from the
		//        return type of the class-wrapped function FN
		//        that parameterizes DivconqHelper.
		//
		// NOTES :
		//        The operator() function can only operate on
		//        power-of-two-length arrays
		//
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <class FN, typename T>
		auto operator()(FN const& F, std::array<T, (1<<LEV)> const& IN) const
			-> decltype(F(DivconqHelper<LEV-1>()
					(F, std::array<T, (1<<(LEV-1))>()),
					DivconqHelper<LEV-1>()
					(F, std::array<T, (1<<(LEV-1))>()))) {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE

			auto p = split<(1<<(LEV-1))>(IN);
			auto dh = DivconqHelper<LEV-1>();
			return F(dh(F, p.first), dh(F, p.second));
		}
		// TODO: In order to handle non-power-of-two-length
		// arrays, we need overloading (and there's a bug in
		// GCC)
	};

	// MODULE :
	//        struct DivconqHelper
	// 
	// DESCRIPTION :
	//        Implements the base-case (LEV = 1)
	//        Divide-and-Conquer recurisve structure using C++
	//        Template Meta-Programming and Partial Template
	//        Specialization
	// 
	// FUNCTIONS :
	//        auto divconq(std::array<T, 2>)
	//
	// NOTES :
	//        The divconq function can only operate on two-element
	//        arrays
	template<>
	struct DivconqHelper<1>{
		// FUNCTION :
		//        divconq
		// 
		// DESCRIPTION :
		//        Implements the Divide-and-Conquer function
		//        application from within the recursive
		//        structure defined by DivconqHelper.
		// 
        	// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
        	// 
        	//        T : typename (Inferred)
        	//            Type of the std::array values in the input array.
        	//
		// ARGUMENTS :
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function to apply to the input array.
		//
		//        IN : std::array<T, 2> const&
		//            The input array to apply the
		//            Divide-and-Conquer function on.
		// 
		// RETURNS : auto
		//        The return type is determined from the
		//        return type of the class-wrapped function FN
		//        that parameterizes DivconqHelper.
		//
		// NOTES :
		//        The divconq function can only operate on
		//        two-element arrays
		//
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <class FN, typename T>
		auto operator()(FN const& F, std::array<T, 2> const& IN) ->
			decltype(F(std::array<T, 1>(), std::array<T, 1>())){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			auto p = split<1>(IN);
			return F(p.first, p.second);
		}
		// TODO: In order to handle non-power-of-2-length
		// arrays, we need overloading (and there's a bug in
		// GCC)
	};

	// MODULE :
	//        struct Divconq
	// 
	// DESCRIPTION :
	//        Wraps the Divide-and-Conquer recursive structure
	//        provided by DivconqHelper with the () operator to
	//        provide a function-like API.
	// 
	// FUNCTIONS :
	//        auto operator()(FN const& F, std::array<T, LEN>) const
	//
	// NOTES :
	//        The operator function can only operate on two-element
	//        arrays
	struct Divconq{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Provides a function-like interface to the
		//        recurisve Divide-and-Conquer structure
		//        defined by DivconqHelper.
		// 
        	// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a class-wrapped function. 
        	// 
        	//        T : typename (Inferred)
        	//            Type of the std::array values in the input array.
        	//
        	//        LEN : std::size_t (Inferred)
        	//            Length of the std::array input argument
        	// 
		// ARGUMENTS :
		//        F : FN const&
		//            Instantiation of the class-wrapped
		//            function to apply to the input array.
		// 
		//        IN : std::array<T, LEN> const&
		//            Input array to apply the
		//            Divide-and-Conquer pattern to.
		//
		// RETURNS : auto
		//        The return type is determined from the
		//        return type of the class-wrapped function
		//        parameter FN.
		//
		// NOTES :
		//        The operator() function can only operate on
		//        power-of-two-legnth arrays
		//
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <class FN, typename T, std::size_t LEN>
		auto operator()(FN const& F, std::array<T, LEN> const& IN) const
			-> decltype(DivconqHelper<clog2(LEN)>()(F, IN)){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return DivconqHelper<clog2(LEN)>()(F, IN);
		}
	};

	// VARIABLE :
	//        divconq (Instance of Divconq)
	// 
	// DESCRIPTION :
	//        Instantion of the Divconq struct that provides a
	//        function-like API with the operator() function.
	// 
	// EXAMPLE:
	//            std::array<int, 2> a = {1, 2, 3, 4}, b;
	//            b = divconq(concatenate, a); // b = {1, 2, 3, 4}
	//
	// NOTES :
	//        The operator() function can only operate on
	//        power-of-two-legnth arrays
	Divconq divconq;
} // namespace hops
#endif // __DIVCONQ_HPP
