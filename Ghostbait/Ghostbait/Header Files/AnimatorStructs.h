#pragma once
#include <string>
//#include "Transform.h"
#include <DirectXMath.h>

//This file exists simply to keep DirectXMath out of some h files.

struct animJoint {
	DirectX::XMFLOAT4X4 transform;
	int* child_index = nullptr;
	int child_count = 0;
	int parent_index = -1;
	std::string name;
};
