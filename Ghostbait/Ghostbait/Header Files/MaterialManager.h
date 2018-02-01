#pragma once
#include <unordered_map>
#include <Pool.h>
#include "IComponentManager.h"
#include "Material.h"
#define MAX_MATS 1024

struct ID3D11Device;
struct ID3D11DeviceContext;

class MaterialManager: public IComponentManager {

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	std::unordered_map<std::string, Material*> materialNames; //Checking for duplicates
	Pool<Material> trackedMaterials = Pool<Material>(MAX_MATS);
	//std::vector<Material> trackedMaterials;
	Material* ConstructMaterial(const char* _materialFilePath);
public:
	MaterialManager();
	~MaterialManager();

	void Initialize(ID3D11Device* deviceIn, ID3D11DeviceContext* contextIn);
	void Destroy();

	unsigned int AddElement(const char* _materialFilePath);

	Material* GetElement(const unsigned int _id);
	Material* CloneComponent(ComponentBase* reference) override;
	Material* GetReferenceComponent(const char * _FilePath, const char * _data) override;
	void ResetComponent(ComponentBase * reset) override;

};
