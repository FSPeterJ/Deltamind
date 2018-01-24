#pragma once

#include <vector>
#include <d3d11.h>
#include "IComponentManager.h"

struct Material : ComponentBase
{
	struct matComponent
	{
		float factor;
		ID3D11Buffer * texture;
		ID3D11ShaderResourceView * texView;
	};
	struct factorBufferStructure
	{
		float diffuseFactor;
		float specularFactor;
		float emissiveFactor;
		float padding;
	};
	unsigned int matID;
	matComponent diffuse;
	matComponent specular;
	matComponent emissive;

	void release()
	{
		if (diffuse.texture)
		{
			diffuse.texture->Release();
			diffuse.texView->Release();
		}
		if (specular.texture)
		{
			specular.texture->Release();
			specular.texView->Release();
		}
		if (emissive.texture)
		{
			emissive.texture->Release();
			emissive.texView->Release();
		}
	}

	void bindToShader(ID3D11DeviceContext* context, ID3D11Buffer* factorBuffer)
	{
		context->PSSetShaderResources(0, 1, &diffuse.texView);
		context->PSSetShaderResources(1, 1, &specular.texView);
		context->PSSetShaderResources(2, 1, &emissive.texView);
		factorBufferStructure toShader;

		//!this is not how you use the ternary operator
		//diffuse.texture ? toShader.diffuseFactor = diffuse.factor : toShader.diffuseFactor = 0.0f;
		//specular.texture ? toShader.specularFactor = specular.factor : toShader.specularFactor = 0.0f;
		//emissive.texture ? toShader.emissiveFactor = emissive.factor : toShader.emissiveFactor = 0.0f;

		//!this is how you use the ternary operator
		toShader.diffuseFactor = diffuse.texture ? diffuse.factor : 0.0f;
		toShader.specularFactor = specular.texture ? specular.factor : 0.0f;
		toShader.emissiveFactor = emissive.texture ? emissive.factor : 0.0f;
		
		context->UpdateSubresource(factorBuffer, NULL, NULL, &toShader, NULL, NULL);
	}
};

class MaterialManager
{
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	std::vector<Material> trackedMaterials;
	Material* ConstructMaterial(const char* _materialFilePath);
public:
	MaterialManager();
	~MaterialManager();

	void Initialize(ID3D11Device* deviceIn, ID3D11DeviceContext* contextIn);
	void Destroy();

	unsigned int AddElement(const char* _materialFilePath);

	Material* GetElement(const unsigned int _id);
};

