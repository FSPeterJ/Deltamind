#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "ParticleTextureManager.h"
#include "Pool.h"
#include "ComponentBase.h"
#include "Transform.h"
#include "IComponentManager.h"
#include "Emitter.h"

#define MAX_PARTICLES 50
#define MAX_REFERENCE_PARTICLES 50
class ParticleManager: public IComponentManager {



	struct EmitterConstant {
		//16
		DirectX::XMFLOAT3 Position;
		unsigned MaxParticlesThisFrame;

		//16
		DirectX::XMFLOAT3 Velocity;
		float ParticleVelocityVariance;
		//16
		DirectX::XMFLOAT3 ParticlePositionVariance;
		float VelocityMagnatude;  // This might be removed soon

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
		unsigned padding[2];

		//16
	} emitterConstant;

	//float3 float float3 float float float float uint float4 float4
	struct GPUParticle {
		//Dynamic data  (ID3D11Device * _device, ID3D11DeviceContext * _context16 bytes)
		DirectX::XMFLOAT3 position;
		float rotation;

		//Dynamic data  (16 bytes)
		DirectX::XMFLOAT3 velocity;
		float age;

		//Constant data (16 bytes)
		float lifespan; // how long the particle will last
		float startSize; //particle scale over time data
		float endSize;
		unsigned properties; // 12 bits - U Axis UV end | 12 bits - V Axis UV end | 8 bits - texture W index

		//Constant data (16 bytes)
		DirectX::XMFLOAT4 StartColor;

		//Constant data (16 bytes)
		DirectX::XMFLOAT4 EndColor;
	};

	Pool<Emitter> emitterPool = Pool<Emitter>(MAX_PARTICLES);
	Pool<Emitter> referenceEemitterPool = Pool<Emitter>(MAX_REFERENCE_PARTICLES);

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11GeometryShader * GeometryShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;


	ID3D11Texture2D* randomTexture = nullptr;
	ID3D11ShaderResourceView* randomTextureSRV = nullptr;

	ID3D11Buffer* ParticleBuffer = nullptr;
	ID3D11ShaderResourceView* ParticleSRV = nullptr;
	ID3D11UnorderedAccessView* ParticleUAV = nullptr;
	ID3D11Buffer* InactiveParticleIndexBuffer = nullptr;
	ID3D11UnorderedAccessView* InactiveParticleIndexUAV = nullptr;
	ID3D11Buffer* InactiveParticleConstantBuffer = nullptr;
	ID3D11Buffer* ActiveParticleConstantBuffer = nullptr;
	ID3D11Buffer* EmitterConstantBuffer = nullptr;
	ID3D11Buffer* ActiveParticleIndexBuffer = nullptr;
	ID3D11ShaderResourceView* ActiveParticleIndexSRV = nullptr;
	ID3D11UnorderedAccessView* ActiveParticleIndexUAV = nullptr;
	ID3D11ShaderResourceView* InactiveParticleIndexSRV = nullptr;
	ID3D11Buffer* IndirectDrawArgsBuffer = nullptr;
	ID3D11UnorderedAccessView* IndirectDrawArgsUAV = nullptr;
	ID3D11ComputeShader* ParticleUpdateShader = nullptr;
	ID3D11ComputeShader* ParticleEmitShader = nullptr;

	//static ID3D11Buffer* viewBuff;
	//static ID3D11Buffer* projBuff;

	ParticleTextureManager* texMan;
public:
	ParticleManager(ID3D11Device* _device, ID3D11DeviceContext* _context);
	~ParticleManager();
	void Update();
	void InitShaders();
	void RenderParticles();
	ComponentBase* GetReferenceComponent(const char * _FilePath, const char * _data) override;
	ComponentBase* CloneComponent(ComponentBase* reference) override;
	void ResetComponent(ComponentBase* reset) override;
	void ActivateComponent(ComponentBase* component)override;
	void DeactivateComponent(ComponentBase* component) override;
	unsigned AddMaterial(Material* mat);
};

