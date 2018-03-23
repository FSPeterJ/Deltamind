#include "RigidBody.h"
#include "GhostTime.h"

using namespace DirectX;

RigidBody::RigidBody() {
	hasGavity = false;
	mass = 1.0f;
	terminalSpeed = 100.0f;
	velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	netAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	actingForces.reserve(MAX_APPLIED_FORCE);
	actingForces.push_back(AppliedForce(1.8f, 0.0f, -1.0f, 0.0f, 1.0f, true));
}
RigidBody::RigidBody(bool _hasGravity, float _mass, float veloX, float veloY, float veloZ, float maxSpeed) {
	hasGavity = _hasGravity;
	mass = _mass;
	terminalSpeed = maxSpeed;
	velocity = XMFLOAT3(veloX, veloY, veloZ);
	netAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	actingForces.reserve(MAX_APPLIED_FORCE);
	actingForces.push_back(AppliedForce(9.8f, 0.0f, -1.0f, 0.0f, 1.0f, true));
}
void RigidBody::SetVelocity(float x, float y, float z) {
	velocity = XMFLOAT3(x, y, z);
}
void RigidBody::SetVelocity(XMVECTOR _velocity) {
	XMStoreFloat3(&velocity, _velocity);
}
void RigidBody::SetMass(float _mass) {
	mass = _mass;
}
void RigidBody::GravityOn(bool _hasGravity) {
	hasGavity = _hasGravity;
}
void RigidBody::CalculateNetAccelaration() {
	XMVECTOR net = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	for(unsigned int i = 0; i < actingForces.size(); ++i) {
		if(i == 0 && !hasGavity)
			continue;
		//Need to take mass into consideration
		net += XMVectorScale(XMLoadFloat3(&actingForces[i].direction), actingForces[i].magnitude);
	}
	XMStoreFloat3(&netAcceleration, net);
}
bool RigidBody::AddForce(float _magnitude) {
	if(actingForces.size() < MAX_APPLIED_FORCE) {
		actingForces.push_back(AppliedForce(_magnitude, velocity.x, velocity.y, velocity.z));
		return true;
	}
	return false;
}
bool RigidBody::AddForce(float _magnitude, float x, float y, float z, float _time, bool _isConstant) {
	if(actingForces.size() < MAX_APPLIED_FORCE) {
		if(_isConstant)
			_time = 1.0f;
		actingForces.push_back(AppliedForce(_magnitude, x, y, z, _time, _isConstant));
		return true;
	}
	return false;
}
void RigidBody::AdjustGravityMagnitude(float magnitude) {
	actingForces[0].magnitude = magnitude;
}
void RigidBody::Update(XMMATRIX* _orientation) {
	float delta = (float) GhostTime::DeltaTime();
	unsigned int i = 0;
	while(i < actingForces.size()) {
		if(actingForces[i].timeInAction <= 0.0f) {
			actingForces.erase(actingForces.begin() + i);
		} else
			++i;
	}
	for(i = 0; i < actingForces.size(); ++i) {
		if(!actingForces[i].isConstant) {
			actingForces[i].timeInAction -= delta;
		}
	}
	CalculateNetAccelaration();
	XMVECTOR newVelo = XMLoadFloat3(&velocity) + (XMLoadFloat3(&netAcceleration) * delta);
	XMVECTOR normVelo = XMVector3Normalize(newVelo);
	float newVelLengSq = XMVectorGetX(XMVector3LengthSq(newVelo));
	if (newVelLengSq > FLT_EPSILON) {
		_orientation->r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		_orientation->r[0] = XMVector3Normalize(XMVector3Cross(_orientation->r[1], normVelo));
		_orientation->r[2] = normVelo;
	}

	if (newVelLengSq > terminalSpeed * terminalSpeed)
		newVelo = XMVectorScale(normVelo, terminalSpeed);

	XMStoreFloat3(&velocity, newVelo);
}
XMVECTOR RigidBody::GetVelocity() {
	return XMLoadFloat3(&velocity);
}

float RigidBody::GetSpeedSq() {
	return XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&velocity)));
}

inline RigidBody::AppliedForce::AppliedForce(float _magnitude, float x, float y, float z, float _time, bool _isConstant) :
	isConstant(_isConstant), timeInAction(_time), magnitude(_magnitude) {
	DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(DirectX::XMVectorSet(x, y, z, 0.0f)));
}

void RigidBody::ClearForces() {
	while (actingForces.size() > 1) actingForces.pop_back();
}

void RigidBody::Stop() {
	ClearForces();
	SetVelocity(0.0f, 0.0f, 0.0f);
}

void RigidBody::SetTerminalSpeed(float _speed) {
	terminalSpeed = _speed;
}

void RigidBody::Reset() {
	GravityOn(false);
	SetMass(1.0f);
	SetTerminalSpeed(100.0f);
	SetVelocity(0.0f, 0.0f, 0.0f);
	netAcceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	ClearForces();
}