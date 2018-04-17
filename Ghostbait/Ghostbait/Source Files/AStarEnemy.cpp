#include "AStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"
#include "GhostTime.h"
#include "ThreadPool.h"
#include "RandomEngine.h"

AStarEnemy::AStarEnemy() {
	tag = std::string("Enemy");
}

//Main Overrides

void AStarEnemy::Awake(Object* obj) {
	EnemyBase::Awake(obj);
	
	grid = 0;
	//rb = 0;
	goal = 0;
	next = 0;
	curTile = 0;

	start = false;
	eventAdd = 0;
	eventRemove = 0;

	lingerTime = 10.0f;
	scentStrength = 2.0f;	
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

	if (isPathing) {
		pathing.get();
		isPathing = false;
	}

	if(!isPathing && (!goal || path.size() < 1)) {
		isPathing = true;
		pathing = Threadding::ThreadPool::MakeJob(false, [&]() {NewAroundPath(); });
	}

	//if (!path.size()) throw std::runtime_error("Enemy could not find path.");
	AntColony::LeavePheromone(&path, lingerTime, scentStrength);
	next = curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));
	rb->SetTerminalSpeed(maxSpeed);

	EnemyBase::Enable();
	AStarEnemy::Subscribe();
}
void AStarEnemy::Disable() {
	AStarEnemy::UnSubscribe();
	EnemyBase::Disable();
}
void AStarEnemy::Destroy() {
	isDying = true;
	if (isPathing) pathing.get();
	EnemyBase::Destroy();
}
//void AStarEnemy::Update() {
//	EnemyBase::Update();
//
//	//DirectX::XMFLOAT3 vel;
//	//DirectX::XMStoreFloat3(&vel, rb->GetVelocity());
//	//DirectX::XMFLOAT3 newPoint = { transform.GetPosition().x + vel.x, transform.GetPosition().y, transform.GetPosition().z + vel.z };
//	//transform.TurnTowards(newPoint, 1);
//
//	//if (isPathing) { 
//	//	if (pathing.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) return;
//	//	pathing.get(); 
//	//	isPathing = false; 
//	//};
//
//	////Update Path
//	//HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));
//	//if(curTile) {
//	//	if(curTile == next) {
//	//		if (path.size() > 0) {
//	//			if (path.goal() == curTile) {
//	//				//Console::WriteLine << "We made it to our goal.";
//	//				Attack();
//
//	//				rb->Stop();
//	//			}
//	//			else {
//	//				Step();
//	//				howFarAlong++;
//	//				if (howFarAlong > path.size() - 1) { return; }
//	//				next = path[howFarAlong];
//	//				auto nextPathPoint = grid->TileToPoint(next);
//
//	//				//position._41 = nextPathPoint.x;
//	//				//position._43 = nextPathPoint.y;
//	//				DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 1.0f);
//	//				DirectX::XMVECTOR velocity = rb->GetVelocity();
//	//				rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 0.5f);
//	//			}
//	//		}
//	//		else { 
//	//			rb->Stop();
//	//			isPathing = true;
//	//			pathing = Threadding::ThreadPool::MakeJob([&]() {NewRandPath(); });
//	//			return;
//	//		}
//	//	}
//	//}
//	//else {
//	//	//rb->SetVelocity(DirectX::XMVectorScale(rb->GetVelocity(), -1.0f));
//	//	//NewRandPath();
//	//	rb->Stop();
//	//}
//}
//void AStarEnemy::Attack() {
//	if (timeSinceLastAttack == -1) {
//		if (core) core->AdjustHealth(-attackDamage);
//		RecordAttack();
//		Console::WriteLine << "Core health: " << core->PercentHealth();
//		timeSinceLastAttack = 0;
//		return;
//	}
//
//	float dt = (float)GhostTime::DeltaTime();
//	timeSinceLastAttack += dt;
//
//	float timeToAttack = 1 / attackSpeed;
//	if (timeSinceLastAttack >= timeToAttack) {
//		core->AdjustHealth(-attackDamage);
//		RecordAttack();
//		Console::WriteLine << "Core health: " << core->PercentHealth();
//		timeSinceLastAttack = 0;
//	}
//}

