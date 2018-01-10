#include "ObjectManager.h"


void MakeActive(EventMessageBase e) {
	InstantiateMessage* instantiate = (InstantiateMessage*) &e;
	//instantiate->GetId();
	//ObjectFactory::CreateObject(id);
}


ObjectManager::ObjectManager() {
	MessageEvents::Subscribe(EVENT_Instantiate, MakeActive);
}

ObjectManager::~ObjectManager() {}
