#pragma once
#include "openvr.h"

class VRManager
{
public:
	vr::IVRSystem *pVRHMD;

	VRManager();
	~VRManager();

	bool init();
};

