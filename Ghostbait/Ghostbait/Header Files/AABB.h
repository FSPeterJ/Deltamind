#pragma once
#include <DirectXMath.h>

struct AABB {
	DirectX::XMFLOAT3 min, max;

	bool operator==(const AABB& other);
	bool operator!=(const AABB& other);
	AABB operator=(const AABB& other);

	AABB() {};
	AABB(DirectX::XMFLOAT3& _min, DirectX::XMFLOAT3& _max);
};