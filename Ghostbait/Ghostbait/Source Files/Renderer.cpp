#include "Renderer.h"
#include <math.h>
#include <fstream>
#include <VertexTypes.h>
#include "DebugRenderer.h"

using namespace DirectX;

void Renderer::createDeviceContextAndSwapchain(Window window) {
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
	D3D_FEATURE_LEVEL * outputFeature = nullptr;
#if _DEBUG
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, feature, 1, D3D11_SDK_VERSION, &desc, &swapchain, &device, outputFeature, &context);
#else
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, feature, 1, D3D11_SDK_VERSION, &desc, &swapchain, &device, outputFeature, &context);
#endif
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

	delete feature;
}

void Renderer::clearPipelineMemory(pipeline_state_t * pipeline) {
	pipeline->depth_stencil_buffer->Release();
	pipeline->depth_stencil_state->Release();
	pipeline->depth_stencil_view->Release();
	pipeline->rasterizer_state->Release();
}

void Renderer::clearTextureMemory(renderTargetInfo * info) {
	info->depthBuffer->Release();
	info->dsv->Release();
	info->rtv->Release();
	info->texture->Release();
}

bool Renderer::LoadShaderFromCSO(char ** szByteCode, size_t & szByteCodeSize, const char * szFileName) {
	std::ifstream load;
	load.open(szFileName, std::ios_base::binary);
	if(!load.is_open()) return false;
	load.seekg(0, std::ios_base::end);
	szByteCodeSize = size_t(load.tellg());
	*szByteCode = new char[szByteCodeSize];
	load.seekg(0, std::ios_base::beg);
	load.read(*szByteCode, szByteCodeSize);
	load.close();
	return true;
}

void Renderer::setupVRTargets() {
	leftEye.renderInfo.viewport = D3D11_VIEWPORT();
	leftEye.renderInfo.viewport.Height = (float)VRManagement->RecommendedRenderHeight;
	leftEye.renderInfo.viewport.Width = (float)VRManagement->RecommendedRenderWidth;
	leftEye.renderInfo.viewport.MaxDepth = 1.0f;
	leftEye.renderInfo.viewport.MinDepth = 0.0f;
	leftEye.renderInfo.viewport.TopLeftX = 0.0f;
	leftEye.renderInfo.viewport.TopLeftY = 0.0f;

	rightEye.renderInfo.viewport = D3D11_VIEWPORT();
	rightEye.renderInfo.viewport.Height = (float)VRManagement->RecommendedRenderHeight;
	rightEye.renderInfo.viewport.Width = (float)VRManagement->RecommendedRenderWidth;
	rightEye.renderInfo.viewport.MaxDepth = 1.0f;
	rightEye.renderInfo.viewport.MinDepth = 0.0f;
	rightEye.renderInfo.viewport.TopLeftX = 0.0f;
	rightEye.renderInfo.viewport.TopLeftY = 0.0f;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Height = VRManagement->RecommendedRenderHeight;
	texDesc.Width = VRManagement->RecommendedRenderWidth;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	texDesc.ArraySize = 1;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc = sampleDesc;

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture2D.MostDetailedMip = 0;

	device->CreateTexture2D(&texDesc, nullptr, &leftEye.renderInfo.texture);
	device->CreateRenderTargetView(leftEye.renderInfo.texture, nullptr, &leftEye.renderInfo.rtv); //TODO: leftEye.renderInfo.texture can be null, this goes against SAL annotation

	device->CreateTexture2D(&texDesc, nullptr, &rightEye.renderInfo.texture);
	device->CreateRenderTargetView(rightEye.renderInfo.texture, nullptr, &rightEye.renderInfo.rtv); //TODO: rightEye.renderInfo.texture can be null, this goes against SAL annotation

	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.MiscFlags = NULL;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.Texture2D.MipSlice = 0;
	depthStencilDesc.Flags = 0;

	device->CreateTexture2D(&texDesc, nullptr, &leftEye.renderInfo.depthBuffer);
	device->CreateDepthStencilView(leftEye.renderInfo.depthBuffer, &depthStencilDesc, &leftEye.renderInfo.dsv); //TODO: leftEye.renderInfo.depthBuffer could be null, this goes against SAL annotation

	device->CreateTexture2D(&texDesc, nullptr, &rightEye.renderInfo.depthBuffer);
	device->CreateDepthStencilView(rightEye.renderInfo.depthBuffer, &depthStencilDesc, &rightEye.renderInfo.dsv); //TODO: rightEye.renderInfo.depthBuffer could be null, this goes against SAL annotation
}

