#include "Player.h"
#include "InputManager.h"
#include "VRManager.h"
#include "MessageEvents.h"
#include "GhostTime.h"

Player::Player() {
	Enable(false);
	VRManager::GetInstance().Init(&transform);
}

void Player::Update() {
	static float rotationY = 0.0f;
	static float rotationX = 0.0f;
	float dt = (float)GhostTime::DeltaTime();
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
	if (KeyIsDown(Control::forward)) {
		transform.MoveAlongForward(10.0f);
		//ResetKey(Control::forward);
	}
	if (KeyIsDown(Control::backward)) {
		transform.MoveAlongForward(-10.0f);
		//ResetKey(Control::backward);
	}
	if (KeyIsDown(Control::LeftAction)) {
		transform.MoveAlongUp(10.0f);
		//ResetKey(Control::leftAttack);
	}
	if (KeyIsDown(Control::RightAction)) {
		transform.MoveAlongUp(-10.0f);
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

	transform.SetRotationRadians(rotationX, rotationY, 0.0f);
}

void Player::Teleport() {
	if (IsVR()) {
		VRManager::GetInstance().Teleport();
	}
}
void Player::LoadControllers(VRControllerTypes type) {
	//Read in from file and assign the correct items

	
	
	//Left
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ControllerObject>({ 0,0,0 }, &leftController));
	leftController->Init(this, ControllerHand::HAND_Left);
	leftController->SetGunData(1, Gun::FireType::SEMI, 60, 50);
	leftController->SetGunData(2, Gun::FireType::AUTO, 8, 25);
	//Right
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ControllerObject>({ 1,0,1 }, &rightController));
	rightController->Init(this, ControllerHand::HAND_Right);
	rightController->SetGunData(1, Gun::FireType::SEMI, 60, 50);
	rightController->SetGunData(2, Gun::FireType::AUTO, 8, 25);

	if (IsVR()) {
		VRManager::GetInstance().SetControllers(leftController, rightController);
	}
}

bool Player::IsVR() const {
	return VRManager::GetInstance().IsEnabled();
}

/*
void Player::SetPosition(DirectX::XMFLOAT4X4 newPos) {
	if(IsVR()) {
		VRManager::GetInstance().MovePlayer({ newPos._41, newPos._42, newPos._43 });
	}
	position = newPos;
}
const DirectX::XMFLOAT4X4& Player::GetPosition() const {
	if (IsVR()) {
		return VRManager::GetInstance().GetPlayerPosition();
	}
	return position;
}
*/