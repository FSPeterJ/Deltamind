#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "ParticleTextureManager.h"
#include "Pool.h"
#include "ComponentBase.h"
#include "Transform.h"
#include "IComponentManager.h"
#include "Emitter.h"

#define MAX_PARTICLES 100
#define MAX_REFERENCE_PARTICLES 10
class ParticleManager: public IComponentManager {



	struct EmitterConstant {
		//16
		DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0,1,0);
		unsigned MaxParticlesThisFrame = 1;

		//16
		DirectX::XMFLOAT3 Velocity = DirectX::XMFLOAT3(0,0,1);
		float ParticleVelocityVariance;
		//16
		DirectX::XMFLOAT3 ParticlePositionVariance;
		float VelocityMagnatude;  // This might be removed soon

		//16
		float StartSize = 0.5f;
		float EndSize = 1.5f;
		float ParticleLifeSpan = 10;
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

		float size;
		float size1;
		float size2;
		float size3;

		//Constant data (16 bytes)
		DirectX::XMFLOAT4 Color;

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

	ID3D11Buffer* perFrame = nullptr;;

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
	~ParticleManager();
	void Update();
	void InitShaders();
	ParticleManager(ID3D11Device* _device, ID3D11DeviceContext* _context, ID3D11Buffer* _perFrame);
	void RenderParticles();
	ComponentBase* GetReferenceComponent(const char * _FilePath, const char * _data) override;
	ComponentBase* CloneComponent(ComponentBase* reference) override;
	void ResetComponent(ComponentBase* reset) override;
	void ActivateComponent(ComponentBase* component)override;
	void DeactivateComponent(ComponentBase* component) override;
	unsigned AddMaterial(Material* mat);
};

