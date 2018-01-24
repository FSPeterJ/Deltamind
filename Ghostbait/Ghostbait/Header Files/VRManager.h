#pragma once
#include "openvr.h"
#include "ObjectManager.h"
#include <DirectXMath.h>
#include "GameObject.h"

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
	struct LeftVRController {
		int index;
		DirectX::XMFLOAT4X4 pose = FLOAT4X4Identity;
		LeftControllerObject* obj;
	};
	struct RightVRController {
		int index;
		DirectX::XMFLOAT4X4 pose = FLOAT4X4Identity;
		RightControllerObject* obj;
	};

	static DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 hmdPose = FLOAT4X4Identity;
	static LeftVRController leftController;
	static RightVRController rightController;

	vr::IVRSystem *pVRHMD;
	vr::IVRRenderModels* pVRRenderModel;
	vr::IVRCompositor* pVRCompositor;
	uint32_t RecommendedRenderHeight, RecommendedRenderWidth;

	VRManager();
	~VRManager();

	bool Init();

	void CreateControllers() {
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(TypeMap::getTypeId<LeftControllerObject>(), {0,0,0,1}, (Object**) &leftController.obj));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(TypeMap::getTypeId<RightControllerObject>(), {1,0,100,1}, (Object**) &rightController.obj));
	}
	//void RegisterController(ControllerObject* _left, ControllerObject* _right) {
	//
	//	leftController.obj = _left;
	//	rightController.obj= _right;
	//}

	void GetVRMatrices(DirectX::XMFLOAT4X4* leftProj, DirectX::XMFLOAT4X4* rightProj, DirectX::XMFLOAT4X4* leftView, DirectX::XMFLOAT4X4* rightView);
	void SendToHMD(void* leftTexture, void* rightTexture);
};
