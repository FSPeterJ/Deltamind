#pragma once
#include <unordered_set>
#include "HexagonTile.h"

namespace std {
	template <> struct hash<HexTile*> {
		size_t operator()(const HexTile* h) const {
			std::hash<int> int_hash;
			size_t hq = int_hash(h->q);
			size_t hr = int_hash(h->r);
			//uses the magic number from boost::hash_combine.
			//it's essentially a number with an equal likelyhood of 1s and 0s to give 32 'random' bits
			//also it's the inverse of the golden ratio
			//the idea is, for each tile, consecutive values will be far apart, thus ensuring an even spread, even for small ranges
			return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
		}
	};
}
