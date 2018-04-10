#pragma once
#include "Item.h"
#include "VRStructs.h"

class PDA : public Item {
	ControllerHand hand = HAND_Invalid;
	float distance = 0.11f;
public:
	PDA();

	void Awake(Object* obj) override;
	void ActiveUpdate() override;

	void SetHand(ControllerHand _hand) { hand = _hand; }
};