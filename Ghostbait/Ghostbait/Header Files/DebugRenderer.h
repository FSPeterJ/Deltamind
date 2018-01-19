#pragma once
#include <d3d11.h>
#include <VertexTypes.h>

#define MAX_VERTS 2048
class DebugRenderer
{
	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static ID3D11Buffer * vertBuff;
	static ID3D11Buffer* modelBuffer;
	static int v_count;
	static VertexPositionColor cpu_side_buffer[MAX_VERTS];
	static ID3D11VertexShader* vShader;
	static ID3D11PixelShader* pShader;
	static ID3D11InputLayout* iLayout;
public:
	//Both Initialize and Destroy are called by Renderer
	static void Initialize(ID3D11Device* deviceIn, ID3D11DeviceContext* contextIn, ID3D11Buffer* modelBufferIn, ID3D11VertexShader* vIn, ID3D11PixelShader* pIn, ID3D11InputLayout* layoutIn);
	static void Destroy();

	static void AddLine(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 color);
	static void drawTo(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport);
	static void flushTo(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport);
};

