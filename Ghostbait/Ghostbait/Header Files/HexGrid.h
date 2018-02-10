#pragma once
#include <unordered_set>
#include "Delegate.h"
#include "Controlable.h"

struct HexagonalGridLayout;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

struct CostComparator;

class HexGrid : public Controlable {
	//	std::unordered_map<HexTile, float> heights;

	struct HexRegion {
		std::vector<HexTile> tiles;
	};

	float map_radius = 0.0f;

	std::unordered_set<HexTile*> map;

	std::unordered_set<HexTile*>::iterator iter;

	Delegate<HexagonalGridLayout*> DrawLine;
	Delegate<HexagonalGridLayout*> DrawRange;
	Delegate<HexagonalGridLayout*> DrawIntersect;

	int curPos = 0;
	void SetUpDrawingPaths();

	void step();

	HexRegion GetRegion(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax);
	void ColorRegion(HexRegion region, HexagonalGridLayout* layout,DirectX::XMFLOAT3 color, float offset);

public:
	HexGrid(float _radius) : map_radius(_radius) {}
	~HexGrid();

	std::vector<HexTile*> breadthFirstSearch(HexTile* start, HexTile* goal);

	std::vector<HexTile*> DijkstraSearch(HexTile* start, HexTile* goal);

	std::vector<HexTile*> GetTilesNStepsAway(HexTile* tile, size_t n);

	std::vector<HexTile*> GetIntersectingTilesRanges(HexagonalGridLayout* layout, HexTile* fixedCenter, int fixedN, HexTile* b, int bn);

	void Fill();

	void Display(HexagonalGridLayout layout);
};
