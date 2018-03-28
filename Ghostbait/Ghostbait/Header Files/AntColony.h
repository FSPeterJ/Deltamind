#pragma once
#include <vector>

class HexGrid;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

struct Pheromone {
	HexTile* tile;
	float lingerTime;
	float scentStrength;
};

class AntColony {
	HexGrid* activeGrid = nullptr;
	std::vector<Pheromone> trails;
	unsigned updateID;

public:
	AntColony(HexGrid* _grid);
	void Update();
	void LeavePheromone(Pheromone& _pheromone);
	void ClearTrails();
};