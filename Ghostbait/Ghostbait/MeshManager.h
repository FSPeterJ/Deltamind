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
	ID3D11Device* device;
	void generateCube();
public:
	MeshManager();
	~MeshManager();

	void Initialize(ID3D11Device* deviceIn);
	void Destroy();
	void AddElement(const char * _meshFilePath, const unsigned int _id) override;
	meshPositionColor* GetElement(const unsigned int _id) override;
};

