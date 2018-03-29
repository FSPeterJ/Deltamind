#include "DStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"
#include "GhostTime.h"

void DStarEnemy::Enable() {
	tag = std::string("Enemy");
	if (!goal) { Console::ErrorLine << "No Goal! I'm gonna blowwwwww!!!!"; }

	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));
	if (!curTile) { Console::ErrorLine << "Ahhhh! Initalize me on the grid please!!"; }

	next = curTile; //is this needed or can i pass a ref to a null var below
	//grid->RemoveObstacle(curTile);//Remove on final build
	dstarId = PathPlanner::DStarLiteSearch(&curTile, &goal, &next, 3, Heuristics::OctileDistance);

	rb->SetTerminalSpeed(maxSpeed);
	EnemyBase::Enable();
}

void DStarEnemy::Disable() {
	DStarEnemy::UnSubscribe();
	EnemyBase::Disable();
}

void DStarEnemy::Subscribe() {}

void DStarEnemy::UnSubscribe() {}

void DStarEnemy::Destroy() {
	EnemyBase::Destroy();
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

void DStarEnemy::SetCore(Core* _core) {
	EnemyBase::SetCore(_core);
	goal = grid->PointToTile({ core->transform.GetPosition().x, core->transform.GetPosition().z });
}

void DStarEnemy::Awake(Object* obj) {
	grid = 0;
	//rb = 0;
	goal = 0;
	next = 0;
	curTile = 0;
	dstarId = 0;
	eventAdd = 0;
	eventRemove = 0;

	EnemyBase::Awake(obj);
	rb = &(GetComponent<PhysicsComponent>()->rigidBody);
}

void DStarEnemy::Attack() {
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
		EnemyBase::Attack();
		Console::WriteLine << "Core health: " << core->PercentHealth();
		timeSinceLastAttack = 0;
	}
}

void DStarEnemy::Update() {
	EnemyBase::Update();

	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));
	if (curTile) {

		if (goal == curTile) {
			Console::WriteLine << "We made it to our goal.";
			rb->Stop();
			Attack();
		}
		else {
			//if (KeyIsHit(Control::TestInputO)) {
			if (curTile == next) {
				PathPlanner::UpdateDStar(dstarId);

				//next = PathPlanner::GetDStarNextTile(dstarId);
				if (next) {
					auto nextPathPoint = grid->TileToPoint(next);
					Step();
					DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.matrix._41, 0.0f, nextPathPoint.y - transform.matrix._43, 1.0f);
					DirectX::XMVECTOR velocity = rb->GetVelocity();
					rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.matrix._41, 0.0f, nextPathPoint.y - transform.matrix._43, 0.5f);
					//Console::WriteLine << "Velocity: " << "(" << DirectX::XMVectorGetX(velocity) << ", " << DirectX::XMVectorGetY(velocity) << ", " << DirectX::XMVectorGetZ(velocity) << ")";
				}
				else {
					rb->Stop();
					Console::WriteLine << "There's no next path for me to GO!!";
				}
			}
		}
	}
}
