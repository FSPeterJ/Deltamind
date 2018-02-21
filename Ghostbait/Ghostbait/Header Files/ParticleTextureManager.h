#pragma once
#include <d3d11.h>
#include <vector>
#define MAX_TEXTURES 32

class ParticleTextureManager
{
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	struct texture
	{
		ID3D11Texture2D* tex;
		ID3D11ShaderResourceView* srv;
	};
	std::vector<texture> textures;
	std::vector<ID3D11ShaderResourceView*> srvPointers;
	ID3D11Texture2D* texArray;
	ID3D11Texture2D* stagingTex;
	ID3D11ShaderResourceView* srv;
public:
	ParticleTextureManager(ID3D11Device* _device, ID3D11DeviceContext* _context);
	~ParticleTextureManager();
	int AddTexture(const char* filePath);
	void bindToShader();
};