void AStarEnemy::Patrol() {

	if (isPathing && pathing.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) return;

	//Update Path
	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));

	if (!curTile) {
		Console::ErrorLine << "Out of BOUNDS!!";
		isOutofBounds = true;
		rb->SetVelocity(DirectX::XMVectorScale(rb->GetVelocity(), -1.0f));
		return;
	}

	isOutofBounds = false;
	if (path.size() < 1) {
		Console::ErrorLine << "Lost its Path!!";
		rb->Stop();
		if (isPathing) return;
		isPathing = true;
		pathing = Threadding::ThreadPool::MakeJob(false, [&]() {NewAroundPath(); });
		return;
	}

	if (curTile == goal) {
		Console::WriteLine << "We made it to our goal.";
		rb->Stop();
		//isChasing = false;

		ChangeState(ATTACK);
		return;
	}

	EnemyBase::Patrol();
	
	if (curTile == next) {
		Step();
		next = path.Next(curTile);
		if (!next) { return; }
		auto nextPathPoint = grid->TileToPoint(next);

		//position._41 = nextPathPoint.x;
		//position._43 = nextPathPoint.y;
		DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 1.0f);
		DirectX::XMVECTOR velocity = rb->GetVelocity();
		rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 0.5f);
		return;
	}

	if (path.Next(curTile) != next) {
		Console::ErrorLine << "Strayed off path!!";
		rb->Stop();
		Repath();
		next = curTile;
		return;
	}
}

//Other Overrides
void AStarEnemy::Repath() {
	if (isPathing) return;
	isPathing = true;
	pathing = Threadding::ThreadPool::MakeJob(false, [&]() {NewAroundPath(); });
}
void AStarEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}
void AStarEnemy::SetCore(Core* _core) {
	EnemyBase::SetCore(_core);
	//SetGoal(grid->PointToTile({ core->transform.GetPosition().x, core->transform.GetPosition().z }));
}
bool AStarEnemy::ReTarget(GameObject* _obj) {
	if (_obj) Console::WriteLine << "Retargetting to a Turret...";
	else Console::WriteLine << "Retargetting to CORE!!!...";

	if (isPathing) return false;
	if (!EnemyBase::ReTarget(_obj)) return false;
	//GameObject* goalObj = _obj ? _obj : core;
	//SetGoal(grid->PointToTile(DirectX::XMFLOAT2(goalObj->transform.matrix._41, goalObj->transform.matrix._43)));
	isPathing = true;
	pathing = Threadding::ThreadPool::MakeJob(false, [&]() {NewAroundPath(); });

	return true;
}

//Other
void AStarEnemy::SetGoal(HexTile* _goal) {
	if (!grid->IsValidTile(*_goal)) throw std::runtime_error("Goal is Shit!");
	goal = _goal;
}
bool AStarEnemy::NewPath() {
	path.clear();

	CalcPath(goal);

	return path.size();
}
bool AStarEnemy::NewAroundPath() {
	HexTile* tempGoal = grid->PointToTile({ targetPos[0], targetPos[2] });
	HexTile* tile = nullptr;
	HexRegion ring;
	bool pathFound = false;
	int startInd = 0, endInd = 0, curInd = 0;
	path.clear();

	HexPath spire;// = grid->Spiral(goal, 5, false);

	for (int i = 1; i < 10; ++i) {
		ring = grid->Ring(tempGoal, i);
		ring.Filter(*grid);
		spire = ring.ToGrid(grid);

		endInd = (int)spire.size();
		curInd = startInd = Omiracon::Random::RandomNumber(0, endInd);
		
		do {
			if (isDying) return false;
			tile = spire[curInd];
			if (!grid->IsBlocked(tile)) {
				CalcPath(tile);
				if (!path.size()) continue;
				SetGoal(path.goal());
				//Console::WarningLine << &goal <<" Goal: " << goal->q << ", " << goal->r;
				pathFound = true;
				break;
			}
			if (++curInd >= endInd) curInd = 0;
		} while (curInd != startInd);

		if (pathFound) break;
	}
	//goal = grid->GetRandomTile();
	//CalcPath(goal);
	
	//if(!path.size()) {
	//	NewRandPath();
	//}
	isPathing = false;
	return pathFound;
}
void AStarEnemy::CalcPath(DirectX::XMFLOAT2& where) {
	HexTile* whereTile = grid->PointToTile(where);
	CalcPath(whereTile);
}
void AStarEnemy::CalcPath(HexTile* where) {
	HexTile* tempTile = grid->PointToTile({ transform.matrix._41, transform.matrix._43 });

	if (!tempTile || !where) return;
	path = PathPlanner::FindPath(tempTile, where, TileType::Static, TileType::Static);
}

bool AStarEnemy::ValidateAttackTarget() {
	HexTile* tar = grid->PointToTile({ targetPos[0], targetPos[2] });
	return grid->GetIntersectingTilesRanges(curTile, attackRange, tar, 0).size() > 0;
}