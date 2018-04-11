#include "ParticleManager.h"
#include "RenderUtil.h"

void ParticleManager::InitShaders() {

	char* byteCode = nullptr;
	size_t byteCodeSize;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "Particle_GeometryShader.cso");
	device->CreateGeometryShader(byteCode, byteCodeSize, NULL, &GeometryShader);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "Particle_PixelShader.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &PixelShader);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "Particle_VertexShader.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &VertexShader);
	delete[] byteCode;
	byteCode = nullptr;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleEmit_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleEmitShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleUpdate_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleUpdateShader);
	delete[] byteCode;
}

ParticleManager::ParticleManager(ID3D11Device * _device, ID3D11DeviceContext * _context, ID3D11Buffer* _perFrame) {//no mom I don't do member initializer lists on long function signatures

	device = _device;
	context = _context;
	perFrame = _perFrame;

	InitShaders();

	texMan = new ParticleTextureManager(device, context);

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(DirectX::XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER);
	//device->CreateBuffer(&constantBufferDesc, nullptr, &viewBuff);
	//device->CreateBuffer(&constantBufferDesc, nullptr, &projBuff);
	//TEMP CODE YOU DUMMY GET THIS OUT OF HERE

	//texMan->AddTexture("HELLO.png");
	//texMan->AddTexture("StartCube.fbm/startCube.png");

	//Particle Buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.ByteWidth = sizeof(GPUParticle) * MAX_PARTICLES;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(GPUParticle);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = MAX_PARTICLES;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	D3D11_SUBRESOURCE_DATA resData = { 0 };

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = MAX_PARTICLES;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

	//TESTING CODE
	GPUParticle* testParticle = new GPUParticle[MAX_PARTICLES];
	testParticle->position = DirectX::XMFLOAT3(1, 2.0f, 1.5f);
	testParticle->endSize = 0.5f;
	testParticle->velocity = DirectX::XMFLOAT3(0, 0, 0.5f);
	testParticle->startSize = 1.5f;
	testParticle->age = 9.5f;
	testParticle->lifespan = 9.5f;
	testParticle->StartColor = DirectX::XMFLOAT4(0, 1, 0, 0.9f);;
	testParticle->EndColor = DirectX::XMFLOAT4(1, 0, 0, 0);;
	testParticle->properties = 512u << 20| 512u << 8 | 0u;
	resData.pSysMem = testParticle;
	//END TEST

	//device->CreateBuffer(&bufferDesc, nullptr, &ParticleBuffer);

	assert(&bufferDesc != nullptr);
	assert(&resData != nullptr);
	auto temp = device->CreateBuffer(&bufferDesc, &resData, &ParticleBuffer);
	assert(ParticleBuffer);

	delete[] testParticle;
	device->CreateShaderResourceView(ParticleBuffer, &srvDesc, &ParticleSRV);
	device->CreateUnorderedAccessView(ParticleBuffer, &uavDesc, &ParticleUAV);

	// Index Buffers
	bufferDesc.ByteWidth = sizeof(UINT) * MAX_PARTICLES;
	bufferDesc.StructureByteStride = sizeof(UINT);
	//Active

	//TESTING CODE
	ZeroMemory(&resData, sizeof(resData));
	UINT* indexData = new UINT[MAX_PARTICLES]();
	resData.pSysMem = indexData;

	//END TEST

	//device->CreateBuffer(&bufferDesc, nullptr, &ActiveParticleIndexBuffer);
	device->CreateBuffer(&bufferDesc, &resData, &ActiveParticleIndexBuffer);
	device->CreateShaderResourceView(ActiveParticleIndexBuffer, &srvDesc, &ActiveParticleIndexSRV);
	device->CreateUnorderedAccessView(ActiveParticleIndexBuffer, &uavDesc, &ActiveParticleIndexUAV);

	for(unsigned i = 0; i < MAX_PARTICLES; ++i) {
		indexData[i] = i;
	}
	resData.pSysMem = indexData;

	//Inactive
	device->CreateBuffer(&bufferDesc, &resData, &InactiveParticleIndexBuffer);
	device->CreateShaderResourceView(InactiveParticleIndexBuffer, &srvDesc, &InactiveParticleIndexSRV);
	device->CreateUnorderedAccessView(InactiveParticleIndexBuffer, &uavDesc, &InactiveParticleIndexUAV);
	delete[] indexData;

	//Active / Inactive count Constant buffer
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(UINT) * 4;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&resData, sizeof(resData));
	UINT* countData = new UINT[4];
	countData[0] = 1u;

	resData.pSysMem = countData;
	device->CreateBuffer(&bufferDesc, &resData, &ActiveParticleConstantBuffer);
	countData[0] = MAX_PARTICLES;
	resData.pSysMem = countData;
	device->CreateBuffer(&bufferDesc, &resData, &InactiveParticleConstantBuffer);
	delete[] countData;

	//Emitter constant buffer
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = sizeof(EmitterConstant);
	//device->CreateBuffer(&bufferDesc, nullptr, &EmitterConstantBuffer);

	CD3D11_BUFFER_DESC NconstantBufferDesc(sizeof(EmitterConstant), D3D11_BIND_CONSTANT_BUFFER);
	resData.pSysMem = &emitterConstant;

	device->CreateBuffer(&NconstantBufferDesc, &resData, &EmitterConstantBuffer);

	ZeroMemory(&resData, sizeof(resData));
	UINT* argData = new UINT[5];
	argData[0] = 1u;
	argData[1] = 1u;
	argData[2] = 0;
	argData[3] = 0;
	argData[4] = 0;
	resData.pSysMem = argData;

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.ByteWidth = sizeof(UINT) * 5;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	device->CreateBuffer(&bufferDesc, &resData, &IndirectDrawArgsBuffer);

	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_R32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = 5;
	uavDesc.Buffer.Flags = 0;
	device->CreateUnorderedAccessView(IndirectDrawArgsBuffer, &uavDesc, &IndirectDrawArgsUAV);

	ID3D11UnorderedAccessView* uavs[] = { ActiveParticleIndexUAV, InactiveParticleIndexUAV };
	UINT counts[] = { 0, (MAX_PARTICLES) };
	//This fakes setting one particle as active
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, counts);
	uavs[0] = nullptr;
	uavs[1] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, counts);

	//ID3D11UnorderedAccessView* uavs[] = { ParticleUAV, ActiveParticleIndexUAV, InactiveParticleIndexUAV };

	//UINT counts[] = { (UINT)1, (UINT)1, (UINT)(MAX_PARTICLES) };
	////This fakes setting one particle as active
	//context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, counts);
	//uavs[0] = nullptr;
	//uavs[1] = nullptr;
	//uavs[2] = nullptr;
	//context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, counts);


	ID3D11Buffer* buffers[] = { perFrame, ActiveParticleConstantBuffer, InactiveParticleConstantBuffer, EmitterConstantBuffer };
	context->CSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);

}


