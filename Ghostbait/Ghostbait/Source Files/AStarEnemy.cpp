#include "AStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"
#include "GhostTime.h"

AStarEnemy::AStarEnemy() {
	tag = std::string("Enemy");
}

//Main Overrides

void AStarEnemy::Awake(Object* obj) {
	howFarAlong = 0;
	grid = 0;
	//rb = 0;
	goal = 0;
	next = 0;


	start = false;
	eventAdd = 0;
	eventRemove = 0;
	
	EnemyBase::Awake(obj);
	rb = &(GetComponent<PhysicsComponent>()->rigidBody);
}

void AStarEnemy::Subscribe() {
	//Why 2 events?  Are adds different than removes or just futureproof?
	if(!eventAdd) eventAdd = MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->Repath(); });
	if(!eventRemove) eventRemove = MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->Repath(); });
}
void AStarEnemy::UnSubscribe() {
	if (eventAdd) { MessageEvents::UnSubscribe(EVENT_AddObstacle, eventAdd); eventAdd = 0; }
	if (eventRemove) { MessageEvents::UnSubscribe(EVENT_RemoveObstacle, eventRemove); eventRemove = 0; }
}
void AStarEnemy::Enable() {
	if(!goal) {
		NewRandPath();
	}
	rb->SetTerminalSpeed(maxSpeed);
	next = path.start();
	AStarEnemy::Subscribe();
	EnemyBase::Enable();
}
void AStarEnemy::Disable() {
	AStarEnemy::UnSubscribe();
	EnemyBase::Disable();
}
void AStarEnemy::Destroy() {
	EnemyBase::Destroy();
}
void AStarEnemy::Update() {
	EnemyBase::Update();

	//DirectX::XMFLOAT3 vel;
	//DirectX::XMStoreFloat3(&vel, rb->GetVelocity());
	//DirectX::XMFLOAT3 newPoint = { transform.GetPosition().x + vel.x, transform.GetPosition().y, transform.GetPosition().z + vel.z };
	//transform.TurnTowards(newPoint, 1);


	//Update Path
	HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));
	if(curTile) {
		if(curTile == next) {
			if(path.goal() == curTile) {
				//Console::WriteLine << "We made it to our goal.";
				Attack();
				rb->Stop();
			}
			else {
				howFarAlong++;
				if(howFarAlong > path.size() - 1) { return; }
				next = path[howFarAlong];
				auto nextPathPoint = grid->TileToPoint(next);

				//position._41 = nextPathPoint.x;
				//position._43 = nextPathPoint.y;
				DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 1.0f);
				DirectX::XMVECTOR velocity = rb->GetVelocity();
				rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 0.5f);
			}
		}
	}
}
void AStarEnemy::Attack() {
	if (timeSinceLastAttack == -1) {
		if (core) core->AdjustHealth(-attackDamage);
		Console::WriteLine << "Core health: " << core->PercentHealth();
		timeSinceLastAttack = 0;
		return;
	}

	float dt = (float)GhostTime::DeltaTime();
	timeSinceLastAttack += dt;

	float timeToAttack = 1 / attackSpeed;
	if (timeSinceLastAttack >= timeToAttack) {
		core->AdjustHealth(-attackDamage);
		Console::WriteLine << "Core health: " << core->PercentHealth();
		timeSinceLastAttack = 0;
	}
}


//Other Overrides
void AStarEnemy::Repath() {
	NewPath();
}
void AStarEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}
void AStarEnemy::SetCore(Core* _core) {
	EnemyBase::SetCore(_core);
	HexTile* goalTile = grid->PointToTile({ core->transform.GetPosition().x, core->transform.GetPosition().z });
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
	HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));
	path = PathPlanner::FindPath(curTile, where, TileType::Static, TileType::Static);
}