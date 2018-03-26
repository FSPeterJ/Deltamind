#pragma once

#include "ComponentBase.h"


struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;

struct Material: ComponentBase {
	enum MaterialFlags
	{
		POINT = 1,
	};
	struct matComponent {
		float factor;
		ID3D11Buffer * texture;
		ID3D11ShaderResourceView * texView;
	};
	struct factorBufferStructure {
		float diffuseFactor;
		float specularFactor;
		float emissiveFactor;
		float unlit;
	};
	unsigned int matID;
	matComponent diffuse;
	matComponent specular;
	matComponent emissive;
	unsigned int flags = 0;

	void release();

	void bindToShader(ID3D11DeviceContext* context, ID3D11Buffer* factorBuffer, bool unlit = false);
};
