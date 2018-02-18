#include "AStarEnemy.h"
#include "HexGrid.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"

AStarEnemy::AStarEnemy() {}

void AStarEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}


void AStarEnemy::Awake() {
	rb = &(GetComponent<PhysicsComponent>()->rigidBody);
	CalcPath({10, 11});
}


void AStarEnemy::CalcPath(DirectX::XMFLOAT2 where) {
	path.clear();
	howFarAlong = 0;
	path = grid->AStarSearch(DirectX::XMFLOAT2(position._41, position._43), where, Heuristics::ManhattanDistance);
}


void AStarEnemy::Update() {
	if(!path.size()) {
		CalcPath({10, 11});
		return;
	}

	if(howFarAlong > path.size() - 1) {
		return;
	}

	DirectX::XMFLOAT2 curTile = grid->TileToWorld(DirectX::XMFLOAT2(position._41, position._43));


	if(path[howFarAlong+1].x != curTile.x && path[howFarAlong+1].y != curTile.y) {

	} else {
		rb->AddForce(2.0f, path[howFarAlong].x - position._41, 0.0f, path[howFarAlong++].y - position._43);
	}
}
