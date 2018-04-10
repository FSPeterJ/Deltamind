#include "ControllerPillar.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "GhostTime.h"
#include "Material.h"
#include "VRManager.h"

void ControllerPillar::Awake(Object* obj) {
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_GetPlayerPos, GetPlayerPosMessage(&playerPos));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/Display_Lifter.ghost")), { 0, 0, 0 }, &lifter));
	isVR = VRManager::GetInstance().IsEnabled();
}
void ControllerPillar::Update() {
	GameObject::Update();
	if (lifter) lifter->transform.SetMatrix(transform.GetMatrix());
	
	//--Base display matrix
	{
		DirectX::XMFLOAT4X4 newMat;
		DirectX::XMStoreFloat4x4(&newMat, DirectX::XMLoadFloat4x4(&transform.GetMatrix()));
		newMat._42 += 1.695f;
		displayCenter.SetMatrix(newMat);
		displayCenter.LookAt(playerPos->GetPosition());
	}
	//--Scale and Rotation matrices
	float scaleFactor = isVR ? 3.0f : 1.0f;
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0, isVR ? rot : 0, 0);
	rot += (float)GhostTime::DeltaTime();
	DirectX::XMMATRIX center = scale * rotation * DirectX::XMMatrixTranslation(displayCenter.GetPosition().x, displayCenter.GetPosition().y, displayCenter.GetPosition().z);
	DirectX::XMFLOAT4X4 adjustmentMat;
	DirectX::XMStoreFloat4x4(&adjustmentMat, center);
	float deltaX = 0.221f;
	float deltaY = 0.221f;
	if (leftDisplay) {
		DirectX::XMFLOAT4X4 newMat = adjustmentMat;
		newMat._41 += displayCenter.GetMatrix()._11 * deltaX;
		newMat._42 += displayCenter.GetMatrix()._12 * deltaX;
		newMat._43 += displayCenter.GetMatrix()._13 * deltaX;
		leftDisplay->transform.SetMatrix(newMat);
	}
	if (rightDisplay) {
		DirectX::XMFLOAT4X4 newMat = adjustmentMat;
		newMat._41 += displayCenter.GetMatrix()._11 * -deltaX;
		newMat._42 += displayCenter.GetMatrix()._12 * -deltaX;
		newMat._43 += displayCenter.GetMatrix()._13 * -deltaX;
		rightDisplay->transform.SetMatrix(newMat);
	}
	if (topDisplay){
		DirectX::XMFLOAT4X4 newMat = displayCenter.GetMatrix();
		newMat._41 += displayCenter.GetMatrix()._21 * deltaY;
		newMat._42 += displayCenter.GetMatrix()._22 * deltaY;
		newMat._43 += displayCenter.GetMatrix()._23 * deltaY;
		topDisplay->transform.SetMatrix(newMat);
	}
	if (bottomDisplay) {
		DirectX::XMFLOAT4X4 newMat = displayCenter.GetMatrix();
		newMat._41 += displayCenter.GetMatrix()._21 * -deltaY;
		newMat._42 += displayCenter.GetMatrix()._22 * -deltaY;
		newMat._43 += displayCenter.GetMatrix()._23 * -deltaY;
		bottomDisplay->transform.SetMatrix(newMat);
	}
	if (centerDisplay) {
		centerDisplay->transform.SetMatrix(displayCenter.GetMatrix());
	}
}
void ControllerPillar::Destroy() {
	if (lifter) lifter->Destroy();
	if (leftDisplay) leftDisplay->Destroy();
	if (rightDisplay) rightDisplay->Destroy();
	if (centerDisplay) centerDisplay->Destroy();
	if (topDisplay) topDisplay->Destroy();
	if (bottomDisplay) bottomDisplay->Destroy();

	GameObject::Destroy();
}

void ControllerPillar_Move::Awake(Object* obj) {
	ControllerPillar::Awake(obj);
	if (isVR) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &leftDisplay));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &rightDisplay));
		leftDisplay->SwapComponentVarient<Material>("default");
		rightDisplay->SwapComponentVarient<Material>("menu");
		leftDisplay->ToggleFlag(UNLIT);
		rightDisplay->ToggleFlag(UNLIT);
	}
	else {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/WASD.ghost")), { 0, 0, 0 }, &centerDisplay));
	}
}
void ControllerPillar_Pause::Awake(Object* obj) {
	ControllerPillar::Awake(obj);
	if (isVR) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &leftDisplay));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &rightDisplay));
		leftDisplay->SwapComponentVarient<Material>("menu");
		rightDisplay->SwapComponentVarient<Material>("default");
		leftDisplay->ToggleFlag(UNLIT);
		rightDisplay->ToggleFlag(UNLIT);
	}
	else {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ESC.ghost")), { 0, 0, 0 }, &centerDisplay));
	}
}
void ControllerPillar_Switch::Awake(Object* obj) {
	ControllerPillar::Awake(obj);
	if (isVR) {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &leftDisplay));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &rightDisplay));
		leftDisplay->SwapComponentVarient<Material>("touchpad");
		rightDisplay->SwapComponentVarient<Material>("touchpad");
		leftDisplay->ToggleFlag(UNLIT);
		rightDisplay->ToggleFlag(UNLIT);
	}
	else {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/123.ghost")), { 0, 0, 0 }, &topDisplay));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/456.ghost")), { 0, 0, 0 }, &bottomDisplay));
	}
}