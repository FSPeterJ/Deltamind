#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "HexTileVector.h"
#include "Controlable.h"
#include "AntColony.h"
//class RigidBody;
//class Transform;

namespace DirectX { struct XMFLOAT2; struct XMFLOAT4X4; }
class HexGrid;
class MTDSLEnemy : public EnemyBase, public AntProperties {

protected:
	HexPath path;
	HexGrid* grid = 0;
	//RigidBody* rb = 0;
	HexTile* goal = 0;
	HexTile* next = 0;
	HexTile* curTile = 0;
	HexTile* lastTile = nullptr;

	bool isPathing = false;
	bool pathIsBlocked = false;
	std::future<void> pathing;
	bool start = false;
	unsigned eventAdd = 0;
	unsigned eventRemove = 0;

	std::size_t mtdstarId = 0;

public:
	void SetGrid(HexGrid* _grid) override;
	void SetCore(Core* _core) override;
	void SetGoal(HexTile* _goal);
	void SetGoal(DirectX::XMFLOAT2 _goal);

	//bool ReTarget(GameObject* _obj = nullptr) override;
	bool ValidateAttackTarget() override;
	void Repath() override {};

	void Enable() override;
	void Disable() override;
	void Subscribe() override;
	void UnSubscribe() override;
	void Destroy() override;
	void Awake(Object* obj) override;

	void Patrol() override;
};
