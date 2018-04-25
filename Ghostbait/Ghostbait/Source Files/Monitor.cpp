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
	screenMat = TextManager::DrawTextTo(font, "\n Shoot the core's\n shield to begin \n", { 1, 1, 1, 1 }, { 0, 0, 0, 1 }).mat;
	screen->SetComponent<Material>(screenMat);
	coreHealth = 100;

	eventWaveChange = MessageEvents::Subscribe(EVENT_WaveChange, [=](EventMessageBase* e) {
		WriteToScreen("\n Enemies Detected!\n Protect the core! \n");
	});
	eventWaveComplete = MessageEvents::Subscribe(EVENT_WaveComplete, [=](EventMessageBase* e) {
		const GameData* gameData = *((GameDataMessage*)e)->RetrieveData();
		WriteToScreen("\n Shoot the core's\n shield to start wave " + std::to_string(gameData->waveManager.GetCurrentWaveNumber() + 1) + "\n");
	});
	eventLose = MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {
		WriteToScreen("\n  TOTAL SYSTEM FAILURE  \n     Restart to try again     ");
	});
	eventWin = MessageEvents::Subscribe(EVENT_GameWin, [=](EventMessageBase* e) {
		WriteToScreen("\n	All Threats Purged!	\n        Congradulations        ");
	});
}
void Monitor::Update() {
	GameObject::Update();
	if (!positioned) {
		if (screen) screen->transform.SetMatrix(transform.GetMatrix());
		positioned = true;
	}
	//WriteToScreen(std::to_string(GhostTime::FrameRate()));
}
void Monitor::Destroy() {
	if (screen) screen->Destroy(); 
	if (eventCoreDamaged) MessageEvents::UnSubscribe(EVENT_CoreDamaged, eventCoreDamaged);
	if (eventWin) MessageEvents::UnSubscribe(EVENT_GameWin, eventWin);
	if (eventLose) MessageEvents::UnSubscribe(EVENT_GameLose, eventLose);
	if (eventWaveComplete) MessageEvents::UnSubscribe(EVENT_WaveComplete, eventWaveComplete);
	if (eventWaveChange) MessageEvents::UnSubscribe(EVENT_WaveChange, eventWaveChange);

	GameObject::Destroy();
}

void Monitor::WriteToScreen(const std::string text, const DirectX::XMFLOAT4 foreground, const DirectX::XMFLOAT4 background) {
	if (screen) {
		TextManager::DrawTextExistingMat(font, text, screenMat, foreground, background);
	}
}