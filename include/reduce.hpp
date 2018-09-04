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
//        reduce.hpp
//
// DESCRIPTION :
//        This file implements the left and right reduce functions
//        lreduce and reduce using C++ Template Metaprogramming.
//       
// NAMESPACES :
//        hops
//
// PUBLIC MODULES :
//        Lreduce
//        Rreduce
//
// PRIVATE MODULES :
//        ReduceHelper
//
// PUBLIC VARIABLES :
//        lreduce
//        rreduce
//
// NOTES :
//        This file requires a compiler implementing the C++11
//        standard.
//
// AUTHOR : Dustin Richmond (drichmond)
//
// **********************************************************************
#ifndef __REDUCE_HPP
#define __REDUCE_HPP
#include <array>
#include "arrayops.hpp"
namespace hops{
	// MODULE :
	//        struct ReduceHelper
	// 
	// DESCRIPTION :
	//        Implements the linear, loop-like recursive structure
	//        of lreduce and rreduce functions using C++ Template
	//        Meta-Programming
	// 
	// PARAMETERS :
	//        LEN : std::size_t
	//            The length of the input list, and the iteration
	//            variable for C++ Template Meta-Programming
	// 
	// FUNCTIONS :
	//        auto lreduce(FN const&, TI const&, std::array<TA, LEN> const&)
	//        auto rreduce(FN const&, std::array<TA, LEN> const&, TI const&)
	template <std::size_t LEN>
	struct ReduceHelper{
		// FUNCTION :
		//        lreduce
		// 
		// DESCRIPTION :
		//        Implements the lreduce function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function
		//
		//        TI : typename (Inferred)
		//            Type of the the initalization value 
		//
		//        TA : typename (Inferred)
		//            Type of the the values in the std::array
		//            argument
		//
		// ARGUMENTS :
		//        F : FN const&
		//            Wrapped object, unused except in
		//            compiler inference to deduce the class
		//            name for FN
		// 
		//        INIT : TI const&
		//            Initialization value argument
		//
		//        IN : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            reduced
		// 
		// RETURNS : auto
		//            The return type is determined by the
		//            wrapped function FN
		template<class FN, typename TI, typename TA>
		auto lreduce(FN const& F, TI const& INIT, std::array<TA, LEN> const& IN) const 
			-> decltype(ReduceHelper<LEN-1>().lreduce(F, F(INIT, head(IN)), tail(IN))) {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return ReduceHelper<LEN-1>().lreduce(F, F(INIT, head(IN)), tail(IN));
		}

		// FUNCTION :
		//        rreduce
		// 
		// DESCRIPTION :
		//        Implements the rreduce function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function
		//
		//        TA : typename (Inferred)
		//            Type of the the values in the std::array
		//            argument
		//
		//        TI : typename (Inferred)
		//            Type of the the initalization value 
		//
		// ARGUMENTS :
		//        F : FN const&
		//            Wrapped object, unused except in
		//            compiler inference to deduce the class
		//            name for FN
		// 
		//        IN : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            reduced
		// 
		//        INIT : TI const&
		//            Initialization value argument
		//
		// RETURNS : auto
		//            The return type is determined by the
		//            wrapped function FN
		template<class FN, typename TA, typename TI>
		auto rreduce(FN const& F, std::array<TA, LEN> const& IN, TI const& INIT) const
			-> decltype(F(head(IN), ReduceHelper<LEN-1>().rreduce(F, tail(IN), INIT))) {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return F(head(IN), ReduceHelper<LEN-1>().rreduce(F, tail(IN), INIT));
		}

	};

	// MODULE :
	//        struct ReduceHelper
	// 
	// DESCRIPTION :
	//        Implements the base case of the loop-like recursive
	//        structure of lreduce and rreduce functions using C++
	//        Template Meta-Programming and specifically, template
	//        specialization.
	// 
	// FUNCTIONS :
	//        auto lreduce(FN, TI const&, std::array<TA, 0> const&)
	//        auto rreduce(FN, std::array<TA, 0> const&, TI const&)
	template <>
	struct ReduceHelper<0>{
		// FUNCTION :
		//        lreduce
		// 
		// DESCRIPTION :
		//        Implements the base case of the lreduce
		//        function with C++ Template Metaprogramming
		//        using the recursive structure defined by
		//        XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function
		//
		//        TI : typename (Inferred)
		//            Type of the the initalization value 
		//
		//        TA : typename (Inferred)
		//            Type of the the values in the std::array
		//            argument
		//
		// ARGUMENTS :
		//        F : FN const&
		//            Wrapped object, unused except in
		//            compiler inference to deduce the class
		//            name for FN
		// 
		//        INIT : TI const&
		//            Initialization value argument
		//
		//        IN : std::array<TA, LEN> const&
		//            Nil input array, ignored in the base
		//            case
		// 
		// RETURNS : auto
		//            The return type is determined by the
		//            wrapped function FN
		template<class FN, typename TI, typename TA>
		auto lreduce(FN const& F, TI const& INIT, std::array<TA, 0> const& IN) const
			-> TI {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return INIT;
		}

