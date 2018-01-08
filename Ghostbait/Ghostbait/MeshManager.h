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
	void generateCube();
public:
	MeshManager();
	~MeshManager();

	void destroy();
	void registerMesh(const char * meshFilePath, const unsigned int id);
};

