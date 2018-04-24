#pragma once
#include "IComponentManager.h"
#include "Pool.h"
#include "SpatialPartition.h"

struct Collider;
struct ColliderData;
struct PhysicsComponent;

namespace DirectX {
	struct XMMATRIX;
}

class GameObject;

#define MAX_PHYSICALS 1024
#define MAX_COLLIDER_DATA 128
#define MAX_PREFABS 100

class PhysicsRegistry:
	public IComponentManager {
	Pool<PhysicsComponent> components = Pool<PhysicsComponent>(MAX_PHYSICALS);
	std::vector<ColliderData> colliderDataList;
	std::vector<PhysicsComponent> prefabComponents;

	static Collider defaultColider;
	static ColliderData defaultSphereColider;
	static SpatialPartition partitionSpace;
public:
	PhysicsRegistry();
	~PhysicsRegistry();
	ComponentBase* GetReferenceComponent(const char * _FilePath, const char * _data) override;
	ComponentBase* CloneComponent(ComponentBase* reference) override;
	void ResetComponent(ComponentBase* reset) override;
	void ActivateComponent(ComponentBase* component) override;
	void DeactivateComponent(ComponentBase* component) override;
	ColliderData* AddColliderData(float _radius);
	ColliderData* AddColliderData(float _radius, float _height);
	ColliderData* AddColliderData(float trfX, float trfY, float trfZ, float blbX, float blbY, float blbZ);
};

