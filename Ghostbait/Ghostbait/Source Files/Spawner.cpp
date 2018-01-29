#include "Spawner.h"

Spawner::Spawner() {
	timeSinceLastSpawn = runSpawnDelay - startSpawnDelay;
	SetTag("Spawner");
}
void Spawner::SpawnObject() {
	Object* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, { 0, 0, 0 }, &obj));
	obj->position = position;
	timeSinceLastSpawn = 0;
	++spawnCount;
}
void Spawner::Update() {
	timeSinceLastSpawn += (float)GhostTime::DeltaTime();
	if (timeSinceLastSpawn >= runSpawnDelay && spawnCount < objectsToSpawn) {
		SpawnObject();
	}
}