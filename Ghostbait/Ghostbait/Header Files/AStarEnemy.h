#pragma once
#include "HexGrid.h"
#include "EnemyBase.h"
#include "HexTileVector.h"
#include "Controlable.h"
#include "AntColony.h"
//class RigidBody;

namespace DirectX { struct XMFLOAT2; }
class HexGrid;
class AStarEnemy: public EnemyBase, public AntProperties {
	HexPath path;
	size_t howFarAlong = 0;
	HexGrid* grid = 0;
	//RigidBody* rb = 0;
	HexTile* goal = 0;
	HexTile* next = 0;


	bool start = false;
	unsigned eventAdd = 0;
	unsigned eventRemove = 0;

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



	void SetGoal(HexTile* _goal);
	void SetCore(Core* _core) override;
	void SetGrid(HexGrid* _grid) override;
	void Repath() override;

	void Attack();

	void Subscribe() override;
	void UnSubscribe() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void Update() override;
	void Awake(Object* obj);
};
