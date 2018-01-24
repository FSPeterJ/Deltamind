#include "Camera.h"

using namespace DirectX;

Camera::Camera() {}

Camera::~Camera() {}

void Camera::setCameraPosition(const float x, const float y, const float z) {
	transform._41 = x;
	transform._42 = y;
	transform._43 = z;
}

void Camera::setCameraRotationDegrees(const float x, const float y, const float z) {
	XMFLOAT4X4 rotatedBy;
	XMStoreFloat4x4(&rotatedBy, DirectX::XMMatrixRotationRollPitchYaw(x * XM_PI / 180.0f, y * XM_PI / 180.0f, z * XM_PI / 180.0f));
	transform._11 = rotatedBy._11;
	transform._12 = rotatedBy._12;
	transform._13 = rotatedBy._13;
	transform._21 = rotatedBy._21;
	transform._22 = rotatedBy._22;
	transform._23 = rotatedBy._23;
	transform._31 = rotatedBy._31;
	transform._32 = rotatedBy._32;
	transform._33 = rotatedBy._33;
}

void Camera::setCameraRotationRadians(const float x, const float y, const float z) {
	XMFLOAT4X4 rotatedBy;
	XMStoreFloat4x4(&rotatedBy, XMMatrixRotationRollPitchYaw(x, y, z));
	transform._11 = rotatedBy._11;
	transform._12 = rotatedBy._12;
	transform._13 = rotatedBy._13;
	transform._21 = rotatedBy._21;
	transform._22 = rotatedBy._22;
	transform._23 = rotatedBy._23;
	transform._31 = rotatedBy._31;
	transform._32 = rotatedBy._32;
	transform._33 = rotatedBy._33;
}

void Camera::setCameraScale(const float x, const float y, const float z) {
	transform._14 = x;
	transform._24 = y;
	transform._34 = z;
}

void Camera::pointCameraAt(XMFLOAT3 pos, XMFLOAT3 target, XMFLOAT3 up) {
	XMStoreFloat4x4(&transform, XMMatrixIdentity());
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 z;
	XMStoreFloat3(&z, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&pos)));
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&z))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&z), XMLoadFloat3(&x))));
	transform._11 = x.x;
	transform._12 = x.y;
	transform._13 = x.z;
	transform._41 = pos.x;
	transform._21 = y.x;
	transform._22 = y.y;
	transform._23 = y.z;
	transform._42 = pos.y;
	transform._31 = z.x;
	transform._32 = z.y;
	transform._33 = z.z;
	transform._43 = pos.z;
}

void Camera::turnCameraTowards(XMFLOAT3 target, float speed) {
	XMFLOAT3 toTarget;
	XMFLOAT3 camPos;
	camPos.x = transform._41;
	camPos.y = transform._42;
	camPos.z = transform._43;
	XMStoreFloat3(&toTarget, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&camPos)));
	XMFLOAT3 viewX;
	viewX.x = transform._11;
	viewX.y = transform._12;
	viewX.z = transform._13;

	float dotX = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	viewX.x = transform._21;
	viewX.y = transform._22;
	viewX.z = transform._23;
	float dotY = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	XMStoreFloat4x4(&transform, XMMatrixRotationRollPitchYaw(-dotY * speed, dotX * speed, 0.0f) * XMLoadFloat4x4(&transform));
	viewX.x = transform._11;
	viewX.y = transform._12;
	viewX.z = transform._13;

	XMFLOAT3 viewY;
	viewY.x = transform._21;
	viewY.y = transform._22;
	viewY.z = transform._23;

	XMFLOAT3 viewZ;
	viewZ.x = transform._31;
	viewZ.y = transform._32;
	viewZ.z = transform._33;
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&viewZ))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&viewZ), XMLoadFloat3(&x))));
	transform._11 = x.x;
	transform._12 = x.y;
	transform._13 = x.z;
	transform._21 = y.x;
	transform._22 = y.y;
	transform._23 = y.z;
}
