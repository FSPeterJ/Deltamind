#pragma once

 
class Object;
class Transform;

namespace DirectX {
	struct XMFLOAT3;
}

class GameObject;

bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint = nullptr, GameObject** colObject = nullptr, float maxCastDistance = 100.0f, const char* tag = nullptr);

bool ArcCast(Transform* transform, DirectX::XMFLOAT3* outPos, float maxDistance = 3, float minAngle = 60, float maxAngle = 120, float
	castHeight = 1, const char* tag = nullptr);