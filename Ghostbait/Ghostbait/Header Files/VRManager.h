#pragma once
#include "openvr.h"
#include <DirectXMath.h>
#include "ControllerObject.h"

#define FLOAT4X4Identity DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

class VRManager {
private:
	DirectX::XMFLOAT4X4 leftProj;
	DirectX::XMFLOAT4X4 rightProj;
	DirectX::XMFLOAT4X4 leftEyeToHead;
	DirectX::XMFLOAT4X4 rightEyeToHead;

	DirectX::XMFLOAT4X4 VRProjectionToDirectXMatrix(vr::EVREye eye, float nearPlane, float farPlane);
	DirectX::XMFLOAT4X4 VRMatrix34ToDirectXMatrix44(vr::HmdMatrix34_t m);
	DirectX::XMFLOAT4X4 VRMatrix44ToDirectXMatrix44(vr::HmdMatrix44_t m);

	void WriteMatrix(DirectX::XMFLOAT4X4 m, int frame);

	void UpdateVRPoses();
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	void Shutdown();

public:
	struct VRController {
		int index;
		DirectX::XMFLOAT4X4 pose = FLOAT4X4Identity;
		ControllerObject* obj;
	};

	static DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 hmdPose = FLOAT4X4Identity;
	static VRController leftController, rightController;

	vr::IVRSystem *pVRHMD;
	vr::IVRRenderModels* pVRRenderModel;
	vr::IVRCompositor* pVRCompositor;
	uint32_t RecommendedRenderHeight, RecommendedRenderWidth;

	VRManager();
	~VRManager();

	bool Init();

	void CreateControllers() {
		//Left
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, {0,0,0}, (GameObject**) &leftController.obj));
		leftController.obj->SetControllerHand(ControllerObject::ControllerHand::LEFT);
		leftController.obj->AddController(0, 1);
		leftController.obj->AddGun(1, 2, Gun::FireType::SEMI, 60, 1);
		leftController.obj->AddGun(2, 2, Gun::FireType::AUTO, 4, 1);
		//Right
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, {1,0,1}, (GameObject**) &rightController.obj));
		rightController.obj->SetControllerHand(ControllerObject::ControllerHand::RIGHT);
		rightController.obj->AddController(0, 1);
		rightController.obj->AddGun(1, 2, Gun::FireType::SEMI, 60, 1);
		rightController.obj->AddGun(2, 2, Gun::FireType::AUTO, 4, 1);
	}

	void GetVRMatrices(DirectX::XMFLOAT4X4* leftProj, DirectX::XMFLOAT4X4* rightProj, DirectX::XMFLOAT4X4* leftView, DirectX::XMFLOAT4X4* rightView);
	void SendToHMD(void* leftTexture, void* rightTexture);
};
