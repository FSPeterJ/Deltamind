#pragma once
#include "openvr.h"

class VRManager
{
private:
	float* hmdPose;
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	float* trackedDevicePoseMatrices[vr::k_unMaxTrackedDeviceCount];

public:
	vr::IVRSystem *pVRHMD;
	vr::IVRRenderModels* pVRRenderModel;
	vr::IVRCompositor* pVRCompositor;
	uint32_t RecommendedRenderHeight, RecommendedRenderWidth;

	VRManager();
	~VRManager();

	bool init();
	void UpdateVRPoses();
	void shutdown();
};

