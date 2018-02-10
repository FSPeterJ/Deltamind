#pragma once
//#include "RigidBody.h" // DirectXMath, vector
#include "ComponentBase.h"
#include <vector>

class AABB;
class RigidBody;

//class PhysicsManager;

namespace DirectX
{
	struct XMFLOAT3;
}

enum ColliderType {
	SPHERE,
	CAPSULE,
	BOX,
	INVALID,
};

//union candidate

struct ColliderData {
	ColliderType colliderType;
	union Data {
		struct SphereCollider {
			float radius;
		} sphereCollider;

		struct CapsuleCollider {
			float radius;
			float height;
		} capsuleCollider;

		struct BoxCollider {
			DirectX::XMFLOAT3 topRightFrontCorner;
			DirectX::XMFLOAT3 bottLeftBackCorner;
		} boxCollider;
	} colliderInfo;
};

struct Collider {
	ColliderData* colliderData;
	DirectX::XMFLOAT3 centerOffset;
	bool isTrigger = false;
};

struct PhysicsComponent: public InstantiatedCompBase {
	RigidBody rigidBody;
	std::vector<Collider> colliders;
	AABB baseAABB;
	AABB currentAABB;
	AABB previousAABB;

	//bool operator==(const PhysicsComponent other) {
	//	if (!(this->rigidBody == other.rigidBody)) return false;
	//	if (colliders != other.colliders) return false;
	//	return true;
	//}

	bool AddCollider(ColliderData* _colData, float _offsetX = 0.0f, float _offsetY = 0.0f, float _offsetZ = 0.0f, bool _isTrigger = false);
};

struct Ray {
	DirectX::XMFLOAT3 origin;
	DirectX::XMFLOAT3 direction;
	float currCastTime;

	Ray() {};
	Ray(DirectX::XMFLOAT3 _origin, DirectX::XMFLOAT3 _direction) : origin(_origin), direction(_direction) {};
};