#include "DisplayBoard.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"

void DisplayBoard::Awake(Object* obj) {
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/Display_Lifter.ghost")), { 0, 0, 0 }, &lifter));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/DisplayBoard_Screen.ghost")), { 0, 0, 0 }, &screen));
}

void DisplayBoard::Update() {
	GameObject::Update();
	if (lifter) {
		lifter->transform.SetMatrix(transform.GetMatrix());
		screen->transform.SetMatrix(transform.GetMatrix());
	}
}

void DisplayBoard::Destroy() {
	if (lifter) lifter->Destroy(); 
	if (screen) screen->Destroy(); 
	GameObject::Destroy();
}