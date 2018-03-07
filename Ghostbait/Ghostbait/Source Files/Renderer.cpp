#include "Renderer.h"
#include <fstream>
#include "DebugRenderer.h"
#include "MessageStructs.h"
#include "MessageEvents.h"
#include "MeshManager.h"
#include "VRManager.h"
#include "MaterialManager.h"
#include "Transform.h"
#include "Animator.h"
#include "LightManager.h"
#include "ParticleManager.h"
#include "GameObject.h"
#include "AnimatorStructs.h"
#include "WICTextureLoader.h"

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
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer);

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
	leftEye.renderInfo.viewport.Height = (float)VRManager::GetInstance().RecommendedRenderHeight;
	leftEye.renderInfo.viewport.Width = (float)VRManager::GetInstance().RecommendedRenderWidth;
	leftEye.renderInfo.viewport.MaxDepth = 1.0f;
	leftEye.renderInfo.viewport.MinDepth = 0.0f;
	leftEye.renderInfo.viewport.TopLeftX = 0.0f;
	leftEye.renderInfo.viewport.TopLeftY = 0.0f;

	rightEye.renderInfo.viewport = D3D11_VIEWPORT();
	rightEye.renderInfo.viewport.Height = (float)VRManager::GetInstance().RecommendedRenderHeight;
	rightEye.renderInfo.viewport.Width = (float)VRManager::GetInstance().RecommendedRenderWidth;
	rightEye.renderInfo.viewport.MaxDepth = 1.0f;
	rightEye.renderInfo.viewport.MinDepth = 0.0f;
	rightEye.renderInfo.viewport.TopLeftX = 0.0f;
	rightEye.renderInfo.viewport.TopLeftY = 0.0f;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Height = VRManager::GetInstance().RecommendedRenderHeight;
	texDesc.Width = VRManager::GetInstance().RecommendedRenderWidth;
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
	if(leftEye.renderInfo.texture)
		device->CreateRenderTargetView(leftEye.renderInfo.texture, nullptr, &leftEye.renderInfo.rtv);

	device->CreateTexture2D(&texDesc, nullptr, &rightEye.renderInfo.texture);
	if(rightEye.renderInfo.texture)
		device->CreateRenderTargetView(rightEye.renderInfo.texture, nullptr, &rightEye.renderInfo.rtv);

	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.MiscFlags = NULL;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.Texture2D.MipSlice = 0;
	depthStencilDesc.Flags = 0;

	device->CreateTexture2D(&texDesc, nullptr, &leftEye.renderInfo.depthBuffer);
	if(leftEye.renderInfo.depthBuffer)
		device->CreateDepthStencilView(leftEye.renderInfo.depthBuffer, &depthStencilDesc, &leftEye.renderInfo.dsv);

	device->CreateTexture2D(&texDesc, nullptr, &rightEye.renderInfo.depthBuffer);
	if(rightEye.renderInfo.depthBuffer)
		device->CreateDepthStencilView(rightEye.renderInfo.depthBuffer, &depthStencilDesc, &rightEye.renderInfo.dsv);
}

