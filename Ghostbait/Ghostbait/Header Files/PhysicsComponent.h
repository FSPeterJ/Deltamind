#pragma once
#include <DirectXMath.h>
#include <vector>
#include "RigidBody.h"
#include "Object.h"

enum ColliderType {
	SPHERE,
	CAPSULE,
	BOX,
	INVALID,
};

struct ColliderData {
	ColliderType colliderType;
};

struct SphereCollider : ColliderData {
	float radius;
};

struct CapsuleCollider : ColliderData {
	float radius;
	float height;
};

struct BoxCollider : ColliderData {
	DirectX::XMFLOAT3 topRightFrontCorner;
	DirectX::XMFLOAT3 bottLeftBackCorner;
};

struct Collider {
	ColliderData* colliderData;
	DirectX::XMFLOAT3 centerOffset;
	bool isTrigger = false;
};

struct PhysicsComponent : public InstantiatedCompBase {
	RigidBody rigidBody;
	std::vector<Collider> colliders;


};