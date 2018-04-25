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

	isPathing = false;
	pathIsBlocked = false;
	start = false;
	eventAdd = 0;
	eventRemove = 0;

	lingerTime = 10.0f;
	scentStrength = 2.0f;	

	path.clear();
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
	AStarEnemy::Subscribe();
	EnemyBase::Enable();

	next = curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetMatrix()._41, transform.GetMatrix()._43));
	rb->SetTerminalSpeed(maxSpeed);

	Repath();
	//if (isPathing) {
	//	pathing.get();
	//	isPathing = false;
	//}

	/*if(!isPathing && (!goal || path.size() < 1)) {
		ReTarget();
	}*/

	//if (!path.size()) throw std::runtime_error("Enemy could not find path.");
}
void AStarEnemy::Disable() {
	AStarEnemy::UnSubscribe();
	EnemyBase::Disable();
}
void AStarEnemy::Destroy() {
	Console::consoleMutex.lock();
	Console::WarningLine <<"ASTAR Destory!!";
	Console::consoleMutex.unlock();
	isDying = true;

	if (isPathing) {
		try {
			pathing.get();
		}
		catch (...) {
			Console::consoleMutex.lock();
			Console::WarningLine << "Something went wrong with the Thread!!!";
			Console::consoleMutex.unlock();
		}
	}
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

	//Update Path
	HexTile* tempTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));

	if (!tempTile) {
		if (isOutofBounds) return;
		
		Console::ErrorLine << "Out of BOUNDS!!";
		isOutofBounds = true;
		rb->SetVelocity(DirectX::XMVectorScale(rb->GetVelocity(), -1.0f));
		EnemyBase::Patrol();
		return;
	}

	if (pathing.valid() && (isPathing && pathing.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)) return;

	curTile = tempTile;

	if (isOutofBounds) {
		if (pathIsBlocked) {
			next = curTile;
			isPathing = true;
			pathing = ThreadPool::MakeJob(false, [&]() { LastResort(); });
		}
		else
			Repath();
		isRedirecting = false;
		isOutofBounds = false;
		return;
	}

	if (path.size() < 1) {
		Console::ErrorLine << "Lost its Path!!";
		rb->Stop();
		Repath();
		return;
	}

	if (curTile == goal) {
		isRedirecting = false;
		Console::WriteLine << "We made it to our goal.";
		rb->Stop();
		if (pathIsBlocked) {
			next = curTile;
			isPathing = true;
			pathing = ThreadPool::MakeJob(false, [&]() { LastResort(); });
			return;
		}
		//isChasing = false;

		ChangeState(ATTACK);
		return;
	}

	EnemyBase::Patrol();
	
	if (curTile == next) {
		isRedirecting = false;
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
		if (isRedirecting) return;
		Console::ErrorLine << "Strayed off path!!";
		rb->Stop();

		isRedirecting = true;
		if (next) {
			auto nextPathPoint = grid->TileToPoint(next);

			DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 1.0f);
			DirectX::XMVECTOR velocity = rb->GetVelocity();
			rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 0.5f);

			return;
		}

		Repath();
		next = curTile;
		return;
	}
}

//Other Overrides
void AStarEnemy::Repath() {
	if (isPathing) return;
	isPathing = true;
	//Console::consoleMutex.lock();
	//Console::ErrorLine << "isDying address Before Thread: " << &isDying;
	//Console::WriteLine << "Path size: " << path.size() << "Path Address: " << &path;
	//Console::consoleMutex.unlock();
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
	if (!grid->IsValidTile(*_goal)) throw std::runtime_error("Goal is INVALID!");
	goal = _goal;
}
bool AStarEnemy::NewPath() {
	path.clear();

	CalcPath(goal);

	return path.size();
}
bool AStarEnemy::NewAroundPath() {
	pathIsBlocked = false;
	HexTile* tempGoal = grid->PointToTile({ targetPos[0], targetPos[2] });
	HexTile* tile = nullptr;
	HexRegion ring;
	bool pathFound = false;
	int startInd = 0, endInd = 0, curInd = 0;
	path.clear();

	HexPath spire;// = grid->Spiral(goal, 5, false);

	//Console::consoleMutex.lock();
	//Console::ErrorLine << "isDying address in Thread: " << &isDying;
	//Console::WriteLine << "Path size: " << path.size() << "Path Address: " << &path;
	//Console::consoleMutex.unlock();

	int r = dynamic_cast<GridBasedObject*>(targetObj)->gridRadius + 1;
	for (int i = r; i < r + 5 ; ++i) {
		ring = grid->Ring(tempGoal, i);
		ring.Filter(*grid);
		spire = ring.ToGrid(grid);

		endInd = (int)spire.size();
		curInd = startInd = Omiracon::Random::RandomNumber(0, endInd);
		
		do {
			if (isDying) {
				//Console::consoleMutex.lock();
				//Console::ErrorLine << &isDying << "isDying is TRUE in Thread!";
				//Console::consoleMutex.unlock();
				return false;
			}
			tile = spire[curInd];
			if (!grid->IsBlocked(tile)) {
				CalcPath(tile);
				if (!path.size()) {
					if (++curInd >= endInd) curInd = 0;
					continue;
				}
				SetGoal(path.goal());
				//Console::WarningLine << &goal <<" Goal: " << goal->q << ", " << goal->r;
				pathFound = true;
				break;
			}
			if (++curInd >= endInd) curInd = 0;
		} while (curInd != startInd);

		if (pathFound) {
			break;
		}
	}
	//goal = grid->GetRandomTile();
	//CalcPath(goal);
	
	//if(!path.size()) {
	//	NewRandPath();
	//}
	if (pathFound)
		AntColony::LeavePheromone(&path, lingerTime, scentStrength);
	else {
		//pathIsBlocked = true;
		this->LastResort();
	}

	//Console::consoleMutex.lock();
	//Console::WriteLine << "Path size: " << path.size() << "Path Address: " << &path;
	//Console::consoleMutex.unlock();
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
	GridBasedObject* obj = dynamic_cast<GridBasedObject*>(targetObj);
	bool res = grid->GetIntersectingTilesRanges(curTile, attackRange, tar, obj->gridRadius).size() > 0;
	if (!res) return false;
	DirectX::XMFLOAT3 newPoint = { targetPos[0], transform.GetPosition().y, targetPos[2] };
	transform.TurnTowards(newPoint, 1);
	return true;
}

void AStarEnemy::LastResort() {

	isRedirecting = false;
	HexTile* goalTile = grid->PointToTile({ targetPos[0], targetPos[2] });
	HexPath spiral = grid->Spiral(curTile, perceptionRange + 2, false, perceptionRange).ToGrid(grid);
	std::sort(spiral.begin(), spiral.end(), [=](HexTile* a, HexTile* b) {return a->DistanceFrom(goalTile) < b->DistanceFrom(goalTile); });
	
	for (int i = 0; i < spiral.size(); ++i) {
		if (grid->IsBlocked(spiral[i])) continue;
		//if (spiral[i] == curTile) {
		//	SetGoal(curTile);
		//	break;
		//}
		CalcPath(spiral[i]);
		if (path.size() > 0) {
			SetGoal(path.goal());
			break;
		}
	}
	isPathing = false;
	//Console::WriteLine << "Path size: " << path.size() << "Path Address: " << &path;
}