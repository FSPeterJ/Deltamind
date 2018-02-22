#include "Spawner.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"
#include "EnemyBase.h"

void Spawner::Awake() {
	SetTag("Spawner");
	MessageEvents::SendMessage(EVENT_SpawnerCreated, SpawnerCreatedMessage(this));
}
Spawner::Spawner() {
}
void Spawner::SpawnObject(char* prefabName, HexGrid* grid, DirectX::XMFLOAT2 goal) {
	EnemyBase* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<EnemyBase>(prefabName, {0, 0, 0}, &obj));
	obj->position = position;
	obj->SetGrid(grid);
	obj->SetGoal(goal);
	obj->Repath();
	obj->Enable();
	//obj->GetComponent<PhysicsComponent>()->rigidBody.AddForce(100);
}