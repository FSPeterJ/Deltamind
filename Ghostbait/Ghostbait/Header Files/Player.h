#pragma once
#include "ControllerObject.h"
#include "Camera.h"

#define INVENTORY_FILE "Save Files\\Inventory.save"

class InputManager;

enum VRControllerTypes {
	CONTROLLER_Full,
	CONTROLLER_ModelsOnly,
};

class Player : public GameObject, public Controlable {
	struct Hand {
		int inventory[CONTROLLER_MAX_ITEMS] = { -1 };
		ControllerObject* controller = nullptr;
	};
	Hand left, right;
	
	std::vector<unsigned> ownedItems;
	ControllerObject* leftController, *rightController;

	//Camera cam;

	//void LoadInventory(const char* fileName = INVENTORY_FILE);

public:
	Player();

	void Update() override;
	
	void LoadControllers(VRControllerTypes type = CONTROLLER_Full);

//	void SetPosition(DirectX::XMFLOAT4X4 newPos) override;
//	const DirectX::XMFLOAT4X4& GetPosition() const override;

	void Teleport();

	bool IsVR() const;
};