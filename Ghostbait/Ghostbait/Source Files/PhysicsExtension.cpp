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
#include "Material.h"
using namespace Common;

#define ArcPoints 20

void CastObject::Create(bool renderToFront, Color color) {
	if (!object) {
		MessageEvents::SendMessage(EVENT_InstantiateRequestByType, InstantiateTypeMessage<GameObject>(ObjectFactory::CreatePrefab(&std::string(fileName)), { 0, 0, 0 }, &object));
		object->PersistOnReset();
		object->ToggleFlag(GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
		backup = object;
		if (renderToFront) {
			object->RenderToFront();
		}
		if (object->GetComponent<Animator>()) {
			for (int i = 0; i < ArcPoints; ++i) {
				DirectX::XMFLOAT4X4 temp = object->GetComponent<Animator>()->GetJointMatrix(i);
				temp._11 *= -1;
				temp._12 *= -1;
				temp._13 *= -1;
				
				temp._31 *= -1;
				temp._32 *= -1;
				temp._33 *= -1;
				object->GetComponent<Animator>()->SetJointMatrix(i, temp);
			}
		}

		switch (color) {
			case COLOR_Red:
				object->SwapComponentVarient<Material>("red");
				break;
			case COLOR_Green:
				object->SwapComponentVarient<Material>("green");
				break;
			case COLOR_Yellow:
				object->SwapComponentVarient<Material>("yellow");
				break;
			case COLOR_Cyan:
				object->SwapComponentVarient<Material>("cyan");
				break;
		}
	}
}
void CastObject::Destroy() {
	if (object) {
		//MessageEvents::SendQueueMessage(EVENT_Late, [=] { 
			backup->Destroy(); backup = nullptr;// });
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

	void DrawArc(Transform* transform, const DirectX::XMFLOAT3& end, CastObject* arc, const float arcHeight = 1) {
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
			else {
				DirectX::XMFLOAT4X4 endMat = tran.GetMatrix();
				endMat._42 -= 1;
				tran.LookAt(DirectX::XMFLOAT3(endMat.m[3]));
			}
			arc->Get()->GetComponent<Animator>()->SetJointMatrix(i, tran.GetMatrix());
		}
	}
	void DrawRay(Transform* transform, const DirectX::XMFLOAT3& end, CastObject* ray) {
		float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&end), DirectX::XMLoadFloat3(&transform->GetPosition()))));
		DirectX::XMMATRIX scale =  DirectX::XMMatrixScaling(1, 1, dist);
		DirectX::XMFLOAT4X4 controllerMat = transform->GetMatrix();
		controllerMat._41 = 0;
		controllerMat._42 = 0;
		controllerMat._43 = 0;
		DirectX::XMMATRIX rotation = DirectX::XMLoadFloat4x4(&controllerMat);
		DirectX::XMFLOAT4X4 newRotation;
		DirectX::XMStoreFloat4x4(&newRotation, rotation);
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z);
		
		DirectX::XMFLOAT4X4 finalMat;
		DirectX::XMStoreFloat4x4(&finalMat, scale * rotation * translation);
		ray->Get()->transform.SetMatrix(finalMat);
		
		//ray->Get()->transform.SetPosition(transform->GetPosition());
		//ray->Get()->transform.LookAt(end);
		
		/*
		DirectX::XMFLOAT4X4 newMat;
		DirectX::XMStoreFloat4x4(&newMat, DirectX::XMMatrixScaling(1, 1, dist));
		ray->Get()->transform.SetMatrix(newMat);
		ray->Get()->transform.SetPosition(origin);
		ray->Get()->transform.LookAt(end);
		*/
	}
}


bool Raycast(Transform* transform, DirectX::XMFLOAT3& direction, DirectX::XMFLOAT3* colPoint, GameObject ** colObject, CastObject* ray, float maxCastDistance, const char* tag) {
	DirectX::XMFLOAT3 endPoint;
	bool success;
	DirectX::XMFLOAT3 origin = transform->GetPosition();
	if(!colPoint)
		success = PhysicsManager::Raycast(origin, direction, &endPoint, colObject, maxCastDistance, tag);
	else {
		success = PhysicsManager::Raycast(origin, direction, colPoint, colObject, maxCastDistance, tag);
		endPoint = *colPoint;
	}

	if (!success) {
		endPoint.x = origin.x + (direction.x * maxCastDistance);
		endPoint.y = origin.y + (direction.y * maxCastDistance);
		endPoint.z = origin.z + (direction.z * maxCastDistance);
	}

	if (ray && ray->Get()) 
		DrawRay(transform, endPoint, ray);
	return success;
}

bool ArcCast(Transform* transform, DirectX::XMFLOAT3* outPos, CastObject* arc, float maxDistance, float minAngle, float maxAngle, float castHeight, const char* tag) {
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
		Transform tran;
		tran.SetPosition(rayStart);
		if (Raycast(&tran, rayDirection, outPos, nullptr, nullptr, 100, tag)) {
			if (arc && arc->Get()) DrawArc(transform, *outPos, arc);
			return true;
		}
		else
			return false;
	}
}
