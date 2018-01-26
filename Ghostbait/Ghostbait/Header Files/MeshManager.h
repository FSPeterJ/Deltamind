#pragma once
#include <Pool.h>
#include <unordered_map>
#include "IComponentManager.h"	// For Interface
#include "ComponentBase.h"		// For Interface

struct ID3D11Device;
struct ID3D11Buffer;

// Highly doubt we will hit this
#define MAX_MESHES 512

#include "ComponentBase.h"
struct Mesh: ComponentBase {
	unsigned int meshId;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	unsigned int indexCount;
};

class MeshManager: public IComponentManager {
	unsigned idCounter = 0;
	Pool<Mesh> trackedMeshes = Pool<Mesh>(MAX_MESHES);
	std::unordered_map<std::string, Mesh*> meshNames;
	ID3D11Device* device = nullptr;
	void generateCube();
	Mesh* ConstructMesh(const char* _meshFilePath);
public:
	MeshManager();
	~MeshManager();
	void Initialize(ID3D11Device* _deviceIn);
	void Destroy();
	int AddElement(const char* _meshFilePath);
	Mesh* GetElement(const unsigned _id);
	Mesh* GetReferenceComponent(const char * _FilePath, const char * _data) override;
	Mesh* CloneComponent(ComponentBase* meshReference) override;
	void ResetComponent(ComponentBase * reset) override;

};
