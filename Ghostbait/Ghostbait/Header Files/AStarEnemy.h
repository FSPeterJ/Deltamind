#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "HexTileVector.h"
#include "Controlable.h"
class RigidBody;

namespace DirectX { struct XMFLOAT2; }
class HexGrid;
class AStarEnemy:public EnemyBase,public Controlable {
	HexGrid* grid;
	RigidBody* rb;
	size_t howFarAlong = 0;

	HexPath path;
	HexTile* goal = 0;
	HexTile* next = 0;

	bool start = false;
public:
	AStarEnemy();

	void NewPath();

	void SetGrid(HexGrid* _grid);

	void CalcPath(DirectX::XMFLOAT2 where);
	void CalcPath(HexTile* where);

	void Awake();
	void Update();
};
