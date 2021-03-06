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
	float terminalSpeed;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 netAcceleration;
	std::vector<AppliedForce> actingForces;

	void CalculateNetAccelaration();

public:
	RigidBody();
	RigidBody(bool _hasGravity, float _mass, float veloX, float veloY, float veloZ, float maxSpeed = 100.0f);
	void SetVelocity(float x, float y, float z);
	void SetVelocity(DirectX::XMVECTOR _velocity);
	void SetVelocity(DirectX::XMFLOAT3& _velocity);
	void SetMass(float _mass);
	void SetTerminalSpeed(float _speed);
	void GravityOn(bool _hasGravity);
	void AdjustGravityMagnitude(float magnitude);
	float GetSpeedSq();
	void Reset();
	

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
	/// Adds force in the current forward Z diretion </summary>
	/// <returns>Returns FALSE if force cannot be added on this rigid body</returns>
	bool AddForce(float _magnitude);

	/// <summary>
	/// Adds force in the direction of (x,y,z)
	/// with _time being the length at which force is applied to the rigid body
	/// If accerleration is constant, set time to 1.0f
	/// </summary>
	/// <returns>Returns FALSE if force cannot be added on this rigid body</returns>
	bool AddForce(float _magnitude, float x, float y, float z, float _time = 0.01f, bool _isConstant = false);

	/// <summary>
	/// Removes all acting forces except gravity </summary>
	void ClearForces();

	/// <summary>
	/// Sets velocity to zero and clear all forces except gravity </summary>
	void Stop();

	void Update(DirectX::XMMATRIX* _orietation = nullptr);
	DirectX::XMVECTOR GetVelocity();
	DirectX::XMFLOAT3 GetVelocityFloat3();
};
