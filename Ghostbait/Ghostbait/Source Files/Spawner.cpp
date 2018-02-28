#include "Spawner.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"
#include "EnemyBase.h"

#include "HexGrid.h"
void Spawner::Awake(Object* obj) {
	SetTag("Spawner");
	MessageEvents::SendMessage(EVENT_SpawnerCreated, SpawnerCreatedMessage(this));
}
Spawner::Spawner() {
}
void Spawner::SpawnObject(char* prefabName, HexGrid* grid, DirectX::XMFLOAT2 goal) {
	grid->RemoveObstacle(goal);
	grid->RemoveObstacle({ GetPosition()._41,GetPosition()._43});

	EnemyBase* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByName_DEBUG_ONLY, InstantiateNameMessage<EnemyBase>(prefabName, {0, 0, 0}, &obj));
	obj->SetPosition(GetPosition());
	obj->SetGrid(grid);
	obj->SetGoal(goal);
	obj->Repath();
	obj->Enable();
	//obj->GetComponent<PhysicsComponent>()->rigidBody.AddForce(100);
}
