#include "VRManager.h"
#include "Console.h"


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
		shutdown();
		Console::Write("Unable to get render model: ");
		Console::WriteLine(vr::VR_GetVRInitErrorAsSymbol(error));
		return false;
	}

	pVRCompositor = vr::VRCompositor();
	if (!pVRCompositor) {
		shutdown();
		Console::WriteLine("Compositor initialization failed! ");
		return false;
	}

	leftProj = MatToFloatArr(Transpose(pVRHMD->GetProjectionMatrix(vr::EVREye::Eye_Left, 0.1f, 30.0f)));
	rightProj = MatToFloatArr(Transpose(pVRHMD->GetProjectionMatrix(vr::EVREye::Eye_Right, 0.1f, 30.0f)));
	leftEyeToHead = MatToFloatArr(Transpose(Mat34ToMat44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Left))));
	rightEyeToHead = MatToFloatArr(Transpose(Mat34ToMat44(pVRHMD->GetEyeToHeadTransform(vr::EVREye::Eye_Right))));


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
	tran.m[0][1] = m.m[1][0];
	tran.m[0][2] = m.m[2][0];
	tran.m[0][3] = m.m[3][0];

	tran.m[1][0] = m.m[0][1];
	tran.m[1][2] = m.m[2][1];
	tran.m[1][3] = m.m[3][1];

	tran.m[2][0] = m.m[0][2];
	tran.m[2][1] = m.m[1][2];
	tran.m[2][3] = m.m[3][2];

	tran.m[3][0] = m.m[0][3];
	tran.m[3][1] = m.m[1][3];
	tran.m[3][2] = m.m[2][3];
	return tran;
}
float* VRManager::MatToFloatArr(vr::HmdMatrix44_t m) {
	float mat[16];
	mat[0] = m.m[0][0];
	mat[1] = m.m[0][1];
	mat[2] = m.m[0][2];
	mat[3] = m.m[0][3];
	mat[4] = m.m[1][0];
	mat[5] = m.m[1][1];
	mat[6] = m.m[1][2];
	mat[7] = m.m[1][3];
	mat[8] = m.m[2][0];
	mat[9] = m.m[2][1];
	mat[10] = m.m[2][2];
	mat[11] = m.m[2][3];
	mat[12] = m.m[3][0];
	mat[13] = m.m[3][1];
	mat[14] = m.m[3][2];
	mat[15] = m.m[3][3];
	return mat;
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
float* VRManager::FloatArrTimesFloatArr(float* m1, float* m2) {
	float result[16];
	
	//		m1					m2
	//	0	1	2	3	:	0	1	2	3
	//	4	5	6	7	:	4	5	6	7
	//	8	9	10	11	:	8	9	10	11
	//	12	13	14	15	:	12	13	14	15

	result[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];	
	result[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
	result[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
	result[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

	result[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
	result[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
	result[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
	result[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

	result[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
	result[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
	result[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
	result[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

	result[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
	result[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
	result[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
	result[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];

	return result;
}

void VRManager::GetVRMatricies(float** _leftProj, float** _rightProj, float** _leftView, float** _rightView) {
	//Get hmdPosition

	*_leftProj = leftProj;
	*_rightProj = rightProj;
	*_leftView = FloatArrTimesFloatArr(leftEyeToHead, hmdPose);
	*_rightView = FloatArrTimesFloatArr(rightEyeToHead, hmdPose);
}

void VRManager::UpdateVRPoses() {
	if (!pVRHMD) {
		return;
	}
	
	pVRCompositor->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
}