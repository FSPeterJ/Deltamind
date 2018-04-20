#pragma once
#include "Transform.h"
#include "ComponentBase.h"


struct Material;




struct EmitterData {
	//16
	DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0, 1, 0);
	float emissionIntervalSec = 0.1f;

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
	DirectX::XMFLOAT4 StartColor = DirectX::XMFLOAT4(0, 0, 1, 0.8f);
	//16
	DirectX::XMFLOAT4 EndColor = DirectX::XMFLOAT4(1, 0, 0, 1);
	//16
	float rotationVarience;
	unsigned properties;
	float xAngleVariance;
	float yAngleVariance;
	//16
	float emissionOverflow = 0;
	float padding[3];
};



class Emitter: public InstantiatedCompBase {
	unsigned updateID = 0;
	float carryoverTick = 0;
	float previousDT = 0;
	float previousOverflow = 0;
public:
	Material* materials[4] = { 0 };
	Transform transform;
	EmitterData mainData;
	void AddMaterial(Material* mat);

	void Update();
	void Enable();
	void Disable();
};