void ParticleManager::RenderParticles() {

	texMan->BindToShader();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->VSSetShader(VertexShader, NULL, NULL);
	context->GSSetShader(GeometryShader, NULL, NULL);
	context->PSSetShader(PixelShader, NULL, NULL);

	ID3D11ShaderResourceView* SRV[] = { ParticleSRV, ActiveParticleIndexSRV };
	context->VSSetShaderResources(10, 2, SRV);

	context->CopyStructureCount(IndirectDrawArgsBuffer, 0, ActiveParticleIndexUAV);
	context->DrawInstancedIndirect(IndirectDrawArgsBuffer, 0);
	//context->Draw(20, 0);
	context->GSSetShader(nullptr, NULL, NULL);
	//SRV[0] = nullptr;
	//SRV[1] = nullptr;
	//context->VSSetShaderResources(10, 2, SRV);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	SRV[0] = nullptr;
	SRV[1] = nullptr;
	context->VSSetShaderResources(10, 2, SRV);

	Update();
}

ComponentBase* ParticleManager::GetReferenceComponent(const char* _FilePath, const char* _data) {
	ComponentBase* newEmitter = referenceEemitterPool.ActivateMemory();
	//EmitterComponent* data = (EmitterComponent*)reference;
	return newEmitter;
}

ComponentBase* ParticleManager::CloneComponent(ComponentBase* reference) {
	ComponentBase* newEmitter = emitterPool.ActivateMemory();
	//EmitterComponent* data = (EmitterComponent*)reference;
	memcpy(newEmitter, reference, sizeof(Emitter));
	return newEmitter;
}

