#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "HexTileVector.h"
#include "Controlable.h"
class RigidBody;

namespace DirectX { struct XMFLOAT2; }
class HexGrid;
class AStarEnemy:public EnemyBase, public Controlable {
	HexGrid* grid;
	RigidBody* rb;
	size_t howFarAlong = 0;

	HexPath path;
	HexTile* goal = 0;
	HexTile* next = 0;

	bool start = false;

	void CalcPath(DirectX::XMFLOAT2 where);
	void CalcPath(HexTile* where);

	/// <summary>
	/// Generates a new path.
	/// </summary>
	/// <returns>True if a path was found, otherwise false.</returns>
	bool NewPath();

	void NewRandPath();

public:
	AStarEnemy();

	void SetGrid(HexGrid* _grid);

	void SetGoal(HexTile* _goal);
	void SetGoal(DirectX::XMFLOAT2 _goal);

	void Repath();

	void Awake();
	void Update();
};
