#include "ObjectManager.h"
#include "MessageEvents.h"
#include "GameObject.h"

ObjectManager::ObjectManager(MemoryManager* _memMan) : memMan(_memMan) {
	//TODO: Fix this into Initialize
	poolListCount = 80;
	poolList = (_Pool_Base*) memMan->RequestMemory(poolListCount, sizeof(Pool<size_t>));
	//objectpool.reserve(prefabCount);

	//DISABLED - USE PER OBJECT NOW
	//EngineStructure::Update.Add([=]() {this->Update(); });
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

GameObject* ObjectManager::Instantiate(unsigned typeID) {
	//Write(typeID);
	_Pool_Base* poolBase = (_Pool_Base*) ((char*) poolList + sizeof(Pool<size_t>) * typeID);
	GameObject* newobject = (GameObject*) poolBase->ActivateMemory();
	pointers2Bucket[newobject] = poolBase;
	return newobject;
}

void ObjectManager::Destroy(EventMessageBase *e) {
	StandardObjectMessage* destroy = (StandardObjectMessage*)e;
	Object* o = destroy->RetrieveObject();
	auto it = pointers2Bucket.find(o);
	if (it != pointers2Bucket.end()) {
	pointers2Bucket.find(o)->second->DeactivateMemory((char*)o);
}
}
