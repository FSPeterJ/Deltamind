#pragma once
#include <vector>
#include "VertexTypes.h"
#include "ManagerInterface.h"
#include <d3d11.h>

struct Mesh : ComponentBase
{
	unsigned int meshId;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	unsigned int indexCount;
};

class MeshManager : virtual public ManagerInterface
{
private:
	std::vector<Mesh> trackedMeshes;
	ID3D11Device* device = nullptr;
	void generateCube();
	Mesh* ConstructMesh(const char* _meshFilePath);
public:
	MeshManager();
	~MeshManager();

	void Initialize(ID3D11Device* _deviceIn);
	void Destroy();
	int AddElement(const char* _meshFilePath) override;
	Mesh* GetElement(const unsigned int _id) override;
};

