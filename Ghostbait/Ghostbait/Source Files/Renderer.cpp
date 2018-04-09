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
#include "TextManager.h"
#include "ScrollingUVManager.h"

//TODO: TEMP for testing a weird crash
#include "Projectile.h"

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

	D3D_FEATURE_LEVEL * feature = new D3D_FEATURE_LEVEL(D3D_FEATURE_LEVEL_11_0);
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
	pipeline->blend_state->Release();
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

	createDeferredRTVs(&leftEye.targets, leftEye.renderInfo.texture);
	createDeferredRTVs(&rightEye.targets, rightEye.renderInfo.texture);
}

void Renderer::releaseDeferredTarget(DeferredRTVs * in)
{
	for (int i = 0; i < 6; ++i)
	{
		in->RTVs[i]->Release();
		in->SRVs[i]->Release();
		in->textures[i]->Release();
	}
	in->DSV->Release();
	in->depthBuffer->Release();
}

void Renderer::combineDeferredTargets(DeferredRTVs * in, ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport)
{
	context->PSSetSamplers(0, 1, &LinearSamplerState);
	blurTexture(viewport, in->textures[1], in->SRVs[1], 9, in->RTVs[1], in->DSV);
	context->PSSetSamplers(0, 1, &PointSamplerState);

	float color[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	UINT stride = sizeof(XMFLOAT4);
	UINT offset = 0;
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &rtv, dsv);
	context->RSSetViewports(1, &viewport);
	context->VSSetShader(PassThroughPositionVS, NULL, NULL);
	context->GSSetShader(NDCQuadGS, NULL, NULL);
	context->PSSetShader(StandardPixelShader, NULL, NULL);
	context->PSSetShaderResources(0, 6, in->SRVs);
	context->PSSetConstantBuffers(2, 1, &cameraBuffer);
	context->IASetVertexBuffers(0, 1, &emptyFloat3Buffer, &stride, &offset);
	context->IASetInputLayout(ILPosition);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw(1, 0);
	ID3D11GeometryShader* temp = nullptr;
	context->GSSetShader(temp, NULL, NULL);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11ShaderResourceView* getemouttahere[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 6, getemouttahere);
	context->IASetInputLayout(ILStandard);
}

bool Renderer::compareDistToCam(const XMFLOAT3 & t1, const XMFLOAT3 & t2, const XMFLOAT3& camPos)
{
	return manhat(t1, camPos) > manhat(t2, camPos);
}

float Renderer::manhat(const XMFLOAT3 & center1, const XMFLOAT3 &center2)
{
	float distX = center2.x - center1.x;
	if (distX < 0.0f) distX *= -1.0f;

	float distY = center2.y - center1.y;
	if (distY < 0.0f) distY *= -1.0f;

	float distZ = center2.z - center1.z;
	if (distZ < 0.0f) distZ *= -1.0f;
	return distX + distY + distZ;
}

