#include "DMLogo.h"
#include "GhostTime.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"

void DMLogo::Awake(Object* obj) {
	GameObject::Awake(obj);
	speed = 0.8f;
	rot = 0.0f;
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/DM_LogoText.ghost")), {0, 0, 0}, &text));
}
void DMLogo::Update() {
	if(!rot && text) text->transform.SetMatrix(transform.GetMatrix());
	rot += (speed * (float)GhostTime::DeltaTime());
	transform.SetRotationRadians(0, rot, rot);
}

void DMLogo::Destroy() {
	text->Destroy();
	text = nullptr;
	GameObject::Destroy();
}