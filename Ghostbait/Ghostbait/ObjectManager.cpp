#include "ObjectManager.h"
#include "ObjectFactory.h"

#undef GetObject

std::vector<ObjectManager::Pool> ObjectManager::objectPool;
std::unordered_map<const Object *, ObjectManager::Pool*> ObjectManager::poolScope;

ObjectManager::ObjectManager() {
	
	
}

ObjectManager::~ObjectManager() {}

void ObjectManager::Initialize()
{
		MessageEvents::Subscribe(EVENT_Instantiate, Instantiate);
		MessageEvents::Subscribe(EVENT_Destroy, Destroy);
		objectPool.resize(ObjectFactory::GetPrefabCount());
}

void ObjectManager::Instantiate(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*) e;
	WriteLine("An object was instantiated with a prefab ID of " + std::to_string(instantiate->GetId()));

	PrefabId pid = instantiate->GetId();

	const Object * o = ObjectFactory::RequestPrefab(pid);

	Pool pool = objectPool[pid];

	if(pool.Activate(o)) {
		poolScope[o] = &pool;
	}
}

void ObjectManager::Destroy(EventMessageBase *e) {
	DestroyMessage* destroy = (DestroyMessage*) e;
	Object* o = destroy->GetObject();
	poolScope[o]->Deactivate(o);
}
