#include "Camera.h"

Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::setCameraPosition(const float x, const float y, const float z)
{
	transform._41 = x;
	transform._42 = y;
	transform._43 = z;
}

void Camera::setCameraRotationDegrees(const float x, const float y, const float z)
{
	DirectX::XMFLOAT4X4 rotatedBy;
	DirectX::XMStoreFloat4x4(&rotatedBy, DirectX::XMMatrixRotationRollPitchYaw(x * DirectX::XM_PI / 180.0f, y * DirectX::XM_PI / 180.0f, z * DirectX::XM_PI / 180.0f));
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

void Camera::setCameraRotationRadians(const float x, const float y, const float z)
{
	DirectX::XMFLOAT4X4 rotatedBy;
	DirectX::XMStoreFloat4x4(&rotatedBy, DirectX::XMMatrixRotationRollPitchYaw(x, y, z));
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

void Camera::setCameraScale(const float x, const float y, const float z)
{
	transform._14 = x;
	transform._24 = y;
	transform._34 = z;
}
