#include "Door.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"

void Door::Awake(Object* obj) {
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/Wall.ghost")), { 0, 0, 0 }, &wall));
	wall->UnRender();
	wall->RenderTransparent();
	first = true;
	GameObject::Awake(obj);
}

void Door::Update() {
	if (first) {
		wall->transform.SetMatrix(transform.GetMatrix());
		first = false;
	}
}