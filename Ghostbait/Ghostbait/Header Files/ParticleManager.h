#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "ParticleTextureManager.h"
#include "Pool.h"
#include "ComponentBase.h"
#include "Transform.h"
#include "IComponentManager.h"
#include "Emitter.h"
#include "MessageStructs.h"

//#define MAX_PARTICLES 524288 // 2^19
#define MAX_PARTICLES 2000 // 2^4
#define MAX_REFERENCE_PARTICLES 100
class ParticleManager: public IComponentManager {

	struct EmitterConstant {
		//16
		DirectX::XMFLOAT3 Position = DirectX::XMFLOAT3(0, 1, 0);
		float emissionIntervalMS = 10;

		//16
		DirectX::XMFLOAT3 Velocity = DirectX::XMFLOAT3(0, 0, 1);
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
		float EmissionCount;
		float mass;
		unsigned Per;

		DirectX::XMFLOAT3 acceleration;
		float velocityLossFactor;

		DirectX::XMFLOAT3 Gravity;
		float Unused;


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

		DirectX::XMFLOAT3 acceleration;
		float velocityLossFactor;

		//Constant data (16 bytes)
		DirectX::XMFLOAT3 Gravity;
		float Unused;
	};

	struct SortParameters {
		int distance;
		int jump;
		int direction;
		int padding = 0;
	} sortParams;

	struct SortIndex {
		float distanceToCamera;
		float index;
	};

	Pool<Emitter> emitterPool = Pool<Emitter>(MAX_PARTICLES);
	Pool<Emitter> referenceEmitterPool = Pool<Emitter>(MAX_REFERENCE_PARTICLES);

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
	ID3D11Buffer* InactiveParticleConstantBuffer = nullptr;
	ID3D11Buffer* ActiveParticleConstantBuffer = nullptr;
	ID3D11Buffer* EmitterConstantBuffer = nullptr;
	ID3D11Buffer* InactiveParticleIndexBuffer = nullptr;
	ID3D11UnorderedAccessView* InactiveParticleIndexUAV = nullptr;
	ID3D11Buffer* SortParticleIndexBuffer = nullptr;
	ID3D11ShaderResourceView* SortParticleIndexSRV = nullptr;
	ID3D11UnorderedAccessView* SortParticleIndexUAV = nullptr;
	ID3D11Buffer* ActiveParticleIndexBuffer = nullptr;
	ID3D11ShaderResourceView* ActiveParticleIndexSRV = nullptr;
	ID3D11UnorderedAccessView* ActiveParticleIndexUAV = nullptr;
	ID3D11ShaderResourceView* InactiveParticleIndexSRV = nullptr;
	ID3D11Buffer* IndirectDrawArgsBuffer = nullptr;
	ID3D11UnorderedAccessView* IndirectDrawArgsUAV = nullptr;
	ID3D11ComputeShader* ParticleUpdateShader = nullptr;
	ID3D11ComputeShader* ParticleEmitShader = nullptr;
	ID3D11ComputeShader* ParticleSortInitialShader = nullptr;
	ID3D11ComputeShader* ParticleSortStepShader = nullptr;
	ID3D11ComputeShader* ParticleSortFinalShader = nullptr;
	ID3D11ComputeShader* ParticleIndirectArgsInitShader = nullptr;


	ID3D11Buffer* SortParametersConstantBuffer = nullptr;
	ID3D11Buffer* DispatchIndirectArgsBuffer = nullptr;
	ID3D11UnorderedAccessView* IndirectSortArgsBufferUAV = nullptr;



	//static ID3D11Buffer* viewBuff;
	//static ID3D11Buffer* projBuff;

	ParticleTextureManager* texMan;
public:
	~ParticleManager();
	void Update();
	void Sort() const;
	bool sortInitial() const;
	bool sortIncremental(unsigned presorted) const;
	void NewEmitter(EventMessageBase* _e);
	void InitEmitters();
	void InitShaders();
	ParticleManager(ID3D11Device* _device, ID3D11DeviceContext* _context, ID3D11Buffer* _perFrame, ID3D11ShaderResourceView* randomTexture);
	void RenderParticles();
	ComponentBase* GetReferenceComponent(const char * _FilePath, const char * _data) override;
	ComponentBase* CloneComponent(ComponentBase* reference) override;
	void ResetComponent(ComponentBase* reset) override;
	void ActivateComponent(ComponentBase* component)override;
	void DeactivateComponent(ComponentBase* component) override;
	unsigned AddMaterial(Material* mat);
};

