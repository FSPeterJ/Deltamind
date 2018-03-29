#pragma once
#include <vector>

class HexGrid;

template <typename T>
struct HexagonTile;
typedef HexagonTile<int> HexTile;

struct AntProperties {
	float lingerTime = 5.0f;
	float scentStrength = 2.0f;

	AntProperties();
	AntProperties(float _lingerTime, float _scentStrength);
	AntProperties(const AntProperties& _other);
	AntProperties& operator=(const AntProperties& _other);
};

struct Pheromone {
	HexTile* tile = nullptr;
	AntProperties prop;

	Pheromone();
	Pheromone(HexTile* const _tile, const AntProperties& _prop);
	Pheromone(HexTile* const _tile, float _lingerTime, float _scentStrength);
	Pheromone& operator=(const Pheromone& _other);
};

class AntColony {
	static HexGrid* activeGrid;
	static std::vector<Pheromone> trails;
	static unsigned updateID;

public:
	static void SetGrid(HexGrid* _grid);
	static void AddUpdateToEngineStruct();
	static void RemoveUpdateFromEngineStruct();
	static void Update();
	static void LeavePheromone(const Pheromone& _pheromone);
	static void LeavePheromone(HexTile* const _tile, const AntProperties& _prop);
	static void LeavePheromone(HexTile* const _tile, float _lingerTime, float _scentStrength);
	static void ClearTrails();
};