#include "Monitor.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "TextManager.h"
#undef SendMessage

void Monitor::Awake(Object* obj) {
	screen = nullptr;
	font = "Assets/Fonts/defaultFont.png";
	positioned = false;
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/MonitorScreen.ghost")), { 0, 0, 0 }, &screen));
	screen->SetComponent<Material>(TextManager::DrawTextTo(font, "test", { 1, 1, 1, 1 }, { 0, 0, 0, 1 }).mat);
}
void Monitor::Update() {
	GameObject::Update();
	if (!positioned) {
		if (screen) screen->transform.SetMatrix(transform.GetMatrix());
		WriteToScreen("HEY!");
		positioned = true;
	}
}

void Monitor::WriteToScreen(const std::string text, const DirectX::XMFLOAT4 foreground, const DirectX::XMFLOAT4 background) {
	if (screen) {
		TextManager::DrawTextExistingMat(font, text, screen->GetComponent<Material>(), foreground, background);
	}
}