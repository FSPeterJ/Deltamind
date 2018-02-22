#include "PhysicsRegistry.h"
#include <cstring>
#include "PhysicsComponent.h"
#include "TypeMapping.h"
#include "SpatialPartition.h"


Collider PhysicsRegistry::defaultColider;
ColliderData PhysicsRegistry::defaultSphereColider;
SpatialPartition PhysicsRegistry::partitionSpace;

PhysicsRegistry::PhysicsRegistry() {
	defaultSphereColider.colliderInfo.sphereCollider.radius = 0.5f;
	defaultSphereColider.colliderType = SPHERE;

	defaultColider.centerOffset = { 0, 0, 0 };
	defaultColider.isTrigger = true;
	defaultColider.colliderData = &defaultSphereColider;
	TypeMap::RegisterComponentAlias<PhysicsComponent>("Physical");
	TypeMap::RegisterComponentAlias<PhysicsComponent>("Phys");

	colliderDataList.reserve(MAX_COLLIDER_DATA);
	prefabComponents.reserve(MAX_PREFABS);
}


PhysicsRegistry::~PhysicsRegistry() {
}

ComponentBase* PhysicsRegistry::GetReferenceComponent(const char* _FilePath, const char* _dataBlock) {
	PhysicsComponent compHolder;
	DirectX::XMFLOAT3 offsetHolder;
	DirectX::XMFLOAT3 aabbMax(FLT_MIN_EXP, FLT_MIN_EXP, FLT_MIN_EXP);
	DirectX::XMFLOAT3 aabbMin(FLT_MAX_EXP, FLT_MAX_EXP, FLT_MAX_EXP);
	DirectX::XMFLOAT3 incomingMax = aabbMax, incomingMin = aabbMin;

	ColliderData* colDataHolder = nullptr;
	int numofColliders, lengthofTypeName, currIndex = 0;
	float radiusHolder, heightHolder;
	char* typeName;

	char sphere[] = "SPHERE";
	char capsule[] = "CAPSULE";
	char box[] = "BOX";

	memcpy(&numofColliders, &_dataBlock[currIndex], sizeof(numofColliders));
	currIndex += sizeof(numofColliders);

	for(int i = 0; i < numofColliders; ++i) {
		memcpy(&lengthofTypeName, &_dataBlock[currIndex], sizeof(lengthofTypeName));
		currIndex += sizeof(lengthofTypeName);
		typeName = new char[lengthofTypeName];
		memcpy(&typeName[0], &_dataBlock[currIndex], lengthofTypeName);
		currIndex += lengthofTypeName;
		memcpy(&offsetHolder, &_dataBlock[currIndex], sizeof(float) * 3);
		currIndex += sizeof(float) * 3;

		if(!strcmp(typeName, sphere)) {
			memcpy(&radiusHolder, &_dataBlock[currIndex], sizeof(radiusHolder));
			currIndex += sizeof(radiusHolder);
			colDataHolder = AddColliderData(radiusHolder);

			incomingMax = { offsetHolder.x + radiusHolder, offsetHolder.y + radiusHolder, offsetHolder.z + radiusHolder };
			incomingMin = { offsetHolder.x - radiusHolder, offsetHolder.y - radiusHolder, offsetHolder.z - radiusHolder };
		}
		else if(!strcmp(typeName, capsule)) {
			memcpy(&radiusHolder, &_dataBlock[currIndex], sizeof(radiusHolder));
			currIndex += sizeof(radiusHolder);
			memcpy(&heightHolder, &_dataBlock[currIndex], sizeof(heightHolder));
			currIndex += sizeof(heightHolder);
			colDataHolder = AddColliderData(radiusHolder, heightHolder);

			incomingMax = { offsetHolder.x + radiusHolder, offsetHolder.y + radiusHolder + (heightHolder * 0.5f), offsetHolder.z + radiusHolder };
			incomingMin = { offsetHolder.x - radiusHolder, offsetHolder.y - radiusHolder - (heightHolder * 0.5f), offsetHolder.z - radiusHolder };
		}
		else if(!strcmp(typeName, box)) {
			memcpy(&incomingMax, &_dataBlock[currIndex], sizeof(float) * 3);
			currIndex += sizeof(float) * 3;
			memcpy(&incomingMin, &_dataBlock[currIndex], sizeof(float) * 3);
			currIndex += sizeof(float) * 3;
			colDataHolder = AddColliderData(incomingMax.x, incomingMax.y, incomingMax.z, incomingMin.x, incomingMin.y, incomingMin.z);

			//incomingMax = { incomingMax.x + radiusHolder, incomingMax.y + radiusHolder, incomingMax.z + radiusHolder };
			//incomingMin = { incomingMin.x - radiusHolder, incomingMin.y - radiusHolder, incomingMin.z - radiusHolder };
		}

		//Update AABB
		if(incomingMax.x > aabbMax.x) { aabbMax.x = incomingMax.x; };
		if(incomingMax.y > aabbMax.y) { aabbMax.y = incomingMax.y; };
		if(incomingMax.z > aabbMax.z) { aabbMax.z = incomingMax.z; };
		if(incomingMin.x < aabbMin.x) { aabbMin.x = incomingMin.x; };
		if(incomingMin.y < aabbMin.y) { aabbMin.y = incomingMin.y; };
		if(incomingMin.z < aabbMin.z) { aabbMin.z = incomingMin.z; };

		compHolder.AddCollider(colDataHolder, offsetHolder.x, offsetHolder.y, offsetHolder.z);
		colDataHolder = nullptr;
		delete[] typeName; //TODO: typeName is allocated with new[] but is only being deleted with delete (not delete[])
	}

	compHolder.previousAABB = compHolder.currentAABB = compHolder.baseAABB = AABB(aabbMin, aabbMax);

	if(prefabComponents.size() < MAX_PREFABS) {
		prefabComponents.push_back(compHolder);
		return &prefabComponents.back();
	}
	else {
		throw "If it breaks here, you ran out of prefab space. change MAX_PREFABS ";
		return nullptr;
	}
}

