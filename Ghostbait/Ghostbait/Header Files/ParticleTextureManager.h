#pragma once
#include <d3d11.h>
#include <vector>
#include "Material.h"


#define MAX_TEXTURES 32
#define MIP_LEVELS 1

class ParticleTextureManager
{
	unsigned activatedCount = 0;
	unsigned maxWidth = 10;
	unsigned maxHeight = 10;
	struct dimens {
		UINT width;
		UINT height;
		UINT padding[2];
	}rData;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;

	/*struct texture
	{
		ID3D11Texture2D* tex = nullptr;
		ID3D11ShaderResourceView* srv = nullptr;
	};*/
	//std::vector<texture> textures;
	std::vector<Material*> mats;
	std::vector<ID3D11ShaderResourceView*> srvPointers;
	ID3D11Texture2D* texArray = nullptr;
	ID3D11Texture2D* stagingTex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	UINT maxMipLevel = 0;
	ID3D11Buffer* UVDimensionsConstantBuffer;
public:
	ParticleTextureManager(ID3D11Device* _device, ID3D11DeviceContext* _context);
	~ParticleTextureManager();
	void RegenerateTexture2DArray();
	unsigned AddTexture(Material* mat);
	void BindToShader();
};

