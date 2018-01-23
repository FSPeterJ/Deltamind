#include "VRManager.h"
#include "Console.h"
#include "Messagebox.h"
#include <string>

DirectX::XMFLOAT4X4 VRManager::world = FLOAT4X4Identity;
VRManager::LeftVRController VRManager::leftController;
VRManager::RightVRController VRManager::rightController;


VRManager::VRManager()
{
}

VRManager::~VRManager() {
	Shutdown();
}

bool VRManager::Init() {
	vr::EVRInitError error = vr::VRInitError_None;
	pVRHMD = vr::VR_Init(&error, vr::VRApplication_Scene);

	if(error != vr::VRInitError_None) {
		pVRHMD = nullptr;
		Console::Write("Unable to initialize VR: ");
		Console::WriteLine(vr::VR_GetVRInitErrorAsSymbol(error));
		return false;
	}

	pVRHMD->GetRecommendedRenderTargetSize(&RecommendedRenderWidth, &RecommendedRenderHeight);

	pVRRenderModel = (vr::IVRRenderModels*) vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
	if(!pVRRenderModel) {
		Shutdown();
		Console::Write("Unable to get render model: ");
		Console::WriteLine(vr::VR_GetVRInitErrorAsSymbol(error));
		return false;
	}

	pVRCompositor = vr::VRCompositor();
	if(!pVRCompositor) {
		Shutdown();
		Console::WriteLine("Compositor initialization failed! ");
		return false;
	}

	leftProj = VRProjectionToDirectXMatrix(vr::EVREye::Eye_Left, 0.01f, 100.0f);
	rightProj = VRProjectionToDirectXMatrix(vr::EVREye::Eye_Right, 0.01f, 100.0f);
	leftEyeToHead = VRMatrix34ToDirectXMatrix44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Left));
	rightEyeToHead = VRMatrix34ToDirectXMatrix44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Right));
	return true;
}

DirectX::XMFLOAT4X4 VRManager::VRProjectionToDirectXMatrix(vr::EVREye eye, float nearPlane, float farPlane) {
	DirectX::XMFLOAT4X4 proj;

	float top, bott, left, right;
	pVRHMD->GetProjectionRaw(eye, &left, &right, &top, &bott);
	left *= nearPlane;
	right *= nearPlane;
	top *= -1.0f * nearPlane;
	bott *= -1.0f * nearPlane;
	DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bott, top, nearPlane, farPlane));

	return proj;
}
DirectX::XMFLOAT4X4 VRManager::VRMatrix34ToDirectXMatrix44(vr::HmdMatrix34_t m) {
	vr::HmdMatrix44_t mat;
	mat.m[0][0] = m.m[0][0];
	mat.m[0][1] = m.m[0][1];
	mat.m[0][2] = m.m[0][2];
	mat.m[0][3] = m.m[0][3];

	mat.m[1][0] = m.m[1][0];
	mat.m[1][1] = m.m[1][1];
	mat.m[1][2] = m.m[1][2];
	mat.m[1][3] = m.m[1][3];

	mat.m[2][0] = m.m[2][0];
	mat.m[2][1] = m.m[2][1];
	mat.m[2][2] = m.m[2][2];
	mat.m[2][3] = m.m[2][3];

	mat.m[3][0] = 0.0f;
	mat.m[3][1] = 0.0f;
	mat.m[3][2] = 0.0f;
	mat.m[3][3] = 1.0f;
	return VRMatrix44ToDirectXMatrix44(mat);
}
DirectX::XMFLOAT4X4 VRManager::VRMatrix44ToDirectXMatrix44(vr::HmdMatrix44_t m) {
	DirectX::XMFLOAT4X4 outM;
	outM = DirectX::XMFLOAT4X4(
		m.m[0][0], m.m[1][0], m.m[2][0] * -1.0f, m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1] * -1.0f, m.m[3][1],
		m.m[0][2] * -1.0f, m.m[1][2] * -1.0f, m.m[2][2], m.m[3][2] * -1.0f,
		m.m[0][3], m.m[1][3], m.m[2][3] * -1.0f, m.m[3][3]
	);

	return outM;
}

