#pragma once
#include <d3d11.h>
#include <VertexTypes.h>
namespace DirectX { struct XMFLOAT3;  struct XMFLOAT4X4; }
struct ColliderData;

#define MAX_VERTS 8192
class DebugRenderer {
	static ID3D11Device* device;
	static ID3D11DeviceContext* context;
	static ID3D11Buffer * vertBuff;
	static ID3D11Buffer* modelBuffer;
	static int v_count;
	static int tri_count;
	static VertexPositionColor cpu_side_buffer[MAX_VERTS];
	static VertexPositionColor cpu_side_triangles[MAX_VERTS];
	static ID3D11VertexShader* vShader;
	static ID3D11PixelShader* pShader;
	static ID3D11InputLayout* iLayout;
	static ID3D11RasterizerState* defaultState;
	static ID3D11RasterizerState* wireframeState;

	static void AddTri(VertexPositionColor* toMessWith, VertexPositionColor v1, VertexPositionColor v2, VertexPositionColor v3, int triIndex);
public:
	//Both Initialize and Destroy are called by Renderer
	static void Initialize(ID3D11Device* deviceIn, ID3D11DeviceContext* contextIn, ID3D11Buffer* modelBufferIn, ID3D11VertexShader* vIn, ID3D11PixelShader* pIn, ID3D11InputLayout* layoutIn, ID3D11RasterizerState* stateIn);
	static void Destroy();

	static void AddLine(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 color);
	static void AddBox(ColliderData* cIn, DirectX::XMFLOAT3 color);
	static void AddBox(DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max, DirectX::XMFLOAT3& color);
	static void AddSphere(DirectX::XMFLOAT3 pos, float radius, DirectX::XMFLOAT3 color);
	static void DrawAxes(DirectX::XMFLOAT4X4 toDraw, float length);
	static void drawTo(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport);
	static void flushTo(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT& viewport);
};
