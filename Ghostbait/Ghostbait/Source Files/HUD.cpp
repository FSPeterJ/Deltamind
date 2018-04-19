#include "HUD.h"
#include "WICTextureLoader.h"
#include <string>
#include <DirectXMath.h>
#include "MessageEvents.h"
#include "TextManager.h"
#include "GameData.h"

void HUD::ClearHUDElements()
{
	HideInventory();
	HideWaveInfo();
	for (size_t i = 0; i < HUDElements.size(); ++i)
	{
		delete HUDElements[i];
	}
	delete inv;
	delete wavIn;
	HUDElements.clear();
}

HUD::HUD(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight, ID3D11VertexShader* vs, ID3D11GeometryShader* gs, ID3D11PixelShader* ps, ID3D11InputLayout* il)
{
	TexToQuadPS = ps;
	PassThroughVS = vs;
	PointToNDCQuadGS = gs;
	IL = il;
	DirectX::XMFLOAT4 IseriouslyNeedthis = { 0.0f, 0.0f, 0.0f, 1.0f };
	CD3D11_BUFFER_DESC pointBufferDesc(sizeof(IseriouslyNeedthis), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA bufferData = { 0 };
	bufferData.pSysMem = &IseriouslyNeedthis;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;
	device->CreateBuffer(&pointBufferDesc, &bufferData, &throwawayVertexBuffer);
	Crosshair* temp = new Crosshair();
	temp->Initialize(device, context, windowWidth, windowHeight);
	HUDElements.push_back(temp);
	inv = new Inventory();
	inv->Initialize(device, context, windowWidth, windowHeight);
	wavIn = new WaveIndicator();
	wavIn->Initialize(device, context, windowWidth, windowHeight);
	MessageEvents::Subscribe(EVENT_WaveChange, [=](EventMessageBase* e) { UpdateWaveInfo(e); });
	MessageEvents::Subscribe(EVENT_EnemyDied, [=](EventMessageBase* e) { UpdateWaveInfo(); });

}

HUD::~HUD()
{
	ClearHUDElements();
	throwawayVertexBuffer->Release();
}

void HUD::Draw(ID3D11DeviceContext * context, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	UINT stride = sizeof(DirectX::XMFLOAT4);
	UINT offset = 0;
	ID3D11GeometryShader* clearGS = nullptr;
	context->VSSetShader(PassThroughVS, NULL, NULL);
	context->GSSetShader(PointToNDCQuadGS, NULL, NULL);
	context->PSSetShader(TexToQuadPS, NULL, NULL);
	context->IASetInputLayout(IL);
	context->IASetVertexBuffers(0, 1, &throwawayVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	for (size_t i = 0; i < HUDElements.size(); ++i)
	{
		context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		HUDElements[i]->Draw(context, dsv);
	}
	context->GSSetShader(clearGS, NULL, NULL);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void HUD::HideInventory()
{
	for (auto iter = HUDElements.begin(); iter != HUDElements.end(); ++iter)
	{
		if (*iter == inv)
		{
			HUDElements.erase(iter);
			break;
		}
	}
	showingInventory = false;
}

void HUD::ShowInventory()
{
	if (!showingInventory)
	{
		HUDElements.push_back(inv);
		showingInventory = true;
	}
}

void HUD::HideWaveInfo()
{
	for (auto iter = HUDElements.begin(); iter != HUDElements.end(); ++iter)
	{
		if (*iter == wavIn)
		{
			HUDElements.erase(iter);
			break;
		}
	}
	showingWave = false;
}

void HUD::ShowWaveInfo()
{
	if (!showingWave)
	{
		HUDElements.push_back(wavIn);
		showingWave = true;
	}
}

void HUD::UpdateWaveInfo(EventMessageBase * e)
{
	if (e)
	{
		const GameData* gameData = *((GameDataMessage*)e)->RetrieveData();
		if (gameData) {
			wavIn->SetEnemyCount(gameData->waveManager.GetCurrentWave()->enemyCount);
			wavIn->SetWave(gameData->waveManager.GetCurrentWaveNumber(), gameData->waveManager.GetWaveCount());
		}
	}
	else
	{
		wavIn->OffsetEnemyCount();
	}
}

HUD::Crosshair::~Crosshair()
{
	if (tex) tex->Release();
	if (srv) srv->Release();
}

void HUD::Crosshair::Initialize(ID3D11Device * device, ID3D11DeviceContext * context, float windowWidth, float windowHeight)
{
	std::wstring path(L"Assets//crosshair.png");
	HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&tex, &srv);
	viewport = D3D11_VIEWPORT();
	viewport.Height = 32.0f;
	viewport.Width = 32.0f;
	viewport.TopLeftX = (windowWidth * 0.5f) - (viewport.Width * 0.5f);
	viewport.TopLeftY = (windowHeight * 0.5f) - (viewport.Height * 0.5f);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void HUD::Crosshair::Draw(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv)
{
	context->RSSetViewports(1, &viewport);
	context->PSSetShaderResources(0, 1, &srv);
	context->Draw(1, 0);
}

void HUD::Inventory::loadTexture(const wchar_t * name, ID3D11Device* device, ID3D11DeviceContext* context, bool screenshot)
{
	std::wstring path(name);
	ID3D11Texture2D* tex;
	ID3D11ShaderResourceView* srv;
	HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&tex, &srv);
	textures.push_back(tex);
	if (screenshot)
		screenSRVs.push_back(srv);
	else
		srvs.push_back(srv);
}

void HUD::Inventory::initBlendStates(ID3D11Device* device)
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

	device->CreateBlendState(&blendDesc, &blend);
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.AlphaToCoverageEnable = true;
	device->CreateBlendState(&blendDesc, &noBlend);
}

bool HUD::Inventory::determineSelected(const int index)
{
	if (index < 3)
	{
		int selectedItem = player->leftController->GetSelectedItemIndex();

		if (selectedItem == -1)
			return false;
		else if (selectedItem == index)
			return true;
	}
	else
	{
		int invItem = index - 3;
		int selectedItem = player->rightController->GetSelectedItemIndex();

		if (selectedItem == -1)
			return false;
		else if (selectedItem == invItem)
			return true;
	}
	return false;
}

HUD::Inventory::~Inventory()
{
	for (size_t i = 0; i < viewports.size(); ++i)
	{
		srvs[i]->Release();
		screenSRVs[i]->Release();
	}
	for (size_t i = 0; i < textures.size(); ++i)
	{
		textures[i]->Release();
	}
	inactiveSRV->Release();
	inactiveTex->Release();
	activeSRV->Release();
	activeTex->Release();
	noBlend->Release();
	blend->Release();
}

void HUD::Inventory::Initialize(ID3D11Device * device, ID3D11DeviceContext * context, float windowWidth, float windowHeight)
{
	MessageEvents::SendMessage(EVENT_GetPlayer, GetPlayerMessage(&player));
	initBlendStates(device);
	float normalRangeWidth = 1.0f / 1200.0f;
	float normalRangeHeight = 1.0f / 600.0f;

	std::wstring path(L"Assets/InventoryPictures/baseInventoryFrame.png");
	HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&inactiveTex, &inactiveSRV);
	path = std::wstring(L"Assets/InventoryPictures/SelectedInventoryFrame.png");
	didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&activeTex, &activeSRV);
