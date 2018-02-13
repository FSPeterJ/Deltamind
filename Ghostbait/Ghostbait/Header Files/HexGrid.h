#pragma once
#include <unordered_set>
#include "Delegate.h"
#include "Controlable.h"
#include "HexagonTileSpecializer.h"

struct HexagonalGridLayout;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

struct CostComparator;

struct EqualComparator {
	bool operator()(const HexTile* lhs, const HexTile* rhs) const;
	bool operator()(const HexTile& lhs, const HexTile& rhs) const;
};

using VisitedMap = std::unordered_map<    HexTile  *const, HexTile*, std::hash<HexTile*>, EqualComparator>;
using CostMap = std::unordered_map<  HexTile*, int, std::hash<HexTile*>, EqualComparator>;

struct BreadthTraversalResult {
	VisitedMap came_from;
	CostMap cost_so_far;
	std::vector<std::vector<HexTile*>> reachableTiles; //reachableTiles[k] contains all tiles k steps away
};

class HexRegion;

class HexGrid : public Controlable {
	//	std::unordered_map<HexTile, float> heights;

	int map_radius = 0;

	std::unordered_set<HexTile*, std::hash<HexTile*>, EqualComparator> map;

	std::unordered_set<HexTile*, std::hash<HexTile*>, EqualComparator>::iterator iter;

	Delegate<HexagonalGridLayout*> DrawLine;
	Delegate<HexagonalGridLayout*> DrawRange;
	Delegate<HexagonalGridLayout*> DrawIntersect;
	Delegate<HexagonalGridLayout*> DrawXStepsPath;

	int curPos = 0;
	void SetUpDrawingPaths();

	void step();

	HexRegion GetRegion(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);

public:
	HexGrid(int _radius) : map_radius(_radius) {}
	~HexGrid();

	std::vector<HexTile*> CalculatePathWithinXSteps(HexTile *const tile, HexTile *const goal, size_t steps, HexagonalGridLayout *const layout);

	std::vector<HexTile*> breadthFirstSearch(HexTile *const start, HexTile *const goal);
	BreadthTraversalResult breadthFirstTraverse(HexTile *const  tile, size_t steps, size_t maxMovement);

	std::vector<HexTile*> DijkstraSearch(HexTile *const start, HexTile *const goal);

	HexRegion GetTilesNStepsAway(HexTile *const tile, int n);

	std::vector<HexTile*> GetIntersectingTilesRanges(HexagonalGridLayout *const layout, HexTile *const fixedCenter, int fixedN, HexTile *const b, int bn);

	void Fill();

	void Display(HexagonalGridLayout layout);
};
