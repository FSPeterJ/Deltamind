#include "ObjectManager.h"
#include "ObjectFactory.h"

Pool<Object>* ObjectManager::objectpool;

ObjectManager::ObjectManager() {

}

ObjectManager::~ObjectManager() {}

void ObjectManager::Initialize()
{
	MessageEvents::Subscribe(EVENT_InstantiateRequest, Instantiate);
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

void ObjectManager::Instantiate(EventMessageBase *e) {
	InstantiateMessage* instantiate = (InstantiateMessage*) e;
	//WriteLine("An object was instantiated with a prefab ID of " + std::to_string(instantiate->GetId()));

	PrefabId pid = instantiate->GetId();

	const Object * o = ObjectFactory::RequestPrefab(pid);


	Object* newobject = objectpool->Activate(o);

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
	objectpool->Deactivate(o);
}