void ParticleManager::ResetComponent(ComponentBase* reset) {
	//((EmitterComponent*)reset)->rigidBody.Reset();
	emitterPool.DeactivateMemory(reset);
}

void ParticleManager::ActivateComponent(ComponentBase* component) {

}

void ParticleManager::DeactivateComponent(ComponentBase* component) {
	emitterPool.DeactivateMemory(component);
}

unsigned ParticleManager::AddMaterial(Material* mat) {
	return texMan->AddTexture(mat);

}

ParticleManager::~ParticleManager() {
	delete texMan;

	//Releasing Particle Resources
	//==========================================

	ParticleBuffer->Release();
	ParticleSRV->Release();
	ParticleUAV->Release();
	//Inactive
	InactiveParticleConstantBuffer->Release();
	InactiveParticleIndexBuffer->Release();
	InactiveParticleIndexSRV->Release();
	InactiveParticleIndexUAV->Release();
	//Active
	ActiveParticleConstantBuffer->Release();
	ActiveParticleIndexBuffer->Release();
	ActiveParticleIndexSRV->Release();
	ActiveParticleIndexUAV->Release();
	//Emitter
	EmitterConstantBuffer->Release();

	//Drawing Arguments
	IndirectDrawArgsUAV->Release();
	IndirectDrawArgsBuffer->Release();
	//Shaders
	ParticleUpdateShader->Release();
	ParticleEmitShader->Release();
	VertexShader->Release();
	GeometryShader->Release();
	PixelShader->Release();

	//==========================================
}

void ParticleManager::Update() {

	//Map emitters and fire off Emit shader
	ID3D11UnorderedAccessView* uavs[] = { ParticleUAV, ActiveParticleIndexUAV, InactiveParticleIndexUAV, IndirectDrawArgsUAV };

	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	context->CSSetShader(ParticleEmitShader, nullptr, 0);

	context->CopyStructureCount(ActiveParticleConstantBuffer, 0, ActiveParticleIndexUAV);
	context->CopyStructureCount(InactiveParticleConstantBuffer, 0, InactiveParticleIndexUAV);
	int numThreadGroups = 1;

	// I do not like that the pool gives access to the vector because that makes it modifiable.  
	// It should give access to the vector array and you should then ask for the active count.
	// A discussion with Kody on an unrelated system has made this apparent to me.
	Emitter* activeEmitters = (*emitterPool.GetActiveList())[0];
	unsigned activeCount = (unsigned)emitterPool.GetActiveCount();
	for(unsigned i = 0; i < activeCount; ++i) {
		Emitter* activeEmit = &activeEmitters[activeCount];
		emitterConstant.EndColor = activeEmit->EndColor;
		emitterConstant.Position = activeEmit->Position;
		emitterConstant.Velocity = activeEmit->Velocity;
		emitterConstant.TextureIndex = activeEmit->TextureIndex;
		context->UpdateSubresource(EmitterConstantBuffer, NULL, NULL, &emitterConstant, NULL, NULL);
		context->Dispatch(numThreadGroups, 1, 1);
	}

	//Process Update of shaders

	context->CSSetShader(ParticleUpdateShader, nullptr, 0);
	context->CopyStructureCount(ActiveParticleConstantBuffer, 0, ActiveParticleIndexUAV);
	context->CopyStructureCount(InactiveParticleConstantBuffer, 0, InactiveParticleIndexUAV);

	//This fakes setting one particle as active


	//ID3D11ShaderResourceView* srvs[] = { ParticleSRV };
	//context->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
	numThreadGroups = 1;
	//int numThreadGroups = MAX_PARTICLES / 1024;
	context->Dispatch(numThreadGroups, 1, 1);

	uavs[0] = nullptr;
	uavs[1] = nullptr;
	uavs[2] = nullptr;
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
	//srvs[0] = nullptr;
	//context->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

	//context->CopyStructureCount(IndirectDrawArgsBuffer, sizeof(unsigned)*2, ActiveParticleIndexUAV);

}