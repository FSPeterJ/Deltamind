#include "DebugRenderer.h"
#include "VRManager.h"
#include "Console.h"
#include "ControllerObject.h"
#include "MessageEvents.h"
#include "PhysicsExtension.h"
#include "ObjectFactory.h"
#include "Transform.h"
using namespace Common;

VRManager& VRManager::GetInstance() {
	static VRManager instance;
	return instance;
}

VRManager::VRManager() {}

VRManager::~VRManager() {
	Shutdown();
}

void VRManager::Vibrate(ControllerHand ctrl, unsigned short durationMs) {
	pVRHMD->TriggerHapticPulse(ctrl == ControllerHand::HAND_Left ? VRManager::leftController.index : VRManager::rightController.index, 0, durationMs);
}

bool VRManager::Init(Transform* _player) {
	player = _player;
	vr::EVRInitError error = vr::VRInitError_None;
	pVRHMD = vr::VR_Init(&error, vr::VRApplication_Scene);

	if(error != vr::VRInitError_None) {
		pVRHMD = nullptr;
		Console::WarningLine << "Unable to initialize VR: " << vr::VR_GetVRInitErrorAsSymbol(error);
		return false;
	}

	pVRHMD->GetRecommendedRenderTargetSize(&RecommendedRenderWidth, &RecommendedRenderHeight);

	pVRRenderModel = (vr::IVRRenderModels*) vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
	if(!pVRRenderModel) {
		Shutdown();
		Console::ErrorLine << "Unable to get render model: " << vr::VR_GetVRInitErrorAsSymbol(error);
		return false;
	}

	pVRCompositor = vr::VRCompositor();
	if(!pVRCompositor) {
		Shutdown();
		Console::ErrorLine << "Compositor initialization failed! ";
		return false;
	}

	leftProj = VRProjectionToDirectXMatrix(vr::EVREye::Eye_Left, 0.01f, 500.0f);
	rightProj = VRProjectionToDirectXMatrix(vr::EVREye::Eye_Right, 0.01f, 500.0f);
	leftEyeToHead = VRMatrix34ToXMFLOAT4X4(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Left));
	rightEyeToHead = VRMatrix34ToXMFLOAT4X4(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Right));
	isEnabled = true;

	DirectX::XMStoreFloat4x4(&cRotation, DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(55)));

	return true;
}

void VRManager::SetControllers(ControllerObject* left, ControllerObject* right) {
	leftController.obj = left;
	rightController.obj = right;
}

DirectX::XMFLOAT4X4 VRManager::VRProjectionToDirectXMatrix(vr::EVREye eye, float nearPlane, float farPlane) {
	DirectX::XMFLOAT4X4 proj;

	float top, bott, left, right;
	pVRHMD->GetProjectionRaw(eye, &left, &right, &top, &bott);
	left *= nearPlane;
	right *= nearPlane;
	top *= -nearPlane;
	bott *= -nearPlane;
	DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bott, top, nearPlane, farPlane));

	return proj;
}
DirectX::XMFLOAT4X4 VRManager::VRMatrix34ToXMFLOAT4X4(vr::HmdMatrix34_t& m) {
	vr::HmdMatrix44_t mat;
	memcpy((char*)&mat, (char*)&m, 48);
	memcpy((char*)&mat.m[3], (char*)&wColumn, 16);
	return VRMatrix44ToXMFLOAT4X4(mat);
}
DirectX::XMFLOAT4X4 VRManager::VRMatrix44ToXMFLOAT4X4(vr::HmdMatrix44_t& m) {
	return DirectX::XMFLOAT4X4(
		m.m[0][0],			m.m[1][0],			m.m[2][0] * -1.0f,	m.m[3][0],
		m.m[0][1],			m.m[1][1],			m.m[2][1] * -1.0f,	m.m[3][1],
		m.m[0][2] * -1.0f,	m.m[1][2] * -1.0f,	m.m[2][2],			m.m[3][2] * -1.0f,
		m.m[0][3],			m.m[1][3],			m.m[2][3] * -1.0f,	m.m[3][3]
	);
}
DirectX::XMMATRIX VRManager::VRMatrix34ToXMMATRIX(vr::HmdMatrix34_t& m) {
	vr::HmdMatrix44_t mat;
	memcpy((char*)&mat, (char*)&m, 48);
	memcpy((char*)&mat.m[3], (char*)&wColumn, 16);
	return VRMatrix44ToXMMATRIX(mat);
}
DirectX::XMMATRIX VRManager::VRMatrix44ToXMMATRIX(vr::HmdMatrix44_t& m) {
	return DirectX::XMMatrixSet(
		m.m[0][0], m.m[1][0], m.m[2][0] * -1.0f, m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1] * -1.0f, m.m[3][1],
		m.m[0][2] * -1.0f, m.m[1][2] * -1.0f, m.m[2][2], m.m[3][2] * -1.0f,
		m.m[0][3], m.m[1][3], m.m[2][3] * -1.0f, m.m[3][3]
	);
}

