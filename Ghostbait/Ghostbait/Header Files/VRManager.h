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
	float* MatToFloatArr(vr::HmdMatrix44_t m);
	float* FloatArrTimesFloatArr(float* m1, float* m2);

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
};

