#pragma once
#include "Transform.h"
#include "ComponentBase.h"


struct Material;

enum ParticleProps {
	HASGRAVITY = 1
};


struct EmitterData {
	//16
	DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0, 1, 0);
	float EmissionRateInterval = 0.1f;

	//16
	DirectX::XMFLOAT3 Velocity = DirectX::XMFLOAT3(0, 0, 5);
	float ParticleVelocityVariance = 0;
	//16
	DirectX::XMFLOAT3 ParticlePositionVariance;
	float VelocityMagnatude;  // This might be removed soon

							  //16
	float StartSize = 0.1f;
	float EndSize = 0.1f;
	float ParticleLifeSpan = 3;
	unsigned TextureIndex;
	//16
	DirectX::XMFLOAT4 StartColorA = DirectX::XMFLOAT4(0, 0, 1, 0.8f);
	DirectX::XMFLOAT4 StartColorB = DirectX::XMFLOAT4(0, 0, 1, 0.8f);
	//16
	DirectX::XMFLOAT4 EndColorA = DirectX::XMFLOAT4(1, 0, 0, 1);
	DirectX::XMFLOAT4 EndColorB = DirectX::XMFLOAT4(1, 0, 0, 1);
	//16
	float rotationVarience;
	unsigned properties = 0;
	float xAngleVariance = 1;
	float yAngleVariance = 1;
	//16
	float EmissionOverflow = 0;
	unsigned EmissionsPerInterval = 10;
	float mass = 1;
	unsigned perInterval = 10;

	DirectX::XMFLOAT3 force;
	float velocityLossFactor;

	DirectX::XMFLOAT3 Gravity;
	float Unused;


};



class Emitter: public InstantiatedCompBase {
	unsigned updateID = 0;
	float carryoverTick = 0;
	float previousDT = 0;
	float age = 0;
public:
	float previousOverflow = 0;
	float lifespan = 4;
	bool enabled = false;
	Material* materials[4] = { 0 };
	Transform transform;
	EmitterData mainData;
	void AddMaterial(Material* mat);

	void Update();
	void Enable();
	void Disable();
	void Destroy();
};


