#ifndef __SYSTOLIC_HPP
#define __SYSTOLIC_HPP
#include <utility>
#include <array>
#include "arrayops.hpp"
#include "reduce.hpp"
#include "zip.hpp"

template<typename T, std::size_t W, std::size_t H>
using matrix = std::array<std::array<T, W>, H>;

template <typename TL, typename TR>
struct pair_t{
	TL first;
	TR second;
};

template <typename TF, typename TS, typename TT>
struct triple{
	TF left;
	TS top;
	TT hist;
	triple<TF, TS, TT> operator()(const TF& f, const TS& s, const TT& t) const{
#pragma HLS INLINE
		return {f, s, t};
	}
};

struct Wrapper{
	template<class FN, typename TL, typename TT, typename TS,
		 std::size_t H, std::size_t W>
	auto operator()(FN const& F,
			pair_t<std::array<TS, H>, matrix<TS, W, H> > const& past, 
			triple<TL, TT, std::array<TS, W> > const& cur) const
		-> pair_t<std::array<TS, H + 1>, matrix<TS, W, H + 1> > {

#pragma HLS ARRAY_PARTITION complete VARIABLE=past.first._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=past.second._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=past.second[0]._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=cur.hist._M_instance
#pragma HLS INLINE
		// Re-create the "past", everything to the right, and above if
		// the algorithm is moving down the column, and each iteration
		// adds a column to the left.
		// std::array<std::array<TS, W>, H + 1>;
		auto smatrix = hops::prepend(cur.hist, past.second);
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
		// To each "Unit" pass the current left, the current top, and
		// score matrix of everything "in the past" (to the right, and
		// above) to compute the result.
		TS result = F(cur.left, cur.top, smatrix);
		return {hops::append(past.first, result), smatrix};
	}
} wrapper;

template<typename TL, typename TT, typename TS,
	 std::size_t H, std::size_t W, class FN>
auto systolic(FN const& F, std::array<TL, H> const& left,
	std::array<TT, H> const& top,
	matrix<TS, W, H> const& smatrix)
	-> matrix<TS, W, H> {

	pair_t< std::array<TS, 0>, matrix<TS, W, 0> > init;
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=top._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=init.first._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=init.second._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=init.second[0]._M_instance
#pragma HLS INLINE

	// (Left, Top, SMatrix Row)
	auto triplizer = triple<TL, TT, std::array<TS, W> >();
	auto trips = hops::zipWith(triplizer, left, top, smatrix);


	// Compute new column
	auto newcol = hops::lreduce(hops::partial(wrapper, F), init, trips).first;

	// Returns: New smatrix
	auto newsmatrix = hops::zipWith(hops::rshift, newcol, smatrix);
#pragma HLS ARRAY_PARTITION complete VARIABLE=newcol._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=newsmatrix._M_instance

	return newsmatrix;
}

namespace sloop{
	template<class FN, typename TL, typename TT,
		 typename TS, std::size_t H, std::size_t W>
	auto systolic(FN const& F, std::array<TL, H> const& left,
		std::array<TT, H> const& top,
		matrix<TS, W, H> const& smatrix)
		-> matrix<TS, W, H> {

		std::array<TS, H> newcol;
		matrix<TS, W, H> newsmatrix;
#pragma HLS ARRAY_PARTITION complete VARIABLE=left._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=top._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=smatrix[0]._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=newcol._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=newsmatrix._M_instance
#pragma HLS ARRAY_PARTITION complete VARIABLE=newsmatrix[0]._M_instance
#pragma HLS INLINE
		for(std::size_t row = 0; row < H; ++row){
#pragma HLS UNROLL
			newcol[row] = F(row, left[row], top[row], smatrix);
		}

		for(std::size_t row = 0; row < H; ++row){
#pragma HLS UNROLL
			for(std::size_t col = W - 1; col > 0; --col){
				newsmatrix[row][col] = smatrix[row][col - 1];
			}
			newsmatrix[row][0] = newcol[row];
		}
		return newsmatrix;
	}
}

#endif // __SYSTOLIC_HPP
