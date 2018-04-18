#include "ForceField.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"

ForceField::ForceField() {
	SetTag("SmokeWall");
}

void ForceField::Awake(Object* obj) {
	GameObject::Awake(obj);
	UnRender();
	RenderTransparent();
	first = true;
}

void ForceField::Update() {
	GameObject::Update();
	if (first) {
		GameObject* backdrop = nullptr;
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ForceFieldBackdrop.ghost")), { 0, 0, 0 }, &backdrop));
		backdrop->transform.SetMatrix(transform.GetMatrix());
		first = false;
	}
}

void ForceField::Destroy() {
	UnRender();
	GameObject::Destroy();
}