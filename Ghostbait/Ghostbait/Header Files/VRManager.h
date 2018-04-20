#pragma once
#include "openvr.h" //todo get rid.  (convert to ptrs and include in cpp) then uncomment namespace below
#include <DirectXMath.h> // same as above
#include "Object.h"
#include "VRStructs.h"

class Transform;
//namespace vr {
//	class IVRSystem;
//	class IVRRenderModels;
//	class IVRCompositor;
//	enum EVREye;
//	struct HmdMatrix34_t;
////	class TrackedDevicePose_t;
//}

class GameObject;
class ControllerObject;

#define FLOAT4X4Identity DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)

class VRManager {
private:
	bool isEnabled = false;

	Transform* player = nullptr;

	DirectX::XMFLOAT4X4 leftProj;
	DirectX::XMFLOAT4X4 rightProj;
	DirectX::XMFLOAT4X4 leftEyeToHead;
	DirectX::XMFLOAT4X4 rightEyeToHead;

	DirectX::XMFLOAT4X4 VRProjectionToDirectXMatrix(vr::EVREye eye, float nearPlane, float farPlane);
	DirectX::XMFLOAT4X4 VRMatrix34ToDirectXMatrix44(vr::HmdMatrix34_t m);
	DirectX::XMFLOAT4X4 VRMatrix44ToDirectXMatrix44(vr::HmdMatrix44_t m);

	vr::IVRRenderModels* pVRRenderModel;
	vr::IVRCompositor* pVRCompositor;

	void WriteMatrix(DirectX::XMFLOAT4X4 m, int frame);

	void UpdateVRPoses();
	void Shutdown();

	DirectX::XMFLOAT4X4 roomPos = FLOAT4X4Identity;
	DirectX::XMFLOAT4X4 hmdPos = FLOAT4X4Identity;
	DirectX::XMFLOAT4X4 playerPos = FLOAT4X4Identity;
public:
	static VRManager& GetInstance();

	struct VRController {
		int index;
		DirectX::XMFLOAT4X4 pos = FLOAT4X4Identity;
		ControllerObject* obj;
	};

	VRController leftController, rightController;

	vr::IVRSystem *pVRHMD;
	uint32_t RecommendedRenderHeight, RecommendedRenderWidth;

	VRManager();
	~VRManager();

	bool Init(Transform* _player);
	inline bool IsEnabled() const { return isEnabled; };

	void Vibrate(ControllerHand ctrl, unsigned short durationMs);
	void SetControllers(ControllerObject* left, ControllerObject* right);
	void SetBuildItems(std::vector<unsigned> prefabIDs);

	void Teleport();
	
	void GetVRMatrices(DirectX::XMFLOAT4X4* _leftProj, DirectX::XMFLOAT4X4* _rightProj, DirectX::XMFLOAT4X4* _leftView, DirectX::XMFLOAT4X4*
	                   _rightView, DirectX::XMMATRIX* _mHMDWorldPos);
	void SendToHMD(void* leftTexture, void* rightTexture);
	DirectX::XMFLOAT4X4& GetPlayerPosition();
	DirectX::XMFLOAT4X4 GetRoomPosition();

	void MovePlayer(DirectX::XMFLOAT3 newPos, bool floorClamp = true);
};
