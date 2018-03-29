#include "Monitor.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "TextManager.h"
#include "GameData.h"
#include "Core.h"
#undef SendMessage

void Monitor::Awake(Object* obj) {
	screen = nullptr;
	font = "Assets/Fonts/defaultFont.png";
	positioned = false;
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/MonitorScreen.ghost")), { 0, 0, 0 }, &screen));
	screen->SetComponent<Material>(TextManager::DrawTextTo(font, "Shoot the white\ncube to begin", { 1, 1, 1, 1 }, { 0, 0, 0, 1 }).mat);

	MessageEvents::Subscribe(EVENT_WaveChange, [=](EventMessageBase* e) {
		curWave = std::to_string((*((GameDataMessage*)e)->RetrieveData())->waveManager.GetCurrentWaveNumber());
		totalWaves = std::to_string((*((GameDataMessage*)e)->RetrieveData())->waveManager.GetWaveCount());
		WriteToScreen("\n Wave: " + curWave + "/" + totalWaves + "\n");
	});
	MessageEvents::Subscribe(EVENT_WaveComplete, [=](EventMessageBase* e) {
		curWave = std::to_string((*((GameDataMessage*)e)->RetrieveData())->waveManager.GetCurrentWaveNumber() + 1);
		WriteToScreen("\n Shoot the white cube\nto begin wave " + curWave + "\n");
	});
	MessageEvents::Subscribe(EVENT_CoreDamaged, [=](EventMessageBase* e) {
		std::string health = std::to_string((int)((*((CoreMessage*)e)->RetrieveData())->PercentHealth() * 100));
		WriteToScreen("\n Core Health: " + health + "%\n");
	});
	MessageEvents::Subscribe(EVENT_CoreStopDamaged, [=](EventMessageBase* e) {
		WriteToScreen("\n Wave: " + curWave + "/" + totalWaves + "\n");
	});
	MessageEvents::Subscribe(EVENT_GameLose, [=](EventMessageBase* e) {
		WriteToScreen("\nYOU LOSE!\n");
	});
	MessageEvents::Subscribe(EVENT_GameWin, [=](EventMessageBase* e) {
		WriteToScreen("\nYOU WIN!\n");
	});
}
void Monitor::Update() {
	GameObject::Update();
	if (!positioned) {
		if (screen) screen->transform.SetMatrix(transform.GetMatrix());
		positioned = true;
	}
}

void Monitor::WriteToScreen(const std::string text, const DirectX::XMFLOAT4 foreground, const DirectX::XMFLOAT4 background) {
	if (screen) {
		TextManager::DrawTextExistingMat(font, text, screen->GetComponent<Material>(), foreground, background);
	}
}