void Renderer::blurTexture(D3D11_VIEWPORT & viewport, ID3D11Texture2D * tex, ID3D11ShaderResourceView * srv, unsigned int passes, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsvIn)
{
	if (!tex || !srv || !passes)
		return;
	ID3D11Texture2D* tempTex;
	ID3D11Texture2D* swapTex;
	ID3D11RenderTargetView* tempRtv;
	ID3D11ShaderResourceView* tempSrv;
	ID3D11RenderTargetView* swapRtv;
	ID3D11ShaderResourceView* swapSrv;
	D3D11_VIEWPORT tempViewport;
	D3D11_TEXTURE2D_DESC texDesc;
	tex->GetDesc(&texDesc);
	texDesc.Height = (UINT)((float)texDesc.Height * 0.35f);
	texDesc.Width = (UINT)((float)texDesc.Width * 0.35f);
	tempViewport.Height = (float)texDesc.Height;
	tempViewport.Width = (float)texDesc.Width;
	tempViewport.MaxDepth = viewport.MaxDepth;
	tempViewport.MinDepth = viewport.MinDepth;
	tempViewport.TopLeftX = viewport.TopLeftX;
	tempViewport.TopLeftY = viewport.TopLeftY;
	device->CreateTexture2D(&texDesc, nullptr, &tempTex);
	device->CreateTexture2D(&texDesc, nullptr, &swapTex);
	device->CreateRenderTargetView((ID3D11Resource*)tempTex, nullptr, &tempRtv);
	device->CreateRenderTargetView((ID3D11Resource*)swapTex, nullptr, &swapRtv);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(tempTex, &srvDesc, &tempSrv);
	device->CreateShaderResourceView(swapTex, &srvDesc, &swapSrv);
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.Texture2D.MipSlice = 0;
	depthStencilDesc.Flags = 0;

	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.MiscFlags = NULL;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	ID3D11Texture2D* depthBuffer;
	ID3D11DepthStencilView* dsv;
	device->CreateTexture2D(&texDesc, nullptr, &depthBuffer);
	device->CreateDepthStencilView(depthBuffer, &depthStencilDesc, &dsv);

	blurData toShader;
	toShader.width = 1.0f / (float)texDesc.Width;
	toShader.height = 1.0f / (float)texDesc.Height;
	context->VSSetShader(PassThroughPositionVS, NULL, NULL);
	UINT stride = sizeof(XMFLOAT4);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &emptyFloat3Buffer, &stride, &offset);
	context->RSSetViewports(1, &tempViewport);
	context->GSSetShader(NDCQuadGS, NULL, NULL);
	context->PSSetShader(BlurPixelShader, NULL, NULL);
	context->PSSetConstantBuffers(3, 1, &blurDataBuffer);
	context->IASetInputLayout(ILPosition);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	ID3D11RenderTargetView* clearrtv = nullptr;
	ID3D11ShaderResourceView* clearsrv = nullptr;
	context->OMSetRenderTargets(1, &clearrtv, nullptr);
	context->OMSetRenderTargets(1, &tempRtv, dsv);
	toShader.dir = { 1.0f, 0.0f };
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->UpdateSubresource(blurDataBuffer, NULL, NULL, &toShader, NULL, NULL);
	context->PSSetShaderResources(0, 1, &srv);
	context->Draw(1, 0);
	context->OMSetRenderTargets(1, &clearrtv, nullptr);
	toShader.dir = { 0.0f, 1.0f };
	context->OMSetRenderTargets(1, &swapRtv, dsv);
	context->UpdateSubresource(blurDataBuffer, NULL, NULL, &toShader, NULL, NULL);
	context->PSSetShaderResources(0, 1, &tempSrv);
	context->Draw(1, 0);
	
	for (unsigned int i = 0; i < passes - 1; ++i)
	{
		context->PSSetShaderResources(0, 1, &clearsrv);
		context->OMSetRenderTargets(1, &tempRtv, dsv);
		toShader.dir = { 1.0f, 0.0f };
		context->UpdateSubresource(blurDataBuffer, NULL, NULL, &toShader, NULL, NULL);
		context->PSSetShaderResources(0, 1, &swapSrv);
		context->Draw(1, 0);
		if (i == passes - 2)
		{
			context->OMSetRenderTargets(1, &clearrtv, nullptr);
			context->PSSetShaderResources(0, 1, &clearsrv);
			toShader.dir = { 0.0f, 1.0f };
			context->OMSetRenderTargets(1, &rtv, dsvIn);
			context->RSSetViewports(1, &viewport);
			context->UpdateSubresource(blurDataBuffer, NULL, NULL, &toShader, NULL, NULL);
			context->PSSetShaderResources(0, 1, &tempSrv);
			context->Draw(1, 0);
		}
		else
		{
			context->PSSetShaderResources(0, 1, &clearsrv);
			toShader.dir = { 0.0f, 1.0f };
			context->OMSetRenderTargets(1, &swapRtv, dsv);
			context->UpdateSubresource(blurDataBuffer, NULL, NULL, &toShader, NULL, NULL);
			context->PSSetShaderResources(0, 1, &tempSrv);
			context->Draw(1, 0);
		}
	}
	context->OMSetRenderTargets(1, &clearrtv, nullptr);
	context->PSSetShaderResources(0, 1, &clearsrv);
	tempSrv->Release();
	swapSrv->Release();
	tempTex->Release();
	swapTex->Release();
	tempRtv->Release();
	swapRtv->Release();
	dsv->Release();
	depthBuffer->Release();
}

