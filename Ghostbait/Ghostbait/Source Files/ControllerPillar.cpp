#include "ControllerPillar.h"
#include "MessageEvents.h"
#include "ObjectFactory.h"
#include "GhostTime.h"
#include "Material.h"

void ControllerPillar::Awake(Object* obj) {
	GameObject::Awake(obj);
	MessageEvents::SendMessage(EVENT_GetPlayerPos, GetPlayerPosMessage(&playerPos));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/Display_Lifter.ghost")), { 0, 0, 0 }, &lifter));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &rightController));
	MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(ObjectFactory::CreatePrefab(&std::string("Assets/ViveControllerMesh.ghost")), { 0, 0, 0 }, &leftController));
	leftController->ToggleFlag(UNLIT);
	rightController->ToggleFlag(UNLIT);
	SetType("build");
}

//inputToDisplay - string name of the input
//	ex. "move", "pause", "shoot", "switch", "build"
void ControllerPillar::SetType(const char* inputToDisplay) {
	if (inputToDisplay == "move") {
		leftController->SwapComponentVarient<Material>("default");
		rightController->SwapComponentVarient<Material>("menu");
	}
	if (inputToDisplay == "pause") {
		leftController->SwapComponentVarient<Material>("menu");
		rightController->SwapComponentVarient<Material>("default");
	}
	if (inputToDisplay == "switch") {
		leftController->SwapComponentVarient<Material>("touchpad");
		rightController->SwapComponentVarient<Material>("touchpad");
	}
	if (inputToDisplay == "shoot") {
		leftController->SwapComponentVarient<Material>("trigger");
		rightController->SwapComponentVarient<Material>("trigger");
	}
	if (inputToDisplay == "build") {
		leftController->SwapComponentVarient<Material>("grip");
		rightController->SwapComponentVarient<Material>("grip");
	}
}

void ControllerPillar::Update() {
	GameObject::Update();
	if (lifter) lifter->transform.SetMatrix(transform.GetMatrix());
	
	//--Base controller matrix
	{
		DirectX::XMFLOAT4X4 newMat;
		DirectX::XMStoreFloat4x4(&newMat, DirectX::XMLoadFloat4x4(&transform.GetMatrix()));
		newMat._42 += 1.695f;
		controllerCenter.SetMatrix(newMat);
		controllerCenter.LookAt(playerPos->GetPosition());
	}
	//--Scale and Rotation matrices
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(3, 3, 3);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0, rot, 0);
	rot += (float)GhostTime::DeltaTime();

	float deltaX = 0.221f;
	if (leftController) {
		DirectX::XMFLOAT4X4 newMat;
		DirectX::XMMATRIX result = scale * rotation * DirectX::XMMatrixTranslation(controllerCenter.GetPosition().x, controllerCenter.GetPosition().y, controllerCenter.GetPosition().z);
		DirectX::XMStoreFloat4x4(&newMat, result);
		newMat._41 += controllerCenter.GetMatrix()._11 * deltaX;
		newMat._42 += controllerCenter.GetMatrix()._12 * deltaX;
		newMat._43 += controllerCenter.GetMatrix()._13 * deltaX;
		leftController->transform.SetMatrix(newMat);

	}
	if (rightController) {
		DirectX::XMFLOAT4X4 newMat;
		DirectX::XMMATRIX result = scale * rotation * DirectX::XMMatrixTranslation(controllerCenter.GetPosition().x, controllerCenter.GetPosition().y, controllerCenter.GetPosition().z);
		DirectX::XMStoreFloat4x4(&newMat, result);
		newMat._41 += controllerCenter.GetMatrix()._11 * -deltaX;
		newMat._42 += controllerCenter.GetMatrix()._12 * -deltaX;
		newMat._43 += controllerCenter.GetMatrix()._13 * -deltaX;
		rightController->transform.SetMatrix(newMat);
	}
}



void ControllerPillar::Destroy() {
	if (lifter) lifter->Destroy();
	if (leftController) leftController->Destroy();
	if (rightController) rightController->Destroy();
	GameObject::Destroy();
}