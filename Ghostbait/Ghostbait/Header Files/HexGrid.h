#pragma once
#include <unordered_set>
#include "Delegate.h"

struct HexagonalGridLayout;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

struct CostComparator;

class HexGrid {
	//	std::unordered_map<HexTile, float> heights;

	float map_radius = 0.0f;

	std::unordered_set<HexTile*> map;

	std::unordered_set<HexTile*>::iterator iter;

	Delegate<HexagonalGridLayout*> Draw;

	int curPos = 0;
	void SetUpDrawingPaths();
public:
	HexGrid(float _radius) : map_radius(_radius) {}
	~HexGrid();

	std::vector<HexTile*> breadthFirstSearch(HexTile* start, HexTile* goal);

	std::vector<HexTile*> DijkstraSearch(HexTile* start, HexTile* goal);

	std::vector<HexTile*> GetTilesNStepsAway(HexTile* tile, size_t n);

	void Fill();

	void Display(HexagonalGridLayout layout);
};
