#pragma once
#include <vector>
#include "PhysicsComponent.h" //This can probably be forward declared with notable effort
#include "Pool.h"

class GameObject;

#define MAX_PHYSICALS 1024
#define MAX_COLLIDER_DATA 24
#define MAX_PREFABS 50

#include "IComponentManager.h"
class PhysicsManager: public IComponentManager {
	//std::vector<PhysicsComponent> components;
	Pool<PhysicsComponent> components = Pool<PhysicsComponent>(MAX_PHYSICALS);
	std::vector<ColliderData> colliderDataList;
	std::vector<PhysicsComponent> prefabComponents;

	ColliderData* AddColliderData(float _radius);
	ColliderData* AddColliderData(float _radius, float _height);
	ColliderData* AddColliderData(float x1, float y1, float z1, float x2, float y2, float z2);

	void CollisionCheck(PhysicsComponent component1, PhysicsComponent component2);

	void TestAllComponentsCollision();
	bool SphereToSphereCollision(Collider col1, DirectX::XMVECTOR& pos1, Collider col2, DirectX::XMVECTOR& pos2);
	bool CapsuleToCapsuleCollision(Collider col1, DirectX::XMMATRIX& pos1, Collider col2, DirectX::XMMATRIX& pos2);
	bool CapsuleToSphereCollision(Collider capCol, DirectX::XMMATRIX& capPos, Collider sphCol, DirectX::XMMATRIX& sphPos);
	bool BoxToBoxCollision();
	void SendCollision(GameObject* obj1, GameObject* obj2);
	//bool BoxToCapsuleCollision();
	//bool CapsuleToSphereCollision();
	//bool BoxToSphereCollision();
	DirectX::XMVECTOR FindClosestPointOnLine(DirectX::XMVECTOR& _lineSegStart, DirectX::XMVECTOR& _lineSegEnd, DirectX::XMVECTOR& _testPoint);

	static Collider defaultColider;
	static ColliderData defaultSphereColider;

public:
	PhysicsManager();
	~PhysicsManager();

	//bool CreateSphereCollider(float radius);
	//bool CreateCapsuleCollider(float radius, float height);
	//bool CreateBoxCollider(XMFLOAT3 p1, XMFLOAT3 p2);

	void AddComponent(GameObject* obj, float veloX = 0.0f, float veloY = 0.0f, float veloZ = 0.0f);
	PhysicsComponent* CloneComponent(ComponentBase * reference) override;
	PhysicsComponent* GetReferenceComponent(const char * _FilePath, const char* _data) override;
	void ResetComponent(ComponentBase * reset) override;
	void Update();
};
