#include "ParticleTextureManager.h"
#include "WICTextureLoader.h"


ParticleTextureManager::ParticleTextureManager(ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	device = _device;
	context = _context;
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
	return (int)srvPointers.size()-1;
}

void ParticleTextureManager::bindToShader()
{
	context->PSSetShaderResources(0, (UINT)srvPointers.size(), &srvPointers[0]);
}
