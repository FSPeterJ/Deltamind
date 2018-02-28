#include "DStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"

DStarEnemy::DStarEnemy() {
	tag = std::string("Enemy");
}

void DStarEnemy::Repath() {
	NewPath();
}

void DStarEnemy::Enable() {
	eventAdd = MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->Repath(); });
	eventRemove = MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->Repath(); });
	GameObject::Enable();
}

void DStarEnemy::Disable() {
	MessageEvents::UnSubscribe(EVENT_AddObstacle, eventAdd);
	MessageEvents::UnSubscribe(EVENT_RemoveObstacle, eventRemove);
	EnemyBase::Disable();
}

bool DStarEnemy::NewPath() {
	path.clear();
	howFarAlong = 0;

	CalcPath(goal);

	return path.size();
}

void DStarEnemy::NewRandPath() {
	path.clear();
	howFarAlong = 0;

	goal = grid->GetRandomTile();
	CalcPath(goal);

	if(!path.size()) {
		NewRandPath();
	}
}

void DStarEnemy::SetGoal(HexTile* _goal) {
	goal = _goal;
}
void DStarEnemy::SetGoal(DirectX::XMFLOAT2 _goal) {
	HexTile* goalTile = grid->PointToTile(_goal);
	if(goalTile) { goal = goalTile; }
}

void DStarEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}

void DStarEnemy::Awake() {
	EnemyBase::Awake();
	rb = &(GetComponent<PhysicsComponent>()->rigidBody);
	if(!goal) {
		NewRandPath();
	}
	next = path.start();
	rb->SetTerminalSpeed(maxSpeed);
}

void DStarEnemy::CalcPath(DirectX::XMFLOAT2 where) {
	HexTile* whereTile = grid->PointToTile(where);
	CalcPath(whereTile);
}

void DStarEnemy::CalcPath(HexTile* where) {
	HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(position._41, position._43));
	path = PathPlanner::FindPath(curTile, where, TileType::Dynamic, TileType::Static);
}

void DStarEnemy::Update() {
	EnemyBase::Update();
	HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(position._41, position._43));
	if(curTile) {
		if(curTile == next) {
			if(path.goal() == curTile) {
				Console::WriteLine << "We made it to our goal.";
				rb->Stop();
			} else {
				//dstar.UpdatePosition(curTile);
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
