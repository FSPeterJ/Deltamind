#pragma once
#include <string>
#include <DirectXMath.h>

struct animJoint {
	DirectX::XMFLOAT4X4 transform;
	int parent_index;
	std::string name;
};
