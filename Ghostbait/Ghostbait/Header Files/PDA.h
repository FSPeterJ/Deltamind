#pragma once
#include "Item.h"
#include "VRStructs.h"

struct Material;

class PDA : public Item {
public:
	enum Purpose {
		DisplayItem,
		InventoryItem,
		Credits,
	};
private:
	ControllerHand hand = HAND_Invalid;
	float distance = 0.11f;
	GameObject* display = nullptr;
	DirectX::XMFLOAT4 foreground = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1);
	DirectX::XMFLOAT4 background = DirectX::XMFLOAT4(40.0f /255.0f, 173.0f /255.0f, 174.0f/255.0f, 1);
	int coreHealth;
	int curWave;
	int totalWaves;
	int totalEnemies = 0;
	bool willUpdateEnemies = false;

	Material* displayMat = nullptr;
	Purpose purpose = DisplayItem;


	unsigned eventWaveChange = 0;
	unsigned eventCoreDamaged = 0;
	unsigned eventEnemyDied = 0;
	unsigned eventReset = 0;
	unsigned eventComplete = 0;
public:
	
	PDA();

	void Awake(Object* obj) override;
	void ActiveUpdate() override;
	void Destroy() override;

	void Selected() override;
	void DeSelected() override;

	void UpdateWaveData(GameDataMessage* e);
	void UpdateEnemyData();
	void UpdateCoreData(CoreMessage* e);

	void ResetDisplay(EventMessageBase* e);

	void WriteData();
	void SetHand(ControllerHand _hand) { hand = _hand; }
	void SetPurpose(Purpose _purpose);
};