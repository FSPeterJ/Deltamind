#pragma once
#include "openvr.h"
#include "ObjectManager.h"
#include <DirectXMath.h>

class VRManager
{
private:
	DirectX::XMMATRIX leftProj;
	DirectX::XMMATRIX rightProj;
	DirectX::XMMATRIX leftEyeToHead;
	DirectX::XMMATRIX rightEyeToHead;

	DirectX::XMMATRIX VRProjectionToDirectXMatrix(vr::EVREye eye, float nearPlane, float farPlane);
	DirectX::XMMATRIX VRMatrix34ToDirectXMatrix44(vr::HmdMatrix34_t m);
	DirectX::XMMATRIX VRMatrix44ToDirectXMatrix44(vr::HmdMatrix44_t m);

	void WriteMatrix(DirectX::XMMATRIX m, int frame);

	void UpdateVRPoses();
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	void Shutdown();



public:
	struct VRController {
		int index;
		DirectX::XMMATRIX pose;
		Object* obj;
	};

	DirectX::XMMATRIX hmdPose;
	VRController leftController;
	VRController rightController;

	vr::IVRSystem *pVRHMD;
	vr::IVRRenderModels* pVRRenderModel;
	vr::IVRCompositor* pVRCompositor;
	uint32_t RecommendedRenderHeight, RecommendedRenderWidth;

	VRManager();
	~VRManager();

	bool Init();

	static DirectX::XMMATRIX world;

	void CreateControllers()
	{
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,0,0,1 }, &leftController.obj));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 1,0,100,1 }, &rightController.obj));
	}
	void RegisterController(Object* _left, Object* _right) {

		leftController.obj = _left;
		rightController.obj= _right;
	}

	void GetVRMatricies(DirectX::XMFLOAT4X4* leftProj, DirectX::XMFLOAT4X4* rightProj, DirectX::XMFLOAT4X4* leftView, DirectX::XMFLOAT4X4* rightView);
	void SendToHMD(void* leftTexture, void* rightTexture);
};

