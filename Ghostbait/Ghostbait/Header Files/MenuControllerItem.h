#pragma once
#include "Item.h"

class MenuControllerItem : public Item {
	GameObject* currentlySelectedItem = nullptr;
public:
	MenuControllerItem();

	void UpdateRay();
	void Activate();
};