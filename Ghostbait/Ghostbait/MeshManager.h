#pragma once
#include <vector>
#include "VertexTypes.h"
#include <d3d11.h>

struct meshPositionColor
{
	unsigned int meshId;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	unsigned int indexCount;
};

class MeshManager
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
	meshPositionColor * findMesh(const unsigned int id);
	void registerMesh(const char * meshFilePath, const unsigned int id);
};

