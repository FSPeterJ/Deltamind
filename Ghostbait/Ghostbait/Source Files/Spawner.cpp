#include "Spawner.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"
void Spawner::Awake() {
	SetTag("Spawner");
	MessageEvents::SendMessage(EVENT_SpawnerCreated, SpawnerCreatedMessage(this));
}
Spawner::Spawner() {
}
void Spawner::SpawnObject(int prefabID) {
	GameObject* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(prefabID, {0, 0, 0}, &obj));
	obj->position = position;
	obj->Enable();
	//obj->GetComponent<PhysicsComponent>()->rigidBody.AddForce(100);
}