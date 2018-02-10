#include "PhysicsComponent.h"
#include <DirectXMath.h>
#include "RigidBody.h"

bool PhysicsComponent::AddCollider(ColliderData * _colData, float _offsetX, float _offsetY, float _offsetZ, bool _isTrigger) {
	if(_colData) {
		Collider temp;
		temp.colliderData = _colData;
		temp.centerOffset = DirectX::XMFLOAT3(_offsetX, _offsetY, _offsetZ);
		temp.isTrigger = _isTrigger;
		colliders.push_back(temp);
		return true;
	}
	return false;
}
