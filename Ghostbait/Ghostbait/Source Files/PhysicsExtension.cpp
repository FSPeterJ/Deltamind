#include "PhysicsExtension.h"
#include "PhysicsManager.h"
#include "Object.h"
#include "DebugRenderer.h"
#include "Console.h"
#include "GhostTime.h"
#include  "MessageEvents.h"
#include "MessageStructs.h"
#include "ObjectFactory.h"
#include "Animator.h"

#define ArcPoints 20


void ArcObject::Create() {
	if (!object) {
		MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<GameObject>(ObjectFactory::CreatePrefab(&std::string("Assets/Arc2.ghost")), { 0, 0, 0 }, &object));
		object->PersistOnReset();
		backup = object;
	}
}
GameObject* ArcObject::Get() {
	return object;
}
void ArcObject::Destroy() {
	if (object) {
		MessageEvents::SendQueueMessage(EVENT_Late, [=] { backup->Destroy(); backup = nullptr; });
		object = nullptr;
	}
}





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

	void DrawArc(Transform* transform, const DirectX::XMFLOAT3& end, ArcObject* arc, const float arcHeight = 1) {
		if (arcHeight < 0) {
			//Calculate specific height
		}

		float tInterval = 1 / (float)(ArcPoints - 1);

		//Create valid control matrix
		DirectX::XMFLOAT3 control;
		{
			control.x = (transform->GetPosition().x + end.x) * 0.5f;
			control.y = transform->GetPosition().y + arcHeight;
			control.z = (transform->GetPosition().z + end.z) * 0.5f;
		}
		
		//Find positions
		DirectX::XMFLOAT3 points[ArcPoints];
		//DirectX::XMStoreFloat3(&points[0], start_M.r[3]);

		float t = 0;
		for (int i = 0; i < ArcPoints; ++i) {
			points[i] = GetPointOnArc(transform->GetPosition(), end, control, t);
			t += tInterval;
		}

		//Set Rotations
		for (int i = 0; i < ArcPoints; ++i) {
			Transform tran;
			tran.SetPosition(points[i]);
			if(i < ArcPoints - 1) tran.LookAt(points[i + 1]);
			arc->Get()->GetComponent<Animator>()->SetJointMatrix(i, tran.GetMatrix());
			//DebugRenderer::DrawAxes(arc->GetComponent<Animator>()->GetJointMatrix(i), 0.25f);
		}
	}
}



bool Raycast(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint, GameObject ** colObject, float maxCastDistance, const char* tag) {
	return PhysicsManager::Raycast(origin, direction, colPoint, colObject, maxCastDistance, tag);
}

bool ArcCast(Transform* transform, DirectX::XMFLOAT3* outPos, ArcObject* arc, float maxDistance, float minAngle, float maxAngle, float castHeight, const char* tag) {
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
			if (arc && arc->Get()) DrawArc(transform, *outPos, arc);
			return true;
		}
		else
			return false;
	}
}