void Renderer::renderObjectDefaultState(Object * obj) {
	UINT stride = sizeof(VertexPositionTextureNormalAnim);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &obj->GetComponent<Mesh>()->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(obj->GetComponent<Mesh>()->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->UpdateSubresource(modelBuffer, 0, NULL, &XMMatrixTranspose(XMLoadFloat4x4(&obj->position)), 0, 0);
	obj->GetComponent<Material>()->bindToShader(context, factorBuffer);
	//materialManagement->GetElement(UINT_MAX)->bindToShader(context, factorBuffer);
	context->DrawIndexed(obj->GetComponent<Mesh>()->indexCount, 0, 0);
}

void Renderer::renderToEye(eye * eyeTo) {
	float color[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	context->ClearRenderTargetView(eyeTo->renderInfo.rtv, color);
	context->ClearDepthStencilView(eyeTo->renderInfo.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv);
	context->RSSetViewports(1, &eyeTo->renderInfo.viewport);
	context->UpdateSubresource(cameraBuffer, 0, NULL, &eyeTo->camera, 0, 0);

	for(size_t i = 0; i < renderedObjects.size(); ++i) {
		renderObjectDefaultState((Object*)renderedObjects[i]);
	}

#if _DEBUG
	DebugRenderer::drawTo(eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv, eyeTo->renderInfo.viewport);
	context->VSSetShader(StandardVertexShader, NULL, NULL);
	context->PSSetShader(StandardPixelShader, NULL, NULL);
	context->IASetInputLayout(ILStandard);
#endif
}

void Renderer::loadPipelineState(pipeline_state_t * pipeline) {
	context->RSSetState(pipeline->rasterizer_state);
	context->OMSetRenderTargets(1, &pipeline->render_target_view, pipeline->depth_stencil_view);
	context->OMSetDepthStencilState(pipeline->depth_stencil_state, NULL);
	context->RSSetViewports(1, &pipeline->viewport);
	context->IASetInputLayout(pipeline->input_layout);
	context->VSSetShader(pipeline->vertex_shader, NULL, NULL);
	context->PSSetShader(pipeline->pixel_shader, NULL, NULL);
}

DirectX::XMFLOAT4X4 Renderer::lookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up) {
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

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::Initialize(Window window, VRManager * vr) {
	VRManagement = vr;

	createDeviceContextAndSwapchain(window);
	RECT windRect;
	GetWindowRect(window.GetOutputWindow(), &windRect);
	initViewport(windRect, &defaultPipeline);
	initDepthStencilBuffer(&defaultPipeline);
	initDepthStencilState(&defaultPipeline);
	initDepthStencilView(&defaultPipeline);
	initRasterState(&defaultPipeline);
	initShaders();
	defaultPipeline.vertex_shader = StandardVertexShader;
	defaultPipeline.pixel_shader = StandardPixelShader;
	defaultPipeline.input_layout = ILStandard;
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device->CreateRenderTargetView(backBuffer, NULL, &defaultPipeline.render_target_view);
	loadPipelineState(&defaultPipeline);
	meshManagement = new MeshManager();
	meshManagement->Initialize(device);
	materialManagement = new MaterialManager();
	materialManagement->Initialize(device, context);
	context->VSSetConstantBuffers(0, 1, &cameraBuffer);
	context->VSSetConstantBuffers(1, 1, &modelBuffer);
	context->PSSetConstantBuffers(0, 1, &lightBuffer);
	context->PSSetConstantBuffers(1, 1, &factorBuffer);
#pragma region SamplerState
	D3D11_SAMPLER_DESC sampleDesc;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampleDesc.MinLOD = -3.402823466e+38F;
	sampleDesc.MaxLOD = 3.402823466e+38F;
	device->CreateSamplerState(&sampleDesc, &OnlySamplerState);
	context->PSSetSamplers(0, 1, &OnlySamplerState);
#pragma endregion

	addDirectionalLight(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -0.5f, 0.5f));
	addPointLight(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 15.0f), 15.0f);
	addSpotLight(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -0.5f, 0.5f), 1.0f, 0.9f);
	keyboardCamera = new Camera();
	keyboardCamera->pointCameraAt(DirectX::XMFLOAT3(0.0f, 2.0f, -15.0f), DirectX::XMFLOAT3(0.0f, -2.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	XMStoreFloat4x4(&defaultCamera.projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(60.0f * XM_PI / 180.0f, defaultPipeline.viewport.Width / defaultPipeline.viewport.Height, 0.001f, 300.0f)));

	if(VRManagement)
		setupVRTargets();

	MessageEvents::Subscribe(EVENT_Instantiated, [this](EventMessageBase * _e) {this->registerObject(_e); });
	MessageEvents::Subscribe(EVENT_Destroy, [this](EventMessageBase * _e) {this->unregisterObject(_e); });
	MessageEvents::Subscribe(EVENT_Unrender, [this](EventMessageBase * _e) {this->unregisterObject(_e); });
	MessageEvents::Subscribe(EVENT_Addrender, [this](EventMessageBase * _e) {this->registerObject(_e); });

#if _DEBUG
	DebugRenderer::Initialize(device, context, modelBuffer, PassThroughPositionColorVS, PassThroughPS, ILPositionColor, defaultPipeline.rasterizer_state);
#endif
}

