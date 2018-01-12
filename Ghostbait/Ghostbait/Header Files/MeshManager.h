#pragma once
#include <vector>
#include "VertexTypes.h"
#include "ManagerInterface.h"
#include <d3d11.h>

struct meshPositionColor : ComponentBase
{
	unsigned int meshId;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	unsigned int indexCount;
};

class MeshManager : virtual public ManagerInterface
{
private:
	std::vector<meshPositionColor> smallMeshes;
	std::vector<meshPositionColor> meshes;
	ID3D11Device* device = nullptr;
	void generateCube();
public:
	MeshManager();
	~MeshManager();

	void Initialize(ID3D11Device* _deviceIn);
	void Destroy();
	meshPositionColor* ConstructMesh(const char* _meshFilePath);
	int AddElement(const char* _meshFilePath, const unsigned int _id) override;
	meshPositionColor* GetElement(const unsigned int _id) override;
};

