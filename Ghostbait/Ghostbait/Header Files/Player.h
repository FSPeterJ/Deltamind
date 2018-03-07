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
	std::vector<unsigned> ownedItems;

	//void LoadInventory(const char* fileName = INVENTORY_FILE);

	float rotationY = 0.0f;
	float rotationX = 0.0f;

public:
	ControllerObject *leftController = 0, *rightController = 0;

	float playerHeight = 1.7f;
	Player();

	void Update() override;
	void PausedUpdate();
	void MenuUpdate();
	
	void LoadControllers(VRControllerTypes type = CONTROLLER_Full);

//	void SetPosition(DirectX::XMFLOAT4X4 newPos) override;
//	const DirectX::XMFLOAT4X4& GetPosition() const override;

	void Teleport();

	bool IsVR() const;
};