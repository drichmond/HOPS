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
//        functools.hpp
//
// DESCRIPTION :
//        This file implements a collection of function manipulation
//        operations that are useful for combining higher-order
//        functions to create new functions
//       
// NAMESPACES :
//        hops
//
// PUBLIC MODULES :
//        Partial
//        Compose
//        Curry
//        Uncurry
//        Flip
//
// PUBLIC FUNCTIONS :
//        partial
//        compose
//        curry
//        uncurry
//        flip
//
// NOTES :
//        This file requires a compiler implementing the C++11
//        standard.
//
// AUTHOR : Dustin Richmond (drichmond)
//
// **********************************************************************
#ifndef __FUNCTOOLS_HPP
#define __FUNCTOOLS_HPP
#include <utility>
namespace hops {
	// MODULE :
	//        struct Partial
	// 
	// DESCRIPTION :
	//        Wraps the the partial-evaluation operation as the
	//        operator() function.
	// 
	//        The partial evaluation operation takes a zero-input
	//        wrapped function and a n-input (n > 1) wrapped
	//        function as template parameters and returns a new
	//        wrapper object with a function where the second
	//        wrapped function the first argument to the first
	//        wrapped function.
	//
	// PARAMETERS :
	//        FN : class 
	//            Typename of a wrapped function on multiple
	//            arguments
	// 
	//        T : class
	//            Typename of the second argument to the
	//            constructor to be used as the first argument to
	//            the object FN
	//
	// FUNCTIONS :
	//        auto operator()(TS const&...) const
	template<class FN, class TB>
	struct Partial{
		FN const& A;
		TB const& B;
		Partial(FN const& A, TB const& B) : A(A), B(B){ }
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Implements the partial evaluation operation
		//        on the two wrapped functions provided as
		//        template parameters to the class.
		//
		// PARAMETERS :
		//        TS : Variadic typename(s) (Inferred)
		//            Input types to the wrapped function
		//            provided as the first parameter
		// 
		// ARGUMENTS :
		//        ARGS : TS const&...
		//            Variadic arguments passed to the first
		//            wrapped function provided as a template
		//            argument
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename... TS>
		auto operator()(TS const&... ARGS) const
			-> decltype(A(B, ARGS...)){
#pragma HLS INLINE
			return A(B, ARGS...);
		}
	};
	// FUNCTION :
	//        partial
	// 
	// DESCRIPTION :
	//        Takes a zero-input wrapped function and a n-input (n
	//        > 1) wrapped function as template parameters and
	//        returns a new wrapper object with a function where
	//        the second argument is the first argument to
	//        the first argument.
	// 
	// PARAMETERS :
	//        FN : class (Inferred) 
	//            Typename of the left wrapped function. 
	// 
	//        TB : class (Inferred)
	//            Typename of the right wrapped function. 
	//
	// ARGUMENTS :
	//        A : FN const&
	//            Object implementing the operator() function. 
	//            
	//        B : TB const&
	//            The object to pass as the first argument to A.
	//
	// EXAMPLE :
	//        std::array<int, 4> a = {1, 2, 3, 4};
	//        auto fn = partial(std::multiplies<int>(),
	//                          std::integral_constant<int, 42>())();
	//        b = map(fn, a) // b === {42, 84, 126, 168};
	template<class FN, class TB>
	Partial<FN, TB> partial(FN const& A, TB const& B){
#pragma HLS INLINE
		return Partial<FN, TB>(A, B);
	}
	
