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

template <typename TileType>
class GridTileVector;
typedef GridTileVector<HexTile> HexRegion;
typedef GridTileVector<HexTile*> HexPath;

using VisitedMap = std::unordered_map<HexTile *const, HexTile*, std::hash<HexTile*>, EqualComparator>;
using CostMap = std::unordered_map<HexTile*, int, std::hash<HexTile*>, EqualComparator>;

struct BreadthTraversalResult {
	VisitedMap came_from;
	CostMap cost_so_far;
	std::vector<HexPath> reachableTiles; //reachableTiles[k] contains all tiles k steps away
};

class HexGrid: public Controlable {
	using GridContainer = std::unordered_set<HexTile*, std::hash<HexTile*>, EqualComparator>;

	float map_radius = 0;

	GridContainer map;
	GridContainer::iterator iter;

	Delegate<HexagonalGridLayout*> DrawLine;
	Delegate<HexagonalGridLayout*> DrawRange;
	Delegate<HexagonalGridLayout*> DrawIntersect;
	Delegate<HexagonalGridLayout*> DrawXStepsPath;

	int curPos = 0;
	void SetUpDrawingPaths();

	void step();

	HexRegion GetRegion(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);

public:
	HexGrid(float _radius) : map_radius(_radius) {}
	~HexGrid();

	HexPath breadthFirstSearch(HexTile *const start, HexTile *const goal);

	BreadthTraversalResult breadthFirstTraverse(HexTile *const  tile, size_t steps, size_t maxMovement);

	HexPath DijkstraSearch(HexTile *const start, HexTile *const goal);

	//HexPath AStarSearch(HexTile *const start, HexTile *const goal);

	HexRegion GetTilesNStepsAway(HexTile *const tile, int n);

	HexPath CalculatePathWithinXSteps(HexTile *const start, HexTile *const goal, size_t steps, HexagonalGridLayout *const layout);

	HexRegion GetIntersectingTilesRanges(HexagonalGridLayout *const layout, HexTile *const fixedCenter, int fixedN, HexTile *const b, int bn);

	void Fill();

	void Display(HexagonalGridLayout layout);
};
