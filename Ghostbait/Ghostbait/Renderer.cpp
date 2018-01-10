#include "Renderer.h"
#include <math.h>
#include <fstream>

void Renderer::createDeviceContextAndSwapchain(Window window)
{
	RECT rect;
	GetWindowRect(window.GetOutputWindow(), &rect);
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
	desc.OutputWindow = window.GetOutputWindow();
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

void Renderer::clearPipelineMemory(pipeline_state_t * pipeline)
{
	pipeline->depth_stencil_buffer->Release();
	pipeline->depth_stencil_state->Release();
	pipeline->depth_stencil_view->Release();
	pipeline->rasterizer_state->Release();
}

bool Renderer::LoadShaderFromCSO(char ** szByteCode, size_t & szByteCodeSize, const char * szFileName)
{
	std::ifstream load;
	load.open(szFileName, std::ios_base::binary);
	if (!load.is_open()) return false;
	load.seekg(0, std::ios_base::end);
	szByteCodeSize = size_t(load.tellg());
	*szByteCode = new char[szByteCodeSize];
	load.seekg(0, std::ios_base::beg);
	load.read(*szByteCode, szByteCodeSize);
	load.close();
	return true;
}

void Renderer::renderObjectDefaultState(const Object * obj)
{
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &meshManagement->findMesh(UINT_MAX)->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(meshManagement->findMesh(UINT_MAX)->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->UpdateSubresource(modelBuffer, 0, NULL, &XMMatrixTranspose(obj->object), 0, 0);

	context->DrawIndexed(meshManagement->findMesh(UINT_MAX)->indexCount, 0, 0);
}

void Renderer::loadPipelineState(pipeline_state_t * pipeline)
{
	context->RSSetState(pipeline->rasterizer_state);
	context->OMSetRenderTargets(1, &pipeline->render_target_view, pipeline->depth_stencil_view);
	context->OMSetDepthStencilState(pipeline->depth_stencil_state, NULL);
	context->RSSetViewports(1, &pipeline->viewport);
	context->IASetInputLayout(pipeline->input_layout);
	context->VSSetShader(pipeline->vertex_shader, NULL, NULL);
	context->PSSetShader(pipeline->pixel_shader, NULL, NULL);
}

DirectX::XMFLOAT4X4 Renderer::lookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up)
{
	DirectX::XMFLOAT4X4 ret;
	DirectX::XMStoreFloat4x4(&ret, XMMatrixIdentity());
	DirectX::XMFLOAT3 x;
	DirectX::XMFLOAT3 y;
	DirectX::XMFLOAT3 z;
	XMStoreFloat3(&z, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&pos)));
	XMStoreFloat3(&x, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&z))));
	XMStoreFloat3(&y, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&z), XMLoadFloat3(&x))));
	ret._11 = x.x;
	ret._12 = x.y;
	ret._13 = x.z;
	ret._41 = pos.x;
	ret._21 = y.x;
	ret._22 = y.y;
	ret._23 = y.z;
	ret._42 = pos.y;
	ret._31 = z.x;
	ret._32 = z.y;
	ret._33 = z.z;
	ret._43 = pos.z;
	//XMStoreFloat4x4(&ret, XMMatrixTranslation(pos.x, pos.y, pos.z) * XMLoadFloat4x4(&ret));
	return ret;
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Initialize(Window window)
{
	createDeviceContextAndSwapchain(window);
	RECT windRect;
	GetWindowRect(window.GetOutputWindow(), &windRect);
	initViewport(windRect, &defaultPipeline);
	initDepthStencilBuffer(&defaultPipeline);
	initDepthStencilState(&defaultPipeline);
	initDepthStencilView(&defaultPipeline);
	initRasterState(&defaultPipeline);
	initShaders();
	defaultPipeline.vertex_shader = PassThroughPositionColorVS;
	defaultPipeline.pixel_shader = PassThroughPS;
	defaultPipeline.input_layout = ILPositionColor;
	device->CreateRenderTargetView(backBuffer, NULL, &defaultPipeline.render_target_view);
	loadPipelineState(&defaultPipeline);
	meshManagement = new MeshManager();
	meshManagement->Initialize(device);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetConstantBuffers(0, 1, &cameraBuffer);
	context->VSSetConstantBuffers(1, 1, &modelBuffer);

	XMMATRIX camTemp = XMMatrixTranspose(XMLoadFloat4x4(&lookAt(XMFLOAT3(0.0f, 2.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f))));
	XMStoreFloat4x4(&defaultCamera.view, XMMatrixInverse(&XMMatrixDeterminant(camTemp), camTemp));
	XMStoreFloat4x4(&defaultCamera.projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(60.0f * XM_PI / 180.0f, defaultPipeline.viewport.Width / defaultPipeline.viewport.Height, 0.001f, 300.0f)));
}

void Renderer::Destroy()
{
	cameraBuffer->Release();
	modelBuffer->Release();
	ILPositionColor->Release();
	PassThroughPositionColorVS->Release();
	PassThroughPS->Release();
	backBuffer->Release();
	swapchain->Release();
	context->Release();
	device->Release();
	defaultPipeline.render_target_view->Release();
	clearPipelineMemory(&defaultPipeline);

	meshManagement->Destroy();
	delete meshManagement;
}

void Renderer::registerObject(const Object * toRegister, renderState specialInstructions)
{
	switch (specialInstructions)
	{
	case RENDER_STATE_DEFAULT:
	{
		renderedObjects.push_back(toRegister);
	}
	break;
	case RENDER_STATE_TRANSPARENT:
	{

	}
	break;
	}
}

bool Renderer::unregisterObject(const Object * toRemove, renderState specialInstructions)
{
	switch (specialInstructions)
	{
	case RENDER_STATE_DEFAULT:
	{
		for (std::vector<const Object*>::iterator iter = renderedObjects.begin(); iter != renderedObjects.end(); ++iter)
		{
			if (*iter == toRemove)
			{
				renderedObjects.erase(iter);
				return true;
			}
		}
	}
	break;
	case RENDER_STATE_TRANSPARENT:
	{

	}
	break;
	}
	return false;
}

void Renderer::Render()
{
	
	float color[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	context->ClearRenderTargetView(defaultPipeline.render_target_view, color);
	context->ClearDepthStencilView(defaultPipeline.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->UpdateSubresource(cameraBuffer, 0, NULL, &defaultCamera, 0, 0);
	for (size_t i = 0; i < renderedObjects.size(); ++i)
	{
		renderObjectDefaultState(renderedObjects[i]);
	}

	swapchain->Present(0, 0);
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
	char* byteCode = nullptr;
	size_t byteCodeSize;
	LoadShaderFromCSO(&byteCode, byteCodeSize, "PassThroughPositionColorVS.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &PassThroughPositionColorVS);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), byteCode, byteCodeSize, &ILPositionColor);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "PassThroughPS.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &PassThroughPS);
	delete[] byteCode;
	byteCode = nullptr;

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(viewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&constantBufferDesc, nullptr, &cameraBuffer);

	CD3D11_BUFFER_DESC modelBufferDesc(sizeof(DirectX::XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&modelBufferDesc, nullptr, &modelBuffer);
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


