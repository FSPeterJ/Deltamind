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
	GameObject::Awake(obj);
}
SpawnerObject::SpawnerObject() {
}
EnemyBase* SpawnerObject::SpawnObject(const char* prefabName, HexGrid* grid, Core* _core) {
	EnemyBase* obj;
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<EnemyBase>(ObjectFactory::CreatePrefab(&std::string(prefabName)), {0, 0, 0}, &obj));
	obj->transform.SetMatrix(transform.GetMatrix());
	obj->SetGrid(grid);
	obj->SetCore(_core);
	obj->Repath();
	obj->Enable();
	//obj->GetComponent<PhysicsComponent>()->rigidBody.AddForce(100);

	return obj;
}

void SpawnerObject::Destroy() {
	GameObject::Destroy();
}
