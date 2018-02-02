#pragma once
#include <vector>
#define MAX_APPLIED_FORCE 5
#include <DirectXMath.h>//todo get rid

class RigidBody {
	struct AppliedForce {
		bool isConstant = false;
		float timeInAction = 0.0f;
		float magnitude = 0.0f;
		DirectX::XMFLOAT3 direction;

		AppliedForce() {};
		AppliedForce(float _magnitude, float x, float y, float z, float _time = 1.0f, bool _isConstant = false);;
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
	void SetVelocity(DirectX::XMVECTOR _velocity);
	void SetMass(float _mass);
	void GravityOn(bool _hasGravity);
	void AdjustGravityMagnitude(float magnitude);
	float GetSpeedSq();

	//bool operator==(const RigidBody other) {
	//	if (hasGavity != other.hasGavity) return false;
	//	if (mass != other.mass) return false;
	//	if (velocity.x != other.velocity.x) return false;
	//	if (velocity.y != other.velocity.y) return false;
	//	if (velocity.z != other.velocity.z) return false;
	//	if (netAcceleration.x != other.netAcceleration.x) return false;
	//	if (netAcceleration.y != other.netAcceleration.y) return false;
	//	if (netAcceleration.z != other.netAcceleration.z) return false;
	//	if (actingForces != other.actingForces) return false;
	//	return true;
	//}

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

	void Update();
	DirectX::XMVECTOR GetVelocity();
};
