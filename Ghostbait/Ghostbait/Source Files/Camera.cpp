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

void Camera::setCamera(const DirectX::XMFLOAT4X4 matrixToUse) { position = matrixToUse; }

void Camera::setCameraPosition(const float x, const float y, const float z) {
	position._41 = x;
	position._42 = y;
	position._43 = z;
}

void Camera::setCameraRotationDegrees(const float x, const float y, const float z) {
	XMFLOAT4X4 rotatedBy;
	XMStoreFloat4x4(&rotatedBy, DirectX::XMMatrixRotationRollPitchYaw(x * XM_PI / 180.0f, y * XM_PI / 180.0f, z * XM_PI / 180.0f));
	position._11 = rotatedBy._11;
	position._12 = rotatedBy._12;
	position._13 = rotatedBy._13;
	position._21 = rotatedBy._21;
	position._22 = rotatedBy._22;
	position._23 = rotatedBy._23;
	position._31 = rotatedBy._31;
	position._32 = rotatedBy._32;
	position._33 = rotatedBy._33;
}

void Camera::setCameraRotationRadians(const float x, const float y, const float z) {
	XMFLOAT4X4 rotatedBy;
	XMStoreFloat4x4(&rotatedBy, XMMatrixRotationRollPitchYaw(x, y, z));
	position._11 = rotatedBy._11;
	position._12 = rotatedBy._12;
	position._13 = rotatedBy._13;
	position._21 = rotatedBy._21;
	position._22 = rotatedBy._22;
	position._23 = rotatedBy._23;
	position._31 = rotatedBy._31;
	position._32 = rotatedBy._32;
	position._33 = rotatedBy._33;
}

void Camera::setCameraScale(const float x, const float y, const float z) {
	position._14 = x;
	position._24 = y;
	position._34 = z;
}

void Camera::pointCameraAt(XMFLOAT3 pos, XMFLOAT3 target, XMFLOAT3 up) {
	XMStoreFloat4x4(&position, XMMatrixIdentity());
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 z;
	XMStoreFloat3(&z, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&pos)));
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&z))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&z), XMLoadFloat3(&x))));
	position._11 = x.x;
	position._12 = x.y;
	position._13 = x.z;
	position._41 = pos.x;
	position._21 = y.x;
	position._22 = y.y;
	position._23 = y.z;
	position._42 = pos.y;
	position._31 = z.x;
	position._32 = z.y;
	position._33 = z.z;
	position._43 = pos.z;
}

void Camera::turnCameraTowards(XMFLOAT3 target, float speed) {
	XMFLOAT3 toTarget;
	XMFLOAT3 camPos;
	camPos.x = position._41;
	camPos.y = position._42;
	camPos.z = position._43;
	XMStoreFloat3(&toTarget, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&camPos)));
	XMFLOAT3 viewX;
	viewX.x = position._11;
	viewX.y = position._12;
	viewX.z = position._13;

	float dotX = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	viewX.x = position._21;
	viewX.y = position._22;
	viewX.z = position._23;
	float dotY = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	XMStoreFloat4x4(&position, XMMatrixRotationRollPitchYaw(-dotY * speed, dotX * speed, 0.0f) * XMLoadFloat4x4(&position));
	viewX.x = position._11;
	viewX.y = position._12;
	viewX.z = position._13;

	XMFLOAT3 viewY;
	viewY.x = position._21;
	viewY.y = position._22;
	viewY.z = position._23;

	XMFLOAT3 viewZ;
	viewZ.x = position._31;
	viewZ.y = position._32;
	viewZ.z = position._33;
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&viewZ))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&viewZ), XMLoadFloat3(&x))));
	position._11 = x.x;
	position._12 = x.y;
	position._13 = x.z;
	position._21 = y.x;
	position._22 = y.y;
	position._23 = y.z;
}

void Camera::moveCameraAlongForward(float speed) {
	float dt = (float) GhostTime::DeltaTime();
	position._41 += position._31 * speed * dt;
	position._42 += position._32 * speed * dt;
	position._43 += position._33 * speed * dt;
}

void Camera::moveCameraAlongUp(float speed) {
	float dt = (float) GhostTime::DeltaTime();
	position._41 += position._21 * speed * dt;
	position._42 += position._22 * speed * dt;
	position._43 += position._23 * speed * dt;
}

void Camera::moveCameraAlongSide(float speed) {
	float dt = (float) GhostTime::DeltaTime();
	position._41 += position._11 * speed * dt;
	position._42 += position._12 * speed * dt;
	position._43 += position._13 * speed * dt;
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
