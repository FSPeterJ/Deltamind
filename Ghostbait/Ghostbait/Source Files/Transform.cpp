#include "Transform.h"
#include "GhostTime.h"
using namespace DirectX;
/*
DirectX::XMFLOAT3 Transform::GetRotationValuesFromMatrix(DirectX::XMFLOAT4X4& _mat) {
	DirectX::XMFLOAT3 result;
	DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&_mat);
	DirectX::XMMATRIX rot = mat;
	DirectX::XMFLOAT4X4 rot_f;

	DirectX::XMFLOAT4 pos;
	DirectX::XMStoreFloat4(&pos, mat.r[0]);
	rot.r[0] = DirectX::XMVectorSet(pos.x / scale.x, pos.y / scale.y, pos.z / scale.z, 0);

	XMStoreFloat4(&pos, mat.r[1]);
	rot.r[1] = DirectX::XMVectorSet(pos.x / scale.x, pos.y / scale.y, pos.z / scale.z, 0);

	XMStoreFloat4(&pos, mat.r[2]);
	rot.r[2] = DirectX::XMVectorSet(pos.x / scale.x, pos.y / scale.y, pos.z / scale.z, 0);

	XMStoreFloat4(&pos, mat.r[3]);
	rot.r[3] = DirectX::XMVectorSet(0, 0, 0, pos.w);

	DirectX::XMStoreFloat4x4(&rot_f, rot);

	result.x = (float)atan2((double)rot_f._32, (double)rot_f._33);
	result.y = (float)(-asin((double)rot_f._31));
	result.z = (float)atan2((double)rot_f._21, (double)rot_f._11);
	if (result.x < 0)
		result.x += DirectX::XM_2PI;
	if (result.y < 0)
		result.y += DirectX::XM_2PI;
	if (result.x < 0)
		result.x += DirectX::XM_2PI;
	return result;
}

void Transform::SetPosition(const float x, const float y, const float z) {
	matrix._41 = x;
	matrix._42 = y;
	matrix._43 = z;
}
void Transform::SetPosition(const DirectX::XMFLOAT3& pos) {
	SetPosition(pos.x, pos.y, pos.z);
}


void Transform::SetRotationRadians(const float x, const float y, const float z) {
	//Recreate a new matrix with our old scale and translation, but a new rotation. Set it to our matrix
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationRollPitchYaw(x, y, z) * DirectX::XMMatrixTranslation(position.x, position.y, position.z));
}
void Transform::SetRotationDegrees(const float x, const float y, const float z) {
	SetRotationRadians(x * DirectX::XM_PI / 180.0f, y * DirectX::XM_PI / 180.0f, z * DirectX::XM_PI / 180.0f);
}

void Transform::SetScale(const float x, const float y, const float z) {
	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixScaling(x, y, z) * DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * DirectX::XMMatrixTranslation(position.x, position.y, position.z));
	scale = { x, y, z };
}
void Transform::Scale(const float x, const float y, const float z) {
	SetScale(scale.x * x, scale.y * y, scale.z * z);
}


void Transform::LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3 up) {
	DirectX::XMFLOAT4X4 newPos;
	DirectX::XMStoreFloat4x4(&newPos, DirectX::XMMatrixIdentity());
	DirectX::XMFLOAT3 newX;
	DirectX::XMFLOAT3 newY;
	DirectX::XMFLOAT3 newZ;
	DirectX::XMStoreFloat3(&newZ, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&target), DirectX::XMLoadFloat3(&position))));
	DirectX::XMStoreFloat3(&newX, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat3(&newZ))));
	DirectX::XMStoreFloat3(&newY, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&newZ), DirectX::XMLoadFloat3(&newX))));
	newPos._11 = newX.x;
	newPos._12 = newX.y;
	newPos._13 = newX.z;
	newPos._41 = position.x;
	newPos._21 = newY.x;
	newPos._22 = newY.y;
	newPos._23 = newY.z;
	newPos._42 = position.y;
	newPos._31 = newZ.x;
	newPos._32 = newZ.y;
	newPos._33 = newZ.z;
	newPos._43 = position.z;
	SetPosition(newPos);
}
void Transform::turnCameraTowards(XMFLOAT3 target, float speed) {
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
void Transform::moveCameraAlongForward(float speed) {
	float dt = (float)GhostTime::DeltaTime();
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._41 += newPos._31 * speed * dt;
	newPos._42 += newPos._32 * speed * dt;
	newPos._43 += newPos._33 * speed * dt;
	SetPosition(newPos);
}
void Transform::moveCameraAlongUp(float speed) {
	float dt = (float)GhostTime::DeltaTime();
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._41 += newPos._21 * speed * dt;
	newPos._42 += newPos._22 * speed * dt;
	newPos._43 += newPos._23 * speed * dt;
	SetPosition(newPos);
}
void Transform::moveCameraAlongSide(float speed) {
	float dt = (float)GhostTime::DeltaTime();
	DirectX::XMFLOAT4X4 newPos = GetPosition();
	newPos._41 += newPos._11 * speed * dt;
	newPos._42 += newPos._12 * speed * dt;
	newPos._43 += newPos._13 * speed * dt;
	SetPosition(newPos);
}
*/

