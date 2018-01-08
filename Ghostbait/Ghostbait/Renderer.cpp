#include "Renderer.h"
#include <math.h>


void Renderer::createDeviceContextAndSwapchain(HWND window)
{
	RECT rect;
	GetWindowRect(window, &rect);
	DXGI_SWAP_CHAIN_DESC desc = DXGI_SWAP_CHAIN_DESC();
	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.Height = rect.bottom - rect.top;
	desc.BufferDesc.Width = rect.right - rect.left;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Flags += DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.OutputWindow = window;
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D_FEATURE_LEVEL * feature = new D3D_FEATURE_LEVEL(D3D_FEATURE_LEVEL_10_1);
	D3D_FEATURE_LEVEL * outputFeature = 0;
#if _DEBUG
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, feature, 1, D3D11_SDK_VERSION, &desc, &swapchain, &device, outputFeature, &context);
#else
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, feature, 1, D3D11_SDK_VERSION, &desc, &swapchain, &device, outputFeature, &context);
#endif
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	delete feature;
}

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(HWND window)
{
}

void Renderer::Destroy()
{
}

void Renderer::initDepthStencilBuffer(pipeline_state_t* pipelineTo)
{
	CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround(pipelineTo->viewport.Width), lround(pipelineTo->viewport.Height), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	device->CreateTexture2D(&depthStencilDesc, NULL, &pipelineTo->depth_stencil_buffer);
}

void Renderer::initDepthStencilState(pipeline_state_t * pipelineTo)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&depthStencilDesc, &pipelineTo->depth_stencil_state);
}

void Renderer::initDepthStencilView(pipeline_state_t * pipelineTo)
{
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	device->CreateDepthStencilView((ID3D11Resource*)pipelineTo->depth_stencil_buffer, &depthStencilViewDesc, &pipelineTo->depth_stencil_view);
}

void Renderer::initRasterState(pipeline_state_t * pipelineTo, bool wireFrame)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	if (wireFrame)
		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	else
		rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	device->CreateRasterizerState(&rasterDesc, &pipelineTo->rasterizer_state);
}

void Renderer::initShaders()
{
}

void Renderer::initViewport(RECT window, pipeline_state_t * pipelineTo)
{
	D3D11_VIEWPORT tempView;
	tempView.Height = (float)window.bottom - (float)window.top;
	tempView.Width = (float)window.right - (float)window.left;
	tempView.MaxDepth = 1.0f;
	tempView.MinDepth = 0.0f;
	tempView.TopLeftX = 0.0f;
	tempView.TopLeftY = 0.0f;

	pipelineTo->viewport = tempView;
}


