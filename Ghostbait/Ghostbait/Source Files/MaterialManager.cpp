#include "MaterialManager.h"
#include <fstream>
#include "WICTextureLoader.h"
#include <string>

MaterialManager::MaterialManager() {}

MaterialManager::~MaterialManager() {}

void MaterialManager::Initialize(ID3D11Device * deviceIn, ID3D11DeviceContext* contextIn) {
	device = deviceIn;
	context = contextIn;
	Material* toEdit = ConstructMaterial("Assets/ViveController_mat.bin");
	toEdit->matID = UINT_MAX;
}

void MaterialManager::Destroy() {
	for(size_t i = 0; i < trackedMaterials.size(); ++i) {
		trackedMaterials[i].release();
	}
}

Material * MaterialManager::ConstructMaterial(const char * _materialFilePath) {
	Material ret = Material();
	std::ifstream reader;
	reader.open(_materialFilePath, std::ios_base::binary);
	if(reader.is_open()) {
		unsigned int length = 0;
		reader.read((char*) &length, sizeof(length));
		if(length > 0) {
			char buffer[256];
			reader.read(&buffer[0], length);
			std::string thePath(buffer);
			thePath = "Assets\\" + thePath;
			std::wstring path(thePath.begin(), thePath.end());
			HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**) &ret.diffuse.texture, &ret.diffuse.texView);
		} else {
			ret.diffuse.texture = nullptr;
			ret.diffuse.texView = nullptr;
		}
		reader.read((char*) &length, sizeof(length));
		if(length > 0) {
			char buffer[256];
			reader.read(&buffer[0], length);
			std::string thePath(buffer);
			thePath = "Assets\\" + thePath;
			std::wstring path(thePath.begin(), thePath.end());
			HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**) &ret.specular.texture, &ret.specular.texView);
		} else {
			ret.specular.texture = nullptr;
			ret.specular.texView = nullptr;
		}
		reader.read((char*) &length, sizeof(length));
		if(length > 0) {
			char buffer[256];
			reader.read(&buffer[0], length);
			std::string thePath(buffer);
			thePath = "Assets\\" + thePath;
			std::wstring path(thePath.begin(), thePath.end());
			HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**) &ret.emissive.texture, &ret.emissive.texView);
		} else {
			ret.emissive.texture = nullptr;
			ret.emissive.texView = nullptr;
		}
		reader.read((char*) &ret.diffuse.factor, sizeof(ret.diffuse.factor));
		reader.read((char*) &ret.specular.factor, sizeof(ret.specular.factor));
		reader.read((char*) &ret.emissive.factor, sizeof(ret.emissive.factor));
	}
	ret.matID = (unsigned int) trackedMaterials.size();
	trackedMaterials.push_back(ret);
	return &trackedMaterials[trackedMaterials.size() - 1];
}

unsigned int MaterialManager::AddElement(const char * _materialFilePath) {
	return ConstructMaterial(_materialFilePath)->matID;
}

Material * MaterialManager::GetElement(const unsigned int _id) {
	for(size_t i = 0; i < trackedMaterials.size(); ++i) {
		if(trackedMaterials[i].matID == _id)
			return &trackedMaterials[i];
	}
	return nullptr;
}
