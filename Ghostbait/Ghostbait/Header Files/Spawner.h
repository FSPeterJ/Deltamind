#pragma once
#include "GameObject.h"
class HexGrid;

class SpawnerObject : public GameObject {
public:
	SpawnerObject();
	void Awake(Object* obj);
	void SpawnObject(const char* prefabName, HexGrid* grid, const DirectX::XMFLOAT2& goal);
	void Destroy() override;
};
