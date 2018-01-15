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

	Object* left_controller;
	Object* right_controller;

public:
	DirectX::XMMATRIX hmdPose;
	DirectX::XMMATRIX controller1Pose;
	DirectX::XMMATRIX controller2Pose;
	vr::IVRSystem *pVRHMD;
	vr::IVRRenderModels* pVRRenderModel;
	vr::IVRCompositor* pVRCompositor;
	uint32_t RecommendedRenderHeight, RecommendedRenderWidth;

	VRManager();
	~VRManager();

	bool Init();
	void CreateControllers()
	{
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,0,0,0 }, &left_controller));
		MessageEvents::SendMessage(EVENT_InstantiateRequest, InstantiateMessage(0, { 0,0,0,0 }, &right_controller));
	}
	void RegisterController(Object* _left, Object* _right) {

		left_controller = _left;
		right_controller = _right;
	}

	void GetVRMatricies(DirectX::XMFLOAT4X4* leftProj, DirectX::XMFLOAT4X4* rightProj, DirectX::XMFLOAT4X4* leftView, DirectX::XMFLOAT4X4* rightView);
	void SendToHMD(void* leftTexture, void* rightTexture);
};

