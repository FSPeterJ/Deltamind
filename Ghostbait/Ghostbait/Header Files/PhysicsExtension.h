#pragma once

namespace DirectX {
	struct XMFLOAT3;
}

class GameObject;

bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint = nullptr, GameObject** colObject = nullptr, float maxCastDistance = 100.0f);

