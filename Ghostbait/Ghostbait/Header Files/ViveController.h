#pragma once
#include "Item.h"

class ViveController: public Item {
public:
	ViveController();
	void Awake(Object* obj) override;

	void ActiveUpdate() override;
	void InactiveUpdate() override;
};
