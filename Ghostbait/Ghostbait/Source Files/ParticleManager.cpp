#include "ParticleManager.h"
#include "RenderUtil.h"

void ParticleManager::InitShaders() {

	char* byteCode = nullptr;
	size_t byteCodeSize;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "Particle_GeometryShader.cso");
	device->CreateGeometryShader(byteCode, byteCodeSize, NULL, &GeometryShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "Particle_PixelShader.cso");
	device->CreatePixelShader(byteCode, byteCodeSize, NULL, &PixelShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "Particle_VertexShader.cso");
	device->CreateVertexShader(byteCode, byteCodeSize, NULL, &VertexShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleEmit_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleEmitShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleUpdate_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleUpdateShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleSortInitial_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleSortInitialShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleSortStep_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleSortStepShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleSortFinal_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleSortFinalShader);
	delete[] byteCode;

	LoadShaderFromCSO(&byteCode, byteCodeSize, "ParticleSortInitArgs_ComputeShader.cso");
	device->CreateComputeShader(byteCode, byteCodeSize, NULL, &ParticleSortInitArgsShader);
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

	device->CreateBuffer(&bufferDesc, nullptr, &ActiveParticleIndexBuffer);
	//device->CreateBuffer(&bufferDesc, &resData, &ActiveParticleIndexBuffer);
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

	bufferDesc.ByteWidth = sizeof(SortIndex) * MAX_PARTICLES;
	bufferDesc.StructureByteStride = sizeof(SortIndex);

	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	//Sort Buffer
	device->CreateBuffer(&bufferDesc, &resData, &SortParticleIndexBuffer);
	device->CreateShaderResourceView(SortParticleIndexBuffer, &srvDesc, &SortParticleIndexSRV);
	device->CreateUnorderedAccessView(SortParticleIndexBuffer, &uavDesc, &SortParticleIndexUAV);

	//Active / Inactive count Constant buffer
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(UINT) * 4;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&resData, sizeof(resData));
	UINT* countData = new UINT[4];
	//countData[0] = 1u;

	resData.pSysMem = countData;
	device->CreateBuffer(&bufferDesc, &resData, &ActiveParticleConstantBuffer);
	//countData[0] = MAX_PARTICLES;
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

	CD3D11_BUFFER_DESC SortParametersconstantBufferDesc(sizeof(SortParameters), D3D11_BIND_CONSTANT_BUFFER);

	resData.pSysMem = &sortParams;
	device->CreateBuffer(&NconstantBufferDesc, &resData, &SortParametersConstantBuffer);

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


	bufferDesc.ByteWidth = sizeof(UINT) * 4;
	device->CreateBuffer(&bufferDesc, nullptr, &IndirectSortArgsBuffer);

	uavDesc.Buffer.NumElements = 4;
	device->CreateUnorderedAccessView(IndirectSortArgsBuffer, &uavDesc, &IndirectSortArgsBufferUAV);

	ID3D11UnorderedAccessView* uavs[] = {  InactiveParticleIndexUAV, SortParticleIndexUAV, IndirectSortArgsBufferUAV };
	UINT counts[] = { (MAX_PARTICLES), 0, 0 };
	//This initializes the count in a worse way.  Please look into setting it on creation
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, counts);
	ZeroMemory(uavs, sizeof(uavs)); // Disconnect
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, counts);

	ID3D11Buffer* buffers[] = { perFrame, ActiveParticleConstantBuffer, InactiveParticleConstantBuffer, EmitterConstantBuffer, SortParametersConstantBuffer };
	context->CSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);

}


void ParticleManager::RenderParticles() {

	texMan->BindToShader();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->VSSetShader(VertexShader, NULL, NULL);
	context->GSSetShader(GeometryShader, NULL, NULL);
	context->PSSetShader(PixelShader, NULL, NULL);

	ID3D11ShaderResourceView* SRV[] = { ParticleSRV, SortParticleIndexSRV };
	context->VSSetShaderResources(10, 2, SRV);

	context->CopyStructureCount(IndirectDrawArgsBuffer, 0, SortParticleIndexUAV);
	context->DrawInstancedIndirect(IndirectDrawArgsBuffer, 0);
	//context->Draw(20, 0);
	context->GSSetShader(nullptr, NULL, NULL);

	//context->VSSetShaderResources(10, 2, SRV);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ZeroMemory(SRV, sizeof(SRV));
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
	ID3D11UnorderedAccessView* uavs[] = { ParticleUAV, ActiveParticleIndexUAV, InactiveParticleIndexUAV, IndirectDrawArgsUAV, SortParticleIndexUAV, IndirectSortArgsBufferUAV };
	int counters[] = { -1, -1, -1, -1, 0 }; //Reset the sorted particle Index Append/Consume

	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, (UINT*)&counters);


	context->CSSetShader(ParticleEmitShader, nullptr, 0);
	context->CopyStructureCount(ActiveParticleConstantBuffer, 0, ActiveParticleIndexUAV);
	context->CopyStructureCount(InactiveParticleConstantBuffer, 0, InactiveParticleIndexUAV);
	int numThreadGroups = 1;

	// I do not like that the pool gives access to the vector because that makes it modifiable.  
	// It should give access to the vector array and you should then ask for the active count.
	// A discussion with Kody on an unrelated system has made this apparent to me.
	Emitter* activeEmitters = (*emitterPool.GetActiveList())[0];
	const unsigned activeCount = (unsigned)emitterPool.GetActiveCount();

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
	context->CopyStructureCount(IndirectSortArgsBuffer, 0, ActiveParticleIndexUAV);

	//This fakes setting one particle as active


	//ID3D11ShaderResourceView* srvs[] = { ParticleSRV };
	//context->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
	numThreadGroups = 1;
	//int numThreadGroups = MAX_PARTICLES / 1024;
	context->Dispatch(numThreadGroups, 1, 1);

	ZeroMemory(uavs, sizeof(uavs));

	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	Sort();
}




