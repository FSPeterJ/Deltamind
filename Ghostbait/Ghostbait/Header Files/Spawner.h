#pragma once
#include "GameObject.h"
class HexGrid;

class Spawner : public GameObject {
public:
	Spawner();
	void Awake(Object* obj);
	void SpawnObject(char* prefabName, HexGrid* grid, DirectX::XMFLOAT2 goal);
};
