#pragma once
#include <unordered_set>
#include "Delegate.h"
#include "Controlable.h"
#include "HexagonalGridLayout.h"
#include "HexagonTileSpecializer.h"

namespace DirectX { struct XMFLOAT2; }

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

using CostMap = std::unordered_map<HexTile*, float, std::hash<HexTile*>, EqualComparator>;


struct TraversalResult {
	VisitedMap visitedMap;
	CostMap costMap;
	std::vector<HexPath> reachableTiles; //reachableTiles[k] contains all tiles k steps/cost away
};

class HexGrid: public Controlable {
	using GridContainer = std::unordered_set<HexTile*, std::hash<HexTile*>, EqualComparator>;

	float map_radius = 0;

	GridContainer map;
	GridContainer::iterator iter;

	Delegate<> DrawLine;
	Delegate<> DrawRange;
	Delegate<> DrawIntersect;
	Delegate<> DrawXStepsPath;
	Delegate<> DrawAIPath;

	static HexRegion blocked;

	int curPos = 0;
	void SetUpDrawingPaths();

	void step();


	/// <summary>
	/// Gets the region cooresponding to the specified bounds. Can return tiles that are not part of the grid.
	/// If you only want tiles on the grid, use Filter().
	/// </summary>
	/// <param name="xmin">The xmin.</param>
	/// <param name="xmax">The xmax.</param>
	/// <param name="ymin">The ymin.</param>
	/// <param name="ymax">The ymax.</param>
	/// <param name="zmin">The zmin.</param>
	/// <param name="zmax">The zmax.</param>
	/// <returns>HexRegion.</returns>
	HexRegion GetRegion(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);
	
	HexagonalGridLayout layout;
public:
	const float Blocked = float(0xDEAD);

	HexGrid(float _radius, const HexagonalGridLayout& _layout);
	~HexGrid();

	HexTile* PointToTile(const DirectX::XMFLOAT2& p);
	bool Snap(const DirectX::XMFLOAT2& p, OUT DirectX::XMFLOAT2& snapPoint);

	HexTile* GetTile(const int x, const int y);
	HexTile* GetTile(HexTile& t) const;

	HexPath breadthFirstSearch(HexTile *const start, HexTile *const goal);

	TraversalResult breadthFirstTraverse(HexTile *const tile, size_t steps, size_t maxMovement);

	HexPath DijkstraSearch(HexTile *const start, HexTile *const goal);
	TraversalResult DijkstraTraverse(HexTile *const tile, size_t cost, size_t maxMovement);

	HexPath AStarSearch(HexTile *const start, HexTile *const goal, std::function<float(HexTile*, HexTile*)> Heuristic, const float heuristicWeight = 1.2f);

	HexRegion GetTilesNStepsAway(HexTile *const tile, int n);

	HexPath CalculatePathWithinXSteps(HexTile *const start, HexTile *const goal, size_t steps);
	HexPath CalculatePathWithinXCost(HexTile *const start, HexTile *const goal, size_t cost);

	HexRegion GetIntersectingTilesRanges(HexTile *const fixedCenter, int fixedN, HexTile *const b, int bn);

	void Fill();

	void Display();
};
