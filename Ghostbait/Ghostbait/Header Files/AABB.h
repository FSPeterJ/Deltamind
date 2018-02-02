#pragma once

namespace DirectX {
	struct XMFLOAT3;
}

struct AABB {
	DirectX::XMFLOAT3* min, *max;

	bool operator==(const AABB& other);
	bool operator!=(const AABB& other);
	AABB operator=(const AABB& other) {
		this->max = other.max;
		this->min = other.min;
		return *this;
	}

	AABB() {};
	AABB(DirectX::XMFLOAT3& _min, DirectX::XMFLOAT3& _max) { min = _min; max = _max; };
};