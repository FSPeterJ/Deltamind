#include "Player.h"
#include "InputManager.h"
#include "VRManager.h"
#include "MessageEvents.h"
#include "GhostTime.h"
#include "PhysicsExtension.h"
#include "ObjectFactory.h"
#include "HexGrid.h"
#include "BuildTool.h"
#include "DebugRenderer.h"


Player::Player() {
	Enable();
	PersistOnReset();
	teleportArc.SetFile("Assets/Arc3.ghost");
	VRManager::GetInstance().Init(&transform);
	transform.SetPosition(0, 1.7f, 0);
	transform.LookAt({ 0, 1.7f, 1 });
	MessageEvents::Subscribe(EVENT_God, [=](EventMessageBase* e) {this->GodDetected(); });
	MessageEvents::Subscribe(EVENT_GetPlayer, [=](EventMessageBase* e) {this->GiveTransform(e); });
}

void Player::GiveTransform(EventMessageBase* e) {
	GetPlayerMessage* message = (GetPlayerMessage*)e;
	Player const** data = message->RetrieveData();
	(*data) = this;
}
void Player::ChangeStance(Stance newStance) {
	if (stance == newStance) return;
	stance = newStance;
	switch (newStance) {
		case STANCE_Stand:
			stance = STANCE_Stand;
			playerHeight = standHeight;
			playerSpeed = walkSpeed;
			transform.SetPosition(transform.GetPosition().x, playerHeight, transform.GetPosition().z);
			break;
		case STANCE_Crouch:
			stance = STANCE_Crouch;
			playerHeight = crouchHeight;
			playerSpeed = crouchSpeed;
			transform.SetPosition(transform.GetPosition().x, playerHeight, transform.GetPosition().z);
			break;
		case STANCE_God:
			playerHeight = standHeight;
			playerSpeed = runSpeed;
			transform.SetPosition(transform.GetPosition().x, playerHeight, transform.GetPosition().z);
			break;
	}
}

void Player::GodDetected() {
	if (IsGod()) {
		ChangeStance(STANCE_Stand);
		//MessageEvents::SendQueueMessage(EVENT_Late, [=]() {if (this->editItem) editItem->Destroy(); });
	}
	else{
		ChangeStance(STANCE_God);
		MessageEvents::SendMessage(EVENT_BecameGod, EventMessageBase());
		//MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/Monitor.ghost")), { 0, 0, 0 }, &editItem));
	}
}

