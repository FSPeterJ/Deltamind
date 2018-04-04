#pragma once
#include "Transform.h"
#include "ComponentBase.h"


struct Material;

class Emitter: public InstantiatedCompBase {
public:
	Material* Materials[4];
	float xAngle;
	float yAngle;

	float colorStart[4];
	float colorEnd[4];

	//Dynamic Data
	Transform transform;

	//16
	DirectX::XMFLOAT3 Position;
	unsigned MaxParticlesThisFrame;

	//16
	DirectX::XMFLOAT3 Velocity;
	float VelocityMagnatude;

	//16
	DirectX::XMFLOAT3 ParticlePositionVariance;
	float ParticleVelocityVariance;

	//16
	float StartSize;
	float EndSize;
	float ParticleLifeSpan;
	unsigned TextureIndex;

	//16
	DirectX::XMFLOAT4 StartColor;
	//16
	DirectX::XMFLOAT4 EndColor;
	//16
	float rotationVarience;
	unsigned properties;
	//
	//

	void AddMaterial(Material* mat);
};


