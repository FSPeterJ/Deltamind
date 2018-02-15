#pragma once
#include "GameObject.h"

class Spawner : public GameObject {
public:
	Spawner();
	void Awake();
	void SpawnObject(int prefabID);
};
