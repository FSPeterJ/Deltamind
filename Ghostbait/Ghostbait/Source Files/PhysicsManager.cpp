#include "PhysicsManager.h"
#include "DebugRenderer.h"
#include "GameObject.h"
#include "Console.h"
#include <DirectXMath.h>
#include "GhostTime.h"

Collider PhysicsManager::defaultColider;
ColliderData PhysicsManager::defaultSphereColider;
SpatialPartition PhysicsManager::partitionSpace;
Pool<PhysicsComponent> PhysicsManager::dynamicComponents = Pool<PhysicsComponent>(MAX_PHYSICALS, true);
Pool<PhysicsComponent> PhysicsManager::staticComponents = Pool<PhysicsComponent>(MAX_STATIC_PHYSICALS, true);

using namespace DirectX;

#pragma region Public Functions

PhysicsManager::PhysicsManager() {

	defaultSphereColider.colliderInfo.sphereCollider.radius = 0.5f;
	defaultSphereColider.colliderType = SPHERE;

	defaultColider.centerOffset = {0, 0, 0};
	defaultColider.isTrigger = true;
	defaultColider.colliderData = &defaultSphereColider;
	TypeMap::RegisterComponentAlias<PhysicsComponent>("Physical");
	TypeMap::RegisterComponentAlias<PhysicsComponent>("Phys");

	colliderDataList.reserve(MAX_COLLIDER_DATA);
	prefabComponents.reserve(MAX_PREFABS);

	dynamicComponents.RequestMemoryFromMemManager(MAX_PHYSICALS);
	staticComponents.RequestMemoryFromMemManager(MAX_STATIC_PHYSICALS);
}
PhysicsManager::~PhysicsManager() {}

void PhysicsManager::AddComponent(GameObject* obj, float veloX, float veloY, float veloZ) {
	PhysicsComponent* physComponent = dynamicComponents.ActivateMemory();
	physComponent->colliders.push_back(defaultColider);
	physComponent->parentObject = obj;
	physComponent->rigidBody = RigidBody();
	physComponent->rigidBody.SetVelocity(veloX, veloY, veloZ);
}

PhysicsComponent* PhysicsManager::CloneComponent(ComponentBase* reference) {
	PhysicsComponent* physComponent;

	if(((PhysicsComponent*)reference)->isStatic)
		physComponent = staticComponents.ActivateMemory();
	else
		 physComponent = dynamicComponents.ActivateMemory();
	
	// SHALLOW COPY - this only copies the std::vector head.
	physComponent->colliders = ((PhysicsComponent*)reference)->colliders;	
	physComponent->currentAABB = ((PhysicsComponent*)reference)->currentAABB;
	physComponent->previousAABB = ((PhysicsComponent*)reference)->previousAABB;
	physComponent->baseAABB = ((PhysicsComponent*)reference)->baseAABB;
	physComponent->isStatic = ((PhysicsComponent*)reference)->isStatic;
	physComponent->isActive = ((PhysicsComponent*)reference)->isActive;

	if (((PhysicsComponent*)reference)->isStatic) return physComponent;
		
	partitionSpace.AddComponent(physComponent);
	return physComponent;
}
PhysicsComponent* PhysicsManager::GetReferenceComponent(const char * _FilePath, const char * _dataBlock) {
	PhysicsComponent compHolder;
	XMFLOAT3 offsetHolder;
	XMFLOAT3 aabbMax(FLT_MIN_EXP, FLT_MIN_EXP, FLT_MIN_EXP);
	XMFLOAT3 aabbMin(FLT_MAX_EXP, FLT_MAX_EXP, FLT_MAX_EXP);
	XMFLOAT3 incomingMax = aabbMax, incomingMin = aabbMin;

	ColliderData* colDataHolder = nullptr;
	int numofColliders, lengthofTypeName, currIndex = 0;
	float radiusHolder, heightHolder;
	char* typeName;
	bool triggerHolder;

	char sphere[] = "SPHERE";
	char capsule[] = "CAPSULE";
	char box[] = "BOX";

	memcpy(&compHolder.isStatic, &_dataBlock[currIndex], sizeof(bool));
	currIndex += sizeof(bool);
	memcpy(&numofColliders, &_dataBlock[currIndex], sizeof(numofColliders));
	currIndex += sizeof(numofColliders);

	for(int i = 0; i < numofColliders; ++i) {
		memcpy(&lengthofTypeName, &_dataBlock[currIndex], sizeof(lengthofTypeName));
		currIndex += sizeof(lengthofTypeName);
		typeName = new char[lengthofTypeName];
		memcpy(&typeName[0], &_dataBlock[currIndex], lengthofTypeName);
		currIndex += lengthofTypeName;
		memcpy(&triggerHolder, &_dataBlock[currIndex], sizeof(bool));
		currIndex += sizeof(bool);
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
		if (incomingMax.x > aabbMax.x) { aabbMax.x = incomingMax.x; };
		if (incomingMax.y > aabbMax.y) { aabbMax.y = incomingMax.y; };
		if (incomingMax.z > aabbMax.z) { aabbMax.z = incomingMax.z; };
		if (incomingMin.x < aabbMin.x) { aabbMin.x = incomingMin.x; };
		if (incomingMin.y < aabbMin.y) { aabbMin.y = incomingMin.y; };
		if (incomingMin.z < aabbMin.z) { aabbMin.z = incomingMin.z; };

		compHolder.AddCollider(colDataHolder, offsetHolder.x, offsetHolder.y, offsetHolder.z, triggerHolder);
		colDataHolder = nullptr;
		delete[] typeName; 
	}

	compHolder.previousAABB = compHolder.currentAABB = compHolder.baseAABB = AABB(aabbMin, aabbMax);

	if(prefabComponents.size() < MAX_PREFABS) {
		prefabComponents.push_back(compHolder);
		return &prefabComponents.back();
	} else {
		throw "If it breaks here, you ran out of prefab space. change MAX_PREFABS ";
		return nullptr;
	}
}

void PhysicsManager::ResetComponent(ComponentBase * reset) {
	if (((PhysicsComponent*)reset)->isStatic) {
		staticComponents.DeactivateMemory(reset);
		return;
	}

	dynamicComponents.DeactivateMemory(reset);
	partitionSpace.RemoveComponent((PhysicsComponent*)reset);
}

void PhysicsManager::ActivateComponent(ComponentBase* component)
{

}

void PhysicsManager::DeactivateComponent(ComponentBase* component)
{

}

void PhysicsManager::PausedUpdate() {
	const int activeCount = (int)dynamicComponents.GetActiveCount();
	for (int i = 0; i < activeCount; ++i) {
		if (!dynamicComponents[i].isActive) continue;
		UpdateAABB(dynamicComponents[i]);
		partitionSpace.UpdateComponent(&dynamicComponents[i]);
	}
}

