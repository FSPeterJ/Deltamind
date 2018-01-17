#include "VRManager.h"
#include "Console.h"
#include "Messagebox.h"
#include <string>

VRManager::VRManager()
{
}

VRManager::~VRManager() {
	Shutdown();
}

bool VRManager::Init() {
	vr::EVRInitError error = vr::VRInitError_None;
	pVRHMD = vr::VR_Init(&error, vr::VRApplication_Scene);

	if (error != vr::VRInitError_None) {
		pVRHMD = nullptr;
		Console::Write("Unable to initialize VR: ");
		Console::WriteLine(vr::VR_GetVRInitErrorAsSymbol(error));
		return false;
	}

	pVRHMD->GetRecommendedRenderTargetSize(&RecommendedRenderWidth, &RecommendedRenderHeight);

	pVRRenderModel = (vr::IVRRenderModels*) vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
	if (!pVRRenderModel) {
		Shutdown();
		Console::Write("Unable to get render model: ");
		Console::WriteLine(vr::VR_GetVRInitErrorAsSymbol(error));
		return false;
	}

	pVRCompositor = vr::VRCompositor();
	if (!pVRCompositor) {
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

DirectX::XMMATRIX VRManager::VRProjectionToDirectXMatrix(vr::EVREye eye, float nearPlane, float farPlane) {
	DirectX::XMMATRIX proj;
	
	float top, bott, left, right;
	pVRHMD->GetProjectionRaw(eye, &left, &right, &top, &bott);
	left *= nearPlane;
	right *= nearPlane;
	top *= -1.0f * nearPlane;
	bott *= -1.0f * nearPlane;
	proj = DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bott, top, nearPlane, farPlane);

	return proj;
}
DirectX::XMMATRIX VRManager::VRMatrix34ToDirectXMatrix44(vr::HmdMatrix34_t m) {
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
DirectX::XMMATRIX VRManager::VRMatrix44ToDirectXMatrix44(vr::HmdMatrix44_t m) {
	DirectX::XMMATRIX outM;
	outM = DirectX::XMMATRIX(
		m.m[0][0], m.m[1][0], m.m[2][0] * -1.0f, m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1] * -1.0f, m.m[3][1],
		m.m[0][2] * -1.0f, m.m[1][2] * -1.0f, m.m[2][2], m.m[3][2] * -1.0f,
		m.m[0][3], m.m[1][3], m.m[2][3] * -1.0f, m.m[3][3]
	);

	return outM;
}

void VRManager::WriteMatrix(DirectX::XMMATRIX m, int frame = 60) {
	static int i = 0;
	if (++i >= frame) {
		DirectX::XMFLOAT4X4 mat;
		DirectX::XMStoreFloat4x4(&mat, m);

		Console::WriteLine("-----------------------------------------------------------------------------");
		Console::Write(mat._11); Console::Write(" "); Console::Write(mat._12); Console::Write(" "); Console::Write(mat._13); Console::Write(" "); Console::WriteLine(mat._14);
		Console::Write(mat._21); Console::Write(" "); Console::Write(mat._22); Console::Write(" "); Console::Write(mat._23); Console::Write(" "); Console::WriteLine(mat._24);
		Console::Write(mat._31); Console::Write(" "); Console::Write(mat._32); Console::Write(" "); Console::Write(mat._33); Console::Write(" "); Console::WriteLine(mat._34);
		Console::Write(mat._41); Console::Write(" "); Console::Write(mat._42); Console::Write(" "); Console::Write(mat._43); Console::Write(" "); Console::WriteLine(mat._44);
		i = 0;
	}
}

void VRManager::Shutdown() {
	if (pVRHMD) {
		pVRHMD = nullptr;
		pVRRenderModel = nullptr;
		pVRCompositor = nullptr;
		vr::VR_Shutdown();
	}
}
DirectX::XMMATRIX VRManager::world = DirectX::XMMatrixIdentity();
void VRManager::GetVRMatricies(DirectX::XMFLOAT4X4* _leftProj, DirectX::XMFLOAT4X4* _rightProj, DirectX::XMFLOAT4X4* _leftView, DirectX::XMFLOAT4X4* _rightView) {
	
	UpdateVRPoses();
	hmdPose *= world;
	
	DirectX::XMMATRIX leftView = leftEyeToHead * hmdPose;
	DirectX::XMMATRIX rightView = rightEyeToHead * hmdPose;

	DirectX::XMFLOAT3 leftPos, rightPos;
	DirectX::XMStoreFloat3(&leftPos, leftController.pose.r[3]);
	DirectX::XMStoreFloat3(&rightPos, rightController.pose.r[3]);
	
	//WriteMatrix(, 60);

	DirectX::XMStoreFloat4x4(_leftProj, leftProj);
	DirectX::XMStoreFloat4x4(_rightProj, rightProj);
	DirectX::XMStoreFloat4x4(_leftView, leftView);
	DirectX::XMStoreFloat4x4(_rightView, rightView);
}

void VRManager::UpdateVRPoses() {
	if (!pVRHMD) return;
	
	pVRCompositor->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	
	for (int deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex)
	{
		if (trackedDevicePose[deviceIndex].bPoseIsValid)
		{
			switch (pVRHMD->GetTrackedDeviceClass(deviceIndex)) {
			case vr::TrackedDeviceClass_Controller:
				if(pVRHMD->GetControllerRoleForTrackedDeviceIndex(deviceIndex) == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand){
					leftController.pose = VRMatrix34ToDirectXMatrix44(trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking) *world;
					leftController.index = deviceIndex;
					leftController.obj->position = DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f) * DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(55)) * leftController.pose;// *DirectX::XMMatrixScaling(0.25f, 0.25f, 0.25f);
				}
				else if (pVRHMD->GetControllerRoleForTrackedDeviceIndex(deviceIndex) == vr::ETrackedControllerRole::TrackedControllerRole_RightHand) {
					rightController.pose = VRMatrix34ToDirectXMatrix44(trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking) *world;
					rightController.index = deviceIndex;
					rightController.obj->position = DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f) * DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(55)) * rightController.pose;// *DirectX::XMMatrixScaling(0.25f, 0.25f, 0.25f);
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
