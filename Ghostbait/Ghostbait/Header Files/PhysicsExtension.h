#pragma once
 
class GameObject;
class Transform;
class Animator;
struct Material;
namespace DirectX {
	struct XMFLOAT3;
}


class CastObject {
	GameObject* object = nullptr;
	GameObject* backup = nullptr;
	Material* mat = nullptr;
	Animator* anim = nullptr;
	char* fileName;
public:
	inline void SetFile(char* _fileName) {fileName = _fileName;}
	void Create(bool renderToFront = false, const char* variantColor = "?");
	inline GameObject* Get() const { return object; };
	inline Material* GetMaterial() const { return mat; };
	inline Animator* GetAnimator() const { return anim; };
	void Destroy();
};



bool Raycast(Transform* transform, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint = nullptr, GameObject** colObject = nullptr, CastObject* ray = nullptr, float maxCastDistance = 100.0f, const char* tag = nullptr);

bool ArcCast(Transform* transform, DirectX::XMFLOAT3* outPos, GameObject** colObject = nullptr, CastObject* arc = nullptr, float maxDistance = 3, float minAngle = 45, float maxAngle = 120, float
	castHeight = 1, const char* tag = nullptr);