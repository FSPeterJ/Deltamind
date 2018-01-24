#pragma once
#include "IComponentManager.h"
#include <d3d11.h>
#include <unordered_map>
#include <Pool.h>

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
	Mesh* GetComponent(const char* _meshFilePath) override;
	Mesh* GetElement(const unsigned int _id) override;
};
