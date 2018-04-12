#include "AStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"
#include "GhostTime.h"
#include "ThreadPool.h"

AStarEnemy::AStarEnemy() {
	tag = std::string("Enemy");
}

//Main Overrides

void AStarEnemy::Awake(Object* obj) {
	EnemyBase::Awake(obj);
	
	howFarAlong = 0;
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

	if(!isPathing && (!goal || !(path.size() > 0))) {
		isPathing = true;
		pathing = Threadding::ThreadPool::MakeJob([&]() {NewRandPath(); });
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
	EnemyBase::Destroy();
}
void AStarEnemy::Update() {
	EnemyBase::Update();

	//DirectX::XMFLOAT3 vel;
	//DirectX::XMStoreFloat3(&vel, rb->GetVelocity());
	//DirectX::XMFLOAT3 newPoint = { transform.GetPosition().x + vel.x, transform.GetPosition().y, transform.GetPosition().z + vel.z };
	//transform.TurnTowards(newPoint, 1);

	//if (isPathing) { 
	//	if (pathing.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) return;
	//	pathing.get(); 
	//	isPathing = false; 
	//};

	////Update Path
	//HexTile* curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));
	//if(curTile) {
	//	if(curTile == next) {
	//		if (path.size() > 0) {
	//			if (path.goal() == curTile) {
	//				//Console::WriteLine << "We made it to our goal.";
	//				Attack();

	//				rb->Stop();
	//			}
	//			else {
	//				Step();
	//				howFarAlong++;
	//				if (howFarAlong > path.size() - 1) { return; }
	//				next = path[howFarAlong];
	//				auto nextPathPoint = grid->TileToPoint(next);

	//				//position._41 = nextPathPoint.x;
	//				//position._43 = nextPathPoint.y;
	//				DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 1.0f);
	//				DirectX::XMVECTOR velocity = rb->GetVelocity();
	//				rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 0.5f);
	//			}
	//		}
	//		else { 
	//			rb->Stop();
	//			isPathing = true;
	//			pathing = Threadding::ThreadPool::MakeJob([&]() {NewRandPath(); });
	//			return;
	//		}
	//	}
	//}
	//else {
	//	//rb->SetVelocity(DirectX::XMVectorScale(rb->GetVelocity(), -1.0f));
	//	//NewRandPath();
	//	rb->Stop();
	//}
}
void AStarEnemy::Attack() {
	if (timeSinceLastAttack == -1) {
		if (core) core->AdjustHealth(-attackDamage);
		RecordAttack();
		Console::WriteLine << "Core health: " << core->PercentHealth();
		timeSinceLastAttack = 0;
		return;
	}

	float dt = (float)GhostTime::DeltaTime();
	timeSinceLastAttack += dt;

	float timeToAttack = 1 / attackSpeed;
	if (timeSinceLastAttack >= timeToAttack) {
		core->AdjustHealth(-attackDamage);
		RecordAttack();
		Console::WriteLine << "Core health: " << core->PercentHealth();
		timeSinceLastAttack = 0;
	}
}

void AStarEnemy::Patrol() {
	if (isPathing) {
		if (pathing.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) return;
		pathing.get();
		isPathing = false;
	};

	//Update Path
	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));

	if (!curTile) {
		isOutofBounds = true;
		rb->SetVelocity(DirectX::XMVectorScale(rb->GetVelocity(), -1.0f));
		return;
	}

	isOutofBounds = false;
	if (path.size() < 1) {
		rb->Stop();
		isPathing = true;
		pathing = Threadding::ThreadPool::MakeJob([&]() {NewRandPath(); });
		return;
	}

	if (curTile == path.goal()) {
		//Console::WriteLine << "We made it to our goal.";
		rb->Stop();
		ChangeState(ATTACK);
		return;
	}

	EnemyBase::Patrol();
	
	if (curTile == next) {
		Step();
		howFarAlong++;
		if (howFarAlong > path.size() - 1) { return; }
		next = path[howFarAlong];
		auto nextPathPoint = grid->TileToPoint(next);

		//position._41 = nextPathPoint.x;
		//position._43 = nextPathPoint.y;
		DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 1.0f);
		DirectX::XMVECTOR velocity = rb->GetVelocity();
		rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 0.5f);
		return;
	}

	if (path.Next(curTile) != next) {
		rb->Stop();
		Repath();
		next = curTile;
		return;
	}

}


//Other Overrides
void AStarEnemy::Repath() {
	isPathing = true;
	pathing = Threadding::ThreadPool::MakeJob([&]() {NewPath(); });
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

	HexRegion spire = grid->Spiral(goal, 10);
	spire.remove(*goal);

	for (auto& tile : spire) {
		if (!grid->IsBlocked(&tile)) {
			CalcPath(&tile);
			if (!path.size()) continue;
			break;
		}
	}
	//goal = grid->GetRandomTile();
	//CalcPath(goal);

	//if(!path.size()) {
	//	NewRandPath();
	//}
}
void AStarEnemy::CalcPath(DirectX::XMFLOAT2 where) {
	HexTile* whereTile = grid->PointToTile(where);
	CalcPath(whereTile);
}
void AStarEnemy::CalcPath(HexTile* where) {
	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));

	if (!curTile || !where) return;
	path = PathPlanner::FindPath(curTile, where, TileType::Static, TileType::Static);
}
