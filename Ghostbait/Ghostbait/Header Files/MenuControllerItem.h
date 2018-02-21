#pragma once
#include "Item.h"

class MenuOption;

class MenuControllerItem : public Item {
	MenuOption* currentMenuItem = nullptr;

public:
	MenuControllerItem();

	void UpdateRay();
	void Activate();
};