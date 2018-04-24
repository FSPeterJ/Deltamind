#include "ParticleTextureManager.h"
#include "WICTextureLoader.h"
#include "Material.h"
#include <memory>
#include "Renderer.h"
#include "RenderUtil.h"


ParticleTextureManager::ParticleTextureManager(ID3D11Device* _device, ID3D11DeviceContext* _context) {
	device = _device;
	context = _context;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.MipLevels = MIP_LEVELS;
	desc.ArraySize = MAX_TEXTURES;
	desc.Height = 512;
	desc.Width = 512;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, &texArray);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvdesc.Texture2DArray.MostDetailedMip = desc.MipLevels - 1;
	srvdesc.Texture2DArray.MipLevels = desc.MipLevels;
	srvdesc.Texture2DArray.ArraySize = MAX_TEXTURES;
	srvdesc.Texture2DArray.FirstArraySlice = 0;
	device->CreateShaderResourceView((ID3D11Resource*)texArray, &srvdesc, &srv);

	//desc.ArraySize = 1;
	//desc.Usage = D3D11_USAGE_STAGING;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	//desc.BindFlags = 0;
	//device->CreateTexture2D(&desc, nullptr, &stagingTex);
}


ParticleTextureManager::~ParticleTextureManager() {

	//stagingTex->Release();
	texArray->Release();
	srv->Release();
}



void ParticleTextureManager::RegenerateTexture2DArray() {
	SAFE_RELEASE(texArray);
	SAFE_RELEASE(srv);
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.MipLevels = maxMipLevel;
	desc.ArraySize = (unsigned)mats.size();
	desc.Width = maxWidth;
	desc.Height = maxHeight;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, &texArray);
	//D3D11 ERROR: ID3D11Device::CreateShaderResourceView: 
	//The Dimensions of the View are invalid due to at least one of the following conditions. 
	//		MostDetailedMip (value = 8) must be between 0 and MipLevels-1 of the Texture Resource, 8, inclusively. 
	//		With the current MostDetailedMip, MipLevels (value = 9) must be between 1 and 1, inclusively, or -1 to default to all mips from MostDetailedMip, in order that the View fit on the Texture. 
	//		FirstArraySlice (value = 0) must be between 0 and ArraySize-1 of the Texture Resource, 0, inclusively. 
	//		With the current FirstArraySlice, ArraySize (value = 1) must be between 1 and 1, inclusively, or -1 to default to all slices from FirstArraySlice, in order that the View fit on the Texture. 
	//		[ STATE_CREATION ERROR #128: CREATESHADERRESOURCEVIEW_INVALIDDIMENSIONS]

	

	for(UINT i = 0; i < mats.size(); ++i) {
		context->CopySubresourceRegion(texArray, i, 0, 0, 0, mats[i]->diffuse.texture, 0, nullptr);
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvdesc.Texture2DArray.MostDetailedMip = desc.MipLevels - 1;
	srvdesc.Texture2DArray.MipLevels = -1;
	srvdesc.Texture2DArray.ArraySize = (unsigned)mats.size();;
	srvdesc.Texture2DArray.FirstArraySlice = 0;
	device->CreateShaderResourceView((ID3D11Resource*)texArray, &srvdesc, &srv);


	rData.height = maxHeight;
	rData.width = maxWidth;

	D3D11_SUBRESOURCE_DATA resData = { 0 };


	//countData[0] = 1u;
	CD3D11_BUFFER_DESC UVDimensionsConstant(sizeof(dimens), D3D11_BIND_CONSTANT_BUFFER);
	resData.pSysMem = &rData;
	device->CreateBuffer(&UVDimensionsConstant, &resData, &UVDimensionsConstantBuffer);


	context->GSSetConstantBuffers(1,1, &UVDimensionsConstantBuffer);

}

unsigned ParticleTextureManager::AddTexture(Material* mat) {

	mats.push_back(mat);

	//texture toAdd;	
	//toAdd.tex = mat->diffuse.texture;
	++activatedCount;

	ID3D11Texture2D *pTextureInterface = 0;
	ID3D11Resource *res;
	mat->diffuse.texView->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);
	if(desc.Width > maxWidth) {

		maxWidth = desc.Width;
	}
	if(desc.Height > maxHeight) {
		maxHeight = desc.Height;
	}
	if(desc.MipLevels > maxMipLevel) {
		maxMipLevel = desc.MipLevels;
	}
	RegenerateTexture2DArray();

	return (unsigned)mats.size() - 1u;

}

void ParticleTextureManager::BindToShader() {
	//ID3D11ShaderResourceView* prevSRV = nullptr;
	//ID3D11ShaderResourceView* newSRV = nullptr;
	//context->PSGetShaderResources(0, 6, &prevSRV);
	//context->PSGetShaderResources(0, 6, &newSRV);
	//newSRV[0] = *srv;
	context->PSSetShaderResources(6, 1, &srv);
	//context->PSSetShaderResources(6, 1, &mats[0]->diffuse.texView);
}