void VRManager::WriteMatrix(DirectX::XMFLOAT4X4 m, int frame = 60) {
	static int i = 0;
	if(++i >= frame) {
		Console::WriteLine << "-----------------------------------------------------------------------------";
		Console::WriteLine << m._11 << " " << m._12 << " " << m._13 << " " << m._14;
		Console::WriteLine << m._21 << " " << m._22 << " " << m._23 << " " << m._24;
		Console::WriteLine << m._31 << " " << m._32 << " " << m._33 << " " << m._34;
		Console::WriteLine << m._41 << " " << m._42 << " " << m._43 << " " << m._44;
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

	DirectX::XMMATRIX mHMDWorldPos = DirectX::XMLoadFloat4x4(&GetPlayerPosition());
	DirectX::XMMATRIX leftView = DirectX::XMLoadFloat4x4(&leftEyeToHead) * mHMDWorldPos;
	DirectX::XMMATRIX rightView = DirectX::XMLoadFloat4x4(&rightEyeToHead) * mHMDWorldPos;

	*_leftProj = leftProj;
	*_rightProj = rightProj;
	DirectX::XMStoreFloat4x4(_leftView, leftView);
	DirectX::XMStoreFloat4x4(_rightView, rightView);
}

void VRManager::UpdateVRPoses() {
	if(!pVRHMD) return;
	vr::TrackedDevicePose_t trackedDevicePos[vr::k_unMaxTrackedDeviceCount];
	pVRCompositor->WaitGetPoses(trackedDevicePos, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	//pVRCompositor->GetLastPoses(trackedDevicePos, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	DirectX::XMMATRIX room_mat = DirectX::XMLoadFloat4x4(&roomPos);
	DirectX::XMMATRIX controllerRotation = DirectX::XMLoadFloat4x4(&cRotation);
	
	for(int deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; ++deviceIndex) {
		vr::TrackedDevicePose_t devicePos = trackedDevicePos[deviceIndex];
		if(devicePos.bPoseIsValid) {
			switch(pVRHMD->GetTrackedDeviceClass(deviceIndex)) {
				case vr::TrackedDeviceClass_Controller:
					if(pVRHMD->GetControllerRoleForTrackedDeviceIndex(deviceIndex) == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand) {
						DirectX::XMStoreFloat4x4(&leftController.pos, DirectX::XMMatrixMultiply(VRMatrix34ToXMMATRIX(devicePos.mDeviceToAbsoluteTracking), room_mat));
						leftController.index = deviceIndex;
						if (leftController.obj) {
							DirectX::XMFLOAT4X4 newPos;
							XMStoreFloat4x4(&newPos, DirectX::XMMatrixMultiply(controllerRotation, DirectX::XMLoadFloat4x4(&leftController.pos)));
							leftController.obj->transform.SetMatrix(newPos);
						}
					}
					else if (pVRHMD->GetControllerRoleForTrackedDeviceIndex(deviceIndex) == vr::ETrackedControllerRole::TrackedControllerRole_RightHand) {
						DirectX::XMStoreFloat4x4(&rightController.pos, DirectX::XMMatrixMultiply(VRMatrix34ToXMMATRIX(devicePos.mDeviceToAbsoluteTracking), room_mat));
						rightController.index = deviceIndex;
						if (rightController.obj) {
							DirectX::XMFLOAT4X4 newPos;
							XMStoreFloat4x4(&newPos, DirectX::XMMatrixMultiply(controllerRotation, DirectX::XMLoadFloat4x4(&rightController.pos)));
							rightController.obj->transform.SetMatrix(newPos);
						}
					}
					break;
				case vr::TrackedDeviceClass_HMD:
					DirectX::XMMATRIX mHMDPose = VRMatrix34ToXMMATRIX(devicePos.mDeviceToAbsoluteTracking);
					DirectX::XMStoreFloat4x4(&playerPos, DirectX::XMMatrixMultiply(mHMDPose, room_mat));
					player->SetMatrix(playerPos);
					break;
				default:
					break;
			}
		}
	}
}

void VRManager::SendToHMD(void* leftTexture, void* rightTexture) {
	vr::Texture_t leftTex = { leftTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };
	vr::Texture_t rightTex = { rightTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };

	pVRCompositor->Submit(vr::EVREye::Eye_Right, &rightTex);
	pVRCompositor->Submit(vr::EVREye::Eye_Left, &leftTex);
}

DirectX::XMFLOAT4X4& VRManager::GetPlayerPosition() {
	return playerPos;
}
DirectX::XMFLOAT4X4& VRManager::GetRoomPosition() {
	return roomPos;
}

void VRManager::Teleport() {
	DirectX::XMFLOAT3 endPos;
	GameObject* colObject = nullptr;
	if(ArcCast(&rightController.obj->transform, &endPos)) {
		MovePlayer(endPos);
	}
}

void VRManager::MovePlayer(DirectX::XMFLOAT3 newPos, bool floorClamp) {
	float deltaX = newPos.x - playerPos._41;
	float deltaY = newPos.y - (floorClamp ? roomPos._42 : playerPos._42);
	float deltaZ = newPos.z - playerPos._43;
	DirectX::XMStoreFloat4x4(&roomPos, DirectX::XMLoadFloat4x4(&roomPos) * DirectX::XMMatrixTranslation(deltaX, deltaY, deltaZ));
}
