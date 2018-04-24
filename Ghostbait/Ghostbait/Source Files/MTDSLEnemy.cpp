#include "MTDSLEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "MessageEvents.h"
#include "PathPlanner.h"
#include "GhostTime.h"
#include "ThreadPool.h"
#include "RandomEngine.h"

//#include "Console.h"

void MTDSLEnemy::Enable() {
	//if (!goalReference) {
	//	if (!core) { Console::ErrorLine << "No Goal! I'm gonna blowwwwww!!!!"; }
	//	else { goalReference = &core->transform.matrix; }
	//}
	//ultimateTarget = goalReference;
	//SetGoalReference(ultimateTarget);
	//if (!goal) { Console::ErrorLine << "Goal Tile DOESN'T EXIST!!!"; }

	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));
	if (!curTile) { Console::ErrorLine << "Ahhhh! Initalize me on the grid please!!"; }

	next = curTile; //is this needed or can i pass a ref to a null var below
					//grid->RemoveObstacle(curTile);//Remove on final build
	rb->SetTerminalSpeed(maxSpeed);
	SetGoaltoRandAround();

	mtdstarId = PathPlanner::MTDStarLiteSearch(&curTile, &goal, &next, &path, 3, Heuristics::OctileDistance); //Change perception range to a variable

	//Threadding::ThreadPool::MakeJob([&] {  
		//std::lock_guard<std::mutex> lock(enemyMutex);
		//AntColony::LeavePheromone(&path, lingerTime, scentStrength); });
	//AntColony::LeavePheromone(&path, lingerTime, scentStrength);
	EnemyBase::Enable();

	Repath();
	//pathing = ThreadPool::MakeJob(false, [&]() { return NewAroundPath(); });
}

void MTDSLEnemy::Disable() {
	MTDSLEnemy::UnSubscribe();
	EnemyBase::Disable();
}

void MTDSLEnemy::Subscribe() {
	//Why 2 events?  Are adds different than removes or just futureproof?
	if (!eventAdd) eventAdd = MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) { PathPlanner::CostChangeNotice(*((SnapMessage*)e)->position); });
	if (!eventRemove) eventRemove = MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) { PathPlanner::CostChangeNotice(*((SnapMessage*)e)->position); });
}
void MTDSLEnemy::UnSubscribe() {
	if (eventAdd) { MessageEvents::UnSubscribe(EVENT_AddObstacle, eventAdd); eventAdd = 0; }
	if (eventRemove) { MessageEvents::UnSubscribe(EVENT_RemoveObstacle, eventRemove); eventRemove = 0; }
}

void MTDSLEnemy::Destroy() {
	PathPlanner::RemoveMTDStar(mtdstarId);

	Console::consoleMutex.lock();
	Console::WarningLine << "MTDSL Destory!!";
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

void MTDSLEnemy::SetGoal(HexTile* _goal) {
	if (!grid->IsValidTile(*_goal)) throw std::runtime_error("Goal is INVALID!");
	goal = _goal;
}

void MTDSLEnemy::SetGoal(DirectX::XMFLOAT2 _goal) {
	HexTile* goalTile = grid->PointToTile(_goal);
	SetGoal(goalTile);
}

void MTDSLEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}

void MTDSLEnemy::SetCore(Core* _core) {
	EnemyBase::SetCore(_core);
}