void Renderer::renderObjectDefaultState(Object * obj) {
	UINT stride = sizeof(VertexPositionTextureNormalAnim);
	//UINT stride = 0;
	UINT offset = 0;
	Mesh* y= obj->GetComponent<Mesh>();
	ID3D11Buffer* x = y->vertexBuffer;

	context->IASetVertexBuffers(0, 1, &x, &stride, &offset);
	context->IASetIndexBuffer(obj->GetComponent<Mesh>()->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->UpdateSubresource(modelBuffer, 0, NULL, &XMMatrixTranspose(XMLoadFloat4x4(&obj->transform.GetMatrix())), 0, 0);
	Material* mat = obj->GetComponent<Material>();
	if (mat)
		obj->GetComponent<Material>()->bindToShader(context, factorBuffer);
	else
		materialManagement->GetNullMaterial()->bindToShader(context, factorBuffer);
	Animator* anim = obj->GetComponent<Animator>();
	if(anim) {
		const std::vector<animJoint>* joints = anim->getTweens();
		const std::vector<animJoint>* bindPose = &anim->getCurrentAnimation()->bPose->joints;
		size_t test = joints->size();
		for(size_t i = 0; i < joints->size(); ++i) {
			DirectX::XMStoreFloat4x4(&cpuAnimationData.cpu_side_joints[i], XMLoadFloat4x4(&bindPose->operator[](i).transform) * XMLoadFloat4x4(&joints->operator[](i).transform));
		}
		cpuAnimationData.willAnimate = true;
	} else
		cpuAnimationData.willAnimate = false;
	context->UpdateSubresource(animDataBuffer, 0, NULL, &cpuAnimationData, 0, 0);
	//materialManagement->GetElement(UINT_MAX)->bindToShader(context, factorBuffer);
	context->DrawIndexed(obj->GetComponent<Mesh>()->indexCount, 0, 0);
}

void Renderer::renderToEye(eye * eyeTo) {
	float color[] = {0.5f, 0.5f, 1.0f, 1.0f};
	context->ClearRenderTargetView(eyeTo->renderInfo.rtv, color);
	context->ClearDepthStencilView(eyeTo->renderInfo.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->UpdateSubresource(cameraBuffer, 0, NULL, &eyeTo->camera, 0, 0);
	drawSkyboxTo(eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv, eyeTo->renderInfo.viewport, eyeTo->camPos);
	context->OMSetRenderTargets(1, &eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv);
	context->RSSetViewports(1, &eyeTo->renderInfo.viewport);

	for(size_t i = 0; i < renderedObjects.size(); ++i) {
		renderObjectDefaultState((Object*) renderedObjects[i]);
	}
#if _DEBUG
	DebugRenderer::drawTo(eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv, eyeTo->renderInfo.viewport);
	context->VSSetShader(StandardVertexShader, NULL, NULL);
	context->PSSetShader(StandardPixelShader, NULL, NULL);
	context->IASetInputLayout(ILStandard);
#endif
	context->ClearDepthStencilView(eyeTo->renderInfo.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (size_t i = 0; i < frontRenderedObjects.size(); ++i)
	{
		renderObjectDefaultState((Object*)frontRenderedObjects[i]);
	}


	//ParticleManager::RenderParticlesTo(eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv, eyeTo->renderInfo.viewport, eyeTo->camera.view, eyeTo->camera.projection);
	//context->VSSetConstantBuffers(0, 1, &cameraBuffer);
	//context->VSSetShader(StandardVertexShader, NULL, NULL);
	//context->PSSetShader(StandardPixelShader, NULL, NULL);
	//context->IASetInputLayout(ILStandard);
}

void Renderer::drawSkyboxTo(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport, DirectX::XMFLOAT3& pos)
{
	if (!currSkybox)
		return;
	UINT stride = sizeof(VertexPositionTextureNormalAnim);
	UINT offset = 0;

	context->OMSetRenderTargets(1, &rtv, dsv);
	context->RSSetViewports(1, &viewport);
	context->VSSetShader(SkyboxVS, NULL, NULL);
	context->PSSetShader(SkyboxPS, NULL, NULL);
	context->UpdateSubresource(modelBuffer, NULL, NULL, &XMMatrixTranspose(XMMatrixTranslationFromVector(XMLoadFloat3(&pos))), NULL, NULL);
	context->IASetVertexBuffers(0, 1, &skyball->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(skyball->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->PSSetShaderResources(0, 1, &currSkybox->srv);
	context->DrawIndexed(skyball->indexCount, 0, 0);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->VSSetShader(StandardVertexShader, NULL, NULL);
	context->PSSetShader(StandardPixelShader, NULL, NULL);
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

void Renderer::createDeferredRTVs()
{
	for (int i = 0; i < 4; ++i)
	{
		createRTVandSRV(&deferredTextures.textures[i], &deferredTextures.SRVs[i], &deferredTextures.RTVs[i]);
	}
	D3D11_TEXTURE2D_DESC texDesc;
	deferredTextures.textures[0]->GetDesc(&texDesc);

	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.MiscFlags = NULL;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.Texture2D.MipSlice = 0;
	depthStencilDesc.Flags = 0;
	device->CreateTexture2D(&texDesc, nullptr, &deferredTextures.depthBuffer);
	device->CreateDepthStencilView(deferredTextures.depthBuffer, &depthStencilDesc, &deferredTextures.DSV);
}

void Renderer::createRTVandSRV(ID3D11Texture2D ** texture, ID3D11ShaderResourceView ** srv, ID3D11RenderTargetView ** rtv)
{
	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	D3D11_TEXTURE2D_DESC refDesc;
	backBuffer->GetDesc(&refDesc);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Height = refDesc.Height;
	texDesc.Width = refDesc.Width;
	texDesc.Width = 512;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	texDesc.ArraySize = 1;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc = sampleDesc;
	device->CreateTexture2D(&texDesc, nullptr, texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(*texture, &srvDesc, srv);
	device->CreateRenderTargetView(*texture, nullptr, rtv);
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

void Renderer::loadSkyboxFaces(Skybox * toLoad, const char * directory, const char * filePrefix)
{
	std::string path = std::string(directory);
	path = "Assets/Skyboxes/" + path;
	ID3D11ShaderResourceView* throwitaway;
	for (int i = 0; i < 6; ++i)
	{
		std::string truepath = path + '/' + filePrefix + "_c0" + (char)(i+48) + ".png";
		std::wstring forrealthistime(truepath.begin(), truepath.end());

		ID3D11Texture2D* tex;
		DirectX::CreateWICTextureFromFile(device, context, forrealthistime.c_str(), (ID3D11Resource**)&tex, &throwitaway);
		toLoad->faces[i] = tex;
		throwitaway->Release();
	}
}

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::Initialize(Window window, Transform* _cameraPos) {
	cameraPos = _cameraPos;
	createDeviceContextAndSwapchain(window);
	RECT windRect;
	GetWindowRect(window.GetOutputWindow(), &windRect);
	initViewport(windRect, &defaultPipeline);
	initDepthStencilBuffer(&defaultPipeline);
	initDepthStencilState(&defaultPipeline);
	initDepthStencilView(&defaultPipeline);
	initRasterState(&defaultPipeline);
	initShaders();
	//ParticleManager::Initialize(device, context, ParticleVS, ParticleGS, ParticlePS, ILParticle);
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
	animationManagement = new AnimationManager();
	context->VSSetConstantBuffers(0, 1, &cameraBuffer);
	context->VSSetConstantBuffers(1, 1, &modelBuffer);
	context->VSSetConstantBuffers(2, 1, &animDataBuffer);
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
	
	cameraPos->LookAt(DirectX::XMFLOAT3(0.0f, 0.0f, 5.0f));
	XMStoreFloat4x4(&defaultCamera.projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(60.0f * XM_PI / 180.0f, defaultPipeline.viewport.Width / defaultPipeline.viewport.Height, 0.001f, 300.0f)));

	if(VRManager::GetInstance().IsEnabled())
		setupVRTargets();

	MessageEvents::Subscribe(EVENT_Instantiated, [this](EventMessageBase * _e) {this->registerObject(_e); });
	MessageEvents::Subscribe(EVENT_Destroy, [this](EventMessageBase * _e) {this->unregisterObject(_e); });
	MessageEvents::Subscribe(EVENT_Unrender, [this](EventMessageBase * _e) {this->unregisterObject(_e); });
	MessageEvents::Subscribe(EVENT_Addrender, [this](EventMessageBase * _e) {this->registerObject(_e); });
	MessageEvents::Subscribe(EVENT_Rendertofront, [this](EventMessageBase * _e) {this->moveToFront(_e); });

#if _DEBUG
	DebugRenderer::Initialize(device, context, modelBuffer, PassThroughPositionColorVS, PassThroughPS, ILPositionColor, defaultPipeline.rasterizer_state);
#endif
	setSkybox("Ghostbait", "ghostbait");
	skyball = meshManagement->GetReferenceComponent("Assets/Skyball.mesh", nullptr);

	createDeferredRTVs();
}

void Renderer::Destroy() {
	OnlySamplerState->Release();
	cameraBuffer->Release();
	modelBuffer->Release();
	factorBuffer->Release();
	lightBuffer->Release();
	animDataBuffer->Release();
	ILPositionColor->Release();
	ILStandard->Release();
	ILParticle->Release();
	PassThroughPositionColorVS->Release();
	PassThroughPS->Release();
	StandardVertexShader->Release();
	StandardPixelShader->Release();
	ParticleVS->Release();
	ParticleGS->Release();
	ParticlePS->Release();
	SkyboxVS->Release();
	SkyboxPS->Release();
	backBuffer->Release();
	swapchain->Release();
	context->Release();
	device->Release();
	defaultPipeline.render_target_view->Release();
	clearPipelineMemory(&defaultPipeline);
	if(VRManager::GetInstance().IsEnabled()) {
		clearTextureMemory(&leftEye.renderInfo);
		clearTextureMemory(&rightEye.renderInfo);
	}
	meshManagement->Destroy();
	delete meshManagement;
	materialManagement->Destroy();
	delete materialManagement;
	animationManagement->Destroy();
	//ParticleManager::Destroy();
	delete animationManagement;
#if _DEBUG
	DebugRenderer::Destroy();
#endif
	if (currSkybox)
	{
		currSkybox->srv->Release();
		currSkybox->box->Release();
		for (int i = 0; i < 6; ++i)
		{
			currSkybox->faces[i]->Release();
		}
		delete currSkybox;
		currSkybox = nullptr;
	}
}

void Renderer::registerObject(EventMessageBase* e) {
	//TODO: Need logic to determine which objects group to push to
	//based off of what the object has - may include instructions in the future
	NewObjectMessage* instantiate = (NewObjectMessage*) e;
	if(instantiate->RetrieveObject()->GetComponent<Mesh>()) {
		renderedObjects.push_back(instantiate->RetrieveObject());
	}
}

void Renderer::unregisterObject(EventMessageBase* e) {
	StandardObjectMessage* removeobjMessage = (StandardObjectMessage*) e;
	//TODO: Need logic for which register it is under
	for(std::vector<const GameObject*>::iterator iter = renderedObjects.begin(); iter != renderedObjects.end(); ++iter) {
		if(*iter == removeobjMessage->RetrieveObject()) {
			renderedObjects.erase(iter);
			return;
		}
	}

	for (std::vector<const GameObject*>::iterator iter = frontRenderedObjects.begin(); iter != frontRenderedObjects.end(); ++iter)
	{
		if (*iter == removeobjMessage->RetrieveObject()) {
			frontRenderedObjects.erase(iter);
			return;
		}
	}
}

void Renderer::moveToFront(EventMessageBase * e)
{
	NewObjectMessage* move = (NewObjectMessage*)e;
	auto iter = renderedObjects.begin();
	for (; iter != renderedObjects.end(); ++iter)
	{
		if (*iter == move->RetrieveObject())
		{
			renderedObjects.erase(iter);
			break;
		}
	}
	frontRenderedObjects.push_back(move->RetrieveObject());
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
	XMMATRIX cameraObj = XMMatrixTranspose(XMLoadFloat4x4(&cameraPos->GetMatrix()));
	XMStoreFloat4x4(&defaultCamera.view, XMMatrixInverse(&XMMatrixDeterminant(cameraObj), cameraObj));
	if(VRManager::GetInstance().IsEnabled()) {
		VRManager::GetInstance().GetVRMatrices(&leftEye.camera.projection, &rightEye.camera.projection, &leftEye.camera.view, &rightEye.camera.view);

		//XMStoreFloat4x4(&leftEye.camera.projection, (XMLoadFloat4x4(&defaultCamera.projection)));
		//XMStoreFloat4x4(&rightEye.camera.projection, (XMLoadFloat4x4(&defaultCamera.projection)));

		XMStoreFloat4x4(&leftEye.camera.projection, XMMatrixTranspose(XMLoadFloat4x4(&leftEye.camera.projection)));
		XMStoreFloat4x4(&rightEye.camera.projection, XMMatrixTranspose(XMLoadFloat4x4(&rightEye.camera.projection)));
		leftEye.camPos = DirectX::XMFLOAT3(leftEye.camera.view._41, leftEye.camera.view._42, leftEye.camera.view._43);
		rightEye.camPos = DirectX::XMFLOAT3(rightEye.camera.view._41, rightEye.camera.view._42, rightEye.camera.view._43);
		XMStoreFloat4x4(&leftEye.camera.view, XMMatrixTranspose(XMMatrixInverse(&XMVectorSet(0, 0, 0, 0), XMLoadFloat4x4(&leftEye.camera.view))));
		XMStoreFloat4x4(&rightEye.camera.view, XMMatrixTranspose(XMMatrixInverse(&XMVectorSet(0, 0, 0, 0), XMLoadFloat4x4(&rightEye.camera.view))));

		LightManager::getLightBuffer()->cameraPos = leftEye.camPos;
		context->UpdateSubresource(lightBuffer, NULL, NULL, LightManager::getLightBuffer(), 0, 0);
		renderToEye(&leftEye);
		LightManager::getLightBuffer()->cameraPos = rightEye.camPos;
		context->UpdateSubresource(lightBuffer, NULL, NULL, LightManager::getLightBuffer(), 0, 0);
		renderToEye(&rightEye);
		VRManager::GetInstance().SendToHMD((void*) leftEye.renderInfo.texture, (void*) rightEye.renderInfo.texture);
		context->UpdateSubresource(cameraBuffer, 0, NULL, &(leftEye.camera), 0, 0);
	} else context->UpdateSubresource(cameraBuffer, 0, NULL, &defaultCamera, 0, 0);
	float color[] = {0.5f, 0.5f, 1.0f, 1.0f};
	context->ClearRenderTargetView(defaultPipeline.render_target_view, color);
	context->ClearDepthStencilView(defaultPipeline.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &defaultPipeline.render_target_view, defaultPipeline.depth_stencil_view);
	context->RSSetViewports(1, &defaultPipeline.viewport);

	DirectX::XMFLOAT3 camPos;
	if (VRManager::GetInstance().IsEnabled())
		camPos = leftEye.camPos;
	else
		camPos = DirectX::XMFLOAT3(cameraPos->GetMatrix()._41, cameraPos->GetMatrix()._42, cameraPos->GetMatrix()._43);
	LightManager::getLightBuffer()->cameraPos = camPos;
	context->UpdateSubresource(lightBuffer, NULL, NULL, LightManager::getLightBuffer(), 0, 0);
	drawSkyboxTo(defaultPipeline.render_target_view, defaultPipeline.depth_stencil_view, defaultPipeline.viewport, camPos);
	for(size_t i = 0; i < renderedObjects.size(); ++i) {
		renderObjectDefaultState((Object*) renderedObjects[i]);
	}
#if _DEBUG
	DebugRenderer::flushTo(defaultPipeline.render_target_view, defaultPipeline.depth_stencil_view, defaultPipeline.viewport);
	context->VSSetShader(StandardVertexShader, NULL, NULL);
	context->PSSetShader(StandardPixelShader, NULL, NULL);
	context->IASetInputLayout(ILStandard);
#endif
	context->ClearDepthStencilView(defaultPipeline.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (size_t i = 0; i < frontRenderedObjects.size(); ++i)
	{
		renderObjectDefaultState((Object*)frontRenderedObjects[i]);
	}
	//DirectX::XMFLOAT4X4 view, proj;
	//if (VRManager::GetInstance().IsEnabled())
	//{
	//	view = leftEye.camera.view;
	//	proj = leftEye.camera.projection;
	//}
	//else
	//{
	//	view = defaultCamera.view;
	//	proj = defaultCamera.projection;
	//}
	//ParticleManager::RenderParticlesTo(defaultPipeline.render_target_view, defaultPipeline.depth_stencil_view, defaultPipeline.viewport, view, proj);
	//context->VSSetConstantBuffers(0, 1, &cameraBuffer);

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
	device->CreateDepthStencilView((ID3D11Resource*) pipelineTo->depth_stencil_buffer, &depthStencilViewDesc, &pipelineTo->depth_stencil_view);
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

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleVS.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &ParticleVS);

	D3D11_INPUT_ELEMENT_DESC particleVSDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	device->CreateInputLayout(particleVSDesc, ARRAYSIZE(particleVSDesc), byteCode, byteCodeSize, &ILParticle);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleGS.cso");
	device->CreateGeometryShader(byteCode, byteCodeSize, NULL, &ParticleGS);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticlePS.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &ParticlePS);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "SkyboxVertexShader.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &SkyboxVS);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "SkyboxPixelShader.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &SkyboxPS);
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

	CD3D11_BUFFER_DESC animBufferDesc(sizeof(animDataBufferStruct), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&animBufferDesc, nullptr, &animDataBuffer);
}

void Renderer::initViewport(const RECT window, pipeline_state_t * pipelineTo) {
	D3D11_VIEWPORT tempView;
	tempView.Height = (float) window.bottom - (float) window.top;
	tempView.Width = (float) window.right - (float) window.left;
	tempView.MaxDepth = 1.0f;
	tempView.MinDepth = 0.0f;
	tempView.TopLeftX = 0.0f;
	tempView.TopLeftY = 0.0f;

	pipelineTo->viewport = tempView;
}

void Renderer::setSkybox(const char* directoryName, const char* filePrefix)
{
	if (currSkybox)
	{
		currSkybox->srv->Release();
		currSkybox->box->Release();
		for (int i = 0; i < 6; ++i)
		{
			currSkybox->faces[i]->Release();
		}
		delete currSkybox;
		currSkybox = nullptr;
	}

	Skybox* toSet = new Skybox();
	loadSkyboxFaces(toSet, directoryName, filePrefix);
	D3D11_TEXTURE2D_DESC refDesc;
	D3D11_TEXTURE2D_DESC texdesc;
	toSet->faces[0]->GetDesc(&refDesc);
	texdesc.Height = refDesc.Height;
	texdesc.Width = refDesc.Width;
	texdesc.Format = refDesc.Format;
	texdesc.CPUAccessFlags = NULL;
	texdesc.SampleDesc.Count = 1;
	texdesc.SampleDesc.Quality = 0;
	texdesc.Usage = D3D11_USAGE_DEFAULT;
	texdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texdesc.MipLevels = 1;
	texdesc.ArraySize = 6;
	texdesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	device->CreateTexture2D(&texdesc, nullptr, &toSet->box);
	
	D3D11_BOX src;
	src.left = 0;
	src.top = 0;
	src.right = refDesc.Width;
	src.bottom = refDesc.Height;
	src.front = 0;
	src.back = 1;

	for (int i = 0; i < 6; ++i)
	{
		context->CopySubresourceRegion(toSet->box, D3D11CalcSubresource(0, i, 1), 0, 0, 0, toSet->faces[i], 0, &src);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.Format = texdesc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvdesc.TextureCube.MostDetailedMip = 0;
	srvdesc.TextureCube.MipLevels = texdesc.MipLevels;
	device->CreateShaderResourceView((ID3D11Resource*)toSet->box, &srvdesc, &toSet->srv);
	currSkybox = toSet;
}

MeshManager* Renderer::getMeshManager() { return meshManagement; }
MaterialManager* Renderer::getMaterialManager() { return materialManagement; }
AnimationManager* Renderer::getAnimationManager() { return animationManagement; }
Transform* Renderer::getCamera() { return cameraPos; }
