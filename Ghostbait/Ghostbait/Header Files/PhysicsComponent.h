#pragma once
#include <DirectXMath.h>
#include <vector>
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

struct SphereCollider : public ColliderData {
	float radius;
};

struct CapsuleCollider : public ColliderData {
	float radius;
	float height;
};

struct BoxCollider : public ColliderData {
	DirectX::XMFLOAT3 topLeftFrontCorner;
	DirectX::XMFLOAT3 bottRightBackCorner;
};

struct Collider {
	ColliderData* colliderData;
	DirectX::XMFLOAT3 centerOffset;
	bool isTrigger = false;
};

struct PhysicsComponent : public ComponentBase {
	Object* srcObj = nullptr;
	//RidgidBody ridgidBody;
	std::vector<Collider> colliders;
};