void PhysicsManager::Update() {

	const int activeCount = (int)dynamicComponents.GetActiveCount();
	for(int i = 0; i < activeCount; ++i) {
		float delta = (float)GhostTime::DeltaTime();

		//This seems absurd, are we sure we can't use XMVECTOR and XMMATRIX in a more manageable manner?
		if (!dynamicComponents[i].isActive) continue;
		XMFLOAT4* objectPosition = (XMFLOAT4*) &dynamicComponents[i].parentObject->GetPosition().m[3];
		XMVECTOR newposition = XMLoadFloat4(objectPosition);
		dynamicComponents[i].rigidBody.Update();
		newposition += dynamicComponents[i].rigidBody.GetVelocity() * delta;
		XMStoreFloat4(objectPosition, newposition);
		UpdateAABB(dynamicComponents[i]);
		partitionSpace.UpdateComponent(&dynamicComponents[i]);
		//components[i].parentObject->position.r[3] += components[i].rigidBody.GetVelocity() * dt;

#if _DEBUG
		for(unsigned int colInd = 0; colInd < dynamicComponents[i].colliders.size(); ++colInd) {
			XMVECTOR offset = XMLoadFloat3(&(dynamicComponents[i].colliders[colInd].centerOffset));
			XMFLOAT3 colPos;
			XMStoreFloat3(&colPos, newposition + offset);

			switch(dynamicComponents[i].colliders[colInd].colliderData->colliderType) {
			case SPHERE:
				DebugRenderer::AddSphere(colPos, dynamicComponents[i].colliders[colInd].colliderData->colliderInfo.sphereCollider.radius, XMFLOAT3(1.0f, 0.0f, 0.0f));
				break;
			case CAPSULE:
			{
				float _height = dynamicComponents[i].colliders[colInd].colliderData->colliderInfo.capsuleCollider.height;
				XMVECTOR cap1A = offset + XMVectorSet(0, _height * 0.5f, 0, 0);
				XMVECTOR cap1B = offset - XMVectorSet(0, _height * 0.5f, 0, 0);
				cap1A = XMVector3TransformCoord(cap1A, XMLoadFloat4x4(&(dynamicComponents[i].parentObject->GetPosition())));
				cap1B = XMVector3TransformCoord(cap1B, XMLoadFloat4x4(&(dynamicComponents[i].parentObject->GetPosition())));
				XMFLOAT3 capStart, capEnd;
				XMStoreFloat3(&capStart, cap1A);
				XMStoreFloat3(&capEnd, cap1B);
				DebugRenderer::AddSphere(capStart, dynamicComponents[i].colliders[colInd].colliderData->colliderInfo.capsuleCollider.radius, XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddSphere(capEnd, dynamicComponents[i].colliders[colInd].colliderData->colliderInfo.capsuleCollider.radius, XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(capStart, capEnd, XMFLOAT3(1.0f, 0.0f, 0.0f));
			} break;
			case BOX:
			{
				std::vector<XMVECTOR> corners = GetBoxCorners(dynamicComponents[i].colliders[colInd], XMLoadFloat4x4(&(dynamicComponents[i].parentObject->GetPosition())));
				std::vector<XMFLOAT3> cornersStored;
				XMFLOAT3 temp;
				for (unsigned int i = 0; i < corners.size(); ++i) {
					XMStoreFloat3(&temp, corners[i]);
					cornersStored.push_back(temp);
				}
				
				DebugRenderer::AddLine(cornersStored[0], cornersStored[1], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[1], cornersStored[3], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[3], cornersStored[2], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[2], cornersStored[0], XMFLOAT3(1.0f, 0.0f, 0.0f));
									  
				DebugRenderer::AddLine(cornersStored[4], cornersStored[5], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[5], cornersStored[7], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[7], cornersStored[6], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[6], cornersStored[4], XMFLOAT3(1.0f, 0.0f, 0.0f));
									   
				DebugRenderer::AddLine(cornersStored[4], cornersStored[0], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[5], cornersStored[1], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[6], cornersStored[2], XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(cornersStored[7], cornersStored[3], XMFLOAT3(1.0f, 0.0f, 0.0f));

			} break;
			default:
				break;
			}
		}

		DebugRenderer::AddBox(dynamicComponents[i].currentAABB.min, dynamicComponents[i].currentAABB.max, XMFLOAT3(0.0f, 1.0f, 0.0f));

#endif
	}
	//components[0].srcObj->position.r[3] -= XMVectorSet(0, dt, 0, 0);

	TestAllComponentsCollision();
}

bool PhysicsManager::Raycast(XMFLOAT3& origin, XMFLOAT3& direction, XMFLOAT3* colPoint, GameObject** colObject, float maxCastDistance, const char* tag) {
	bool collided = false;
	uint32_t nextIndex, currBucketIndex = -1;
	XMVECTOR vecOrigin = XMLoadFloat3(&origin);
	XMVECTOR vecDirection = XMVector3Normalize(XMLoadFloat3(&direction));
	XMVECTOR vecNextSeg = vecOrigin;
	XMVECTOR vecSegInterval = vecDirection * 0.01f;
	XMVECTOR closestCollision = vecOrigin + (vecDirection * maxCastDistance);
	XMVECTOR tempCollidePt;
	GameObject* tempCollideObj = nullptr;
	XMFLOAT3 nextSegment;
	std::vector<PhysicsComponent*> compToTest;
	std::vector<XMVECTOR> collisionPoints;
	std::vector<GameObject*> collidedObjects;
	if(colObject)
		*colObject = nullptr;

	for (int iteration = 0; iteration < (int)(maxCastDistance * 100); ++iteration) {
		XMStoreFloat3(&nextSegment, vecNextSeg);
		nextIndex = partitionSpace.GetHashedIndex(nextSegment);
		vecNextSeg += vecSegInterval;
		if (currBucketIndex == nextIndex)
			continue;
		currBucketIndex = nextIndex;
		compToTest = partitionSpace.GetComponentsToTest(currBucketIndex);
		
		for (int compIndex = 0; compIndex < compToTest.size(); ++compIndex) {
			if (!compToTest[compIndex]->isActive) continue;
			if (tag && strcmp(dynamic_cast<GameObject*>(compToTest[compIndex]->parentObject)->GetTag().c_str(), tag)) continue;
			if (RaycastCollisionCheck(vecOrigin, vecDirection, compToTest[compIndex], &tempCollidePt, &tempCollideObj, maxCastDistance)) {
				collisionPoints.push_back(tempCollidePt);
				collidedObjects.push_back(tempCollideObj);
				collided = true;
			}
		}

		if (collided)
			break;
	}

	std::vector<PhysicsComponent*>* staticComps = staticComponents.GetActiveList();
	for (int i = 0; i < staticComps->size(); ++i) {
		if (!(*staticComps)[i]->isActive) continue;
		if (tag && strcmp(dynamic_cast<GameObject*>((*staticComps)[i]->parentObject)->GetTag().c_str(), tag)) continue;
		if (RaycastCollisionCheck(vecOrigin, vecDirection, (*staticComps)[i], &tempCollidePt, &tempCollideObj, maxCastDistance)) {
			collisionPoints.push_back(tempCollidePt);
			collidedObjects.push_back(tempCollideObj);
			collided = true;
		}
	}

	if (collided) {
		float lastClosestDist = XMVectorGetX(XMVector3LengthSq(closestCollision - vecOrigin));
		float nextDist;
		for (int i = 0; i < (int)collisionPoints.size(); ++i) {
			nextDist = XMVectorGetX(XMVector3LengthSq(collisionPoints[i] - vecOrigin));
			if (lastClosestDist > nextDist) {
				closestCollision = collisionPoints[i];
				if (colObject)
					*colObject = collidedObjects[i];
				lastClosestDist = nextDist;
				//Console::WriteLine << "RAY HIT";
			}
		}

		if (colPoint)
			XMStoreFloat3(colPoint, closestCollision);
	}

#if _DEBUG
	XMFLOAT3 line;
	XMStoreFloat3(&line, closestCollision);
	DebugRenderer::AddLine(origin, line, XMFLOAT3(0.0f, 1.0f, 1.0f));

#endif
	return collided;
}

#pragma endregion

#pragma region Private Functions

ColliderData* PhysicsManager::AddColliderData(float _radius) {
	if(colliderDataList.size() < MAX_COLLIDER_DATA) {
		ColliderData temp;
		temp.colliderType = SPHERE;
		temp.colliderInfo.sphereCollider.radius = _radius;
		colliderDataList.push_back(temp);
		return &colliderDataList.back();
	}
	return nullptr;
}
ColliderData* PhysicsManager::AddColliderData(float _radius, float _height) {
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
ColliderData* PhysicsManager::AddColliderData(float trfX, float trfY, float trfZ, float blbX, float blbY, float blbZ) {
	if(colliderDataList.size() < MAX_COLLIDER_DATA) {
		ColliderData temp;
		temp.colliderType = BOX;
		temp.colliderInfo.boxCollider.topRightFrontCorner = XMFLOAT3(trfX, trfY, trfZ);
		temp.colliderInfo.boxCollider.bottLeftBackCorner = XMFLOAT3(blbX, blbY, blbZ);

		colliderDataList.push_back(temp);
		return &colliderDataList.back();
	}
	return nullptr;
}
void PhysicsManager::UpdateAABB(PhysicsComponent& component) {
	//DOES NOT ACCOUNT FOR ROTATION!
	std::vector<XMVECTOR> corners = GetBoxCorners(component.baseAABB, XMLoadFloat4x4(&component.parentObject->GetPosition()));
	XMFLOAT3* newMax = &component.currentAABB.max;
	XMFLOAT3* newMin = &component.currentAABB.min;
	*newMax = { -FLT_MAX, -FLT_MAX , -FLT_MAX };
	*newMin = { FLT_MAX, FLT_MAX , FLT_MAX };

	XMFLOAT3 temp;
	for (unsigned int i = 0; i < corners.size(); ++i) {
		XMStoreFloat3(&temp, corners[i]);
		if (temp.x > newMax->x) newMax->x = temp.x;
		if (temp.y > newMax->y) newMax->y = temp.y;
		if (temp.z > newMax->z) newMax->z = temp.z;
		if (temp.x < newMin->x) newMin->x = temp.x;
		if (temp.y < newMin->y) newMin->y = temp.y;
		if (temp.z < newMin->z) newMin->z = temp.z;
	}
}
void PhysicsManager::CollisionCheck(PhysicsComponent component1, PhysicsComponent component2) {
	bool collisionResult = false, isCol1Trigger = false, isCol2Trigger = false;
	ColliderType colliderType1, colliderType2;

	XMMATRIX matrixComA = XMLoadFloat4x4(&component1.parentObject->GetPosition());
	XMMATRIX matrixComB = XMLoadFloat4x4(&component2.parentObject->GetPosition());

	for(unsigned int com1 = 0; com1 < component1.colliders.size(); ++com1) {
		colliderType1 = component1.colliders[com1].colliderData->colliderType;
		isCol1Trigger = component1.colliders[com1].isTrigger;
		for (unsigned int com2 = 0; com2 < component2.colliders.size(); ++com2) {
			isCol2Trigger = component2.colliders[com2].isTrigger;
			if (isCol1Trigger && isCol2Trigger) continue;
			collisionResult = false;
			colliderType2 = component2.colliders[com2].colliderData->colliderType;

			switch (colliderType1) {
			case SPHERE:
			{
				switch (colliderType2) {
				case SPHERE:
					collisionResult = SphereToSphereCollision(component1.colliders[com1], matrixComA.r[3], component2.colliders[com2], matrixComB.r[3]);
					break;
				case CAPSULE:
					collisionResult = CapsuleToSphereCollision(component2.colliders[com2], matrixComB, component1.colliders[com1], matrixComA);
					break;
				case BOX:
					collisionResult = BoxToSphereCollision(component2.colliders[com2], matrixComB, component1.colliders[com1], matrixComA);
					break;
				default:
					break;
				}
			} break;

			case CAPSULE:
			{
				switch (colliderType2) {
				case SPHERE:
					collisionResult = CapsuleToSphereCollision(component1.colliders[com1], matrixComA, component2.colliders[com2], matrixComB);
					break;
				case CAPSULE:
					collisionResult = CapsuleToCapsuleCollision(component1.colliders[com1], matrixComA, component2.colliders[com2], matrixComB);
					break;
				case BOX:
					collisionResult = BoxToCapsuleCollision(component2.colliders[com2], matrixComB, component1.colliders[com1], matrixComA);
					break;
				default:
					break;
				}
			} break;

			case BOX:
			{
				switch (colliderType2) {
				case SPHERE:
					collisionResult = BoxToSphereCollision(component1.colliders[com1], matrixComA, component2.colliders[com2], matrixComB);
					break;
				case CAPSULE:
					collisionResult = BoxToCapsuleCollision(component1.colliders[com1], matrixComA, component2.colliders[com2], matrixComB);
					break;
				case BOX:
					collisionResult = BoxToBoxCollision(component1.colliders[com1], matrixComA, component2.colliders[com2], matrixComB);
					break;
				default:
					break;
				}
			} break;
			}

			if (collisionResult) {
				if (isCol1Trigger)
					SendTrigger((GameObject*)component1.parentObject, (GameObject*)component2.parentObject);
				else if (isCol2Trigger)
					SendTrigger((GameObject*)component2.parentObject, (GameObject*)component1.parentObject);
				else
					SendCollision((GameObject*)component1.parentObject, (GameObject*)component2.parentObject);
			}
		}
	}
}
bool PhysicsManager::IsVectorZero(XMVECTOR& _toTest) {
	if (fabsf(XMVectorGetX(_toTest)) < FLT_EPSILON && fabsf(XMVectorGetY(_toTest)) < FLT_EPSILON && fabsf(XMVectorGetZ(_toTest)) < FLT_EPSILON)
		return true;
	return false;
}
bool PhysicsManager::SphereToSphereCollision(Collider& col1, XMVECTOR& pos1, Collider& col2, XMVECTOR& pos2) {
	XMVECTOR offset1 = XMLoadFloat3(&col1.centerOffset);
	XMVECTOR offset2 = XMLoadFloat3(&col2.centerOffset);
	XMVECTOR position1 = offset1 + pos1;
	XMVECTOR position2 = offset2 + pos2;
	XMFLOAT3 between;
	XMStoreFloat3(&between, position1 - position2);
	float sqrDist = between.x*between.x + between.y*between.y + between.z*between.z;
	float combinedRad = col1.colliderData->colliderInfo.sphereCollider.radius + col2.colliderData->colliderInfo.sphereCollider.radius;
	if(sqrDist < (combinedRad*combinedRad)) return true;
	return false;
}
bool PhysicsManager::CapsuleToCapsuleCollision(Collider& col1, XMMATRIX& pos1, Collider& col2, XMMATRIX& pos2) {
	
	float combineRadiusSq = col1.colliderData->colliderInfo.capsuleCollider.radius + col2.colliderData->colliderInfo.capsuleCollider.radius;
	combineRadiusSq *= combineRadiusSq;

	//Create Capsule 1
	XMVECTOR cap1Offset = XMLoadFloat3(&col1.centerOffset);
	XMVECTOR cap1Start = cap1Offset + XMVectorSet(0, col1.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	XMVECTOR cap1End = cap1Offset - XMVectorSet(0, col1.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	cap1Start = XMVector3TransformCoord(cap1Start, pos1);
	cap1End = XMVector3TransformCoord(cap1End, pos1);
	XMVECTOR seg1 = cap1End - cap1Start;

	//Create Capsule 2
	XMVECTOR cap2Offset = XMLoadFloat3(&col2.centerOffset);
	XMVECTOR cap2Start = cap2Offset + XMVectorSet(0, col2.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	XMVECTOR cap2End = cap2Offset - XMVectorSet(0, col2.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	cap2Start = XMVector3TransformCoord(cap2Start, pos2);
	cap2End = XMVector3TransformCoord(cap2End, pos2);
	XMVECTOR seg2 = cap2End - cap2Start;

//	//XMVECTOR segBetweenStarts = cap1Start - cap2Start;
//
//	////Find shortest line between line segments
//	////Reference: http://paulbourke.net/geometry/pointlineplane/lineline.c
//	//if (IsVectorZero(seg1) || IsVectorZero(seg2))
//	//	return false;
//
//	//float segTweenDotSeg1, segTweenDotSeg2, seg1DotSeg2, seg2SqLength, seg1SqLength, numer, denom, ratioOnSeg1, ratioOnSeg2;
//
//	//segTweenDotSeg1 = XMVectorGetX(XMVector3Dot(segBetweenStarts, seg1));
//	//segTweenDotSeg2 = XMVectorGetX(XMVector3Dot(segBetweenStarts, seg2));
//	//seg1DotSeg2 = XMVectorGetX(XMVector3Dot(seg1, seg2));
//	//seg1SqLength = XMVectorGetX(XMVector3LengthSq(seg1));
//	//seg2SqLength = XMVectorGetX(XMVector3LengthSq(seg2));
//
//	//denom = (seg1SqLength * seg2SqLength) - (seg1DotSeg2 * seg1DotSeg2);
//
//	////If Denom is zero, lines are parallel
//	//if (fabsf(denom) < FLT_EPSILON) {
//		float closestDistSq, testNextClosestDistSq;
//		XMVECTOR closestTo1Start, closestTo1End, closestTo2Start, closestTo2End;
//
//		closestTo1Start = FindClosestPointOnLine(cap2Start, cap2End, cap1Start);
//		closestTo1End = FindClosestPointOnLine(cap2Start, cap2End, cap1End);
//		closestTo2Start = FindClosestPointOnLine(cap1Start, cap1End, cap2Start);
//		closestTo2End = FindClosestPointOnLine(cap1Start, cap1End, cap2End);
//
//#if _DEBUG
//		XMFLOAT3 start, end;
//		XMStoreFloat3(&start, cap1Start);
//		XMStoreFloat3(&end, closestTo1Start);
//		DebugRenderer::AddLine(start, end, XMFLOAT3(0.0f, 0.0f, 1.0f));
//		XMStoreFloat3(&start, cap1End);
//		XMStoreFloat3(&end, closestTo1End);
//		DebugRenderer::AddLine(start, end, XMFLOAT3(0.0f, 0.0f, 1.0f));
//		XMStoreFloat3(&start, cap2Start);
//		XMStoreFloat3(&end, closestTo2Start);
//		DebugRenderer::AddLine(start, end, XMFLOAT3(0.0f, 0.0f, 1.0f));
//		XMStoreFloat3(&start, cap2End);
//		XMStoreFloat3(&end, closestTo2End);
//		DebugRenderer::AddLine(start, end, XMFLOAT3(0.0f, 0.0f, 1.0f));
//#endif
//		closestDistSq = XMVectorGetX(XMVector3LengthSq(closestTo1Start - cap1Start));
//
//		testNextClosestDistSq = XMVectorGetX(XMVector3LengthSq(closestTo1End - cap1End));
//		if (testNextClosestDistSq < closestDistSq) {
//			closestDistSq = testNextClosestDistSq;
//		}
//		testNextClosestDistSq = XMVectorGetX(XMVector3LengthSq(closestTo2Start - cap2Start));
//		if (testNextClosestDistSq < closestDistSq) {
//			closestDistSq = testNextClosestDistSq;
//		}
//		testNextClosestDistSq = XMVectorGetX(XMVector3LengthSq(closestTo2End - cap2End));
//		if (testNextClosestDistSq < closestDistSq) {
//			closestDistSq = testNextClosestDistSq;
//		}
//		
//		XMVECTOR cap2A = cap2Offset + XMVectorSet(0, col2.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
//		XMVECTOR cap2B = cap2Offset - XMVectorSet(0, col2.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
//		cap2A = XMVector3TransformCoord(cap2A, pos2);
//		cap2B = XMVector3TransformCoord(cap2B, pos2);
//		//XMVECTOR seg2 = cap2A - cap2B;
//
//		if (closestDistSq < combineRadiusSq)
//			return true;
//
//		return false;
//	//}
//	//numer = (segTweenDotSeg2 * seg1DotSeg2) - (segTweenDotSeg1 * seg2SqLength);
//
//	//ratioOnSeg1 = numer / denom;
//	//ratioOnSeg2 = (segTweenDotSeg2 + (seg1DotSeg2 * ratioOnSeg1)) / seg2SqLength;
//
//	//XMVECTOR pointB = (cap2Start + (seg2 * ratioOnSeg2));
//	//XMVECTOR pointA = (cap1Start + (seg1 * ratioOnSeg1));
//	//XMVECTOR shortestLine = pointB - pointA;
//
//	//if (XMVectorGetX(XMVector3LengthSq(shortestLine)) > combineRadiusSq)
//	//	return false;
//
//	//return true;

//*************** VERSION 2 ************************ (FULLY FUNCTIONAL)
//http://geomalgorithms.com/a07-_distance.html

	XMVECTOR u = seg1;
	XMVECTOR v = seg2;
	XMVECTOR w = cap1Start - cap2Start;
	float a = XMVectorGetX(XMVector3Dot(u, u));  // always >= 0
	float b = XMVectorGetX(XMVector3Dot(u, v));
	float c = XMVectorGetX(XMVector3Dot(v, v));  // always >= 0
	float d = XMVectorGetX(XMVector3Dot(u, w));
	float e = XMVectorGetX(XMVector3Dot(v, w)); 
	float D = a*c - b*b;        // always >= 0
	float sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

								   // compute the line parameters of the two closest points
	if (D < FLT_EPSILON) { // the lines are almost parallel
		sN = 0.0f;         // force using point P0 on segment S1
		sD = 1.0f;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0f) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0f;
		// recompute sc for this edge
		if (-d < 0.0f)
			sN = 0.0f;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0f)
			sN = 0.0f;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (abs(sN) < FLT_EPSILON ? 0.0f : sN / sD);
	tc = (abs(tN) < FLT_EPSILON ? 0.0f : tN / tD);

	// get the difference of the two closest points
	XMVECTOR dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)
	float dist = XMVectorGetX(XMVector3LengthSq(dP));

#if _DEBUG
		XMFLOAT3 start, end;
		XMStoreFloat3(&start, sc * u + cap1Start);
		XMStoreFloat3(&end, tc * v + cap2Start);
		DebugRenderer::AddLine(start, end, XMFLOAT3(0.0f, 0.0f, 1.0f));
#endif

	return dist < combineRadiusSq;   // return the closest distance
}

bool PhysicsManager::CapsuleToSphereCollision(Collider& capCol, XMMATRIX& capPos, Collider& sphCol, XMMATRIX& sphPos) {
	XMVECTOR sphereCenter, closestOnCap, capStart, capEnd;

	XMVECTOR capOffset = XMLoadFloat3(&capCol.centerOffset);
	capStart = capOffset + XMVectorSet(0, capCol.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	capEnd = capOffset - XMVectorSet(0, capCol.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	capStart = XMVector3TransformCoord(capStart, capPos);
	capEnd = XMVector3TransformCoord(capEnd, capPos);

	sphereCenter = XMVector3TransformCoord(XMLoadFloat3(&sphCol.centerOffset), sphPos);
	closestOnCap = FindClosestPointOnLine(capStart, capEnd, sphereCenter);

	float radii = capCol.colliderData->colliderInfo.capsuleCollider.radius + sphCol.colliderData->colliderInfo.sphereCollider.radius;
	XMVECTOR sphCenterToClosest = closestOnCap - sphereCenter;
	return !(XMVectorGetX(XMVector3Dot(sphCenterToClosest, sphCenterToClosest)) > (radii * radii));
}

bool PhysicsManager::BoxToBoxCollision(Collider& boxCol1, XMMATRIX& boxPos1, Collider& boxCol2, XMMATRIX& boxPos2) {
	//std::vector<XMVECTOR> box1Corners, box2Corners, testAxis;
	//box1Corners = GetBoxCorners(boxCol1, boxPos1);
	//box2Corners = GetBoxCorners(boxCol2, boxPos2);
	//testAxis = GetSATAxis(box1Corners, box2Corners);

//#if _DEBUG
//	XMFLOAT3 start(0.0f, 0.0f, 0.0f);
//	XMFLOAT3 end;
//	for (unsigned int i = 0; i < testAxis.size(); ++i) {
//		XMStoreFloat3(&end, testAxis[i]);
//		DebugRenderer::AddLine(start, end, XMFLOAT3(0.0f, 0.0f, 1.0f));
//	}
//#endif

//	float box1AxisMax, box1AxisMin, box2AxisMax, box2AxisMin;
//	float tempPoint1, tempPoint2;
//
//	for (unsigned int axisIndex = 0; axisIndex < testAxis.size(); ++axisIndex) {
//		box1AxisMax = -FLT_MAX;
//		box1AxisMin = FLT_MAX;
//		box2AxisMax = -FLT_MAX;
//		box2AxisMin = FLT_MAX;
//		for (unsigned testIndex = 0; testIndex < box1Corners.size(); ++testIndex) {
//			tempPoint1 = XMVectorGetX(XMVector3Dot(testAxis[axisIndex], box1Corners[testIndex]));
//			tempPoint2 = XMVectorGetX(XMVector3Dot(testAxis[axisIndex], box2Corners[testIndex]));
//
//			if (box1AxisMax < tempPoint1) box1AxisMax = tempPoint1;
//			if (box1AxisMin > tempPoint1) box1AxisMin = tempPoint1;
//			if (box2AxisMax < tempPoint2) box2AxisMax = tempPoint2;
//			if (box2AxisMin > tempPoint2) box2AxisMin = tempPoint2;
//
//#if _DEBUG
//			if (axisIndex == 4) {
//				XMFLOAT3 start1, start2, end1, end2;
//				XMStoreFloat3(&start1, box1Corners[testIndex]);
//				XMStoreFloat3(&start2, box2Corners[testIndex]);
//				XMStoreFloat3(&end1, testAxis[axisIndex] * tempPoint1);
//				XMStoreFloat3(&end2, testAxis[axisIndex] * tempPoint2);
//
//				DebugRenderer::AddLine(start1, end1, XMFLOAT3(1.0f, 0.0f, 1.0f));
//				DebugRenderer::AddLine(start2, end2, XMFLOAT3(0.0f, 0.0f, 1.0f));
//			}
//#endif 
//
//		}
//		if (box1AxisMax < box2AxisMin) return false;
//		if (box1AxisMin > box2AxisMax) return false;
//	}
//	return true;


	////*************************** VERSION 2 *************************************
	//XMVECTOR ext1, ext2, box1ToBox2; 
	//std::vector<XMVECTOR> testAxis = GetSATAxis(boxPos1, boxPos2);
	//XMMATRIX box1Rotation = { boxPos1.r[0], boxPos1.r[1], boxPos1.r[2], XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
	//XMMATRIX box2Rotation = { boxPos2.r[0], boxPos2.r[1], boxPos2.r[2], XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };

	//box1ToBox2 = (boxPos2.r[3] + XMLoadFloat3(&boxCol2.centerOffset)) - (boxPos1.r[3] + XMLoadFloat3(&boxCol1.centerOffset));
	//ext1 = XMVector3TransformCoord((XMLoadFloat3(&boxCol1.colliderData->colliderInfo.boxCollider.topRightFrontCorner) - XMLoadFloat3(&boxCol1.colliderData->colliderInfo.boxCollider.bottLeftBackCorner)) * 0.5f, box1Rotation);
	//ext2 = XMVector3TransformCoord((XMLoadFloat3(&boxCol2.colliderData->colliderInfo.boxCollider.topRightFrontCorner) - XMLoadFloat3(&boxCol2.colliderData->colliderInfo.boxCollider.bottLeftBackCorner)) * 0.5f, box2Rotation);

	//float ext1Proj, ext2Proj, centerProj;
	//
	//for (unsigned int axisIndex = 0; axisIndex < testAxis.size(); ++axisIndex) {
	//	ext1Proj = XMVectorGetX(XMVector3Dot(testAxis[axisIndex], ext1));
	//	ext2Proj = XMVectorGetX(XMVector3Dot(testAxis[axisIndex], ext2));
	//	centerProj = XMVectorGetX(XMVector3Dot(testAxis[axisIndex], box1ToBox2));

	//	if (fabsf(centerProj) > fabsf(ext1Proj) + fabsf(ext2Proj)) return false;
	//}
	//return true;

//**************************** VERSION 3 *****************************************
	XMFLOAT3 *max, *min;

	max = &boxCol1.colliderData->colliderInfo.boxCollider.topRightFrontCorner;
	min = &boxCol1.colliderData->colliderInfo.boxCollider.bottLeftBackCorner;
	float ext1[] = { (max->x - min->x) * 0.5f, (max->y - min->y) * 0.5f, (max->z - min->z) * 0.5f };
	max = &boxCol2.colliderData->colliderInfo.boxCollider.topRightFrontCorner;
	min = &boxCol2.colliderData->colliderInfo.boxCollider.bottLeftBackCorner;
	float ext2[] = { (max->x - min->x) * 0.5f, (max->y - min->y) * 0.5f, (max->z - min->z) * 0.5f };

	float ra, rb;
	XMFLOAT3X3 R, AbsR;
	//Compute rotation matrix expressing b in a’s coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R.m[i][j] = XMVectorGetX(XMVector3Dot(boxPos1.r[i], boxPos2.r[j]));

	//Compute translation vector t
	XMVECTOR t = (boxPos2.r[3] + XMLoadFloat3(&boxCol2.centerOffset)) - (boxPos1.r[3] + XMLoadFloat3(&boxCol1.centerOffset));
	//Bring translation into a’s coordinate frame
	t = XMVectorSet(XMVectorGetX(XMVector3Dot(t, boxPos1.r[0])), XMVectorGetX(XMVector3Dot(t, boxPos1.r[1])), XMVectorGetX(XMVector3Dot(t, boxPos1.r[2])), 1.0f);
	
	float between[] = { XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t) };
	//Compute common subexpressions. Add in an epsilon term to
	//counteract arithmetic errors when two edgesare parallel and
	//their cross product is(near) null
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR.m[i][j] = fabsf(R.m[i][j]) + FLT_EPSILON;
	//TestaxesL=A0, L=A1, L=A2
	for (int i = 0; i < 3; i++) {
		ra = ext1[i];
		rb = ext2[0] * AbsR.m[i][0] + ext2[1] * AbsR.m[i][1] + ext2[2] * AbsR.m[i][2];
		if (fabsf(between[i]) > ra + rb) return false;
	}
	//TestaxesL=B0, L=B1, L=B2
	for (int i = 0; i < 3; i++) {
		ra = ext1[0] * AbsR.m[0][i] + ext1[1] * AbsR.m[1][i] + ext1[2] * AbsR.m[2][i];
		rb = ext2[i];
		if (fabsf(between[0] * R.m[0][i] + between[1] * R.m[1][i] + between[2] * R.m[2][i]) > ra + rb) return false;
	}
	//TestaxisL=A0xB0
	ra = ext1[1] * AbsR.m[2][0] + ext1[2] * AbsR.m[1][0];
	rb = ext2[1] * AbsR.m[0][2] + ext2[2] * AbsR.m[0][1];
	if (fabsf(between[2] * R.m[1][0] - between[1] * R.m[2][0]) > ra + rb) return false;
	//TestaxisL=A0xB1
	ra = ext1[1] * AbsR.m[2][1] + ext1[2] * AbsR.m[1][1];
	rb = ext2[0] * AbsR.m[0][2] + ext2[2] * AbsR.m[0][0];
	if (fabsf(between[2] * R.m[1][1] - between[1] * R.m[2][1]) > ra + rb) return false;
	//TestaxisL=A0xB2
	ra = ext1[1] * AbsR.m[2][2] + ext1[2] * AbsR.m[1][2];
	rb = ext2[0] * AbsR.m[0][1] + ext2[1] * AbsR.m[0][0];
	if (fabsf(between[2] * R.m[1][2] - between[1] * R.m[2][2]) > ra + rb) return false;
	//TestaxisL=A1xB0
	ra = ext1[0] * AbsR.m[2][0] + ext1[2] * AbsR.m[0][0];
	rb = ext2[1] * AbsR.m[1][2] + ext2[2] * AbsR.m[1][1];
	if (fabsf(between[0] * R.m[2][0] - between[2] * R.m[0][0]) > ra + rb) return false;
	//TestaxisL=A1xB1
	ra = ext1[0] * AbsR.m[2][1] + ext1[2] * AbsR.m[0][1];
	rb = ext2[0] * AbsR.m[1][2] + ext2[2] * AbsR.m[1][0];
	if (fabsf(between[0] * R.m[2][1] - between[2] * R.m[0][1]) > ra + rb) return false;
	//TestaxisL=A1xB2
	ra = ext1[0] * AbsR.m[2][2] + ext1[2] * AbsR.m[0][2];
	rb = ext2[0] * AbsR.m[1][1] + ext2[1] * AbsR.m[1][0];
	if (fabsf(between[0] * R.m[2][2] - between[2] * R.m[0][2]) > ra + rb) return false;
	//TestaxisL=A2xB0
	ra = ext1[0] * AbsR.m[1][0] + ext1[1] * AbsR.m[0][0];
	rb = ext2[1] * AbsR.m[2][2] + ext2[2] * AbsR.m[2][1];
	if (fabsf(between[1] * R.m[0][0] - between[0] * R.m[1][0]) > ra + rb) return false;
	//TestaxisL=A2xB1
	ra = ext1[0] * AbsR.m[1][1] + ext1[1] * AbsR.m[0][1];
	rb = ext2[0] * AbsR.m[2][2] + ext2[2] * AbsR.m[2][0];
	if (fabsf(between[1] * R.m[0][1] - between[0] * R.m[1][1]) > ra + rb) return false;
	//TestaxisL=A2xB2
	ra = ext1[0] * AbsR.m[1][2] + ext1[1] * AbsR.m[0][2];
	rb = ext2[0] * AbsR.m[2][1] + ext2[1] * AbsR.m[2][0];
	if (fabsf(between[1] * R.m[0][2] - between[0] * R.m[1][2]) > ra + rb) return false;
	//Sincenoseparatingaxis isfound, theOBBsmustbe intersecting

	return true;
}

bool PhysicsManager::BoxToSphereCollision(Collider& boxCol, XMMATRIX& boxPos, Collider& sphCol, XMMATRIX& sphPos) {
	XMFLOAT3 *max, *min;

	max = &boxCol.colliderData->colliderInfo.boxCollider.topRightFrontCorner;
	min = &boxCol.colliderData->colliderInfo.boxCollider.bottLeftBackCorner;
	float ext1[] = { (max->x - min->x) * 0.5f, (max->y - min->y) * 0.5f, (max->z - min->z) * 0.5f };

	XMVECTOR t = (sphPos.r[3] + XMLoadFloat3(&sphCol.centerOffset)) - (boxPos.r[3] + XMLoadFloat3(&boxCol.centerOffset));
	//Bringtranslation intoa’scoordinateframe
	t = XMVectorSet(XMVectorGetX(XMVector3Dot(t, boxPos.r[0])), XMVectorGetX(XMVector3Dot(t, boxPos.r[1])), XMVectorGetX(XMVector3Dot(t, boxPos.r[2])), 1.0f);

	float between[] = { XMVectorGetX(t), XMVectorGetY(t), XMVectorGetZ(t) };
	float ra, rb;
	rb = sphCol.colliderData->colliderInfo.sphereCollider.radius;

#ifdef _DEBUG
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, sphPos.r[3] + XMLoadFloat3(&sphCol.centerOffset));
	DebugRenderer::AddSphere(temp, rb, XMFLOAT3(1.0f, 1.0f, 0.0f));
#endif // DEBUG

	for (int i = 0; i < 3; i++) {
		ra = ext1[i];
		if (fabsf(between[i]) > ra + rb) return false;
	}

	return true;
}

bool PhysicsManager::BoxToCapsuleCollision(Collider& boxCol, XMMATRIX& boxPos, Collider& capCol, XMMATRIX& capPos) {
	XMVECTOR capStart = XMVectorSet(0, capCol.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	XMVECTOR capEnd =  - XMVectorSet(0, capCol.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	capStart = XMVector3TransformCoord(capStart, capPos);
	capEnd = XMVector3TransformCoord(capEnd, capPos);
	XMVECTOR seg1 = capEnd - capStart;

	XMMATRIX currSphereMat = capPos;
	float height = capCol.colliderData->colliderInfo.capsuleCollider.height;
	float halfRadius = capCol.colliderData->colliderInfo.capsuleCollider.radius * 0.5f;
	XMVECTOR interval = XMVector3Normalize(seg1) * halfRadius;

	currSphereMat.r[3] = capEnd;
	if (BoxToSphereCollision(boxCol, boxPos, capCol, currSphereMat)) return true;
	
	currSphereMat.r[3] = capStart;
	for (float current = 0; current < height; current += halfRadius) {
		if (BoxToSphereCollision(boxCol, boxPos, capCol, currSphereMat)) return true;
		currSphereMat.r[3] += interval;
	}

	return false;
}

std::vector<XMVECTOR> PhysicsManager::GetSATAxis(std::vector<XMVECTOR>& boxCorners) {
	std::vector<XMVECTOR> axisToTest;

	axisToTest.push_back(XMVector3Normalize(boxCorners[2] - boxCorners[0])); //"X" axis
	axisToTest.push_back(XMVector3Normalize(boxCorners[4] - boxCorners[0])); //"Y" axis
	axisToTest.push_back(XMVector3Normalize(boxCorners[1] - boxCorners[0])); //"Z" axis

	return axisToTest;
}

std::vector<XMVECTOR> PhysicsManager::GetSATAxis(std::vector<XMVECTOR>& box1Corners, std::vector<XMVECTOR>& box2Corners) {
	std::vector<XMVECTOR> axisToTest;
	//Box 1
	axisToTest.push_back(XMVector3Normalize(box1Corners[2] - box1Corners[0])); //"X" axis
	axisToTest.push_back(XMVector3Normalize(box1Corners[4] - box1Corners[0])); //"Y" axis
	axisToTest.push_back(XMVector3Normalize(box1Corners[1] - box1Corners[0])); //"Z" axis
	//Box 2
	axisToTest.push_back(XMVector3Normalize(box2Corners[2] - box2Corners[0])); //"X" axis
	axisToTest.push_back(XMVector3Normalize(box2Corners[4] - box2Corners[0])); //"Y" axis
	axisToTest.push_back(XMVector3Normalize(box2Corners[1] - box2Corners[0])); //"Z" axis

	return axisToTest;
}

std::vector<XMVECTOR> PhysicsManager::GetSATAxis(XMMATRIX& box1Pos, XMMATRIX& box2Pos) {
	std::vector<XMVECTOR> axisToTest;
	//Box 1
	axisToTest.push_back(box1Pos.r[0]); //"X" axis
	axisToTest.push_back(box1Pos.r[1]); //"Y" axis
	axisToTest.push_back(box1Pos.r[2]); //"Z" axis
	//Box 2
	axisToTest.push_back(box2Pos.r[0]); //"X" axis
	axisToTest.push_back(box2Pos.r[1]); //"Y" axis
	axisToTest.push_back(box2Pos.r[2]); //"Z" axis

	//for (int i = 0; i < 3; ++i) {
	//	for (int j = 0; j < 3; ++j) {
	//		axisToTest.push_back((XMVector3Cross(box1Pos.r[i], box2Pos.r[j]))); //Cross axis
	//	}
	//}

	return axisToTest;
}

std::vector<XMVECTOR> PhysicsManager::GetBoxCorners(AABB& _aabbBox, XMMATRIX& boxPos) {
	std::vector<XMVECTOR> corners;
	XMFLOAT3 min = _aabbBox.min;
	XMFLOAT3 max = _aabbBox.max;

	corners.push_back(XMVectorSet(min.x, min.y, min.z, 1.0f)); //Bottom Left Back
	corners.push_back(XMVectorSet(min.x, min.y, max.z, 1.0f)); //Bottom Left Front
	corners.push_back(XMVectorSet(max.x, min.y, min.z, 1.0f)); //Bottom Right Back
	corners.push_back(XMVectorSet(max.x, min.y, max.z, 1.0f)); //Bottom Right Front

	corners.push_back(XMVectorSet(min.x, max.y, min.z, 1.0f)); //Top Left Back
	corners.push_back(XMVectorSet(min.x, max.y, max.z, 1.0f)); //Top Left Front
	corners.push_back(XMVectorSet(max.x, max.y, min.z, 1.0f)); //Top Right Back
	corners.push_back(XMVectorSet(max.x, max.y, max.z, 1.0f)); //Top Right Front

	for (unsigned int i = 0; i < corners.size(); ++i) {
		corners[i] = XMVector3TransformCoord(corners[i], boxPos);
	}

	return corners;
}

std::vector<XMVECTOR> PhysicsManager::GetBoxCorners(Collider& boxCol, XMMATRIX& boxPos) {
	std::vector<XMVECTOR> corners;
	XMVECTOR offset = XMLoadFloat3(&boxCol.centerOffset);
	XMFLOAT3 min = boxCol.colliderData->colliderInfo.boxCollider.bottLeftBackCorner;
	XMFLOAT3 max = boxCol.colliderData->colliderInfo.boxCollider.topRightFrontCorner;

	corners.push_back(XMVectorSet(min.x, min.y, min.z, 1.0f)); //Bottom Left Back
	corners.push_back(XMVectorSet(min.x, min.y, max.z, 1.0f)); //Bottom Left Front
	corners.push_back(XMVectorSet(max.x, min.y, min.z, 1.0f)); //Bottom Right Back
	corners.push_back(XMVectorSet(max.x, min.y, max.z, 1.0f)); //Bottom Right Front

	corners.push_back(XMVectorSet(min.x, max.y, min.z, 1.0f)); //Top Left Back
	corners.push_back(XMVectorSet(min.x, max.y, max.z, 1.0f)); //Top Left Front
	corners.push_back(XMVectorSet(max.x, max.y, min.z, 1.0f)); //Top Right Back
	corners.push_back(XMVectorSet(max.x, max.y, max.z, 1.0f)); //Top Right Front

	for (unsigned int i = 0; i < corners.size(); ++i) {
		corners[i] = XMVector3TransformCoord((corners[i] + offset), boxPos);
	}
	
	return corners;
}

XMVECTOR PhysicsManager::FindClosestPointOnLine(XMVECTOR& _lineSegStart, XMVECTOR& _lineSegEnd, XMVECTOR& _testPoint) {
	XMVECTOR lineSegment, lineToPoint, closest;
	lineSegment = _lineSegEnd - _lineSegStart;
	lineToPoint = _testPoint - _lineSegStart;
	float ratio = XMVectorGetX(XMVector3Dot(lineToPoint, lineSegment)) / XMVectorGetX(XMVector3Dot(lineSegment, lineSegment));
	closest = _lineSegStart + (lineSegment * ratio);

	float closestToStartLengthSq = XMVectorGetX(XMVector3LengthSq(closest - _lineSegStart));
	float closestToEndLengthSq = XMVectorGetX(XMVector3LengthSq(closest - _lineSegEnd));
	float lineSegLengthSq = XMVectorGetX(XMVector3LengthSq(lineSegment));

	if (closestToStartLengthSq > lineSegLengthSq)
		return _lineSegEnd;
	else if (closestToEndLengthSq > lineSegLengthSq)
		return _lineSegStart;
	else
		return closest;
}

void PhysicsManager::SendCollision(GameObject* obj1, GameObject* obj2) {
	//TODO: Fix this frown
	(obj1)->OnCollision(obj2);
	(obj2)->OnCollision(obj1);

	//Console::WriteLine << obj1->GetTag().c_str() << " collided with " << obj2->GetTag().c_str();
}
void PhysicsManager::SendTrigger(GameObject* obj1, GameObject* obj2) {
	obj1->OnTrigger(obj2);
}

void PhysicsManager::TestAllComponentsCollision() {
	//Console::WriteLine((int)components.GetActiveCount());
	/*static int counter;
	counter = 0;
	std::vector<PhysicsComponent*> collidingList;
	int range = (int)components.GetActiveCount();
	for(int comp1 = 0; comp1 < range; ++comp1) {
		collidingList = partitionSpace.GetComponentsToTest(&components[comp1]);
		for(int comp2 = 0; comp2 < collidingList.size(); ++comp2) {
			CollisionCheck(components[comp1], *(collidingList[comp2]));
			counter++;
		}
	}
	Console::WriteLine << counter;*/

	std::vector<PhysicsComponent*>* dynamicComp = dynamicComponents.GetActiveList();
	std::vector<PhysicsComponent*>* staticComp = staticComponents.GetActiveList();
	std::vector<PhysicsComponent*> collidingList = partitionSpace.GetComponentsToTest();

	for (unsigned int comp1Index = 0; comp1Index < collidingList.size(); ++comp1Index) {
		if (!collidingList[comp1Index])
			continue;
		for (unsigned int comp2Index = comp1Index + 1; collidingList[comp2Index]; ++comp2Index) {
			CollisionCheck(*(collidingList[comp1Index]), *(collidingList[comp2Index]));
		}
	}

	for (unsigned int staticIndex = 0; staticIndex < staticComp->size(); ++staticIndex) {
		for (unsigned int dynamicIndex = 0; dynamicIndex < dynamicComp->size(); ++dynamicIndex) {
			CollisionCheck(*(*staticComp)[staticIndex], *(*dynamicComp)[dynamicIndex]);
		}
	}
}

bool PhysicsManager::RaycastCollisionCheck(XMVECTOR& origin, XMVECTOR& direction, PhysicsComponent* collidingComp, XMVECTOR* colPoint, GameObject** colObject, float maxCastDistance) {
	bool collided = false;
	bool hasCollidingComp = false;
	XMVECTOR closestCollision = origin + (direction * maxCastDistance);
	XMVECTOR tempColPoint = g_XMFltMax;
	XMMATRIX objMatrix = XMLoadFloat4x4(&(collidingComp->parentObject->GetPosition()));
	std::vector<XMVECTOR> collisionPoints;
	if (colObject)
		*colObject = nullptr;

	for (int colliderIndex = 0; colliderIndex < collidingComp->colliders.size(); ++colliderIndex) {
		if (collidingComp->colliders[colliderIndex].isTrigger) continue;
		switch (collidingComp->colliders[colliderIndex].colliderData->colliderType)
		{
		case SPHERE:
			hasCollidingComp = RayToSphere(origin, direction, collidingComp->colliders[colliderIndex], objMatrix.r[3], &tempColPoint);
			break;
		case CAPSULE:
			hasCollidingComp = RayToCapsule(origin, direction, collidingComp->colliders[colliderIndex], objMatrix, &tempColPoint);
			break;
		case BOX:
			hasCollidingComp = RayToBox(origin, direction, collidingComp->colliders[colliderIndex], objMatrix, &tempColPoint);
			break;
		default:
			break;
		}

		if (hasCollidingComp) {
			collisionPoints.push_back(tempColPoint);
			if (colObject)
				*colObject = dynamic_cast<GameObject*>(collidingComp->parentObject);
			collided = true;
		}
	}

	if (collided) {
		float lastClosestDist = XMVectorGetX(XMVector3LengthSq(closestCollision - origin));
		float nextDist;
		for (int i = 0; i < (int)collisionPoints.size(); ++i) {
			nextDist = XMVectorGetX(XMVector3LengthSq(collisionPoints[i] - origin));
			if (lastClosestDist > nextDist) {
				closestCollision = collisionPoints[i];
				lastClosestDist = nextDist;
			}
		}
	}
	*colPoint = closestCollision;

	return collided;
}

bool PhysicsManager::RayToSphere(XMVECTOR& origin, XMVECTOR& direction, Collider& collidingComp, XMVECTOR& objectPos, XMVECTOR* colPoint, bool* isInside) {
	//If origin is inside the Sphere, it does not test it. 
	XMVECTOR collCenter = objectPos + XMLoadFloat3(&collidingComp.centerOffset);
	float radiusSq = collidingComp.colliderData->colliderInfo.sphereCollider.radius;
#ifdef _DEBUG
	DebugRenderer::AddSphere(XMFLOAT3(XMVectorGetX(objectPos), XMVectorGetY(objectPos), XMVectorGetZ(objectPos)), radiusSq, XMFLOAT3(1.0f, 1.0f, 0.0f));
#endif
	radiusSq *= radiusSq;
	if(isInside)
		*isInside = false;

	if (!IsRayInCollider(origin, collidingComp, collCenter)) {
		//Project center onto ray
		XMVECTOR rayOriginToCenter = collCenter - origin;
		float centerProjRatio = XMVectorGetX(XMVector3Dot(direction, rayOriginToCenter));
		if (centerProjRatio < -FLT_EPSILON) return false;
		
		XMVECTOR centerProjRay =  centerProjRatio * direction;
		XMVECTOR centerToRay = rayOriginToCenter - centerProjRay;
		float centerToRayLengthSq = XMVectorGetX(XMVector3LengthSq(centerToRay));

		float distanceSqDiff = radiusSq - centerToRayLengthSq;
		if (distanceSqDiff < -FLT_EPSILON)
			return false;
		if (fabsf(distanceSqDiff) < FLT_EPSILON) {
			if(colPoint)
				*colPoint = collCenter + centerToRay;
			return true;
		}
		
		if(colPoint)
			*colPoint = origin + (direction * (XMVectorGetX(XMVector3Length(centerProjRay)) - sqrt(distanceSqDiff)));
		return true;
	}
	
	if(isInside)
		*isInside = true;
	return false;
}
bool PhysicsManager::RayToCapsule(XMVECTOR& origin, XMVECTOR& direction, Collider& collidingComp, XMMATRIX& objectPos, XMVECTOR* colPoint) {
	std::vector<XMVECTOR> contactPoints;
	XMVECTOR capStart = XMVectorSet(0, collidingComp.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	XMVECTOR capEnd = -XMVectorSet(0, collidingComp.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	capStart = XMVector3TransformCoord(capStart, objectPos);
	capEnd = XMVector3TransformCoord(capEnd, objectPos);
	XMVECTOR seg1 = capEnd - capStart;

	float height = collidingComp.colliderData->colliderInfo.capsuleCollider.height;
	float halfRadius = collidingComp.colliderData->colliderInfo.capsuleCollider.radius * 0.5f;
	XMVECTOR interval = XMVector3Normalize(seg1) * halfRadius;

	XMVECTOR testPoint = capEnd;
	XMVECTOR tempContactPoint;
	bool isInside = false;

	if(RayToSphere(origin, direction, collidingComp, testPoint, &tempContactPoint, &isInside))
		contactPoints.push_back(tempContactPoint);
	else if(isInside) return false;

	testPoint = capStart;
	for (float current = 0; current < height; current += halfRadius) {
		if (RayToSphere(origin, direction, collidingComp, testPoint, &tempContactPoint, &isInside))
			contactPoints.push_back(tempContactPoint);
		else if (isInside) return false;
		testPoint += interval;
	}

	if (contactPoints.size() < 1)
		return false;

	if (colPoint) {
		float nextClosest = 0.0f, lastClosest = FLT_MAX;
		for (unsigned int i = 0; i < contactPoints.size(); ++i) {
			nextClosest = XMVectorGetX(XMVector3LengthSq(contactPoints[i] - origin));
			if (nextClosest - lastClosest < FLT_EPSILON) {
				*colPoint = contactPoints[i];
			}
		}
	}
	return true;
}
bool PhysicsManager::RayToBox(XMVECTOR& origin, XMVECTOR& direction, Collider& collidingComp, XMMATRIX& objectPos, XMVECTOR* colPoint) {
	//Point p, Vector d, AABB a, float& tmin, Point& q
	float tmin = 0.0f;	//set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX;	//set to max distance ray can travel (forsegment)

	XMMATRIX inverseObjPos = objectPos;
	inverseObjPos.r[3] += XMLoadFloat3(&collidingComp.centerOffset); //Get actual position of collider
	inverseObjPos = XMMatrixInverse(nullptr, inverseObjPos);

	XMVECTOR tranlatedOrigin = XMVector3TransformCoord(origin, inverseObjPos);
	if (IsRayInCollider(tranlatedOrigin, collidingComp, XMVectorZero())) return false;

	XMVECTOR rotationDirection = XMVector3TransformNormal(direction, inverseObjPos);
	XMFLOAT3* colMin = &collidingComp.colliderData->colliderInfo.boxCollider.bottLeftBackCorner;
	XMFLOAT3* colMax = &collidingComp.colliderData->colliderInfo.boxCollider.topRightFrontCorner;
	
	float originPoint[] = { XMVectorGetX(tranlatedOrigin), XMVectorGetY(tranlatedOrigin), XMVectorGetZ(tranlatedOrigin) };
	float dir[] = { XMVectorGetX(rotationDirection), XMVectorGetY(rotationDirection), XMVectorGetZ(rotationDirection) };
	float boxMin[] = {colMin->x, colMin->y, colMin->z};
	float boxMax[] = {colMax->x, colMax->y, colMax->z};
	
	//For all threeslabs
	for (int i = 0; i < 3; i++) {
		if (fabsf(dir[i]) < FLT_EPSILON) {
			//Ray is parallel to slab. No hit if origin not within slab
			if (originPoint[i] < boxMin[i] || originPoint[i] > boxMax[i]) return false;
		}
		else {
			//Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / dir[i];
			float t1 = (boxMin[i] - originPoint[i]) * ood;
			float t2 = (boxMax[i] - originPoint[i]) * ood;
			//Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) {
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}
			//Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			//Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
	}
	//Ray intersectsall3slabs. Returnpoint(q) andintersectiontvalue(tmin)
	if (colPoint) {
		*colPoint = origin + direction * tmin;
	}
	return true;
}

bool PhysicsManager::IsRayInCollider(XMVECTOR& origin, Collider& collidingComp, XMVECTOR& colliderCenter) {
	bool isInside = false;

	switch (collidingComp.colliderData->colliderType)
	{
	case SPHERE:
		isInside = XMVectorGetX(XMVector3LengthSq(colliderCenter - origin)) <
									(collidingComp.colliderData->colliderInfo.sphereCollider.radius * collidingComp.colliderData->colliderInfo.sphereCollider.radius);
		break;
	case CAPSULE:
		isInside = XMVectorGetX(XMVector3LengthSq(colliderCenter - origin)) <
									(collidingComp.colliderData->colliderInfo.capsuleCollider.radius * collidingComp.colliderData->colliderInfo.capsuleCollider.radius);
		break;
	case BOX:
		//Assumes box is at origin and not rotated
		{
			XMFLOAT3 point;
			XMStoreFloat3(&point, origin);
			XMFLOAT3 *min = &collidingComp.colliderData->colliderInfo.boxCollider.bottLeftBackCorner;
			XMFLOAT3 *max = &collidingComp.colliderData->colliderInfo.boxCollider.topRightFrontCorner;
			if (point.x >= min->x && point.x <= max->x) {
				if (point.y >= min->y && point.y <= max->y) {
					if (point.z >= min->z && point.z <= max->z) {
						isInside = true;
					}
				}
			}
		}
		break;
	default:
		break;
	}

	return isInside;
}


#pragma endregion