	// MODULE :
	//        struct Compose
	// 
	// DESCRIPTION :
	//        Wraps the implementation of the compose operation
	//        with the operator() function.
	//
	//        The compose operation takes a one-input wrapped
	//        function and a n-input wrapped function as template
	//        parameters and returns a wrapper object with a
	//        function where the result of the second function is
	//        the input to the first (right to left composition).
	// 
	// PARAMETERS :
	//        FNA : class
	//            Typename of the wrapped function passed as the
	//            first argument.
	// 
	//        FNB : class
	//            Typename of the wrapped function passed as the
	//            second argument.
	//
	// FUNCTIONS :
	//        auto operator()(TS const&...) const
	template <class FNA, class FNB>
	struct Compose{
		FNA const& A;
		FNB const& B;
		Compose(FNA const& A, FNB const&B) : A(A), B(B){}
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Implements the compose operation on the
		//        wrapped functions provided as template
		//        parameters to the class.
		//
		// PARAMETERS :
		//        TS : Variadic typename(s) (Inferred)
		//            Input types to the wrapped function
		//            provided as the second parameter
		// 
		// ARGUMENTS :
		//        ARGS : TS const& ...
		//            Variadic arguments passed to the second
		//            wrapped function provided as a template
		//            argument
		template<typename... TS>
		auto operator()(TS const&... ARGS) const
			-> decltype(A(B(ARGS...))){
#pragma HLS INLINE
			return A(B(ARGS...));
		}
	};
	// FUNCTION :
	//        compose
	// 
	// DESCRIPTION :
	//        Takes a one-input wrapped function and a n-input
	//        wrapped function as template parameters and returns
	//        a wrapper object with a function where the result of
	//        the second function is the input to the first (right
	//        to left composition).
	// 
	// PARAMETERS :
	//        FNA : class (Inferred)
	//            Typename of the left wrapped function. 
	// 
	//        FNB : class (Inferred)
	//            Typename of the right wrapped function. 
	//
	// ARGUMENTS :
	//        A : FNA const&
	//            Object implementing the operator() function. 
	//            
	//        B : FNB const&
	//            Object implementing the operator() function. 
	//
	// EXAMPLE :
	//        std::array<int, 4> a = {1, 2, 3, 4}, b;
	//        auto m2 = partial(std::multiplies<int>(), 
	//                          std::integral_constant<int, 4>())();
	//        auto p4 = partial(std::plus<int>(),
	//                          std::integral_<int, 3>())();
	//        auto fn = compose(m2, p4); // 4 * (x + 3)
	//        b = map(fn, a) // b === {16, 20, 24, 28};
	template<class FNA, class FNB>
	Compose<FNA, FNB> compose(FNA const& A, FNB const& B){
#pragma HLS INLINE	
		return Compose<FNA, FNB>(A, B);
	}

	// MODULE :
	//        struct Curry
	// 
	// DESCRIPTION :
	//        Wraps the curry operation as the operator()
	//        function.
	// 
	//        The curry operation takes a one-input wrapped
	//        function on pairs as a template parameter and
	//        returns a wrapper object with a two-input function
	//        where the arguments are paired and passed to the
	//        input function.
	//
	// PARAMETERS :
	//        FN : class
	//            Typename of a one-argument wrapped function on
	//            pairs.
	// 
	// FUNCTIONS :
	//        auto operator()(TL const&, TR const&) const
	template <class FN>
	struct Curry {
		FN const& F;
		Curry(FN const& F) : F(F) { }
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Implements the curry operation on the
		//        one-input wrapped function provided as a
		//        template parameter to the class. 
		//
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Input type of the first argument
		// 
		//        TR : typename (Inferred)
		//            Input type of the second argument
		// 
		// ARGUMENTS :
		//        L : TL const&
		//            The first input argument and first
		//            element of the pair passed to the
		//            function provided as the template
		//            parameter
		//
		//        R : TR const&
		//            The second input argument and second
		//            element of the pair passed to the
		//            function provided as the template
		//            parameter
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename TL, typename TR>
		auto operator()(TL const& L, TR const& R) const
			-> decltype(F(std::pair<TL, TR>(L, R))){
#pragma HLS INLINE
			return F(std::pair<TL, TR>(L, R));
		}
	};
	// FUNCTION :
	//        curry
	// 
	// DESCRIPTION :
	//        Takes a one-input wrapped function on pairs as a
	//        template parameter and returns a wrapper object with
	//        a two-input function where the arguments are paired
	//        and passed to the input function.
	// 
	// PARAMETERS :
	//        FN : class (Inferred)
	//            Typename of the wrapped function. 
	// 
	// ARGUMENTS :
	//        F   : FN
	//            Object implementing the one-input operator()
	//            function on pairs. 
	template <class FN>
	Curry<FN> curry(FN const& F){
#pragma HLS INLINE
		return Curry<FN>(F);
	}

