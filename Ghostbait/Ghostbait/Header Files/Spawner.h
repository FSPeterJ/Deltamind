#pragma once
#include "GameObject.h"

class Spawner : public GameObject {
	float dt = 0;
	float timeSinceLastSpawn = 0;
	int spawnCount = 0;

	int prefabID = 6;
	int objectsToSpawn = 1;
	float startSpawnDelay = 0;
	float runSpawnDelay = 2;

	void SpawnObject();
public:
	Spawner();
	void Update();
};