void VRManager::WriteMatrix(DirectX::XMFLOAT4X4 m, int frame = 60) {
	static int i = 0;
	if(++i >= frame) {

		Console::WriteLine("-----------------------------------------------------------------------------");
		Console::Write(m._11); Console::Write(" "); Console::Write(m._12); Console::Write(" "); Console::Write(m._13); Console::Write(" "); Console::WriteLine(m._14);
		Console::Write(m._21); Console::Write(" "); Console::Write(m._22); Console::Write(" "); Console::Write(m._23); Console::Write(" "); Console::WriteLine(m._24);
		Console::Write(m._31); Console::Write(" "); Console::Write(m._32); Console::Write(" "); Console::Write(m._33); Console::Write(" "); Console::WriteLine(m._34);
		Console::Write(m._41); Console::Write(" "); Console::Write(m._42); Console::Write(" "); Console::Write(m._43); Console::Write(" "); Console::WriteLine(m._44);
		i = 0;
	}
}

void VRManager::Shutdown() {
	if(pVRHMD) {
		pVRHMD = nullptr;
		pVRRenderModel = nullptr;
		pVRCompositor = nullptr;
		vr::VR_Shutdown();
	}
}
void VRManager::GetVRMatrices(DirectX::XMFLOAT4X4* _leftProj, DirectX::XMFLOAT4X4* _rightProj, DirectX::XMFLOAT4X4* _leftView, DirectX::XMFLOAT4X4* _rightView) {
	UpdateVRPoses();

	DirectX::XMMATRIX mHMDPose;
	mHMDPose = DirectX::XMLoadFloat4x4(&hmdPose);

	mHMDPose *= DirectX::XMLoadFloat4x4(&world);
	DirectX::XMMATRIX leftView = DirectX::XMLoadFloat4x4(&leftEyeToHead) * mHMDPose;
	DirectX::XMMATRIX rightView = DirectX::XMLoadFloat4x4(&rightEyeToHead) * mHMDPose;
	DirectX::XMStoreFloat4x4(&hmdPose, mHMDPose);

	*_leftProj = leftProj;
	*_rightProj = rightProj;
	DirectX::XMStoreFloat4x4(_leftView, leftView);
	DirectX::XMStoreFloat4x4(_rightView, rightView);

}

void VRManager::UpdateVRPoses() {
	if(!pVRHMD) return;

	pVRCompositor->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	for(int deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex)
	{
		if(trackedDevicePose[deviceIndex].bPoseIsValid)
		{
			switch(pVRHMD->GetTrackedDeviceClass(deviceIndex)) {
				case vr::TrackedDeviceClass_Controller:
					if(pVRHMD->GetControllerRoleForTrackedDeviceIndex(deviceIndex) == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand) {
						DirectX::XMStoreFloat4x4(&leftController.pose, DirectX::XMLoadFloat4x4(&VRMatrix34ToDirectXMatrix44(trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking)) * DirectX::XMLoadFloat4x4(&world));
						leftController.index = deviceIndex;
						XMStoreFloat4x4(&leftController.obj->position, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(55)) * DirectX::XMLoadFloat4x4(&leftController.pose));// *DirectX::XMMatrixScaling(0.25f, 0.25f, 0.25f);
					}
					else if(pVRHMD->GetControllerRoleForTrackedDeviceIndex(deviceIndex) == vr::ETrackedControllerRole::TrackedControllerRole_RightHand) {
						DirectX::XMStoreFloat4x4(&rightController.pose, DirectX::XMLoadFloat4x4(&VRMatrix34ToDirectXMatrix44(trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking)) * DirectX::XMLoadFloat4x4(&world));
						rightController.index = deviceIndex;
						XMStoreFloat4x4(&rightController.obj->position, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(55)) * DirectX::XMLoadFloat4x4(&rightController.pose));// *DirectX::XMMatrixScaling(0.25f, 0.25f, 0.25f);
					}
					break;
				case vr::TrackedDeviceClass_HMD:
					hmdPose = VRMatrix34ToDirectXMatrix44(trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking);
					break;
				default:
					break;
			}
		}
	}
}

void VRManager::SendToHMD(void* leftTexture, void* rightTexture) {
	vr::EVRCompositorError error;
	vr::Texture_t leftTex = { leftTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };
	vr::Texture_t rightTex = { rightTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };

	error = pVRCompositor->Submit(vr::EVREye::Eye_Left, &leftTex);
	//if (error) 
		//Console::Write("Unable to submit left eye texture");
	error = pVRCompositor->Submit(vr::EVREye::Eye_Right, &rightTex);
	//if (error) 
	//	Console::Write("Unable to submit right eye texture");
}
