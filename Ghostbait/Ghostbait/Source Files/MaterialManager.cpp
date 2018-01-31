#include "MaterialManager.h"
#include <fstream>
#include "WICTextureLoader.h"
#include "TypeMapping.h"

MaterialManager::MaterialManager() {}

MaterialManager::~MaterialManager() {}

void MaterialManager::Initialize(ID3D11Device * deviceIn, ID3D11DeviceContext* contextIn) {
	device = deviceIn;
	context = contextIn;
	TypeMap::RegisterComponentAlias<Material>("mat");
	TypeMap::RegisterComponentAlias<Material>("Mat");
	TypeMap::RegisterComponentAlias<Material>("MAT");

}

void MaterialManager::Destroy() {
	for(size_t i = 0; i < trackedMaterials.GetActiveCount(); ++i) {
		trackedMaterials[i].release();
	}
}

Material* MaterialManager::ConstructMaterial(const char * _materialFilePath) {
	Material* ret = trackedMaterials.ActivateMemory();
	std::ifstream reader;
	reader.open(_materialFilePath, std::ios_base::binary);
	if(reader.is_open()) {
		unsigned int length = 0;
		reader.read((char*)&length, sizeof(length));
		if(length > 0) {
			char buffer[256];
			reader.read(&buffer[0], length);
			std::string thePath(buffer);
			thePath = "Assets\\" + thePath;
			std::wstring path(thePath.begin(), thePath.end());
			HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&ret->diffuse.texture, &ret->diffuse.texView);
		}
		else {
			ret->diffuse.texture = nullptr;
			ret->diffuse.texView = nullptr;
		}
		reader.read((char*)&length, sizeof(length));
		if(length > 0) {
			char buffer[256];
			reader.read(&buffer[0], length);
			std::string thePath(buffer);
			thePath = "Assets\\" + thePath;
			std::wstring path(thePath.begin(), thePath.end());
			HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&ret->specular.texture, &ret->specular.texView);
		}
		else {
			ret->specular.texture = nullptr;
			ret->specular.texView = nullptr;
		}
		reader.read((char*)&length, sizeof(length));
		if(length > 0) {
			char buffer[256];
			reader.read(&buffer[0], length);
			std::string thePath(buffer);
			thePath = "Assets\\" + thePath;
			std::wstring path(thePath.begin(), thePath.end());
			HRESULT didItBlend = DirectX::CreateWICTextureFromFile(device, context, path.c_str(), (ID3D11Resource**)&ret->emissive.texture, &ret->emissive.texView);
		}
		else {
			ret->emissive.texture = nullptr;
			ret->emissive.texView = nullptr;
		}
		reader.read((char*)&ret->diffuse.factor, sizeof(ret->diffuse.factor));
		reader.read((char*)&ret->specular.factor, sizeof(ret->specular.factor));
		reader.read((char*)&ret->emissive.factor, sizeof(ret->emissive.factor));
	}
	ret->matID = (unsigned int)trackedMaterials.GetActiveCount();
	materialNames[std::string(_materialFilePath)] = ret;
	return ret;
}

unsigned int MaterialManager::AddElement(const char * _materialFilePath) {
	return ConstructMaterial(_materialFilePath)->matID;
}

Material * MaterialManager::GetElement(const unsigned int _id) {

	// TODO: Better ID tracking ?
	for(Material* m : *trackedMaterials.GetActiveList()) {
		if(m->matID == _id)
			return m;
	}
	return nullptr;
}

Material* MaterialManager::GetReferenceComponent(const char* _FilePath, const char* _data) {
	Material* material = materialNames[std::string(_FilePath)];
	if(!material) {
		material = ConstructMaterial(_FilePath);
	}
	return material;
}
void MaterialManager::ResetComponent(ComponentBase * reset) {
	trackedMaterials.DeactivateMemory(reset);
}
Material* MaterialManager::CloneComponent(ComponentBase* reference){

	
	return nullptr;
}

