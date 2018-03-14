#include "Player.h"
#include "InputManager.h"
#include "VRManager.h"
#include "MessageEvents.h"
#include "GhostTime.h"
#include "PhysicsExtension.h"
#include "ObjectFactory.h"
#include "HexGrid.h"
#include "BuildTool.h"



/*
static int i = 0;
if (i == 0) {
MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<GameObject>(ObjectFactory::CreatePrefab(&std::string("Assets/Arc2.ghost")), { 0, 0, 0 }, &arc));
i = 1;
}
*/

Player::Player() {
	Enable();
	PersistOnReset();
	VRManager::GetInstance().Init(&transform);
	transform.SetPosition(0, 1.7f, 0);
	transform.LookAt({ 0, 1.7f, 1 });
}

void Player::Update() {
	float dt = (float)GhostTime::DeltaTime();

	if (IsVR()) 
		transform.SetMatrix(VRManager::GetInstance().GetPlayerPosition());
	else {
		DirectX::XMFLOAT3 prevPos = transform.GetPosition();
		float rotationLimit = DirectX::XMConvertToRadians(80);

		if (KeyIsDown(Control::CameraLeftRight)) {
			//position._41 -= 50.0f * dt;
			float prevY = rotationY;
			rotationY += Amount(CameraLeftRight) * 0.01f;
			ResetKey(Control::CameraLeftRight);
			//ResetKey(Control::left);
		}
		if (KeyIsDown(Control::CameraUpDown)) {
			//position._41 += 50.0f * dt;
			rotationX += Amount(CameraUpDown) * 0.01f;
			ResetKey(Control::CameraUpDown);
			//ResetKey(Control::right);
		}
		if (KeyIsDown(Control::forward)) {
			if (!godMode) {
				//Oriented Matrix
				DirectX::XMMATRIX newMat_M = DirectX::XMLoadFloat4x4(&transform.GetMatrix());
				newMat_M.r[1] = DirectX::XMVectorSet(0, 1, 0, 0);
				newMat_M.r[2] = DirectX::XMVector3Cross(newMat_M.r[0], newMat_M.r[1]);

				//Translate from oriented mat
				DirectX::XMFLOAT4X4 tempMat;
				DirectX::XMStoreFloat4x4(&tempMat, newMat_M);
				transform.SetMatrix(tempMat);
			}
			transform.MoveAlongForward(godMode ? 30 : 10.0f);
		}
		if (KeyIsDown(Control::backward)) {
			if (!godMode) {
				//Oriented Matrix
				DirectX::XMMATRIX newMat_M = DirectX::XMLoadFloat4x4(&transform.GetMatrix());
				newMat_M.r[1] = DirectX::XMVectorSet(0, 1, 0, 0);
				newMat_M.r[2] = DirectX::XMVector3Cross(newMat_M.r[0], newMat_M.r[1]);

				//Translate from oriented mat
				DirectX::XMFLOAT4X4 tempMat;
				DirectX::XMStoreFloat4x4(&tempMat, newMat_M);
				transform.SetMatrix(tempMat);
			}
			transform.MoveAlongForward(godMode ? -30 : -10.0f);
			//ResetKey(Control::backward);
		}
		if (KeyIsDown(Control::LeftAction)) {
			transform.MoveAlongUp(godMode ? 30 : 10.0f);
			//ResetKey(Control::leftAttack);
		}
		if (KeyIsDown(Control::RightAction)) {
			transform.MoveAlongUp(godMode ? -30 : -10.0f);
			//ResetKey(Control::rightAttack);
		}

		if (KeyIsDown(Control::left)) {
			transform.MoveAlongSide(-10.0f);
			//ResetKey(Control::TestInputZ);
		}
		if (KeyIsDown(Control::right)) {
			transform.MoveAlongSide(10.0f);
			//ResetKey(Control::TestInputC);
		}

		if (KeyIsDown(Control::TestInputZ)) {
			godMode = !godMode;
			ResetKey(Control::TestInputZ);
		}

		if (rotationX < -rotationLimit) {
			rotationX = -rotationLimit;
		}
		if (rotationX > rotationLimit) {
			rotationX = rotationLimit;
		}
		if (rotationY < -DirectX::XM_2PI || rotationY > DirectX::XM_2PI) {
			rotationY = 0.0f;
		}
		transform.SetRotationRadians(rotationX, rotationY, 0.0f);

		if (!godMode) {
			//Ground Clamp
			DirectX::XMFLOAT3 start = { transform.GetMatrix()._41, transform.GetMatrix()._42 - playerHeight + 0.1f, transform.GetMatrix()._43 };
			DirectX::XMFLOAT3 direction = { 0, -1, 0 };
			DirectX::XMFLOAT3 end;
			HexTile* tile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetPosition().x, transform.GetPosition().z));
			if (Raycast(start, direction, &end, nullptr, 100) && tile && !grid->IsBlocked(tile)) {
				DirectX::XMFLOAT4X4 newPos = transform.GetMatrix();
				newPos._42 = end.y + playerHeight;
				transform.SetMatrix(newPos);
			}
			else {
				transform.SetPosition(prevPos);
			}
		}
	}
}
void Player::PausedUpdate() {
	float dt = (float)GhostTime::DeltaTime();

	if (!IsVR()) {
		if (rotationY < -DirectX::XM_2PI || rotationY > DirectX::XM_2PI)
			rotationY = 0.0f;
		if (rotationX < -DirectX::XM_2PI || rotationX > DirectX::XM_2PI)
			rotationX = 0.0f;

		if (KeyIsDown(Control::CameraLeftRight)) {
			//position._41 -= 50.0f * dt;
			rotationY += Amount(CameraLeftRight) * dt;
			ResetKey(Control::CameraLeftRight);
			//ResetKey(Control::left);
		}
		if (KeyIsDown(Control::CameraUpDown)) {
			//position._41 += 50.0f * dt;
			rotationX += Amount(CameraUpDown) * dt;
			ResetKey(Control::CameraUpDown);
			//ResetKey(Control::right);
		}

		transform.SetRotationRadians(rotationX, rotationY, 0.0f);
	}
}

