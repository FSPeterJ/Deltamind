#include "VRManager.h"
#include "Console.h"


VRManager::VRManager()
{
}


VRManager::~VRManager()
{
	if (pVRHMD) {
		vr::VR_Shutdown();
	}
}

bool VRManager::init()
{
	vr::EVRInitError error = vr::VRInitError_None;
	pVRHMD = vr::VR_Init(&error, vr::VRApplication_Scene);

	if (error != vr::VRInitError_None) {
		pVRHMD = nullptr;
		Console::Write("Unable to initialize VR: ");
		Console::WriteLine(vr::VR_GetVRInitErrorAsSymbol(error));
		return false;
	}
	return true;
}