#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "HexTileVector.h"
#include "Controlable.h"
class RigidBody;
class Transform;

namespace DirectX { struct XMFLOAT2; struct XMFLOAT4X4; }
class HexGrid;
class MTDSLEnemy : public EnemyBase, public Controlable {
	HexGrid* grid;
	RigidBody* rb;

	std::size_t mtdstarId = 0;

	DirectX::XMFLOAT4X4* goalReference = nullptr;
	HexTile* goal = nullptr;
	HexTile* next = nullptr;
	HexTile* curTile = nullptr;

	unsigned eventAdd;
	unsigned eventRemove;

public:
	void SetGrid(HexGrid* _grid) override;

	void SetGoal(HexTile* _goal);
	void SetGoal(DirectX::XMFLOAT2 _goal) override;
	void SetGoalReference(DirectX::XMFLOAT4X4* _goal);

	void Repath() override {};
	void Enable(bool _destroyOnReset = false);
	void Disable();

	void Awake(Object* obj);
	void Start();
	void Update();
};
#pragma once