void Renderer::Destroy() {
	OnlySamplerState->Release();
	cameraBuffer->Release();
	modelBuffer->Release();
	factorBuffer->Release();
	lightBuffer->Release();
	ILPositionColor->Release();
	ILStandard->Release();
	PassThroughPositionColorVS->Release();
	PassThroughPS->Release();
	StandardVertexShader->Release();
	StandardPixelShader->Release();
	backBuffer->Release();
	swapchain->Release();
	context->Release();
	device->Release();
	defaultPipeline.render_target_view->Release();
	clearPipelineMemory(&defaultPipeline);
	if(VRManagement) {
		clearTextureMemory(&leftEye.renderInfo);
		clearTextureMemory(&rightEye.renderInfo);
	}
	meshManagement->Destroy();
	delete meshManagement;
	materialManagement->Destroy();
	delete materialManagement;
	delete keyboardCamera;
#if _DEBUG
	DebugRenderer::Destroy();
#endif
}

void Renderer::registerObject(const Object * toRegister, renderState specialInstructions) {
	switch(specialInstructions) {
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

void Renderer::registerObject(EventMessageBase* e) {
	//TODO: Need logic to determine which objects group to push to
	//based off of what the object has - may include instructions in the future
	NewObjectMessage* instantiate = (NewObjectMessage*)e;
	if(instantiate->RetrieveObject()->GetComponent<Mesh>())
	{
		renderedObjects.push_back(instantiate->RetrieveObject());
	}
}

void Renderer::unregisterObject(EventMessageBase* e) {
	DestroyMessage* removeobjMessage = (DestroyMessage*)e;
	//TODO: Need logic for which register it is under
	for(std::vector<const Object*>::iterator iter = renderedObjects.begin(); iter != renderedObjects.end(); ++iter) {
		if(*iter == removeobjMessage->RetrieveObject()) {
			renderedObjects.erase(iter);
			return;
		}
	}
}

bool Renderer::unregisterObject(const Object * toRemove, renderState specialInstructions) {
	switch(specialInstructions) {
		case RENDER_STATE_DEFAULT:
		{
			for(std::vector<const Object*>::iterator iter = renderedObjects.begin(); iter != renderedObjects.end(); ++iter) {
				if(*iter == toRemove) {
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

void Renderer::addDirectionalLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir)
{
	genericLight toManager;
	toManager.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);
	toManager.dir = dir;
	lightManager.addLight(toManager);
}

void Renderer::addPointLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, float radius)
{
	genericLight toManager;
	toManager.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);
	toManager.pos = pos;
	toManager.radius = radius;
	lightManager.addLight(toManager);
}

void Renderer::addSpotLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float radius, float outerRadius)
{
	genericLight toManager;
	toManager.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f);
	toManager.pos = pos;
	toManager.dir = dir;
	toManager.radius = radius;
	toManager.outerRadius = outerRadius;
	lightManager.addLight(toManager);
}

XMFLOAT4X4 FloatArrayToFloat4x4(float* arr) {
	XMFLOAT4X4 mat;
	mat._11 = arr[0];
	mat._12 = arr[1];
	mat._13 = arr[2];
	mat._14 = arr[3];
	mat._21 = arr[4];
	mat._22 = arr[5];
	mat._23 = arr[6];
	mat._24 = arr[7];
	mat._31 = arr[8];
	mat._32 = arr[9];
	mat._33 = arr[10];
	mat._34 = arr[11];
	mat._41 = arr[12];
	mat._42 = arr[13];
	mat._43 = arr[14];
	mat._44 = arr[15];
	return mat;
}

void Renderer::Render() {
	loadPipelineState(&defaultPipeline);
	context->UpdateSubresource(lightBuffer, NULL, NULL, lightManager.getLightBuffer(), 0, 0);
	XMMATRIX cameraObj = XMMatrixTranspose(XMLoadFloat4x4(&keyboardCamera->position));
	XMStoreFloat4x4(&defaultCamera.view, XMMatrixInverse(&XMMatrixDeterminant(cameraObj), cameraObj));
	if(VRManagement) {
		VRManagement->GetVRMatrices(&leftEye.camera.projection, &rightEye.camera.projection, &leftEye.camera.view, &rightEye.camera.view);

		//XMStoreFloat4x4(&leftEye.camera.projection, (XMLoadFloat4x4(&defaultCamera.projection)));
		//XMStoreFloat4x4(&rightEye.camera.projection, (XMLoadFloat4x4(&defaultCamera.projection)));

		XMStoreFloat4x4(&leftEye.camera.projection, XMMatrixTranspose(XMLoadFloat4x4(&leftEye.camera.projection)));
		XMStoreFloat4x4(&rightEye.camera.projection, XMMatrixTranspose(XMLoadFloat4x4(&rightEye.camera.projection)));
		XMStoreFloat4x4(&leftEye.camera.view, XMMatrixTranspose(XMMatrixInverse(&XMVectorSet(0, 0, 0, 0), XMLoadFloat4x4(&leftEye.camera.view))));
		XMStoreFloat4x4(&rightEye.camera.view, XMMatrixTranspose(XMMatrixInverse(&XMVectorSet(0, 0, 0, 0), XMLoadFloat4x4(&rightEye.camera.view))));

		renderToEye(&leftEye);
		renderToEye(&rightEye);
		VRManagement->SendToHMD((void*)leftEye.renderInfo.texture, (void*)rightEye.renderInfo.texture);
		context->UpdateSubresource(cameraBuffer, 0, NULL, &(leftEye.camera), 0, 0);
	}
	else context->UpdateSubresource(cameraBuffer, 0, NULL, &defaultCamera, 0, 0);
	float color[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	context->ClearRenderTargetView(defaultPipeline.render_target_view, color);
	context->ClearDepthStencilView(defaultPipeline.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &defaultPipeline.render_target_view, defaultPipeline.depth_stencil_view);
	context->RSSetViewports(1, &defaultPipeline.viewport);

	for(size_t i = 0; i < renderedObjects.size(); ++i) {
		renderObjectDefaultState((Object*)renderedObjects[i]);
	}

#if _DEBUG
	DebugRenderer::flushTo(defaultPipeline.render_target_view, defaultPipeline.depth_stencil_view, defaultPipeline.viewport);
#endif
	swapchain->Present(0, 0);
}

void Renderer::initDepthStencilBuffer(pipeline_state_t* pipelineTo) {
	CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround(pipelineTo->viewport.Width), lround(pipelineTo->viewport.Height), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	device->CreateTexture2D(&depthStencilDesc, NULL, &pipelineTo->depth_stencil_buffer);
}

void Renderer::initDepthStencilState(pipeline_state_t * pipelineTo) {
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

void Renderer::initDepthStencilView(pipeline_state_t * pipelineTo) {
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	device->CreateDepthStencilView((ID3D11Resource*)pipelineTo->depth_stencil_buffer, &depthStencilViewDesc, &pipelineTo->depth_stencil_view);
}

void Renderer::initRasterState(pipeline_state_t * pipelineTo, bool wireFrame) {
	D3D11_RASTERIZER_DESC rasterDesc;
	if(wireFrame)
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

void Renderer::initShaders() {
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

	LoadShaderFromCSO(&byteCode, byteCodeSize, "StandardVertexShader.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &StandardVertexShader);

	D3D11_INPUT_ELEMENT_DESC standardVSDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(standardVSDesc, ARRAYSIZE(standardVSDesc), byteCode, byteCodeSize, &ILStandard);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "StandardPixelShader.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &StandardPixelShader);
	delete[] byteCode;
	byteCode = nullptr;

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(viewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&constantBufferDesc, nullptr, &cameraBuffer);

	CD3D11_BUFFER_DESC modelBufferDesc(sizeof(DirectX::XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&modelBufferDesc, nullptr, &modelBuffer);

	CD3D11_BUFFER_DESC factorBufferDesc(sizeof(Material::factorBufferStructure), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&factorBufferDesc, nullptr, &factorBuffer);

	CD3D11_BUFFER_DESC dirBufferDesc(sizeof(lightBufferStruct), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&dirBufferDesc, nullptr, &lightBuffer);
}

void Renderer::initViewport(const RECT window, pipeline_state_t * pipelineTo) {
	D3D11_VIEWPORT tempView;
	tempView.Height = (float)window.bottom - (float)window.top;
	tempView.Width = (float)window.right - (float)window.left;
	tempView.MaxDepth = 1.0f;
	tempView.MinDepth = 0.0f;
	tempView.TopLeftX = 0.0f;
	tempView.TopLeftY = 0.0f;

	pipelineTo->viewport = tempView;
}
