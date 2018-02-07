#pragma once
#include "HexagonTile.h"

#include <unordered_set>
#include "Delegate.h"
#include <memory>         // for allocator
#include <stddef.h>       // for size_t
#include "cstdlib"        // for size_t
#include "type_traits"    // for hash, move, _Conditionally_enabled_hash
#include "vector"         // for vector
#include "xstddef"        // for equal_to
#include "xutility"       // for _Unchecked, _Rechecked

struct HexagonalGridLayout;

template <> struct std::hash<HexTile> {
	size_t operator()(const HexTile& h) const {
		std::hash<int> int_hash;
		size_t hq = int_hash(h.q);
		size_t hr = int_hash(h.r);
		return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2)); //uses the magic number from boost::hash_combine.
		//it's essentially a number with an equal likelyhood of 1s and 0s to give 32 'random' bits
		//also it's the inverse of the golden ratio
		//the idea is, for each tile, consecutive values will be far apart, thus ensuring an even spread, even for small ranges
	}
};

class HexGrid {
	//	std::unordered_map<HexTile, float> heights;

	float map_radius = 0.0f;

	std::unordered_set<HexTile> map;

	std::unordered_set<HexTile>::iterator iter;

	Delegate<HexagonalGridLayout*> Draw;

	int curPos = 0;
public:
	HexGrid(float _radius) : map_radius(_radius) {}

	std::vector<HexTile*> breadthFirstSearch(HexTile* start, HexTile* goal);

	std::vector<HexTile*> DijkstraSearch(HexTile* start, HexTile* goal);

	void stuff();

	void Fill();

	void Display(HexagonalGridLayout layout);
};
