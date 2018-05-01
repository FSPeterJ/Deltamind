#include "SSLogo.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"

void SSLogo::Awake(Object* obj) {
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/SS_LogoText.ghost")), { 0, 0, 0 }, &text));
}
void SSLogo::Update() {
	if(text) text->transform.SetMatrix(transform.GetMatrix());
}

void SSLogo::Destroy() {
	text->Destroy();
	text = nullptr;
	GameObject::Destroy();
}