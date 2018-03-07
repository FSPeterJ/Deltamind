#pragma once

 
class GameObject;
class Transform;

namespace DirectX {
	struct XMFLOAT3;
}





class ArcObject {
	GameObject* object = nullptr;
	GameObject* backup = nullptr;
public:
	void Create();
	GameObject* Get();
	void Destroy();
};





bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint = nullptr, GameObject** colObject = nullptr, float maxCastDistance = 100.0f, const char* tag = nullptr);

bool ArcCast(Transform* transform, DirectX::XMFLOAT3* outPos, ArcObject* arc = nullptr, float maxDistance = 3, float minAngle = 60, float maxAngle = 120, float
	castHeight = 1, const char* tag = nullptr);