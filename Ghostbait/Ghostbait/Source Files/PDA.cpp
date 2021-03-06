#include "PDA.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "Material.h"
#include "TextManager.h"
#include "MeshManager.h"
#include "GameData.h"
#include "ScrollingUVManager.h"
#include "Core.h"
#undef SendMessage

PDA::PDA() {
	SetTag("PDA");
}

void PDA::Awake(Object* obj) {
	Item::Awake(obj);
	state = Item::State::PDA;
	eventWaveChange = 0;
	eventCoreDamaged = 0;
	eventEnemyDied = 0;
	eventReset = 0;
	eventComplete = 0;
	willUpdateEnemies = false;
}
void PDA::ActiveUpdate() {
	Item::ActiveUpdate();

	//Update PDA Location
	transform.NormalizeAllAxis();
	float dist = (hand == HAND_Left ? distance : -distance);
	DirectX::XMFLOAT4X4 mat = transform.GetMatrix();
	mat._41 += (mat._11 * dist);
	mat._42 += (mat._12 * dist);
	mat._43 += (mat._13 * dist);
	transform.SetMatrix(mat);

	if (display)
		display->transform.SetMatrix(transform.GetMatrix());
}
void PDA::Destroy() {
	if (display) {
		display->Destroy();
		display = nullptr;
	}
	
	if (eventWaveChange) MessageEvents::UnSubscribe(EVENT_WaveChange, eventWaveChange);
	if (eventCoreDamaged) MessageEvents::UnSubscribe(EVENT_CoreDamaged, eventCoreDamaged);
	if (eventEnemyDied) MessageEvents::UnSubscribe(EVENT_EnemyDied, eventEnemyDied);
	if (eventReset) MessageEvents::UnSubscribe(EVENT_ReadyToStart, eventReset);
	if (eventComplete) MessageEvents::UnSubscribe(EVENT_WaveComplete, eventComplete);
	Item::Destroy();
}
void PDA::Selected() {
	Item::Selected();
	if (display) display->Render();
}
void PDA::DeSelected() {
	Item::DeSelected();
	if (display) display->UnRender();
}

void PDA::SetPurpose(Purpose _purpose) {
	purpose = _purpose;
	ScrollingUV* comp = nullptr;
	switch (purpose) {
		case Purpose::InventoryItem:
			SwapComponentVariant<Mesh>("inHand");
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/PDAScreen.ghost")), { 0, 0, 0 }, &display));
			displayMat = TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", " Wave: 0 \n EnemiesRemaining: 0 ", foreground, background).mat;
			display->SetComponent<Material>(displayMat);
			display->PersistOnReset();
			display->ToggleFlag(UNLIT);
			eventWaveChange = MessageEvents::Subscribe(EVENT_WaveChange, [=](EventMessageBase* e) { if (display) UpdateWaveData((GameDataMessage*)e); });
			eventEnemyDied = MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) { if (display) UpdateEnemyData(); });
			eventReset = MessageEvents::Subscribe(EVENT_ReadyToStart, [=](EventMessageBase* e) {if (display) ResetDisplay(e); });
			eventComplete = MessageEvents::Subscribe(EVENT_WaveComplete, [=](EventMessageBase* e) {if (display) curWave += 1; });
			eventCoreDamaged = MessageEvents::Subscribe(EVENT_CoreDamaged, [=](EventMessageBase* e) {if (display) UpdateCoreData((CoreMessage*)e); });
			comp = display->GetComponent<ScrollingUV>();
			if (comp) {
				comp->velocity.x = 0;
				comp->velocity.y = 0;
				comp->offset.x = 0;
				comp->offset.y = 0;
			}

			break;
		case Purpose::DisplayItem:
			break;
		case Purpose::Credits:
			SwapComponentVariant<Mesh>("large");
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/PDAScreen.ghost")), { 0, 0, 0 }, &display));
			display->SwapComponentVariant<Mesh>("large");
			display->SwapComponentVariant<Material>("Large");
			display->PersistOnReset();
			display->ToggleFlag(UNLIT);
			display->transform.SetMatrix(transform.GetMatrix());

			if (eventWaveChange) MessageEvents::UnSubscribe(EVENT_WaveChange, eventWaveChange);
			if (eventEnemyDied) MessageEvents::UnSubscribe(EVENT_EnemyDied, eventEnemyDied);
			if (eventReset) MessageEvents::UnSubscribe(EVENT_ReadyToStart, eventReset);
			if (eventComplete) MessageEvents::UnSubscribe(EVENT_WaveComplete, eventComplete);
			comp = display->GetComponent<ScrollingUV>();
			if (comp) {
				comp->offset.y = 0;
				comp->velocity.y = 0.015f;
			}
			break;
	}
}

void PDA::UpdateWaveData(GameDataMessage* e) {
	const GameData* gameData = *(e->RetrieveData());
	if (gameData) {
		totalEnemies = gameData->waveManager.GetCurrentWave()->enemyCount;
		curWave = gameData->waveManager.GetCurrentWaveNumber();
		totalWaves = gameData->waveManager.GetWaveCount();
		willUpdateEnemies = true;
	}
	WriteData();
}
void PDA::UpdateEnemyData() {
	if (willUpdateEnemies)
		totalEnemies -= 1;
	WriteData();
}
void PDA::UpdateCoreData(CoreMessage* e) {
	coreHealth = (int)((*(e->RetrieveData()))->PercentHealth() * 100);
	WriteData();
}

void PDA::ResetDisplay(EventMessageBase * e)
{
	const GameData* gameData = *((GameDataMessage*)e)->RetrieveData();
	curWave = 1;
	totalEnemies = 0;
	totalWaves = gameData->waveManager.GetWaveCount();
	coreHealth = 100;
	WriteData();
	willUpdateEnemies = false;
}

void PDA::WriteData()
{
	//Write to file
	std::string message = " Core Health: ";
	message.append(std::to_string(coreHealth) + "%");
	message.append(" \n Wave: ");
	message.append(std::to_string(curWave) + " / " + std::to_string(totalWaves));
	message.append(" \n Enemies Remaining: ");
	message.append(std::to_string(totalEnemies));
	message.append(" ");
	TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", message, displayMat, foreground, background);
}
