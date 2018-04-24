#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "Controlable.h"
#include "AntColony.h"
#include "HexTileVector.h"
#include <future>
//class RigidBody;

namespace DirectX { struct XMFLOAT2; }
class HexGrid;
class AStarEnemy: public EnemyBase, public AntProperties {
	HexPath path;
	HexGrid* grid = 0;
	//RigidBody* rb = 0;
	HexTile* goal = 0;
	HexTile* next = 0;
	HexTile* curTile = 0;
	
	bool isPathing = false;
	bool pathIsBlocked = false;
	std::future<void> pathing;
	bool start = false;
	unsigned eventAdd = 0;
	unsigned eventRemove = 0;

	void CalcPath(DirectX::XMFLOAT2& where);
	void CalcPath(HexTile* where);

	/// <summary>
	/// Generates a new path.
	/// </summary>
	/// <returns>True if a path was found, otherwise false.</returns>
	bool NewPath();

	bool NewAroundPath();

public:
	AStarEnemy();

	void SetGoal(HexTile* _goal);
	void SetCore(Core* _core) override;
	void SetGrid(HexGrid* _grid) override;
	void Repath() override;
	bool ReTarget(GameObject* _obj = nullptr) override;
	bool ValidateAttackTarget() override;
	
	//void Attack() override;
	void Patrol() override;

	void Subscribe() override;
	void UnSubscribe() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	//void Update() override;
	void Awake(Object* obj) override;
	void LastResort();
};
