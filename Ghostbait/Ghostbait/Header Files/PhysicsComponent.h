#pragma once
#include "RigidBody.h" // DirectXMath, vector
#include "Object.h"

//class PhysicsManager;

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

#include "ComponentBase.h"
struct PhysicsComponent: public InstantiatedCompBase {
	RigidBody rigidBody;
	std::vector<Collider> colliders;

	bool AddCollider(ColliderData* _colData, float _offsetX = 0.0f, float _offsetY = 0.0f, float _offsetZ = 0.0f, bool _isTrigger = false) {
		if (_colData) {
			Collider temp;
			temp.colliderData = _colData;
			temp.centerOffset = DirectX::XMFLOAT3(_offsetX, _offsetY, _offsetZ);
			temp.isTrigger = _isTrigger;
			colliders.push_back(temp);
			return true;
		}
		return false;
	}
};
