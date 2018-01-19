#pragma once
#include <DirectXMath.h>
#include <vector>
#include "PhysicsComponent.h"

using namespace DirectX;

enum ColliderDataType{
	Sphere,
	Capsule,
	Box
};

class PhysicsManager {
	std::vector<PhysicsComponent> components;

	void CollisionCheck(PhysicsComponent component1, PhysicsComponent component2);

	void TestAllComponentsCollision();
	bool SphereToSphereCollision(Collider col1, XMVECTOR& pos1, Collider col2, XMVECTOR& pos2);
	bool CapsuleToCapsuleCollision(Collider col1, XMMATRIX& pos1, Collider col2, XMMATRIX& pos2);
	bool BoxToBoxCollision();
	void SendCollision(Object* obj1, Object* obj2);
	//bool BoxToCapsuleCollision();
	//bool CapsuleToSphereCollision();
	//bool BoxToSphereCollision();

	static Collider defaultColider;
	static SphereCollider defaultSphereColider;


public:
	PhysicsManager();
	~PhysicsManager();


	//bool CreateSphereCollider(float radius);
	//bool CreateCapsuleCollider(float radius, float height);
	//bool CreateBoxCollider(XMFLOAT3 p1, XMFLOAT3 p2);

	void AddComponent(Object* obj, float veloX = 0.0f, float veloY = 0.0f, float veloZ = 0.0f);
	void Update(float dt);

};