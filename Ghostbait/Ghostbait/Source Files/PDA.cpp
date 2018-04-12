#include "PDA.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "Material.h"
#include "TextManager.h"
#include "MeshManager.h"
#include "GameData.h"
#undef SendMessage

PDA::PDA() {
	SetTag("PDA");
}

void PDA::Awake(Object* obj) {
	Item::Awake(obj);

	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/PDAWaveCounter.ghost")), { 0, 0, 0 }, &waveCounter));
	waveCounter->UnRender();
	waveCounter->RenderTransparent();
	waveCounter->SetComponent<Material>(TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "Wave: 0", foreground, background).mat);
	waveCounter->PersistOnReset();
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/PDAEnemyCounter.ghost")), { 0, 0, 0 }, &enemyCounter));
	enemyCounter->UnRender();
	enemyCounter->RenderTransparent();
	enemyCounter->SetComponent<Material>(TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "Enemies Remaining: 00", foreground, background).mat);
	enemyCounter->PersistOnReset();


	MessageEvents::Subscribe(EVENT_WaveChange, [=](EventMessageBase* e) {
		if (enemyCounter) {
			totalEnemies = (*((GameDataMessage*)e)->RetrieveData())->waveManager.GetCurrentWave()->enemyCount;
			std::string message = "Enemies Remaining: ";
			message.append(std::to_string(totalEnemies));
			TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", message, enemyCounter->GetComponent<Material>(), foreground, background);
		}
		if (waveCounter) {
			std::string curWave = std::to_string((*((GameDataMessage*)e)->RetrieveData())->waveManager.GetCurrentWaveNumber());
			std::string totalWaves = std::to_string((*((GameDataMessage*)e)->RetrieveData())->waveManager.GetWaveCount());
			std::string message = "Wave: ";
			message.append(curWave + " / " + totalWaves);
			TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", message, waveCounter->GetComponent<Material>(), foreground, background);
		}
	});
	MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) {
		if (enemyCounter) {
			--totalEnemies;
			std::string message = "Enemies Remaining: ";
			message.append(std::to_string(totalEnemies));
			TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", message, enemyCounter->GetComponent<Material>(), foreground, background);
		}
	});
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

	if (waveCounter) {
		waveCounter->transform.SetMatrix(transform.GetMatrix());
	}
	if (enemyCounter) {
		enemyCounter->transform.SetMatrix(transform.GetMatrix());
	}
}
void PDA::Destroy() {
	if (waveCounter) {
		waveCounter->Destroy();
		waveCounter = nullptr;
	}
	if (enemyCounter) {
		enemyCounter->Destroy();
		enemyCounter = nullptr;
	}
	Item::Destroy();
}

void PDA::Selected() {
	Item::Selected();
	if (waveCounter) waveCounter->Render();
	if (enemyCounter) enemyCounter->Render();
}
void PDA::DeSelected() {
	Item::DeSelected();
	if (waveCounter) waveCounter->UnRender();
	if (enemyCounter) enemyCounter->UnRender();
}