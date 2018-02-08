#include "PhysicsManager.h"
#include "DebugRenderer.h"
#include "GameObject.h"
#include "Console.h"
//#include <DirectXMath.h>

Collider PhysicsManager::defaultColider;
ColliderData PhysicsManager::defaultSphereColider;
SpatialPartition PhysicsManager::partitionSpace;


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
}
PhysicsManager::~PhysicsManager() {}

void PhysicsManager::AddComponent(GameObject* obj, float veloX, float veloY, float veloZ) {
	PhysicsComponent* physComponent = components.ActivateMemory();
	physComponent->colliders.push_back(defaultColider);
	physComponent->parentObject = obj;
	physComponent->rigidBody = RigidBody();
	physComponent->rigidBody.SetVelocity(veloX, veloY, veloZ);
}

PhysicsComponent* PhysicsManager::CloneComponent(ComponentBase* reference) {
	PhysicsComponent* physComponent = components.ActivateMemory();
	// SHALLOW COPY - this only copies the std::vector head.
	physComponent->colliders = ((PhysicsComponent*)reference)->colliders;	physComponent->currentAABB = ((PhysicsComponent*)reference)->currentAABB;
	physComponent->previousAABB = ((PhysicsComponent*)reference)->previousAABB;
	physComponent->baseAABB = ((PhysicsComponent*)reference)->baseAABB;

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

			incomingMax = { incomingMax.x + radiusHolder, incomingMax.y + radiusHolder, incomingMax.z + radiusHolder };
			incomingMin = { incomingMin.x - radiusHolder, incomingMin.y - radiusHolder, incomingMin.z - radiusHolder };
		}

		//Update AABB
		if (incomingMax.x > aabbMax.x) { aabbMax.x = incomingMax.x; };
		if (incomingMax.y > aabbMax.y) { aabbMax.y = incomingMax.y; };
		if (incomingMax.z > aabbMax.z) { aabbMax.z = incomingMax.z; };
		if (incomingMin.x < aabbMin.x) { aabbMin.x = incomingMin.x; };
		if (incomingMin.y < aabbMin.y) { aabbMin.y = incomingMin.y; };
		if (incomingMin.z < aabbMin.z) { aabbMin.z = incomingMin.z; };

		compHolder.AddCollider(colDataHolder, offsetHolder.x, offsetHolder.y, offsetHolder.z);
		colDataHolder = nullptr;
		delete[] typeName; //TODO: typeName is allocated with new[] but is only being deleted with delete (not delete[])
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
	components.DeactivateMemory(reset);
	partitionSpace.RemoveComponent((PhysicsComponent*)reset);
}

