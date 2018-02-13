#pragma once
//#include <unordered_set>
//#include "HexagonTile.h"

//template <> struct hash<HexTile*> {
//	size_t operator()(const HexTile* h) const {
//		std::hash<int> int_hash;
//		size_t hq = int_hash(h->v[0]);
//		size_t hr = int_hash(h->v[1]);
//		//uses the magic number from boost::hash_combine.
//		//it's essentially a number with an equal likelyhood of 1s and 0s to give 32 'random' bits
//		//also it's the inverse of the golden ratio
//		//the idea is, for each tile, consecutive values will be far apart, thus ensuring an even spread, even for small ranges
//		return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
//	}
//};
template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

namespace std {
	template<>
	struct hash<HexTile*> {
		typedef HexTile* argument_type;
		typedef size_t result_type;
		size_t operator()(const argument_type& data) const noexcept;
	};
}
