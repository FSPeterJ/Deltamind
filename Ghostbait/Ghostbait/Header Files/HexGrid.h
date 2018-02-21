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

using VisitedMap = std::unordered_map<HexTile *const, HexTile*, std::hash<HexTile*>>;
using CostMap = std::unordered_map<HexTile*, float, std::hash<HexTile*>, EqualComparator>;

using HeuristicsFunction = std::function<float(HexTile*, HexTile*)>;

class HexRegion;
class HexPath;

bool operator<(const HexPath&p, const HexPath&p2);

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

	std::size_t curPos = 0;
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
	static const float Blocked;

	HexGrid(float _radius, const HexagonalGridLayout& _layout);
	~HexGrid();

	HexTile* PointToTile(const DirectX::XMFLOAT2& p);
	DirectX::XMFLOAT2 TileToPoint(HexTile * tile);

	HexTile* GetRandomTile();

	DirectX::XMFLOAT2 TileToWorld(const DirectX::XMFLOAT2& p);

	bool Snap(const DirectX::XMFLOAT2& p, OUT DirectX::XMFLOAT2& snapPoint);

	HexTile* GetTile(const int x, const int y);
	HexTile* GetTile(HexTile& t) const;

	HexPath breadthFirstSearch(HexTile *const start, HexTile *const goal);

	TraversalResult breadthFirstTraverse(HexTile *const tile, size_t steps, size_t maxMovement);

	HexPath DijkstraSearch(HexTile *const start, HexTile *const goal);
	TraversalResult DijkstraTraverse(HexTile *const tile, size_t cost, size_t maxMovement);

	HexPath AStarSearch(HexTile *const start, HexTile *const goal, HeuristicsFunction Heuristic);

	HexPath AStarSearch(const DirectX::XMFLOAT2& start, const DirectX::XMFLOAT2& goal, HeuristicsFunction Heuristic);

	HexRegion GetTilesNStepsAway(HexTile *const tile, int n);

	bool AddObstacle(const DirectX::XMFLOAT2& obstaclePosition);
	bool RemoveObstacle(const DirectX::XMFLOAT2& obstaclePosition);

	HexPath CalculatePathWithinXSteps(HexTile *const start, HexTile *const goal, size_t steps);
	HexPath CalculatePathWithinXCost(HexTile *const start, HexTile *const goal, size_t cost);

	HexRegion GetIntersectingTilesRanges(HexTile *const fixedCenter, int fixedN, HexTile *const b, int bn);

	bool IsBlocked(HexTile* tile);
	bool IsBlocked(const DirectX::XMFLOAT2& pos);
	bool IsBlocked(const float x, const float z);

	void Fill();

	void Display(DirectX::XMFLOAT2& player);
};

