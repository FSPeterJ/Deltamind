#include "Monitor.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "TextManager.h"
#include "GameData.h"
#include "Core.h"
#include "GhostTime.h"
#undef SendMessage

void Monitor::Awake(Object* obj) {
	screen = nullptr;
	font = "Assets/Fonts/defaultFont.png";
	positioned = false;
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/MonitorScreen.ghost")), { 0, 0, 0 }, &screen));
	screenMat = TextManager::DrawTextTo(font, "\n Core Health: 100%\n", { 1, 1, 1, 1 }, { 0, 0, 0, 1 }).mat;
	screen->SetComponent<Material>(screenMat);

	eventCoreDamaged = MessageEvents::Subscribe(EVENT_CoreDamaged, [=](EventMessageBase* e) {
		std::string health = std::to_string((int)((*((CoreMessage*)e)->RetrieveData())->PercentHealth() * 100));
		WriteToScreen("\n Core Health: " + health + "%\n");
	});
	eventLose = MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {
		WriteToScreen("\nYOU LOSE!\n");
	});
	eventWin = MessageEvents::Subscribe(EVENT_GameWin, [=](EventMessageBase* e) {
		WriteToScreen("\nYOU WIN!\n");
	});
}
void Monitor::Update() {
	GameObject::Update();
	if (!positioned) {
		if (screen) screen->transform.SetMatrix(transform.GetMatrix());
		positioned = true;
	}
	WriteToScreen(std::to_string(GhostTime::FrameRate()));
}
void Monitor::Destroy() {
	if (screen) screen->Destroy(); 
	if (eventCoreDamaged) MessageEvents::UnSubscribe(EVENT_CoreDamaged, eventCoreDamaged);
	if (eventWin) MessageEvents::UnSubscribe(EVENT_GameWin, eventWin);
	if (eventLose) MessageEvents::UnSubscribe(EVENT_GameLose, eventLose);

	GameObject::Destroy();
}

void Monitor::WriteToScreen(const std::string text, const DirectX::XMFLOAT4 foreground, const DirectX::XMFLOAT4 background) {
	if (screen) {
		TextManager::DrawTextExistingMat(font, text, screenMat, foreground, background);
	}
}