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

	MatToFloatArr(Transpose(pVRHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, 0.1f, 30.0f)), &leftProj);
	MatToFloatArr(Transpose(pVRHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, 0.1f, 30.0f)), &rightProj);
	MatToFloatArr(Transpose(Mat34ToMat44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Left))), &leftEyeToHead);
	MatToFloatArr(Transpose(Mat34ToMat44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Right))), &rightEyeToHead);


	return true;
}

void VRManager::Shutdown() {
	if (pVRHMD) {
		pVRHMD = nullptr;
		pVRRenderModel = nullptr;
		pVRCompositor = nullptr;
		vr::VR_Shutdown();
	}
}

vr::HmdMatrix44_t VRManager::Transpose(vr::HmdMatrix44_t m) {
	vr::HmdMatrix44_t tran;
	tran.m[0][0] = m.m[0][0];
	tran.m[0][1] = m.m[1][0];
	tran.m[0][2] = m.m[2][0];
	tran.m[0][3] = m.m[3][0];

	tran.m[1][0] = m.m[0][1];
	tran.m[1][1] = m.m[1][1];
	tran.m[1][2] = m.m[2][1];
	tran.m[1][3] = m.m[3][1];

	tran.m[2][0] = m.m[0][2];
	tran.m[2][1] = m.m[1][2];
	tran.m[2][2] = m.m[2][2];
	tran.m[2][3] = m.m[3][2];

	tran.m[3][0] = m.m[0][3];
	tran.m[3][1] = m.m[1][3];
	tran.m[3][2] = m.m[2][3];
	tran.m[3][3] = m.m[3][3];
	return tran;
}
void VRManager::MatToFloatArr(vr::HmdMatrix44_t m, float** outM) {
	(*outM) = new float[16];
	(*outM)[0] = m.m[0][0];
	(*outM)[1] = m.m[0][1];
	(*outM)[2] = m.m[0][2];
	(*outM)[3] = m.m[0][3];
	(*outM)[4] = m.m[1][0];
	(*outM)[5] = m.m[1][1];
	(*outM)[6] = m.m[1][2];
	(*outM)[7] = m.m[1][3];
	(*outM)[8] = m.m[2][0];
	(*outM)[9] = m.m[2][1];
	(*outM)[10] = m.m[2][2];
	(*outM)[11] = m.m[2][3];
	(*outM)[12] = m.m[3][0];
	(*outM)[13] = m.m[3][1];
	(*outM)[14] = m.m[3][2];
	(*outM)[15] = m.m[3][3];
}
vr::HmdMatrix44_t VRManager::Mat34ToMat44(vr::HmdMatrix34_t m) {
	vr::HmdMatrix44_t mat;
	mat.m[0][0] = m.m[0][0];
	mat.m[0][1] = m.m[0][1];
	mat.m[0][2] = m.m[0][2];
	mat.m[0][3] = 0;
				  
	mat.m[1][0] = m.m[1][0];
	mat.m[1][1] = m.m[1][1];
	mat.m[1][2] = m.m[1][2];
	mat.m[1][3] = 0;
				  
	mat.m[2][0] = m.m[2][0];
	mat.m[2][1] = m.m[2][1];
	mat.m[2][2] = m.m[2][2];
	mat.m[2][3] = 0;
				  
	mat.m[3][0] = m.m[3][0];
	mat.m[3][1] = m.m[3][1];
	mat.m[3][2] = m.m[3][2];
	mat.m[3][3] = 1;
	return mat;
}
void VRManager::FloatArrTimesFloatArr(float* m1, float* m2, float** outM) {
	(*outM) = new float[16];
	(*outM)[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];	
	(*outM)[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
	(*outM)[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
	(*outM)[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

	(*outM)[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
	(*outM)[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
	(*outM)[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
	(*outM)[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

	(*outM)[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
	(*outM)[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
	(*outM)[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
	(*outM)[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

	(*outM)[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
	(*outM)[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
	(*outM)[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
	(*outM)[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];
}

void VRManager::FloatArrInverse44(float* m, float** invOut) {
	float inv[16], det, result[16];

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return;

	det = 1.0f / det;

	for (int i = 0; i < 16; i++)
		result[i] = inv[i] * det;
	*invOut = result;
}

void VRManager::GetVRMatricies(float** _leftProj, float** _rightProj, float** _leftView, float** _rightView) {
	UpdateVRPoses();

	*_leftProj = leftProj;
	*_rightProj = rightProj;
	FloatArrTimesFloatArr(leftEyeToHead, hmdPose, _leftView);
	FloatArrTimesFloatArr(rightEyeToHead, hmdPose, _rightView);
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
			MatToFloatArr(Transpose(Mat34ToMat44(trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking)), &trackedDevicePoseMatrices[deviceIndex]);
		}
	}
	
	if (trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
		FloatArrInverse44(trackedDevicePoseMatrices[vr::k_unTrackedDeviceIndex_Hmd], &hmdPose);
	}
}

void VRManager::SendToHMD(void* leftTexture, void* rightTexture) {
	vr::EVRCompositorError error;
	vr::Texture_t leftTex = { leftTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };
	vr::Texture_t rightTex = { rightTexture, vr::TextureType_DirectX, vr::ColorSpace_Auto };

	error = pVRCompositor->Submit(vr::EVREye::Eye_Left, &leftTex);
	if (error) Console::Write("Unable to submit left eye texture");
	error = pVRCompositor->Submit(vr::EVREye::Eye_Right, &rightTex);
	if (error) Console::Write("Unable to submit right eye texture");
}