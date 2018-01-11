#include "ObjectManager.h"
#include "ObjectFactory.h"

#undef GetObject

std::vector<ObjectManager::Pool> ObjectManager::objectPool;
std::unordered_map<const Object *, ObjectManager::Pool*> ObjectManager::poolScope;

ObjectManager::ObjectManager() {
	MessageEvents::Subscribe(EVENT_Instantiate, Instantiate);
	MessageEvents::Subscribe(EVENT_Destroy, Destroy);

	//After factory has the list of prefab ids, ask it for them so the pool list can be made
}

ObjectManager::~ObjectManager() {}

void ObjectManager::Instantiate(EventMessageBase e) { //PrefabId pid
	InstantiateMessage* instantiate = (InstantiateMessage*) &e;
	PrefabId pid = instantiate->GetId();

	//TODO uncomment below once it exists
	const Object * o = ObjectFactory::RequestPrefab(pid);

	Pool pool = objectPool[pid];

	if(pool.Activate(o)) {
		poolScope[o] = &pool;
	}
}

void ObjectManager::Destroy(EventMessageBase e) {
	DestroyMessage* destroy = (DestroyMessage*) &e;
	Object* o = destroy->GetObject();
	poolScope[o]->Deactivate(o);
}
