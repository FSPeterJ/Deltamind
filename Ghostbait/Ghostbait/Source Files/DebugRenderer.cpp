#include "DebugRenderer.h"
#include "PhysicsComponent.h"  // for ColliderData, ColliderData::Data, ColliderData::Data::BoxCollider

ID3D11Device* DebugRenderer::device;
ID3D11DeviceContext* DebugRenderer::context;
ID3D11Buffer* DebugRenderer::vertBuff;
int DebugRenderer::v_count;
int DebugRenderer::tri_count;
ID3D11Buffer* DebugRenderer::modelBuffer;
VertexPositionColor DebugRenderer::cpu_side_buffer[MAX_VERTS];
VertexPositionColor DebugRenderer::cpu_side_triangles[MAX_VERTS];
ID3D11VertexShader* DebugRenderer::vShader;
ID3D11PixelShader* DebugRenderer::pShader;
ID3D11InputLayout* DebugRenderer::iLayout;
ID3D11RasterizerState* DebugRenderer::wireframeState;
ID3D11RasterizerState* DebugRenderer::defaultState;

using namespace DirectX;

void DebugRenderer::AddTri(VertexPositionColor * toMessWith, VertexPositionColor v1, VertexPositionColor v2, VertexPositionColor v3, int triIndex) {
	toMessWith[triIndex * 3] = v1;
	toMessWith[(triIndex * 3) + 1] = v2;
	toMessWith[(triIndex * 3) + 2] = v3;
}

void DebugRenderer::Initialize(ID3D11Device * deviceIn, ID3D11DeviceContext * contextIn, ID3D11Buffer * modelBufferIn, ID3D11VertexShader* vIn, ID3D11PixelShader* pIn, ID3D11InputLayout* layoutIn, ID3D11RasterizerState* stateIn) {
	device = deviceIn;
	context = contextIn;
	modelBuffer = modelBufferIn;
	vShader = vIn;
	pShader = pIn;
	iLayout = layoutIn;
	v_count = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
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

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;
	device->CreateRasterizerState(&rasterDesc, &wireframeState);
}

void DebugRenderer::Destroy() {
	vertBuff->Release();
	wireframeState->Release();
}

void DebugRenderer::AddLine(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 color) {
	if(v_count >= MAX_VERTS - 1)
		return;
	VertexPositionColor t1 = VertexPositionColor();
	t1.pos = p1;
	t1.color = color;
	VertexPositionColor t2 = VertexPositionColor();
	t2.pos = p2;
	t2.color = color;

	cpu_side_buffer[v_count] = t1;
	cpu_side_buffer[v_count + 1] = t2;
	v_count += 2;
}