//void MTDSLEnemy::FindTempPath() {
//	//rb->Stop();
//	Console::WriteLine << "Finding temporary path!!";
//
//	float closest = grid->BlockWeight() + 1;
//	float temp = 0;
//	HexTile* nextClosest = nullptr;
//	HexRegion ring;
//	//HexTile* currentlyOn = curTile ? curTile : goal;
//	HexTile currentlyOn = grid->PointToTileOffGrid(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));
//	HexTile* ultiGoal = grid->PointToTile(DirectX::XMFLOAT2(ultimateTarget->_41, ultimateTarget->_43));
//
//	//if (currentlyOn) {
//	Console::WriteLine << "Enemy #" << mtdstarId << " ";
//	//int range = 3 * 2; //Perception range
//	int currFromGoal = currentlyOn.DistanceFrom(ultiGoal);
//	//range = range > currFromGoal ? currFromGoal : range;
//	int range = (int)(currFromGoal * 0.5);
//		for (; range > 0; --range) { //Perception range
//			ring = grid->Ring(&currentlyOn, range);
//			//Console::WarningLine << "RING SIZE BEFORE: " << ring.size();
//			ring.Filter(*grid);
//			//Console::WarningLine << "RING SIZE AFTER: " << ring.size();
//			for (int region = 0; region < ring.size(); ++region) {
//				if (!grid->GetTileExact(ring[region])) { Console::ErrorLine << "SOMEBODY DONE MESSED UP!!"; }
//				if (grid->IsBlocked(&ring[region])) continue;
//				temp = (float)ring[region].DistanceFrom(ultiGoal);
//				if (temp < closest) {
//					closest = temp;
//					nextClosest = &ring[region];
//				}
//			}
//			if (nextClosest) break;
//		}
//		if (nextClosest) {
//			DirectX::XMFLOAT2 holder = grid->TileToPoint(nextClosest);
//			tempGoal._41 = holder.x;
//			tempGoal._43 = holder.y;
//			SetGoalReference(&tempGoal);
//		
//			Console::WriteLine << "Found temporary objective...";
//		}
//	//}
//
//		if (!goal) {
//			Console::ErrorLine << "WHERE THE F IS THE GOAL!?!?!?!";
//	}
//}

void MTDSLEnemy::Awake(Object* obj) {
	grid = 0;
	//rb = 0;
	//start = 0;
	goal = 0;
	next = 0;
	curTile = 0;
	//lastTile = 0;
	eventAdd = 0;
	eventRemove = 0;

	isPathing = false;
	pathIsBlocked = false;

	mtdstarId = 0;
	lingerTime = 30.0f;
	scentStrength = 15.0f;

	EnemyBase::Awake(obj);
}

void MTDSLEnemy::Patrol() {

	//grid->Color(path, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), 3);
	//if (goalReference) {
	//	goal = grid->PointToTile(DirectX::XMFLOAT2(goalReference->_41, goalReference->_43));
	//	if (!goal) {
	//		if(goalReference == &tempGoal)
	//			Console::WarningLine << "GOAL IS NULL";
 //		}
	//}
	////lastTile = curTile;
	//curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));

	//if (!curTile) {
	//	Console::WriteLine << "OUT OF BOUNDS!";
	//	rb->Stop();
	//	//rb->SetVelocity(DirectX::XMVectorScale(rb->GetVelocity(), -1.0f));
	//	FindTempPath();
	//	curTile = lastTile;
	//}
	//else {

		//if (curTile == next) {
		//	rb->Stop();
		//	//HexRegion neigh = grid->Spiral(curTile, 3);
		//	//grid->Color(neigh, { 1.0f, 0.0f, 0.0f }, 3);
		//	grid->GetTileExact(3, 4)->DrawCheapFill(HexagonalGridLayout::FlatLayout, {0.0f, 1.0f, 0.0f});
		//	grid->GetTileExact(-1, 8)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 0.0f, 1.0f, 1.0f });
		//	grid->GetTileExact(2, 6)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 1.0f });

		//	//blocked
		//	grid->GetTileExact(1, 5)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });
		//	grid->GetTileExact(0, 5)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });
		//	grid->GetTileExact(-1, 5)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });
		//	grid->GetTileExact(-2, 5)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });

		//	grid->GetTileExact(2, 5)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });
		//	grid->GetTileExact(3, 6)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });
		//	grid->GetTileExact(3, 7)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });
		//	grid->GetTileExact(3, 5)->DrawCheapFill(HexagonalGridLayout::FlatLayout, { 1.0f, 1.0f, 0.0f });

	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));

	if (!curTile) {
		if (isOutofBounds) return;

		Console::ErrorLine << "Out of BOUNDS!!";
		isOutofBounds = true;
		rb->SetVelocity(DirectX::XMVectorScale(rb->GetVelocity(), -1.0f));
		return;
	}

	if (pathing.valid() && (isPathing && pathing.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)) return;

	//PathPlanner::MTDStarPollPath(mtdstarId);

	//GhostTime::Moment m = GhostTime::Now();
	//Console::WriteLine << "Polling......";
	
	//PathPlanner::MTDStarPollPath(mtdstarId);

	//Console::WriteLine << "Polled duration: " << GhostTime::Duration(m, GhostTime::Now());

	if (isOutofBounds) {
		rb->Stop();
		Repath();
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
		Console::WriteLine << "We made it to our goal.";
		rb->Stop();
		//isChasing = false;

		ChangeState(ATTACK);
		return;
	}

	EnemyBase::Patrol();

	if (curTile == next) {
		//Console::WriteLine << "Reached next Tile.";d
		isRedirecting = false;
		Step();
		//GhostTime::Moment m = GhostTime::Now();
		//Console::WriteLine << "Leaving Pheromone......";
		//ThreadPool::MakeJob(true, [=]() {AntColony::LeavePheromone(curTile, { lingerTime, scentStrength }); });
		//Console::WriteLine << "Polled duration: " << GhostTime::Duration(m, GhostTime::Now());

		next = path.Next(curTile);
		if (!next) { return; }
		auto nextPathPoint = grid->TileToPoint(next);

		//position._41 = nextPathPoint.x;
		//position._43 = nextPathPoint.y;
		DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 1.0f);
		DirectX::XMVECTOR velocity = rb->GetVelocity();
		rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.GetMatrix()._41, 0.0f, nextPathPoint.y - transform.GetMatrix()._43, 0.5f);
		Repath();
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

