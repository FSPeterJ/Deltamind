#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "HexTileVector.h"
#include "Controlable.h"
class RigidBody;

namespace DirectX { struct XMFLOAT2; }
class HexGrid;
class DStarEnemy: public EnemyBase, public Controlable  {
	HexGrid* grid;
	RigidBody* rb;

	std::size_t dstarId = 0;
	HexTile* goal = nullptr;
	HexTile* next = nullptr;
	HexTile* curTile = nullptr;

	unsigned eventAdd;
	unsigned eventRemove;

public:
	void SetGrid(HexGrid* _grid) override;
	void SetCore(Core* _core) override;
	void SetGoal(HexTile* _goal);
	void SetGoal(DirectX::XMFLOAT2 _goal);

	void Repath() override {};
	void Enable() override;
	void Disable() override;
	void Subscribe() override;
	void UnSubscribe() override;
	void Destroy() override;
	void Awake(Object* obj);
	void Attack();
	void Update();

};
