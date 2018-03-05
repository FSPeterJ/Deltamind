#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "HexTileVector.h"
#include "Controlable.h"
class RigidBody;

namespace DirectX { struct XMFLOAT2; }
class HexGrid;
class MTDSLEnemy : public EnemyBase, public Controlable {
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

	void SetGoal(HexTile* _goal);
	void SetGoal(DirectX::XMFLOAT2 _goal) override;

	void Repath() override {};
	void Enable(bool _destroyOnReset = false);
	void Disable();

	void Awake(Object* obj);
	void Start();
	void Update();
};
#pragma once
