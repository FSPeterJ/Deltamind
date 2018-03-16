#pragma once
 
class GameObject;
class Transform;
namespace DirectX {
	struct XMFLOAT3;
}


class CastObject {
	GameObject* object = nullptr;
	GameObject* backup = nullptr;
	char* fileName;
public:
	inline void SetFile(char* _fileName) {fileName = _fileName;}
	void Create();
	inline GameObject* Get() const { return object; };
	void Destroy();
};



bool Raycast(Transform* transform, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint = nullptr, GameObject** colObject = nullptr, CastObject* ray = nullptr, float maxCastDistance = 100.0f, const char* tag = nullptr);

bool ArcCast(Transform* transform, DirectX::XMFLOAT3* outPos, CastObject* arc = nullptr, float maxDistance = 3, float minAngle = 60, float maxAngle = 120, float
	castHeight = 1, const char* tag = nullptr);