void Renderer::sortTransparentObjects(DirectX::XMFLOAT3 &camPos)
{
	if (transparentObjects.size() <= 1)
		return;
	//Insertion sort simply because I don't anticipate the size of this pool getting too large
	for (size_t outer = 0; outer < transparentObjects.size() - 1; outer++)
	{
		int counter = (int)outer + 1;
		while (counter>0)
		{
			if (compareDistToCam(transparentObjects[counter]->transform.GetPosition(), transparentObjects[counter-1]->transform.GetPosition(), camPos))
			{
				const GameObject* temp = transparentObjects[counter];
				transparentObjects[counter] = transparentObjects[counter - 1];
				transparentObjects[counter - 1] = temp;
			}
			counter--;
		}
	}
}

void Renderer::renderObjectDefaultState(const GameObject * obj) {
	UINT stride = sizeof(VertexPositionTextureNormalAnim);
	UINT offset = 0;
	Mesh* y= obj->GetComponent<Mesh>();
	if (!y) {
		Console::ErrorLine << "Object does not have a mesh component!";
		assert(1 == 0);
	}
	ID3D11Buffer* x = y->vertexBuffer;

	context->IASetVertexBuffers(0, 1, &x, &stride, &offset);
	context->IASetIndexBuffer(obj->GetComponent<Mesh>()->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->UpdateSubresource(modelBuffer, 0, NULL, &XMMatrixTranspose(XMLoadFloat4x4(&obj->transform.GetMatrix())), 0, 0);
	Material* mat = obj->GetComponent<Material>();
	if (mat)
	{
		obj->GetComponent<Material>()->bindToShader(context, factorBuffer, obj->GetFlags() & GAMEOBJECT_PUBLIC_FLAGS::UNLIT);
		if (mat->flags & Material::MaterialFlags::POINT)
			context->PSSetSamplers(0, 1, &PointSamplerState);
		else
			context->PSSetSamplers(0, 1, &LinearSamplerState);
	}
	else
	{
		context->PSSetSamplers(0, 1, &PointSamplerState);
		materialManagement->GetNullMaterial()->bindToShader(context, factorBuffer, true);
	}
	
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
	ScrollingUV* scroll = obj->GetComponent<ScrollingUV>();
	if (scroll)
		uvData.offsets = scroll->offset;
	else
		uvData.offsets = { 0.0f, 0.0f };
	context->UpdateSubresource(uvDataBuffer, NULL, NULL, &uvData, NULL, NULL);
	context->PSSetShader(DeferredTargetPS, NULL, NULL);
	//materialManagement->GetElement(UINT_MAX)->bindToShader(context, factorBuffer);
	context->DrawIndexed(obj->GetComponent<Mesh>()->indexCount, 0, 0);
}

void Renderer::renderToEye(eye * eyeTo) {
	context->PSSetSamplers(0, 1, &LinearSamplerState);

	float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
	for (int i = 0; i < 6; ++i)
	{
		context->ClearRenderTargetView(eyeTo->targets.RTVs[i], color);
	}
	context->ClearDepthStencilView(eyeTo->targets.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->UpdateSubresource(cameraBuffer, 0, NULL, &eyeTo->camera, 0, 0);
	drawSkyboxTo(eyeTo->targets.RTVs, eyeTo->targets.DSV, eyeTo->renderInfo.viewport, eyeTo->camPos);
	context->OMSetRenderTargets(6, eyeTo->targets.RTVs, eyeTo->targets.DSV);
	context->RSSetViewports(1, &eyeTo->renderInfo.viewport);

	context->PSSetConstantBuffers(2, 1, &uvDataBuffer);
	for(size_t i = 0; i < renderedObjects.size(); ++i) {
		renderObjectDefaultState(renderedObjects[i]);
	}
#if _DEBUG
	DebugRenderer::drawTo(eyeTo->targets.RTVs, eyeTo->targets.DSV, eyeTo->renderInfo.viewport);
	context->VSSetShader(StandardVertexShader, NULL, NULL);
	context->PSSetShader(DeferredTargetPS, NULL, NULL);
	context->OMSetRenderTargets(6, eyeTo->targets.RTVs, eyeTo->targets.DSV);
	context->IASetInputLayout(ILStandard);
#endif
	for (size_t i = 0; i < transparentObjects.size(); ++i)
	{
		renderObjectDefaultState(transparentObjects[i]);
	}
	context->ClearDepthStencilView(eyeTo->targets.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (size_t i = 0; i < frontRenderedObjects.size(); ++i)
	{
		renderObjectDefaultState(frontRenderedObjects[i]);
	}


	//ParticleManager::RenderParticlesTo(eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv, eyeTo->renderInfo.viewport, eyeTo->camera.view, eyeTo->camera.projection);
	//context->VSSetConstantBuffers(0, 1, &cameraBuffer);
	//context->VSSetShader(StandardVertexShader, NULL, NULL);
	//context->PSSetShader(StandardPixelShader, NULL, NULL);
	//context->IASetInputLayout(ILStandard);

	viewProjectionConstantBuffer buff;
	
	XMStoreFloat4x4(&buff.view, XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&eyeTo->camera.view)), XMLoadFloat4x4(&eyeTo->camera.view)));
	XMStoreFloat4x4(&buff.projection, XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&eyeTo->camera.projection)), XMLoadFloat4x4(&eyeTo->camera.projection)));
	
	context->UpdateSubresource(cameraBuffer, NULL, NULL, &buff, NULL, NULL);
	combineDeferredTargets(&eyeTo->targets, eyeTo->renderInfo.rtv, eyeTo->renderInfo.dsv, eyeTo->renderInfo.viewport);
	
}

