#include "PhysicsExtension.h"
#include "PhysicsManager.h"
#include "Object.h"
#include "DebugRenderer.h"
#include "Console.h"

#define ArcSegments 15

namespace {
	DirectX::XMFLOAT3 Lerp(const DirectX::XMFLOAT3& A, const DirectX::XMFLOAT3& B, const float t) {
		//A + t(B - A)
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&A);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&B);
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, DirectX::XMVectorAdd(a, DirectX::XMVectorScale(DirectX::XMVectorSubtract(b, a), t)));
		return result;
	}

	DirectX::XMFLOAT3 GetPointOnArc(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& control, const float t) {
		DirectX::XMFLOAT3 Q0 = Lerp(start, control, t);
		DirectX::XMFLOAT3 Q1 = Lerp(control, end, t);
		DirectX::XMFLOAT3 Q2 = Lerp(Q0, Q1, t);
		return Q2;
	}
}

bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint, GameObject ** colObject, float maxCastDistance, const char* tag) {
	return PhysicsManager::Raycast(origin, direction, colPoint, colObject, maxCastDistance, tag);
}

void DrawArc(Transform* transform, const DirectX::XMFLOAT3& end, const int segmentCount = ArcSegments, const float arcHeight = 1) {
	// = { (transform->GetPosition().x + end.x) * 0.5f, transform->GetPosition().y + arcHeight, (transform->GetPosition().z + end.z) * 0.5f };
	//

	if (arcHeight < 0) {
		//Calculate specific height
	}

	float tInterval = 1 / (float)segmentCount;

	//Create valid start matrix
	DirectX::XMFLOAT4X4 start;
	DirectX::XMMATRIX start_M = DirectX::XMLoadFloat4x4(&transform->GetMatrix());
	{

		DirectX::XMVECTOR planeNormalVec = DirectX::XMVector3Cross(DirectX::XMVectorSet(0, 1, 0, 0), start_M.r[2]);
		DirectX::XMVECTOR yAxis = DirectX::XMVector3Normalize(start_M.r[1]);
		//-------------------Project y axis onto Y/Z plane
		DirectX::XMVECTOR temp = DirectX::XMVectorScale(planeNormalVec, DirectX::XMVectorGetX(DirectX::XMVector3Dot(yAxis, planeNormalVec)));
		start_M.r[1] = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(yAxis, temp));
		start_M.r[0] = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(start_M.r[1], start_M.r[2]));
		
		DirectX::XMStoreFloat4x4(&start, start_M);
		DebugRenderer::DrawAxes(start, 0.5);
	}

	//Create valid control matrix
	DirectX::XMFLOAT4X4 control;
	DirectX::XMMATRIX control_M;
	{
		control_M.r[0] = start_M.r[0];
		control_M.r[1] = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0, 1, 0, 0));
		control_M.r[2] = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(control_M.r[0], control_M.r[1]));
		control_M.r[3] = DirectX::XMVectorSet((transform->GetPosition().x + end.x) * 0.5f, transform->GetPosition().y + arcHeight, (transform->GetPosition().z + end.z) * 0.5f, 1);
		DirectX::XMStoreFloat4x4(&control, control_M);
	}

	//Find angle between Y axiis
	float theta;
	float thetaInterval;
	{
		theta = (float)acos(DirectX::XMVectorGetX(DirectX::XMVector3Dot(start_M.r[1], control_M.r[1])));
		thetaInterval = theta / (segmentCount * 0.5f);
	}


	DirectX::XMMATRIX prevPoint = start_M;

	float t = tInterval;
	for (int i = 0; i < segmentCount; ++i) {
		DirectX::XMFLOAT3 controlPos;
		DirectX::XMStoreFloat3(&controlPos, control_M.r[3]);
		DirectX::XMFLOAT4X4 point;
		//DirectX::XMMATRIX point_M = DirectX::XMMatrixMultiply( prevPoint, DirectX::XMMatrixRotationX(thetaInterval));
		DirectX::XMMATRIX point_M;
		point_M.r[0] = start_M.r[0];
		point_M.r[1] = DirectX::XMVector3Rotate(prevPoint.r[1], DirectX::XMQuaternionRotationAxis(start_M.r[0], thetaInterval));
		point_M.r[2] = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(point_M.r[0], point_M.r[1]));
		point_M.r[3] = DirectX::XMLoadFloat3(&GetPointOnArc(transform->GetPosition(), end, controlPos, t));
		DirectX::XMStoreFloat4x4(&point, point_M);

		//Do something to represent arc (current segment goes from prevPoint to point)
		DebugRenderer::DrawAxes(point, 0.5);


		prevPoint = point_M;
		t += tInterval;
	}
}

bool ArcCast(Transform* transform, DirectX::XMFLOAT3* outPos, bool Draw, float maxDistance, float minAngle, float maxAngle, float castHeight, const char* tag) {
	DirectX::XMMATRIX controllerMat = DirectX::XMLoadFloat4x4(&transform->GetMatrix());
	DirectX::XMVECTOR planeNormalVec = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR forwardVec = DirectX::XMVector3Normalize(controllerMat.r[2]);
	//This was swapped off to allow testing without vive
	//DirectX::XMVECTOR playerVec = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(playerPos._41, playerPos._42, playerPos._43));
	DirectX::XMVECTOR playerVec = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(transform->GetMatrix()._41, transform->GetMatrix()._42, transform->GetMatrix()._43));

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
		if (Raycast(rayStart, rayDirection, outPos, nullptr, 100, tag)) {
			if (Draw) DrawArc(transform, *outPos);
			return true;
		}
		else
			return false;
	}
}