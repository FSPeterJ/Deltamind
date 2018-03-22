#include "MTDSLEnemy.h"
#include "Heuristics.h"
#include "PhysicsComponent.h"
//#include "Console.h"
#include "MessageEvents.h"
#include "PathPlanner.h"

void MTDSLEnemy::Enable(bool _destroyOnReset) {
	eventAdd = MessageEvents::Subscribe(EVENT_AddObstacle, [=](EventMessageBase* e) {this->Repath(); });
	eventRemove = MessageEvents::Subscribe(EVENT_RemoveObstacle, [=](EventMessageBase* e) {this->Repath(); });
	GameObject::Enable(_destroyOnReset);
	Start();
}

void MTDSLEnemy::Disable() {
	MessageEvents::UnSubscribe(EVENT_AddObstacle, eventAdd);
	MessageEvents::UnSubscribe(EVENT_RemoveObstacle, eventRemove);
	EnemyBase::Disable();
}

void MTDSLEnemy::SetGoal(HexTile* _goal) {
	goal = _goal;
}

void MTDSLEnemy::SetGoal(DirectX::XMFLOAT2 _goal) {
	HexTile* goalTile = grid->PointToTile(_goal);
	if (goalTile) { goal = goalTile; }
}

void MTDSLEnemy::SetGoalReference(DirectX::XMFLOAT4X4* _goal) {
	goalReference = _goal;
}

void MTDSLEnemy::SetGrid(HexGrid* _grid) {
	grid = _grid;
}

void MTDSLEnemy::Awake(Object* obj) {
	EnemyBase::Awake(obj);
}

void MTDSLEnemy::Start() {

	tag = std::string("Enemy");

	rb = &(GetComponent<PhysicsComponent>()->rigidBody);

	if (!goalReference) { Console::ErrorLine << "No Goal! I'm gonna blowwwwww!!!!"; }

	goal = grid->PointToTile(DirectX::XMFLOAT2(goalReference->_41, goalReference->_43));
	if (!goal) { Console::ErrorLine << "Goal Tile DOESN'T EXIST!!!"; }

	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));
	if (!curTile) { Console::ErrorLine << "Ahhhh! Initalize me on the grid please!!"; }

	next = curTile; //is this needed or can i pass a ref to a null var below
	//grid->RemoveObstacle(curTile);//Remove on final build
	mtdstarId = PathPlanner::MTDStarLiteSearch(&(transform.matrix), goalReference, Heuristics::OctileDistance);

	rb->SetTerminalSpeed(maxSpeed);
}

void MTDSLEnemy::Update() {
	EnemyBase::Update();

	//grid->Color(path, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), 3);
	if (goalReference) {
		goal = grid->PointToTile(DirectX::XMFLOAT2(goalReference->_41, goalReference->_43));
	}
	curTile = grid->PointToTile(DirectX::XMFLOAT2(transform.matrix._41, transform.matrix._43));
	if (curTile) {
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


			if (goal == curTile) {
				Console::WriteLine << "We made it to our goal.";
				rb->Stop();
			}
			else {
				//if (KeyIsHit(Control::TestInputO)) {
				//if (curTile == next) {
					PathPlanner::UpdateMTDStar(mtdstarId);

					next = PathPlanner::GetMTDStarNextTile(mtdstarId);
					if (next) {
						auto nextPathPoint = grid->TileToPoint(next);
						//transform.SetPosition(nextPathPoint.x, 0.0f, nextPathPoint.y);
						DirectX::XMFLOAT3 nextDirection;
						DirectX::XMStoreFloat3(&nextDirection, DirectX::XMVector3Normalize(DirectX::XMVectorSet(nextPathPoint.x - transform.matrix._41, 0.0f, nextPathPoint.y - transform.matrix._43, 1.0f)));
						float dotProd = (nextDirection.x * transform.matrix._31 + nextDirection.y * transform.matrix._32 + nextDirection.z * transform.matrix._33);
						Console::WriteLine << "Dot Product:" << dotProd;
						rb->AddForce(4.0f * (2.0f - dotProd), nextDirection.x, nextDirection.y, nextDirection.z, 0.02f);
						Console::WriteLine << "Current Pos: " << "(" << transform.matrix._41 << ", " << transform.matrix._42 << ", " << transform.matrix._43 << ")";
						Console::WriteLine << "Next Point: " << "(" << nextPathPoint.x << ", " << 0.0f << ", " << nextPathPoint.y << ")";
						Console::WriteLine << "Next Direction: " << "(" << nextDirection.x << ", " << nextDirection.y << ", " << nextDirection.z << ")";
						//Console::WriteLine << "Velocity: " << "(" << DirectX::XMVectorGetX(velocity) << ", " << DirectX::XMVectorGetY(velocity) << ", " << DirectX::XMVectorGetZ(velocity) << ")";
					}
					else {
						rb->Stop();
						Console::WriteLine << "There's no next path for me to GO!!";
					}
				//}
			}
			//}
		//}
	}

}
