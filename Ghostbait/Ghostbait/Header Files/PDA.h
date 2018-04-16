#pragma once
#include "Item.h"
#include "VRStructs.h"

struct Material;

class PDA : public Item {
	ControllerHand hand = HAND_Invalid;
	float distance = 0.11f;
	GameObject* waveCounter = nullptr;
	GameObject* enemyCounter = nullptr;
	DirectX::XMFLOAT4 foreground = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f);
	DirectX::XMFLOAT4 background = DirectX::XMFLOAT4(40.0f /255.0f, 173.0f /255.0f, 174.0f/255.0f, 1);
	int totalEnemies = 0;

	Material* waveMat = nullptr;
	Material* enemyMat = nullptr;

	unsigned eventWaveChange = 0;
	unsigned eventEnemyDied = 0;
public:
	PDA();

	void Awake(Object* obj) override;
	void ActiveUpdate() override;
	void Destroy() override;

	void Selected() override;
	void DeSelected() override;

	void SetHand(ControllerHand _hand) { hand = _hand; }
};