#include "HUD.h"
#include "WICTextureLoader.h"
#include <string>
#include <DirectXMath.h>

void HUD::ClearHUDElements()
{
	for (size_t i = 0; i < HUDElements.size(); ++i)
	{
		delete HUDElements[i];
	}
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
	temp = new Crosshair();
	temp->Initialize(device, context, windowWidth, windowHeight);
	HUDElements.push_back(temp);
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
		HUDElements[i]->Draw(context);
	}
	context->GSSetShader(clearGS, NULL, NULL);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

void HUD::Crosshair::Draw(ID3D11DeviceContext* context)
{
	context->RSSetViewports(1, &viewport);
	context->PSSetShaderResources(0, 1, &srv);
	context->Draw(1, 0);
}