#pragma region Entering the Hardcode Zone
	D3D11_VIEWPORT viewport;
	viewport.Width = 92.0f;
	viewport.Height = 92.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	loadTexture(L"Assets/InventoryPictures/Slot1.png", device, context);
	viewport.TopLeftX = (32.0f * normalRangeWidth) * windowWidth;
	viewport.TopLeftY = (414.0f * normalRangeHeight) * windowHeight;
	viewports.push_back(viewport);
	loadTexture(L"Assets/InventoryPictures/Slot2.png", device, context);
	viewport.TopLeftX = (32.0f * normalRangeWidth) * windowWidth;
	viewport.TopLeftY = (472.0f * normalRangeHeight) * windowHeight;
	viewports.push_back(viewport);
	loadTexture(L"Assets/InventoryPictures/Slot3.png", device, context);
	viewport.TopLeftX = (32.0f * normalRangeWidth) * windowWidth;
	viewport.TopLeftY = (530.0f * normalRangeHeight) * windowHeight;
	viewports.push_back(viewport);
	loadTexture(L"Assets/InventoryPictures/Slot4.png", device, context);
	viewport.TopLeftX = (1104.0f * normalRangeWidth) * windowWidth;
	viewport.TopLeftY = (414.0f * normalRangeHeight) * windowHeight;
	viewports.push_back(viewport);
	loadTexture(L"Assets/InventoryPictures/Slot5.png", device, context);
	viewport.TopLeftX = (1104.0f * normalRangeWidth) * windowWidth;
	viewport.TopLeftY = (472.0f * normalRangeHeight) * windowHeight;
	viewports.push_back(viewport);
	loadTexture(L"Assets/InventoryPictures/Slot6.png", device, context);
	viewport.TopLeftX = (1104.0f * normalRangeWidth) * windowWidth;
	viewport.TopLeftY = (530.0f * normalRangeHeight) * windowHeight;
	viewports.push_back(viewport);

	loadTexture(L"Assets/InventoryPictures/leftPistolInv.png", device, context, true);
	loadTexture(L"Assets/InventoryPictures/leftSMGInv.png", device, context, true);
	loadTexture(L"Assets/InventoryPictures/leftBuildToolInv.png", device, context, true);
	loadTexture(L"Assets/InventoryPictures/rightPistolInv.png", device, context, true);
	loadTexture(L"Assets/InventoryPictures/rightSMGInv.png", device, context, true);
	loadTexture(L"Assets/InventoryPictures/rightBuildToolInv.png", device, context, true);