	// MODULE :
	//        struct Uncurry
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the uncurry
	//        operation as the operator() function. 
	// 
	//        The uncurry operation takes a two-input wrapped
	//        function as a template parameter and returns a
	//        wrapper object with a one-input function that takes
	//        a std::pair and passes its elements to the wrapped
	//        function provided as input.
	// 
	// PARAMETERS :
	//        FN : class
	//            Typename of a one-argument wrapped function.
	// 
	// FUNCTIONS :
	//        auto operator()(std::pair<TL, TR> const&) const
	template <class FN>
	struct Uncurry{
		FN const& F;
		Uncurry(FN const& F) : F(F) {}
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Implements the uncurry operation on the
		//        wrapped function provided as a template
		//        parameters to this class.
		//
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Input type of the first pair element
		// 
		//        TR : typename (Inferred)
		//            Input type of the second pair element
		// 
		// ARGUMENTS :
		//        P : std::pair<TL, TR>
		//            The pair to split and provide to the
		//            function provided as a template
		//            parameter to the class
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename TL, typename TR>
		auto operator()(std::pair<TL, TR> const& P) const
			-> decltype(F(P.first, P.second)){
#pragma HLS INLINE
			return F(P.first, P.second);
		}
	};

	// FUNCTION :
	//        uncurry
	// 
	// DESCRIPTION :
	//        Takes a two-input wrapped function as a template
	//        parameter and returns a wrapper object with a
	//        one-input function that takes a std::pair and passes
	//        its elements to the wrapped function provided as
	//        input.
	// 
	// PARAMETERS :
	//        FN : class (Inferred)
	//            Typename of the wrapped function. 
	// 
	// ARGUMENTS :
	//        F : FN const
	//            Object implementing the two-input operator()
	//            function. 
	// EXAMPLE :
	//        std::array<int, 4> a = {1, 2, 3, 4};
	//        std::array<int, 4> b = {2, 4, 6, 8};
	//        std::array<int, 4> c;
	//        auto fn = uncurry(std::multiplies<int>())();
	//        c = map(fn, zip(a, b)) // c === {2, 8, 18, 32};
	template <class FN>
	Uncurry<FN> uncurry(FN const& F){
#pragma HLS INLINE
		return Uncurry<FN>(F);
	}

	// MODULE :
	//        struct Flip
	// 
	// DESCRIPTION :
	//        Function-wrapped-struct implementing the flip
	//        operation as the operator() function.
	//
	//        The flip operation takes a two-input wrapped
	//        function as a template parameter and returns a
	//        wrapper object with a function that flips the order
	//        of the arguments and passes them to the input
	//        function.
	// 
	// PARAMETERS :
	//        FN : class
	//            Typename of a two-argument wrapped function.
	// 
	// FUNCTIONS :
	//        auto operator()(TL const&, TR const&) const
	template <class FN>
	struct Flip{
		FN const& F;
		Flip(FN const&F) : F(F) {}
		// FUNCTION :
		//        operator()
		// 
		// DESCRIPTION :
		//        Implements the flip operation on the
		//        one-input wrapped function provided as a
		//        template parameter to the class. 
		//
		// PARAMETERS :
		//        TL : typename (Inferred)
		//            Input type of the first argument
		// 
		//        TR : typename (Inferred)
		//            Input type of the second argument
		// 
		// ARGUMENTS :
		//        L : TL
		//            The first input argument, provided as
		//            the second argument to the wrapped
		//            function passed to the class as a
		//            template parameter.
		//
		//        R : TR
		//            The second input argument, provided as
		//            the first argument to the wrapped
		//            function passed to the class as a
		//            template parameter.
		//
		// NOTES :
		//        The trailing const declaration on the
		//        operator() function is required by our
		//        library, and indicates to the compiler that
		//        the method modifies no internal state
		template <typename TL, typename TR>
		auto operator()(TL const& L, TR const& R) const
			-> decltype(F(R, L)){
#pragma HLS INLINE
			return F(R, L);
		}
	};

	// FUNCTION :
	//        flip
	// 
	// DESCRIPTION :
	//        Takes a two-input wrapped function as a template
	//        parameter and returns a wrapper object with a
	//        function that flips the order of the arguments and
	//        passes them to the input function.
	// 
	// PARAMETERS :
	//        FN : class (Inferred)
	//            Typename of the wrapped function. 
	// 
	// ARGUMENTS :
	//        F : FN
	//            Object implementing the two-input operator()
	//            function. Not explicitly used.
	// EXAMPLE :
	//        std::array<int, 4> a = {1, 2, 3, 4}, b;
	//        b = rreduce(flip(prepend), a) // b === {4, 3, 2, 1}
	template <class FN>
	Flip<FN> flip(FN const& F){
#pragma HLS INLINE
		return Flip<FN>(F);
	}
} // namespace hops
#endif // __FUNCTOOLS_HPP
