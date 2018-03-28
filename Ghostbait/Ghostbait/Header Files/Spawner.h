#pragma once
#include "GameObject.h"
class HexGrid;
class Core;
class EnemyBase;

class SpawnerObject : public GameObject {
public:
	SpawnerObject();
	void Awake(Object* obj);
	EnemyBase* SpawnObject(const char* prefabName, HexGrid* grid, Core* _core);
	void Destroy() override;
};
