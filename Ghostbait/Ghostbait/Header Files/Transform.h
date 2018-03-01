#pragma once
#include <DirectXMath.h>

class Transform {
public:
	DirectX::XMFLOAT4X4 matrix = DirectX::XMFLOAT4X4(1, 0, 0, 0,
													 0, 1, 0, 0,
													 0, 0, 1, 0,
													 0, 0, 0, 1);
	//DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0);
	//DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0, 0, 0); //stored in radians
	//DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(0, 0, 0);
	const DirectX::XMFLOAT4X4& GetMatrix() const;
	const DirectX::XMFLOAT3 GetPosition() const;
	void SetMatrix(const DirectX::XMFLOAT4X4& mat);
	void SetPosition(const float x, const float y, const float z);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetRotationDegrees(const float x, const float y, const float z);
	void SetRotationRadians(const float x, const float y, const float z);
	void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3 up = { 0, 1, 0 });
	void TurnTowards(const DirectX::XMFLOAT3& target, const float speed);
	void MoveAlongForward(const float speed);
	void MoveAlongUp(const float speed);
	void MoveAlongSide(const float speed);
	//DirectX::XMFLOAT3 GetRotationValuesFromMatrix(DirectX::XMFLOAT4X4& _mat);
	//void SetTransform(DirectX::XMFLOAT4X4& trans);
	//const DirectX::XMFLOAT4X4& GetTransform() const;
	//
	//void SetPosition(const float x, const float y, const float z);
	//void SetPosition(const DirectX::XMFLOAT3& pos);
	//
	//void SetRotationDegrees(const float x, const float y, const float z);
	//void SetRotationRadians(const float x, const float y, const float z);
	//void RotateDegrees(const float x, const float y, const float z);
	//void RotateRadians(const float x, const float y, const float z);
	//
	//void SetScale(const float x, const float y, const float z);
	//void Scale(const float x, const float y, const float z);
	//
	//void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3 up = { 0, 1, 0 });
	//void TurnTowards(const DirectX::XMFLOAT3& target, const float speed);
	//void moveAlongForward(const float speed);
	//void moveAlongUp(const float speed);
	//void moveAlongSide(const float speed);
};
