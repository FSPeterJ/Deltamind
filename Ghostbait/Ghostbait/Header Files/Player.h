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
		STANCE_Crouch,
		STANCE_Stand,
		STANCE_God,
	};
	std::vector<unsigned> ownedItems;
	//void LoadInventory(const char* fileName = INVENTORY_FILE);

	float rotationY = 0.0f;
	float rotationX = 0.0f;
	float rotationLimit = DirectX::XMConvertToRadians(80);
	HexGrid* grid = nullptr;

	#pragma region Stats
	Stance stance = STANCE_Stand; 
	float standHeight = 1.7f;
	float crouchHeight = 1;
	
	float crouchSpeed = 4;
	float walkSpeed = 10;
	float runSpeed = 20;
	float godSpeed = 40;
	
	GameObject* editItem = nullptr;
	DirectX::XMFLOAT3 editScale;
	DirectX::XMFLOAT3 editRotation;
	DirectX::XMFLOAT3 editPos;
	float editMoveSpeed = 1;
	float editScaleSpeed = 1;
	float editRotationSpeed = 1;

	float playerHeight = standHeight;
	float playerSpeed = walkSpeed;
#pragma endregion

	void ChangeStance(Stance newStance);
	void GodDetected();
	void FreeMoney();
public:
	CastObject teleportArc;
	ControllerObject *leftController = 0, *rightController = 0;


	Player();

	void Update() override;
	void PausedUpdate();
	inline const float PlayerHeight() const { return playerHeight; }
	inline const float PlayerSpeed() const { return playerSpeed; }
	inline void ResetStance() { stance = STANCE_Stand; }
	void LoadControllers(VRControllerTypes type = CONTROLLER_Full);
	void SetBuildToolData(HexGrid* _grid, GameData* _gameData);
	inline HexGrid* GetBuildGrid() { return grid; }
	inline const bool IsGod() const { return stance == STANCE_God; }

	void Teleport(DirectX::XMFLOAT3* pos = nullptr);

	bool IsVR() const;
};