void Player::Update() {
	float dt = (float)GhostTime::DeltaTime();

	if (IsVR()) {
		transform.SetMatrix(VRManager::GetInstance().GetPlayerPosition());
		
		if (IsGod()) {
			//Fly
			if (KeyIsDown(Control::teleportDown)) {
				DirectX::XMFLOAT4X4 playerMat = VRManager::GetInstance().GetPlayerPosition();
				DirectX::XMFLOAT4X4 controllerMat = rightController->transform.GetMatrix();
				playerMat._41 += controllerMat._31;
				playerMat._42 += controllerMat._32;
				playerMat._43 += controllerMat._33;
				VRManager::GetInstance().MovePlayer({ playerMat._41, playerMat._42, playerMat._43 }, false);
			}
		}
	}
	else {
		
		DirectX::XMFLOAT3 prevPos = transform.GetPosition();

		if (KeyIsDown(Control::Crouch)) {
			ResetKey(Control::Crouch);
			if (stance == STANCE_Stand) ChangeStance(STANCE_Crouch);
			else if (stance == STANCE_Crouch) ChangeStance(STANCE_Stand);
		}
		if (KeyIsDown(Control::Sprint)) {
			switch (stance) {
				case STANCE_God:
					playerSpeed = godSpeed;
					break;
				case STANCE_Crouch:
					ChangeStance(STANCE_Stand);
				case STANCE_Stand:
					playerSpeed = runSpeed;
					break;
			}
		}
		else {
			switch (stance) {
				case STANCE_God:
					playerSpeed = runSpeed;
					break;
				case STANCE_Stand:
					playerSpeed = walkSpeed;
					break;
			}
		}

		if (KeyIsDown(Control::CameraLeftRight)) {
			//position._41 -= 50.0f * dt;
			float prevY = rotationY;
			rotationY += Amount(CameraLeftRight) * sensitivity;
			ResetKey(Control::CameraLeftRight);
		}
		if (KeyIsDown(Control::CameraUpDown)) {
			//position._41 += 50.0f * dt;
			rotationX += Amount(CameraUpDown) * sensitivity;
			ResetKey(Control::CameraUpDown);
		}
		if (KeyIsDown(Control::forward)) {
			if (stance != STANCE_God) {
				//Oriented Matrix
				DirectX::XMMATRIX newMat_M = DirectX::XMLoadFloat4x4(&transform.GetMatrix());
				newMat_M.r[1] = DirectX::XMVectorSet(0, 1, 0, 0);
				newMat_M.r[2] = DirectX::XMVector3Cross(newMat_M.r[0], newMat_M.r[1]);

				//Translate from oriented mat
				DirectX::XMFLOAT4X4 tempMat;
				DirectX::XMStoreFloat4x4(&tempMat, newMat_M);
				transform.SetMatrix(tempMat);
			}
			transform.MoveAlongForward(playerSpeed);
		}
		if (KeyIsDown(Control::backward)) {
			if (stance != STANCE_God) {
				//Oriented Matrix
				DirectX::XMMATRIX newMat_M = DirectX::XMLoadFloat4x4(&transform.GetMatrix());
				newMat_M.r[1] = DirectX::XMVectorSet(0, 1, 0, 0);
				newMat_M.r[2] = DirectX::XMVector3Cross(newMat_M.r[0], newMat_M.r[1]);

				//Translate from oriented mat
				DirectX::XMFLOAT4X4 tempMat;
				DirectX::XMStoreFloat4x4(&tempMat, newMat_M);
				transform.SetMatrix(tempMat);
			}
			transform.MoveAlongForward(-playerSpeed);
			//ResetKey(Control::backward);
		}
		if (KeyIsDown(Control::LeftAction)) {
			transform.MoveAlongUp(playerSpeed);
			//ResetKey(Control::leftAttack);
		}
		if (KeyIsDown(Control::RightAction)) {
			transform.MoveAlongUp(-playerSpeed);
			//ResetKey(Control::rightAttack);
		}

		if (KeyIsDown(Control::left)) {
			transform.MoveAlongSide(-(playerSpeed * 0.6f));
			//ResetKey(Control::TestInputZ);
		}
		if (KeyIsDown(Control::right)) {
			transform.MoveAlongSide(playerSpeed * 0.6f);
			//ResetKey(Control::TestInputC);
		}

		//Rotate Camera
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

		//Ground Clamp
		if (!IsGod()) {
			//Ground Clamp
			Transform start;
			start.SetPosition(transform.GetMatrix()._41, transform.GetMatrix()._42 - playerHeight + 0.1f, transform.GetMatrix()._43);
			DirectX::XMFLOAT3 direction = { 0, -1, 0 };
			DirectX::XMFLOAT3 end;
			HexTile* tile = grid->PointToTile(DirectX::XMFLOAT2(transform.GetPosition().x, transform.GetPosition().z));
			if (Raycast(&start, direction, &end, nullptr, nullptr, 100, "Ground") && tile && !grid->IsBlocked(tile)) {
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

		if (KeyIsDown(Control::CameraLeftRight)) {
			//position._41 -= 50.0f * dt;
			rotationY += Amount(CameraLeftRight) * sensitivity;
			ResetKey(Control::CameraLeftRight);
			//ResetKey(Control::left);
		}
		if (KeyIsDown(Control::CameraUpDown)) {
			//position._41 += 50.0f * dt;
			rotationX += Amount(CameraUpDown) * sensitivity;
			ResetKey(Control::CameraUpDown);
			//ResetKey(Control::right);
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
	}
}

void Player::Teleport(DirectX::XMFLOAT3 pos) {
	if (IsVR()) {
		VRManager::GetInstance().MovePlayer(pos);
	}
}
void Player::Teleport() {
	if (IsVR())
		VRManager::GetInstance().Teleport();
}

void Player::InitControllers(VRControllerTypes type) {
	//Left
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ControllerObject>({ 0,0,0 }, &leftController));
	leftController->Init(this, ControllerHand::HAND_Left);
	//Right
	MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<ControllerObject>({ 1,0,1 }, &rightController));
	rightController->Init(this, ControllerHand::HAND_Right);


	if (IsVR()) {
		VRManager::GetInstance().SetControllers(leftController, rightController);
	}
}

void Player::SetBuildToolData(HexGrid* _grid, GameData* _gameData) {
	grid = _grid;
	BuildTool* buildTool = leftController->GetBuildTool();
	if (buildTool) {
		buildTool->SetGrid(_grid);
		buildTool->SetGameData(_gameData);
	}
	buildTool = rightController->GetBuildTool();
	if (buildTool) {
		buildTool->SetGrid(_grid);
		buildTool->SetGameData(_gameData);
	}
}

bool Player::IsVR() const {
	return VRManager::GetInstance().IsEnabled();
}
