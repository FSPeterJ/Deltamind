#include "DStarEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"

void DStarEnemy::Enable(bool _destroyOnReset) {
	eventAdd = MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->Repath(); });
	eventRemove = MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->Repath(); });
	GameObject::Enable(_destroyOnReset);
	Start();
}

void DStarEnemy::Disable() {
	MessageEvents::UnSubscribe(EVENT_AddObstacle, eventAdd);
	MessageEvents::UnSubscribe(EVENT_RemoveObstacle, eventRemove);
	EnemyBase::Disable();
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

void DStarEnemy::Awake(Object* obj) {
	EnemyBase::Awake(obj);
}

void DStarEnemy::Start() {

	tag = std::string("Enemy");

	rb = &(GetComponent<PhysicsComponent>()->rigidBody);

	if(!goal) { Console::ErrorLine << "No Goal! I'm gonna blowwwwww!!!!"; }

	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));

	if(!curTile) { Console::ErrorLine << "Ahhhh! Initalize me on the grid please!!"; }

	next = curTile; //is this needed or can i pass a ref to a null var below
	grid->RemoveObstacle(curTile);//Remove on final build
	dstarId = PathPlanner::DStarLiteSearch(curTile, goal, &next, Heuristics::OctileDistance);
	

	rb->SetTerminalSpeed(maxSpeed);
}

void DStarEnemy::Update() {
	EnemyBase::Update();

	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));
	if (curTile) {

		if (goal == curTile) {
			Console::WriteLine << "We made it to our goal.";
			rb->Stop();
		}
		else {
			//if (KeyIsHit(Control::TestInputO)) {
			if (curTile == next) {
				PathPlanner::UpdateDStar(dstarId);

				next = PathPlanner::GetDStarNextTile(dstarId);
				if (next) {
					auto nextPathPoint = grid->TileToPoint(next);

					DirectX::XMVECTOR nextDirection = DirectX::XMVectorSet(nextPathPoint.x - transform.matrix._41, 0.0f, nextPathPoint.y - transform.matrix._43, 1.0f);
					DirectX::XMVECTOR velocity = rb->GetVelocity();
					rb->AddForce(3.0f * (DirectX::XMVectorGetX(DirectX::XMVector3Dot(nextDirection, velocity)) + 1.0f), nextPathPoint.x - transform.matrix._41, 0.0f, nextPathPoint.y - transform.matrix._43, 0.5f);
					Console::WriteLine << "Velocity: " << "(" << DirectX::XMVectorGetX(velocity) << ", " << DirectX::XMVectorGetY(velocity) << ", " << DirectX::XMVectorGetZ(velocity) << ")";
				}
				else {
					rb->Stop();
					Console::WriteLine << "There's no next path for me to GO!!";
				}
			}
		}
	}
}
