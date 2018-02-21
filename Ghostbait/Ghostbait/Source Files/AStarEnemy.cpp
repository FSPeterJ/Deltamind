#include "AStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"


AStarEnemy::AStarEnemy() {}

void AStarEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}

void AStarEnemy::Awake() {
	rb = &(GetComponent<PhysicsComponent>()->rigidBody);
	CalcPath({10, 11});
	next = path.start();
}

void AStarEnemy::CalcPath(DirectX::XMFLOAT2 where) {
	path.clear();
	howFarAlong = 0;
	path = grid->AStarSearch(DirectX::XMFLOAT2(position._41, position._43), where, Heuristics::ManhattanDistance);
}

void AStarEnemy::Update() {
	if(KeyIsHit(Control::TestInputO)) start = true;

	if(!start) return;


	if(!path.size()) {
		CalcPath({10, 10});
		return;
	}


	HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(position._41, position._43));
	if(curTile) {

		if(curTile == next) {


			if(path.goal() == curTile) {
				Console::WriteLine << "We made it to our goal.";
			} else {
				howFarAlong++;
				if(howFarAlong > path.size() - 1) { return; }
				next = path[howFarAlong];
				auto nextPathPoint = grid->TileToPoint(next);
				
				position._41 = nextPathPoint.x;
				position._43 = nextPathPoint.y;
				//rb->AddForce(2.0f, nextPathPoint.x - position._41, 0.0f, nextPathPoint.y - position._43, 0.5f);
			}
		}



	}
}
