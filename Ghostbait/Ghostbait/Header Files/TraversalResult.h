#pragma once

#include <unordered_map>
#include <vector>
#include "HexTileEqualityCheck.h"
#include "HexagonTileSpecializer.h"

class HexPath;

using VisitedMap = std::unordered_map<HexTile *const, HexTile*, std::hash<HexTile*>, EqualComparator>;
using CostMap = std::unordered_map<HexTile*, float, std::hash<HexTile*>, EqualComparator>;

struct TraversalResult {
	VisitedMap visitedMap;
	CostMap costMap;
	std::vector<HexPath> reachableTiles; //reachableTiles[k] contains all tiles k steps/cost away
};
