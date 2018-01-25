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
			DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(DirectX::XMVectorSet(x, y, z, 0.0f)));
		};
	};

	bool hasGavity;
	float mass;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 netAcceleration;
	std::vector<AppliedForce> actingForces;

	void CalculateNetAccelaration();

public:
	RigidBody();
	RigidBody(bool _hasGravity, float _mass, float veloX, float veloY, float veloZ);
	void SetVelocity(float x, float y, float z);
	void SetMass(float _mass);
	void GravityOn(bool _hasGravity);
	void AdjustGravityMagnitude(float magnitude);

	/// <summary>
	/// Adds acceleration in the current forward Z diretion </summary>
	/// <returns>Returns FALSE if force cannot be added on this rigid body</returns>
	bool AddForce(float _magnitude);

	/// <summary>
	/// Adds acceleration in the direction of (x,y,z)
	/// with _time being the length at which acceleration is applied to the rigid body 
	/// If accerleration is constant, set time to 1.0f
	/// </summary>
	/// <returns>Returns FALSE if force cannot be added on this rigid body</returns>
	bool AddForce(float _magnitude, float x, float y, float z, float _time = 0.01f, bool _isConstant = false);

	void Update(float _delta);
	DirectX::XMVECTOR GetVelocity();
};
