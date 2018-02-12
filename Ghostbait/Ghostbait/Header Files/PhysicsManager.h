#pragma once
#include <vector>
#include "PhysicsComponent.h" //This can probably be forward declared with notable effort
#include "Pool.h"
#include "SpatialPartition.h"
#include "PhysicsExtension.h"

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
	static SpatialPartition partitionSpace;

	ColliderData* AddColliderData(float _radius);
	ColliderData* AddColliderData(float _radius, float _height);
	ColliderData* AddColliderData(float x1, float y1, float z1, float x2, float y2, float z2);

	void CollisionCheck(PhysicsComponent component1, PhysicsComponent component2);
	void UpdateAABB(PhysicsComponent& component);
	void TestAllComponentsCollision();
	bool IsVectorZero(DirectX::XMVECTOR& _toTest);
	bool SphereToSphereCollision(Collider& col1, DirectX::XMVECTOR& pos1, Collider& col2, DirectX::XMVECTOR& pos2);
	bool CapsuleToCapsuleCollision(Collider& col1, DirectX::XMMATRIX& pos1, Collider& col2, DirectX::XMMATRIX& pos2);
	bool CapsuleToSphereCollision(Collider& capCol, DirectX::XMMATRIX& capPos, Collider& sphCol, DirectX::XMMATRIX& sphPos);
	bool BoxToBoxCollision(Collider& boxCol1, DirectX::XMMATRIX& boxPos1, Collider& boxCol2, DirectX::XMMATRIX& boxPos2);
	bool BoxToSphereCollision(Collider& boxCol, DirectX::XMMATRIX& boxPos, Collider& sphCol, DirectX::XMMATRIX& sphPos);
	bool BoxToCapsuleCollision(Collider& boxCol, DirectX::XMMATRIX& boxPos, Collider& capCol, DirectX::XMMATRIX& capPos);
	std::vector<DirectX::XMVECTOR> GetSATAxis(std::vector<DirectX::XMVECTOR>& boxCorners);
	std::vector<DirectX::XMVECTOR> GetSATAxis(std::vector<DirectX::XMVECTOR>& box1Corners, std::vector<DirectX::XMVECTOR>& box2Corners);
	std::vector<DirectX::XMVECTOR> GetSATAxis(DirectX::XMMATRIX& box1Pos, DirectX::XMMATRIX& box2Pos);
	std::vector<DirectX::XMVECTOR> GetBoxCorners(Collider& boxCol, DirectX::XMMATRIX& boxPos);

	void SendCollision(GameObject* obj1, GameObject* obj2);

	static DirectX::XMVECTOR FindClosestPointOnLine(DirectX::XMVECTOR& _lineSegStart, DirectX::XMVECTOR& _lineSegEnd, DirectX::XMVECTOR& _testPoint);
	static bool IsRayInCollider(DirectX::XMVECTOR& origin, Collider& collidingComp, DirectX::XMVECTOR& objectPos);
	static bool RaycastCollisionCheck(DirectX::XMVECTOR& origin, DirectX::XMVECTOR& direction, PhysicsComponent* collidingComp, DirectX::XMVECTOR* colPoint = nullptr, GameObject* colObject = nullptr, float maxCastDistance = 100.0f);
	static bool RayToSphere(DirectX::XMVECTOR& origin, DirectX::XMVECTOR& direction, Collider& collidingComp, DirectX::XMVECTOR& objectPos, DirectX::XMVECTOR* colPoint = nullptr);
	static bool RayToCapsule(DirectX::XMVECTOR& origin, DirectX::XMVECTOR& direction, Collider& collidingComp, DirectX::XMVECTOR& objectPos, DirectX::XMVECTOR* colPoint = nullptr);
	static bool RayToBox(DirectX::XMVECTOR& origin, DirectX::XMVECTOR& direction, Collider& collidingComp, DirectX::XMVECTOR& objectPos, DirectX::XMVECTOR* colPoint = nullptr);


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
	static bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint = nullptr, GameObject* colObject = nullptr, float maxCastDistance = 100.f);

};
