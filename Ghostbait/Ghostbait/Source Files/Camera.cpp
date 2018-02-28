#include "Camera.h"
#include "GhostTime.h"
#include "EngineStructure.h"
#include <DirectXMath.h>
using namespace DirectX;

Camera::Camera() {
	EngineStructure::Update.Add([=]() {
		this->Update();
	});
}

Camera::~Camera() {}

void Camera::setCamera(const DirectX::XMFLOAT4X4 matrixToUse) { SetPosition(matrixToUse); }

void Camera::setCameraPosition(const float x, const float y, const float z) {
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._41 = x;
	newPos._42 = y;
	newPos._43 = z;
	SetPosition(newPos);
}

void Camera::setCameraRotationDegrees(const float x, const float y, const float z) {
	XMFLOAT4X4 rotatedBy;
	XMStoreFloat4x4(&rotatedBy, DirectX::XMMatrixRotationRollPitchYaw(x * XM_PI / 180.0f, y * XM_PI / 180.0f, z * XM_PI / 180.0f));
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._11 = rotatedBy._11;
	newPos._12 = rotatedBy._12;
	newPos._13 = rotatedBy._13;
	newPos._21 = rotatedBy._21;
	newPos._22 = rotatedBy._22;
	newPos._23 = rotatedBy._23;
	newPos._31 = rotatedBy._31;
	newPos._32 = rotatedBy._32;
	newPos._33 = rotatedBy._33;
	SetPosition(newPos);
}

void Camera::setCameraRotationRadians(const float x, const float y, const float z) {
	XMFLOAT4X4 rotatedBy;
	XMStoreFloat4x4(&rotatedBy, XMMatrixRotationRollPitchYaw(x, y, z));
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._11 = rotatedBy._11;
	newPos._12 = rotatedBy._12;
	newPos._13 = rotatedBy._13;
	newPos._21 = rotatedBy._21;
	newPos._22 = rotatedBy._22;
	newPos._23 = rotatedBy._23;
	newPos._31 = rotatedBy._31;
	newPos._32 = rotatedBy._32;
	newPos._33 = rotatedBy._33;
	SetPosition(newPos);
}

void Camera::setCameraScale(const float x, const float y, const float z) {
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._14 = x;
	newPos._24 = y;
	newPos._34 = z;
	SetPosition(newPos);
}

void Camera::pointCameraAt(XMFLOAT3 pos, XMFLOAT3 target, XMFLOAT3 up) {
	DirectX::XMFLOAT4X4 newPos;
	XMStoreFloat4x4(&newPos, XMMatrixIdentity());
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 z;
	XMStoreFloat3(&z, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&pos)));
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&z))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&z), XMLoadFloat3(&x))));
	newPos._11 = x.x;
	newPos._12 = x.y;
	newPos._13 = x.z;
	newPos._41 = pos.x;
	newPos._21 = y.x;
	newPos._22 = y.y;
	newPos._23 = y.z;
	newPos._42 = pos.y;
	newPos._31 = z.x;
	newPos._32 = z.y;
	newPos._33 = z.z;
	newPos._43 = pos.z;
	SetPosition(newPos);
}

void Camera::turnCameraTowards(XMFLOAT3 target, float speed) {
	XMFLOAT3 toTarget;
	XMFLOAT3 camPos;
	camPos.x = GetPosition()._41;
	camPos.y = GetPosition()._42;
	camPos.z = GetPosition()._43;
	XMStoreFloat3(&toTarget, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&camPos)));
	XMFLOAT3 viewX;
	viewX.x = GetPosition()._11;
	viewX.y = GetPosition()._12;
	viewX.z = GetPosition()._13;

	float dotX = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	viewX.x = GetPosition()._21;
	viewX.y = GetPosition()._22;
	viewX.z = GetPosition()._23;
	float dotY = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	DirectX::XMFLOAT4X4 newPos;
	XMStoreFloat4x4(&newPos, XMMatrixRotationRollPitchYaw(-dotY * speed, dotX * speed, 0.0f) * XMLoadFloat4x4(&GetPosition()));
	SetPosition(newPos);
	viewX.x = GetPosition()._11;
	viewX.y = GetPosition()._12;
	viewX.z = GetPosition()._13;

	XMFLOAT3 viewY;
	viewY.x = GetPosition()._21;
	viewY.y = GetPosition()._22;
	viewY.z = GetPosition()._23;

	XMFLOAT3 viewZ;
	viewZ.x = GetPosition()._31;
	viewZ.y = GetPosition()._32;
	viewZ.z = GetPosition()._33;
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&viewZ))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&viewZ), XMLoadFloat3(&x))));
	newPos = GetPosition();
	newPos._11 = x.x;
	newPos._12 = x.y;
	newPos._13 = x.z;
	newPos._21 = y.x;
	newPos._22 = y.y;
	newPos._23 = y.z;
	SetPosition(newPos);
}

void Camera::moveCameraAlongForward(float speed) {
	float dt = (float) GhostTime::DeltaTime();
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._41 += newPos._31 * speed * dt;
	newPos._42 += newPos._32 * speed * dt;
	newPos._43 += newPos._33 * speed * dt;
	SetPosition(newPos);
}

void Camera::moveCameraAlongUp(float speed) {
	float dt = (float) GhostTime::DeltaTime();
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._41 += newPos._21 * speed * dt;
	newPos._42 += newPos._22 * speed * dt;
	newPos._43 += newPos._23 * speed * dt;
	SetPosition(newPos);
}

void Camera::moveCameraAlongSide(float speed) {
	float dt = (float) GhostTime::DeltaTime();
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._41 += newPos._11 * speed * dt;
	newPos._42 += newPos._12 * speed * dt;
	newPos._43 += newPos._13 * speed * dt;
	SetPosition(newPos);
}

void Camera::Update() {
	static float rotationY = 0.0f;
	static float rotationX = 0.0f;
	float dt = (float) GhostTime::DeltaTime();
	if(rotationY < -XM_2PI || rotationY > XM_2PI)
		rotationY = 0.0f;
	if (rotationX < -XM_2PI || rotationX > XM_2PI)
		rotationX = 0.0f;

	if(KeyIsDown(Control::CameraLeftRight)) {
		//position._41 -= 50.0f * dt;
		rotationY += Amount(CameraLeftRight) * dt;
		ResetKey(Control::CameraLeftRight);
		//ResetKey(Control::left);
	}
	if(KeyIsDown(Control::CameraUpDown)) {
		//position._41 += 50.0f * dt;
		rotationX += Amount(CameraUpDown) * dt;
		ResetKey(Control::CameraUpDown);
		//ResetKey(Control::right);
	}
	if(KeyIsDown(Control::forward)) {
		moveCameraAlongForward(10.0f);
		//ResetKey(Control::forward);
	}
	if(KeyIsDown(Control::backward)) {
		moveCameraAlongForward(-10.0f);
		//ResetKey(Control::backward);
	}
	if(KeyIsDown(Control::LeftAction)) {
		moveCameraAlongUp(10.0f);
		//ResetKey(Control::leftAttack);
	}
	if(KeyIsDown(Control::RightAction)) {
		moveCameraAlongUp(-10.0f);
		//ResetKey(Control::rightAttack);
	}

	if(KeyIsDown(Control::left)) {
		moveCameraAlongSide(-10.0f);
		//ResetKey(Control::TestInputZ);
	}
	if(KeyIsDown(Control::right)) {
		moveCameraAlongSide(10.0f);
		//ResetKey(Control::TestInputC);
	}

	setCameraRotationRadians(rotationX, rotationY, 0.0f);

	GameObject::Update();
}
