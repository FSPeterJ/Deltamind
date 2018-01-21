#include "ObjectManager.h"
#include "ObjectFactory.h"

ObjectManager::ObjectManager(MemoryManager* _memMan, size_t prefabCount): memMan(_memMan)
{

	poolList = (_Pool_Base* )memMan->RequestMemory(prefabCount, sizeof(Pool<size_t>));
	poolListCount = prefabCount;
	//objectpool.reserve(prefabCount);
}

ObjectManager::~ObjectManager()
{
	Delete();

}

void ObjectManager::Initialize()
{

	MessageEvents::Subscribe(EVENT_Destroy, [this](EventMessageBase * _e) {this->Destroy(_e); });
	//objectpool = new Pool<Object>(MAX_ENTITY_COUNT);
	int x = 0;
}

void ObjectManager::Shutdown() const
{
	//MessageEvents::Unsubscribe(EVENT_InstantiateRequest, Instantiate);
	//MessageEvents::Unsubscribe(EVENT_Destroy, Destroy);
	Delete();
	int x = 0;
}




Object* ObjectManager::Instantiate(int typeID) {
	Object* newobject = (Object*)poolList[typeID].Activate();
	return newobject;
}

void ObjectManager::Destroy(EventMessageBase *e) {
	DestroyMessage* destroy = (DestroyMessage*)e;
	Object* o = destroy->RetrieveObject();
	pointers2Bucket.find(o)->second->Deactivate(o);
}