		// FUNCTION :
		//        rreduce
		// 
		// DESCRIPTION :
		//        Implements the rreduce function with C++
		//        Template Metaprogramming using the recursive
		//        structure defined by XipHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function
		//
		//        TA : typename (Inferred)
		//            Type of the the values in the std::array
		//            argument
		//
		//        TI : typename (Inferred)
		//            Type of the the initalization value 
		//
		// ARGUMENTS :
		//        F : FN const&
		//            Wrapped object, unused except in
		//            compiler inference to deduce the class
		//            name for FN
		// 
		//        IN : std::array<TA, 0> const&
		//            Nil input array, ignored in the base-case.
		// 
		//        INIT : TI const&
		//            Initialization value argument
		//
		// RETURNS : TI
		//            Returns the initialization value
		template<class FN, typename TA, typename TI>
		auto rreduce(FN const& F, std::array<TA, 0> const& IN, TI const& INIT) const
			-> TI {
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return INIT;
		}
	};

	// MODULE :
	//        struct Lreduce
	// 
	// DESCRIPTION :
	//        Wraps the lreduce implementation provided by
	//        ReduceHelper to provide a function-like API using
	//        the operator() function.
	//        
	//        The lreduce operation takes a two-input function, an
	//        initialization value, and an input array. It
	//        iterates iterates from first to last in the input
	//        array, applying the function to return value of the
	//        previous iteration and the current element.
	// 
	// FUNCTIONS :
	//        auto operator()(FN const&, TI const&,
	//                        std::array<TA, LEN> const&) const
	struct Lreduce{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Provides a function-like interface to the
		//        linear recurisve structure defined ReduceHelper
		//
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function
		//
		//        TA : typename (Inferred)
		//            Type of the the values in the std::array
		//            argument
		//
		//        LEN : std::size_t (Inferred)
		//            The length of the input list, and the iteration
		//            variable for C++ Template Meta-Programming
		//
		//        TI : typename (Inferred)
		//            Type of the the initalization value 
		//
		// ARGUMENTS :
		//        F : FN const&
		//            Wrapped object, unused except in
		//            compiler inference to deduce the class
		//            name for FN
		//
		//        INIT : TI const&
		//            Initialization value argument
		// 
		//        IN : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            reduced
		// 
		// RETURNS : auto
		//            The return type is determined by the
		//            wrapped function FN
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <class FN, typename TA, std::size_t LEN, typename TI>
		auto operator()(FN const& F, TI const& INIT, std::array<TA, LEN> const& IN) const
			-> decltype(ReduceHelper<LEN>().lreduce(F, INIT, IN)){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return ReduceHelper<LEN>().lreduce(F, INIT, IN);
		}
	};
	// VARIABLE :
	//        lreduce (instance of Lreduce)
	// 
	// DESCRIPTION :
	//        Instantion of the Lreduce struct that provides a
	//        function-like API for the lreduce function using the
	//        operator() function of Lreduce.
	//
	// EXAMPLE :
	//        std::array<float, 4> a = {1.0, 2.0, 3.0, 4.0};
	//        float b;
	//        b = lreduce(std::divides<float>(), 1.0, a);
	//        // b === ((((1.0/1.0)/2.0)/3.0)/4.0) === .042
	//        
	// NOTES :
	//        There is no corresponding loop implementation
	//        because providing polymorphism without an allocator
	//        from a loop is impossible
	Lreduce lreduce;

	// MODULE :
	//        struct Rreduce
	// 
	// DESCRIPTION :
	//        Wraps the rreduce implementation provided by
	//        ReduceHelper to provide a function-like API using
	//        the operator() function.
	//        
	//        The rreduce operation takes a two-input function, an
	//        input array, and an initialization value. It
	//        iterates iterates from last to first in the input
	//        array, applying the function to the current element,
	//        and the return value of the previous iteration.
	// 
	// FUNCTIONS :
	//        auto operator()(FN const&, std::array<TA, LEN> const&, TI const&)
	struct Rreduce{
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Provides a function-like interface to the
		//        linear recurisve structure defined ReduceHelper
		//
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function
		//
		//        TA : typename (Inferred)
		//            Type of the the values in the std::array
		//            argument
		//
		//        LEN : std::size_t (Inferred)
		//            The length of the input list, and the iteration
		//            variable for C++ Template Meta-Programming
		//
		//        TI : typename (Inferred)
		//            Type of the the initalization value 
		//
		// ARGUMENTS :
		//        F : FN const&
		//            Wrapped object, unused except in
		//            compiler inference to deduce the class
		//            name for FN
		//
		//        IN : std::array<TA, LEN> const&
		//            Input array whose elements will be
		//            reduced
		// 
		//        INIT : TI const&
		//            Initialization value argument
		// 
		// RETURNS : auto
		//            The return type is determined by the
		//            wrapped function FN
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <class FN, typename TA, std::size_t LEN, typename TI>
		auto operator()(FN const& F, std::array<TA, LEN> const& IN, TI const& INIT) const
			-> decltype(ReduceHelper<LEN>().rreduce(F, IN, INIT)){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return ReduceHelper<LEN>().rreduce(F, IN, INIT);
		}
	};

	// VARIABLE :
	//        rreduce (instance of Rreduce)
	// 
	// DESCRIPTION :
	//        Instantion of the Rreduce struct that provides a
	//        function-like API for the rreduce function using the
	//        operator() function of Rreduce.
	//
	// EXAMPLE :
	//        std::array<float, 4> a = {1.0, 2.0, 3.0, 4.0};
	//        float b;
	//        b = rreduce(std::divides<float>(), a, 1.0);
	//        // b === (1.0/(2.0/(3.0/(4.0/1.0)))) === .375
	//        
	// NOTES :
	//        There is no corresponding loop implementation
	//        because providing polymorphism without an allocator
	//        from a loop is impossible
	Rreduce rreduce;
} // namespace hops
#endif // __REDUCE_HPP
