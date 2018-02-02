#include "Material.h"

#include <d3d11.h>
void Material::bindToShader(ID3D11DeviceContext* context, ID3D11Buffer* factorBuffer) {
	context->PSSetShaderResources(0, 1, &diffuse.texView);
	context->PSSetShaderResources(1, 1, &specular.texView);
	context->PSSetShaderResources(2, 1, &emissive.texView);
	factorBufferStructure toShader;

	//!this is not how you use the ternary operator
	//diffuse.texture ? toShader.diffuseFactor = diffuse.factor : toShader.diffuseFactor = 0.0f;
	//specular.texture ? toShader.specularFactor = specular.factor : toShader.specularFactor = 0.0f;
	//emissive.texture ? toShader.emissiveFactor = emissive.factor : toShader.emissiveFactor = 0.0f;

	//!this is how you use the ternary operator
	//appreciate the hindsight but it worked so I'm not sure why you're in my house telling me this
	toShader.diffuseFactor = diffuse.texture ? diffuse.factor : 0.0f;
	toShader.specularFactor = specular.texture ? specular.factor : 0.0f;
	toShader.emissiveFactor = emissive.texture ? emissive.factor : 0.0f;

	context->UpdateSubresource(factorBuffer, NULL, NULL, &toShader, NULL, NULL);
}

void Material::release() {
	if(diffuse.texture) {
		diffuse.texture->Release();
		diffuse.texView->Release();
	}
	if(specular.texture) {
		specular.texture->Release();
		specular.texView->Release();
	}
	if(emissive.texture) {
		emissive.texture->Release();
		emissive.texView->Release();
	}
}
