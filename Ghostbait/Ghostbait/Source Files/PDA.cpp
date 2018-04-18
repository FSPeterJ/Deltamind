#include "PDA.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "Material.h"
#include "TextManager.h"
#include "MeshManager.h"
#include "GameData.h"
#include "ScrollingUVManager.h"
#undef SendMessage

PDA::PDA() {
	SetTag("PDA");
}

void PDA::Awake(Object* obj) {
	Item::Awake(obj);
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
	if (eventEnemyDied) MessageEvents::UnSubscribe(EVENT_EnemyDied, eventEnemyDied);

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
			SwapComponentVarient<Mesh>("inHand");
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/PDAScreen.ghost")), { 0, 0, 0 }, &display));
			displayMat = TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", " Wave: 0 \n EnemiesRemaining: 0 ", foreground, background).mat;
			display->SetComponent<Material>(displayMat);
			display->PersistOnReset();
			display->ToggleFlag(UNLIT);
			eventWaveChange = MessageEvents::Subscribe(EVENT_WaveChange, [=](EventMessageBase* e) { if (display) UpdateDisplay(e); });
			eventEnemyDied = MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) { if (display) UpdateDisplay(); });
			
			comp = display->GetComponent<ScrollingUV>();
			if (comp) {
				comp->velocity.x = 0;
				comp->velocity.y = 0;
			}

			break;
		case Purpose::DisplayItem:
			break;
		case Purpose::Credits:
			SwapComponentVarient<Mesh>("large");
			MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/PDAScreen.ghost")), { 0, 0, 0 }, &display));
			displayMat = TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", " Wave: 0 \n EnemiesRemaining: 0 ", foreground, background).mat;
			display->SetComponent<Material>(displayMat);
			display->SwapComponentVarient<Mesh>("large");
			display->PersistOnReset();
			display->ToggleFlag(UNLIT);
			display->transform.SetMatrix(transform.GetMatrix());

			if (eventWaveChange) MessageEvents::UnSubscribe(EVENT_WaveChange, eventWaveChange);
			if (eventEnemyDied) MessageEvents::UnSubscribe(EVENT_EnemyDied, eventEnemyDied);

			comp = display->GetComponent<ScrollingUV>();
			if (comp) {
				comp->offset.y = 0;
				comp->velocity.y = 0.02f;
			}
			break;
	}
}
void PDA::UpdateDisplay(EventMessageBase* e) {
	//Get new data
	if (e) {
		const GameData* gameData = *((GameDataMessage*)e)->RetrieveData();
		if (gameData) {
			totalEnemies = gameData->waveManager.GetCurrentWave()->enemyCount;
			curWave = gameData->waveManager.GetCurrentWaveNumber();
			totalWaves = gameData->waveManager.GetWaveCount();
		}
	}
	else {
		totalEnemies -= 1;
	}
	//Write to file
	std::string message = " Wave: ";
	message.append(std::to_string(curWave) + " / " + std::to_string(totalWaves));
	message.append(" \n Enemies Remaining: ");
	message.append(std::to_string(totalEnemies));
	message.append(" ");
	TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", message, displayMat, foreground, background);
}