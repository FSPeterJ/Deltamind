#include "ObjectManager.h"
#include "ObjectFactory.h"

Pool<Object>* ObjectManager::objectpool;

ObjectManager::ObjectManager() {

}

ObjectManager::~ObjectManager() {}

void ObjectManager::Initialize()
{
	MessageEvents::Subscribe(EVENT_Destroy, Destroy);	
	objectpool = new Pool<Object>(MAX_ENTITY_COUNT);
	int x = 0;
}

void ObjectManager::Shutdown()
{
	//MessageEvents::Unsubscribe(EVENT_InstantiateRequest, Instantiate);
	//MessageEvents::Unsubscribe(EVENT_Destroy, Destroy);
	delete objectpool;
	int x = 0;
}


Object* ObjectManager::Instantiate(const Object* obj) {
	Object* newobject = objectpool->Activate(obj);
	return newobject;
}

void ObjectManager::Destroy(EventMessageBase *e) {
	DestroyMessage* destroy = (DestroyMessage*)e;
	Object* o = destroy->RetrieveObject();
	objectpool->Deactivate(o);
}