#pragma endregion
}

void HUD::Inventory::Draw(ID3D11DeviceContext * context, ID3D11DepthStencilView* dsv)
{
	for (size_t i = 0; i < viewports.size(); ++i)
	{
		context->RSSetViewports(1, &viewports[i]);
		if (determineSelected((int)i))
			context->PSSetShaderResources(0, 1, &activeSRV);
		else
			context->PSSetShaderResources(0, 1, &inactiveSRV);

		context->Draw(1, 0);
		context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->PSSetShaderResources(0, 1, &srvs[i]);
		context->OMSetBlendState(noBlend, 0, 0xffffffff);
		context->Draw(1, 0);
		context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->PSSetShaderResources(0, 1, &screenSRVs[i]);
		context->Draw(1, 0);
		context->OMSetBlendState(blend, 0, 0xffffffff);
	}
}

HUD::WaveIndicator::~WaveIndicator()
{
	tex->Release();
	srv->Release();
}

void HUD::WaveIndicator::Initialize(ID3D11Device * device, ID3D11DeviceContext * context, float windowWidth, float windowHeight)
{
	float normalRangeHeight = 1.0f / 600.0f;

	std::wstring path(L"Assets/InventoryPictures/HUDWaveInfoBorder.png");
	HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&tex, &srv);
	mats[0] = TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "W: 0/0", { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }).mat;
	mats[1] = TextManager::DrawTextTo("Assets/Fonts/defaultFont.png", "E: 0", { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }).mat;
	viewport[0].Height = 96.0f;
	viewport[0].Width = 256.0f;
	viewport[0].TopLeftX = (windowWidth * 0.5f) - (viewport[0].Width * 0.5f);
	viewport[0].TopLeftY = (20.0f * normalRangeHeight) * windowHeight;
	viewport[0].MinDepth = 0.0f;
	viewport[0].MaxDepth = 1.0f;
	viewport[1].Height = 88.0f;
	viewport[1].Width = 120.0f;
	viewport[1].TopLeftX = viewport[0].TopLeftX + 6;
	viewport[1].TopLeftY = viewport[0].TopLeftY + 4;
	viewport[1].MinDepth = 0.0f;
	viewport[1].MaxDepth = 1.0f;
	viewport[2].Height = 88.0f;
	viewport[2].Width = 114.0f;
	viewport[2].TopLeftX = viewport[0].TopLeftX + 137;
	viewport[2].TopLeftY = viewport[0].TopLeftY + 4;
	viewport[2].MinDepth = 0.0f;
	viewport[2].MaxDepth = 1.0f;
}

void HUD::WaveIndicator::Draw(ID3D11DeviceContext * context, ID3D11DepthStencilView * dsv)
{
	context->PSSetShaderResources(0, 1, &srv);
	context->RSSetViewports(1, &viewport[0]);
	context->Draw(1,0);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->PSSetShaderResources(0, 1, &mats[0]->diffuse.texView);
	context->RSSetViewports(1, &viewport[1]);
	context->Draw(1, 0);
	context->PSSetShaderResources(0, 1, &mats[1]->diffuse.texView);
	context->RSSetViewports(1, &viewport[2]);
	context->Draw(1, 0);
}

void HUD::WaveIndicator::OffsetEnemyCount(int offset)
{
	EnemiesRemaining += offset;
	TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", std::string("E: " + std::to_string(EnemiesRemaining)), mats[1], { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f });

}

void HUD::WaveIndicator::SetEnemyCount(int enemies)
{
	EnemiesRemaining = enemies;
	TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", std::string("E: " + std::to_string(EnemiesRemaining)), mats[1], { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f });

}


void HUD::WaveIndicator::SetWave(int wave, int maxWaves)
{
	TextManager::DrawTextExistingMat("Assets/Fonts/defaultFont.png", std::string("W: " + std::to_string(wave) + "/" + std::to_string(maxWaves)), mats[0], { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f });
}
