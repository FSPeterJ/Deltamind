#include "ObjectManager.h"
#include "ObjectFactory.h"

std::vector<ObjectManager::PoolCluster> ObjectManager::objectPool;
std::unordered_map<const Object *, ObjectManager::PoolCluster*> ObjectManager::poolScope;

ObjectManager::ObjectManager() {

}

ObjectManager::~ObjectManager() {}

void ObjectManager::Initialize()
{
	MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
	MessageEvents::Subscribe(EVENT_Destroy, Destroy);
	objectPool.resize(ObjectFactory::GetPrefabCount());
	
}

void ObjectManager::Instantiate(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*) e;
	//WriteLine("An object was instantiated with a prefab ID of " + std::to_string(instantiate->GetId()));

	PrefabId pid = instantiate->GetId();

	const Object * o = ObjectFactory::RequestPrefab(pid);

	PoolCluster* poolcluster = &objectPool[pid];

	Object* newobject = poolcluster->ActivateObject(o);
	if(newobject) {
		poolScope[o] = poolcluster;
	}
	if(instantiate->GetReturnObject() != nullptr)
	{

		instantiate->SetReturnObject(newobject);
	}

	newobject->position.r[3] = XMLoadFloat4(&instantiate->GetPosition());
	MessageEvents::SendMessage(EVENT_Instantiated, NewObjectMessage(newobject));
}

void ObjectManager::Destroy(EventMessageBase *e) {
	DestroyMessage* destroy = (DestroyMessage*)e;
	Object* o = destroy->GetObject();
	poolScope[o]->DeactivateObject(o);
}
