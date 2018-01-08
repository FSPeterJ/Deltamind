#include "MeshManager.h"



MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
}

void MeshManager::destroy()
{
	for (size_t i = 0; i < smallMeshes.size(); ++i)
	{
		smallMeshes[i].vertexBuffer->Release();
		smallMeshes[i].indexBuffer->Release();
	}
}
