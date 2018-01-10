#pragma once
#include "openvr.h"

class VRManager
{
private:
	float* leftProj;
	float* rightProj;
	float* leftEyeToHead;
	float* rightEyeToHead;
	float* hmdPose;

	vr::HmdMatrix44_t Transpose(vr::HmdMatrix44_t m);
	vr::HmdMatrix44_t Mat34ToMat44(vr::HmdMatrix34_t m);

	void FloatArrInverse44(float* m, float** invOut);
	void MatToFloatArr(vr::HmdMatrix44_t m, float** outM);
	void FloatArrTimesFloatArr(float* m1, float* m2, float** outM);

	void UpdateVRPoses();
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	float* trackedDevicePoseMatrices[vr::k_unMaxTrackedDeviceCount];

public:
	vr::IVRSystem *pVRHMD;
	vr::IVRRenderModels* pVRRenderModel;
	vr::IVRCompositor* pVRCompositor;
	uint32_t RecommendedRenderHeight, RecommendedRenderWidth;

	VRManager();
	~VRManager();

	bool Init();
	void Shutdown();

	void GetVRMatricies(float** leftProj, float** rightProj, float** leftView, float** rightView);
	void SendToHMD(void* leftTexture, void* rightTexture);
};