void Renderer::drawSkyboxTo(ID3D11RenderTargetView ** rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport, DirectX::XMFLOAT3& pos)
{
	if (!currSkybox)
		return;
	UINT stride = sizeof(VertexPositionTextureNormalAnim);
	UINT offset = 0;

	context->OMSetRenderTargets(6, rtv, dsv);
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

void Renderer::createDeferredRTVs(DeferredRTVs* toWrite, ID3D11Texture2D* refTex)
{
	createRTVandSRV(&toWrite->textures[0], &toWrite->SRVs[0], &toWrite->RTVs[0], refTex, DXGI_FORMAT_R16G16B16A16_FLOAT);
	createRTVandSRV(&toWrite->textures[1], &toWrite->SRVs[1], &toWrite->RTVs[1], refTex, DXGI_FORMAT_R16G16B16A16_FLOAT);
	createRTVandSRV(&toWrite->textures[2], &toWrite->SRVs[2], &toWrite->RTVs[2], refTex, DXGI_FORMAT_R16G16B16A16_FLOAT);
	createRTVandSRV(&toWrite->textures[3], &toWrite->SRVs[3], &toWrite->RTVs[3], refTex, DXGI_FORMAT_R16_FLOAT);
	createRTVandSRV(&toWrite->textures[4], &toWrite->SRVs[4], &toWrite->RTVs[4], refTex, DXGI_FORMAT_R16G16_FLOAT);
	createRTVandSRV(&toWrite->textures[5], &toWrite->SRVs[5], &toWrite->RTVs[5], refTex, DXGI_FORMAT_R16_FLOAT);
									   
	D3D11_TEXTURE2D_DESC texDesc;
	toWrite->textures[0]->GetDesc(&texDesc);

	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.MiscFlags = NULL;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.Texture2D.MipSlice = 0;
	depthStencilDesc.Flags = 0;

	device->CreateTexture2D(&texDesc, nullptr, &toWrite->depthBuffer);
	device->CreateDepthStencilView(toWrite->depthBuffer, &depthStencilDesc, &toWrite->DSV);
}

void Renderer::createRTVandSRV(ID3D11Texture2D ** texture, ID3D11ShaderResourceView ** srv, ID3D11RenderTargetView ** rtv, ID3D11Texture2D* refTex, DXGI_FORMAT format)
{
	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	D3D11_TEXTURE2D_DESC refDesc;
	refTex->GetDesc(&refDesc);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Height = refDesc.Height;
	texDesc.Width = refDesc.Width;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MipLevels = 1;
	texDesc.Format = format;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	texDesc.ArraySize = 1;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc = sampleDesc;
	device->CreateTexture2D(&texDesc, nullptr, texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
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
	initBlendState(&defaultPipeline);
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
	device->CreateSamplerState(&sampleDesc, &LinearSamplerState);
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	device->CreateSamplerState(&sampleDesc, &PointSamplerState);
	context->PSSetSamplers(0, 1, &LinearSamplerState);
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
	MessageEvents::Subscribe(EVENT_Rendertransparent, [this](EventMessageBase * _e) {this->moveToTransparent(_e); });
#if _DEBUG
	DebugRenderer::Initialize(device, context, modelBuffer, PassThroughPositionColorVS, PassThroughPS, ILPositionColor, defaultPipeline.rasterizer_state);
#endif
	setSkybox("Ghostbait", "ghostbait");
	skyball = meshManagement->GetReferenceComponent("Assets/Skyball.mesh", nullptr);

	createDeferredRTVs(&deferredTextures, backBuffer);
	context->OMSetBlendState(defaultPipeline.blend_state, 0, 0xffffffff);

	TextManager::Initialize(device, context, TextVertexShader, PositionTexturePixelShader, ILPositionTexture);
	TextManager::LoadFont("Assets/Fonts/defaultFontIndex.txt", "Assets/Fonts/defaultFont.png");

	LightManager::Initialize(device, context);
}

void Renderer::Destroy() {
	TextManager::Destroy();
	LightManager::Destroy();
	emptyFloat3Buffer->Release();
	LinearSamplerState->Release();
	PointSamplerState->Release();
	cameraBuffer->Release();
	modelBuffer->Release();
	factorBuffer->Release();
	lightBuffer->Release();
	blurDataBuffer->Release();
	uvDataBuffer->Release();
	animDataBuffer->Release();
	ILPositionColor->Release();
	ILStandard->Release();
	ILParticle->Release();
	ILPosition->Release();
	ILPositionTexture->Release();
	PassThroughPositionColorVS->Release();
	PassThroughPositionVS->Release();
	PassThroughPS->Release();
	StandardVertexShader->Release();
	StandardPixelShader->Release();
	ParticleVS->Release();
	ParticleGS->Release();
	NDCQuadGS->Release();
	ParticlePS->Release();
	SkyboxVS->Release();
	SkyboxPS->Release();
	DeferredTargetPS->Release();
	BlurPixelShader->Release();
	TextVertexShader->Release();
	PositionTexturePixelShader->Release();
	backBuffer->Release();
	swapchain->Release();
	context->Release();
	device->Release();
	defaultPipeline.render_target_view->Release();
	clearPipelineMemory(&defaultPipeline);
	if(VRManager::GetInstance().IsEnabled()) {
		releaseDeferredTarget(&leftEye.targets);
		releaseDeferredTarget(&rightEye.targets);
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

	releaseDeferredTarget(&deferredTextures);
}

void Renderer::registerObject(EventMessageBase* e) {
	NewObjectMessage* instantiate = (NewObjectMessage*) e;
	if(instantiate->obj->GetComponent<Mesh>()) {
		renderedObjects.push_back(instantiate->RetrieveObject());
	}
}

void Renderer::unregisterObject(EventMessageBase* e) {
	StandardObjectMessage* removeobjMessage = (StandardObjectMessage*) e;
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

	for (std::vector<const GameObject*>::iterator iter = transparentObjects.begin(); iter != transparentObjects.end(); ++iter)
	{
		if (*iter == removeobjMessage->RetrieveObject()) {
			transparentObjects.erase(iter);
			return;
		}
	}
}

void Renderer::moveToFront(EventMessageBase * e)
{
	NewObjectMessage* move = (NewObjectMessage*)e;
	if (!move->RetrieveObject()->GetComponent<Mesh>())
		return;
	auto iter = renderedObjects.begin();
	for (; iter != renderedObjects.end(); ++iter)
	{
		if (*iter == move->RetrieveObject())
		{
			frontRenderedObjects.push_back(move->RetrieveObject());
			renderedObjects.erase(iter);
			return;
		}
	}
	frontRenderedObjects.push_back(move->RetrieveObject());

}

void Renderer::moveToTransparent(EventMessageBase * e)
{
	NewObjectMessage* move = (NewObjectMessage*)e;
	auto iter = renderedObjects.begin();
	if (!move->RetrieveObject()->GetComponent<Mesh>())
		return;
	for (; iter != renderedObjects.end(); ++iter)
	{
		if (*iter == move->RetrieveObject())
		{
			transparentObjects.push_back(move->RetrieveObject());
			renderedObjects.erase(iter);
			return;
		}
	}
	transparentObjects.push_back(move->RetrieveObject());
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
		sortTransparentObjects(leftEye.camPos);
		rightEye.camPos = DirectX::XMFLOAT3(rightEye.camera.view._41, rightEye.camera.view._42, rightEye.camera.view._43);
		XMStoreFloat4x4(&leftEye.camera.view, XMMatrixTranspose(XMMatrixInverse(&XMVectorSet(0, 0, 0, 0), XMLoadFloat4x4(&leftEye.camera.view))));
		XMStoreFloat4x4(&rightEye.camera.view, XMMatrixTranspose(XMMatrixInverse(&XMVectorSet(0, 0, 0, 0), XMLoadFloat4x4(&rightEye.camera.view))));

		LightManager::getLightBuffer()->cameraPos = leftEye.camPos;
		context->UpdateSubresource(lightBuffer, NULL, NULL, LightManager::getLightBuffer(), 0, 0);
		LightManager::BindLightArray(context);
		renderToEye(&leftEye);
		LightManager::getLightBuffer()->cameraPos = rightEye.camPos;
		context->UpdateSubresource(lightBuffer, NULL, NULL, LightManager::getLightBuffer(), 0, 0);
		LightManager::BindLightArray(context);
		renderToEye(&rightEye);
		VRManager::GetInstance().SendToHMD((void*) leftEye.renderInfo.texture, (void*) rightEye.renderInfo.texture);
		context->UpdateSubresource(cameraBuffer, 0, NULL, &(leftEye.camera), 0, 0);
	}
	else
	{
		context->UpdateSubresource(cameraBuffer, 0, NULL, &defaultCamera, 0, 0);
		DirectX::XMFLOAT3 tempPos = cameraPos->GetPosition();
		sortTransparentObjects(tempPos);
	}
	context->PSSetSamplers(0, 1, &LinearSamplerState);

	float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
	context->ClearRenderTargetView(defaultPipeline.render_target_view, color);
	context->ClearDepthStencilView(defaultPipeline.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (int i = 0; i < 6; ++i)
	{
		context->ClearRenderTargetView(deferredTextures.RTVs[i], color);
	}
	context->ClearDepthStencilView(deferredTextures.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	

	DirectX::XMFLOAT3 camPos;
	if (VRManager::GetInstance().IsEnabled())
		camPos = leftEye.camPos;
	else
		camPos = DirectX::XMFLOAT3(cameraPos->GetMatrix()._41, cameraPos->GetMatrix()._42, cameraPos->GetMatrix()._43);
	LightManager::getLightBuffer()->cameraPos = camPos;
	context->UpdateSubresource(lightBuffer, NULL, NULL, LightManager::getLightBuffer(), 0, 0);
	LightManager::BindLightArray(context);
	drawSkyboxTo(deferredTextures.RTVs, deferredTextures.DSV, defaultPipeline.viewport, camPos);

	context->RSSetViewports(1, &defaultPipeline.viewport);

#if _DEBUG
	DebugRenderer::flushTo(deferredTextures.RTVs, deferredTextures.DSV, defaultPipeline.viewport);
	context->VSSetShader(StandardVertexShader, NULL, NULL);
	context->PSSetShader(DeferredTargetPS, NULL, NULL);
	context->OMSetRenderTargets(6, deferredTextures.RTVs, deferredTextures.DSV);
	context->IASetInputLayout(ILStandard);
#endif
	context->PSSetConstantBuffers(2, 1, &uvDataBuffer);

	for(size_t i = 0; i < renderedObjects.size(); ++i) {
		renderObjectDefaultState(renderedObjects[i]);
	}

	for (size_t i = 0; i < transparentObjects.size(); ++i)
	{
		renderObjectDefaultState(transparentObjects[i]);
	}
	context->ClearDepthStencilView(deferredTextures.DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (size_t i = 0; i < frontRenderedObjects.size(); ++i)
	{
		renderObjectDefaultState(frontRenderedObjects[i]);
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

	viewProjectionConstantBuffer buff;
	if (VRManager::GetInstance().IsEnabled())
	{
		XMStoreFloat4x4(&buff.view, XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&leftEye.camera.view)), XMLoadFloat4x4(&leftEye.camera.view)));
		XMStoreFloat4x4(&buff.projection, XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&leftEye.camera.projection)), XMLoadFloat4x4(&leftEye.camera.projection)));
	}
	else
	{
		XMStoreFloat4x4(&buff.view, XMMatrixInverse(nullptr, XMLoadFloat4x4(&defaultCamera.view)));
		XMStoreFloat4x4(&buff.projection, XMMatrixInverse(nullptr, XMLoadFloat4x4(&defaultCamera.projection)));
	}
	context->UpdateSubresource(cameraBuffer, NULL, NULL, &buff, NULL, NULL);
	combineDeferredTargets(&deferredTextures, defaultPipeline.render_target_view, defaultPipeline.depth_stencil_view, defaultPipeline.viewport);
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

void Renderer::initBlendState(pipeline_state_t * pipelineTo)
{
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendDesc, &pipelineTo->blend_state);
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

	LoadShaderFromCSO(&byteCode, byteCodeSize, "DeferredTargetPixelShader.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &DeferredTargetPS);
	delete[] byteCode;
	byteCode = nullptr;

	D3D11_INPUT_ELEMENT_DESC deferredVSDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	LoadShaderFromCSO(&byteCode, byteCodeSize, "PassThroughPositionVS.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &PassThroughPositionVS);

	device->CreateInputLayout(deferredVSDesc, ARRAYSIZE(deferredVSDesc), byteCode, byteCodeSize, &ILPosition);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "PointToNDCQuadGS.cso");
	device->CreateGeometryShader(byteCode, byteCodeSize, NULL, &NDCQuadGS);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "TextVertexShader.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &TextVertexShader);

	D3D11_INPUT_ELEMENT_DESC textVertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	device->CreateInputLayout(textVertexDesc, ARRAYSIZE(textVertexDesc), byteCode, byteCodeSize, &ILPositionTexture);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "PositionTexturePixelShader.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &PositionTexturePixelShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "BlurPixelShader.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &BlurPixelShader);
	delete[] byteCode;

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

	CD3D11_BUFFER_DESC blurBufferDesc(sizeof(blurData), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&blurBufferDesc, nullptr, &blurDataBuffer);

	CD3D11_BUFFER_DESC uvOffsetBufferDesc(sizeof(uvOffsetData), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&uvOffsetBufferDesc, nullptr, &uvDataBuffer);

	DirectX::XMFLOAT4 IseriouslyNeedthis = { 0.0f, 0.0f, 0.0f, 1.0f };
	CD3D11_BUFFER_DESC pointBufferDesc(sizeof(IseriouslyNeedthis), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA bufferData = { 0 };
	bufferData.pSysMem = &IseriouslyNeedthis;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;
	device->CreateBuffer(&pointBufferDesc, &bufferData, &emptyFloat3Buffer);
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