bool MTDSLEnemy::ReTarget(GameObject* _obj) {
	if (_obj) Console::WriteLine << "Retargetting to a Turret...";
	else Console::WriteLine << "Retargetting to CORE!!!...";

	if (isPathing) return false;
	if (!EnemyBase::ReTarget(_obj)) return false;
	//GameObject* goalObj = _obj ? _obj : core;
	//SetGoal(grid->PointToTile(DirectX::XMFLOAT2(goalObj->transform.matrix._41, goalObj->transform.matrix._43)));
	//isPathing = true;
	//pathing = ThreadPool::MakeJob(false, [&]() { return NewAroundPath(); });
	SetGoaltoRandAround();
	Repath();
	return true;
}

bool MTDSLEnemy::ValidateAttackTarget() {
	HexTile* tar = grid->PointToTile({ targetPos[0], targetPos[2] });
	GridBasedObject* obj = dynamic_cast<GridBasedObject*>(targetObj);
	bool res = grid->GetIntersectingTilesRanges(curTile, attackRange, tar, obj->gridRadius).size() > 0;
	if (!res) return false;
	DirectX::XMFLOAT3 newPoint = { targetPos[0], transform.GetPosition().y, targetPos[2] };
	transform.TurnTowards(newPoint, 1);
	return true;
}

void MTDSLEnemy::Repath() {
	if (isPathing) return;

	isPathing = true;
	pathing = ThreadPool::MakeJob(false, [&](){
		PathPlanner::UpdateMTDStar(mtdstarId); 
		if (isDying) return;
		isPathing = false;
		PathPlanner::MTDStarPollPath(mtdstarId);
		//if (path.size() > 0) { AntColony::LeavePheromone(&path, lingerTime, scentStrength); }
		return;
	});
}

bool MTDSLEnemy::NewAroundPath() {
	isPathing = true;
	HexTile* tempGoal = grid->PointToTile({ targetPos[0], targetPos[2] });
	HexTile* tile = nullptr;
	HexRegion ring;
	bool pathFound = false;
	int startInd = 0, endInd = 0, curInd = 0;
	path.clear();

	HexPath spire;// = grid->Spiral(goal, 5, false);

	//Console::consoleMutex.lock();
	//Console::ErrorLine << "isDying address in Thread: " << &isDying;
	//Console::consoleMutex.unlock();

	for (int i = 1; i < 10; ++i) {
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
				PathPlanner::UpdateMTDStar(mtdstarId);
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
	//if (pathFound)
	//	AntColony::LeavePheromone(&path, lingerTime, scentStrength);

	return pathFound;
}

void MTDSLEnemy::SetGoaltoRandAround() {
	HexTile* tempGoal = grid->PointToTile({ targetPos[0], targetPos[2] });
	HexRegion ring = grid->Ring(tempGoal, dynamic_cast<GridBasedObject*>(targetObj)->gridRadius + 1);
	ring.Filter(*grid);
	HexPath spire = ring.ToGrid(grid);
	SetGoal(spire[Omiracon::Random::RandomNumber(0, (int)spire.size())]);
}
