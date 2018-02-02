#pragma once

namespace DirectX {
	struct XMFLOAT3;
}

struct AABB {
	DirectX::XMFLOAT3* min, *max;

	bool operator==(const AABB& other);
	bool operator!=(const AABB& other);
};
