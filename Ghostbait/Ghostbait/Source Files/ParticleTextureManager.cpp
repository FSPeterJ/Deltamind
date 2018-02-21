#include "ParticleTextureManager.h"
#include "WICTextureLoader.h"


ParticleTextureManager::ParticleTextureManager(ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	device = _device;
	context = _context;
	D3D11_TEXTURE2D_DESC desc;
	desc.MipLevels = 1;
	desc.ArraySize = MAX_TEXTURES;
	desc.Height = 512;
	desc.Width = 512;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, &texArray);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.Format = desc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvdesc.Texture2DArray.MostDetailedMip = 0;
	srvdesc.Texture2DArray.MipLevels = desc.MipLevels;
	srvdesc.Texture2DArray.ArraySize = MAX_TEXTURES;
	srvdesc.Texture2DArray.FirstArraySlice = 0;
	device->CreateShaderResourceView((ID3D11Resource*)texArray, &srvdesc, &srv);

	desc.MipLevels = 1;
	desc.ArraySize = MAX_TEXTURES;
	desc.Height = 512;
	desc.Width = 512;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, &stagingTex);
}


ParticleTextureManager::~ParticleTextureManager()
{
	for (size_t i = 0; i < textures.size(); ++i)
	{
		textures[i].tex->Release();
		textures[i].srv->Release();
	}
}

int ParticleTextureManager::AddTexture(const char * filePath)
{
	texture toAdd;
	std::string thePath(filePath);
	thePath = "Assets\\" + thePath;
	std::wstring path(thePath.begin(), thePath.end());
	HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&toAdd.tex, &toAdd.srv);
	textures.push_back(toAdd);
	srvPointers.push_back(toAdd.srv);
	context->UpdateSubresource(stagingTex, (UINT)textures.size() - 1, nullptr, toAdd.tex, NULL, NULL);
	context->CopySubresourceRegion(texArray, (UINT)textures.size() - 1, 0, 0, 0, stagingTex, (UINT)textures.size() - 1, nullptr);
	return (int)srvPointers.size()-1;
}

void ParticleTextureManager::bindToShader()
{
	context->PSSetShaderResources(0, (UINT)textures.size(), &srv);
}
