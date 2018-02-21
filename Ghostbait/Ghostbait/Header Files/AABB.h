#pragma once
#include <DirectXMath.h>

#define PARTITION_UNIT_SIZE 3.0f

struct AABB {
	DirectX::XMFLOAT3 min, max;
	bool isLarge = false;

	bool operator==(const AABB& other);
	bool operator!=(const AABB& other);
	AABB operator=(const AABB& other);

	AABB() {};
	AABB(DirectX::XMFLOAT3& _min, DirectX::XMFLOAT3& _max);
};