const DirectX::XMFLOAT4X4& Transform::GetMatrix() const {
	return matrix;
}
void Transform::SetMatrix(const DirectX::XMFLOAT4X4& mat) {
	matrix = mat;
}

const DirectX::XMFLOAT3 Transform::GetPosition() const {
	return XMFLOAT3(matrix._41, matrix._42, matrix._43);
}
void Transform::SetPosition(const float x, const float y, const float z) {
	matrix._41 = x;
	matrix._42 = y;
	matrix._43 = z;
}
void Transform::SetPosition(const DirectX::XMFLOAT3& pos) {
	SetPosition(pos.x, pos.y, pos.z);
}
void Transform::MoveToOrigin(float height) {
	SetPosition(0, height, 0);
}

void Transform::SetRotationRadians(const float x, const float y, const float z) {
	XMFLOAT4X4 rotatedBy;
	XMStoreFloat4x4(&rotatedBy, XMMatrixRotationRollPitchYaw(x, y, z));
	matrix._11 = rotatedBy._11;
	matrix._12 = rotatedBy._12;
	matrix._13 = rotatedBy._13;
	matrix._21 = rotatedBy._21;
	matrix._22 = rotatedBy._22;
	matrix._23 = rotatedBy._23;
	matrix._31 = rotatedBy._31;
	matrix._32 = rotatedBy._32;
	matrix._33 = rotatedBy._33;
}
void Transform::SetRotationDegrees(const float x, const float y, const float z) {
	SetRotationRadians(x * XM_PI / 180.0f, y * XM_PI / 180.0f, z * XM_PI / 180.0f);
}
void Transform::LookAt(const XMFLOAT3& target, const XMFLOAT3 up) {
	DirectX::XMFLOAT3 pos = { matrix._41, matrix._42, matrix._43 };
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 z;
	XMStoreFloat3(&z, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&pos)));
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&z))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&z), XMLoadFloat3(&x))));
	matrix._11 = x.x;
	matrix._12 = x.y;
	matrix._13 = x.z;
	matrix._41 = pos.x;
	matrix._21 = y.x;
	matrix._22 = y.y;
	matrix._23 = y.z;
	matrix._42 = pos.y;
	matrix._31 = z.x;
	matrix._32 = z.y;
	matrix._33 = z.z;
	matrix._43 = pos.z;
}
void Transform::TurnTowards(const XMFLOAT3& target, const float speed) {
	XMFLOAT3 toTarget;
	XMFLOAT3 camPos;
	camPos.x = matrix._41;
	camPos.y = matrix._42;
	camPos.z = matrix._43;
	XMStoreFloat3(&toTarget, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&camPos)));
	XMFLOAT3 viewX;
	viewX.x = matrix._11;
	viewX.y = matrix._12;
	viewX.z = matrix._13;

	float dotX = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	viewX.x = matrix._21;
	viewX.y = matrix._22;
	viewX.z = matrix._23;
	float dotY = XMVector3Dot(XMLoadFloat3(&viewX), XMLoadFloat3(&toTarget)).m128_f32[0];

	XMStoreFloat4x4(&matrix, XMMatrixRotationRollPitchYaw(-dotY * speed, dotX * speed, 0.0f) * XMLoadFloat4x4(&matrix));

	viewX.x = matrix._11;
	viewX.y = matrix._12;
	viewX.z = matrix._13;

	XMFLOAT3 viewY;
	viewY.x = matrix._21;
	viewY.y = matrix._22;
	viewY.z = matrix._23;

	XMFLOAT3 viewZ;
	viewZ.x = matrix._31;
	viewZ.y = matrix._32;
	viewZ.z = matrix._33;
	XMFLOAT3 x;
	XMFLOAT3 y;
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&viewZ))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&viewZ), XMLoadFloat3(&x))));
	matrix._11 = x.x;
	matrix._12 = x.y;
	matrix._13 = x.z;
	matrix._21 = y.x;
	matrix._22 = y.y;
	matrix._23 = y.z;
}
void Transform::MoveAlongForward(const float speed) {
	float dt = (float)GhostTime::DeltaTime();
	matrix._41 += matrix._31 * speed * dt;
	matrix._42 += matrix._32 * speed * dt;
	matrix._43 += matrix._33 * speed * dt;
}
void Transform::MoveAlongUp(const float speed) {
	float dt = (float)GhostTime::DeltaTime();
	matrix._41 += matrix._21 * speed * dt;
	matrix._42 += matrix._22 * speed * dt;
	matrix._43 += matrix._23 * speed * dt;
}
void Transform::MoveAlongSide(const float speed) {
	float dt = (float)GhostTime::DeltaTime();
	matrix._41 += matrix._11 * speed * dt;
	matrix._42 += matrix._12 * speed * dt;
	matrix._43 += matrix._13 * speed * dt;
}