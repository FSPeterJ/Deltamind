#include "AStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"

AStarEnemy::AStarEnemy() {
	tag = std::string("Enemy");
}

//Main Overrides

void AStarEnemy::Awake(Object* obj) {
	EnemyBase::Awake(obj);
	rb = &(GetComponent<PhysicsComponent>()->rigidBody);

}

void AStarEnemy::Subscribe() {
	eventAdd = MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->Repath(); });
	eventRemove = MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->Repath(); });

}
void AStarEnemy::UnSubscribe() {
	MessageEvents::UnSubscribe(EVENT_AddObstacle, eventAdd);
	MessageEvents::UnSubscribe(EVENT_RemoveObstacle, eventRemove);
}
void AStarEnemy::Enable(bool _destroyOnReset) {
	if(!goal) {
		NewRandPath();
	}
	rb->SetTerminalSpeed(maxSpeed);
	next = path.start();
	if (!enabled) { // What does this do?
		AStarEnemy::Subscribe();
		EnemyBase::Enable(_destroyOnReset);
	}
	GameObject::Enable();
}
void AStarEnemy::Disable() {
	if (enabled) {
		AStarEnemy::UnSubscribe();
		EnemyBase::Disable();
	}
}
void AStarEnemy::Destroy() {
	EnemyBase::Destroy();
}
void AStarEnemy::Update() {
	EnemyBase::Update();
	HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(position._41, position._43));
	if(curTile) {
		if(curTile == next) {
			if(path.goal() == curTile) {
				Console::WriteLine << "We made it to our goal.";
				rb->Stop();
			}
			else {
				howFarAlong++;
				if(howFarAlong > path.size() - 1) { return; }
				next = path[howFarAlong];
				auto nextPathPoint = grid->TileToPoint(next);

				//position._41 = nextPathPoint.x;
				//position._43 = nextPathPoint.y;
				DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - position._41, 0.0f, nextPathPoint.y - position._43, 1.0f);
				DirectX::XMVECTOR velocity = rb->GetVelocity();
				rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - position._41, 0.0f, nextPathPoint.y - position._43, 0.5f);
			}
		}
	}
}

//Other Overrides
void AStarEnemy::Repath() {
	NewPath();
}
void AStarEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}
void AStarEnemy::SetGoal(DirectX::XMFLOAT2 _goal) {
	HexTile* goalTile = grid->PointToTile(_goal);
	if(goalTile) { goal = goalTile; }
}

//Other
void AStarEnemy::SetGoal(HexTile* _goal) {
	goal = _goal;
}
bool AStarEnemy::NewPath() {
	path.clear();
	howFarAlong = 0;

	CalcPath(goal);

	return path.size();
}
void AStarEnemy::NewRandPath() {
	path.clear();
	howFarAlong = 0;

	goal = grid->GetRandomTile();
	CalcPath(goal);

	if(!path.size()) {
		NewRandPath();
	}
}
void AStarEnemy::CalcPath(DirectX::XMFLOAT2 where) {
	HexTile* whereTile = grid->PointToTile(where);
	CalcPath(whereTile);
}
void AStarEnemy::CalcPath(HexTile* where) {
	HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(position._41, position._43));
	path = PathPlanner::FindPath(curTile, where, TileType::Static, TileType::Static);
}
