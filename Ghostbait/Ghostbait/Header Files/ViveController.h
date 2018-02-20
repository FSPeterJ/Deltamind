#pragma once
#include "Item.h"

class ViveController: public Item {
public:
	ViveController();
	void ActiveUpdate() override;
	void InactiveUpdate() override;
	void CloneData(Object* obj);
};