//
//
//void ParticleManager::Sort() {
//
//	//ID3D11Buffer* cbs[] = { itemCountBuffer, m_pcbDispatchInfo };
//	//context->CSSetConstantBuffers(0, ARRAYSIZE(cbs), cbs);
//
//	// Write the indirect args to a UAV
//	//context->CSSetUnorderedAccessViews(0, 1, &IndirectSortArgsBufferUAV, nullptr);
//
//	context->CSSetUnorderedAccessViews(0, 1, &sortBufferUAV, nullptr);
//
//
//
//	bool completed = false;
//
//	context->CSSetShader(ParticleSortStepShader, nullptr, 0);
//
//	unsigned int numThreadGroups = 0;
//
//	UINT Data[100];
//	for(int subArraySize = 2; subArraySize < MAX_PARTICLES; subArraySize <<= 1) {
//		for(int compareDistance = (subArraySize >> 1); compareDistance > 0; compareDistance >>= 1) {
//
//			//double result = ((Data[GI & ~comparedistance] <= Data[GI | comparedistance]) == (bool)(g_iLevelMask & DTid.x)) ? Data[GI ^ comparedistance] : Data[GI];
//			//Data[GI] = result;
//
//			D3D11_MAPPED_SUBRESOURCE MappedResource;
//
//			context->Map(SortParametersConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
//			SortParameters* JobParameters = (SortParameters*)MappedResource.pData;
//			JobParameters->distance = compareDistance;
//			if(compareDistance == subArraySize) {
//				JobParameters->jump = (2*compareDistance-1);
//				JobParameters->direction = -1;
//			}
//			else {
//				JobParameters->jump = compareDistance;
//				JobParameters->direction = 1;
//			}
//			context->Unmap(SortParametersConstantBuffer, 0);
//			context->Dispatch(1024, 1, 1);
//		}
//	}
//
//
//}





const int MAX_NUM_TG = 1024;//128; // max 128 * 512 elements = 64k elements




void ParticleManager::Sort() const {


	context->CSSetShader(ParticleSortInitArgsShader, nullptr, 0);
	context->Dispatch(1, 1, 1);

	//This looks terrible
	bool bDone = sortInitial();

	int presorted = 512;
	while(!bDone) {
		bDone = sortIncremental(presorted);
		presorted *= 2;
	}


}



bool ParticleManager::sortInitial() const {
	bool bDone = true;

	// calculate how many threads we'll require:
	//   we'll sort 512 elements per CU (threadgroupsize 256)
	//     maybe need to optimize this or make it changeable during init
	//     TGS=256 is a good intermediate value


	const unsigned int numThreadGroups = ((MAX_PARTICLES-1)>>9)+1;

	assert(numThreadGroups <=MAX_NUM_TG);

	if(numThreadGroups>1) bDone = false;

	// sort all buffers of size 512 (and presort bigger ones)
	context->CSSetShader(ParticleSortInitialShader, nullptr, 0);
	context->DispatchIndirect(IndirectSortArgsBuffer, 0);

	return bDone;
}

bool ParticleManager::sortIncremental(unsigned presorted) const {
	bool bDone = true;
	context->CSSetShader(ParticleSortStepShader, nullptr, 0);

	// prepare thread group description data
	unsigned numThreadGroups = 0;

	if(MAX_PARTICLES > presorted) {
		if(MAX_PARTICLES>presorted*2)
			bDone = false;

		unsigned pow2 = presorted;
		while(pow2<MAX_PARTICLES)
			pow2 *= 2;
		numThreadGroups = pow2>>9;
	}

	const unsigned subArraySize = presorted;

	for(unsigned distance = subArraySize; distance>256; distance = distance>>1)
		//	for( int nMergeSubSize=nMergeSize>>1; nMergeSubSize>0; nMergeSubSize=nMergeSubSize>>1 ) 
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;

		context->Map(SortParametersConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		SortParameters* sc = (SortParameters*)MappedResource.pData;
		sc->distance = distance;
		if(distance == subArraySize) {
			sc->jump = (2*distance-1);
			sc->direction = -1;
		}
		else {
			sc->jump = distance;
			sc->direction = 1;
		}
		context->Unmap(SortParametersConstantBuffer, 0);

		context->Dispatch(numThreadGroups, 1, 1);
	}

	context->CSSetShader(ParticleSortFinalShader, nullptr, 0);
	context->Dispatch(numThreadGroups, 1, 1);

	return bDone;
}