void DebugRenderer::AddBox(ColliderData* cIn, DirectX::XMFLOAT3 color) {
	DirectX::XMFLOAT3 cubeCorners[8] = {DirectX::XMFLOAT3()}, min, max;
	max = cIn->colliderInfo.boxCollider.topRightFrontCorner;
	min = cIn->colliderInfo.boxCollider.bottLeftBackCorner;

	cubeCorners[0] = DirectX::XMFLOAT3(min.x, min.y, min.z);
	cubeCorners[1] = DirectX::XMFLOAT3(min.x, max.y, min.z);
	cubeCorners[2] = DirectX::XMFLOAT3(max.x, min.y, min.z);
	cubeCorners[3] = DirectX::XMFLOAT3(max.x, max.y, min.z);
	cubeCorners[4] = DirectX::XMFLOAT3(min.x, min.y, max.z);
	cubeCorners[5] = DirectX::XMFLOAT3(min.x, max.y, max.z);
	cubeCorners[6] = DirectX::XMFLOAT3(max.x, min.y, max.z);
	cubeCorners[7] = DirectX::XMFLOAT3(max.x, max.y, max.z);

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

void DebugRenderer::AddBox(DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max, DirectX::XMFLOAT3& color) {
	DirectX::XMFLOAT3 cubeCorners[8] = { DirectX::XMFLOAT3() };

	cubeCorners[0] = DirectX::XMFLOAT3(min.x, min.y, min.z);
	cubeCorners[1] = DirectX::XMFLOAT3(min.x, max.y, min.z);
	cubeCorners[2] = DirectX::XMFLOAT3(max.x, min.y, min.z);
	cubeCorners[3] = DirectX::XMFLOAT3(max.x, max.y, min.z);
	cubeCorners[4] = DirectX::XMFLOAT3(min.x, min.y, max.z);
	cubeCorners[5] = DirectX::XMFLOAT3(min.x, max.y, max.z);
	cubeCorners[6] = DirectX::XMFLOAT3(max.x, min.y, max.z);
	cubeCorners[7] = DirectX::XMFLOAT3(max.x, max.y, max.z);

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

void DebugRenderer::AddSphere(DirectX::XMFLOAT3 pos, float radius, DirectX::XMFLOAT3 color) {
	
	//http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

	if(tri_count + 60 > MAX_VERTS)
		return;
	const float t = (1.0f + sqrt(5.0f))*0.5f;
	float correctedR = (radius + 0.005f) / sqrt(t*t + 1.0f);

	VertexPositionColor verts[12];
	VertexPositionColor toBuffer[60];

	XMStoreFloat3(&verts[0].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(-1.0f, t, 0.0f))));
	XMStoreFloat3(&verts[1].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(1.0f, t, 0.0f))));
	XMStoreFloat3(&verts[2].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(-1.0f, -t, 0.0f))));
	XMStoreFloat3(&verts[3].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(1.0f, -t, 0.0f))));
													   
	XMStoreFloat3(&verts[4].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(0.0f, -1.0f, t))));
	XMStoreFloat3(&verts[5].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, t))));
	XMStoreFloat3(&verts[6].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(0.0f, -1.0f, -t))));
	XMStoreFloat3(&verts[7].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, -t))));
													   
	XMStoreFloat3(&verts[8].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(t, 0.0f, -1.0f))));
	XMStoreFloat3(&verts[9].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(t, 0.0f, 1.0f))));
	XMStoreFloat3(&verts[10].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(-t, 0.0f, -1.0f))));
	XMStoreFloat3(&verts[11].pos, XMLoadFloat3(&pos) + (correctedR * XMLoadFloat3(&XMFLOAT3(-t, 0.0f, 1.0f))));

	for(int i = 0; i < 12; ++i)
		verts[i].color = color;

	AddTri(toBuffer, verts[0], verts[11], verts[5], 0);
	AddTri(toBuffer, verts[0], verts[5], verts[1], 1);
	AddTri(toBuffer, verts[0], verts[1], verts[7], 2);
	AddTri(toBuffer, verts[0], verts[7], verts[10], 3);
	AddTri(toBuffer, verts[0], verts[10], verts[11], 4);

	AddTri(toBuffer, verts[1], verts[5], verts[9], 5);
	AddTri(toBuffer, verts[5], verts[11], verts[4], 6);
	AddTri(toBuffer, verts[11], verts[10], verts[2], 7);
	AddTri(toBuffer, verts[10], verts[7], verts[6], 8);
	AddTri(toBuffer, verts[7], verts[1], verts[8], 9);

	AddTri(toBuffer, verts[3], verts[9], verts[4], 10);
	AddTri(toBuffer, verts[3], verts[4], verts[2], 11);
	AddTri(toBuffer, verts[3], verts[2], verts[6], 12);
	AddTri(toBuffer, verts[3], verts[6], verts[8], 13);
	AddTri(toBuffer, verts[3], verts[8], verts[9], 14);

	AddTri(toBuffer, verts[4], verts[9], verts[5], 15);
	AddTri(toBuffer, verts[2], verts[4], verts[11], 16);
	AddTri(toBuffer, verts[6], verts[2], verts[10], 17);
	AddTri(toBuffer, verts[8], verts[6], verts[7], 18);
	AddTri(toBuffer, verts[9], verts[8], verts[1], 19);

	for(int i = 0; i < 60; ++i)
		cpu_side_triangles[i + tri_count] = toBuffer[i];
	tri_count += 60;
}

