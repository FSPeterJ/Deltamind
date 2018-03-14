#pragma once
#include "ControllerObject.h"
#include "Camera.h"




#include "PhysicsExtension.h"




#define INVENTORY_FILE "Save Files\\Inventory.save"

class InputManager;
class HexGrid;

enum VRControllerTypes {
	CONTROLLER_Full,
	CONTROLLER_ModelsOnly,
};

class Player : public GameObject, public Controlable {
	enum Stance {
		STANCE_Stand,
		STANCE_Crouch,
	};
	std::vector<unsigned> ownedItems;

	//void LoadInventory(const char* fileName = INVENTORY_FILE);

	float rotationY = 0.0f;
	float rotationX = 0.0f;
	HexGrid* grid = nullptr;
	bool godMode = false;
	Stance stance = STANCE_Stand;

	float standHeight = 1.7f;
	float crouchHeight = 1;
public:
	ArcObject teleportArc;
	ControllerObject *leftController = 0, *rightController = 0;

	float playerHeight = 1.7f;

	Player();

	void Update() override;
	void PausedUpdate();
	void MenuUpdate();
	
	inline void ResetStance() { stance = STANCE_Stand; }
	void LoadControllers(VRControllerTypes type = CONTROLLER_Full);
	void SetBuildToolData(HexGrid* _grid, unsigned* _gears);
	inline HexGrid* GetBuildGrid() { return grid; }

	void Teleport(DirectX::XMFLOAT3* pos = nullptr);

	bool IsVR() const;
};