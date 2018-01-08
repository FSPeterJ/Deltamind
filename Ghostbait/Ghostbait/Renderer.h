#pragma once

#include <d3d11.h>
#include "MeshManager.h"

class Renderer
{
private:
	struct pipeline_state_t
	{
		ID3D11InputLayout* 	input_layout;
		ID3D11VertexShader* vertex_shader;
		ID3D11PixelShader* pixel_shader;
		ID3D11RenderTargetView* render_target_view;
		ID3D11Texture2D* depth_stencil_buffer;
		ID3D11DepthStencilState* depth_stencil_state;
		ID3D11DepthStencilView* depth_stencil_view;
		ID3D11RasterizerState* rasterizer_state;
		D3D11_VIEWPORT viewport;
	};

	struct viewProjectionConstantBuffer
	{
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;
	ID3D11Texture2D* backBuffer;

	ID3D11VertexShader* PassThroughPositionColorVS;
	ID3D11PixelShader* PassThroughPS;

	ID3D11InputLayout* ILPositionColor;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* modelBuffer;
	pipeline_state_t defaultPipeline;

	void initDepthStencilBuffer(pipeline_state_t* pipelineTo);
	void initDepthStencilState(pipeline_state_t* pipelineTo);
	void initDepthStencilView(pipeline_state_t* pipelineTo);
	void initRasterState(pipeline_state_t* pipelineTo, bool wireFrame = false);
	void initShaders();
	void initViewport(RECT window, pipeline_state_t* pipelineTo);
	void createDeviceContextAndSwapchain(HWND window);
	bool LoadShaderFromCSO(char ** szByteCode, size_t& szByteCodeSize, const char* szFileName);

public:
	Renderer();
	~Renderer();

	void Initialize(HWND window);
	void Destroy();
};