ComponentBase* PhysicsRegistry::CloneComponent(ComponentBase* reference) {
	PhysicsComponent* physComponent = components.ActivateMemory();
	// SHALLOW COPY - this only copies the std::vector head.
	physComponent->colliders = ((PhysicsComponent*)reference)->colliders;	physComponent->currentAABB = ((PhysicsComponent*)reference)->currentAABB;
	physComponent->previousAABB = ((PhysicsComponent*)reference)->previousAABB;
	physComponent->baseAABB = ((PhysicsComponent*)reference)->baseAABB;

	partitionSpace.AddComponent(physComponent);

	return physComponent;
}

void PhysicsRegistry::ResetComponent(ComponentBase* reset) {
	components.DeactivateMemory(reset);
	partitionSpace.RemoveComponent((PhysicsComponent*)reset);
}

void PhysicsRegistry::ActivateComponent(ComponentBase* component) {
}

void PhysicsRegistry::DeactivateComponent(ComponentBase* component) {
}



ColliderData* PhysicsRegistry::AddColliderData(float _radius) {
	if(colliderDataList.size() < MAX_COLLIDER_DATA) {
		ColliderData temp;
		temp.colliderType = SPHERE;
		temp.colliderInfo.sphereCollider.radius = _radius;
		colliderDataList.push_back(temp);
		return &colliderDataList.back();
	}
	return nullptr;
}
ColliderData* PhysicsRegistry::AddColliderData(float _radius, float _height) {
	if(colliderDataList.size() < MAX_COLLIDER_DATA) {
		ColliderData temp;
		temp.colliderType = CAPSULE;
		temp.colliderInfo.capsuleCollider.radius = _radius;
		temp.colliderInfo.capsuleCollider.height = _height;

		colliderDataList.push_back(temp);
		return &colliderDataList.back();
	}
	return nullptr;
}
ColliderData* PhysicsRegistry::AddColliderData(float trfX, float trfY, float trfZ, float blbX, float blbY, float blbZ) {
	if(colliderDataList.size() < MAX_COLLIDER_DATA) {
		ColliderData temp;
		temp.colliderType = BOX;
		temp.colliderInfo.boxCollider.topRightFrontCorner = DirectX::XMFLOAT3(trfX, trfY, trfZ);
		temp.colliderInfo.boxCollider.bottLeftBackCorner = DirectX::XMFLOAT3(blbX, blbY, blbZ);

		colliderDataList.push_back(temp);
		return &colliderDataList.back();
	}
	return nullptr;
}