void Player::Teleport(DirectX::XMFLOAT3* pos) {
	if (IsVR()) {
		if(pos)
			VRManager::GetInstance().MovePlayer(*pos);
		else
			VRManager::GetInstance().Teleport();
	}
}
void Player::LoadControllers(VRControllerTypes type) {
	//Read in from save file and assign the correct items

	//Left
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ControllerObject>({ 0,0,0 }, &leftController));
	leftController->Init(this, ControllerHand::HAND_Left);
	leftController->SetGunData(1, Gun::FireType::SEMI, 60, 50);
	leftController->SetGunData(2, Gun::FireType::AUTO, 8, 25);
	leftController->SetBuildItems({ /*TODO: FIX THIS LATER*/ ObjectFactory::CreatePrefab(&std::string("Assets/TestTurret.ghost")), ObjectFactory::CreatePrefab(&std::string("Assets/HexPole.ghost")) });
	//Right
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ControllerObject>({ 1,0,1 }, &rightController));
	rightController->Init(this, ControllerHand::HAND_Right);
	rightController->SetGunData(1, Gun::FireType::SEMI, 60, 50);
	rightController->SetGunData(2, Gun::FireType::AUTO, 8, 25);
	rightController->SetBuildItems({ /*TODO: FIX THIS LATER*/ ObjectFactory::CreatePrefab(&std::string("Assets/TestTurret.ghost")) });


	if (IsVR()) {
		VRManager::GetInstance().SetControllers(leftController, rightController);
	}
}

void Player::SetBuildGrid(HexGrid* _grid) {
	grid = _grid;
	BuildTool* buildTool = leftController->GetBuildTool();
	if (buildTool) {
		buildTool->SetGrid(_grid);
	}
	buildTool = rightController->GetBuildTool();
	if (buildTool) {
		buildTool->SetGrid(_grid);
	}
}

bool Player::IsVR() const {
	return VRManager::GetInstance().IsEnabled();
}