void DebugRenderer::AddSphere(DirectX::XMFLOAT3 pos, float radius, int LatLines, int LongLines, DirectX::XMFLOAT3 colo) {


	int NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	int NumSphereFaces = ((LatLines - 3)*(LongLines) * 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<VertexPositionColor> vertices(NumSphereVertices);
	std::vector<UINT> indices(NumSphereFaces * 3);

	DirectX::XMVECTOR currVertPos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 0.0f;
	vertices[0].pos.z = 1.0f * radius;

	for(int i = 0; i < LatLines - 2; ++i) {
		spherePitch = (i + 1) * (3.14f / (LatLines - 1));
		DirectX::XMMATRIX Rotationx = DirectX::XMMatrixRotationX(spherePitch);
		for(int j = 0; j < LongLines; ++j) {
			sphereYaw = j * (6.28f / (LongLines));
			DirectX::XMMATRIX Rotationy = DirectX::XMMatrixRotationZ(sphereYaw);
			currVertPos = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = DirectX::XMVector3Normalize(currVertPos);
			vertices[i*LongLines + j + 1].pos.x = DirectX::XMVectorGetX(currVertPos) * radius;
			vertices[i*LongLines + j + 1].pos.y = DirectX::XMVectorGetY(currVertPos) * radius;
			vertices[i*LongLines + j + 1].pos.z = DirectX::XMVectorGetZ(currVertPos) * radius;
		}
	}

	vertices[NumSphereVertices - 1].pos.x = 0.0f;
	vertices[NumSphereVertices - 1].pos.y = 0.0f;
	vertices[NumSphereVertices - 1].pos.z = -1.0f * radius;

	int k = 0;
	for(int l = 0; l < LongLines - 1; ++l) {
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for(int i = 0; i < LatLines - 3; ++i) {
		for(int j = 0; j < LongLines - 1; ++j) {
			indices[k] = i*LongLines + j + 1;
			indices[k + 1] = i*LongLines + j + 2;
			indices[k + 2] = (i + 1)*LongLines + j + 1;

			indices[k + 3] = (i + 1)*LongLines + j + 1;
			indices[k + 4] = i*LongLines + j + 2;
			indices[k + 5] = (i + 1)*LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i*LongLines) + LongLines;
		indices[k + 1] = (i*LongLines) + 1;
		indices[k + 2] = ((i + 1)*LongLines) + LongLines;

		indices[k + 3] = ((i + 1)*LongLines) + LongLines;
		indices[k + 4] = (i*LongLines) + 1;
		indices[k + 5] = ((i + 1)*LongLines) + 1;

		k += 6;
	}

	for(int l = 0; l < LongLines - 1; ++l) {
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;

	//return new MeshObj(_pDevice, pDeviceContext, _pPixelShader, &vertices[0], (UINT)vertices.size(), &indices[0], (UINT)indices.size());
}

void DebugRenderer::AddCapsule(DirectX::XMFLOAT3& start, DirectX::XMFLOAT3& end, DirectX::XMFLOAT4X4& orientation, float radius, DirectX::XMFLOAT3& color) {

}


void DebugRenderer::DrawAxes(DirectX::XMFLOAT4X4 toDraw, float length) {
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

void DebugRenderer::drawTo(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport) {
	context->OMSetRenderTargets(1, &rtv, dsv);
	context->RSSetViewports(1, &viewport);
	context->VSSetShader(vShader, NULL, NULL);
	context->PSSetShader(pShader, NULL, NULL);
	context->IASetInputLayout(iLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map((ID3D11Resource *) vertBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
	memcpy(mapped.pData, cpu_side_buffer, sizeof(VertexPositionColor) * v_count);
	context->Unmap((ID3D11Resource *) vertBuff, 0);
	//context->UpdateSubresource(vertBuff, 0, NULL, &cpu_side_buffer, 0, 0);
	context->UpdateSubresource(modelBuffer, 0, NULL, &DirectX::XMMatrixIdentity(), 0, 0);

	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	context->Draw(v_count, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Map((ID3D11Resource *) vertBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
	memcpy(mapped.pData, cpu_side_triangles, sizeof(VertexPositionColor) * tri_count);
	context->Unmap((ID3D11Resource *) vertBuff, 0);
	context->RSSetState(wireframeState);
	context->Draw(tri_count, 0);
	context->RSSetState(defaultState);
}

void DebugRenderer::flushTo(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport) {
	drawTo(rtv, dsv, viewport);
	v_count = 0;
	tri_count = 0;
}
