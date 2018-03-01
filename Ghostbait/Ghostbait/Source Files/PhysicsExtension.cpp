#include "PhysicsExtension.h"
#include "PhysicsManager.h"
#include "Object.h"

bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint, GameObject ** colObject, float maxCastDistance) {
	return PhysicsManager::Raycast(origin, direction, colPoint, colObject, maxCastDistance);
}

bool ArcCast(Object* controller, DirectX::XMFLOAT3* outPos, float maxDistance, float minAngle, float maxAngle, float castHeight) {
	DirectX::XMMATRIX controllerMat = DirectX::XMLoadFloat4x4(&controller->transform.GetMatrix());
	DirectX::XMVECTOR planeNormalVec = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR forwardVec = DirectX::XMVector3Normalize(controllerMat.r[2]);
	//This was swapped off to allow testing without vive
	//DirectX::XMVECTOR playerVec = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(playerPos._41, playerPos._42, playerPos._43));
	DirectX::XMVECTOR playerVec = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(controller->transform.GetMatrix()._41, controller->transform.GetMatrix()._42, controller->transform.GetMatrix()._43));

	float maxRaycastDist = 100;

	//1	--Get horizontal Line direction from forward vector
	DirectX::XMVECTOR direction;
	{
		//-------------------Project Controller forward onto X/Z plane
		DirectX::XMVECTOR temp = DirectX::XMVectorScale(planeNormalVec, DirectX::XMVectorGetX(DirectX::XMVector3Dot(forwardVec, planeNormalVec)));
		direction = DirectX::XMVectorSubtract(forwardVec, temp);
	}

	//2 --Find point on Horizontal Line
	DirectX::XMVECTOR pointOnLine;
	{
		//-------------------Calculate current controller angle
		float controllerAngle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(DirectX::XMVectorSet(0, -1, 0, 0), forwardVec));
		controllerAngle = DirectX::XMConvertToDegrees(controllerAngle);
		//-------------------clamp it between min and max
		controllerAngle = (controllerAngle < minAngle ? minAngle : (controllerAngle > maxAngle ? maxAngle : controllerAngle));
		//-------------------Calculate distance to use (t of max distance)
		float t = (controllerAngle - minAngle) / (maxAngle - minAngle); // Normalized pitch within range
		float distOnLine = (maxDistance + 1) * t;
		//-------------------Find point along the horizontal ray
		pointOnLine = DirectX::XMVectorAdd(playerVec, DirectX::XMVectorScale(direction, distOnLine));
	}

	//3 --Cast ray from cast point to the horizontal line point
	{
		//-------------------Create a vector from castPoint to the point along the horizontal ray
		DirectX::XMFLOAT3 player3; DirectX::XMStoreFloat3(&player3, playerVec);
		player3.y += castHeight;
		DirectX::XMVECTOR castPoint = DirectX::XMLoadFloat3(&player3);
		DirectX::XMVECTOR castDirection = DirectX::XMVectorSubtract(pointOnLine, castPoint);
		//-------------------Cast a rayfrom the cast point along this vector
		DirectX::XMFLOAT3 rayStart, rayDirection;
		DirectX::XMStoreFloat3(&rayStart, castPoint);
		DirectX::XMStoreFloat3(&rayDirection, castDirection);
		if (Raycast(rayStart, rayDirection, outPos))
			return true;
		else
			return false;
	}
}
