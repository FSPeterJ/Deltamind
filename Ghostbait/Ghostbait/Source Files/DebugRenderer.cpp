#include "DebugRenderer.h"

ID3D11Device* DebugRenderer::device;
ID3D11DeviceContext* DebugRenderer::context;
ID3D11Buffer* DebugRenderer::vertBuff;
int DebugRenderer::v_count;
ID3D11Buffer* DebugRenderer::modelBuffer;
VertexPositionColor DebugRenderer::cpu_side_buffer[MAX_VERTS];
ID3D11VertexShader* DebugRenderer::vShader;
ID3D11PixelShader* DebugRenderer::pShader;
ID3D11InputLayout* DebugRenderer::iLayout;

void DebugRenderer::Initialize(ID3D11Device * deviceIn, ID3D11DeviceContext * contextIn, ID3D11Buffer * modelBufferIn, ID3D11VertexShader* vIn, ID3D11PixelShader* pIn, ID3D11InputLayout* layoutIn)
{
	device = deviceIn;
	context = contextIn;
	modelBuffer = modelBufferIn;
	vShader = vIn;
	pShader = pIn;
	iLayout = layoutIn;
	v_count = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cpu_side_buffer;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	D3D11_BUFFER_DESC vertBufferDesc;
	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertBufferDesc.MiscFlags = 0;
	vertBufferDesc.StructureByteStride = 0;
	vertBufferDesc.ByteWidth = sizeof(VertexPositionColor) * MAX_VERTS;
	HRESULT gotem = device->CreateBuffer(&vertBufferDesc, &vertexBufferData, &vertBuff);
}

void DebugRenderer::Destroy()
{
	vertBuff->Release();
}

void DebugRenderer::AddLine(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 color)
{
	if (v_count >= MAX_VERTS)
		return;
	VertexPositionColor t1;
	t1.pos = p1;
	t1.color = color;
	VertexPositionColor t2;
	t2.pos = p2;
	t2.color = color;
	cpu_side_buffer[v_count] = t1;
	cpu_side_buffer[v_count + 1] = t2;
	v_count += 2;
}

void DebugRenderer::drawTo(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport)
{
	context->OMSetRenderTargets(1, &rtv, dsv);
	context->RSSetViewports(1, &viewport);
	context->VSSetShader(vShader, NULL, NULL);
	context->PSSetShader(pShader, NULL, NULL);
	context->IASetInputLayout(iLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map((ID3D11Resource *)vertBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
	memcpy(mapped.pData, cpu_side_buffer, sizeof(VertexPositionColor) * v_count);
	context->Unmap((ID3D11Resource *)vertBuff, 0);
	//context->UpdateSubresource(vertBuff, 0, NULL, &cpu_side_buffer, 0, 0);
	context->UpdateSubresource(modelBuffer, 0, NULL, &DirectX::XMMatrixIdentity(), 0, 0);

	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	context->Draw(v_count, 0);
}

void DebugRenderer::flushTo(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport)
{
	drawTo(rtv, dsv, viewport);
	v_count = 0;
}
