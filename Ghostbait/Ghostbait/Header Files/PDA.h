#pragma once
#include "Item.h"
#include "VRStructs.h"

class PDA : public Item {
	ControllerHand hand = HAND_Invalid;
	float distance = 0.11f;
	GameObject* waveCounter = nullptr;
	GameObject* enemyCounter = nullptr;
	DirectX::XMFLOAT4 foreground = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.75f);
	DirectX::XMFLOAT4 background = DirectX::XMFLOAT4(0.15686274509803921568627450980392f, 0.67843137254901960784313725490196f, 0.68235294117647058823529411764706f, 1);
	int totalEnemies = 0;
public:
	PDA();

	void Awake(Object* obj) override;
	void ActiveUpdate() override;
	void Destroy() override;

	void Selected() override;
	void DeSelected() override;

	void SetHand(ControllerHand _hand) { hand = _hand; }
};