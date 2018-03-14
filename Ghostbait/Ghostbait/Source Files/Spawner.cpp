#include "Spawner.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "PhysicsComponent.h"
#include "EnemyBase.h"
#include "ObjectFactory.h"

#include "HexGrid.h"
void SpawnerObject::Awake(Object* obj) {
	SetTag("Spawner");
	MessageEvents::SendMessage(EVENT_SpawnerCreated, SpawnerCreatedMessage(this));
}
SpawnerObject::SpawnerObject() {
}
void SpawnerObject::SpawnObject(const char* prefabName, HexGrid* grid, const DirectX::XMFLOAT2& goal) {
	grid->RemoveObstacle(goal);
	grid->RemoveObstacle({ transform.GetMatrix()._41,transform.GetMatrix()._43});

	EnemyBase* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<EnemyBase>(ObjectFactory::CreatePrefab(&std::string(prefabName)), {0, 0, 0}, &obj));
	obj->transform.SetMatrix(transform.GetMatrix());
	obj->SetGrid(grid);
	obj->SetGoal(goal);
	obj->Repath();
	obj->Enable();
	//obj->GetComponent<PhysicsComponent>()->rigidBody.AddForce(100);
}
