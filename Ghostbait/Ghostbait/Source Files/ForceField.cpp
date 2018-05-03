#include "ForceField.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"

ForceField::ForceField() {
	SetTag("Wall");
}

void ForceField::Awake(Object* obj) {
	GameObject::Awake(obj);
	first = true;
}

void ForceField::Update() {
	GameObject::Update();
	if (first) {
		UnRender();
		RenderTransparent();
		GameObject* backdrop = nullptr;
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ForceFieldBackdrop.ghost")), { 0, 0, 0 }, &backdrop));
		backdrop->transform.SetMatrix(transform.GetMatrix());
		first = false;
		UnRender();
		RenderTransparent();
	}
}

void ForceField::Destroy() {
	GameObject::Destroy();
}