#pragma once
#include "Item.h"
#include "PhysicsExtension.h"

class MenuOption;

class MenuControllerItem : public Item {
	MenuOption* currentMenuItem = nullptr;
	CastObject ray;
public:
	MenuControllerItem();

	void UpdateRay();
	void Activate();

	void Render() override;
	void UnRender() override;

	void Awake(Object* obj) override;
};