#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "ParticleTextureManager.h"

#define MAX_PARTICLESS 100000
class ParticleManager
{
	struct ParticleShaderInfo
	{
		DirectX::XMFLOAT3 pos;
		float size;
		int tex;
	};
	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static ID3D11VertexShader* VertexShader;
	static ID3D11GeometryShader * GeometryShader;
	static ID3D11PixelShader* PixelShader;
	static ID3D11InputLayout* InputLayout;
	static ID3D11Buffer* viewBuff;
	static ID3D11Buffer* projBuff;

	static ParticleTextureManager* texMan;
	static ID3D11Buffer* vertBuff;
	static int pCount;
	static ParticleShaderInfo cpu_side_particles[MAX_PARTICLESS];
public:
	static void Initialize(ID3D11Device* _device, ID3D11DeviceContext* _context, ID3D11VertexShader* _vs, ID3D11GeometryShader* _gs, ID3D11PixelShader* _ps, ID3D11InputLayout* _il);
	static void RenderParticlesTo(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport, DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& proj);
	static void Destroy();
};