void PhysicsManager::Update() {
	std::vector<PhysicsComponent*>*temp = components.GetActiveList();
	const int activeCount = (int) components.GetActiveCount();
	for(int i = 0; i < activeCount; ++i) {
		//This seems absurd, are we sure we can't use XMVECTOR and XMMATRIX in a more manageable manner?
		XMFLOAT4* objectPosition = (XMFLOAT4*) &components[i].parentObject->position.m[3];
		XMVECTOR newposition = XMLoadFloat4(objectPosition);
		components[i].rigidBody.Update();
		newposition += components[i].rigidBody.GetVelocity();
		XMStoreFloat4(objectPosition, newposition);
		UpdateAABB(components[i]);
		partitionSpace.UpdateComponent(&components[i]);
		//components[i].parentObject->position.r[3] += components[i].rigidBody.GetVelocity() * dt;

#if _DEBUG
		for(unsigned int colInd = 0; colInd < components[i].colliders.size(); ++colInd) {
			XMVECTOR offset = XMLoadFloat3(&(components[i].colliders[colInd].centerOffset));
			XMFLOAT3 colPos;
			XMStoreFloat3(&colPos, newposition + offset);

			switch(components[i].colliders[colInd].colliderData->colliderType) {
			case SPHERE:
				DebugRenderer::AddSphere(colPos, components[i].colliders[colInd].colliderData->colliderInfo.sphereCollider.radius, XMFLOAT3(1.0f, 0.0f, 0.0f));
				break;
			case CAPSULE:
			{
				float _height = components[i].colliders[colInd].colliderData->colliderInfo.capsuleCollider.height;
				XMVECTOR cap1A = offset + XMVectorSet(0, _height * 0.5f, 0, 0);
				XMVECTOR cap1B = offset - XMVectorSet(0, _height * 0.5f, 0, 0);
				cap1A = XMVector3TransformCoord(cap1A, XMLoadFloat4x4(&(components[i].parentObject->position)));
				cap1B = XMVector3TransformCoord(cap1B, XMLoadFloat4x4(&(components[i].parentObject->position)));
				XMFLOAT3 capStart, capEnd;
				XMStoreFloat3(&capStart, cap1A);
				XMStoreFloat3(&capEnd, cap1B);
				DebugRenderer::AddSphere(capStart, components[i].colliders[colInd].colliderData->colliderInfo.capsuleCollider.radius, XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddSphere(capEnd, components[i].colliders[colInd].colliderData->colliderInfo.capsuleCollider.radius, XMFLOAT3(1.0f, 0.0f, 0.0f));
				DebugRenderer::AddLine(capStart, capEnd, XMFLOAT3(1.0f, 0.0f, 0.0f));
			}
			break;
			default:
				break;
			}
		}

		DebugRenderer::AddBox(components[i].currentAABB.min, components[i].currentAABB.max, XMFLOAT3(0.0f, 1.0f, 0.0f));

#endif
	}
	//components[0].srcObj->position.r[3] -= XMVectorSet(0, dt, 0, 0);

	TestAllComponentsCollision();
}

bool PhysicsManager::Raycast(XMFLOAT3& origin, XMFLOAT3& direction, XMFLOAT3* colPoint, GameObject* colObject, float maxCastDistance) {
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

	colObject = nullptr;

	for (int iteration = 0; iteration < 10000; ++iteration) {
		XMStoreFloat3(&nextSegment, vecNextSeg);
		nextIndex = partitionSpace.GetHashedIndex(nextSegment);
		vecNextSeg += vecSegInterval;
		if (currBucketIndex == nextIndex)
			continue;
		currBucketIndex = nextIndex;
		compToTest = partitionSpace.GetComponentsToTest(currBucketIndex);

		for (int compIndex = 0; compIndex < compToTest.size(); ++compIndex) {
			if (RaycastCollisionCheck(vecOrigin, vecDirection, compToTest[compIndex], &tempCollidePt, tempCollideObj, maxCastDistance)) {
				collisionPoints.push_back(tempCollidePt);
				collidedObjects.push_back(tempCollideObj);
				collided = true;
			}
		}

		if (collided)
			break;
	}

	if (collided) {
		float lastClosestDist = XMVectorGetX(XMVector3LengthSq(closestCollision - vecOrigin));
		float nextDist;
		for (int i = 0; i < (int)collisionPoints.size(); ++i) {
			nextDist = XMVectorGetX(XMVector3LengthSq(collisionPoints[i] - vecOrigin));
			if (lastClosestDist > nextDist) {
				closestCollision = collisionPoints[i];
				colObject = collidedObjects[i];
				lastClosestDist = nextDist;
				Console::WriteLine << "RAY HIT";
			}
		}
	}
	if(colPoint)
		XMStoreFloat3(colPoint, closestCollision);

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
	component.currentAABB.max.x = component.baseAABB.max.x + component.parentObject->position._41;
	component.currentAABB.max.y = component.baseAABB.max.y + component.parentObject->position._42;
	component.currentAABB.max.z = component.baseAABB.max.z + component.parentObject->position._43;
	component.currentAABB.min.x = component.baseAABB.min.x + component.parentObject->position._41;
	component.currentAABB.min.y = component.baseAABB.min.y + component.parentObject->position._42;
	component.currentAABB.min.z = component.baseAABB.min.z + component.parentObject->position._43;

}
void PhysicsManager::CollisionCheck(PhysicsComponent component1, PhysicsComponent component2) {
	bool collisionResult = false;
	ColliderType colliderType1, colliderType2;

	XMMATRIX matrixComA = XMLoadFloat4x4(&component1.parentObject->position);
	XMMATRIX matrixComB = XMLoadFloat4x4(&component2.parentObject->position);

	for(unsigned int com1 = 0; com1 < component1.colliders.size(); ++com1) {
		colliderType1 = component1.colliders[com1].colliderData->colliderType;

		for (unsigned int com2 = 0; com2 < component2.colliders.size(); ++com2) {
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
				default:
					break;
				}
			}
			break;

			case BOX:
				break;
			default:
				break;
			}

			if (collisionResult) {
				SendCollision((GameObject*)component1.parentObject, (GameObject*)component2.parentObject);
				//Console::WriteLine << "Collided";
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
bool PhysicsManager::BoxToBoxCollision() {
	return false;
}
bool PhysicsManager::CapsuleToCapsuleCollision(Collider& col1, XMMATRIX& pos1, Collider& col2, XMMATRIX& pos2) {
	//*** Edge case where tall/long capsules intersect will not collide ***
	
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

	std::vector<PhysicsComponent*> collidingList = partitionSpace.GetComponentsToTest();

	for (unsigned int comp1Index = 0; comp1Index < collidingList.size(); ++comp1Index) {
		if (!collidingList[comp1Index])
			continue;
		for (unsigned int comp2Index = comp1Index + 1; collidingList[comp2Index]; ++comp2Index) {
			CollisionCheck(*(collidingList[comp1Index]), *(collidingList[comp2Index]));
		}
	}
}

bool PhysicsManager::RaycastCollisionCheck(XMVECTOR& origin, XMVECTOR& direction, PhysicsComponent* collidingComp, XMVECTOR* colPoint, GameObject* colObject, float maxCastDistance) {
	bool collided = false;
	bool hasCollidingComp = false;
	XMVECTOR closestCollision = origin + (direction * maxCastDistance);
	XMVECTOR tempColPoint = g_XMFltMax;
	XMVECTOR center = XMVectorSet(collidingComp->parentObject->position._41, collidingComp->parentObject->position._42, collidingComp->parentObject->position._43, collidingComp->parentObject->position._44);
	std::vector<XMVECTOR> collisionPoints;

	colObject = nullptr;

	for (int colliderIndex = 0; colliderIndex < collidingComp->colliders.size(); ++colliderIndex) {
		switch (collidingComp->colliders[colliderIndex].colliderData->colliderType)
		{
		case SPHERE:
			hasCollidingComp = RayToSphere(origin, direction, collidingComp->colliders[colliderIndex], center, &tempColPoint);
			break;
		case CAPSULE:
			hasCollidingComp = RayToCapsule(origin, direction, collidingComp->colliders[colliderIndex], center, &tempColPoint);
			break;
		case BOX:
			hasCollidingComp = RayToBox(origin, direction, collidingComp->colliders[colliderIndex], center, &tempColPoint);
			break;
		default:
			break;
		}

		if (hasCollidingComp) {
			collisionPoints.push_back(tempColPoint);
			colObject = dynamic_cast<GameObject*>(collidingComp->parentObject);
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

bool PhysicsManager::RayToSphere(XMVECTOR& origin, XMVECTOR& direction, Collider& collidingComp, XMVECTOR& objectPos, XMVECTOR* colPoint) {
	//If origin is inside the Sphere, it does not test it. 
	XMVECTOR collCenter = objectPos + XMLoadFloat3(&collidingComp.centerOffset);
	float radiusSq = collidingComp.colliderData->colliderInfo.sphereCollider.radius;
	radiusSq *= radiusSq;

	if (!IsRayInCollider(origin, collidingComp, collCenter)) {
		//Project center onto ray
		XMVECTOR rayOriginToCenter = collCenter - origin;
		XMVECTOR centerProjRay = XMVectorGetX(XMVector3Dot(direction, rayOriginToCenter)) * direction;
		XMVECTOR centerToRay = rayOriginToCenter - centerProjRay;
		float centerToRayLengthSq = XMVectorGetX(XMVector3LengthSq(centerToRay));

		float distanceSqDiff = radiusSq - centerToRayLengthSq;
		if (distanceSqDiff < -FLT_EPSILON)
			return false;
		if (fabsf(distanceSqDiff) < FLT_EPSILON) {
			*colPoint = collCenter + centerToRay;
			return true;
		}
		
		*colPoint = origin + (direction * (XMVectorGetX(XMVector3Length(centerProjRay)) - sqrt(distanceSqDiff)));
		return true;
	}
	return false;
}
bool PhysicsManager::RayToCapsule(XMVECTOR& origin, XMVECTOR& direction, Collider& collidingComp, XMVECTOR& objectPos, XMVECTOR* colPoint) {
	return false;
}
bool PhysicsManager::RayToBox(XMVECTOR& origin, XMVECTOR& direction, Collider& collidingComp, XMVECTOR& objectPos, XMVECTOR* colPoint) {
	return false;
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
		break;
	case BOX:
		break;
	default:
		break;
	}

	return isInside;
}


#pragma endregion
