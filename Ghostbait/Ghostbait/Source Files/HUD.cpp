#include "HUD.h"
#include "WICTextureLoader.h"
#include <string>

void HUD::ClearHUDElements()
{
	for (size_t i = 0; i < HUDElements.size(); ++i)
	{
		delete HUDElements[i];
	}
	HUDElements.clear();
}

HUD::HUD(ID3D11Device* device, ID3D11DeviceContext* context, float windowWidth, float windowHeight)
{
	Crosshair* temp = new Crosshair();
	temp->Initialize(device, context, windowWidth, windowHeight);
	HUDElements.push_back(temp);
	temp = new Crosshair();
	temp->Initialize(device, context, windowWidth, windowHeight);
	HUDElements.push_back(temp);
}

HUD::~HUD()
{
	ClearHUDElements();
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

void HUD::Crosshair::Draw()
{
}
