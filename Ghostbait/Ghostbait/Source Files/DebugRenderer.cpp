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

void DebugRenderer::AddBox(BoxCollider cIn, DirectX::XMFLOAT3 color)
{
	DirectX::XMFLOAT3 cubeCorners[8];
	cubeCorners[0] = DirectX::XMFLOAT3(cIn.bottLeftBackCorner.x, cIn.bottLeftBackCorner.y, cIn.bottLeftBackCorner.z);
	cubeCorners[1] = DirectX::XMFLOAT3(cIn.topRightFrontCorner.x, cIn.bottLeftBackCorner.y, cIn.bottLeftBackCorner.z);
	cubeCorners[2] = DirectX::XMFLOAT3(cIn.bottLeftBackCorner.x, cIn.topRightFrontCorner.y, cIn.bottLeftBackCorner.z);
	cubeCorners[3] = DirectX::XMFLOAT3(cIn.topRightFrontCorner.x, cIn.topRightFrontCorner.y, cIn.bottLeftBackCorner.z);
	cubeCorners[4] = DirectX::XMFLOAT3(cIn.bottLeftBackCorner.x, cIn.bottLeftBackCorner.y, cIn.topRightFrontCorner.z);
	cubeCorners[5] = DirectX::XMFLOAT3(cIn.topRightFrontCorner.x, cIn.bottLeftBackCorner.y, cIn.topRightFrontCorner.z);
	cubeCorners[6] = DirectX::XMFLOAT3(cIn.bottLeftBackCorner.x, cIn.topRightFrontCorner.y, cIn.topRightFrontCorner.z);
	cubeCorners[7] = DirectX::XMFLOAT3(cIn.topRightFrontCorner.x, cIn.topRightFrontCorner.y, cIn.topRightFrontCorner.z);

	AddLine(cubeCorners[0], cubeCorners[1], color);
	AddLine(cubeCorners[0], cubeCorners[2], color);
	AddLine(cubeCorners[0], cubeCorners[4], color);
	AddLine(cubeCorners[1], cubeCorners[3], color);
	AddLine(cubeCorners[1], cubeCorners[5], color);
	AddLine(cubeCorners[2], cubeCorners[3], color);
	AddLine(cubeCorners[4], cubeCorners[5], color);
	AddLine(cubeCorners[4], cubeCorners[6], color);
	AddLine(cubeCorners[5], cubeCorners[7], color);
	AddLine(cubeCorners[3], cubeCorners[7], color);
	AddLine(cubeCorners[2], cubeCorners[6], color);
	AddLine(cubeCorners[6], cubeCorners[7], color);
}

void DebugRenderer::DrawAxes(DirectX::XMFLOAT4X4 toDraw, float length)
{
	XMFLOAT3 xAxis;
	xAxis.x = toDraw._11;
	xAxis.y = toDraw._12;
	xAxis.z = toDraw._13;
	XMFLOAT3 pos;
	pos.x = toDraw._41;
	pos.y = toDraw._42;
	pos.z = toDraw._43;
	XMFLOAT3 p1temp;
	p1temp = pos;
	XMFLOAT3 p2temp;
	XMStoreFloat3(&p2temp, XMLoadFloat3(&pos) + (length * XMLoadFloat3(&xAxis)));
	AddLine(p1temp, p2temp, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f));
	xAxis.x = toDraw._21;
	xAxis.y = toDraw._22;
	xAxis.z = toDraw._23;
	XMStoreFloat3(&p2temp, XMLoadFloat3(&pos) + (length * XMLoadFloat3(&xAxis)));
	AddLine(p1temp, p2temp, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	xAxis.x = toDraw._31;
	xAxis.y = toDraw._32;
	xAxis.z = toDraw._33;
	XMStoreFloat3(&p2temp, XMLoadFloat3(&pos) + (length * XMLoadFloat3(&xAxis)));
	AddLine(p1temp, p2temp, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
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
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DebugRenderer::flushTo(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport)
{
	drawTo(rtv, dsv, viewport);
	v_count = 0;
}
