#pragma once
#include "ControllerObject.h"

#define INVENTORY_FILE "Save Files\\Inventory.save"

class InputManager;

enum VRControllerTypes {
	CONTROLLER_Full,
	CONTROLLER_ModelsOnly,
};


class Player : public GameObject {
	struct Hand {
		int inventory[CONTROLLER_MAX_ITEMS] = { -1 };
		ControllerObject* controller = nullptr;
	};
	Hand left, right;
	
	std::vector<unsigned> ownedItems;
	InputManager* im;
	
	void LoadInventory(const char* fileName = INVENTORY_FILE);

public:
	Player(InputManager* _im);

	void Update() override;
	
	void LoadControllers(VRControllerTypes type = CONTROLLER_Full);
};