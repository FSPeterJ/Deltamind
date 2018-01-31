#include "PhysicsManager.h"
#include "DebugRenderer.h"
#include "GameObject.h"

Collider PhysicsManager::defaultColider;
ColliderData PhysicsManager::defaultSphereColider;

using namespace DirectX;

#pragma region Public Functions

PhysicsManager::PhysicsManager() {
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
PhysicsManager::~PhysicsManager() {}

void PhysicsManager::AddComponent(GameObject* obj, float veloX, float veloY, float veloZ) {
	PhysicsComponent* physComponent = components.ActivateMemory();
	physComponent->colliders.push_back(defaultColider);
	physComponent->parentObject = obj;
	physComponent->rigidBody = RigidBody();
	physComponent->rigidBody.SetVelocity(veloX, veloY, veloZ);
}

PhysicsComponent* PhysicsManager::CloneComponent(ComponentBase* reference){
	PhysicsComponent* physComponent = components.ActivateMemory();
	// SHALLOW COPY - this only copies the std::vector head.
	physComponent->colliders = ((PhysicsComponent*)reference)->colliders;
	physComponent->rigidBody = RigidBody();
	return physComponent;
}
PhysicsComponent* PhysicsManager::GetReferenceComponent(const char * _FilePath, const char * _dataBlock){
	PhysicsComponent compHolder;
	XMFLOAT3 offsetHolder;
	ColliderData* colDataHolder = nullptr;
	int numofColliders, lengthofTypeName, currIndex = 0;
	float radiusHolder, heightHolder;
	char* typeName;

	char sphere[] = "SPHERE";
	char capsule[] = "CAPSULE";
	char box[] = "BOX";


	memcpy(&numofColliders, &_dataBlock[currIndex], sizeof(numofColliders));
	currIndex += sizeof(numofColliders);

	for (int i = 0; i < numofColliders; ++i) {
		memcpy(&lengthofTypeName, &_dataBlock[currIndex], sizeof(lengthofTypeName));
		currIndex += sizeof(lengthofTypeName);
		typeName = new char[lengthofTypeName];
		memcpy(&typeName[0], &_dataBlock[currIndex], lengthofTypeName);
		currIndex += lengthofTypeName;
		memcpy(&offsetHolder, &_dataBlock[currIndex], sizeof(float) * 3);
		currIndex += sizeof(float) * 3;

		if (!strcmp(typeName, sphere)) {
			memcpy(&radiusHolder, &_dataBlock[currIndex], sizeof(radiusHolder));
			currIndex += sizeof(radiusHolder);
			colDataHolder = AddColliderData(radiusHolder);
		}
		else if (!strcmp(typeName, capsule)) {
			memcpy(&radiusHolder, &_dataBlock[currIndex], sizeof(radiusHolder));
			currIndex += sizeof(radiusHolder);
			memcpy(&heightHolder, &_dataBlock[currIndex], sizeof(heightHolder));
			currIndex += sizeof(heightHolder);
			colDataHolder = AddColliderData(radiusHolder, heightHolder);
		}
		else if (!strcmp(typeName, box)) {
			XMFLOAT3 tempMax, tempMin;

			memcpy(&tempMax, &_dataBlock[currIndex], sizeof(float) * 3);
			currIndex += sizeof(float) * 3;
			memcpy(&tempMin, &_dataBlock[currIndex], sizeof(float) * 3);
			currIndex += sizeof(float) * 3;
			colDataHolder = AddColliderData(tempMax.x, tempMax.y, tempMax.z, tempMin.x, tempMin.y, tempMin.z);
		}

		compHolder.AddCollider(colDataHolder, offsetHolder.x, offsetHolder.y, offsetHolder.z);
		colDataHolder = nullptr;
		delete[] typeName; //TODO: typeName is allocated with new[] but is only being deleted with delete (not delete[])
	}

	if (prefabComponents.size() < MAX_PREFABS) {
		prefabComponents.push_back(compHolder);
		return &prefabComponents.back();
	}
	else {
		//If it breaks here, you ran out of prefab space. change MAX_PREFABS 
		return nullptr;
	}
}

void PhysicsManager::ResetComponent(ComponentBase * reset) {
	components.DeactivateMemory(reset);
}

void PhysicsManager::Update() {
	std::vector<PhysicsComponent*>*temp = components.GetActiveList();
	const int activeCount = (int)components.GetActiveCount();
	for (int i = 0; i < activeCount; ++i) {
		//This seems absurd, are we sure we can't use XMVECTOR and XMMATRIX in a more manageable manner?
		XMFLOAT4* objectPosition = (XMFLOAT4*)&components[i].parentObject->position.m[3];
		XMVECTOR newposition = XMLoadFloat4(objectPosition);
		components[i].rigidBody.Update();
		newposition += components[i].rigidBody.GetVelocity();
		XMStoreFloat4(objectPosition, newposition);
		//components[i].parentObject->position.r[3] += components[i].rigidBody.GetVelocity() * dt;

#if _DEBUG
		for (int colInd = 0; colInd < components[i].colliders.size(); ++colInd) {
			XMVECTOR offset = XMLoadFloat3(&(components[i].colliders[colInd].centerOffset));
			XMFLOAT3 colPos;
			XMStoreFloat3(&colPos, newposition + offset);

			switch (components[i].colliders[colInd].colliderData->colliderType)
			{
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

#endif
	}
	//components[0].srcObj->position.r[3] -= XMVectorSet(0, dt, 0, 0);
	
	TestAllComponentsCollision();
}

#pragma endregion

#pragma region Private Functions

ColliderData* PhysicsManager::AddColliderData(float _radius) {
	if (colliderDataList.size() < MAX_COLLIDER_DATA) {
		ColliderData temp;
		temp.colliderType = SPHERE;
		temp.colliderInfo.sphereCollider.radius = _radius;
		colliderDataList.push_back(temp);
		return &colliderDataList.back();
	}
	return nullptr;
}
ColliderData* PhysicsManager::AddColliderData(float _radius, float _height) {
	if (colliderDataList.size() < MAX_COLLIDER_DATA) {
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
	if (colliderDataList.size() < MAX_COLLIDER_DATA) {
		ColliderData temp;
		temp.colliderType = BOX;
		temp.colliderInfo.boxCollider.topRightFrontCorner = XMFLOAT3(trfX, trfY, trfZ);
		temp.colliderInfo.boxCollider.bottLeftBackCorner = XMFLOAT3(blbX, blbY, blbZ);

		colliderDataList.push_back(temp);
		return &colliderDataList.back();
	}
	return nullptr;
}

void PhysicsManager::CollisionCheck(PhysicsComponent component1, PhysicsComponent component2) {
	bool collisionResult = false;
	ColliderType colliderType1, colliderType2;

	XMMATRIX matrixComA = XMLoadFloat4x4(&component1.parentObject->position);
	XMMATRIX matrixComB = XMLoadFloat4x4(&component2.parentObject->position);

	for (int com1 = 0; com1 < component1.colliders.size(); ++com1) {
		colliderType1 = component1.colliders[com1].colliderData->colliderType;

		for (int com2 = 0; com2 < component2.colliders.size(); ++com2) {
			collisionResult = false;
			colliderType2 = component2.colliders[com2].colliderData->colliderType;

			switch (colliderType1)
			{
			case SPHERE: 
			{
				switch (colliderType2)
				{
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
				switch (colliderType2)
				{
				case SPHERE:
					collisionResult = CapsuleToSphereCollision(component1.colliders[com1], matrixComA, component2.colliders[com2], matrixComB);
					break;
				case CAPSULE:
					collisionResult = CapsuleToCapsuleCollision(component1.colliders[com1], matrixComA, component2.colliders[com2], matrixComB);
					break;
				default:
					break;
				}
			} break;

			case BOX:
				break;
			default:
				break;
			}

			if(collisionResult)
				SendCollision((GameObject*)component1.parentObject, (GameObject*)component2.parentObject);
		}
	}
}
bool PhysicsManager::SphereToSphereCollision(Collider col1, XMVECTOR& pos1, Collider col2, XMVECTOR& pos2) {
	XMVECTOR offset1 = XMLoadFloat3(&col1.centerOffset);
	XMVECTOR offset2 = XMLoadFloat3(&col2.centerOffset);
	XMVECTOR position1 = offset1 + pos1;
	XMVECTOR position2 = offset2 + pos2;
	XMFLOAT3 between;
	XMStoreFloat3(&between, position1 - position2);
	float sqrDist = between.x*between.x + between.y*between.y + between.z*between.z;
	float combinedRad = col1.colliderData->colliderInfo.sphereCollider.radius + col2.colliderData->colliderInfo.sphereCollider.radius;
	if (sqrDist < (combinedRad*combinedRad)) return true;
	return false;
}
bool PhysicsManager::BoxToBoxCollision() {
	return false;
}
bool PhysicsManager::CapsuleToCapsuleCollision(Collider col1, XMMATRIX& pos1, Collider col2, XMMATRIX& pos2) {
	//Create Capsule 1
	XMVECTOR cap1Offset = XMLoadFloat3(&col1.centerOffset);
	XMVECTOR cap1A = cap1Offset + XMVectorSet(0, col1.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	XMVECTOR cap1B = cap1Offset - XMVectorSet(0, col1.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	cap1A = XMVector3TransformCoord(cap1A, pos1);
	cap1B = XMVector3TransformCoord(cap1B, pos1);
	//XMVECTOR seg1 = cap1A - cap1B;

	//Create Capsule 2
	XMVECTOR cap2Offset = XMLoadFloat3(&col2.centerOffset);
	XMVECTOR cap2A = cap2Offset + XMVectorSet(0, col2.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	XMVECTOR cap2B = cap2Offset - XMVectorSet(0, col2.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	cap2A = XMVector3TransformCoord(cap2A, pos2);
	cap2B = XMVector3TransformCoord(cap2B, pos2);
	//XMVECTOR seg2 = cap2A - cap2B;

	

	//float dot;// , segmentDistance;
	//XMStoreFloat(&dot, XMVector3Dot(seg1, seg2));

	//If lines intersecting (Collision)
	//Else find closest points on the line segments

	return false;
}

bool PhysicsManager::CapsuleToSphereCollision(Collider capCol, DirectX::XMMATRIX& capPos, Collider sphCol, DirectX::XMMATRIX& sphPos) {
	XMVECTOR sphereCenter, closestOnCap, capStart, capEnd;

	XMVECTOR capOffset = XMLoadFloat3(&capCol.centerOffset);
	capStart = capOffset + XMVectorSet(0, capCol.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	capEnd = capOffset - XMVectorSet(0, capCol.colliderData->colliderInfo.capsuleCollider.height * 0.5f, 0, 0);
	capStart = XMVector3TransformCoord(capStart, capPos);
	capEnd = XMVector3TransformCoord(capEnd, capPos);

	sphereCenter = XMVector3TransformCoord(XMLoadFloat3(&sphCol.centerOffset),sphPos);
	closestOnCap = FindClosestPointOnLine(capStart, capEnd, sphereCenter);

	float radii = capCol.colliderData->colliderInfo.capsuleCollider.radius + sphCol.colliderData->colliderInfo.sphereCollider.radius;
	XMVECTOR sphCenterToClosest = closestOnCap - sphereCenter;
	return !(XMVectorGetX(XMVector3Dot(sphCenterToClosest, sphCenterToClosest)) > (radii * radii));
}

XMVECTOR PhysicsManager::FindClosestPointOnLine(XMVECTOR& _lineSegStart, XMVECTOR& _lineSegEnd, XMVECTOR& _testPoint) {
	XMVECTOR lineSegment, lineToPoint;
	lineSegment = _lineSegEnd - _lineSegStart;
	lineToPoint = _testPoint - _lineSegStart;
	float ratio = XMVectorGetX(XMVector3Dot(lineToPoint, lineSegment)) / XMVectorGetX(XMVector3Dot(lineSegment, lineSegment));
	return _lineSegStart + (lineSegment * ratio);
}

void PhysicsManager::SendCollision(GameObject* obj1,GameObject* obj2) {
	//TODO: Fix this frown
	(obj1)->OnCollision(obj2);
	(obj2)->OnCollision(obj1);

}

void PhysicsManager::TestAllComponentsCollision() {
	//Console::WriteLine((int)components.GetActiveCount());
	int range = (int)components.GetActiveCount();
	for (int comp1 = 0; comp1 < range; ++comp1) {
		for (int comp2 = 0; comp2 < range; ++comp2) {
			if (range <= comp1) {
				break;
			}
			if (comp1 != comp2) {
				CollisionCheck(components[comp1], components[comp2]);
			}
			range = (int)components.GetActiveCount();
		}
	}
}

#pragma endregion


