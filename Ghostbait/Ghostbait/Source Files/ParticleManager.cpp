#include "ParticleManager.h"

ID3D11Device* ParticleManager::device;
ID3D11DeviceContext* ParticleManager::context;
ID3D11VertexShader* ParticleManager::VertexShader;
ID3D11GeometryShader * ParticleManager::GeometryShader;
ID3D11PixelShader* ParticleManager::PixelShader;
ID3D11InputLayout* ParticleManager::InputLayout;
ID3D11Buffer* ParticleManager::projBuff;
ID3D11Buffer* ParticleManager::viewBuff;

ParticleTextureManager* ParticleManager::texMan;

ID3D11Buffer* ParticleManager::vertBuff;
int ParticleManager::pCount;
ParticleManager::ParticleShaderInfo ParticleManager::cpu_side_particles[MAX_PARTICLES];

void ParticleManager::Initialize(ID3D11Device * _device, ID3D11DeviceContext * _context, ID3D11VertexShader * _vs, ID3D11GeometryShader * _gs, ID3D11PixelShader * _ps, ID3D11InputLayout * _il)
{//no mom I don't do member initializer lists on long function signatures
	device = _device;
	context = _context;
	VertexShader = _vs;
	GeometryShader = _gs;
	PixelShader = _ps;
	InputLayout = _il;
	pCount = 0;

	texMan = new ParticleTextureManager(device, context);
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cpu_side_particles;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	D3D11_BUFFER_DESC vertBufferDesc;
	vertBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertBufferDesc.MiscFlags = 0;
	vertBufferDesc.StructureByteStride = 0;
	vertBufferDesc.ByteWidth = sizeof(ParticleShaderInfo) * MAX_PARTICLES;
	HRESULT gotem = device->CreateBuffer(&vertBufferDesc, &vertexBufferData, &vertBuff);

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(DirectX::XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&constantBufferDesc, nullptr, &viewBuff);
	device->CreateBuffer(&constantBufferDesc, nullptr, &projBuff);
	//TEMP CODE YOU DUMMY GET THIS OUT OF HERE
	ParticleShaderInfo temp;
	temp.pos = { 3.0f, 2.0f, 0.0f };
	temp.size = 0.5f;
	temp.tex = 1;
	cpu_side_particles[0] = temp;
	pCount++;
	texMan->AddTexture("HELLO.png");
	texMan->AddTexture("NullTex.png");
}

void ParticleManager::RenderParticlesTo(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv, D3D11_VIEWPORT & viewport, DirectX::XMFLOAT4X4 & view, DirectX::XMFLOAT4X4 & proj)
{
	context->OMSetRenderTargets(1, &rtv, dsv);
	context->RSSetViewports(1, &viewport);
	context->VSSetShader(VertexShader, NULL, NULL);
	context->PSSetShader(PixelShader, NULL, NULL);
	context->GSSetShader(GeometryShader, NULL, NULL);
	context->IASetInputLayout(InputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map((ID3D11Resource *)vertBuff, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
	memcpy(mapped.pData, cpu_side_particles, sizeof(ParticleShaderInfo) * pCount);
	context->Unmap((ID3D11Resource *)vertBuff, 0);
	//context->UpdateSubresource(vertBuff, 0, NULL, &cpu_side_buffer, 0, 0);
	context->UpdateSubresource(viewBuff, 0, NULL, &view, 0, 0);
	context->UpdateSubresource(projBuff, 0, NULL, &proj, 0, 0);
	context->VSSetConstantBuffers(0, 1, &viewBuff);
	context->GSSetConstantBuffers(0, 1, &projBuff);
	texMan->bindToShader();
	UINT stride = sizeof(ParticleShaderInfo);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);
	context->Draw(pCount, 0);
	context->GSSetShader(nullptr, NULL, NULL);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleManager::Destroy()
{
	vertBuff->Release();
	viewBuff->Release();
	projBuff->Release();
	delete texMan;
}
