#include "PhysicsExtension.h"
#include "PhysicsManager.h"

bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint, GameObject* colObject, float maxCastDistance) {
	return PhysicsManager::Raycast(origin, direction, colPoint, colObject, maxCastDistance);
}
