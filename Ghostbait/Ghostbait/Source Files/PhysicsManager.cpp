#include "PhysicsManager.h"

PhysicsManager::PhysicsManager() {

}

PhysicsManager::~PhysicsManager() {

}

void PhysicsManager::TestAllComponentsCollision() {
	for (int comp1 = 0; comp1 < components.size(); ++comp1) {
		for (int comp2 = 0; comp2 < components.size(); ++comp2) {
			if (comp1 != comp2) {
				CollisionCheck(components[comp1], components[comp2]);
			}
		}
	}
}

void PhysicsManager::CollisionCheck(PhysicsComponent component1, PhysicsComponent component2) {
	for (int com1 = 0; com1 < component1.colliders.size(); ++com1) {
		for (int com2 = 0; com2 < component2.colliders.size(); ++com2) {
			if (component1.colliders[com1].colliderData->colliderType == SPHERE &&
				component2.colliders[com2].colliderData->colliderType == SPHERE) {
				if (SphereToSphereCollision(component1.colliders[com1], component1.srcObj->position.r[3], component2.colliders[com2], component2.srcObj->position.r[3])) {
					//REACT
				}
			}

			else if (component1.colliders[com1].colliderData->colliderType == CAPSULE &&
				component2.colliders[com2].colliderData->colliderType == CAPSULE) {
				if (CapsuleToCapsuleCollision(component1.colliders[com1], component1.srcObj->position, component2.colliders[com2], component2.srcObj->position)) {
					//REACT
				}
			}

			else if (component1.colliders[com1].colliderData->colliderType == BOX &&
				component2.colliders[com2].colliderData->colliderType == BOX) {
				if (BoxToBoxCollision()) {
					//REACT
				}
			}
		}
	}
}
bool PhysicsManager::SphereToSphereCollision(Collider col1, XMVECTOR& pos1, Collider col2, XMVECTOR& pos2) {
	XMVECTOR offset1 = XMLoadFloat3(&col1.centerOffset);
	XMVECTOR offset2 = XMLoadFloat3(&col2.centerOffset);
	XMVECTOR position1 = offset1 + pos1;
	XMVECTOR position2 = offset2 + pos2;
	XMFLOAT3 between; XMStoreFloat3(&between, position1 - position2);
	float sqrDist = between.x*between.x + between.y*between.y + between.z*between.z;
	float combinedRad = (((SphereCollider*)(col1.colliderData))->radius + ((SphereCollider*)(col2.colliderData))->radius);
	if (sqrDist < (combinedRad*combinedRad)) return true;
	return false;
}
bool PhysicsManager::BoxToBoxCollision() {
	return false;
}
bool PhysicsManager::CapsuleToCapsuleCollision(Collider col1, XMMATRIX& pos1, Collider col2, XMMATRIX& pos2) {
	//Create Capsule 1
	XMVECTOR cap1Offset = XMLoadFloat3(&col1.centerOffset);
	XMVECTOR cap1A = cap1Offset + XMVectorSet(0, ((CapsuleCollider*)col1.colliderData)->height * 0.5f, 0, 0);
	XMVECTOR cap1B = cap1Offset - XMVectorSet(0, ((CapsuleCollider*)col1.colliderData)->height * 0.5f, 0, 0);
	cap1A = XMVector3TransformCoord(cap1A, pos1);
	cap1B = XMVector3TransformCoord(cap1B, pos1);
	XMVECTOR seg1 = cap1A - cap1B;

	//Create Capsule 2
	XMVECTOR cap2Offset = XMLoadFloat3(&col2.centerOffset);
	XMVECTOR cap2A = cap2Offset + XMVectorSet(0, ((CapsuleCollider*)col2.colliderData)->height * 0.5f, 0, 0);
	XMVECTOR cap2B = cap2Offset - XMVectorSet(0, ((CapsuleCollider*)col2.colliderData)->height * 0.5f, 0, 0);
	cap2A = XMVector3TransformCoord(cap2A, pos2);
	cap2B = XMVector3TransformCoord(cap2B, pos2);
	XMVECTOR seg2 = cap2A - cap2B;

	float dot, segmentDistance;
	XMStoreFloat(&dot, XMVector3Dot(seg1, seg2));

	//If lines intersecting (Collision)
	//Else find closest points on the line segments



	return false;
}
