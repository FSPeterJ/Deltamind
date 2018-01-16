#pragma once
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class PhysicsComponent;
class Collider;
class ColliderData;

enum ColliderDataType{
	Sphere,
	Capsule,
	Box
};

class PhysicsManager {
	std::vector<PhysicsComponent> components;
	std::vector<ColliderData> dataPrefabs;

	bool Collision(PhysicsComponent component1, PhysicsComponent component2);

	bool SphereToSphereCollision(Collider col1, XMMATRIX& pos1, Collider col2, XMMATRIX& pos2);
	bool BoxToSphereCollision();
	bool CapsuleToSphereCollision();
	bool BoxToBoxCollision();
	bool BoxToCapsuleCollision();
	bool CapsuleToCapsuleCollision();

public:
	PhysicsManager();
	~PhysicsManager();

	bool CreateSphereCollider(float radius);
	bool CreateCapsuleCollider(float radius, float height);
	bool CreateBoxCollider(XMFLOAT3 p1, XMFLOAT3 p2);


};