#pragma once
#include <vector>

class HexGrid;
class HexTile;

struct Pheromone {
	HexTile* tile;
	float lingerTime;
	float scentStrength;
};

class AntColony {
	std::vector<Pheromone>
};