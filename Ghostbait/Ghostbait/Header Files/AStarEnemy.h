#pragma once
#include "EnemyBase.h"
class RigidBody;

namespace DirectX { struct XMFLOAT2; }
class HexGrid;
class AStarEnemy :public EnemyBase {
	HexGrid* grid;
	RigidBody* rb;
	size_t howFarAlong =0;

	std::vector<DirectX::XMFLOAT2> path;

public:
	AStarEnemy();

	void SetGrid(HexGrid* _grid);

	void CalcPath(DirectX::XMFLOAT2 where);


	void Awake();
	void Update();
};
