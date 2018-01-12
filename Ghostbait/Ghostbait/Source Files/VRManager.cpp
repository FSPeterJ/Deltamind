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
		Messagebox::ShowError(L"Unable to initialize VR", L"Check the CONSOLE!!!");

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

	float top, bott, left, right, nearPlane = 0.01f, farPlane = 100.0f;
	pVRHMD->GetProjectionRaw(vr::Eye_Left, &left, &right, &top, &bott);
	left *= nearPlane;
	right *= nearPlane;
	top *= -1.0f * nearPlane;
	bott *= -1.0f * nearPlane;
	//leftProj = DirectX::XMMatrixPerspectiveLH(abs(left - right), abs(bott - top), nearPlane, farPlane);
	leftProj = DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bott, top, nearPlane, farPlane);
	pVRHMD->GetProjectionRaw(vr::Eye_Right, &left, &right, &top, &bott);
	left *= nearPlane;
	right *= nearPlane;
	top *= -1.0f * nearPlane;
	bott *= -1.0f * nearPlane;
	//rightProj = DirectX::XMMatrixPerspectiveLH(abs(left - right), abs(bott - top), nearPlane, farPlane);
	rightProj = DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bott, top, nearPlane, farPlane);

	//rightProj = VRProjectionToDirectXMatrix(pVRHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, 0.001f, 1000.0f));
	//rightProj = VRProjectionToDirectXMatrix(pVRHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, 0.001f, 1000.0f));
	DirectX::XMVECTOR det;
	leftEyeToHead = DirectX::XMMatrixInverse(&det, VRMatrix34ToDirectXMatrix44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Left)));
	rightEyeToHead = DirectX::XMMatrixInverse(&det, VRMatrix34ToDirectXMatrix44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Right)));

	trackedDevicePoseMatrices = new DirectX::XMMATRIX[vr::k_unMaxTrackedDeviceCount];

	return true;
}

DirectX::XMMATRIX VRManager::VRProjectionToDirectXMatrix(vr::HmdMatrix44_t m) {
	DirectX::XMMATRIX outM;
	outM = DirectX::XMMATRIX(
		m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
		m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],// * -1.0f,
		m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
	);

	//outM = DirectX::XMMATRIX(
	//	m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3],
	//	m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3],
	//	m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3],// * -1.0f,
	//	m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]
	//);
	return outM;
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

void VRManager::GetVRMatricies(DirectX::XMFLOAT4X4* _leftProj, DirectX::XMFLOAT4X4* _rightProj, DirectX::XMFLOAT4X4* _leftView, DirectX::XMFLOAT4X4* _rightView) {
	UpdateVRPoses();

	DirectX::XMMATRIX finalLeftViewProj = hmdPose * leftEyeToHead * leftProj;
	DirectX::XMMATRIX finalRightViewProj = hmdPose * rightEyeToHead * rightProj;

	DirectX::XMMATRIX invHMD = DirectX::XMMatrixInverse(&DirectX::XMVectorSet(0, 0, 0, 0), hmdPose);
	DirectX::XMMATRIX invLEFT = DirectX::XMMatrixInverse(&DirectX::XMVectorSet(0, 0, 0, 0), leftEyeToHead);
	DirectX::XMMATRIX invRight = DirectX::XMMatrixInverse(&DirectX::XMVectorSet(0, 0, 0, 0), rightEyeToHead);

	DirectX::XMMATRIX LeftViewProj = leftEyeToHead * invHMD;
	DirectX::XMMATRIX RightViewProj = invRight * invHMD;// *hmdPose;
	
	WriteMatrix(hmdPose, 60);

	DirectX::XMStoreFloat4x4(_leftProj, leftProj);
	DirectX::XMStoreFloat4x4(_rightProj, rightProj);
	DirectX::XMStoreFloat4x4(_leftView, hmdPose);
	DirectX::XMStoreFloat4x4(_rightView, hmdPose);
}

void VRManager::UpdateVRPoses() {
	if (!pVRHMD) {
		return;
	}
	
	pVRCompositor->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	for (int deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex)
	{
		if (trackedDevicePose[deviceIndex].bPoseIsValid)
		{
			trackedDevicePoseMatrices[deviceIndex] = VRMatrix34ToDirectXMatrix44(trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking);
		}
	}
	
	if (trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
		hmdPose = trackedDevicePoseMatrices[vr::k_unTrackedDeviceIndex_Hmd];

		int i = 0;
	}
}

void VRManager::SendToHMD(void* leftTexture, void* rightTexture) {
	vr::EVRCompositorError error;
	vr::Texture_t leftTex = { leftTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };
	vr::Texture_t rightTex = { rightTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };

	error = pVRCompositor->Submit(vr::EVREye::Eye_Left, &leftTex);
	if (error) 
		Console::Write("Unable to submit left eye texture");
	error = pVRCompositor->Submit(vr::EVREye::Eye_Right, &rightTex);
	if (error) 
		Console::Write("Unable to submit right eye texture");
}