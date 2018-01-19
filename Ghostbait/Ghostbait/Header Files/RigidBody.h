#pragma once
#include <DirectXMath.h>
#include <vector>
#define MAX_APPLIED_FORCE 5

class RigidBody {
	struct AppliedForce {
		bool isConstant = false;
		float timeInAction = 0.0f;
		float magnitude = 0.0f;
		DirectX::XMFLOAT3 direction;

		AppliedForce() {};
		AppliedForce(float _magnitude, float x, float y, float z, float _time = 1.0f, bool _isConstant = false) :
			isConstant(_isConstant), timeInAction(_time), magnitude(_magnitude) {
			XMStoreFloat3(&direction, XMVector3Normalize(XMVectorSet(x, y, z, 0.0f)));
		};
	};

	bool hasGavity;
	float mass;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 netAcceleration;
	std::vector<AppliedForce> actingForces;

public:
	RigidBody();
	RigidBody(bool _hasGravity, float _mass, float veloX, float veloY, float veloZ);
	void SetVelocity(float x, float y, float z);
	void SetMass(float _mass);
	void AdjustGravityMagnitude(float magnitude);
	void CalculateNetAccelaration();
	bool AddForce(float _magnitude);
	bool AddForce(float _magnitude, float x, float y, float z, float _time = 0.01f, bool _isConstant = false);
	void Update(float _delta);
};
