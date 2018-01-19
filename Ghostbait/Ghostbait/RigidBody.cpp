#include "RigidBody.h"
using namespace DirectX;

RigidBody::RigidBody() {
	hasGavity = false;
	mass = 1.0f;
	velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	netAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	actingForces.reserve(MAX_APPLIED_FORCE);
	actingForces.push_back(AppliedForce(9.8f, 0.0f, -1.0f, 0.0f, 1.0f, true));
}
RigidBody::RigidBody(bool _hasGravity, float _mass, float veloX, float veloY, float veloZ) {
	hasGavity = _hasGravity;
	mass = _mass;
	velocity = XMFLOAT3(veloX, veloY, veloZ);
	netAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	actingForces.reserve(MAX_APPLIED_FORCE);
	actingForces.push_back(AppliedForce(9.8f, 0.0f, -1.0f, 0.0f, 1.0f, true));
}
void RigidBody::SetVelocity(float x, float y, float z) {
	velocity = XMFLOAT3(x, y, z);
}
void RigidBody::SetMass(float _mass) {
	mass = _mass;
}
void RigidBody::CalculateNetAccelaration() {
	XMVECTOR net = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < MAX_APPLIED_FORCE; ++i) {
		if (i == 0 && !hasGavity)
			continue;
		//Need to take mass into consideration
		net += XMVectorScale(XMLoadFloat3(&actingForces[i].direction), actingForces[i].magnitude);
	}
	XMStoreFloat3(&netAcceleration, net);
}
bool RigidBody::AddForce(float _magnitude) {
	if (actingForces.size() < MAX_APPLIED_FORCE) {
		actingForces.push_back(AppliedForce(_magnitude, velocity.x, velocity.y, velocity.z));
		return true;
	}
	return false;
}
bool RigidBody::AddForce(float _magnitude, float x, float y, float z, float _time = 1.0f, bool _isConstant = false) {
	if (actingForces.size() < MAX_APPLIED_FORCE) {
		actingForces.push_back(AppliedForce(_magnitude, x, y, z, _time, _isConstant));
		return true;
	}
	return false;
}
void RigidBody::AdjustGravityMagnitude(float magnitude) {
	actingForces[0].magnitude = magnitude;
}
void RigidBody::Update(float _delta) {
	unsigned int i = 0;
	while (i < actingForces.size()) {
		if (actingForces[i].timeInAction <= 0.0f) {
			actingForces.erase(actingForces.begin() + i);
		}
		else
			++i;
	}
	for (unsigned int i = 0; i < actingForces.size(); ++i) {
		if (!actingForces[i].isConstant) {
			actingForces[i].timeInAction -= _delta;
		}
	}

	CalculateNetAccelaration();

	XMStoreFloat3(&velocity, XMLoadFloat3(&velocity) + (XMLoadFloat3(&netAcceleration) * _delta));
}
