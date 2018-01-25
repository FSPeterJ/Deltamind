#include "ObjectManager.h"
#include "ObjectFactory.h"
#include "EngineStructure.h"
#include "GameObject.h"

ObjectManager::ObjectManager(MemoryManager* _memMan) : memMan(_memMan) {
	//TODO: Fix this into Initialize
	poolListCount = 80;
	poolList = (_Pool_Base*) memMan->RequestMemory((unsigned int)poolListCount, sizeof(Pool<size_t>));
	//objectpool.reserve(prefabCount);

	EngineStructure::Update += [=]() {this->Update(); };
}

ObjectManager::~ObjectManager() {
	Delete();
}

void ObjectManager::Initialize(size_t prefabCount) {
	MessageEvents::Subscribe(EVENT_Destroy, [this](EventMessageBase * _e) {this->Destroy(_e); });
	//objectpool = new Pool<Object>(MAX_ENTITY_COUNT);
	int x = 0;
}

void ObjectManager::Shutdown() const {
	//MessageEvents::Unsubscribe(EVENT_InstantiateRequest, Instantiate);
	//MessageEvents::Unsubscribe(EVENT_Destroy, Destroy);
	//Delete();
	//int x = 0;
}

void ObjectManager::Update() {
	Update_Delegate();
}

Object* ObjectManager::Instantiate(unsigned typeID) {
	//Write(typeID);
	_Pool_Base* poolBase = (_Pool_Base*) ((char*) poolList + sizeof(Pool<size_t>) * typeID);
	Object* newobject = (Object*) poolBase->Activate();
	pointers2Bucket[newobject] = poolBase;
	return newobject;
}

void ObjectManager::Destroy(EventMessageBase *e) {
	DestroyMessage* destroy = (DestroyMessage*) e;
	Object* o = destroy->RetrieveObject();
	pointers2Bucket.find(o)->second->Deactivate((char*) o);
}
