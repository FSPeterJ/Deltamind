#pragma once
#include "GameObject.h"
class HexGrid;
class Core;

class SpawnerObject : public GameObject {
public:
	SpawnerObject();
	void Awake(Object* obj);
	void SpawnObject(const char* prefabName, HexGrid* grid, Core* _core);
	void Destroy() override;
};
