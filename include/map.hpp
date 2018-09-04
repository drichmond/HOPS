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
//        map.hpp
//
// DESCRIPTION :
//        This file implements the map Higher-Order Function.
//
// NAMESPACES :
//        hops
//        hops::loop
//        
// PUBLIC MODULES :
//        Map
// 
// PRIVATE MODULES :
//        MapHelper
//
// PUBLIC VARIABLES :
//        map (an instance of Map)
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
#ifndef __MAP_HPP
#define __MAP_HPP
#include <array>
#include "arrayops.hpp"
namespace hops{
	// MODULE :
	//        struct MapHelper
	// 
	// DESCRIPTION :
	//        Implements the linear, loop-like structure of the
	//        map function using C++ Template Meta-Programming and
	//        Partial Template Specialization.
	// 
	// PARAMETERS :
	//        LEN : std::size_t
	//            The length of the input list, and the iteration
	//            variable for C++ Template Meta-Programming
	// 
	// FUNCTIONS :
	//        auto operator()(FN const&, std::array<TL, LEN> const&,
	//                        std::array<TR, LEN> const&) const
	template <std::size_t LEN>
	struct MapHelper{
		// FUNCTION :
		//        map
		// 
		// DESCRIPTION :
		//        Implements the map function using C++
		//        Template Metaprogramming using the recursive
		//        structure defined by MapHelper
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function. 
		//
		//        T : typename (Inferred)
		//            Type of the the std::array values in
		//            input array
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of a wrapped function used
		//            to compute an output value at each array
		//            location
		//
		//        IN : std::array<T, LEN> const&
		//            Input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(IN[0])), LEN>
		//        An array where the output at each location
		//        is the result of F's operator() function
		//        applied each value in the input array.
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA>
		auto operator()(FN const& F, std::array<TA, LEN> const& IN) const
			-> std::array<decltype(F(IN[0])), LEN>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return prepend(F(IN[0]),
				MapHelper<LEN-1>()(F, tail(IN)));
		}
	};

	// MODULE :
	//        struct MapHelper
	// 
	// DESCRIPTION :
	//        Base-case implementation of MapHelper for a
	//        loop-like structure using C++ Template
	//        Meta-Programming and Partial Template
	//        Specialization on LEN = 0
	// 
	// FUNCTIONS :
	//        auto operator()(FN const& F, std::array<TA, 0> const&) const
	template <>
	struct MapHelper<0>{
		// FUNCTION :
		//        map
		// 
		// DESCRIPTION :
		//        Implements the base map function in the
		//        linear recursive structure defined by C++
		//        Template Metaprogramming
		// 
		// PARAMETERS :
        	//        FN : class (Inferred)
        	//            Typename of a wrapped function. 
		//
		//        T : typename (Inferred)
		//            Type of the the std::array values in
		//            input array
		//
		// ARGUMENTS : 
		//        F : FN const&
		//            Instantiation of a wrapped function used
		//            to compute an output value at each array
		//            location
		//
		//        IN : std::array<T, 0> const&
		//            Nil input array whose elements will be
		//            passed as the first argument to F's
		//            operator() function
		// 
		// RETURNS : std::array<decltype(F(IN[0])), 0>
		//        A Nil array where the element type is the
		//        result of F's operator() function applied
		//        to a hypothetical value in the array
		// 
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template<class FN, typename TA>
		auto operator()(FN const& F, std::array<TA, 0> const& IN) const
			-> std::array<decltype(F(IN[0])), 0>{
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			std::array<decltype(F(IN[0])), 0> nil;
			return nil;
		}
	};

	// MODULE :
	//        struct Map
	// 
	// DESCRIPTION :
	//        Wraps the map function using the operator() function
	//        to provide a function-like API on top of the linear
	//        recursive structure the MapHelper with the .
	// 
	// FUNCTIONS :
	//        auto operator()(FN const& F, std::array<TA, LEN> const&) const
	//
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	//
	//        The trailing const declaration on the
	//        operator() function is required by our
	//        library, and indicates to the compiler that
	//        the method modifies no internal state
	struct Map{
		template<class FN, typename TA, std::size_t LEN>
		auto operator()(FN const& F, std::array<TA, LEN> const& IN) const
			-> decltype(MapHelper<LEN>()(F, IN)){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
			return MapHelper<LEN>()(F, IN);
		}
	};

	// VARIABLE :
	//        map (instance of Map)
	// 
	// DESCRIPTION :
	//        Instantion of the map struct that provides a
	//        function-like API with the operator() function.
	//
	// EXAMPLE :
	//
	//            std::array<int, 2> a = {1, 2, 3, 4}, b;
	//            b = map(mulby2, b); // b === {1, 2, 3, 4}
	//        
	// NOTES :
	//        A corresponding loop-based implementation is defined
	//        in the hops::loop namespace
	Map map;

	namespace loop{
		// MODULE :
		//        struct Map
		// 
		// DESCRIPTION :
		//        Implements the map function using a loop in
		//        the operator() function to provide a
		//        function-like API.
		// 
		// FUNCTIONS :
		//        auto operator()(FN const& F, std::array<TA, LEN> const&) const
		//
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops::loop namespace
		struct Map{
			// FUNCTION :
			//        operator()
			// 
			// DESCRIPTION :
			//        Implements the map function using
			//        an unrolled for-loop 
			// 
			// PARAMETERS :
			//        FN : class (Inferred)
			//            Typename of a wrapped function. 
			//
			//        T : typename (Inferred)
			//            Type of the the std::array values in
			//            input array
			//
			// ARGUMENTS : 
			//        F : FN const&
			//            Instantiation of a wrapped function used
			//            to compute an output value at each array
			//            location
			//
			//        IN : std::array<T, LEN> const&
			//            Input array whose elements will be
			//            passed as the first argument to F's
			//            operator() function
			// 
			// RETURNS : std::array<decltype(F(IN[0])), LEN>
			//        An array where the output at each location
			//        is the result of F's operator() function
			//        applied each value in the input array.
			//
			// NOTES :
			//        The trailing const declaration on the
			//        operator() function is required by our
			//        library, and indicates to the compiler that
			//        the method modifies no internal state
			template <class FN, typename TA, std::size_t LEN> 
			auto operator()(FN const& F, std::array<TA, LEN> const& IN) const
				-> decltype(std::array<decltype(F(IN[0])), LEN>()){
#pragma HLS ARRAY_PARTITION complete VARIABLE=IN._M_instance
#pragma HLS INLINE
				std::array<decltype(F(IN[0])), LEN> temp;
#pragma HLS ARRAY_PARTITION complete VARIABLE=temp._M_instance
				for(std::size_t i = 0; i < LEN; ++i){
#pragma HLS UNROLL
					temp[i] = F(IN[i]);
				}
				return temp;
			}
		};
		// VARIABLE :
		//        map (instance of Map)
		// 
		// DESCRIPTION :
		//        Instantion of the loop-based map struct that
		//        provides a function-like API with the
		//        operator() function.
		//
		// EXAMPLE :
		//            std::array<int, 2> a = {1, 2, 3, 4}, b;
		//            b = map(mulby2, b); // b === {1, 2, 3, 4}
		//        
		// NOTES :
		//        A corresponding loop-based implementation is defined
		//        in the hops::loop namespace
		Map map;
	} // namespace loop
} // namespace hops
#endif //__MAP_HPP
